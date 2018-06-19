/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file ByteStreamMetadataTool.cxx
 *  @brief This file contains the implementation for the ByteStreamMetadataTool class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ByteStreamMetadataTool.cxx,v 1.4 2009-05-13 22:06:51 cranshaw Exp $
 **/

#include "ByteStreamMetadataTool.h"

//#include "GaudiKernel/IIncidentSvc.h"
//#include "GaudiKernel/FileIncident.h"

#include "ByteStreamData/ByteStreamMetadataContainer.h"
#include "StoreGate/StoreGateSvc.h"

//___________________________________________________________________________
ByteStreamMetadataTool::ByteStreamMetadataTool(const std::string& type,
	const std::string& name,
	const IInterface* parent) : AthAlgTool(type, name, parent), 
		m_pMetaDataStore("StoreGateSvc/MetaDataStore", name), 
		m_pInputStore("StoreGateSvc/InputMetaDataStore", name) {
   declareInterface<IMetaDataTool>(this);
}

//___________________________________________________________________________
ByteStreamMetadataTool::~ByteStreamMetadataTool() {
}

//___________________________________________________________________________
StatusCode ByteStreamMetadataTool::initialize() {
   ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);
   if (!::AthAlgTool::initialize().isSuccess()) {
      ATH_MSG_FATAL("Cannot initialize AthService base class.");
      return(StatusCode::FAILURE);
   }

   // locate the DetectorStore and initialize our local ptr
   StatusCode status = m_pMetaDataStore.retrieve();
   if (!status.isSuccess() || 0 == m_pMetaDataStore) {
      ATH_MSG_ERROR("Could not find MetaDataStore");
      return(status);
   }
   status = m_pInputStore.retrieve();
   if (!status.isSuccess() || 0 == m_pInputStore) {
      ATH_MSG_ERROR("Could not find InputMetaDataStore");
      return(status);
   }
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode ByteStreamMetadataTool::finalize() {
   ATH_MSG_INFO("in finalize()");
   return(::AthAlgTool::finalize());
}


StatusCode ByteStreamMetadataTool::beginInputFile()
{
   std::vector<std::string> vKeys;
   std::set<std::string> keys;
   m_pInputStore->keys<ByteStreamMetadata>(vKeys);
   keys.insert(vKeys.begin(), vKeys.end());
   m_pInputStore->keys<ByteStreamMetadataContainer>(vKeys);
   keys.insert(vKeys.begin(), vKeys.end());

   std::vector<ByteStreamMetadata*> copy;
   std::set<std::string> transGuids;
   for (std::set<std::string>::const_iterator keyIter = keys.begin(), keyEnd = keys.end();
	      keyIter != keyEnd; keyIter++) {
      ATH_MSG_DEBUG("Processing Input ByteStreamMetadata, key = " << *keyIter);
      copy.clear();
      if (m_pInputStore->contains<ByteStreamMetadata>(*keyIter)) {
         std::list<SG::ObjectWithVersion<ByteStreamMetadata> > allVersions;
         StatusCode status = m_pInputStore->retrieveAllVersions(allVersions, *keyIter);
         if (!status.isSuccess()) {
            ATH_MSG_ERROR("Could not find Input ByteStreamMetadata");
            return StatusCode::FAILURE;
         } else {
            ATH_MSG_DEBUG("Found Input ByteStreamMetadata");
         }
         for (std::list<SG::ObjectWithVersion<ByteStreamMetadata> >::const_iterator versIter =
		            allVersions.begin(), versEnd = allVersions.end(); versIter != versEnd; versIter++) {
            const DataHandle<ByteStreamMetadata> bsmd = versIter->dataObject;
            copy.push_back(new ByteStreamMetadata(*bsmd));
         }
      }
      if (m_pInputStore->contains<ByteStreamMetadataContainer>(*keyIter)) {
         std::list<SG::ObjectWithVersion<ByteStreamMetadataContainer> > allVersions;
         StatusCode status = m_pInputStore->retrieveAllVersions(allVersions, *keyIter);
         if (!status.isSuccess()) {
            ATH_MSG_ERROR("Could not find Input ByteStreamMetadataContainer");
            return StatusCode::FAILURE;
         } else {
            ATH_MSG_DEBUG("Found Input ByteStreamMetadataContainer");
         }
         for (std::list<SG::ObjectWithVersion<ByteStreamMetadataContainer> >::const_iterator versIter =
		            allVersions.begin(), versEnd = allVersions.end(); versIter != versEnd; versIter++) {
            const ByteStreamMetadataContainer* bsmdc = versIter->dataObject;
            for (ByteStreamMetadataContainer::const_iterator elemIter = bsmdc->begin(), elemEnd = bsmdc->end();
	               elemIter != elemEnd; elemIter++) {
               copy.push_back(new ByteStreamMetadata(**elemIter));
            }
         }
      }
      if (!copy.empty()) {
         transGuids.clear();
         // Check for existing container
         ByteStreamMetadataContainer* bsmdc = 0;
         if (m_pMetaDataStore->contains<ByteStreamMetadataContainer>(*keyIter)) {
            ATH_MSG_DEBUG("Pre-existing ByteStreamMetadataContainer found");
            StatusCode status = m_pMetaDataStore->retrieve(bsmdc, *keyIter);
            if (!status.isSuccess()) {
               ATH_MSG_ERROR("Could not retrieve " << *keyIter << " ByteStreamMetadataContainer");
               return StatusCode::FAILURE;
            }
            for (ByteStreamMetadataContainer::const_iterator iter = bsmdc->begin(), iterEnd = bsmdc->end();
	               iter != iterEnd; iter++) {
               transGuids.insert((*iter)->getGuid());
            }
         } else {
            bsmdc = new ByteStreamMetadataContainer;
            StatusCode status = m_pMetaDataStore->record(bsmdc, *keyIter);
            if (!status.isSuccess()) {
               ATH_MSG_ERROR("Could not store ByteStreamMetadata in Metadata store");
               return StatusCode::FAILURE;
            } else {
               ATH_MSG_DEBUG("ByteStreamMetadata copied to MetaDataStore");
            }
         }
         for (std::vector<ByteStreamMetadata*>::iterator iter = copy.begin(), iterEnd = copy.end();
	            iter != iterEnd; iter++) {
            // Only insert new metadata records (with GUID not yet in container)
            if (transGuids.insert((*iter)->getGuid()).second) {
               bsmdc->push_back(*iter);
               *iter = 0;
            } else {
               delete *iter; *iter = 0;
            }
         }
      }
   }
   return StatusCode::SUCCESS;
}


StatusCode ByteStreamMetadataTool::endInputFile()
{
   return StatusCode::SUCCESS;
}


StatusCode ByteStreamMetadataTool::metaDataStop()
{
   return StatusCode::SUCCESS;
}
