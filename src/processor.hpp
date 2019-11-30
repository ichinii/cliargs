#ifndef CLIARGS_PROCESSOR_HPP
#define CLIARGS_PROCESSOR_HPP

#include <functional>
#include "value.hpp"

#include <iostream>
#include <typeinfo>

using ProcessorFn = std::function<void(void)>;

template <typename T>
using ValueProcessorFn = std::function<void(T)>;



template <typename ...Ts>
class OverloadedValueProcessorFn_base : public ValueProcessorFn<Ts>... {
public:
	using ValueProcessorFn<Ts>::operator()...;

	OverloadedValueProcessorFn_base() 
	{
		(..., init<Ts>());
	}

	template <typename T>
	void set(ValueProcessorFn<T> fn)
	{
		dynamic_cast<ValueProcessorFn<T>&>(*this) = fn;
	}

private:
	template <typename T>
	void init()
	{
		set<T>([] (T) {
			std::cout << "unimplemented value processor function called"
			" [T = " << typeid(T).name() << "]" << std::endl; });
	}
};

using OverloadedValueProcessorFn = OverloadedValueProcessorFn_base<bool, float, std::string>;

#endif
