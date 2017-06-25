#pragma once

#include <type_traits>
#include <typeindex>
#include <typeinfo>

/*
	Primitive dimensions
*/

namespace v_dimensions
{
	#define DEFINE_BASE_DIMENSION(NAME_DECLARED, UNIQUE_ID_VAL, NAME_STR, SYMBOL_STR) \
		struct NAME_DECLARED \
		{ \
			static const int unique_id = (UNIQUE_ID_VAL); \
			static const char* name() {return NAME_STR;} \
			static const char* symbol() {return SYMBOL_STR;} \
		}
	
	// SI units base dimensions
	DEFINE_BASE_DIMENSION(length,      100, "Length",      "L");
	DEFINE_BASE_DIMENSION(mass,        200, "Mass",        "M");
	DEFINE_BASE_DIMENSION(time,        300, "Time",        "T");
	DEFINE_BASE_DIMENSION(charge,      400, "Charge",      "Q");
	DEFINE_BASE_DIMENSION(temperature, 500, "Temperature", "K");
	DEFINE_BASE_DIMENSION(substance,   600, "Substance",   "N");
	DEFINE_BASE_DIMENSION(luminance,   700, "Luminance",   "J");
	
	// Useful non-SI base dimensions
	DEFINE_BASE_DIMENSION(angle,       10100, "Angle",       "A");
	DEFINE_BASE_DIMENSION(data_length, 10200, "Data Length", "B");

	// Easy to create user-defined base dimensions as well.
}

namespace v_dimension_traits
{
	template<class D1, class D2>
	struct equal : std::is_same<D1, D2>
	{
		static_assert((D1::unique_id == D2::unique_id) == std::is_same<D1, D2>::value,
			"Two different base dimensions are using the same unique_id.");
	};

	template<class D1, class D2>
	struct before : std::integral_constant<bool, (D1::unique_id < D2::unique_id)> {};
	
	template<class D1, class D2>
	struct after : std::integral_constant<bool, (D1::unique_id > D2::unique_id)> {};
}
