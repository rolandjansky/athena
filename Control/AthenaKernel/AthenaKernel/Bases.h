// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthenaKernel/Bases.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2020
 * @brief Traits class for representing derivation.
 *
 * For a class @c T, its base classes are available via the traits
 * class defined here, as @c Bases<T>::bases.  This will be an instantiation
 * of @c BaseList, defined below, which provides a @c foreach_ function
 * to iterate over the base classes.
 * [<CENSORED> Qt defines `foreach' as a macro, so we can't use that name!]
 *
 * For this to work, the @c Bases class must be specialized for each
 * class @c T.  This is usually done using the @c SG_BASES macro
 * defined in BaseInfo.h.
 */


#ifndef ATHENAKERNEL_BASES_H
#define ATHENAKERNEL_BASES_H


#include <type_traits>


namespace SG {


/**
 * @brief Wrapper to indicate virtual derivation.
 *        @a Virtual<T> will mean that derivation from @a T is virtual.
 */
template <class T> struct Virtual {};


// Helper metafunction to get base class types.
// Generic case.
template <class T>
struct BaseType
{
  typedef T type;
  typedef std::false_type is_virtual;
};


// Helper metafunction to get base class types.
// Virtual derivation case.
template <class T>
struct BaseType<Virtual<T> >
{
  typedef T type;
  typedef std::true_type is_virtual;
};


/**
 * @brief Marker to indicate a nonexistent base class.
 */
struct NoBase {};


/**
 * @brief Represent a list of base classes.
 *
 * This generic definition is used only for the case of an empty
 * base class list; all others use the specialization below.
 */
template <class... T>
struct BaseList
{
public:
  // First base in the list.
  using Base1 = NoBase;


  /**
   * @brief Iterate over base classes.
   * @param f Object to call for each base class.
   * @param is_virtual True if we got to this base via virtual derivation.
   *
   * This is the specialization for the case of no base classes.
   * See the version below for full documentation.
   */
  template <class CALLABLE>
  static
  auto foreach_ (CALLABLE f, bool /*is_virtual*/ = false)
  {
    // Need to find the return type.
    using return_t = decltype (f (static_cast<void*> (nullptr), false));
    return return_t (false);
  }
};


/**
 * @brief Traits class to hold derivation information.
 *
 * The @a bases typedef gives a @c BaseList which holds the list
 * of base classes.  The entries in this list may be of the
 * form @a Virtual<T> to indicate virtual derivation.  This class should
 * be specialized for every class @a T for which we want to know
 * inheritance information.
 */
template <class T>
struct Bases
{
public:
  using bases = BaseList<>;
};


/**
 * @brief Represent a non-empty list of base classes.
 */
template <class BASE, class... REST>
struct BaseList<BASE, REST...>
{
public:
  // First base in the list.
  using Base1 = BASE;


  /**
   * @brief Iterate over base classes.
   * @param f Object to call for each base class.
   * @param is_virtual True if we got to this base via virtual derivation.
   *
   * @c f should be a callable object.  It gets called for each base class
   * like this:
   *
   *@code
   *    auto ret = f (static_cast<T*> (nullptr), is_virtual);
   @endcode
   *
   * Doing it like this allows using a generic lambda.
   *
   * When called from @c Bases<D>::bases, note that the class @c D itself
   * is not included in the iteration.  You should also not assume
   * anything about the order in which the bases are visited.
   *
   * The return type is user-defined, except that it must be convertable
   * to and from a bool.  If @f returns a value that converts to a
   * @c true boolean value, then the iteration stops at that point,
   * and that value is returned.  It must also be possible to initialize
   * the return type from a @c false boolean value.  This allows, for example,
   * a pointer to be used for the return type, with the iteration stopping
   * once a non-null pointer is returned.
   *
   * It must also be possible to instantiate @c f with a @c void* argument.
   * (This will not actually be called, but it must be possible to
   * instantiate it in order to find the return type.)
   *
   * The iteration generated here is fixed at compile time.
   * It does NOT take into account any information added by @c SG_ADD_BASE.
   *
   * Example:
   *
   *@code
   *  // Does T derive from the class described by ti?
   *  template <class T>
   *  bool derivesFrom (const std::type_info& ti)
   *  {
   *    // Check if T itself is ti.
   *    if (typeid(T) == ti) return true;
   *
   *    auto search = [&] (auto* p, bool is_virtual [[maybe_unused]])
   *    {
   *       using base_t = std::remove_pointer_t<decltype(p)>;
   *       if (typeid(base_t) == ti) return true;
   *       return false;
   *    };
   *    return SG::Bases<T>::bases::foreach_ (search);
   *  }
   @endcode
   */
  template <class CALLABLE>
  static
  auto foreach_ (CALLABLE f, bool is_virtual = false)
  {
    // Unwrap a SG::Virtual if needed.
    using base_t = typename BaseType<BASE>::type;
    const bool base_is_virtual = is_virtual || BaseType<BASE>::is_virtual::value;

    // Call the function on our first base.
    auto ret1 = f (static_cast<base_t*> (nullptr), base_is_virtual);
    if (static_cast<bool> (ret1)) {
      return ret1;
    }

    // Call it on bases of our first base.
    auto ret2 = Bases<base_t>::bases::foreach_ (f, base_is_virtual);
    if (static_cast<bool> (ret2)) {
      return ret2;
    }

    // Call it on our remaining bases.
    if constexpr (sizeof... (REST) > 0) {
      auto ret3 = BaseList<REST...>::foreach_ (f, is_virtual);
      if (static_cast<bool> (ret3)) {
        return ret3;
      }
    }

    // Got to the end; return false;
    using return_t = decltype (ret1);
    return return_t (false);
  }
};


} // namespace SG


#endif // not ATHENAKERNEL_BASES_H
