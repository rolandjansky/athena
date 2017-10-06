// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthLinks/tools/SetIndexingPolicy.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2014
 * @brief Indexing policy for a set-like container.
 */


#ifndef ATHLINKS_SETINDEXINGPOLICY_H
#define ATHLINKS_SETINDEXINGPOLICY_H



#include "AthLinks/tools/DefaultIndexingPolicy.h"
#include "AthLinks/tools/IndexHolder.h"
#include "AthLinks/exceptions.h"
#include "AthenaKernel/tools/type_tools.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include <boost/concept_check.hpp>
#include <set>
#include <algorithm>
#include <cassert>


namespace SG {


/**
 * @brief Indexing policy for a set-like container.
 *
 * See ElementLinkTraits.h for a summary of the requirements for an indexing
 * policy.
 *
 * Indexes here are use IndexHolder, with the key of the container
 * as the payload.
 */
template <class SET>
class SetIndexingPolicy
{
private:
  //compiler checks that SET is an stl simple associative container
  BOOST_CONCEPT_ASSERT((boost::SimpleAssociativeContainerConcept<SET>));

  typedef typename SET::reference reference;
  typedef typename SET::iterator iterator;

public:
  /// The type we get when we dereference a link, and derived types.
  typedef typename SET::value_type ElementType;
  typedef typename SET::const_reference ElementConstReference;
  typedef typename SET::const_pointer ElementConstPointer;

  /// The type of an index, as provided to or returned from a link.
  typedef typename SET::key_type index_type;

  /// The type of an index, as stored internally within a link.
  typedef SG::IndexHolder<index_type> stored_index_type;


  /**
   * @brief Test to see if an index is valid.
   * @param index The index to test.
   */
  static bool isValid (const stored_index_type& index);


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
   * Will throw SG::ExcInvalidIndex if the index is invalid and
   * SG::ExcIndexNotFound if the index is not in the container.
   */
  static
  ElementType lookup(const stored_index_type& index, const SET& data);


  /**
   * @brief Find the index of the (first) instance of ELEMENT in DATA.
   * @param data The container to search.
   * @param element The element to find.
   * @param index[out] The index in the container of @c element.
   *
   * Throws SG::ExcElementNotFound if the element is not in the container.
   */
  static void
  reverseLookup (const SET& data,
                 ElementConstReference element,
                 index_type& same);
};

} // namespace SG



/// Set up to automatically use this indexing policy for sets.
template <typename KEY>
struct DefaultIndexingPolicy <std::set<KEY> > {
  typedef SG::SetIndexingPolicy<std::set<KEY> > type;
};


#include "AthLinks/tools/SetIndexingPolicy.icc"


#endif // not ATHLINKS_SETINDEXINGPOLICY_H
