/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file CopyEventStreamInfo.cxx
 *  @brief This file contains the implementation for the CopyEventStreamInfo class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: CopyEventStreamInfo.cxx,v 1.7 2009-02-09 22:48:31 gemmeren Exp $
 **/

#include "CopyEventStreamInfo.h"

#include "GaudiKernel/FileIncident.h"

#include "EventInfo/EventStreamInfo.h"
#include "StoreGate/StoreGateSvc.h"

//___________________________________________________________________________
CopyEventStreamInfo::CopyEventStreamInfo(const std::string& type,
	const std::string& name,
	const IInterface* parent) : ::AthAlgTool(type, name, parent),
		m_metaDataStore("StoreGateSvc/MetaDataStore", name),
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
   // Locate the MetaDataStore and InputMetaDataStore
   if (!m_metaDataStore.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Could not find MetaDataStore");
      return(StatusCode::FAILURE);
   }
   if (!m_inputMetaDataStore.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Could not find InputMetaDataStore");
      return(StatusCode::FAILURE);
   }
   // Set to be listener for end of event
   /*
   ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
   if (incSvc.retrieve().isFailure()) {
      ATH_MSG_FATAL("Unable to get the IncidentSvc");
      return(StatusCode::FAILURE);
   }
   incSvc->addListener(this, "BeginInputFile", 60); // pri has to be < 100 to be after MetaDataSvc.
   */
   return(StatusCode::SUCCESS);
}
//___________________________________________________________________________
StatusCode CopyEventStreamInfo::finalize() {
   ATH_MSG_DEBUG("in finalize()");
   // release the MetaDataStore and InputMetaDataStore
   if (!m_metaDataStore.release().isSuccess()) {
      ATH_MSG_WARNING("Could not release MetaDataStore");
   }
   if (!m_inputMetaDataStore.release().isSuccess()) {
      ATH_MSG_WARNING("Could not release InputMetaDataStore");
   }
   return(StatusCode::SUCCESS);
}


StatusCode CopyEventStreamInfo::beginInputFile()
{
   if (m_inputMetaDataStore->contains<EventStreamInfo>(m_key)) {
      std::list<SG::ObjectWithVersion<EventStreamInfo> > allVersions;
      if (!m_inputMetaDataStore->retrieveAllVersions(allVersions, m_key).isSuccess()) {
         ATH_MSG_ERROR("Could not retrieve all versions for EventStreamInfo");
         return StatusCode::FAILURE;
      }
      EventStreamInfo* evtStrInfo_out = 0;
      for (std::list<SG::ObjectWithVersion<EventStreamInfo> >::const_iterator iter = allVersions.begin(),
		      iterEnd = allVersions.end(); iter != iterEnd; iter++) {
         const EventStreamInfo* evtStrInfo_in = iter->dataObject;
         if (!m_metaDataStore->contains<EventStreamInfo>(m_key)) {
            evtStrInfo_out = new EventStreamInfo(*evtStrInfo_in);
            if (!m_metaDataStore->record(evtStrInfo_out, m_key).isSuccess()) {
               ATH_MSG_ERROR("Could not record DataObject: " << m_key);
               return StatusCode::FAILURE;
            }
         } else {
            if (!m_metaDataStore->retrieve(evtStrInfo_out, m_key).isSuccess()) {
               ATH_MSG_ERROR("Could not find DataObject in output: " << m_key);
               return StatusCode::FAILURE;
            }
            evtStrInfo_out->addEvent(evtStrInfo_in->getNumberOfEvents());
            for (std::set<unsigned int>::const_iterator elem = evtStrInfo_in->getRunNumbers().begin(),
	               lastElem = evtStrInfo_in->getRunNumbers().end(); elem != lastElem; elem++) {
               evtStrInfo_out->insertRunNumber(*elem);
            }
            for (std::set<unsigned int>::const_iterator elem = evtStrInfo_in->getLumiBlockNumbers().begin(),
	               lastElem = evtStrInfo_in->getLumiBlockNumbers().end(); elem != lastElem; elem++) {
               evtStrInfo_out->insertLumiBlockNumber(*elem);
            }
            for (std::set<std::string>::const_iterator elem = evtStrInfo_in->getProcessingTags().begin(),
	               lastElem = evtStrInfo_in->getProcessingTags().end(); elem != lastElem; elem++) {
               evtStrInfo_out->insertProcessingTag(*elem);
            }
            for (std::set<std::pair<CLID, std::string> >::const_iterator elem = evtStrInfo_in->getItemList().begin(),
	               lastElem = evtStrInfo_in->getItemList().end(); elem != lastElem; elem++) {
               evtStrInfo_out->insertItemList((*elem).first, (*elem).second);
            }
            for (std::set<EventType>::const_iterator elem = evtStrInfo_in->getEventTypes().begin(),
	               lastElem = evtStrInfo_in->getEventTypes().end(); elem != lastElem; elem++) {
               evtStrInfo_out->insertEventType(*elem);
            }
         }
      }
   }
   return(StatusCode::SUCCESS);
}
StatusCode CopyEventStreamInfo::endInputFile()
{
   return(StatusCode::SUCCESS);
}
StatusCode CopyEventStreamInfo::metaDataStop()
{
   return(StatusCode::SUCCESS);
}
//___________________________________________________________________________
/*
void CopyEventStreamInfo::handle(const Incident& inc) {
   ATH_MSG_DEBUG("handle() " << inc.type());
   if (inc.type() == "BeginInputFile") {
      std::vector<std::string> keys;
      if (m_key.value().empty()) {
         m_inputMetaDataStore->keys<EventStreamInfo>(keys);
      } else {
         keys.push_back(m_key);
      }
      for (const auto &key : keys) {
         // Ignore versioned container
         if (key.substr(0, 1) == ";" && key.substr(3, 1) == ";") {
            ATH_MSG_VERBOSE( "Ignore versioned containe: " << key );
            continue;
         }
         ATH_MSG_VERBOSE("Attempting to copy " << key);
         if (m_inputMetaDataStore->contains<EventStreamInfo>(key)) {
            std::list<SG::ObjectWithVersion<EventStreamInfo> > allVersions;
            if (!m_inputMetaDataStore->retrieveAllVersions(allVersions, key).isSuccess()) {
               ATH_MSG_ERROR("Could not retrieve all versions for EventStreamInfo");
               return;
            }
            EventStreamInfo* evtStrInfo_out = 0;
            for (std::list<SG::ObjectWithVersion<EventStreamInfo> >::const_iterator iter = allVersions.begin(),
		            iterEnd = allVersions.end(); iter != iterEnd; iter++) {
               const EventStreamInfo* evtStrInfo_in = iter->dataObject;
               if (!m_metaDataStore->contains<EventStreamInfo>(key)) {
                  evtStrInfo_out = new EventStreamInfo(*evtStrInfo_in);
                  if (!m_metaDataStore->record(evtStrInfo_out, key).isSuccess()) {
                     ATH_MSG_ERROR("Could not record DataObject: " << key);
                     return;
                  }
               } else {
                  if (!m_metaDataStore->retrieve(evtStrInfo_out, key).isSuccess()) {
                     ATH_MSG_ERROR("Could not find DataObject in output: " << key);
                     return;
                  }
                  evtStrInfo_out->addEvent(evtStrInfo_in->getNumberOfEvents());
                  for (std::set<unsigned int>::const_iterator elem = evtStrInfo_in->getRunNumbers().begin(),
		                  lastElem = evtStrInfo_in->getRunNumbers().end(); elem != lastElem; elem++) {
                     evtStrInfo_out->insertRunNumber(*elem);
                  }
                  for (std::set<unsigned int>::const_iterator elem = evtStrInfo_in->getLumiBlockNumbers().begin(),
		                  lastElem = evtStrInfo_in->getLumiBlockNumbers().end(); elem != lastElem; elem++) {
                     evtStrInfo_out->insertLumiBlockNumber(*elem);
                  }
                  for (std::set<std::string>::const_iterator elem = evtStrInfo_in->getProcessingTags().begin(),
		                  lastElem = evtStrInfo_in->getProcessingTags().end(); elem != lastElem; elem++) {
                     evtStrInfo_out->insertProcessingTag(*elem);
                  }
                  for (std::set<std::pair<CLID, std::string> >::const_iterator elem = evtStrInfo_in->getItemList().begin(),
		                  lastElem = evtStrInfo_in->getItemList().end(); elem != lastElem; elem++) {
                     evtStrInfo_out->insertItemList((*elem).first, (*elem).second);
                  }
                  for (std::set<EventType>::const_iterator elem = evtStrInfo_in->getEventTypes().begin(),
		                  lastElem = evtStrInfo_in->getEventTypes().end(); elem != lastElem; elem++) {
                     evtStrInfo_out->insertEventType(*elem);
                  }
               }
            }
         }
      }
   }
}
*/
