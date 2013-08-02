// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file LArElecCalib/LArVectorProxy.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2011
 * @brief Proxy for accessing a range of float values like a vector.
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
  typedef const float* const_iterator;


  /**
   * @brief Default constructor. 
   *        Creates the proxy in an invalid state.
   */
  LArVectorProxy();


  /**
   * @brief Construct a proxy referencing an existing vector.
   * @param vec The existing vector to reference.
   */
  LArVectorProxy (const std::vector<float>& vec);


  /**
   * @brief Construct a proxy referencing a range of vectors in memory.
   * @param beg Pointer to the start of the range.
   * @param end Pointer to the (exclusive) end of the range.
   */
  LArVectorProxy (const float* beg, const float* end);


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
  float operator[] (size_t i) const;


  /**
   * @brief Vector indexing with bounds check.
   */
  float at (size_t i) const;


  /**
   * @brief Convert back to a vector.
   */
  std::vector<float> asVector() const;


  /**
   * @brief Begin iterator.
   */
  const_iterator begin() const;


  /**
   * @brief End iterator.
   */
  const_iterator end() const;


private:
  const float* m_beg;
  const float* m_end;
};


#include "LArElecCalib/LArVectorProxy.icc"


#endif // not LARELECCALIB__LARVECTORPROXY_H
