#ifndef CLIARGS_PARSER_HPP
#define CLIARGS_PARSER_HPP

#include <vector>
#include <variant>
#include <string>
#include <tuple>
#include <optional>
#include <map>
#include <memory>
#include <functional>

namespace cliargs {

namespace impl {

using SingleValue     = std::variant<                bool, float, std::string>;
using SingleValueList = std::vector<                 SingleValue>;
using Value           = std::variant<                bool, float, std::string, SingleValueList>;
using MonoValue       = std::variant<std::monostate, bool, float, std::string, SingleValueList>;

enum ErrorCode {
	ERROR_PLAIN_MIN_VALUES,
	ERROR_PLAIN_MAX_VALUES,
	ERROR_ARG_UNKNOWN,
	ERROR_ARG_MAX_OCCURRENCES,
	ERROR_ARG_MIN_VALUES,
	ERROR_ARG_NO_PROCESSOR,
};
using Error = std::optional<std::tuple<ErrorCode, std::string>>;

template <typename Value>
struct Arg_base {
	std::string name;
	Value value;
};

using Arg = Arg_base<Value>;
using ArgList = std::vector<Arg>;

struct Constraints {
	std::optional<int> min_values = {};
	std::optional<int> max_values = {};
	std::optional<int> max_occur = {};
};

// overloaded function
template <typename ...Ts>
struct overloaded : Ts... { using Ts::operator()...; };
template <typename ...Ts>
overloaded(Ts...) -> overloaded<Ts...>;

// functions wrapper
template <typename T>
using process_function = std::function<void(T)>;

template <typename ...Ts>
class ProcessFunctions_base : public process_function<Ts>... {
public:
	using process_function<Ts>::operator()...;

	ProcessFunctions_base() 
	{
		(..., init<Ts>());
	}

private:
	template <typename T>
	void init()
	{
		dynamic_cast<process_function<T>&>(*this) = [] (T) { return Error{{ERROR_ARG_NO_PROCESSOR, ""}} ;};
	}
};

using ProcessFunctions = ProcessFunctions_base<bool, float, std::string, SingleValueList>;

// globals
extern std::map<std::string, Constraints> arg_constraints;
extern std::map<std::string, ProcessFunctions> arg_processors;

// get.cpp
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
void add_value_to_arg(Arg_base<MonoValue>& arg, SingleValue value);
bool check_min_arg_values(Arg& arg, int count);

} // namespace impl

// parse.cpp
std::tuple<impl::ArgList, impl::Error> parse_args(int argc, char* argv[], int begin_offset = 1);
std::tuple<impl::ArgList, impl::Error> process_args(int argc, char* argv[], int begin_offset = 1);
std::tuple<impl::ArgList, impl::Error> process_args(std::tuple<impl::ArgList, impl::Error> args);

inline bool add_arg(const std::string& name, impl::Constraints constraints = {})
{
	auto [it, inserted] = impl::arg_constraints.insert_or_assign(name, constraints);
	(void)it;

	return inserted;
}

template <typename T, typename F>
inline void add_processor(const std::string& name, F f)
{
	auto it = impl::arg_processors.find(name);
	if (it == std::end(impl::arg_processors)) {
		auto [insert_it, inserted] = impl::arg_processors.insert(decltype(impl::arg_processors)::value_type(name, impl::ProcessFunctions{}));

		(void)inserted;
		it = insert_it;
	}

	dynamic_cast<impl::process_function<T>&>(it->second) = f;
}

} // namespace cliargs

#endif // CLIARGS_PARSER_HPP
