#ifndef CLIARGS_PARSE_HPP
#define CLIARGS_PARSE_HPP

#include <cassert>
#include "error.hpp"
#include "value.hpp"

namespace cliargs {

template <typename ...Ts>
struct overloaded : public Ts... { using Ts::operator()...; };
template <typename ...Ts>
overloaded(Ts...) -> overloaded<Ts...>;

struct ParsedArg {
	std::string name;
	ValueList values;
};

using ParsedArgList = std::vector<ParsedArg>;

using ResultArg = std::variant<Error, ParsedArg>;
using ResultArgList = std::vector<ResultArg>;

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

ParsedArg get_single_arg(const std::string& args_str)
{
	assert(is_single_arg(args_str));

	return {args_str.substr(2), {}};
}

ParsedArgList get_multi_args(const std::string& args_str)
{
	assert(is_multi_arg(args_str));

	ParsedArgList args;

	for (const auto& args_char : args_str.substr(1))
		args.push_back({std::string(1, args_char), {}});

	return args;
}

std::tuple<std::size_t, ParsedArgList> get_next_args(int count, char* argv[])
{
	std::size_t step = 0;
	ParsedArgList args;

	for (int i = 0; i < count; i++) {
		/*if (is_single_arg(argv[i]))
		{
			auto short_args = get_single_arg(argv[i]);
			args.insert(std::end(args), std::begin(short_args), std::end(short_args));
			step++;
		}
		else*/ if (is_multi_arg(argv[i]))
		{
			auto multi_args = get_multi_args(argv[i]);
			args.insert(std::end(args), std::begin(multi_args), std::end(multi_args));
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

Value string_to_single_value(const std::string& str)
{
	assert(is_value(str));

	if (auto value = try_string_to_float(str); value)
		return value.value();
	return str;
}

std::tuple<int, ValueList> get_next_values(int count, char* argv[])
{
	int step = 0;
	ValueList values;

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

void add_values_to_arg(ParsedArg& arg, ValueList values)
{
	arg.values.insert(std::end(arg.values), std::begin(values), std::end(values));
}

}

#endif
