// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file CxxUtils/CachedValue.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2017
 * @brief Cached value with atomic update.
 */


#ifndef CXXUTILS_CACHEDVALUE_H
#define CXXUTILS_CACHEDVALUE_H


#include "CxxUtils/checker_macros.h"
#include <atomic>
#include <utility>


namespace CxxUtils {


/// State of the cached value; see below.
enum CacheState {
  INVALID = 0,
  UPDATING = 1,
  VALID = 2
};


/**
 * @brief Cached value with atomic update.
 *
 * Hold a cache of some value.
 * The cache may be set atomically through a const method.
 * This can happen in multiple threads, but the value set should be the same
 * in all threads.  The value can be either valid or invalid; it can be set
 * using the set() method.  (The value can also be set using the non-const
 * store() method; however, this is not atomic and should not be done while
 * other threads may be accessing the value.)  The validity of the value
 * may be tested with isValid(), and ptr() will get back a pointer to the value.
 * ptr() should not be called until isValid() has returned true.
 *
 * The state of the cached value is given by m_cacheState, which can have the values:
 *   INVALID:  No value has been cached.
 *   VALID:    The cached value is valid.
 *   UPDATING: Some thread is in the process of setting the value.
 */
template <class T>
class CachedValue
{
public:
  template <class U>
  friend class CachedValue;

       
  /// Default constructor.  Sets the value to invalid.
  CachedValue();


  /// Constructor from a value.
  CachedValue (const T& val);


  /// Move constructor from a value.
  CachedValue (T&& val);


  /// Copy constructor.
  CachedValue (const CachedValue& other);


  /// Move constructor.
  /// No concurrent operations on OTHER are possible.
  CachedValue (CachedValue&& other);


  /// Copy constructor from other type.
  template <class U>
  CachedValue (const CachedValue<U>& other);


  /// Assignment.
  CachedValue& operator= (const CachedValue& other);

    
  /// Move.
  /// No concurrent operations on OTHER are possible.
  CachedValue& operator= (CachedValue&& other);

    
  /// Set the value, assuming it is currently invalid.
  /// Otherwise, this method will do nothing.
  void set (const T& val) const;
 

  /// Set the value by move, assuming it is currently invalid.
  /// Otherwise, this method will do nothing.
  void set (T&& val) const;
 

  /// Test to see if the value is valid.
  /// May spin if another thread is currently updating the value.
  bool isValid() const;
    

  /// Return a pointer to the cached value.
  /// Should not be called unless isValid() has returned true.
  const T* ptr() const;


  /// Store a new value.
  void store (const T& val);


  /// Store a new value, by move.
  void store (T&& val);


  /// Reset the value to invalid.
  void reset();

    
private:
  /// Current state of the cached value.
  ///   INVALID --- value has not been set.
  ///   VALID --- value has been set.
  ///   UPDATING --- value is in the process of being set by some thread.
  mutable std::atomic<CacheState> m_cacheValid;  //! Transient


  /// The cached value.
  /// Do not return a pointer to this to the user unless the state is VALID.
  mutable T m_val ATLAS_THREAD_SAFE;  //! Transient
};


} // namespace CxxUtils


#include "CxxUtils/CachedValue.icc"


#endif // not CXXUTILS_CACHEDVALUE_H
