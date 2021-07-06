/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaKernel/src/IProxyDict.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2015
 * @brief Default implementations for IProxyDict.
 */


#include "AthenaKernel/IProxyDict.h"
#include "GaudiKernel/IConverter.h"


/**
 * @brief Tell the store that a handle has been bound to a proxy.
 * @param handle The handle that was bound.
 * The default implementation does nothing.
 */
void IProxyDict::boundHandle (IResetable* /*handle*/)
{
}


/**
 * @brief Tell the store that a handle has been unbound from a proxy.
 * @param handle The handle that was unbound.
 * The default implementation does nothing.
 */
void IProxyDict::unboundHandle (IResetable* /*handle*/)
{
}


/**
 * @brief Test to see if the target of an ElementLink has moved.
 * @param sgkey_in Original hashed key of the EL.
 * @param index_in Original index of the EL.
 * @param sgkey_out[out] New hashed key for the EL.
 * @param index_out[out] New index for the EL.
 * @return True if there is a remapping; false otherwise.
 *
 * The default implementation here always returns false.
 */
bool IProxyDict::tryELRemap (sgkey_t /*sgkey_in*/, size_t /*index_in*/,
                             sgkey_t& /*sgkey_out*/, size_t& /*index_out*/)
{
  return false;
}


/**
 * @brief Call converter to create an object, possibly with locking.
 * @param cvt The converter to call.
 * @param addr Opaque address information for the object to create.
 * @param refpObject Reference to location of the pointer of the
 *                   created object.
 *
 * This calls the @c createObj method on @c cvt to create the referenced
 * transient object.
 *
 * This will also lock the store, if that is required.
 *
 * The default implementation just forwards the method call,
 * with no locking.
 */
StatusCode IProxyDict::createObj (IConverter* cvt,
                                  IOpaqueAddress* addr,
                                  DataObject*& refpObject)
{
  return cvt->createObj (addr, refpObject);
}


/**
 * @brief Return the current event-slot-specific store.
 *
 * The default version just returns this.
 */
IProxyDict* IProxyDict::hiveProxyDict()
{
  return this;
}


/**
 * @brief Return the metadata source ID for the current event slot.
 * @param key SG key of the DataHeader to query.
 *        Returns an empty string if no source has been set.
 *
 *        The default version always returns an empty string.
 */
SG::SourceID
IProxyDict::sourceID (const std::string& /*key = "EventSelector"*/) const
{
  return SG::SourceID();
}
