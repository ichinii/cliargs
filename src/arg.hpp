#ifndef CLIARGS_ARG_HPP
#define CLIARGS_ARG_HPP

#include <optional>
#include "value.hpp"
#include "error.hpp"
#include "processor.hpp"

namespace cliargs {

class Arg {
	friend class System;

public:
	Arg& add_multi_name(char multi_name)
	{
		m_multi_names.push_back(multi_name);
		return *this;
	}

	Arg& add_single_name(std::string single_name)
	{
		m_single_names.push_back(single_name);
		return *this;
	}

	Arg& value(Value value)
	{
		m_value = value;
		return *this;
	}

	Arg& min_values(std::size_t value)
	{
		m_min_values = value;
		return *this;
	}

	Arg& max_values(std::size_t value)
	{
		m_max_values = value;
		return *this;
	}

	Arg& min_occur(std::size_t value)
	{
		m_min_occur = value;
		return *this;
	}

	Arg& max_occur(std::size_t value)
	{
		m_max_occur = value;
		return *this;
	}

	Arg& help_short(std::string value)
	{
		m_help_short = value;
		return *this;
	}

	Arg& help_long(std::string value)
	{
		m_help_long = value;
		return *this;
	}

	Arg& requires(std::string arg_name)
	{
		m_requires.push_back(arg_name);
		return *this;
	}

	Arg& error(ErrorFn fn)
	{
		m_error_fn = fn;
		return *this;
	}

	Arg& processor(ProcessorFn fn)
	{
		m_processor_fn = fn;
		return *this;
	}

	template <typename T>
	Arg& value_processor(ValueProcessorFn<T> fn)
	{
		m_value_processor_fn.set<T>(fn);
		return *this;
	}

private:
	std::vector<std::string> m_single_names;
	std::vector<char> m_multi_names;
	std::optional<Value> m_value;
	std::optional<std::size_t> m_min_values;
	std::optional<std::size_t> m_max_values;
	std::optional<std::size_t> m_min_occur;
	std::optional<std::size_t> m_max_occur;
	std::optional<std::string> m_help_short;
	std::optional<std::string> m_help_long;
	std::vector<std::string> m_requires;
	ErrorFn m_error_fn;
	ProcessorFn m_processor_fn;
	OverloadedValueProcessorFn m_value_processor_fn;

	bool hasName(const std::string& name) const
	{
		for (const auto& singleName : m_single_names)
			if (name == singleName)
				return true;
		for (const auto& multiName : m_multi_names)
			if (name == std::string(1, multiName))
				return true;
		return false;
	}
};

inline Arg DefaultArg()
{
	return Arg().add_single_name("");
}

}

#endif
