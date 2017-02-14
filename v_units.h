#pragma once

#include <type_traits>
#include <ratio>

namespace v_dimensions
{
	// SI units
	struct length      { static const int DIM_ID = 100; };
	struct mass        { static const int DIM_ID = 200; };
	struct time        { static const int DIM_ID = 300; };
	struct charge      { static const int DIM_ID = 400; };
	struct temperature { static const int DIM_ID = 500; };
	struct substance   { static const int DIM_ID = 600; };
	struct luminance   { static const int DIM_ID = 700; };

	// Useful nonstandard units
	struct angle       { static const int DIM_ID = 10100; };
	struct bits        { static const int DIM_ID = 10200; };

	// User-defined types could be whatever the use-case demands.
	//   Multiply these dimensions for 1 unit of captain_planet.
	struct fire        { static const int DIM_ID = 1101; };
	struct water       { static const int DIM_ID = 1102; };
	struct earth       { static const int DIM_ID = 1103; };
	struct wind        { static const int DIM_ID = 1104; };
	struct heart       { static const int DIM_ID = 1105; };
}

namespace v_units
{
	// This template represents a base dimension and its exponent as part of a unit's dimensionality.
	template<class BaseDimension, class Exponent>
	struct dimension
	{
		using base     = BaseDimension;
		using exponent = Exponent;

		using inverse  = dimension < BaseDimension, std::ratio_multiply<Exponent, std::ratio<-1> > >;

		static const int  DIM_ID = BaseDimension::DIM_ID;
		static const bool is_whole = (Exponent::den == 1);
	};

	
	namespace checks
	{
		/*
			Template to ensure a dimension list adheres to the required DIM_ID ordering.
		*/
		template<class... Dimensions>
		struct dim_order { static const bool ok = true; };

		template<class Dimension1, class Dimension2, class... Dimensions>
		struct dim_order<Dimension1, Dimension2, Dimensions...>
		{
			static const bool ok =
				(Dimension1::DIM_ID < Dimension2::DIM_ID)
				&& dim_order<Dimension2, Dimensions...>::ok;
		};

		/*
			Template to ensure a dimension list contains no zero exponents.
		*/
		template<class... Dimensions>
		struct exponents { static const bool nonzero = true; };

		template<class Dimension1, class... Dimensions>
		struct exponents<Dimension1, Dimensions...>
		{
			static const bool nonzero = (Dimension1::exponent::num != 0) && exponents<Dimensions...>::nonzero;
		};
	}
	

	/*
		Variadic template for a unit type.
			The 
	*/
	template<class... Dimensions>
	struct dimensions
	{
		/*
			These two type checks ensure that only one valid dimensions template exists for any given dimensionality.
				Dimensions must be defined in order, and must have non-zero exponents.
		*/
		static_assert(checks::dim_order<Dimensions...>::ok,
			"Unit dimensions must be defined in order with respect to DIM_ID.");

		static_assert(checks::exponents<Dimensions...>::nonzero,
			"Unit dimensions must have non-zero exponents.");
	};


	template<class> struct inverse_impl {};

	template<class... Dimensions>
	struct inverse_impl<dimensions<Dimensions...> >
	{
		using type = dimensions<typename Dimensions::inverse...>;

		//using type = dimensions<
		//	dimension < typename Dimensions::base, std::ratio_multiply<typename Dimensions::exponent, std::ratio<-1> > >...>;
	};

	template<class U> using inverse_t = typename inverse_impl<U>::type;


	// Used in multiply_impl
	template<class Dimension, class Dimensions> struct dimension_add_impl {};

	template<class Dimension, class... Dimensions>
	struct dimension_add_impl<Dimension, dimensions<Dimensions...> >
	{
		using prepend = dimensions<Dimension, Dimensions...>;
		//using append  = dimensions<Dimensions..., FirstDimension>;
	};

	template<class Dimension, class Dimensions>
	using dimension_prepend = typename dimension_add_impl<Dimension, Dimensions>::prepend;

	//template<class Dimension, class Dimensions>
	//using dimension_append = dimension_add_impl<Dimension, Dimensions>::append;


	template<class U, class Power> struct pow_impl {};

	template<class... Dimensions, class Power>
	struct pow_impl<dimensions<Dimensions...>, Power>
	{
		using type = dimensions<
			dimension<typename Dimensions::base, std::ratio_multiply<typename Dimensions::exponent, Power>>... >;
	};

	template<class U, class Power>
	using pow_t = typename pow_impl<U, Power>::type;


	/*
		Dimensions multiplication template.  Breaks down into various cases...
	*/
	template<class A, class B, class Enable = void> struct multiply_impl {};

	// A: B is dimensionless; return A.
	template<class... A_Rest>
	struct multiply_impl<dimensions<A_Rest...>, dimensions<>, void> { using type = dimensions<A_Rest...>; };

	// B: A is dimensionless; return B (which is not).
	template<class B1, class... B_Rest>
	struct multiply_impl<dimensions<>, dimensions<B1, B_Rest...>, void> { using type = dimensions<B1, B_Rest...>; };

	// C: A's first dimension precedes B's
	template<class A1, class B1, class... A_Rest, class... B_Rest>
	struct multiply_impl<dimensions<A1, A_Rest...>, dimensions<B1, B_Rest...>,
		typename std::enable_if<(A1::DIM_ID < B1::DIM_ID)>::type>
	{
		using type = dimension_prepend<A1,
			typename multiply_impl<dimensions<A_Rest...>, dimensions<B1, B_Rest...> >::type>;
	};

	// D: B's first dimension precedes A's
	template<class A1, class B1, class... A_Rest, class... B_Rest>
	struct multiply_impl<dimensions<A1, A_Rest...>, dimensions<B1, B_Rest...>,
		typename std::enable_if<(B1::DIM_ID < A1::DIM_ID)>::type>
	{
		using type = dimension_prepend<B1,
			typename multiply_impl<dimensions<A1, A_Rest...>, dimensions<B_Rest...> >::type>;
	};

	// E: A's first dimension is compatible with B's and they don't cancel out
	template<class A1, class B1, class... A_Rest, class... B_Rest>
	struct multiply_impl<dimensions<A1, A_Rest...>, dimensions<B1, B_Rest...>,
		typename std::enable_if<(B1::DIM_ID == A1::DIM_ID) &&
			(std::ratio_add<typename A1::exponent, typename B1::exponent>::num != 0)>::type>
	{
		using type = dimension_prepend<
			dimension<typename A1::base, std::ratio_add<typename A1::exponent, typename B1::exponent>>,
			typename multiply_impl<dimensions<A_Rest...>, dimensions<B_Rest...> >::type>;
	};

	// E: A's first dimension is compatible with B's and they cancel out
	template<class A1, class B1, class... A_Rest, class... B_Rest>
	struct multiply_impl<dimensions<A1, A_Rest...>, dimensions<B1, B_Rest...>,
		typename std::enable_if<(B1::DIM_ID == A1::DIM_ID) &&
			(std::ratio_add<typename A1::exponent, typename B1::exponent>::num == 0)>::type>
	{
		using type = typename multiply_impl<dimensions<A_Rest...>, dimensions<B_Rest...> >::type;
	};

	template<class A, class B>
	using multiply_t = typename multiply_impl<A, B>::type;

	template<class N, class D>
	using divide_t = multiply_t<N, inverse_t<D>>;
}
