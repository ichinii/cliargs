#include <iostream>
#include <typeinfo>
#include <cliargs.hpp>

int main(int argc, char* argv[])
{
	(void)argc;
	(void)argv;

	std::cout << std::boolalpha;

	auto system = cliargs::System()
		.add_help_multi_name('h')
		.add_help_single_name("help")
		.error([] (cliargs::Error err) {
			std::cout << "ERROR: " << err.code
				<< ", arg: " << err.arg
				<< ", name: " << err.name
				<< ", string: " << err.string << std::endl;
		});

	system.add_arg(cliargs::DefaultArg()
		.processor([] () { std::cout << "\"\": processor" << std::endl; })
		.value_processor<bool>([] (bool value) { std::cout << "\"\": value_processor<bool>: " << value << std::endl; })
		.value_processor<float>([] (float value) { std::cout << "\"\": value_processor<float>: " << value << std::endl; })
	);

	system.add_arg(cliargs::Arg()
		.add_multi_name('w')
		.add_single_name("helloworld")
		.processor([] () { std::cout << "w: processor" << std::endl; })
		.value_processor<bool>([] (bool value) { std::cout << "w: value_processor<bool>: " << value << std::endl; })
		.value_processor<float>([] (float value) { std::cout << "w: value_processor<float>: " << value << std::endl; })
	);

	system.process(argc, argv);

	/*
	{
		cliargs::System system;
		system.set_help("-h", "--help");
		system.dry_help(true);
		system.error(false, [] (cliargs::Error err) {
			std::cout << "ERROR: " << err.code << ", arg: " << err.arg << ", string: " << err.string << std::endl;
		});

		cliargs::Arg("-w", "--helloworld")
			.append_to(&system)
			.min_values(0)
			.max_values(0)
			.min_occur(0)
			.max_occur(1)
			.help_short(true, "prints 'Hello World!'");
			.error(true);
			.processor([&] () {
				for (std::size_t i = 0; i < system.values["--count"].value(); i++)
					std::cout << "Hello World!" << std::endl;
			})

		cliargs::Arg("-c", "--count")
			.append_to(&system)
			.value(1)
			.min_values(1)
			.max_values(1)
			.min_occur(0)
			.max_occur(1)
			.requires("--helloworld")
			.help_short(true, "count of prints by --helloworld");

		cliargs::Arg("-x")
			.append_to(&system)
			.min_values(1)
			.value_processor([] (auto v) { std::cout << "x = " << v << ", type = " << typeid(v).name << std::endl; });
	}
	*/

	/*
	cliargs::add_arg("x", {.max_values = 1});
	cliargs::add_processor<bool>("x", [] (bool b) { std::cout << "x<bool> = " << b << std::endl; });
	cliargs::add_processor<float>("x", [] (float b) { std::cout << "x<float> = " << b << std::endl; });
	cliargs::add_processor<std::string>("x", [] (std::string b) { std::cout << "x<std::string> = " << b << std::endl; });

	cliargs::add_arg("y");
	cliargs::add_processor<float>("y", [] (float f) { std::cout << "y<float> = " << f << std::endl; });

	auto [args, error] = cliargs::process_args(argc, argv);
	(void)args;
	(void)error;

	if (error) {
		auto [code, arg] = error.value();
		std::cout << "ERROR: " << code << ", ARG: " << arg << std::endl;
		return 0;
	}
	*/

	/*
	for (const auto& arg : args) {
		std::visit([&arg] (auto&& v) {
			std::cout << (arg.name == "" ? "?" : arg.name) << " " << v << std::endl;
		}, arg.value);
	}
	*/

	return 0;
}
