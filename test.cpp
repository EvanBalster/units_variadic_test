#include <iostream>
#include <iomanip>

#define COMPILE_ERRORS 0
#define PAIR_STYLE 1

#if PAIR_STYLE
	#include "vp_units.h"
	using namespace vp_units;
#else
	#error "Not supported anymore"
	#include "v_units.h"
	using namespace v_units;
#endif

namespace dims = v_dimensions;

using std::ratio;


template<class Base, class Exp>
void printDimension()
{
	// PRETTY PRINTING HACK, NOT RELIABLY PORTABLE
	const char *fullName = typeid(Base).name(), *name = fullName;
	bool prevLC = false;
	while (*fullName != '\0')
	{
		if (*fullName < 'a' || *fullName > 'z') prevLC = false;
		else if (!prevLC) {name = fullName; prevLC = true;}
		++fullName;
	}

	std::cout << std::setw(9) << name << '^' << std::setw(2) << Exp::num;
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
	print(dimensions<dims::time, ratio<-1>, dims::length, ratio<1>>());

	// Invalid unit: time dimension has exponent 0
	print(dimensions<dims::time, ratio<0>>());
	
	// Incompatible assignment, simple case
	dim_length m = divide_t<dim_length, dim_time>();
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
