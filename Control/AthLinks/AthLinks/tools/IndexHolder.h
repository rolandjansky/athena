// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthLinks/tools/IndexHolder.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2014
 * @brief Store an @c ElementLink index for non-vector containers.
 */


#ifndef ATHLINKS_INDEXHOLDER_H
#define ATHLINKS_INDEXHOLDER_H


namespace SG {


/**
 * @brief Store an @c ElementLink index for non-vector containers.
 *
 * When we store an index to a vector container, we use -1 as an invalid
 * index value.  However, in the general case where we have a container
 * with an index of arbitrary type, we don't have a distinguished value
 * we can use to represent invalid.  So for that case, we need to store
 * an additional valid flag along with the index.  Things are factored
 * like this so that we can avoid storing the flag for the common case
 * of indexing a vector.
 */
template <class T>
class IndexHolder
{
public:
  /// Constructor.
  IndexHolder() : m_valid(false) {}
  
  /// Copy constructor.
  IndexHolder (const T& t) : m_index(t), m_valid(true) {}

  /// Return valid flag.
  bool isValid() const { return m_valid; }

  /// Reset the index to a null value.
  void reset() { m_valid = false; m_index = T(); }

  /// Retrieve the index.
  operator const T&() const { return m_index; }


private:
  /// The stored index.
  T m_index;

  /// True if the index is valid.
  bool m_valid;
};


} // namespace SG


#endif // not ATHLINKS_INDEXHOLDER_H
