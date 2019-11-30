#ifndef CLIARGS_SYSTEM_HPP
#define CLIARGS_SYSTEM_HPP

#include <type_traits>
#include <string>
#include <vector>
#include <functional>
#include "error.hpp"
#include "arg.hpp"
#include "parse.hpp"

#include <iostream>

namespace cliargs {

class System {
public:
	System()
		: m_dry_help(true)
	{}

	System& add_help_single_name(const std::string& name)
	{
		m_help_single_arg_names.push_back(name);
		return *this;
	}

	System& add_help_multi_name(char name)
	{
		m_help_multi_arg_names.push_back(name);
		return *this;
	}

	System& dry_help(bool value)
	{
		m_dry_help = value;
		return *this;
	}

	System& error(ErrorFn fn)
	{
		m_error_fn = fn;
		return *this;
	}

	System& add_arg(const Arg& arg)
	{
		m_args.push_back(arg);
		return *this;
	}

	ResultArgList parse(int argc, char* argv[], std::size_t begin_offset = 1);
	void process(const ResultArgList& args);
	void process(int argc, char* argv[]);

private:
	bool m_dry_help;
	std::vector<std::string> m_help_single_arg_names;
	std::vector<char> m_help_multi_arg_names;
	ErrorFn m_error_fn;
	std::vector<Arg> m_args;
};

ResultArgList System::parse(int argc, char* argv[], std::size_t begin_offset)
{
	(void)system;

	ResultArgList results;
	ParsedArgList args;
	ParsedArg plainArg;

	auto offset = [&] (std::size_t value) { argc -= value; argv += value; };
	offset(begin_offset);

	while (true) {
		// collect args
		auto [args_step, new_args] = get_next_args(argc, argv);
		offset(args_step);
		
		// collect values
		auto [values_step, arg_values] = get_next_values(argc, argv);
		offset(values_step);
		
		// loop break condition
		if (args_step == 0 && values_step == 0)
			break;

		// add args
		args.insert(std::end(args), std::begin(new_args), std::end(new_args));

		// add arg values
		if (args.size() >= 1)
			add_values_to_arg(args.back(), arg_values);
		else
			add_values_to_arg(plainArg, arg_values);
	}

	for (auto& arg : args)
		if (arg.values.size() == 0)
			arg.values.push_back(true);

	for (const auto& arg : args)
		results.push_back(arg);

	return results;
}

void System::process(const ResultArgList& resultArgs)
{
	for (const auto& arg : m_args)
	{
		bool found_arg = false;

		for (const auto& resultArg : resultArgs)
		{
			std::visit(overloaded{
				[&] (const ParsedArg& parsedArg) {
					if (arg.hasName(parsedArg.name))
					{
						found_arg = true;
						for (const auto& value : parsedArg.values)
							std::visit(arg.m_value_processor_fn, value);
					}
				}, [] (const auto&) {}
			}, resultArg);
		}

		if (found_arg)
			arg.m_processor_fn();
		else
			arg.m_value_processor_fn(false);
	}
}

void System::process(int argc, char* argv[])
{
	return process(parse(argc, argv));
}

}

#endif
