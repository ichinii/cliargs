#include <iostream>
#include "parser.hpp"

std::ostream& operator<< (std::ostream& os, const SingleValueList& values)
{
	for (const SingleValue& value : values)
		std::visit([&] (auto&& v) { os << "[" << v << "]"; }, value);
	return os;
}

int main(int argc, char* argv[])
{
	std::cout << std::boolalpha;

	auto args = parse_args(argc, argv);

	for (const auto& arg : args) {
		std::visit([&arg] (auto&& v) {
			std::cout << (arg.name == "" ? "[VALUEs]" : arg.name) << " " << v << std::endl;
		}, arg.value);
	}

	return 0;
}
