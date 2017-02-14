#include <iostream>
#include <iomanip>

#include "v_units.h"

#define COMPILE_ERRORS 0

namespace dims = v_dimensions;

using namespace v_units;


template<class Dimension>
void printDimension()
{
	// PRETTY PRINTING HACK, NOT RELIABLY PORTABLE
	const char *fullName = typeid(typename Dimension::base).name(), *name = fullName;
	bool prevLC = false;
	while (*fullName != '\0')
	{
		if (*fullName < 'a' || *fullName > 'z') prevLC = false;
		else if (!prevLC) {name = fullName; prevLC = true;}
		++fullName;
	}

	std::cout << std::setw(9) << name << '^' << std::setw(2) << Dimension::exponent::num;
	if (Dimension::exponent::den != 1) std::cout << '/' << Dimension::exponent::den;
	else std::cout << "  ";
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
	std::cout << std::setw(25) << label << " ==> 1 ";
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
	typedef dimension<dims::time, std::ratio<-2, 1>> time_n2;

	typedef dimension<dims::charge,      std::ratio<1, 1>> charge_1;
	typedef dimension<dims::temperature, std::ratio<1, 1>> temperature_1;
	typedef dimension<dims::substance,   std::ratio<1, 1>> substance_1;
	typedef dimension<dims::mass,        std::ratio<1, 1>> mass_1;

	using dim_velocity = dimensions<length_1, time_n1>;
	using dim_time     = dimensions<time_1>;

	using dim_force = dimensions<length_1, mass_1, time_n2>;
	
#if COMPILE_ERRORS
	// Invalid unit: dimensions must be ordered with respect to DIM_ID
	print(dimensions<time_n1, length_1>());

	// Invalid unit: time dimension has exponent 0
	print(dimensions<time_0, length_1>());
#endif

	// Valid unit
	print("           velocity", dim_velocity());

	// Inverse
	print("   inverse velocity", inverse_t<dim_velocity>());

	// Multiply without cancellation
	print("velocity x velocity", multiply_t<dim_velocity, dim_velocity>());

	// Multiply with cancellation
	print("velocity x time    ", multiply_t<dim_velocity, dimensions<time_1>>());

	// Multiply, heterogenous
	print("velocity x area    ", multiply_t<dim_velocity, dimensions<length_2>>());

	// Full cancellation
	print("velocity / velocity", divide_t<dim_velocity, dim_velocity>());

	// Fractional power
	print("velocity ^ 8/3     ", pow_t<dim_velocity, std::ratio<8, 3>>());

	print("   force           ", dim_force());
	print("   force x time    ", multiply_t<dim_force, dimensions<time_1>>());
	print("   force x time^2  ", multiply_t<dim_force, pow_t<dimensions<time_1>, std::ratio<2>>>());
	print("   force x time^3  ", multiply_t<dim_force, pow_t<dimensions<time_1>, std::ratio<3>>>());
	print("   force / mass    ", divide_t  <dim_force, dimensions<mass_1>>());

	std::cout << std::endl << "Press enter to terminate." << std::endl;
	std::cin.ignore(255, '\n');

	return 0;
}
