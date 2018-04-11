#include <cassert>
#include "parser.hpp"

namespace cliargs {

namespace impl {

bool is_single_arg(const ::std::string& str)
{
	return str.length() >= 3 && str[0] == '-' && str[1] == '-' && str[2] != '-';
}

bool is_multi_arg(const ::std::string& str)
{
	return str.length() >= 2 && str[0] == '-' && str[1] != '-';
}

bool is_arg(const ::std::string& str)
{
	return is_single_arg(str) || is_multi_arg(str);
}

bool is_value(const ::std::string& str)
{
	return str.length() >= 1 && str[0] != '-';
}

ArgList get_single_arg(const ::std::string& args_str)
{
	assert(is_single_arg(args_str));

	return {Arg{args_str.substr(2), true}};
}

ArgList get_multi_args(const ::std::string& args_str)
{
	assert(is_multi_arg(args_str));

	ArgList args;
	auto it = ::std::back_inserter(args);

	for (const auto& args_char : args_str.substr(1))
		it = {::std::string(1, args_char), true};

	return args;
}

::std::tuple<int, ArgList> get_next_args(int count, char* argv[])
{
	int step = 0;
	ArgList args;

	for (int i = 0; i < count; i++) {
		/*if (is_single_arg(argv[i]))
		{
			auto short_args = get_single_arg(argv[i]);
			args.insert(::std::end(args), ::std::begin(short_args), ::std::end(short_args));
			step++;
		}
		else*/ if (is_multi_arg(argv[i]))
		{
			auto short_args = get_multi_args(argv[i]);
			args.insert(::std::end(args), ::std::begin(short_args), ::std::end(short_args));
			step++;
		}
		else
		{
			break;
		}
	}

	return {step, args};
}

::std::optional<float> try_string_to_float(const ::std::string& str)
{
	try { return ::std::stof(str); } catch(::std::exception ex) { return {}; }
}

SingleValue string_to_single_value(const ::std::string& str)
{
	if (auto value = try_string_to_float(str); value)
		return value.value();
	return str;
}

::std::tuple<int, SingleValueList> get_next_single_values(int count, char* argv[])
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

void add_value_to_arg(Arg_base<MonoValue>& arg, SingleValue value)
{
	::std::visit(overloaded{
		[&] (::std::monostate) { // arg.value is empty -> single value
			::std::visit([&] (auto&& value) {
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
	if (auto it = arg_constraints.find(arg.name); it != ::std::end(arg_constraints))
		if (auto opt = it->second.min_values; opt)
			return count < opt.value();
	return false;
}

} // namespace impl

} // namespace cliargs
