#ifndef CLIARGS_VALUE_HPP
#define CLIARGS_VALUE_HPP

#include <string>
#include <vector>
#include <variant>
#include <tuple>

namespace cliargs {

using Value = std::variant<bool, float, std::string>;
using ValueList = std::vector<Value>;

}

#endif
