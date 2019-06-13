// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file CxxUtils/bitmask.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2018
 * @brief Helpers for treating a class enum as a bitmask.
 *
 * C++11 class enums are very nice from a type-safety viewpoint.
 * However, they are a bit awkward if you want to use them to represent
 * a bitmask.  For example:
 *@code
   enum class Mask { Bit0 = 1, Bit1 = 2, Bit2 = 4 };
   Mask m = Mask::Bit1 | Mask::Bit2;
 @endcode
 * doesn't compile because the enumerators are not convertible to integers.
 * One can cast back and forth between the enum and integers, but that's
 * awkward to do everywhere.
 *
 * This header defines overloads for the bitwise operators that work with
 * a class enum.  To enable these overloads, you need to add the @c ATH_BITMASK
 * macro at the end of your enum declaration:
 *
 *@code
   enum class Mask { Bit0 = 1, Bit1 = 2, Bit2 = 4,
                     ATH_BITMASK };
 @endcode
 *
 * After that, the usual bitwise operations (&, |, ^, &=, |=, ^=, ~) should
 * work as expected.  There are also a few convenience functions defined
 * in the @c CxxUtils namespace: @c set, @c reset, and @c test.
 *
 * This approach was suggested by these postings:
 *
 * <http://blog.bitwigglers.org/using-enum-classes-as-type-safe-bitmasks>
 * <https://www.justsoftwaresolutions.co.uk/cplusplus/using-enum-classes-as-bitfields.html>
 *
 * except that we rely on injecting a known enumerator into the type rather
 * than using a separate traits class.  This way works better if the enumeration
 * is defined in a nested scope.
 */


#include <type_traits>


#ifndef CXXUTILS_BITMASK_H
#define CXXUTILS_BITMASK_H


/**
 * @brief Mark that a class enum should be treated as a bitmask.
 *        Put this at the end of the enumeration, after a comma, like this:
 *@code
   enum class Mask { Bit0 = 1, Bit1 = 2, Bit2 = 4,
                     ATH_BITMASK };
 @endcode
 */
#define ATH_BITMASK IS_ATH_BITMASK=1


/*
 * Define bitwise operators for class enums.
 * These all cast the operands to the underlying type, and then cast
 * the result back again to the enumeration.
 *
 * These functions are enabled only for enumerators that have used the
 * @c ATH_BITMASK macro.
 */


/// operator~
template <class E>
inline
typename std::enable_if<(E::IS_ATH_BITMASK,1), E>::type
operator~ (E lhs)
{
  typedef typename std::underlying_type<E>::type underlying;
  return static_cast<E> (~static_cast<underlying>(lhs));
}


/// operator&
template <class E>
inline
typename std::enable_if<(E::IS_ATH_BITMASK,1), E>::type
operator& (E lhs, E rhs)
{
  typedef typename std::underlying_type<E>::type underlying;
  return static_cast<E> (static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
}


/// operator|
template <class E>
inline
typename std::enable_if<(E::IS_ATH_BITMASK,1), E>::type
operator| (E lhs, E rhs)
{
  typedef typename std::underlying_type<E>::type underlying;
  return static_cast<E> (static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
}


/// operator^
template <class E>
inline
typename std::enable_if<(E::IS_ATH_BITMASK,1), E>::type
operator^ (E lhs, E rhs)
{
  typedef typename std::underlying_type<E>::type underlying;
  return static_cast<E> (static_cast<underlying>(lhs) ^ static_cast<underlying>(rhs));
}


/// operator&=
template <class E>
inline
typename std::enable_if<(E::IS_ATH_BITMASK,1), E&>::type
operator&= (E& lhs, E rhs)
{
  typedef typename std::underlying_type<E>::type underlying;
  lhs = static_cast<E> (static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
  return lhs;
}


/// operator|=
template <class E>
inline
typename std::enable_if<(E::IS_ATH_BITMASK,1), E&>::type
operator|= (E& lhs, E rhs)
{
  typedef typename std::underlying_type<E>::type underlying;
  lhs = static_cast<E> (static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
  return lhs;
}


/// operator^=
template <class E>
inline
typename std::enable_if<(E::IS_ATH_BITMASK,1), E&>::type
operator^= (E& lhs, E rhs)
{
  typedef typename std::underlying_type<E>::type underlying;
  lhs = static_cast<E> (static_cast<underlying>(lhs) ^ static_cast<underlying>(rhs));
  return lhs;
}


namespace CxxUtils {


/**
 * @brief Convenience function to set bits in a class enum bitmask.
 * @param lhs Target bitmask.
 * @param rhs Bits to set.
 *
 * Same as lhs |= rhs.  
 * This function is enabled only for enumerators that have used the
 * @c ATH_BITMASK macro.
 */
template <class E>
inline
typename std::enable_if<(E::IS_ATH_BITMASK,1), E&>::type
set (E& lhs, E rhs)
{
  lhs |= rhs;
  return lhs;
}


/**
 * @brief Convenience function to clear bits in a class enum bitmask.
 * @param lhs Target bitmask.
 * @param rhs Bits to clear.
 *
 * Same as lhs &= ~rhs.  
 * This function is enabled only for enumerators that have used the
 * @c ATH_BITMASK macro.
 */
template <class E>
inline
typename std::enable_if<(E::IS_ATH_BITMASK,1), E&>::type
reset (E& lhs, E rhs)
{
  lhs &= ~rhs;
  return lhs;
}


/**
 * @brief Convenience function to test bits in a class enum bitmask.
 * @param lhs Target bitmask.
 * @param rhs Bits to test.
 *
 * Same as (lhs & rhs) != 0.
 * This function is enabled only for enumerators that have used the
 * @c ATH_BITMASK macro.
 */
template <class E>
inline
typename std::enable_if<(E::IS_ATH_BITMASK,1), bool>::type
test (E lhs, E rhs)
{
  typedef typename std::underlying_type<E>::type underlying;
  return static_cast<underlying> (lhs & rhs) != 0;
}


} // namespace CxxUtils


#endif // not CXXUTILS_BITMASK_H
