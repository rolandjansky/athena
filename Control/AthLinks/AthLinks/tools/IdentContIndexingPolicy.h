// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthLinks/tools/IdentContIndexingPolicy.h
 * @author RD Schaffer
 * @brief Indexing policy for an IdentifiableContainer.
 */

#ifndef ATHLINKS_TOOLS_IDENTCONTINDEXINGPOLICY_H
#define ATHLINKS_TOOLS_IDENTCONTINDEXINGPOLICY_H

#include "AthLinks/tools/IdentContIndex.h"
#include "AthLinks/exceptions.h"
#include "AthenaKernel/tools/type_tools.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include <boost/concept_check.hpp>
#include <cassert>
#include <stdexcept>


namespace SG {


/**
 * @brief Indexing policy for an IdentifiableContainer.
 *
 * See ElementLinkTraits.h for a summary of the requirements for an indexing
 * policy.
 *
 * An identifiable container has a two-level index: the collection hash
 * and the index within the collection.  These are packed together
 * in a 32-bit integer, which is what we expose as the index type.
 * There's a value defined for an invalid index, so we don't need
 * to use the @c IndexHolder wrapper.  To unpack this index, use
 * @c IdentContIndex.
 */
template <class CONT>
class IdentContIndexingPolicy
{
private:
  typedef typename CONT::IDENTIFIABLE    coll_type;
  typedef typename coll_type::value_type obj_type;


public:
  /// The type we get when we dereference a link, and derived types.
  typedef typename type_tools::Copy<obj_type>::type            ElementType;
  typedef typename type_tools::Copy<obj_type>::const_reference ElementConstReference;
  typedef typename type_tools::Copy<obj_type>::const_pointer   ElementConstPointer; 

  /// The type of an index, as provided to or returned from a link.
  typedef unsigned int                   index_type;
    
  /// The type of an index, as stored internally within a link.
  typedef index_type stored_index_type;


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
   * Will throw SG::ExcInvalidIndex if the index is invalid and
   * SG::ExcIndexNotFound if the index is not in the container.
   */
  static
  ElementType lookup (index_type index, const CONT& data);


  /**
   * @brief Find the index of the (first) instance of ELEMENT in DATA.
   * @param data The container to search.
   * @param element The element to find.
   * @param index[out] The index in the container of @c element.
   *
   * Throws SG::ExcElementNotFound if the element is not in the container.
   */
  static void
  reverseLookup(const CONT& data, 
                ElementConstReference element,
                index_type& index);
};


} // namespace SG


#include "AthLinks/tools/IdentContIndexingPolicy.icc"


#endif // ATHLINKS_TOOLS_IDENTCONTINDEXINGPOLICY_H
