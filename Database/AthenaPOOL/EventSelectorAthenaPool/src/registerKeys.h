// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: registerKeys.h,v 1.2 2008-04-10 15:29:57 gemmeren Exp $

/**
 * @file  EventSelectorAthenaPool/src/registerKeys.h
 * @author scott snyder
 * @date Dec, 2007
 * @brief Helper functions for registering hash keys with the SG service.
 *
 * Each SG key/CLID pair can also be represented as an integer hash code.
 * This hash is stored in the DataHeader.  While one can always calculate
 * the hash given the key/CLID, sometimes one needs to go the other way
 * as well.  To make this possible, the hash codes are stored in the
 * DataHeader, along with the SG key/CLIDs.  When a DataHeader is read,
 * these mappings from hash code to SG key/CLID are registered with the
 * appropriate SG service.  These helper functions perform this operation.
 */

#ifndef EVENTSELECTORATHENAPOOL_REGISTERKEYS_H
#define EVENTSELECTORATHENAPOOL_REGISTERKEYS_H

class DataHeaderElement;
class DataHeader;
class StoreGateSvc;

namespace EventSelectorAthenaPoolUtil {
   /**
    * @brief Register all hash keys for one DH Element.
    * @param dhe The DataHeader element.
    * @param store The SG store with which the hashes are to be registered.
    */
   void registerKeys(const DataHeaderElement& dhe, StoreGateSvc* store);

   /**
    * @brief Register all hash keys from a DataHeader.
    * @param dh The DataHeader.
    * @param store The SG store with which the hashes are to be registered.
    */
   void registerKeys(const DataHeader& dh, StoreGateSvc* store);
} // namespace EventSelectorAthenaPoolUtil

#endif // not EVENTSELECTORATHENAPOOL_REGISTERKEYS_H
