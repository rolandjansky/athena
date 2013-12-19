/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file InputCollectionMetadataCopy.cxx
 *  @brief This file contains the implementation for the InputCollectionMetadataCopy class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: InputCollectionMetadataCopy.cxx,v 1.4 2009-05-13 22:06:51 cranshaw Exp $
 **/

#include "InputCollectionMetadataCopy.h"

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/FileIncident.h"
#include "GaudiKernel/MsgStream.h"

#include "DBDataModel/CollectionMetadata.h"

#include "StoreGate/StoreGateSvc.h"

//___________________________________________________________________________
InputCollectionMetadataCopy::InputCollectionMetadataCopy(const std::string& type, const std::string& name, const IInterface* parent) : 
   AlgTool(type, name, parent), 
   m_pMetaDataStore ("StoreGateSvc/MetaDataStore",      name), 
   m_pInputStore    ("StoreGateSvc/TagMetaDataStore", name) {
   declareInterface<IMetaDataTool>(this);
}

//___________________________________________________________________________
InputCollectionMetadataCopy::~InputCollectionMetadataCopy() {
}

//___________________________________________________________________________
StatusCode InputCollectionMetadataCopy::initialize() {
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
      log << MSG::ERROR << "Could not find TagMetaDataStore" << endreq;
      return(StatusCode::FAILURE);
   }
   // Set to be listener for end of event
   ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
   sc = incSvc.retrieve();
   if (!sc.isSuccess()) {
      log << MSG::ERROR << "Unable to get the IncidentSvc" << endreq;
      return(sc);
   }
   incSvc->addListener(this, "BeginTagFile", 60); // pri has to be < 100 to be after MetaDataSvc.
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode InputCollectionMetadataCopy::finalize() {
   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "in finalize()" << endreq;
   log << MSG::DEBUG << "DEBYG in finalize()" << endreq;
   return(StatusCode::SUCCESS);
}

//__________________________________________________________________________
void InputCollectionMetadataCopy::handle(const Incident& inc) {

   MsgStream log(msgSvc(), name());

   log << MSG::DEBUG << "handle() " << inc.type() << endreq;

   if (inc.type() == "BeginTagFile") {

      const FileIncident* fileInc  = dynamic_cast<const FileIncident*>(&inc);
      if (fileInc == 0) {
         log << MSG::ERROR << " Unable to get FileName from BeginTagFile incident" << endreq;
         return;
      }
      const std::string fileName = fileInc->fileName();
      log << MSG::DEBUG << "handle() saw BeginTagFile incident for " << fileName << endreq;
      if (m_pInputStore->contains<CollectionMetadata>("CollectionMetadata")) {
         DataHandle<CollectionMetadata> p_cmd;
         StatusCode sc = m_pInputStore->retrieve(p_cmd, "CollectionMetadata");
         if (!sc.isSuccess()) {
            log << MSG::ERROR << "Could not find Input CollectionMetadata" << endreq;
            return;
         }
         else {
            log << MSG::DEBUG << "Found Input CollectionMetadata" << endreq;
         }
         // Make a copy for use in metadata store
         std::map<std::string,std::string> copyM;
         // Establish prefix based on Marcin's rules
         std::string prefix = fileInc->fileName();
         CollectionMetadata::const_iterator name_it = p_cmd->find("Name");
         if ( name_it != p_cmd->end() ) prefix = name_it->second;
         // do copy
         CollectionMetadata::const_iterator it = p_cmd->begin();
         while ( it != p_cmd->end() ) {
            std::string key = prefix + "|" + it->first;
            copyM.insert(std::make_pair(key,it->second));
            ++it;
         }
         CollectionMetadata* copy = new CollectionMetadata(copyM);

         // Check for existing container
         std::string key("Input");
         if (m_pMetaDataStore->contains<CollectionMetadataContainer>(key)) 
         {
            log << MSG::DEBUG << "Pre-existing CollectionMetadataContainer found" << endreq;
            DataHandle<CollectionMetadataContainer> list;
            StatusCode status = m_pMetaDataStore->retrieve(list,key);
            if (!sc.isSuccess()) {
               log << MSG::ERROR << "Could not retrieve " << key 
                   << " CollectionMetadataContainer" << endreq;
            }
            else {
               list->push_back(copy);
               log << MSG::DEBUG << "Added CollectionMetadata to existing container for "
                   << fileInc->fileName() << endreq;
            }
         }
         else
         {
            CollectionMetadataContainer* list = new CollectionMetadataContainer;
            list->push_back(copy);
            sc = m_pMetaDataStore->record(list,key);
            if (!sc.isSuccess()) {
               log << MSG::ERROR << "Could not store CollectionMetadata in Metadata store for file " 
                   << fileInc->fileName() << endreq;
            }
            else {
               log << MSG::INFO << "CollectionMetadata copied to MetaDataStore for " 
                   << fileInc->fileName() << endreq;
            }
         }
      }
      else {
         log << MSG::INFO << "No CollectionMetadata in Input Metadata store" << endreq;
      }
   }
}

//__________________________________________________________________________
