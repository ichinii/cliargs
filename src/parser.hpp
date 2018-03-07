#include <vector>
#include <variant>
#include <string>
#include <tuple>
#include <optional>

using SingleValue     = std::variant<                bool, float, std::string>;
using SingleValueList = std::vector<                 SingleValue>;
using Value           = std::variant<                bool, float, std::string, SingleValueList>;
using MonoValue       = std::variant<std::monostate, bool, float, std::string, SingleValueList>;

enum ErrorCode {
	ERROR_ARG_UNKNOWN,
	ERROR_ARG_MAX_OCCURRENCES,
	ERROR_ARG_MIN_VALUES,
	ERROR_PLAIN_MIN_VALUES,
	ERROR_PLAIN_MAX_VALUES,
};
using Error = std::optional<std::tuple<ErrorCode, std::string>>;

template <typename Value>
struct Arg_base {
	std::string name;
	Value value;
};

using Arg = Arg_base<Value>;
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
std::tuple<ArgList, Error> parse_args(int argc, char* argv[], int begin_offset = 1);

struct Constraints {
	std::optional<int> min_values = {};
	std::optional<int> max_values = {};
	std::optional<int> max_occur = {};
};

bool add_arg(const std::string& name, Constraints constraints = {});
