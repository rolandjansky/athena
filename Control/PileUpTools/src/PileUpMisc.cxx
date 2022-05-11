/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "PileUpTools/PileUpMisc.h"


#include "StoreGate/ActiveStoreSvc.h"
#include "StoreGate/StoreGateSvc.h" /*to print name() */
#include "GaudiKernel/IEvtSelector.h"

#include "xAODEventInfo/EventInfoContainer.h"

#include "PileUpTools/IBeamIntensity.h"


xAOD::EventInfo* addSubEvent( xAOD::EventInfo* targetEv,
                  const xAOD::EventInfo::SubEvent& subev,
                  xAOD::EventInfoContainer* eiContainer,
                  const std::string& eiContKey,
                  StoreGateSvc* subev_store )
{
   // add a EI copy to the EI container
   xAOD::EventInfo* newEv;
   eiContainer->push_back( newEv=new xAOD::EventInfo( * subev.ptr() ) );
   if(newEv->evtStore()==nullptr && subev_store!=nullptr) newEv->setEvtStore(subev_store);
         
   // link to the fresh EI added to the container:
   ElementLink< xAOD::EventInfoContainer > eilink( eiContKey, eiContainer->size()-1, targetEv->evtStore() );
   xAOD::EventInfo::SubEvent  newsubev( subev.time(), subev.index(), subev.type(), eilink );
   targetEv->addSubEvent( newsubev );

   // copy subevents recursively
   for( const auto& se: subev.ptr()->subEvents() ) {
      addSubEvent( targetEv, se, eiContainer, eiContKey );
   }
   
   return newEv;
}


xAOD::EventInfo* addSubEvent( xAOD::EventInfo* targetEv,
                  const xAOD::EventInfo* ev2add,
                  int16_t subev_time,
                  xAOD::EventInfo::PileUpType subev_type,
                  xAOD::EventInfoContainer* eiContainer,
                  const std::string& eiContKey,
                  StoreGateSvc* ev2add_store )
{
   // add a EI copy to the EI container
   xAOD::EventInfo* newEv;
   eiContainer->push_back( newEv=new xAOD::EventInfo( *ev2add ) );
   if(newEv->evtStore()==nullptr && ev2add_store!=nullptr) newEv->setEvtStore(ev2add_store);
         
   // link to the fresh EI added to the container:
   ElementLink< xAOD::EventInfoContainer > eilink( eiContKey, eiContainer->size()-1, targetEv->evtStore() );
   xAOD::EventInfo::SubEvent  newsubev( subev_time, targetEv->subEvents().size(), subev_type, eilink );
   targetEv->addSubEvent( newsubev );

   // copy subevents recursively
   for( const auto& subev: ev2add->subEvents() ) {
      addSubEvent( targetEv, subev, eiContainer, eiContKey );
   }

   return newEv;
}

