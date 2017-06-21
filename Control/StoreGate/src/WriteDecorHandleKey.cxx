// $Id$
/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file StoreGate/src/WriteDecorHandleKey.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2017
 * @brief Property holding a SG store/key/clid/attr name from which a
 *        WriteDecorHandle is made.
 */


#include "StoreGate/WriteDecorHandleKey.h"


namespace SG {
namespace detail {


/**
 * @brief Optionally register read dependency of a @c WriteDecorHandleKey.
 *
 * A @c WriteDecorHandle logically has a read dependency on the container itself.
 * However, just declaring this as a usual @c ReadHandleKey will cause problems
 * if used in an algorithm that also creates the container: we'll get complaints
 * from the @c Algorithm base class about circular dependencies.
 *
 * To deal with this, we don't declare the container @c ReadHandleKey in
 * @c declareProperty.  Instead, we wait until @c initialize and declare
 * the dependency with @c addDependency only if it is not already
 * on a handle list.
 */
void registerWriteDecorHandleKey (IDataHandleHolder* owner,
                                  const DataObjID& contHandleKey)
{
  if (!owner) return;

  bool found = false;
  for (const Gaudi::DataHandle* h : owner->inputHandles()) {
    if (h->fullKey() == contHandleKey) {
      found = true;
      break;
    }
  }

  if (!found) {
    for (const Gaudi::DataHandle* h : owner->outputHandles()) {
      if (h->fullKey() == contHandleKey) {
        found = true;
        break;
      }
    }
  }

  if (!found) {
    owner->addDependency (contHandleKey, Gaudi::DataHandle::Reader);
  }
}


} // namespace detail
} // namespace SG
