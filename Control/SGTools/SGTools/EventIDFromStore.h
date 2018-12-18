// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  SGTools/EventIDFromStore.h
 * @author Marcin Nowak
 * @date Nov 2018
 * @brief Helper method to get EventID from an Event Store
 */

#ifndef SGTOOLS_EVENTIDFROMSTORE_H
#define SGTOOLS_EVENTIDFROMSTORE_H

#include "EventInfo/EventID.h"
#include "GaudiKernel/EventContext.h"
#include "AthenaKernel/IProxyDict.h"
#include "SGTools/DataProxy.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/EventContextClid.h"

/**
 * @brief Retrieve the EventID from EventContext saved in store STORE.
 * @param store The store from which to retrieve the EventID, or nullptr.
 *
 * If there is no context recorded in the store, return nullptr
 */
const EventIDBase* EventIDFromStore( IProxyDict* store )
{
   if( store ) {
      //MN: FIX:  is this OK to have it static in case of multiple Stores?
      static const SG::sgkey_t ctxkey = store->stringToKey( "EventContext", ClassID_traits<EventContext>::ID() );
      SG::DataProxy* proxy = store->proxy_exact(ctxkey);
      if( proxy && proxy->object() ) {
         return & SG::DataProxy_cast<EventContext>( proxy )->eventID();
      }
    }
   return nullptr;
}


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

