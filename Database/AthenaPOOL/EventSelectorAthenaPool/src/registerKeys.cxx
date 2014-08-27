/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: registerKeys.cxx,v 1.2 2008-04-10 15:29:57 gemmeren Exp $
/**
 * @file  EventSelectorAthenaPool/src/registerKeys.cxx
 * @author scott snyder
 * @date Dec, 2007
 * @brief Helper functions for registering hash keys with the SG service.
 */

#include "registerKeys.h"
#include "PersistentDataModel/DataHeader.h"
#include "StoreGate/StoreGateSvc.h"

//namespace EventSelectorAthenaPoolUtil {

/**
 * @brief Register all hash keys for one DH Element.
 * @param dhe The DataHeader element.
 * @param store The SG store with which the hashes are to be registered.
 */
void EventSelectorAthenaPoolUtil::registerKeys(const DataHeaderElement& dhe, StoreGateSvc* store) {
   const std::vector<unsigned int>& hashes = dhe.getHashes();
   if (!hashes.empty()) {
      // May be empty if we're reading an old file.
      const std::set<CLID> clids = dhe.getClassIDs();
      size_t i = 0;
      for (std::set<CLID>::const_iterator it = clids.begin(); it != clids.end(); ++it, ++i) {
         store->registerKey(hashes[i], dhe.getKey(), *it);
      }
   }
}

/**
 * @brief Register all hash keys from a DataHeader.
 * @param dh The DataHeader.
 * @param store The SG store with which the hashes are to be registered.
 */
void EventSelectorAthenaPoolUtil::registerKeys(const DataHeader& dh, StoreGateSvc* store) {
   for (std::vector<DataHeaderElement>::const_iterator dhe_i = dh.begin(); dhe_i != dh.end(); ++dhe_i) {
      registerKeys(*dhe_i, store);
   }
}
//} // namespace EventSelectorAthenaPoolUtil
