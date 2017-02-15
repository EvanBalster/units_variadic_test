#include <iostream>
#include <iomanip>

#define COMPILE_ERRORS 0

#include "vp_units.h"
using namespace vp_units;

namespace dims = v_dimensions;

using std::ratio;


template<class Base, class Exp>
void printDimension()
{
	std::cout << std::setw(9) << Base::name() << '^' << std::setw(2) << Exp::num;
	if (Exp::den != 1) std::cout << '/' << Exp::den;
	else std::cout << "  ";
}

template<class... Dimensions>
struct printDimensions_impl
{
	//using type = int;
	
	static void print() {}
};

template<class Base1, class Exp1, class... Dimensions>
struct printDimensions_impl<Base1, Exp1, Dimensions...>
{
	//using type = printDimensions_impl<Dimensions...>::type;

	static void print()
	{
		printDimension<Base1, Exp1>();
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


#if COMPILE_ERRORS
	// Test what happens when we declare two dimensions with the same unique key
	struct twin_dimension_a { static const int DIM_KEY = 123456; static const char* name() {return "Twin-A";} };
	struct twin_dimension_b { static const int DIM_KEY = 123456; static const char* name() {return "Twin-B";} };
#endif

int main(int argc, char **argv)
{
	// Simple dimensions
	using dim_length = dimensions<dims::length, ratio<1>>;
	using dim_time   = dimensions<dims::time, ratio<1>>;
	using dim_mass   = dimensions<dims::mass, ratio<1>>;

	// More complicated dimensions
	using dim_velocity = dimensions<dims::length, ratio<1>, dims::time, ratio<-1>>;
	using dim_force    = dimensions<dims::length, ratio<1>, dims::mass, ratio<1>, dims::time, ratio<-2>>;
	
#if COMPILE_ERRORS
	// Invalid unit: dimensions must be ordered with respect to DIM_ID to avoid redundant types
	print("Trouble!", dimensions<dims::time, ratio<-1>, dims::length, ratio<1>>());

	// Invalid unit: time dimension has exponent 0
	print("Trouble!", dimensions<dims::time, ratio<0>>());
	
	// Incompatible assignment, simple case
	dim_length m = divide_t<dim_length, dim_time>();
	
	using twin_a = dimensions<twin_dimension_a, ratio<1>>;
	using twin_b = dimensions<twin_dimension_b, ratio<1>>;
	
	print("TWINNING...", multiply_t<twin_a, twin_b>());
	print("TWINNING...", dimensions<twin_dimension_a, ratio<1>, twin_dimension_b, ratio<1>>());
#endif

	// Valid unit
	print("           velocity", dim_velocity());

	// Inverse
	print("   inverse velocity", inverse_t<dim_velocity>());

	// Multiply without cancellation
	print("velocity x velocity", multiply_t<dim_velocity, dim_velocity>());

	// Multiply with cancellation
	print("velocity x time    ", multiply_t<dim_velocity, dimensions<dims::time, ratio<1>>>());

	// Multiply, heterogenous
	print("velocity x area    ", multiply_t<dim_velocity, dimensions<dims::time, ratio<2>>>());

	// Full cancellation
	print("velocity / velocity", divide_t<dim_velocity, dim_velocity>());

	// Fractional power
	print("velocity ^ 8/3     ", pow_t<dim_velocity, ratio<8, 3>>());

	print("   force           ", dim_force());
	print("   force x time    ", multiply_t<dim_force, dim_time>());
	print("   force x time^2  ", multiply_t<dim_force, pow_t<dim_time, ratio<2>>>());
	print("   force x time^3  ", multiply_t<dim_force, pow_t<dim_time, ratio<3>>>());
	print("   force / mass    ", divide_t  <dim_force, dim_mass>());
	

	std::cout << std::endl << "Press enter to terminate." << std::endl;
	std::cin.ignore(255, '\n');

	return 0;
}
