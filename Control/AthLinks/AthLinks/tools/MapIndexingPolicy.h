// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthLinks/tools/MapIndexingPolicy.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2014
 * @brief Indexing policy for a map-like container.
 */


#ifndef ATHLINKS_MAPINDEXINGPOLICY_H
#define ATHLINKS_MAPINDEXINGPOLICY_H



#include "AthLinks/tools/DefaultIndexingPolicy.h"
#include "AthLinks/tools/IndexHolder.h"
#include "AthLinks/exceptions.h"
#include "AthenaKernel/tools/type_tools.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include <boost/concept_check.hpp>
#include <map>
#include <algorithm>
#include <cassert>


namespace SG {


/**
 * @brief Indexing policy for a map-like container.
 *
 * See ElementLinkTraits.h for a summary of the requirements for an indexing
 * policy.
 *
 * Indexes here are use IndexHolder, with the key of the container
 * as the payload.  The type returned from the link is the second type
 * of the map.
 */
template <class MAP>
class MapIndexingPolicy
{
private:
  //compiler checks that MAP is an stl pair associative container
  BOOST_CONCEPT_ASSERT((boost::PairAssociativeContainerConcept<MAP>));

  typedef typename MAP::mapped_type mapped_type;
  typedef typename MAP::const_iterator const_iterator;


public:
  /// The type we get when we dereference a link, and derived types.
  typedef typename type_tools::Copy<mapped_type>::type ElementType;
  typedef typename type_tools::Copy<mapped_type>::const_reference ElementConstReference;
  typedef typename type_tools::Copy<mapped_type>::const_pointer ElementConstPointer; 

  /// The type of an index, as provided to or returned from a link.
  typedef typename MAP::key_type index_type;

  /// The type of an index, as stored internally within a link.
  typedef SG::IndexHolder<index_type> stored_index_type;


  /**
   * @brief Test to see if an index is valid.
   * @param index The index to test.
   */
  static bool isValid (const stored_index_type& index) ;


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
  ElementType lookup(const stored_index_type& index, const MAP& data);


  /**
   * @brief Find the index of the (first) instance of ELEMENT in DATA.
   * @param data The container to search.
   * @param element The element to find.
   * @param index[out] The index in the container of @c element.
   *
   * Throws SG::ExcElementNotFound if the element is not in the container.
   */
  static void
  reverseLookup(const MAP& data,
                ElementConstReference element,
                index_type& index);
};


} // namespace SG


/// Set up to automatically use this indexing policy for maps.
template <typename KEY, typename ELEM>
struct DefaultIndexingPolicy<std::map<KEY, ELEM> > {
  typedef SG::MapIndexingPolicy<std::map<KEY, ELEM> > type;
};


#include "AthLinks/tools/MapIndexingPolicy.icc"


#endif // not ATHLINKS_MAPINDEXINGPOLICY_H
