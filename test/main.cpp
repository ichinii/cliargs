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

	add_arg("x", {.min_values = 1, .max_values = 1, .max_occur = 1});
	add_arg("y", {.min_values = 0, .max_values = 2, .max_occur = 1});

	auto [args, error] = parse_args(argc, argv);

	if (error) {
		auto [code, arg] = error.value();
		std::cout << "ERROR: " << code << ", ARG: " << arg << std::endl;
		return 0;
	}

	for (const auto& arg : args) {
		std::visit([&arg] (auto&& v) {
			std::cout << (arg.name == "" ? "?" : arg.name) << " " << v << std::endl;
		}, arg.value);
	}

	return 0;
}
