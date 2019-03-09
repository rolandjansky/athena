// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file CxxUtils/CachedPointer.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2017, from earlier code in AthLinks.
 * @brief Cached pointer with atomic update.
 */


#ifndef CXXUTILS_CACHEDPOINTER_H
#define CXXUTILS_CACHEDPOINTER_H


#include <atomic>
#include <cassert>


#if ATOMIC_POINTER_LOCK_FREE != 2
# error Code assumes lock-free atomic pointers; see comments below.
#endif


namespace CxxUtils {

/**
 * @brief Cached pointer with atomic update.
 *
 * The pointer may be set atomically through a const method.
 * This can happen in multiple threads, but the value set should be the same
 * in all threads.  (The value can also be set using the non-const
 * store() method; however, this is not atomic and should not be done while
 * other threads may be accessing the value.)
 *
 * In principle, we need this to be atomic, since this cached value
 * can be changed from a const method.  However, because any access should
 * always fill this with the same value, we don't care about ordering,
 * only that the access itself is atomic in the sense of reading/writing
 * the entire object at once.  Hence, we can use a relaxed memory ordering
 * for accesses to the value.
 * A further wrinkle is that the use case in AthLinks requires that we return
 * a pointer to the value here;
 * there doesn't seem to be a way around that without either changing
 * how links are used or storing additional data here.  It should, however,
 * be ok from a data race point of view because we ensure that we don't
 * return a pointer until after it's set, and also that after it's set
 * it isn't written again (from a const method).
 * There is however, no portable way of getting a pointer to the
 * underlying value of a std::atomic, so here we cheat and use a union.
 * As an added check, we require that the compiler say that atomic
 * pointers are always lock-free.
 */
union CachedPointer
{
public:
  /// The stored pointer type.
  typedef const void* pointer_t;


  /// Default constructor.  Sets the element to null.
  CachedPointer();

    
  /// Constructor from an element.
  CachedPointer (pointer_t elt);


  /// Copy constructor.
  CachedPointer (const CachedPointer& other);


  /// Assignment.
  CachedPointer& operator= (const CachedPointer& other);


  /// Set the element, assuming it is currently null.
  void set (pointer_t elt) const;


  /// Store a new value to the element.
  void store (pointer_t elt);

  
  /// Return the current value of the element.
  pointer_t get() const;


  /// Return a pointer to the cached element.
  const pointer_t* ptr() const;


private:
  /// The cached element, both directly and as an atomic
  /// (recall that this is a union).
  mutable std::atomic<pointer_t> m_a;  //! Transient
  pointer_t m_e;    //! Transient
};


} // namespace CxxUtils


#include "CxxUtils/CachedPointer.icc"


#endif // not CXXUTILS_CACHEDPOINTER_H
