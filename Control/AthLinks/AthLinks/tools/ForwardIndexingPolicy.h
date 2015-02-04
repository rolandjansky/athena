// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthLinks/tools/ForwardIndexingPolicy.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2013
 * @brief Indexing policy for a vector-like container.
 */


#ifndef ATHLINKS_FORWARDINDEXINGPOLICY_H
#define ATHLINKS_FORWARDINDEXINGPOLICY_H


#include "AthLinks/exceptions.h"
#include "AthLinks/tools/findInContainer.h"
#include "AthenaKernel/tools/type_tools.h"
#include <boost/concept_check.hpp>
#include <cstddef>
#include <stdint.h>


namespace SG {


/**
 * @brief Indexing policy for a vector-like container.
 *
 * See ElementLinkTraits.h for a summary of the requirements for an indexing
 * policy.
 *
 * Indexes here are size_type, with -1 used to mark an invalid index.
 */
template <class CONT, class VALUE_TYPE=typename CONT::value_type>
class ForwardIndexingPolicy
{
private:
  typedef VALUE_TYPE value_type;


public:
  /// The type we get when we dereference a link, and derived types.
  typedef typename type_tools::Copy<value_type>::type ElementType;
  typedef typename type_tools::Copy<value_type>::const_reference ElementConstReference;
  typedef typename type_tools::Copy<value_type>::const_pointer ElementConstPointer; 

  /// The type of an index, as provided to or returned from a link.
  typedef size_t index_type;

  /// The type of an index, as stored internally within a link.
  typedef uint32_t stored_index_type;


private:
  /// Value to mark an invalid index.
  static const index_type INVALID = static_cast<index_type> (-1);


public:
  /**
   * @brief Test to see if an index is valid.
   * @param index The index to test.
   */
  static bool isValid (stored_index_type index);


  /**
   * @brief Convert from stored to external index types.
   * @param index The stored index.
   */
  static index_type storedToExternal (stored_index_type index);


  /**
   * @brief Make an index invalid.
   * @param index[out] The index to reset.
   */
  static void reset (stored_index_type& index);


  /**
   * @brief Retrieve from a container the element corresponding to an index.
   * @param index The index to fetch.
   * @param data The container.
   *
   * Will throw SG::ExcBadForwardLink if the index is invalid.
   */
  static ElementType lookup (index_type index, const CONT& data);


  /**
   * @brief Find the index of the (first) instance of ELEMENT in DATA.
   * @param data The container to search.
   * @param element The element to find.
   * @param index[out] The index in the container of @c element.
   *
   * Throws SG::ExcElementNotFound if the element is not in the container.
   */
  static void reverseLookup(const CONT& data,
                            ElementConstReference element,
                            index_type& index);
};


} // namespace SG


#include "AthLinks/tools/ForwardIndexingPolicy.icc"


#endif // not ATHLINKS_FORWARDINDEXINGPOLICY_H
