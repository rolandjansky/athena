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
#include "GaudiKernel/FileIncident.h"

#include "StoreGate/StoreGateSvc.h"

using namespace AthPoolEx;

//___________________________________________________________________________
ReadMeta::ReadMeta(const std::string& type, const std::string& name, const IInterface* parent) : 
   AthAlgTool(type, name, parent), 
   m_pMetaDataStore ("StoreGateSvc/MetaDataStore",      name), 
   m_pInputStore    ("StoreGateSvc/InputMetaDataStore", name) {
   declareInterface<IMetaDataTool>(this);
}
//___________________________________________________________________________
ReadMeta::~ReadMeta() {
}
//___________________________________________________________________________
StatusCode ReadMeta::initialize() {
   ATH_MSG_INFO("in initialize()");

   // locate the DetectorStore and initialize our local ptr
   if (!m_pMetaDataStore.retrieve().isSuccess()) {
      ATH_MSG_ERROR("Could not find MetaDataStore");
      return StatusCode::FAILURE;
   }
   if (!m_pInputStore.retrieve().isSuccess()) {
      ATH_MSG_ERROR("Could not find InputMetaDataStore");
      return StatusCode::FAILURE;
   }
   // Set to be listener for end of event
   ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
   if (!incSvc.retrieve().isSuccess()) {
      ATH_MSG_ERROR("Unable to get the IncidentSvc");
      return StatusCode::FAILURE;
   }
   incSvc->addListener(this, "BeginInputFile", 60); // pri has to be < 100 to be after MetaDataSvc.
   incSvc->addListener(this, "EndFile", 50); // pri has to be > 10 to be before MetaDataSvc.
   return StatusCode::SUCCESS;
}
//___________________________________________________________________________
StatusCode ReadMeta::finalize() {
   ATH_MSG_INFO("in finalize()");
   return StatusCode::SUCCESS;
}
//__________________________________________________________________________
void ReadMeta::handle(const Incident& inc) {
   ATH_MSG_DEBUG("handle() " << inc.type());
   const FileIncident* fileInc  = dynamic_cast<const FileIncident*>(&inc);
   if (fileInc == 0) {
      ATH_MSG_ERROR(" Unable to get FileName from BeginInputFile/EndInputFile incident");
      return;
   }
   const std::string fileName = fileInc->fileName();
   ATH_MSG_DEBUG("handle() " << inc.type() << " for " << fileName);

}
//__________________________________________________________________________
StatusCode ReadMeta::beginInputFile()
{
   ATH_MSG_DEBUG("saw BeginInputFile incident.");
   if (m_pInputStore->contains<ExampleHitContainer>("PedestalWriteData")) {
      std::list<SG::ObjectWithVersion<ExampleHitContainer> > allVersions;
      if (m_pInputStore->retrieveAllVersions(allVersions, "PedestalWriteData").isFailure()) {
         ATH_MSG_ERROR("Could not retrieve all versions for PedestalWriteData");
         return StatusCode::FAILURE;
      }
      //const ExampleHitContainer* ep;
      ExampleHitContainer* ep_out = 0;
      for (std::list<SG::ObjectWithVersion<ExampleHitContainer> >::const_iterator iter = allVersions.begin(); iter != allVersions.end(); iter++) {
         const ExampleHitContainer* ep = iter->dataObject;
         if (!m_pMetaDataStore->contains<ExampleHitContainer>("PedestalWriteData")) {
            ep_out = new ExampleHitContainer();
            const ExampleHit* entry = *ep->begin();
            ExampleHit* entry_out = new ExampleHit();
            entry_out->setX(entry->getX());
            entry_out->setY(entry->getY());
            entry_out->setZ(entry->getZ());
            entry_out->setDetector(entry->getDetector());
            ep_out->push_back(entry_out);
            if (m_pMetaDataStore->record(ep_out, "PedestalWriteData").isFailure()) {
               ATH_MSG_ERROR("Could not record DataObject: PedestalWriteData");
               return StatusCode::FAILURE;
            }
         } else {
            if (m_pMetaDataStore->retrieve(ep_out, "PedestalWriteData").isFailure()) {
               ATH_MSG_ERROR("Could not find DataObject in output: PedestalWriteData");
               return StatusCode::FAILURE;
            }
            const ExampleHit* entry = *ep->begin();
            ExampleHit* entry_out = *ep_out->begin();
            int weight = entry->getDetector().size() - 2;
            int weight_out = entry_out->getDetector().size() - 2;
            entry_out->setX((entry->getX() * weight + entry_out->getX() * weight_out) / (weight + weight_out));
            entry_out->setY((entry->getY() * weight + entry_out->getY() * weight_out) / (weight + weight_out));
            entry_out->setZ((entry->getZ() * weight + entry_out->getZ() * weight_out) / (weight + weight_out));
            entry_out->setDetector(entry->getDetector().substr(0, entry->getDetector().size() - 1) + entry_out->getDetector().substr(1));
         }
      }
      if (ep_out != 0) {
         for (ExampleHitContainer::const_iterator obj = ep_out->begin(); obj != ep_out->end(); obj++) {
            ATH_MSG_INFO("Pedestal x = " << (*obj)->getX() << " y = " << (*obj)->getY() << " z = " << (*obj)->getZ() << " string = " << (*obj)->getDetector());
         }
      }
   }

   return StatusCode::SUCCESS;
}
//__________________________________________________________________________
