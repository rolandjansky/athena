/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IProxyDictWithPool.cxx,v 1.1 2008-04-18 03:18:24 ssnyder Exp $
/**
 * @file SGTools/src/IProxyDictWithPool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2007
 * @brief Combine IProxyDict and IStringPool.
 */


#include "SGTools/IProxyDictWithPool.h"


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
bool IProxyDictWithPool::tryELRemap (sgkey_t /*sgkey_in*/,
                                     size_t /*index_in*/,
                                     sgkey_t& /*sgkey_out*/,
                                     size_t& /*index_out*/)
{
  return false;
}

