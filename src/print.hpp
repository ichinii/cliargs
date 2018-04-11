#ifndef CLIARGS_PRINT_HPP
#define CLIARGS_PRINT_HPP

#include <ostream>
#include "parser.hpp"

auto& operator<< (std::ostream& os, const cliargs::impl::SingleValueList& values)
{
	for (const cliargs::impl::SingleValue& value : values)
		std::visit([&] (auto&& v) { os << "[" << v << "]"; }, value);
	return os;
}

#endif // CLIARGS_PRINT_HPP
