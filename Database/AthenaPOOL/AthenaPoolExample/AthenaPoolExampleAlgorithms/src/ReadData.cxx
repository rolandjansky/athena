/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file AthenaPoolExampleAlgorithms/src/ReadData.cxx
 *  @brief This file contains the implementation for the ReadData class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ReadData.cxx,v 1.26 2009-03-14 22:05:57 gemmeren Exp $
 **/

#include "ReadData.h"

// the user data-class defintions
#include "AthenaPoolExampleData/ExampleHitContainer.h"
#include "AthenaPoolExampleData/ExampleTrackContainer.h"

#include "PersistentDataModel/DataHeader.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventStreamInfo.h"
#include "EventBookkeeperMetaData/EventBookkeeperCollection.h"

#include <set>

using namespace AthPoolEx;

//___________________________________________________________________________
ReadData::ReadData(const std::string& name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator),
    p_SGinMeta("StoreGateSvc/InputMetaDataStore", name),
    p_SGmeta("StoreGateSvc/MetaDataStore", name)
{
}
//___________________________________________________________________________
ReadData::~ReadData() {
}
//___________________________________________________________________________
StatusCode ReadData::initialize() {
   ATH_MSG_INFO("in initialize()");

   // Locate the StoreGateSvc and initialize our local ptr
   if (!p_SGinMeta.retrieve().isSuccess()) {
      ATH_MSG_ERROR("Could not find Input MetaData StoreGateSvc");
      return StatusCode::FAILURE;
   }
   if (!p_SGmeta.retrieve().isSuccess()) {
      ATH_MSG_ERROR("Could not find Tag MetaData StoreGateSvc");
      return StatusCode::FAILURE;
   }

   ATH_CHECK( m_dataHeaderKey.initialize() );
   if (!m_exampleTrackKey.key().empty()) {
     ATH_CHECK( m_exampleTrackKey.initialize() );
   }
   ATH_CHECK( m_exampleHitKey.initialize() );
   return StatusCode::SUCCESS;
}
//___________________________________________________________________________
StatusCode ReadData::execute (const EventContext& ctx) const {
   ATH_MSG_DEBUG("in execute()");

   SG::ConstIterator<EventStreamInfo> esi1, esi2;
   if (p_SGinMeta->retrieve(esi1, esi2).isFailure() || esi1 == esi2) {
      ATH_MSG_WARNING("Could not find EventStreamInfo");
   } else {
      for (; esi1 != esi2; esi1++) {
         ATH_MSG_INFO("EventStreamInfo: Number of events = " << esi1->getNumberOfEvents());
         ATH_MSG_INFO("EventStreamInfo: ItemList:");
         for (std::set<std::pair<CLID, std::string> >::const_iterator iter = esi1->getItemList().begin(), iterEnd = esi1->getItemList().end();
		         iter != iterEnd; iter++) {
            ATH_MSG_INFO("CLID = " << iter->first << ", key = " << iter->second);
         }
         for (std::set<EventType>::iterator iter = esi1->getEventTypes().begin(), iterEnd = esi1->getEventTypes().end();
		         iter != iterEnd; iter++) {
            ATH_MSG_INFO("EventType: " << iter->typeToString());
            ATH_MSG_INFO("TagInfo: " << iter->get_detdescr_tags());
         }
      }
   }
   const std::string ebcKey = "EventSelector.Counter";
   if (p_SGmeta->contains<EventBookkeeperCollection>(ebcKey)) {
      const EventBookkeeperCollection* ebc = nullptr;
      if (p_SGmeta->retrieve(ebc, ebcKey).isFailure()) {
         ATH_MSG_FATAL("Could not find EventBookkeeperCollection, key =");
         return StatusCode::FAILURE;
      }
      for (EventBookkeeperCollection::const_iterator iter = ebc->begin(), iterEnd = ebc->end();
		      iter != iterEnd; iter++) {
         ATH_MSG_INFO("EventBookkeeper " << (*iter)->getName() << " accepted events: = " << (*iter)->getNAcceptedEvents());
      }
   }
   const std::string ebcInKey = "EventBookkeepers";
   if (p_SGinMeta->contains<EventBookkeeperCollection>(ebcInKey)) {
      const EventBookkeeperCollection* ebc = nullptr;
      if (p_SGinMeta->retrieve(ebc, ebcInKey).isFailure()) {
         ATH_MSG_FATAL("Could not find EventBookkeeperCollection, key =");
         return StatusCode::FAILURE;
      }
      for (EventBookkeeperCollection::const_iterator iter = ebc->begin(), iterEnd = ebc->end();
		      iter != iterEnd; iter++) {
         ATH_MSG_INFO("EventBookkeeper (In) " << (*iter)->getName() << " accepted events: = " << (*iter)->getNAcceptedEvents());
      }
   }

   SG::ReadHandle<DataHeader> dh (m_dataHeaderKey, ctx);
   for (std::vector<DataHeaderElement>::const_iterator dhe_p = dh->begin(); dhe_p != dh->end(); dhe_p++) {
      ATH_MSG_INFO("DataHeader (Event Content) " << dhe_p->getToken()->toString());
   }
   for (std::vector<DataHeaderElement>::const_iterator dhe_p = dh->beginProvenance(); dhe_p != dh->endProvenance(); dhe_p++) {
      ATH_MSG_INFO("DataHeader (Provenance) " << dhe_p->getToken()->toString());
   }

   // Get the event header, print out event and run number
   const EventIDBase& eid = ctx.eventID();
   ATH_MSG_INFO("EventInfo event: " << eid.event_number() << " run: " << eid.run_number());
   // Get the DataObject, print out its contents
   ATH_MSG_INFO("Get Smart data ptr 1");

   if (!m_exampleTrackKey.key().empty()) {
     SG::ReadHandle<ExampleTrackContainer> tracks (m_exampleTrackKey, ctx);
     if (tracks.isValid()) {
       for (const ExampleTrack* track : *tracks) {
         ATH_MSG_INFO("Track pt = " << track->getPT() << " eta = " << track->getEta() << " phi = " << track->getPhi() << " detector = " << track->getDetector());
         try {
           double x = track->getElement1()->getX();
           ATH_MSG_INFO("ElementLink1 = " << x);
           ATH_MSG_INFO("ElementLink2 = " << track->getElement2()->getX());
           ATH_MSG_INFO("Link ElementLinkVector = " << track->getElementLinkVector()->size());
           for (ElementLinkVector<ExampleHitContainer>::const_iterator iter = track->getElementLinkVector()->begin(); iter != track->getElementLinkVector()->end(); ++iter) {
             ATH_MSG_INFO("Element = " << (**iter) << " : " << (**iter)->getX());
           }
           ATH_MSG_INFO("Link Navigable = " << track->getNavigable()->size());
           for (Navigable<ExampleHitContainer>::object_iter iter = track->getNavigable()->begin(); iter != track->getNavigable()->end(); iter++) {
             ATH_MSG_INFO("Element = " << (*iter) << " : " << (*iter)->getX());
           }
           ATH_MSG_INFO("Link Weighted Navigable = " << track->getWeightedNavigable()->size());
           for (Navigable<ExampleHitContainer, double>::object_iter iter = track->getWeightedNavigable()->begin(); iter != track->getWeightedNavigable()->end(); iter++) {
             ATH_MSG_INFO("Element = " << (*iter) << " : " << (*iter)->getX());
           }
         } catch (...) {
           ATH_MSG_WARNING("Could not follow ExampleTrackContainer/MyTracks ElementLinks to ExampleHitContainer/MyHits");
         }
       }
     }
     else {
       ATH_MSG_INFO("Could not find ExampleTrackContainer/MyTracks");
     }
   }

   SG::ReadHandle<ExampleHitContainer> hits (m_exampleHitKey, ctx);
   if (hits.isValid()) {
     for (const ExampleHit* hit : *hits) {
       ATH_MSG_INFO("Hit x = " << hit->getX() << " y = " << hit->getY() << " z = " << hit->getZ() << " detector = " << hit->getDetector());
     }
     
     if (evtStore()->contains<ExampleHitContainer>("PetersHits")) {
       ATH_MSG_INFO("Found ExampleHitContainer/PetersHits (alias)");
     }
   }
   else {
     ATH_MSG_INFO("Could not find ExampleHitContainer/MyHits");
   }

   return StatusCode::SUCCESS;
}
//___________________________________________________________________________
StatusCode ReadData::finalize() {
   ATH_MSG_INFO("in finalize()");
   return StatusCode::SUCCESS;
}
