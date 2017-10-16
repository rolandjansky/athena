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

#include "DBDataModel/CollectionMetadata.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventStreamInfo.h"
#include "EventBookkeeperMetaData/EventBookkeeperCollection.h"

#include <set>

using namespace AthPoolEx;

//___________________________________________________________________________
ReadData::ReadData(const std::string& name, ISvcLocator* pSvcLocator) : AthAlgorithm(name, pSvcLocator), p_SGinMeta("StoreGateSvc/InputMetaDataStore", name), p_SGtagMeta("StoreGateSvc/TagMetaDataStore", name), p_SGmeta("StoreGateSvc/MetaDataStore", name) {
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
   if (!p_SGtagMeta.retrieve().isSuccess()) {
      ATH_MSG_ERROR("Could not find Tag MetaData StoreGateSvc");
      return StatusCode::FAILURE;
   }
   if (!p_SGtagMeta.retrieve().isSuccess()) {
      ATH_MSG_ERROR("Could not find MetaData StoreGateSvc");
      return StatusCode::FAILURE;
   }
   return StatusCode::SUCCESS;
}
//___________________________________________________________________________
StatusCode ReadData::execute() {
   ATH_MSG_DEBUG("in execute()");

   if (p_SGtagMeta->contains<CollectionMetadata>("CollectionMetadata")) {
      const DataHandle<CollectionMetadata> cm;
      if (p_SGtagMeta->retrieve(cm, "CollectionMetadata").isFailure()) {
         ATH_MSG_FATAL("Could not find CollectionMetadata");
         return StatusCode::FAILURE;
      }
      for (CollectionMetadata::const_iterator iter = cm->begin(), iterEnd = cm->end();
		      iter != iterEnd; iter++) {
         ATH_MSG_INFO("CollectionMetadata, key = " << iter->first << ", value = " << iter->second);
      }
   }
   // Get the event streamheader, print out 
   const DataHandle<EventStreamInfo> esi1, esi2;
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
      const DataHandle<EventBookkeeperCollection> ebc;
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
      const DataHandle<EventBookkeeperCollection> ebc;
      if (p_SGinMeta->retrieve(ebc, ebcInKey).isFailure()) {
         ATH_MSG_FATAL("Could not find EventBookkeeperCollection, key =");
         return StatusCode::FAILURE;
      }
      for (EventBookkeeperCollection::const_iterator iter = ebc->begin(), iterEnd = ebc->end();
		      iter != iterEnd; iter++) {
         ATH_MSG_INFO("EventBookkeeper (In) " << (*iter)->getName() << " accepted events: = " << (*iter)->getNAcceptedEvents());
      }
   }
   // Get the event header, print out event and run number
   const DataHandle<EventInfo> evt;
   if (evtStore()->retrieve(evt).isFailure()) {
      ATH_MSG_FATAL("Could not find EventInfo");
      return StatusCode::FAILURE;
   }
   ATH_MSG_INFO("EventInfo event: " << evt->event_ID()->event_number() << " run: " << evt->event_ID()->run_number());
   // Get the DataObject, print out its contents
   ATH_MSG_INFO("Get Smart data ptr 1");

   if (evtStore()->contains<ExampleTrackContainer>("MyTracks")) {
      const DataHandle<ExampleTrackContainer> cont;
      if (evtStore()->retrieve(cont, "MyTracks").isFailure()) {
         ATH_MSG_ERROR("Could not find ExampleTrackContainer/MyTracks");
         return StatusCode::FAILURE;
      }
      for (ExampleTrackContainer::const_iterator obj = cont->begin(); obj != cont->end(); obj++) {
         ATH_MSG_INFO("Track pt = " << (*obj)->getPT() << " eta = " << (*obj)->getEta() << " phi = " << (*obj)->getPhi() << " detector = " << (*obj)->getDetector());
         try {
            double x = (*obj)->getElement1()->getX();
            ATH_MSG_INFO("ElementLink1 = " << x);
            ATH_MSG_INFO("ElementLink2 = " << (*obj)->getElement2()->getX());
            ATH_MSG_INFO("Link ElementLinkVector = " << (*obj)->getElementLinkVector()->size());
            for (ElementLinkVector<ExampleHitContainer>::const_iterator iter = (*obj)->getElementLinkVector()->begin(); iter != (*obj)->getElementLinkVector()->end(); ++iter) {
               ATH_MSG_INFO("Element = " << (**iter) << " : " << (**iter)->getX());
            }
            ATH_MSG_INFO("Link Navigable = " << (*obj)->getNavigable()->size());
            for (Navigable<ExampleHitContainer>::object_iter iter = (*obj)->getNavigable()->begin(); iter != (*obj)->getNavigable()->end(); iter++) {
               ATH_MSG_INFO("Element = " << (*iter) << " : " << (*iter)->getX());
            }
            ATH_MSG_INFO("Link Weighted Navigable = " << (*obj)->getWeightedNavigable()->size());
            for (Navigable<ExampleHitContainer, double>::object_iter iter = (*obj)->getWeightedNavigable()->begin(); iter != (*obj)->getWeightedNavigable()->end(); iter++) {
               ATH_MSG_INFO("Element = " << (*iter) << " : " << (*iter)->getX());
            }
         } catch (...) {
            ATH_MSG_WARNING("Could not follow ExampleTrackContainer/MyTracks ElementLinks to ExampleHitContainer/MyHits");
         }
      }
   } else {
      ATH_MSG_INFO("Could not find ExampleTrackContainer/MyTracks");
   }
   if (evtStore()->contains<ExampleHitContainer>("MyHits")) {
      const DataHandle<ExampleHitContainer> cont;
      if (evtStore()->retrieve(cont, "MyHits").isFailure()) {
         ATH_MSG_ERROR("Could not find ExampleHitContainer/MyHits");
         return StatusCode::FAILURE;
      }
      for (ExampleHitContainer::const_iterator obj = cont->begin(); obj != cont->end(); obj++) {
         ATH_MSG_INFO("Hit x = " << (*obj)->getX() << " y = " << (*obj)->getY() << " z = " << (*obj)->getZ() << " detector = " << (*obj)->getDetector());
      }
      if (evtStore()->contains<ExampleHitContainer>("PetersHits")) {
         ATH_MSG_INFO("Found ExampleHitContainer/PetersHits (alias)");
      }
   } else {
      ATH_MSG_INFO("Could not find ExampleHitContainer/MyHits");
   }
   return StatusCode::SUCCESS;
}
//___________________________________________________________________________
StatusCode ReadData::finalize() {
   ATH_MSG_INFO("in finalize()");
   return StatusCode::SUCCESS;
}
