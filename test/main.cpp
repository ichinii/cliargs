#include <iostream>
#include <parser.hpp>
#include <print.hpp>

int main(int argc, char* argv[])
{
	std::cout << std::boolalpha;

	cliargs::add_arg("x");
	cliargs::add_processor<bool> ("x", [] (bool  b) { std::cout <<  "x<bool> = " << b << std::endl; });
	cliargs::add_processor<float>("x", [] (float b) { std::cout << "x<float> = " << b << std::endl; });
	cliargs::add_processor<std::string>("x", [] (std::string b) { std::cout << "x<std::string> = " << b << std::endl; });

	auto [args, error] = cliargs::process_args(argc, argv);

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
