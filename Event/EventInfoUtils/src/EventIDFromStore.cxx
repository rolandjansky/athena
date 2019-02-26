
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "EventInfoUtils/EventIDFromStore.h"

#include "GaudiKernel/EventContext.h"
#include "AthenaKernel/EventContextClid.h"
#include "AthenaKernel/IProxyDict.h"
#include "SGTools/DataProxy.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventInfo.h"

const EventIDBase* EventIDFromStore( IProxyDict* store )
{
   if( store ) {
      //MN: FIX:  is this OK to have it static in case of multiple Stores?
      static const SG::sgkey_t ctxkey = store->stringToKey( "EventContext", ClassID_traits<EventContext>::ID() );
      SG::DataProxy* proxy = store->proxy_exact(ctxkey);
      if( proxy && proxy->object() ) {
         return & SG::DataProxy_cast<EventContext>( proxy )->eventID();
      }

      // Check for EventInfo
      proxy = store->proxy( ClassID_traits<EventInfo>::ID(), "" );
      if( proxy && proxy->object() ) {
         return SG::DataProxy_cast<EventInfo>( proxy )->event_ID();
      }
      
    }
   return nullptr;
}

