#include <map>
#include <cassert>
#include "parser.hpp"

// global constraints
std::map<std::string, Constraints> arg_constraints;

bool is_single_arg(const std::string& str)
{
	return str.length() >= 3 && str[0] == '-' && str[1] == '-' && str[2] != '-';
}

bool is_multi_arg(const std::string& str)
{
	return str.length() >= 2 && str[0] == '-' && str[1] != '-';
}

bool is_arg(const std::string& str)
{
	return is_single_arg(str) || is_multi_arg(str);
}

bool is_value(const std::string& str)
{
	return str.length() >= 1 && str[0] != '-';
}

ArgList get_single_arg(const std::string& args_str)
{
	assert(is_single_arg(args_str));

	return {Arg{args_str.substr(2), true}};
}

ArgList get_multi_args(const std::string& args_str)
{
	assert(is_multi_arg(args_str));

	ArgList args;
	auto it = std::back_inserter(args);

	for (const auto& args_char : args_str.substr(1))
		it = {std::string(1, args_char), true};

	return args;
}

std::tuple<int, ArgList> get_next_args(int count, char* argv[])
{
	int step = 0;
	ArgList args;

	for (int i = 0; i < count; i++) {
		if (is_single_arg(argv[i]))
		{
			auto short_args = get_single_arg(argv[i]);
			args.insert(std::end(args), std::begin(short_args), std::end(short_args));
			step++;
		}
		else if (is_multi_arg(argv[i]))
		{
			auto short_args = get_multi_args(argv[i]);
			args.insert(std::end(args), std::begin(short_args), std::end(short_args));
			step++;
		}
		else
		{
			break;
		}
	}

	return {step, args};
}

std::optional<float> try_string_to_float(const std::string& str)
{
	try { return std::stof(str); } catch(std::exception ex) { return {}; }
}

SingleValue string_to_single_value(const std::string& str)
{
	if (auto value = try_string_to_float(str); value)
		return value.value();
	return str;
}

std::tuple<int, SingleValueList> get_next_single_values(int count, char* argv[])
{
	int step = 0;
	SingleValueList values;

	for (int i = 0; i < count; i++) {
		if (is_value(argv[i])) {
			values.push_back(string_to_single_value(argv[i]));
			step++;
		}
		else
		{
			break;
		}
	}

	return {step, values};
}

template <typename ...Ts>
struct overloaded : Ts... { using Ts::operator()...; };
template <typename ...Ts>
overloaded(Ts...) -> overloaded<Ts...>;

void add_value_to_arg(Arg_base<MonoValue>& arg, SingleValue value)
{
	std::visit(overloaded{
		[&] (std::monostate) { // arg.value is empty -> single value
			std::visit([&] (auto&& value) {
				arg.value = value;
			}, value);
		},
		[&] (auto&& v) { // arg.value is a single value -> list of single values
			arg.value =  SingleValueList{v, value};
		},
		[&] (SingleValueList& values) { // arg.value is a list of single values -> list of single values
			values.push_back(value);
		},
	}, arg.value);
}

bool check_min_arg_values(Arg& arg, int count)
{
	if (auto it = arg_constraints.find(arg.name); it != std::end(arg_constraints))
		if (auto opt = it->second.min_values; opt)
			return count < opt.value();
	return false;
}

std::tuple<ArgList, Error> parse_args(int argc, char* argv[], int begin_offset)
{
	ArgList args;
	Arg_base<MonoValue> plainArg {"", {}};

	int index = begin_offset;
	while (true) {

		// collect args
		auto [args_step, new_args] = get_next_args(argc - index, argv + index);
		index += args_step;

		// determine number of maximum collected arg values
		int count_values = 0;
		if (new_args.size() >= 1) {
			count_values = argc - index;

			if (auto it = arg_constraints.find(new_args.back().name); it != std::end(arg_constraints))
				if (auto max_values_constraint = arg_constraints[new_args.back().name].max_values; max_values_constraint)
					count_values = std::min(count_values, max_values_constraint.value());
		}
		
		// collect arg values
		auto [values_step, new_values] = get_next_single_values(
			count_values,
			argv + index);
		index += values_step;

		// collect plain values
		auto [plain_values_step, new_plain_values] = get_next_single_values(
			argc - index,
			argv + index);
		index += plain_values_step;

		// nothing found? return
		if (args_step == 0 && values_step == 0 && plain_values_step == 0) {
			std::visit(overloaded{
				[] (std::monostate) {},
				[&] (auto&& v) { // add plain args if present
					args.push_back({plainArg.name, v});
				}
			}, plainArg.value);

			// recurrence check
			std::map<std::string, int> occurence_count;
			for (const auto& arg : args)
				occurence_count[arg.name]++;
			for (const auto& count : occurence_count)
				if (auto it = arg_constraints.find(count.first); it != std::end(arg_constraints))
					if (it->second.max_occur)
						if (it->second.max_occur.value() < count.second)
							return {{}, {{ERROR_ARG_MAX_OCCURRENCES, count.first}}};

			return {args, {}};
		}

		// unknown args? error
		for (const auto& arg : new_args)
			if (arg_constraints.find(arg.name) == std::end(arg_constraints))
				return {{}, {{ERROR_ARG_UNKNOWN, arg.name}}};

		// not enough values for arg found? error
		for (int i = 0; i < static_cast<int>(new_args.size()) - 1; i++) {
			if (check_min_arg_values(new_args[i], 0)) {
				return {{}, {{ERROR_ARG_MIN_VALUES, new_args[i].name}}};
			}
		}
		if (new_args.size() >= 1) {
			if (check_min_arg_values(new_args.back(), values_step))
				return {{}, {{ERROR_ARG_MIN_VALUES, new_args.back().name}}};
		}

		// add args
		args.insert(std::end(args), std::begin(new_args), std::end(new_args));

		// add arg values
		if (args.size() >= 1 && new_values.size() >= 1) {
			if (new_values.size() == 1)
				std::visit([&args] (auto&& v) { args.back().value = v; }, new_values[0]);
			else
				args.back().value = new_values;
		}

		// add plain values
		for (const auto& value : new_plain_values)
			add_value_to_arg(plainArg, value);
	}
}

bool add_arg(const std::string& name, Constraints constraints)
{
	auto [it, inserted] = arg_constraints.insert_or_assign(name, constraints);
	(void)it;
	return inserted;
}
