/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file ReadData.cxx
 *  @brief This file contains the implementation for the ReadData class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ReadData.cxx,v 1.26 2009-03-14 22:05:57 gemmeren Exp $
 **/

#include "ReadData.h"

// the user data-class defintions
#include "AthenaPoolExampleData/ExampleHitContainer.h"
#include "AthenaPoolExampleData/ExampleTrackContainer.h"

#include "GaudiKernel/MsgStream.h"

#include "DBDataModel/CollectionMetadata.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventStreamInfo.h"
#include "EventBookkeeperMetaData/EventBookkeeperCollection.h"

#include "StoreGate/StoreGateSvc.h"

#include <set>

using namespace AthPoolEx;

//___________________________________________________________________________
ReadData::ReadData(const std::string& name, ISvcLocator* pSvcLocator) : Algorithm(name, pSvcLocator), p_SGevent("StoreGateSvc", name), p_SGinMeta("StoreGateSvc/InputMetaDataStore", name), p_SGtagMeta("StoreGateSvc/TagMetaDataStore", name), p_SGmeta("StoreGateSvc/MetaDataStore", name) {
}
//___________________________________________________________________________
ReadData::~ReadData() {
}
//___________________________________________________________________________
StatusCode ReadData::initialize() {
   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "in initialize()" << endreq;

   // Locate the StoreGateSvc and initialize our local ptr
   StatusCode sc = p_SGevent.retrieve();
   if (!sc.isSuccess() || 0 == p_SGevent) {
      log << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
   }

   // Locate the StoreGateSvc and initialize our local ptr
   sc = p_SGinMeta.retrieve();
   if (!sc.isSuccess() || 0 == p_SGinMeta) {
      log << MSG::ERROR << "Could not find Input MetaData StoreGateSvc" << endreq;
   }
   sc = p_SGtagMeta.retrieve();
   if (!sc.isSuccess() || 0 == p_SGtagMeta) {
      log << MSG::ERROR << "Could not find Tag MetaData StoreGateSvc" << endreq;
   }
   sc = p_SGmeta.retrieve();
   if (!sc.isSuccess() || 0 == p_SGmeta) {
      log << MSG::ERROR << "Could not find MetaData StoreGateSvc" << endreq;
   }
   return(sc);
}
//___________________________________________________________________________
StatusCode ReadData::execute() {
   StatusCode sc = StatusCode::SUCCESS;
   MsgStream log(msgSvc(), name());
   log << MSG::DEBUG << "in execute()" << endreq;

   if (p_SGtagMeta->contains<CollectionMetadata>("CollectionMetadata")) {
      const DataHandle<CollectionMetadata> cm;
      sc = p_SGtagMeta->retrieve(cm, "CollectionMetadata");
      if (sc.isFailure() || !cm) {
         log << MSG::FATAL << "Could not find CollectionMetadata" << endreq;
         return(StatusCode::FAILURE);
      }
      for (CollectionMetadata::const_iterator iter = cm->begin(), iterEnd = cm->end();
		      iter != iterEnd; iter++) {
         log << MSG::INFO << "CollectionMetadata, key = " << iter->first << ", value = " << iter->second << endreq;
      }
   }
   // Get the event streamheader, print out 
   const DataHandle<EventStreamInfo> esi1, esi2;
   sc = p_SGinMeta->retrieve(esi1, esi2);
   if (sc.isFailure() || esi1 == esi2) {
      log << MSG::WARNING << "Could not find EventStreamInfo" << endreq;
   } else {
      for (; esi1 != esi2; esi1++) {
         log << MSG::INFO << "EventStreamInfo: Number of events = " << esi1->getNumberOfEvents() << endreq;
         log << MSG::INFO << "EventStreamInfo: ItemList:" << endreq;
         for (std::set<std::pair<CLID, std::string> >::const_iterator iter = esi1->getItemList().begin(), iterEnd = esi1->getItemList().end();
		         iter != iterEnd; iter++) {
            log << MSG::INFO << "CLID = " << iter->first << ", key = " << iter->second << endreq;
         }
         for (std::set<EventType>::iterator iter = esi1->getEventTypes().begin(), iterEnd = esi1->getEventTypes().end();
		         iter != iterEnd; iter++) {
            log << MSG::INFO << "EventType: " << iter->typeToString() << endreq;
            log << MSG::INFO << "TagInfo: " << iter->get_detdescr_tags() << endreq;
         }
      }
   }
   const std::string ebcKey = "EventSelector.Counter";
   if (p_SGmeta->contains<EventBookkeeperCollection>(ebcKey)) {
      const DataHandle<EventBookkeeperCollection> ebc;
      sc = p_SGmeta->retrieve(ebc, ebcKey);
      if (sc.isFailure() || !ebc) {
         log << MSG::FATAL << "Could not find EventBookkeeperCollection, key =" << endreq;
         return(StatusCode::FAILURE);
      }
      for (EventBookkeeperCollection::const_iterator iter = ebc->begin(), iterEnd = ebc->end();
		      iter != iterEnd; iter++) {
         log << MSG::INFO << "EventBookkeeper " << (*iter)->getName() << " accepted events: = " << (*iter)->getNAcceptedEvents() << endreq;
      }
   }
   const std::string ebcInKey = "EventBookkeepers";
   if (p_SGinMeta->contains<EventBookkeeperCollection>(ebcInKey)) {
      const DataHandle<EventBookkeeperCollection> ebc;
      sc = p_SGinMeta->retrieve(ebc, ebcInKey);
      if (sc.isFailure() || !ebc) {
         log << MSG::FATAL << "Could not find EventBookkeeperCollection, key =" << endreq;
         return(StatusCode::FAILURE);
      }
      for (EventBookkeeperCollection::const_iterator iter = ebc->begin(), iterEnd = ebc->end();
		      iter != iterEnd; iter++) {
         log << MSG::INFO << "EventBookkeeper (In) " << (*iter)->getName() << " accepted events: = " << (*iter)->getNAcceptedEvents() << endreq;
      }
   }
   // Get the event header, print out event and run number
   const DataHandle<EventInfo> evt;
   sc = p_SGevent->retrieve(evt);
   if (sc.isFailure() || !evt) {
      log << MSG::FATAL << "Could not find EventInfo" << endreq;
      return(StatusCode::FAILURE);
   }
   log << MSG::INFO << "EventInfo event: " << evt->event_ID()->event_number() << " run: " << evt->event_ID()->run_number() << endreq;
   // Get the DataObject, print out its contents
   log << MSG::INFO << "Get Smart data ptr 1" << endreq;

   if (p_SGevent->contains<ExampleHitContainer>("MyHits")) {
      const DataHandle<ExampleHitContainer> cont;
      sc = p_SGevent->retrieve(cont, "MyHits");
      if (!sc.isSuccess()) {
         log << MSG::ERROR << "Could not find ExampleHitContainer/MyHits" << endreq;
         return(StatusCode::FAILURE);
      }
      for (ExampleHitContainer::const_iterator obj = cont->begin(); obj != cont->end(); obj++) {
         log << MSG::INFO << "Hit x = " << (*obj)->getX() << " y = " << (*obj)->getY() << " z = " << (*obj)->getZ() << " detector = " << (*obj)->getDetector() << endreq;
      }
      if (p_SGevent->contains<ExampleHitContainer>("PetersHits")) {
         log << MSG::INFO << "Found ExampleHitContainer/PetersHits (alias)" << endreq;
      }
   }
   if (p_SGevent->contains<ExampleTrackContainer>("MyTracks")) {
      const DataHandle<ExampleTrackContainer> cont;
      sc = p_SGevent->retrieve(cont, "MyTracks");
      if (!sc.isSuccess()) {
         log << MSG::ERROR << "Could not find ExampleTrackContainer/MyTracks" << endreq;
         return(StatusCode::FAILURE);
      }
      for (ExampleTrackContainer::const_iterator obj = cont->begin(); obj != cont->end(); obj++) {
         log << MSG::INFO << "Track pt = " << (*obj)->getPT() << " eta = " << (*obj)->getEta() << " phi = " << (*obj)->getPhi() << " detector = " << (*obj)->getDetector() << endreq;
         if (p_SGevent->contains<ExampleHitContainer>("MyHits")) {
            log << MSG::INFO << "ElementLink1 = " << (*obj)->getElement1()->getX() << endreq;
            log << MSG::INFO << "ElementLink2 = " << (*obj)->getElement2()->getX() << endreq;
            log << MSG::INFO << "Link ElementLinkVector = " << (*obj)->getElementLinkVector()->size() << endreq;
            for (ElementLinkVector<ExampleHitContainer>::const_iterator iter = (*obj)->getElementLinkVector()->begin(); iter != (*obj)->getElementLinkVector()->end(); ++iter) {
               log << MSG::INFO << "Element = " << (**iter) << " : " << (**iter)->getX() << endreq;
            }
            log << MSG::INFO << "Link Navigable = " << (*obj)->getNavigable()->size() << endreq;
            for (Navigable<ExampleHitContainer>::object_iter iter = (*obj)->getNavigable()->begin(); iter != (*obj)->getNavigable()->end(); iter++) {
               log << MSG::INFO << "Element = " << (*iter) << " : " << (*iter)->getX() << endreq;
            }
            log << MSG::INFO << "Link Weighted Navigable = " << (*obj)->getWeightedNavigable()->size() << endreq;
            for (Navigable<ExampleHitContainer, double>::object_iter iter = (*obj)->getWeightedNavigable()->begin(); iter != (*obj)->getWeightedNavigable()->end(); iter++) {
               log << MSG::INFO << "Element = " << (*iter) << " : " << (*iter)->getX() << endreq;
            }
         }
      }
   }
   return(StatusCode::SUCCESS);
}
//___________________________________________________________________________
StatusCode ReadData::finalize() {
   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "in finalize()" << endreq;
   return(StatusCode::SUCCESS);
}
