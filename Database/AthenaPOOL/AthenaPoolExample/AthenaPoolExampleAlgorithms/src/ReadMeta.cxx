/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file ReadMeta.cxx
 *  @brief This file contains the implementation for the ReadMeta class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ReadMeta.cxx,v 1.16 2008-12-10 21:28:11 gemmeren Exp $
 **/

#include "ReadMeta.h"

// the user data-class defintions
#include "AthenaPoolExampleData/ExampleHitContainer.h"

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/FileIncident.h"

#include "StoreGate/StoreGateSvc.h"

using namespace AthPoolEx;

//___________________________________________________________________________
ReadMeta::ReadMeta(const std::string& type, const std::string& name, const IInterface* parent) : 
   AlgTool(type, name, parent), 
   m_pMetaDataStore ("StoreGateSvc/MetaDataStore",      name), 
   m_pInputStore    ("StoreGateSvc/InputMetaDataStore", name) {
   declareInterface<IMetaDataTool>(this);
}
//___________________________________________________________________________
ReadMeta::~ReadMeta() {
}
//___________________________________________________________________________
StatusCode ReadMeta::initialize() {
   StatusCode sc = StatusCode::SUCCESS;
   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "in initialize()" << endreq;

   // locate the DetectorStore and initialize our local ptr
   sc = m_pMetaDataStore.retrieve();
   if (!sc.isSuccess() || 0 == m_pMetaDataStore) {
      log << MSG::ERROR << "Could not find MetaDataStore" << endreq;
      return(StatusCode::FAILURE);
   }
   sc = m_pInputStore.retrieve();
   if (!sc.isSuccess() || 0 == m_pInputStore) {
      log << MSG::ERROR << "Could not find InputMetaDataStore" << endreq;
      return(StatusCode::FAILURE);
   }
   // Set to be listener for end of event
   ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
   sc = incSvc.retrieve();
   if (!sc.isSuccess()) {
      log << MSG::ERROR << "Unable to get the IncidentSvc" << endreq;
      return(sc);
   }
   incSvc->addListener(this, "BeginInputFile", 60); // pri has to be < 100 to be after MetaDataSvc.
   incSvc->addListener(this, "EndFile", 50); // pri has to be > 10 to be before MetaDataSvc.
   return(StatusCode::SUCCESS);
}
//___________________________________________________________________________
StatusCode ReadMeta::finalize() {
   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "in finalize()" << endreq;
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
void ReadMeta::handle(const Incident& inc) {
   MsgStream log(msgSvc(), name());
   log << MSG::DEBUG << "handle() " << inc.type() << endreq;
   const FileIncident* fileInc  = dynamic_cast<const FileIncident*>(&inc);
   if (fileInc == 0) {
      log << MSG::ERROR << " Unable to get FileName from BeginInputFile/EndInputFile incident" << endreq;
      return;
   }
   const std::string fileName = fileInc->fileName();
   log << MSG::DEBUG << "handle() " << inc.type() << " for " << fileName << endreq;

   if (inc.type() == "BeginInputFile") {
      log << MSG::DEBUG << "handle() saw BeginInputFile incident." << endreq;
      if (m_pInputStore->contains<ExampleHitContainer>("PedestalWriteData")) {
         std::list<SG::ObjectWithVersion<ExampleHitContainer> > allVersions;
         StatusCode sc = m_pInputStore->retrieveAllVersions(allVersions, "PedestalWriteData");
         if (!sc.isSuccess()) {
            log << MSG::ERROR << "Could not retrieve all versions for PedestalWriteData" << endreq;
            return;
         }
         //const ExampleHitContainer* ep;
         ExampleHitContainer* ep_out = 0;
         for (std::list<SG::ObjectWithVersion<ExampleHitContainer> >::const_iterator iter = allVersions.begin(); iter != allVersions.end(); iter++) {
            const ExampleHitContainer* ep = iter->dataObject;
            if (!m_pMetaDataStore->contains<ExampleHitContainer>("PedestalWriteData")) {
               ep_out = new ExampleHitContainer();
               ExampleHit* entry = *ep->begin();
               ExampleHit* entry_out = new ExampleHit();
               entry_out->setX(entry->getX());
               entry_out->setY(entry->getY());
               entry_out->setZ(entry->getZ());
               entry_out->setDetector(entry->getDetector());
               ep_out->push_back(entry_out);
               StatusCode sc = m_pMetaDataStore->record(ep_out, "PedestalWriteData");
               if (!sc.isSuccess()) {
                  log << MSG::ERROR << "Could not record DataObject: PedestalWriteData" << endreq;
                  return;
               }
            } else {
               StatusCode sc = m_pMetaDataStore->retrieve(ep_out, "PedestalWriteData");
               if (!sc.isSuccess() || 0 == ep_out) {
                  log << MSG::ERROR << "Could not find DataObject in output: PedestalWriteData" << endreq;
                  return;
               }
               ExampleHit* entry = *ep->begin();
               ExampleHit* entry_out = *ep_out->begin();
               int weight = entry->getDetector().size() - 2;
               int weight_out = entry_out->getDetector().size() - 2;
               entry_out->setX((entry->getX() * weight + entry_out->getX() * weight_out) / (weight + weight_out));
               entry_out->setY((entry->getY() * weight + entry_out->getY() * weight_out) / (weight + weight_out));
               entry_out->setZ((entry->getZ() * weight + entry_out->getZ() * weight_out) / (weight + weight_out));
               entry_out->setDetector(entry->getDetector().substr(0, entry->getDetector().size() - 1) + entry_out->getDetector().substr(1));
            }
         }
         for (ExampleHitContainer::const_iterator obj = ep_out->begin(); obj != ep_out->end(); obj++) {
            log << MSG::INFO << "Pedestal x = " << (*obj)->getX() << " y = " << (*obj)->getY() << " z = " << (*obj)->getZ() << " string = " << (*obj)->getDetector() << endreq;
         }
      }
   } else if (inc.type() == "EndFile") {
      log << MSG::DEBUG << "handle() saw EndFile incident." << endreq;
   }
}
//__________________________________________________________________________
