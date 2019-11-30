#ifndef CLIARGS_ERROR_HPP
#define CLIARGS_ERROR_HPP

#include <string>
#include <functional>

namespace cliargs {

enum ErrorCode {

};

struct Error {
	ErrorCode code;
	const char* name;
	const char* string;
	std::string arg;
};

using ErrorFn = std::function<void(Error)>;

}

#endif
