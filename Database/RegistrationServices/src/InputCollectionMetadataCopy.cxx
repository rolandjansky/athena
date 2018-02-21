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

#include "DBDataModel/CollectionMetadata.h"

#include "StoreGate/StoreGateSvc.h"

//___________________________________________________________________________
InputCollectionMetadataCopy::InputCollectionMetadataCopy(const std::string& type, const std::string& name, const IInterface* parent) : 
   AthAlgTool(type, name, parent), 
   m_pMetaDataStore ("StoreGateSvc/MetaDataStore",      name), 
   m_pInputStore    ("StoreGateSvc/TagMetaDataStore", name) {
}

//___________________________________________________________________________
InputCollectionMetadataCopy::~InputCollectionMetadataCopy() {
}

//___________________________________________________________________________
StatusCode InputCollectionMetadataCopy::initialize() {
   StatusCode sc = StatusCode::SUCCESS;
   ATH_MSG_INFO ("in initialize()");

   // locate the DetectorStore and initialize our local ptr
   sc = m_pMetaDataStore.retrieve();
   if (!sc.isSuccess() || 0 == m_pMetaDataStore) {
       ATH_MSG_ERROR ("Could not find MetaDataStore");
      return(StatusCode::FAILURE);
   }
   sc = m_pInputStore.retrieve();
   if (!sc.isSuccess() || 0 == m_pInputStore) {
      ATH_MSG_ERROR ("Could not find TagMetaDataStore");
      return(StatusCode::FAILURE);
   }
   // Set to be listener for end of event
   ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
   sc = incSvc.retrieve();
   if (!sc.isSuccess()) {
       ATH_MSG_ERROR ("Unable to get the IncidentSvc");
      return(sc);
   }
   incSvc->addListener(this, "BeginTagFile", 60); // pri has to be < 100 to be after MetaDataSvc.
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode InputCollectionMetadataCopy::finalize() {
   ATH_MSG_INFO ("in finalize()");
   ATH_MSG_DEBUG ("DEBUG in finalize()");
   return(StatusCode::SUCCESS);
}

//__________________________________________________________________________
void InputCollectionMetadataCopy::handle(const Incident& inc) {

   ATH_MSG_DEBUG ("handle() " << inc.type());

   if (inc.type() == "BeginTagFile") {

      const FileIncident* fileInc  = dynamic_cast<const FileIncident*>(&inc);
      if (fileInc == 0) {
         ATH_MSG_ERROR (" Unable to get FileName from BeginTagFile incident");
         return;
      }
      const std::string fileName = fileInc->fileName();
      ATH_MSG_DEBUG ("handle() saw BeginTagFile incident for " << fileName);
      if (m_pInputStore->contains<CollectionMetadata>("CollectionMetadata")) {
         DataHandle<CollectionMetadata> p_cmd;
         StatusCode sc = m_pInputStore->retrieve(p_cmd, "CollectionMetadata");
         if (!sc.isSuccess()) {
            ATH_MSG_ERROR ("Could not find Input CollectionMetadata");
            return;
         }
         else {
           ATH_MSG_DEBUG ("Found Input CollectionMetadata");
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
            ATH_MSG_DEBUG ("Pre-existing CollectionMetadataContainer found");
            DataHandle<CollectionMetadataContainer> list;
            StatusCode status = m_pMetaDataStore->retrieve(list,key);
            if (!sc.isSuccess()) {
              ATH_MSG_ERROR ("Could not retrieve " << key 
                             << " CollectionMetadataContainer");
            }
            else {
               list->push_back(copy);
               ATH_MSG_DEBUG ("Added CollectionMetadata to existing container for "
                              << fileInc->fileName());
            }
         }
         else
         {
            CollectionMetadataContainer* list = new CollectionMetadataContainer;
            list->push_back(copy);
            sc = m_pMetaDataStore->record(list,key);
            if (!sc.isSuccess()) {
              ATH_MSG_ERROR ("Could not store CollectionMetadata in Metadata store for file " 
                             << fileInc->fileName());
            }
            else {
              ATH_MSG_INFO ("CollectionMetadata copied to MetaDataStore for " 
                            << fileInc->fileName());
            }
         }
      }
      else {
        ATH_MSG_INFO ("No CollectionMetadata in Input Metadata store");
      }
   }
}

//__________________________________________________________________________
