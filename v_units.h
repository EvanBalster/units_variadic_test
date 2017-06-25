#pragma once

#include <type_traits>
#include <ratio>

#include "v_dimensions.h"

// ========================================================== //
#error "This header is currently unsupported --- see vp_units.h"
// ========================================================== //

#if 0

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
			Parameters should be a list of dimensions, EG:
			dimensions<dimension<length, ratio<1, 1>>, dimension<time, ratio<-1, 1>>>
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


	namespace util
	{
		/*
			Compose dimensions by adding a base and power to the front or back of the list.
				This does not automatically place items in order!!
				It is used in other templates below.
		*/
		template<class Dim1, class Dims> struct dim_push_front {};
		template<class DimN, class Dims> struct dim_push_back {};

		template<class Dim1, class... Dims>
		struct dim_push_front<Dim1, dimensions<Dims...> > { using type = dimensions<Dim1, Dims...>; };
		template<class DimN, class... Dims>
		struct dim_push_back <DimN, dimensions<Dims...> > { using type = dimensions<Dims..., DimN>; };

		template<class Dim1, class Dims>
		using dim_push_front_t = typename dim_push_front<Dim1, Dims>::type;
		template<class DimN, class Dims>
		using dim_push_back_t  = typename dim_push_back<DimN, Dims>::type;
	}


	/*
		Raise dimensions to a rational power (always a std::ratio)
	*/
	template<class Dimensions, class Power> struct pow_impl {};

	template<class... Dimensions, class Power>
	struct pow_impl<dimensions<Dimensions...>, Power>
	{
		using type = dimensions<
			dimension<typename Dimensions::base, std::ratio_multiply<typename Dimensions::exponent, Power>>... >;
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
	template<class B1, class... B_Rest>
	struct multiply_impl<dimensions<>, dimensions<B1, B_Rest...>, void> { using type = dimensions<B1, B_Rest...>; };

	// C: A's first dimension precedes B's
	template<class A1, class B1, class... A_Rest, class... B_Rest>
	struct multiply_impl<dimensions<A1, A_Rest...>, dimensions<B1, B_Rest...>,
		typename std::enable_if<(A1::DIM_ID < B1::DIM_ID)>::type>
	{
		using type = util::dim_push_front_t<A1,
			typename multiply_impl<dimensions<A_Rest...>, dimensions<B1, B_Rest...> >::type>;
	};

	// D: B's first dimension precedes A's
	template<class A1, class B1, class... A_Rest, class... B_Rest>
	struct multiply_impl<dimensions<A1, A_Rest...>, dimensions<B1, B_Rest...>,
		typename std::enable_if<(A1::DIM_ID > B1::DIM_ID)>::type>
	{
		using type = util::dim_push_front_t<B1,
			typename multiply_impl<dimensions<A1, A_Rest...>, dimensions<B_Rest...> >::type>;
	};

	// E: A's first dimension is compatible with B's and they don't cancel out
	template<class A1, class B1, class... A_Rest, class... B_Rest>
	struct multiply_impl<dimensions<A1, A_Rest...>, dimensions<B1, B_Rest...>,
		typename std::enable_if<(A1::DIM_ID == B1::DIM_ID) &&
			(std::ratio_add<typename A1::exponent, typename B1::exponent>::num != 0)>::type>
	{
		using type = util::dim_push_front_t<
			dimension<typename A1::base, std::ratio_add<typename A1::exponent, typename B1::exponent>>,
			typename multiply_impl<dimensions<A_Rest...>, dimensions<B_Rest...> >::type>;
	};

	// E: A's first dimension is compatible with B's and they cancel out
	template<class A1, class B1, class... A_Rest, class... B_Rest>
	struct multiply_impl<dimensions<A1, A_Rest...>, dimensions<B1, B_Rest...>,
		typename std::enable_if<(A1::DIM_ID == B1::DIM_ID) &&
			(std::ratio_add<typename A1::exponent, typename B1::exponent>::num == 0)>::type>
	{
		using type = typename multiply_impl<dimensions<A_Rest...>, dimensions<B_Rest...> >::type;
	};

	template<class A, class B>
	using multiply_t = typename multiply_impl<A, B>::type;

	template<class N, class D>
	using divide_t = multiply_t<N, inverse_t<D>>;
}

#endif
