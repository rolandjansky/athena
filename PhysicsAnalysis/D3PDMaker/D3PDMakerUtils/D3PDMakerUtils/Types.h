// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/Types.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2012
 * @brief A simple tuple of multiple types.
 *
 * This is used when declaring tools that can take one of a set of types.
 * For example,
 *
 *@code
 *   BlockFillerTool<Types<Obj1, Obj2> >
 @endcode
 *
 * declares a block filler that can take as input either @c Obj1 or @c Obj2.
 * (Note: the choice is made during configuration, not dynamically.)
 */


#ifndef D3PDMAKERUTILS_TYPES_H
#define D3PDMAKERUTILS_TYPES_H


#include "boost/preprocessor/repetition/enum_shifted_binary_params.hpp"
#include "boost/preprocessor/repetition/enum_params.hpp"
#include "boost/preprocessor/repetition/repeat.hpp"
#include "boost/preprocessor/facilities/intercept.hpp"
#include "boost/preprocessor/iteration/iterate.hpp"
#include <typeinfo>
#include <cstdlib>


#ifndef D3PD_MAX_TYPE_CHOICES
# define D3PD_MAX_TYPE_CHOICES 5
#endif


namespace D3PD {


/**
 * @brief Placeholder for empty type.
 */
class NoType {};


/**
 * @brief A simple tuple of multiple types.
 *
 * This can be used as the type argument of @c BlockFillerTool and
 * related templates in order to define a tool that can take one of
 * a set of types as input.  Eg,
 *
 *@code
 *   D3PD::Types<Obj1, Obj2>
 @endcode
 *
 * The template specification expands to
 *
 *@code
 *   class T0, class T1=NoType, ... , class T<D3PD_MAX_TYPE_CHOICES-1>=NoType
 @endcode
 */
template <class T0,
          BOOST_PP_ENUM_SHIFTED_BINARY_PARAMS(D3PD_MAX_TYPE_CHOICES,
                                              class T,
                                              =NoType BOOST_PP_INTERCEPT)>
class Types
{
};



/**
 * @brief Select one type out of the tuple.
 *
 * If `T` is `Types<T0, ...>`, then `SelectType<T, N>::type` will be `TN`;
 * otherwise it will be `T`.
 */
template <class T, int N>
struct SelectType
{
  typedef T type;
};


/**
 * @brief Return one @c type_info from a tuple.
 *
 * If `T` is `Types<T0, ...>`, then ` multiTypeInfo (T*, which)` will
 * return `typeid(Twhich)`.  Otherwise, it will return `typeid(T)`.
 */
template <BOOST_PP_ENUM_PARAMS(D3PD_MAX_TYPE_CHOICES, class T)>
const std::type_info&
multiTypeInfo (Types<BOOST_PP_ENUM_PARAMS(D3PD_MAX_TYPE_CHOICES, T)>*,
               size_t which)
{
#define BODY(Z,N,DATA) if (which==N) return typeid(T##N);
  BOOST_PP_REPEAT(D3PD_MAX_TYPE_CHOICES, BODY, dum)
#undef BODY
  std::abort();
}


template <class T>
const std::type_info& multiTypeInfo (T*, size_t /*which*/)
{
  return typeid (T);
}


} // namespace D3PD


#define BOOST_PP_ITERATION_LIMITS (0, D3PD_MAX_TYPE_CHOICES-1)
#define BOOST_PP_FILENAME_1  "D3PDMakerUtils/Types.icc"
#include BOOST_PP_ITERATE()



#endif // not D3PDMAKERUTILS_TYPES_H
