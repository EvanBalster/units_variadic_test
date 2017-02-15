#pragma once

#include <type_traits>
#include <ratio>

#include "v_dimensions.h"

/*
	Variadic paired dimensions header.
		Uses a very terse, human-friendly template for dimensionality
*/

namespace vp_units
{
	using namespace v_dimension_traits;
	
	namespace checks
	{
		/*
			Template to ensure a dimension list adheres to the required ordering.
		*/
		template<class... Dims>
		struct dim_order { static const bool ok = true; };

		template<class Base1, class Exp1, class Base2, class Exp2, class... Dims>
		struct dim_order<Base1, Exp1, Base2, Exp2, Dims...>
		{
			static const bool ok =
				v_dimension_traits::before<Base1, Base2>::value
				&& dim_order<Base2, Exp2, Dims...>::ok;
		};

		/*
			Template to ensure a dimension list contains no zero exponents.
		*/
		template<class... Dims>
		struct exponents { static const bool nonzero = true; static const bool even = true; };

		template<class Base1>
		struct exponents<Base1>
		{
			static const bool even = false;
		};

		template<class Base1, class Exp1, class... Dims>
		struct exponents<Base1, Exp1, Dims...>
		{
			static const bool nonzero = (Exp1::num != 0) && exponents<Dims...>::nonzero;
			static const bool even = exponents<Dims...>::even;
		};
	}
	

	/*
		Pair-based variadic template for a unit type.
			Parameters should be dimensions and std::ratio exponents, alternating, ex:
			dimensions<length, ratio<1,1>, time, ratio<-1,1>>
	*/
	template<class... Dimensions>
	struct dimensions
	{
		/*
			These two type checks ensure that only one valid dimensions template exists for any given dimensionality.
				Dimensions must be defined in order, and must have non-zero exponents.
		*/
		static_assert(checks::dim_order<Dimensions...>::ok,
			"Unit dimensions must be defined in canonical order.");

		static_assert(checks::exponents<Dimensions...>::nonzero,
			"Unit dimensions must have non-zero exponents.");

		static_assert(checks::exponents<Dimensions...>::nonzero,
			"Unit dimensions must always be followed by a std::ratio exponent.");
	};


	namespace util
	{
		/*
			Compose dimensions by adding a base and power to the front or back of the list.
				This does not automatically place items in order!!
				It is used in other templates below.
		*/
		template<class Base1, class Exp1, class Dims> struct dim_push_front {};
		template<class BaseN, class ExpN, class Dims> struct dim_push_back {};

		template<class Base1, class Exp1, class... Dims>
		struct dim_push_front<Base1, Exp1, dimensions<Dims...> > { using type = dimensions<Base1, Exp1, Dims...>; };
		template<class BaseN, class ExpN, class... Dims>
		struct dim_push_back <BaseN, ExpN, dimensions<Dims...> > { using type = dimensions<Dims..., BaseN, ExpN>; };

		template<class Base1, class Exp1, class Dimensions>
		using dim_push_front_t = typename dim_push_front<Base1, Exp1, Dimensions>::type;
		template<class BaseN, class ExpN, class Dimensions>
		using dim_push_back_t  = typename dim_push_back<BaseN, ExpN, Dimensions>::type;
	}
	

	/*
		Raise dimensions to a rational power (always a std::ratio)
	*/
	template<class Dimensions, class Power> struct pow_impl {};

	template<class Power>
	struct pow_impl<dimensions<>, Power>
	{
		// Base case
		using type = dimensions<>;
	};

	template<class Base1, class Exp1, class... Dims, class Power>
	struct pow_impl<dimensions<Base1, Exp1, Dims...>, Power>
	{
		// Recursive case
		using type = util::dim_push_front_t<
			Base1, std::ratio_multiply<Exp1, Power>,
			typename pow_impl<dimensions<Dims...>, Power>::type>;
	};

	template<class Dimensions, class Power>
	using pow_t = typename pow_impl<Dimensions, Power>::type;


	/*
		Shorthand for inverse
	*/
	template<class Dimensions>
	using inverse_t = pow_t<Dimensions, std::ratio<-1>>;


	/*
		Dimensions multiplication template.  Breaks down into various cases...
	*/
	template<class A, class B, class Enable = void> struct multiply_impl {};

	// A: B is dimensionless; return A.
	template<class... A_Rest>
	struct multiply_impl<dimensions<A_Rest...>, dimensions<>, void> { using type = dimensions<A_Rest...>; };

	// B: A is dimensionless; return B (which is not).
	template<class BBase, class BExp, class... B_Rest>
	struct multiply_impl<dimensions<>, dimensions<BBase, BExp, B_Rest...>, void> { using type = dimensions<BBase, BExp, B_Rest...>; };

	// C: A's first dimension precedes B's
	template<class ABase, class AExp, class BBase, class BExp, class... A_Rest, class... B_Rest >
	struct multiply_impl<dimensions<ABase, AExp, A_Rest...>, dimensions<BBase, BExp, B_Rest...>,
		typename std::enable_if<v_dimension_traits::before<ABase, BBase>::value>::type>
	{
		using type = util::dim_push_front_t<ABase, AExp,
			typename multiply_impl<dimensions<A_Rest...>, dimensions<BBase, BExp, B_Rest...> >::type>;
	};

	// D: B's first dimension precedes A's
	template<class ABase, class AExp, class BBase, class BExp, class... A_Rest, class... B_Rest>
	struct multiply_impl<dimensions<ABase, AExp, A_Rest...>, dimensions<BBase, BExp, B_Rest...>,
		typename std::enable_if<v_dimension_traits::after<ABase, BBase>::value>::type>
	{
		using type = util::dim_push_front_t<BBase, BExp,
			typename multiply_impl<dimensions<ABase, AExp, A_Rest...>, dimensions<B_Rest...> >::type>;
	};

	// E: A's first dimension is compatible with B's and they don't cancel out
	template<class ABase, class AExp, class BBase, class BExp, class... A_Rest, class... B_Rest>
	struct multiply_impl<dimensions<ABase, AExp, A_Rest...>, dimensions<BBase, BExp, B_Rest...>,
		typename std::enable_if<v_dimension_traits::equal<ABase, BBase>::value
			&& (std::ratio_add<AExp, BExp>::num != 0)>::type>
	{
		using type = util::dim_push_front_t<ABase, std::ratio_add<AExp, BExp>,
			typename multiply_impl<dimensions<A_Rest...>, dimensions<B_Rest...> >::type>;
	};

	// E: A's first dimension is compatible with B's and they cancel out
	template<class ABase, class AExp, class BBase, class BExp, class... A_Rest, class... B_Rest>
	struct multiply_impl<dimensions<ABase, AExp, A_Rest...>, dimensions<BBase, BExp, B_Rest...>,
		typename std::enable_if<v_dimension_traits::equal<ABase, BBase>::value
			&& (std::ratio_add<AExp, BExp>::num == 0)>::type>
	{
		using type = typename multiply_impl<dimensions<A_Rest...>, dimensions<B_Rest...> >::type;
	};

	/*
		Multiply and divide templates for general use
	*/
	template<class A, class B>
	using multiply_t = typename multiply_impl<A, B>::type;

	template<class N, class D>
	using divide_t = multiply_t<N, inverse_t<D>>;
	
	
	/*
		Create a well-formed dimension template by sorting parameters.
			This is implemented using multiply.
	*/
	template<class... Dimensions>
	struct make_dimension_impl {};
	
	template<>
	struct make_dimension_impl<> {using type = dimensions<>;};
	
	template<class Base1, class Exp1, class... Dimensions>
	struct make_dimension_impl<Base1, Exp1, Dimensions...>
	{
		using type = multiply_t<dimensions<Base1, Exp1>, make_dimension_impl<Dimensions...>>;
	};
	
	template<class... Dimensions>
	using make_dimension = typename make_dimension_impl<Dimensions...>::type;
}
