// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CxxUtils/CachedUniquePtr.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2019
 * @brief Cached unique_ptr with atomic update.
 */


#ifndef CXXUTILS_CACHEDUNIQUEPTR_H
#define CXXUTILS_CACHEDUNIQUEPTR_H


#include <atomic>
#include <memory>


namespace CxxUtils {


/**
 * @brief Cached pointer with atomic update.
 *
 * This acts as a @c unique_ptr that may be set atomically through
 * a const method.  If the pointer is already set, then the new value
 * is deleted.  The intended use case is where one maintains some
 * cached object that is computed lazily.  So one can do, for example,
 *@code
 *  CxxUtils::CachedUniquePtr<Payload> m_payload;
 *  ...
 *  const Payload* ...::get() const
 *  {
 *    if (!m_payload) {
 *      m_payload.set (std::make_unique<Payload> (...));
 *    }
 *    return m_payload.get());
 *  }
 @endcode
 *
 * This only makes sense if the objects passed to set() are equivalent
 * in all threads.  This is generally the case for a lazily-computed value,
 * but this class has no way of enforcing this.
 *
 * It is recommended to generally use the template @c CachedUniquePtr, which
 * is specialized for const types.
 * The more general template @c CachedUniquePtrT can be used with a non-const
 * type (but you don't want to do that for the lazy-cache use case).
 */
template <class T>
class CachedUniquePtrT
{
public:
  /// Default constructor.  Sets the element to null.
  CachedUniquePtrT();

   
  /// Constructor from an element.
  CachedUniquePtrT (std::unique_ptr<T> elt);


  /// Move constructor.
  CachedUniquePtrT (CachedUniquePtrT&& other);


  /// Move.
  CachedUniquePtrT& operator= (CachedUniquePtrT&& other);


  // Destructor.
  ~CachedUniquePtrT();


  /// Atomically set the element.  If already set, then @c elt is discarded.
  void set (std::unique_ptr<T> elt) const;


  /// Store a new value to the element.
  /// Not compatible with other concurrent access.
  void store (std::unique_ptr<T> elt);


  /// Return the current value of the element.
  T* get() const;


  /// Dereference the element.
  T& operator*() const;


  /// Dereference the element.
  T* operator->() const;


  /// Test if the element is null.
  explicit operator bool() const;


  /// Transfer ownership from the element: return the current value as a
  /// unique_ptr, leaving the element null.
  std::unique_ptr<T> release();


private:
  /// The cached element.
  mutable std::atomic<T*> m_ptr;
};


template <class T>
using CachedUniquePtr = CachedUniquePtrT<const T>;


} // namespace CxxUtils


#include "CxxUtils/CachedUniquePtr.icc"


#endif // not CXXUTILS_CACHEDUNIQUEPTR_H
