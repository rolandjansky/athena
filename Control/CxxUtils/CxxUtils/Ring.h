// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file CxxUtils/Ring.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2018
 * @brief A simple ring buffer.
 */


#ifndef CXXUTILS_RING_H
#define CXXUTILS_RING_H


#include <vector>
#include <cstddef>


namespace CxxUtils {


/**
 * @brief A very simple ring buffer.
 *
 * The payload class T must support comparison and default construction.
 * Default-constructed T's are used to indicate unfilled slots in the buffer.
 *
 * Start by calling @c reset to set the size of the buffer.
 * Then @c push will add new elements to the ring.
 *
 * The only operation to retrieve values at this point is @c getKeysDedup.
 * More could be added if needed.
 */
template <class T>
class Ring
{
public:
  /** 
   * @brief Clear the buffer and set its size.
   * @param size New size of the buffer.
   */
  void reset (size_t size);


  /**
   * @brief Add a new item to the buffer.
   * @param x Item to add.
   */
  void push (const T& x);


  /**
   * @brief Return a copy of keys in the buffer.
   *
   * Immediately adjacent duplicates and unfilled entries will be removed,
   * so the result may be smaller than the size of the buffer.
   */
  std::vector<T> getKeysDedup() const;


private:
  std::vector<T> m_data;
  size_t m_pos = 0;
};


} // namespace CxxUtils


#include "CxxUtils/Ring.icc"


#endif // not CXXUTILS_RING_H
