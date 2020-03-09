// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file LArElecCalib/LArVectorProxy.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2011
 * @brief Proxy for accessing a range of float values like a vector.
 *
 * Can change to a view once we switch to c++20?
 */


#ifndef LARELECCALIB_LARVECTORPROXY_H
#define LARELECCALIB_LARVECTORPROXY_H


#include <vector>
#include <cstdlib>


/**
 * @brief Proxy for accessing a range of float values like a vector.
 *
 * For the shape and OFC conditions data, the data for many channels
 * is flattened into a single vector.  But we still want to provide
 * a vector-like interface for the data for an individual channel.
 * 
 * We can do that with this proxy object.  It stores two pointers
 * to floats, which define a range.  It then provides methods
 * compatible with std::vector to access these data.
 *
 * We only implement methods that are actually used by existing clients.
 */
class LArVectorProxy
{
public:
  /// The contained type.
  typedef float value_type;


  /// Be lazy and just use a pointer as the iterator type.
  typedef const value_type* const_iterator;


  /**
   * @brief Default constructor. 
   *        Creates the proxy in an invalid state.
   */
  LArVectorProxy();


  /**
   * @brief Construct a proxy referencing an existing vector.
   * @param vec The existing vector to reference.
   */
  LArVectorProxy (const std::vector<value_type>& vec);


  /**
   * @brief Construct a proxy referencing a range of vectors in memory.
   * @param beg Pointer to the start of the range.
   * @param end Pointer to the (exclusive) end of the range.
   */
  LArVectorProxy (const value_type* beg, const value_type* end);


  /**
   * @brief Test to see if the proxy has been initialized.
   */
  bool valid() const;


  /**
   * @brief Return the size of the range being proxied.
   */
  size_t size() const;


  /**
   * @brief Vector indexing.
   */
  value_type operator[] (size_t i) const;


  /**
   * @brief Vector indexing with bounds check.
   */
  value_type at (size_t i) const;


  /**
   * @brief Convert back to a vector.
   */
  std::vector<value_type> asVector() const;


  /**
   * @brief Return a pointer to the start of the data.
   */
  const value_type* data() const;


  /**
   * @brief Begin iterator.
   */
  const_iterator begin() const;


  /**
   * @brief End iterator.
   */
  const_iterator end() const;


private:
  const value_type* m_beg;
  const value_type* m_end;
};


#include "LArElecCalib/LArVectorProxy.icc"


#endif // not LARELECCALIB__LARVECTORPROXY_H
