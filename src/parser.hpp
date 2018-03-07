#include <vector>
#include <variant>
#include <string>
#include <cassert>
#include <tuple>
#include <optional>

using SingleValue = std::variant<bool, float, std::string>;
using SingleValueList = std::vector<SingleValue>;
using Value = std::variant<bool, float, std::string, SingleValueList>;

struct Arg {
	std::string name;
	Value value;
};
using ArgList = std::vector<Arg>;

bool is_single_arg(const std::string& str);
bool is_multi_arg(const std::string& str);
bool is_arg(const std::string& str);
bool is_value(const std::string& str);
ArgList get_single_arg(const std::string& args_str);
ArgList get_multi_args(const std::string& args_str);
std::tuple<int, ArgList> get_next_args(int count, char* argv[]);
std::optional<float> try_string_to_float(const std::string& str);
SingleValue string_to_single_value(const std::string& str);
std::tuple<int, SingleValueList> get_next_single_values(int count, char* argv[]);
ArgList parse_args(int argc, char* argv[], int offset = 1);
