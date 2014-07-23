// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: fast_push_back.h,v 1.2 2009-03-20 20:44:22 ssnyder Exp $
/**
 * @file  CxxUtils/fast_push_back.h
 * @author scott snyder
 * @date Jan, 2007
 * @brief Helper to speed up inserting into an STL vector.
 *
 * Here's the issue.  Suppose one has a class like this:
 *
 *@code
 *  class C
 *  {
 *    C (int x, int y);
 *    ~C();
 *  };
 @endcode
 *
 * and you want to insert elements into a @c std::vector<C>.
 * You might do something like this:
 *
 *@code
 *  v.push_back (C (0, 1));
 @endcode
 *
 * Note what this does though:
 *
 *  - We first make a temporary instance of @c C using @c C's constructor,
 *    to pass to @c push_back.
 *  - @c push_back will somehow allocate the memory within the vector
 *    where the new element will reside.
 *  - @c push_back will copy-construct the new element, using
 *    a placement @c new.
 *  - Finally, we need to destroy the temporary @c C we created.
 *
 * Note that one constructor/destructor here is superfluous.
 * In some cases, this can have a significant effect on performance
 * (this was observed in filling calorimeter towers with cells).
 * So, we'd like to be able to eliminate this superfluous ctor/dtor pair.
 *
 * Suppose that in addition to @c push_back, @c std::vector had a method
 * @c uninitialized_push_back.  This would do the memory allocation
 * (the second bullet above), but would then return the pointer rather
 * than constructing the element itself.  The caller is then free to
 * construct the element using whatever constructor is convenient.
 * For example,
 *
 *@code
 *  new (v.uninitialized_push_back()) C (0, 1);
 @endcode
 *
 * would call only the single @c constructor, with no destructors.
 *
 * Of course, @c uninitialized_push_back doesn't exist; and further,
 * there doesn't seem to be any way to add that functionality
 * to an existing @c std::vector in a portable way.
 *
 * However, for some compilers, we can define such an operation
 * if we're permitted to use knowledge of the internals of
 * @c std::vector.  Thus, this file selects for compilers/versions
 * that we know, and defines a @c uninitialized_push_back operation
 * if possible.
 *
 * The entry points here are the functions @c fast_push_back.
 * These take as arguments first the container, followed by the
 * arguments to use for the element's constructor.  If an
 * @c uninitialized_push_back is defined, then @c fast_push_back
 * will use it.  Otherwise, a slower, portable implementation will be used.
 * So, using this header, the example above would be written:
 *
 *@code
 *  SG::fast_push_back (v, 0, 1);
 @endcode
 *
 * We support up to four arguments to the element constructor.
 *
 * Note that there's standard solution for this in the new C++0x standard.
 * In that version, @c vector supports an emplacement inserter,
 * where instead of
 *
 *@code
 *  v.push_back (C (0, 1));
 @endcode
 *
 * one can do 
 *
 *@code
 *  v.emplace_back (0, 1);
 @endcode
 *
 * to avoid the extra constructors.  We'll use this form if we know
 * that it's available in the compiler version being used.
 */


#ifndef CXXUTILS_FAST_PUSH_BACK_H
#define CXXUTILS_FAST_PUSH_BACK_H

#include <vector>


namespace CxxUtils {


// If __GXX_EXPERIMENTAL_CXX0X__ is on, then we have emplacement push_back.
#ifdef __GXX_EXPERIMENTAL_CXX0X__
# define HAVE_SG_EMPLACEMENT
#endif


// Implementation of @c uninitialized_push_back for gcc.
#if defined(__GNUC__) && !defined(HAVE_SG_EMPLACEMENT)

// We have it.
// (The conditional is for testing purposes.)
#ifndef HAVE_SG_UNINITIALIZED_PUSH_BACK
#define HAVE_SG_UNINITIALIZED_PUSH_BACK 1
#endif


// Helper class used to get access to std::vector's
// protected _M_finish member.
template <typename V>
struct vhelper
  : public V
{
  typename V::pointer uninitialized_push_back()
  {
    typename V::size_type sz = this->size();
    if (sz == this->capacity()) {
      // ??? Ignoring possible overflow here...
      this->reserve (sz ? sz*2 : 1);
    }

    return this->_M_impl._M_finish++;
  }
};


// This is uninitialized_push_back for gcc.
template <typename V>
inline
typename V::pointer uninitialized_push_back (V& v)
{
  return (reinterpret_cast<vhelper<V>&> (v)).uninitialized_push_back();
}


#endif // __GNUC__


/**
 * @brief Construct and push an element onto a vector.
 * @param v The vector onto which to push the element.
 *
 * The element will be default-constructed.
 * A superfluous ctor/dtor pair will be avoided, if possible.
 */
template <typename V>
inline
void fast_push_back (V& v)
{
#ifdef HAVE_SG_EMPLACEMENT
  v.emplace_back ();
#else
# ifdef HAVE_SG_UNINITIALIZED_PUSH_BACK
  new (uninitialized_push_back (v)) typename V::value_type;
# else
  v.push_back (typename V::value_type ());
# endif
#endif
}


/**
 * @brief Construct and push an element onto a vector.
 * @param v The vector onto which to push the element.
 * @param x1 The first argument to the element's constructor.
 *
 * A superfluous ctor/dtor pair will be avoided, if possible.
 */
template <typename V, typename T1>
inline
void fast_push_back (V& v, const T1& x1)
{
#ifdef HAVE_SG_EMPLACEMENT
  v.emplace_back (x1);
#else
# ifdef HAVE_SG_UNINITIALIZED_PUSH_BACK
  new (uninitialized_push_back (v)) typename V::value_type (x1);
# else
  v.push_back (typename V::value_type (x1));
# endif
#endif
}


/**
 * @brief Construct and push an element onto a vector.
 * @param v The vector onto which to push the element.
 * @param x1 The first argument to the element's constructor.
 *
 * A superfluous ctor/dtor pair will be avoided, if possible.
 */
template <typename V, typename T1>
inline
void fast_push_back (V& v, T1& x1)
{
#ifdef HAVE_SG_EMPLACEMENT
  v.emplace_back (x1);
#else
# ifdef HAVE_SG_UNINITIALIZED_PUSH_BACK
  new (uninitialized_push_back (v)) typename V::value_type (x1);
# else
  v.push_back (typename V::value_type (x1));
# endif
#endif
}


/**
 * @brief Construct and push an element onto a vector.
 * @param v The vector onto which to push the element.
 * @param x1 The first argument to the element's constructor.
 * @param x2 The second argument to the element's constructor.
 *
 * A superfluous ctor/dtor pair will be avoided, if possible.
 */
template <typename V, typename T1, typename T2>
inline
void fast_push_back (V& v, const T1& x1, const T2& x2)
{
#ifdef HAVE_SG_EMPLACEMENT
  v.emplace_back (x1, x2);
#else
# ifdef HAVE_SG_UNINITIALIZED_PUSH_BACK
  new (uninitialized_push_back (v)) typename V::value_type (x1, x2);
# else
  v.push_back (typename V::value_type (x1, x2));
# endif
#endif
}


/**
 * @brief Construct and push an element onto a vector.
 * @param v The vector onto which to push the element.
 * @param x1 The first argument to the element's constructor.
 * @param x2 The second argument to the element's constructor.
 *
 * A superfluous ctor/dtor pair will be avoided, if possible.
 */
template <typename V, typename T1, typename T2>
inline
void fast_push_back (V& v, T1& x1, const T2& x2)
{
#ifdef HAVE_SG_EMPLACEMENT
  v.emplace_back (x1, x2);
#else
# ifdef HAVE_SG_UNINITIALIZED_PUSH_BACK
  new (uninitialized_push_back (v)) typename V::value_type (x1, x2);
# else
  v.push_back (typename V::value_type (x1, x2));
# endif
#endif
}


/**
 * @brief Construct and push an element onto a vector.
 * @param v The vector onto which to push the element.
 * @param x1 The first argument to the element's constructor.
 * @param x2 The second argument to the element's constructor.
 * @param x3 The third argument to the element's constructor.
 *
 * A superfluous ctor/dtor pair will be avoided, if possible.
 */
template <typename V, typename T1, typename T2, typename T3>
inline
void fast_push_back (V& v, const T1& x1, const T2& x2, const T3& x3)
{
#ifdef HAVE_SG_EMPLACEMENT
  v.emplace_back (x1, x2, x3);
#else
# ifdef HAVE_SG_UNINITIALIZED_PUSH_BACK
  new (uninitialized_push_back (v)) typename V::value_type (x1, x2, x3);
# else
  v.push_back (typename V::value_type (x1, x2, x3));
# endif
#endif
}


/**
 * @brief Construct and push an element onto a vector.
 * @param v The vector onto which to push the element.
 * @param x1 The first argument to the element's constructor.
 * @param x2 The second argument to the element's constructor.
 * @param x3 The third argument to the element's constructor.
 *
 * A superfluous ctor/dtor pair will be avoided, if possible.
 */
template <typename V, typename T1, typename T2, typename T3>
inline
void fast_push_back (V& v, T1& x1, const T2& x2, const T3& x3)
{
#ifdef HAVE_SG_EMPLACEMENT
  v.emplace_back (x1, x2, x3);
#else
# ifdef HAVE_SG_UNINITIALIZED_PUSH_BACK
  new (uninitialized_push_back (v)) typename V::value_type (x1, x2, x3);
# else
  v.push_back (typename V::value_type (x1, x2, x3));
# endif
#endif
}


/**
 * @brief Construct and push an element onto a vector.
 * @param v The vector onto which to push the element.
 * @param x1 The first argument to the element's constructor.
 * @param x2 The second argument to the element's constructor.
 * @param x3 The third argument to the element's constructor.
 * @param x4 The fourth argument to the element's constructor.
 *
 * A superfluous ctor/dtor pair will be avoided, if possible.
 */
template <typename V, typename T1, typename T2, typename T3, typename T4>
inline
void fast_push_back (V& v,
                     const T1& x1, const T2& x2, const T3& x3, const T4& x4)
{
#ifdef HAVE_SG_EMPLACEMENT
  v.emplace_back (x1, x2, x3, x4);
#else
# ifdef HAVE_SG_UNINITIALIZED_PUSH_BACK
  new (uninitialized_push_back (v)) typename V::value_type (x1, x2, x3, x4);
# else
  v.push_back (typename V::value_type (x1, x2, x3, x4));
# endif
#endif
}


/**
 * @brief Construct and push an element onto a vector.
 * @param v The vector onto which to push the element.
 * @param x1 The first argument to the element's constructor.
 * @param x2 The second argument to the element's constructor.
 * @param x3 The third argument to the element's constructor.
 * @param x4 The fourth argument to the element's constructor.
 *
 * A superfluous ctor/dtor pair will be avoided, if possible.
 */
template <typename V, typename T1, typename T2, typename T3, typename T4>
inline
void fast_push_back (V& v,
                     T1& x1, const T2& x2, const T3& x3, const T4& x4)
{
#ifdef HAVE_SG_EMPLACEMENT
  v.emplace_back (x1, x2, x3, x4);
#else
# ifdef HAVE_SG_UNINITIALIZED_PUSH_BACK
  new (uninitialized_push_back (v)) typename V::value_type (x1, x2, x3, x4);
# else
  v.push_back (typename V::value_type (x1, x2, x3, x4));
# endif
#endif
}


} // namespace CxxUtils


#endif // not CXXUTILS_FAST_PUSH_BACK_H
