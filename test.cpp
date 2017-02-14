#include <iostream>

#include "v_units.h"

#define COMPILE_ERRORS 0

namespace dims = v_dimensions;

using namespace v_units;


template<class Dimension>
void printDimension()
{
	const char *fullName = typeid(typename Dimension::base).name(), *name = fullName;

	while (*fullName != '\0')
	{
		if (*fullName == ':') name = fullName + 1;
		++fullName;
	}

	std::cout << name << '^' << Dimension::exponent::num;
	if (Dimension::exponent::den != 1) std::cout << '/' << Dimension::exponent::den;
}

template<class... Dimensions>
struct printDimensions_impl
{
	//using type = int;
	
	static void print() {}
};

template<class Dimension1, class... Dimensions>
struct printDimensions_impl<Dimension1, Dimensions...>
{
	//using type = printDimensions_impl<Dimensions...>::type;

	static void print()
	{
		printDimension<Dimension1>();
		std::cout << " ";
		printDimensions_impl<Dimensions...>::print();
	}
};


template<class... Dimensions>
void print(const char *label, const dimensions<Dimensions...> &dim)
{
	std::cout << label << " => ";
	printDimensions_impl<Dimensions...>::print();
	std::cout << std::endl;
}


int main(int argc, char **argv)
{
	typedef dimension<dims::length, std::ratio<1, 1>> length_1;
	typedef dimension<dims::length, std::ratio<2, 1>> length_2;

	typedef dimension<dims::time, std::ratio< 1, 1>>  time_1;
	typedef dimension<dims::time, std::ratio< 0, 1>>  time_0; // Invalid if used in a unit!
	typedef dimension<dims::time, std::ratio<-1, 1>> time_n1;

	using dim_velocity = dimensions<length_1, time_n1>;
	using dim_time     = dimensions<time_1>;

	// Valid unit
	print("velocity", dim_velocity());

	// Inverse
	print("inverse velocity", inverse<dim_velocity>());

	// Multiply without cancellation
	print("velocity x velocity", multiply<dim_velocity, dim_velocity>());

	// Multiply with cancellation
	print("velocity x time", multiply<dim_velocity, dimensions<time_1>>());

#if COMPILE_ERRORS
	// Invalid unit: dimensions must be ordered with respect to 
	print(dimensions<time_n1, length_1>);

	// Invalid unit: time dimension has exponent 0
	print(dimensions<time_0, length_1>);
#endif

	std::cout << "It works!" << std::endl;

	std::cin.ignore(255, '\n');

	return 0;
}
