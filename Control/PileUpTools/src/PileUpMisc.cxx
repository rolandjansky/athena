/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PileUpTools/PileUpMisc.h"


#include "StoreGate/ActiveStoreSvc.h"
#include "StoreGate/StoreGateSvc.h" /*to print name() */
#include "GaudiKernel/IEvtSelector.h"

#include "xAODEventInfo/EventInfoContainer.h"

#include "PileUpTools/IBeamIntensity.h"


void addSubEvent( xAOD::EventInfo* targetEv,
                  const xAOD::EventInfo::SubEvent& subev,
                  xAOD::EventInfoContainer* eiContainer,
                  const std::string& eiContKey )
{
   // add a EI copy to the EI container
   eiContainer->push_back( new xAOD::EventInfo( * subev.ptr() ) );
         
   // link to the fresh EI added to the container:
   ElementLink< xAOD::EventInfoContainer > eilink( eiContKey, eiContainer->size()-1, targetEv->evtStore() );
   xAOD::EventInfo::SubEvent  newsubev( subev.time(), subev.index(), subev.type(), eilink );
   targetEv->addSubEvent( newsubev );

   // copy subevents recursively
   for( const auto& se: subev.ptr()->subEvents() ) {
      addSubEvent( targetEv, se, eiContainer, eiContKey );
   }
}


void addSubEvent( xAOD::EventInfo* targetEv,
                  const xAOD::EventInfo* ev2add,
                  int16_t subev_time,
                  xAOD::EventInfo::PileUpType subev_type,
                  xAOD::EventInfoContainer* eiContainer,
                  const std::string& eiContKey )
{
   // add a EI copy to the EI container
   eiContainer->push_back( new xAOD::EventInfo( *ev2add ) );
         
   // link to the fresh EI added to the container:
   ElementLink< xAOD::EventInfoContainer > eilink( eiContKey, eiContainer->size()-1, targetEv->evtStore() );
   xAOD::EventInfo::SubEvent  newsubev( subev_time, targetEv->subEvents().size(), subev_type, eilink );
   targetEv->addSubEvent( newsubev );

   // copy subevents recursively
   for( const auto& subev: ev2add->subEvents() ) {
      addSubEvent( targetEv, subev, eiContainer, eiContKey );
   }
}

