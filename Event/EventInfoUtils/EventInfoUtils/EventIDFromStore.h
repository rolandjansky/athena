// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  SGTools/EventIDFromStore.h
 * @author Marcin Nowak
 * @date Nov 2019
 * @brief Helper method to get EventID from an Event Store
 */

#ifndef SGTOOLS_EVENTIDFROMSTORE_H
#define SGTOOLS_EVENTIDFROMSTORE_H

#include "StoreGate/StoreGateSvc.h"

class IProxyDict;
class EventIDBase;

/**
 * @brief Retrieve the EventID from EventContext saved in store STORE.
 * @param store The store from which to retrieve the EventID, or nullptr.
 *
 * If there is no context recorded in the store, return nullptr
 */
const EventIDBase* EventIDFromStore( IProxyDict* store );

// variation accepting a ServiceHandle to StoreGate
const EventIDBase* EventIDFromStore( const ServiceHandle<StoreGateSvc>& storeH )
{
   return EventIDFromStore( &*storeH );
}


// variation returning a StatusCode
StatusCode  EventIDFromStore( IProxyDict* store, const EventIDBase*& evid )
{
   evid = EventIDFromStore( store );
   return evid? StatusCode::SUCCESS : StatusCode::FAILURE;
}

#endif 

