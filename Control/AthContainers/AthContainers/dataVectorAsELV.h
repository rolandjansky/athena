// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/dataVectorAsELV.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Helper to make a vector<EL> from a DataVector.
 */


#ifndef ATHCONTAINERS_DATAVECTORASELV_H
#define ATHCONTAINERS_DATAVECTORASELV_H


#include "AthContainers/AuxVectorData.h"
#include "AthContainers/OwnershipPolicy.h"
#include "AthContainers/exceptions.h"
#include "AthContainers/tools/CurrentEventStore.h"
#include "AthContainersInterfaces/AuxStore_traits.h"
#include "AthLinks/ElementLink.h"
#include <vector>


namespace SG {


/**
 * @brief Helper for @c dataVectorViewAsELV.  Don't call this directly.
 *
 * Handle the case of a view vector with elements deriving from AuxElement.
 */
template <class DV>
void dataVectorViewAsELV (const std::true_type&,
                          const DV& v,
                          std::vector<ElementLink<DV> >& ret,
                          IProxyDict* store)
{
  const DV* last_d = nullptr;
  const SG::AuxVectorData* last_b = nullptr;
  for (const typename DV::base_value_type* elt : v) {
    const SG::AuxVectorData* cont_b = elt->container();
    if (cont_b != last_b) {
      last_b = cont_b;
      last_d = dynamic_cast<const DV*>(cont_b);
    }
    if (!last_d)
      throw SG::ExcDVToELV ("Element not in a container");
    ret.emplace_back (*last_d, elt->index(), store);
  }
}



/**
 * @brief Helper for @c dataVectorViewAsELV.  Don't call this directly.
 *
 * Handle the case of a view vector with elements not deriving from AuxElement
 * (just throw an exception).
 */
template <class DV>
void dataVectorViewAsELV (const std::false_type&,
                          const DV&,
                          std::vector<ElementLink<DV> >&,
                          IProxyDict*)
{
  throw SG::ExcDVToELV ("Element in view container does not derive from AuxElement.");
}



/**
 * @brief Helper to make a vector<EL> from a DataVector.
 *
 * Given a @c DataVector, return a vector of ElementLink objects that
 * reference the @c DataVector's elements.  We can always do this if the
 * @c DataVector owns its elements.  However, if it is a view container,
 * then the elements must derive from @c SG::AuxElement and must be
 * part of some other container.
 */
template <class DV>
std::vector<ElementLink<DV> >
dataVectorAsELV (const DV& v, IProxyDict* store = 0)
{
  std::vector<ElementLink<DV> > ret;
  size_t sz = v.size();
  ret.reserve (sz);

  if (!store) store = SG::CurrentEventStore::store();

  if (v.ownPolicy() == SG::OWN_ELEMENTS) {
    // Could hoist the sgkey lookup.
    // But this will probably not be a common case, so don't do that
    // unless it's needed.
    for (size_t i = 0; i < sz; i++)
      ret.emplace_back (v, i, store);
  }
  else {
    dataVectorViewAsELV (typename AuxStore_traits<DV>::flag(), v, ret, store);
  }
  
  return ret;
}


} // namespace SG


#endif // not ATHCONTAINERS_DATAVECTORASELV_H
