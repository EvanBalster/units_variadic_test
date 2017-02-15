#pragma once

#include <type_traits>
#include <typeindex>
#include <typeinfo>

/*
	Primitive dimensions
*/

namespace v_dimensions
{
	// SI units
	struct length      { static const int DIM_KEY = 100; static const char* name() {return "Length";} };
	struct mass        { static const int DIM_KEY = 200; static const char* name() {return "Mass";} };
	struct time        { static const int DIM_KEY = 300; static const char* name() {return "Time";} };
	struct charge      { static const int DIM_KEY = 400; static const char* name() {return "charge";} };
	struct temperature { static const int DIM_KEY = 500; static const char* name() {return "Temperature";} };
	struct substance   { static const int DIM_KEY = 600; static const char* name() {return "Substance";} };
	struct luminance   { static const int DIM_KEY = 700; static const char* name() {return "Luminance";} };

	// Useful nonstandard units
	struct angle       { static const int DIM_KEY = 10100; static const char* name() {return "Angle";} };
	struct data_length { static const int DIM_KEY = 10200; static const char* name() {return "Data Length";} };

	// Easy to create user-defined types as well.
}

namespace v_dimension_traits
{
	template<class D1, class D2>
	struct equal
	{
		static_assert((D1::DIM_KEY == D2::DIM_KEY) == std::is_same<D1, D2>::value,
			"Two different dimensions are using the same DIM_KEY.");
		
		static const bool value = std::is_same<D1, D2>::value;
	};

	template<class D1, class D2>
	struct before
	{
		// Integer method
		static const bool value = D1::DIM_KEY < D2::DIM_KEY;
		
		// Typeid method
		//static const bool value = typeid(D1).before(typeid(D2));
		
		// type_index method
		//static const bool value = std::type_index(typeid(D1)) < std::type_index(typeid(D2));
	};
	
	template<class D1, class D2>
	struct after
	{
		// Integer method
		static const bool value = D1::DIM_KEY > D2::DIM_KEY;
		
		// Typeid method
		//static const bool value = typeid(D2).before(typeid(D1));
	};
}
