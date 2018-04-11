#include <map>
#include <cassert>
#include "parser.hpp"
#include <iostream>

namespace cliargs {

namespace impl {
	std::map<std::string, Constraints> arg_constraints;
	std::map<std::string, ProcessFunctions> arg_processors;
}

using namespace impl;

::std::tuple<ArgList, Error> parse_args(int argc, char* argv[], int begin_offset)
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

			if (auto it = arg_constraints.find(new_args.back().name); it != ::std::end(arg_constraints))
				if (auto max_values_constraint = arg_constraints[new_args.back().name].max_values; max_values_constraint)
					count_values = ::std::min(count_values, max_values_constraint.value());
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
			::std::visit(overloaded{
				[] (::std::monostate) {},
				[&] (auto&& v) { // add plain args if present
					args.push_back({plainArg.name, v});
				}
			}, plainArg.value);

			// recurrence check
			::std::map<::std::string, int> occurence_count;
			for (const auto& arg : args)
				occurence_count[arg.name]++;
			for (const auto& count : occurence_count)
				if (auto it = arg_constraints.find(count.first); it != ::std::end(arg_constraints))
					if (it->second.max_occur)
						if (it->second.max_occur.value() < count.second)
							return {{}, {{ERROR_ARG_MAX_OCCURRENCES, count.first}}};

			return {args, {}};
		}

		// unknown args? error
		for (const auto& arg : new_args)
			if (arg_constraints.find(arg.name) == ::std::end(arg_constraints))
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
		args.insert(::std::end(args), ::std::begin(new_args), ::std::end(new_args));

		// add arg values
		if (args.size() >= 1 && new_values.size() >= 1) {
			if (new_values.size() == 1)
				::std::visit([&args] (auto&& v) { args.back().value = v; }, new_values[0]);
			else
				args.back().value = new_values;
		}

		// add plain values
		for (const auto& value : new_plain_values)
			add_value_to_arg(plainArg, value);
	}
}

::std::tuple<impl::ArgList, impl::Error> process_args(int argc, char* argv[], int begin_offset)
{
	return process_args(parse_args(argc, argv, begin_offset));
}

::std::tuple<impl::ArgList, impl::Error> process_args(::std::tuple<impl::ArgList, impl::Error> args)
{
	auto [arglist, error] = args;
	(void)error;

	for (auto& arg : arglist) {
		std::visit(arg_processors[arg.name], arg.value);
	}

	return args;
}

} // namespace cliargs
