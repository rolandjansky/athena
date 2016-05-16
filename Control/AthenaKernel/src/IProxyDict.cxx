/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaKernel/src/IProxyDict.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2015
 * @brief Default implementations for IProxyDict.
 */


#include "AthenaKernel/IProxyDict.h"


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

