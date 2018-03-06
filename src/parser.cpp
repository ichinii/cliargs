#include "parser.hpp"

bool is_short_arg(const std::string& str)
{
	return str.length() >= 2 && str[0] == '-' && str[1] != '-';
}

bool is_long_arg(const std::string& str)
{
	return str.length() >= 3 && str[0] == '-' && str[1] == '-' && str[2] != '-';
}

bool is_arg(const std::string& str)
{
	return is_long_arg(str) || is_short_arg(str);
}

bool is_value(const std::string& str)
{
	return str.length() >= 1 && str[0] != '-';
}

ArgList get_single_arg(const std::string& args_str)
{
	assert(is_long_arg(args_str));

	return {Arg{args_str.substr(2), true}};
}

ArgList get_multi_args(const std::string& args_str)
{
	assert(is_short_arg(args_str));

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
		if (is_long_arg(argv[i]))
		{
			auto short_args = get_single_arg(argv[i]);
			args.insert(std::end(args), std::begin(short_args), std::end(short_args));
			step++;
		}
		else if (is_short_arg(argv[i]))
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
	try { return std::stoi(str); } catch(std::exception ex) { return {}; }
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

ArgList parse_args(int argc, char* argv[], int offset)
{
	ArgList args;

	int index = offset;
	while (true) {
		auto [args_step, new_args] = get_next_args(argc - index, argv + index);
		index += args_step;

		auto [values_step, new_values] = get_next_single_values(argc - index, argv + index);
		index += values_step;

		if (args_step == 0 && values_step == 0)
			return args;

		args.insert(std::end(args), std::begin(new_args), std::end(new_args));

		if (new_values.size() > 0) {
			if (args.size() == 0)
				args.push_back(Arg{"", {}});

			if (new_values.size() == 1)
				std::visit([&args] (auto&& v) { args.back().value = v; }, new_values[0]);
			else
				args.back().value = new_values;
		}
	}
}
