/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/** @file CopyEventStreamInfo.cxx
 *  @brief This file contains the implementation for the CopyEventStreamInfo class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: CopyEventStreamInfo.cxx,v 1.7 2009-02-09 22:48:31 gemmeren Exp $
 **/

#include "CopyEventStreamInfo.h"

#include "EventInfo/EventStreamInfo.h"
#include "StoreGate/StoreGateSvc.h"

//___________________________________________________________________________
CopyEventStreamInfo::CopyEventStreamInfo(const std::string& type,
	const std::string& name,
	const IInterface* parent) : ::AthAlgTool(type, name, parent),
                m_metaDataSvc("MetaDataSvc", name),
		m_inputMetaDataStore("StoreGateSvc/InputMetaDataStore", name) {
   // Declare IMetaDataTool interface
   declareInterface<IMetaDataTool>(this);

   // Declare the properties
   declareProperty("Key", m_key = std::string());
}
//___________________________________________________________________________
CopyEventStreamInfo::~CopyEventStreamInfo() {
}
//___________________________________________________________________________
StatusCode CopyEventStreamInfo::initialize() {
   ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);
   // Locate the MetaDataSvc and InputMetaDataStore
   if (!m_metaDataSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Could not find MetaDataSvc");
      return(StatusCode::FAILURE);
   }
   if (!m_inputMetaDataStore.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Could not find InputMetaDataStore");
      return(StatusCode::FAILURE);
   }
   return(StatusCode::SUCCESS);
}
//___________________________________________________________________________
StatusCode CopyEventStreamInfo::finalize() {
   ATH_MSG_DEBUG("in finalize()");
   // release the MetaDataSvc and InputMetaDataStore
   if (!m_metaDataSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Could not release MetaDataSvc");
   }
   if (!m_inputMetaDataStore.release().isSuccess()) {
      ATH_MSG_WARNING("Could not release InputMetaDataStore");
   }
   return(StatusCode::SUCCESS);
}


StatusCode CopyEventStreamInfo::beginInputFile(const SG::SourceID&)
{
   std::vector<std::string> keys;
   if (m_key.value().empty()) {
      m_inputMetaDataStore->keys<EventStreamInfo>(keys);
   } else {
      keys.push_back(m_key);
   }
   for (const auto &key : keys) {
      // Ignore versioned container
      if (key.substr(0, 1) == ";" && key.substr(3, 1) == ";") {
         ATH_MSG_VERBOSE( "Ignore versioned container: " << key );
         continue;
      }
      if (m_inputMetaDataStore->contains<EventStreamInfo>(key)) {
         std::list<SG::ObjectWithVersion<EventStreamInfo> > allVersions;
         if (!m_inputMetaDataStore->retrieveAllVersions(allVersions, key).isSuccess()) {
            ATH_MSG_ERROR("Could not retrieve all versions for EventStreamInfo");
            return StatusCode::FAILURE;
         }
         EventStreamInfo* evtStrInfo_out = 0;
         for (SG::ObjectWithVersion<EventStreamInfo>& obj : allVersions) {
            const EventStreamInfo* evtStrInfo_in = obj.dataObject.cptr();
            evtStrInfo_out = m_metaDataSvc->tryRetrieve<EventStreamInfo>(key);
            if( !evtStrInfo_out ) {
               auto esinfo_up = std::make_unique<EventStreamInfo>(*evtStrInfo_in);
               if( m_metaDataSvc->record( std::move(esinfo_up), key ).isFailure()) {
                  ATH_MSG_ERROR("Could not record DataObject: " << key);
                  return StatusCode::FAILURE;
               }
            } else {
               evtStrInfo_out->addEvent(evtStrInfo_in->getNumberOfEvents());
               for (auto elem = evtStrInfo_in->getRunNumbers().begin(),
                         lastElem = evtStrInfo_in->getRunNumbers().end(); 
                         elem != lastElem; elem++) {
                  evtStrInfo_out->insertRunNumber(*elem);
               }
               for (auto elem = evtStrInfo_in->getLumiBlockNumbers().begin(),
                         lastElem = evtStrInfo_in->getLumiBlockNumbers().end(); 
                         elem != lastElem; elem++) {
                  evtStrInfo_out->insertLumiBlockNumber(*elem);
               }
               for (auto elem = evtStrInfo_in->getProcessingTags().begin(),
                         lastElem = evtStrInfo_in->getProcessingTags().end(); 
                         elem != lastElem; elem++) {
                  evtStrInfo_out->insertProcessingTag(*elem);
               }
               for (auto elem = evtStrInfo_in->getItemList().begin(),
                         lastElem = evtStrInfo_in->getItemList().end(); 
                         elem != lastElem; elem++) {
                  evtStrInfo_out->insertItemList((*elem).first, (*elem).second);
               }
               for (auto elem = evtStrInfo_in->getEventTypes().begin(),
                         lastElem = evtStrInfo_in->getEventTypes().end(); 
                         elem != lastElem; elem++) {
                  evtStrInfo_out->insertEventType(*elem);
               }
            }
         }
      }
   }
   return(StatusCode::SUCCESS);
}
StatusCode CopyEventStreamInfo::endInputFile(const SG::SourceID&)
{
   return(StatusCode::SUCCESS);
}
StatusCode CopyEventStreamInfo::metaDataStop()
{
   return(StatusCode::SUCCESS);
}
