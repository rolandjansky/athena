/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/** @file EventSelectorAthenaPool.cxx
 *  @brief This file contains the implementation for the EventSelectorAthenaPool class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: EventSelectorAthenaPool.cxx,v 1.226 2009-05-20 18:04:13 gemmeren Exp $
 **/

#include "EventSelectorAthenaPool.h"
#include "EventContextAthenaPool.h"
#include "PoolCollectionConverter.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "PersistentDataModel/Token.h"
#include "PersistentDataModel/TokenAddress.h"
#include "PersistentDataModel/DataHeader.h"
#include "PoolSvc/IPoolSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ActiveStoreSvc.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

#include "AthenaKernel/ICollectionSize.h"

// Framework
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/FileIncident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIoComponentMgr.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/StatusCode.h"

// Pool
#include "PersistencySvc/IPositionSeek.h"
#include "CollectionBase/ICollectionCursor.h"
#include "CollectionBase/CollectionRowBuffer.h"
#include "CollectionBase/TokenList.h"

#include <algorithm>
#include <sstream>
#include <vector>

//________________________________________________________________________________
EventSelectorAthenaPool::EventSelectorAthenaPool(const std::string& name, ISvcLocator* pSvcLocator) :
	base_class(name, pSvcLocator)
{
   declareProperty("HelperTools", m_helperTools);

   // TODO: validate if those are even used
   m_runNo.verifier().setLower(0);
   m_oldRunNo.verifier().setLower(0);
   m_eventsPerRun.verifier().setLower(0);
   m_firstEventNo.verifier().setLower(0);
   m_firstLBNo.verifier().setLower(0);
   m_eventsPerLB.verifier().setLower(0);
   m_initTimeStamp.verifier().setLower(0);

   m_inputCollectionsProp.declareUpdateHandler(&EventSelectorAthenaPool::inputCollectionsHandler, this);
}
//________________________________________________________________________________
void EventSelectorAthenaPool::inputCollectionsHandler(Gaudi::Details::PropertyBase&) {
   if (this->FSMState() != Gaudi::StateMachine::OFFLINE) {
      this->reinit().ignore();
   }
}
//________________________________________________________________________________
EventSelectorAthenaPool::~EventSelectorAthenaPool() {
}
//________________________________________________________________________________
StoreGateSvc* EventSelectorAthenaPool::eventStore() const {
   if (m_activeStoreSvc == 0) {
      if (!m_activeStoreSvc.retrieve().isSuccess()) {
         ATH_MSG_ERROR("Cannot get ActiveStoreSvc");
         throw GaudiException("Cannot get ActiveStoreSvc", name(), StatusCode::FAILURE);
      }
   }
   return(m_activeStoreSvc->operator->());
}
#include <boost/tokenizer.hpp>
#include <sstream>
//________________________________________________________________________________
StatusCode EventSelectorAthenaPool::initialize() {
   if (m_isSecondary.value()) {
      ATH_MSG_DEBUG("Initializing secondary event selector " << name());
   } else {
      ATH_MSG_DEBUG("Initializing " << name());
   }

   if (!::AthService::initialize().isSuccess()) {
      ATH_MSG_FATAL("Cannot initialize AthService base class.");
      return(StatusCode::FAILURE);
   }
   // Check for input collection
   if (m_inputCollectionsProp.value().empty()) {
      ATH_MSG_FATAL("Use the property: EventSelector.InputCollections = "
		      << "[ \"<collectionName>\" ] (list of collections)");
      return(StatusCode::FAILURE);
   }
   boost::char_separator<char> sep_coma(","), sep_hyph("-");
   boost::tokenizer  ranges(m_skipEventRangesProp.value(), sep_coma);
   for( const std::string r: ranges ) {
      boost::tokenizer  fromto(r, sep_hyph);
      auto from_iter = fromto.begin();
      std::stringstream strstr1( *from_iter );
      long from, to;
      strstr1 >> from;
      if( ++from_iter != fromto.end() ) {
         std::stringstream strstr2( *from_iter );
         strstr2 >> to;
      } else {
         to = from;
      }
      m_skipEventRanges.push_back( std::pair(from,to) );
   }

   for( auto v : m_skipEventSequenceProp.value() ) {
      m_skipEventRanges.push_back( std::pair(v,v) );
   }
   std::sort(m_skipEventRanges.begin(), m_skipEventRanges.end());
   if( msgLvl(MSG::DEBUG) ) {
      std::stringstream skip_ranges_ss;
      for( auto& r: m_skipEventRanges ) {
         if( not skip_ranges_ss.str().empty() ) skip_ranges_ss << ", ";
         skip_ranges_ss << r.first;
         if( r.first != r.second) skip_ranges_ss << "-" << r.second;
      }
      if( not skip_ranges_ss.str().empty() )
         ATH_MSG_DEBUG("Events to skip: " << skip_ranges_ss.str());
   }
   // CollectionType must be one of:
   if (m_collectionType.value() != "ExplicitROOT" && m_collectionType.value() != "ImplicitROOT") {
      ATH_MSG_FATAL("EventSelector.CollectionType must be one of: ExplicitROOT, ImplicitROOT (default)");
      return(StatusCode::FAILURE);
   }
   // Get IncidentSvc
   if (!m_incidentSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get " << m_incidentSvc.typeAndName() << ".");
      return(StatusCode::FAILURE);
   }
   // Listen to the Event Processing incidents
   m_incidentSvc->addListener(this,IncidentType::BeginProcessing,0);
   m_incidentSvc->addListener(this,IncidentType::EndProcessing,0);

   // Get AthenaPoolCnvSvc
   if (!m_athenaPoolCnvSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get " << m_athenaPoolCnvSvc.typeAndName() << ".");
      return(StatusCode::FAILURE);
   }
   // Get CounterTool (if configured)
   if (!m_counterTool.empty() && !m_counterTool.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get CounterTool.");
      return(StatusCode::FAILURE);
   }
   // Get HelperTools
   if (!m_helperTools.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get " << m_helperTools);
      return(StatusCode::FAILURE);
   }
   // Get SharedMemoryTool (if configured)
   if (!m_eventStreamingTool.empty() && !m_eventStreamingTool.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get " << m_eventStreamingTool.typeAndName() << "");
      return(StatusCode::FAILURE);
   }

   // Ensure the xAODCnvSvc is listed in the EventPersistencySvc
   ServiceHandle<IProperty> epSvc("EventPersistencySvc", name());
   std::vector<std::string> propVal;
   if (!Gaudi::Parsers::parse(propVal , epSvc->getProperty("CnvServices").toString()).isSuccess()) {
      ATH_MSG_FATAL("Cannot get EventPersistencySvc Property for CnvServices");
      return(StatusCode::FAILURE);
   }
   bool foundCnvSvc = false;
   for (const auto& property : propVal) {
      if (property == m_athenaPoolCnvSvc.type()) { foundCnvSvc = true; }
   }
   if (!foundCnvSvc) {
      propVal.push_back(m_athenaPoolCnvSvc.type());
      if (!epSvc->setProperty("CnvServices", Gaudi::Utils::toString(propVal)).isSuccess()) {
         ATH_MSG_FATAL("Cannot set EventPersistencySvc Property for CnvServices");
         return(StatusCode::FAILURE);
      }
   }

   // Register this service for 'I/O' events
   ServiceHandle<IIoComponentMgr> iomgr("IoComponentMgr", name());
   if (!iomgr.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Could not retrieve IoComponentMgr !");
      return(StatusCode::FAILURE);
   }
   if (!iomgr->io_register(this).isSuccess()) {
      ATH_MSG_FATAL("Could not register myself with the IoComponentMgr !");
      return(StatusCode::FAILURE);
   }
   // Register input file's names with the I/O manager
   const std::vector<std::string>& incol = m_inputCollectionsProp.value();
   bool allGood = true;
   std::string fileName, fileType;
   for (std::size_t icol = 0, imax = incol.size(); icol < imax; icol++) {
      if (incol[icol].substr(0, 4) == "LFN:" || incol[icol].substr(0, 4) == "FID:") {
         m_athenaPoolCnvSvc->getPoolSvc()->lookupBestPfn(incol[icol], fileName, fileType);
      } else {
         fileName = incol[icol];
      }
      if (fileName.substr(0, 4) == "PFN:") {
         fileName = fileName.substr(4);
      }
      if (!iomgr->io_register(this, IIoComponentMgr::IoMode::READ, incol[icol], fileName).isSuccess()) {
         ATH_MSG_FATAL("could not register [" << incol[icol] << "] for output !");
         allGood = false;
      } else {
         ATH_MSG_VERBOSE("io_register[" << this->name() << "](" << incol[icol] << ") [ok]");
      }
   }
   if (!allGood) {
      return(StatusCode::FAILURE);
   }

   // Connect to PersistencySvc
   if (!m_athenaPoolCnvSvc->getPoolSvc()->connect(pool::ITransaction::READ, IPoolSvc::kInputStream).isSuccess()) {
      ATH_MSG_FATAL("Cannot connect to POOL PersistencySvc.");
      return(StatusCode::FAILURE);
   }
   return(this->reinit());
}
//________________________________________________________________________________
StatusCode EventSelectorAthenaPool::reinit() {
   ATH_MSG_DEBUG("reinitialization...");

   // reset markers
   m_numEvt.resize(m_inputCollectionsProp.value().size(), -1);
   for( auto& el : m_numEvt ) el = -1;
   m_firstEvt.resize(m_inputCollectionsProp.value().size(), -1);
   for( auto& el : m_firstEvt ) el = -1;
   m_guid = Guid::null();

   // Initialize InputCollectionsIterator
   m_inputCollectionsIterator = m_inputCollectionsProp.value().begin();
   m_curCollection = 0;
   if (!m_firstEvt.empty()) {
      m_firstEvt[0] = 0;
   }
   m_evtCount = 0;
   m_headerIterator = 0;
   if (!m_eventStreamingTool.empty() && m_eventStreamingTool->isClient()) {
      ATH_MSG_INFO("Done reinitialization for shared reader client");
      return(StatusCode::SUCCESS);
   }
   bool retError = false;
   for (auto& tool : m_helperTools) {
      if (!tool->postInitialize().isSuccess()) {
         ATH_MSG_FATAL("Failed to postInitialize() " << tool->name());
         retError = true;
      }
   }
   if (retError) {
      ATH_MSG_FATAL("Failed to postInitialize() helperTools");
      return(StatusCode::FAILURE);
   }

   ATH_MSG_INFO("EventSelection with query " << m_query.value());
   // Create an m_poolCollectionConverter to read the objects in
   m_poolCollectionConverter = getCollectionCnv();
   if (m_poolCollectionConverter == nullptr) {
      ATH_MSG_INFO("No Events found in any Input Collections");
      if (m_processMetadata.value()) {
	 m_inputCollectionsIterator = m_inputCollectionsProp.value().end();
	 if (!m_inputCollectionsProp.value().empty()) m_inputCollectionsIterator--;
	//NOTE (wb may 2016): this will make the FirstInputFile incident correspond to last file in the collection ... if want it to be first file then move iterator to begin and then move above two lines below this incident firing
         if (m_collectionType.value() == "ImplicitROOT" && !m_firedIncident && !m_inputCollectionsProp.value().empty()) {
            FileIncident firstInputFileIncident(name(), "FirstInputFile", *m_inputCollectionsIterator);
            m_incidentSvc->fireIncident(firstInputFileIncident);
            m_firedIncident = true;
         }
      }
      return(StatusCode::SUCCESS);
   }
   // Check for valid header name
   if (!m_refName.value().empty()) {
      if (m_collectionType.value() == "ExplicitROOT") {
            ATH_MSG_INFO("Using collection ref name: " << m_refName.value());
      } else {
         ATH_MSG_INFO("Using implicit collection, ignore ref name: " << m_refName.value());
      }
   } else if (m_collectionType.value() == "ExplicitROOT") {
      ATH_MSG_INFO("Using standard collection ref ");
   }
   // Get DataHeader iterator
   try {
      m_headerIterator = &m_poolCollectionConverter->executeQuery();
   } catch (std::exception &e) {
      ATH_MSG_FATAL("Cannot open implicit collection - check data/software version.");
      ATH_MSG_ERROR(e.what());
      return(StatusCode::FAILURE);
   }
   while (m_headerIterator == nullptr || m_headerIterator->next() == 0) { // no selected events
      if (m_poolCollectionConverter != nullptr) {
         m_poolCollectionConverter->disconnectDb().ignore();
         delete m_poolCollectionConverter; m_poolCollectionConverter = nullptr;
      }
      m_inputCollectionsIterator++;
      m_poolCollectionConverter = getCollectionCnv();
      if (m_poolCollectionConverter != nullptr) {
         m_headerIterator = &m_poolCollectionConverter->executeQuery();
      } else {
         break;
      }
   }
   if (m_poolCollectionConverter == nullptr || m_headerIterator == nullptr) { // no event selected in any collection
      m_inputCollectionsIterator = m_inputCollectionsProp.value().begin();
      m_curCollection = 0;
      m_poolCollectionConverter = getCollectionCnv();
      if (m_poolCollectionConverter == nullptr) {
         return(StatusCode::SUCCESS);
      }
      m_headerIterator = &m_poolCollectionConverter->selectAll();
      while (m_headerIterator == nullptr || m_headerIterator->next() == 0) { // empty collection
         if (m_poolCollectionConverter != nullptr) {
            m_poolCollectionConverter->disconnectDb().ignore();
            delete m_poolCollectionConverter; m_poolCollectionConverter = nullptr;
         }
         m_inputCollectionsIterator++;
         m_poolCollectionConverter = getCollectionCnv();
         if (m_poolCollectionConverter != nullptr) {
            m_headerIterator = &m_poolCollectionConverter->selectAll();
         } else {
            break;
         }
      }
   }
   if (m_poolCollectionConverter == nullptr || m_headerIterator == nullptr) {
      return(StatusCode::SUCCESS);
   }
   Guid guid;
   int tech = 0;
   if (m_refName.value().empty()) {
      guid = m_headerIterator->eventRef().dbID();
      tech = m_headerIterator->eventRef().technology();
   } else {
      guid = m_headerIterator->currentRow().tokenList()[m_refName.value() + "_ref"].dbID();
      tech = m_headerIterator->currentRow().tokenList()[m_refName.value() + "_ref"].technology();
   }
   // Check if File is BS, for which Incident is thrown by SingleEventInputSvc
   if (tech != 0x00001000 && m_processMetadata.value() && !m_firedIncident) {
      FileIncident firstInputFileIncident(name(), "FirstInputFile", "FID:" + guid.toString(), "FID:" + guid.toString());
      m_incidentSvc->fireIncident(firstInputFileIncident);
      m_firedIncident = true;
   }
   return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode EventSelectorAthenaPool::start() {
   if (m_poolCollectionConverter != nullptr) {
      // Reset iterators and apply new query
      m_poolCollectionConverter->disconnectDb().ignore();
      delete m_poolCollectionConverter; m_poolCollectionConverter = nullptr;
   }
   m_inputCollectionsIterator = m_inputCollectionsProp.value().begin();
   m_curCollection = 0;
   if (!m_eventStreamingTool.empty() && m_eventStreamingTool->isClient()) {
      return(StatusCode::SUCCESS);
   }
   m_poolCollectionConverter = getCollectionCnv(true);
   if (m_poolCollectionConverter == nullptr) {
      ATH_MSG_INFO("No Events found in any Input Collections");
      m_inputCollectionsIterator = m_inputCollectionsProp.value().end();
      if (!m_inputCollectionsProp.value().empty()) {
         m_inputCollectionsIterator--; //leave iterator in state of last input file
      }
   } else {
      m_headerIterator = &m_poolCollectionConverter->executeQuery(/*m_query.value()*/);
   }
   delete m_endIter;   m_endIter   = nullptr;
   m_endIter = new EventContextAthenaPool(nullptr);
   return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode EventSelectorAthenaPool::stop() {
   if (!m_eventStreamingTool.empty() && m_eventStreamingTool->isClient()) {
      return(StatusCode::SUCCESS);
   }
   IEvtSelector::Context* ctxt(nullptr);
   if (!releaseContext(ctxt).isSuccess()) {
      ATH_MSG_WARNING("Cannot release context");
   }
   return(StatusCode::SUCCESS);
}

//________________________________________________________________________________
void EventSelectorAthenaPool::fireEndFileIncidents(bool isLastFile) const {
   if (m_processMetadata.value()) {
      if (m_evtCount >= 0) {
         // Assume that the end of collection file indicates the end of payload file.
         if (m_guid != Guid::null()) {
            // Fire EndInputFile incident
            FileIncident endInputFileIncident(name(), "EndInputFile", "FID:" + m_guid.toString(), m_guid.toString());
            m_incidentSvc->fireIncident(endInputFileIncident);
         }
      }
      if (isLastFile && m_firedIncident) {
         m_firedIncident = false;
      }
   }
}

//________________________________________________________________________________
StatusCode EventSelectorAthenaPool::finalize() {
   if (m_eventStreamingTool.empty() || !m_eventStreamingTool->isClient()) {
      if (!m_counterTool.empty() && !m_counterTool->preFinalize().isSuccess()) {
         ATH_MSG_WARNING("Failed to preFinalize() CounterTool");
      }
      for (auto& tool : m_helperTools) {
         if (!tool->preFinalize().isSuccess()) {
            ATH_MSG_WARNING("Failed to preFinalize() " << tool->name());
         }
      }
   }
   delete m_endIter;   m_endIter   = nullptr;
   m_headerIterator = nullptr;
   if (m_poolCollectionConverter != nullptr) {
      delete m_poolCollectionConverter; m_poolCollectionConverter = nullptr;
   }
   // Release AthenaSharedMemoryTool
   if (!m_eventStreamingTool.empty() && !m_eventStreamingTool.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release AthenaSharedMemoryTool");
   }
   // Release CounterTool
   if (!m_counterTool.empty() && !m_counterTool.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release CounterTool.");
   }
   // Release HelperTools
   if (!m_helperTools.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release " << m_helperTools);
   }
   // Release AthenaPoolCnvSvc
   if (!m_athenaPoolCnvSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release " << m_athenaPoolCnvSvc.typeAndName() << ".");
   }
   // Release IncidentSvc
   if (!m_incidentSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release " << m_incidentSvc.typeAndName() << ".");
   }
   // Finalize the Service base class.
   return(::AthService::finalize());
}

//________________________________________________________________________________
StatusCode EventSelectorAthenaPool::queryInterface(const InterfaceID& riid, void** ppvInterface) {
   if (riid == IEvtSelector::interfaceID()) {
      *ppvInterface = dynamic_cast<IEvtSelector*>(this);
   } else if (riid == IIoComponent::interfaceID()) {
      *ppvInterface = dynamic_cast<IIoComponent*>(this);
   } else if (riid == IProperty::interfaceID()) {
      *ppvInterface = dynamic_cast<IProperty*>(this);
   } else if (riid == IEvtSelectorSeek::interfaceID()) {
      *ppvInterface = dynamic_cast<IEvtSelectorSeek*>(this);
   } else if (riid == IEventShare::interfaceID()) {
      *ppvInterface = dynamic_cast<IEventShare*>(this);
   } else if (riid == ISecondaryEventSelector::interfaceID()) {
      *ppvInterface = dynamic_cast<ISecondaryEventSelector*>(this);
   } else {
      return(::AthService::queryInterface(riid, ppvInterface));
   }
   addRef();
   return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode EventSelectorAthenaPool::createContext(IEvtSelector::Context*& ctxt) const {
   ctxt = new EventContextAthenaPool(this);
   return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode EventSelectorAthenaPool::next(IEvtSelector::Context& ctxt) const {
   std::lock_guard<CallMutex> lockGuard(m_callLock);
   if (!m_eventStreamingTool.empty() && m_eventStreamingTool->isClient()) {
      void* tokenStr = nullptr;
      unsigned int status = 0;
      if (!m_eventStreamingTool->getLockedEvent(&tokenStr, status).isSuccess()) {
         ATH_MSG_FATAL("Cannot get NextEvent from AthenaSharedMemoryTool");
         delete (char*)tokenStr; tokenStr = nullptr;
         return(StatusCode::FAILURE);
      }
      if (!eventStore()->clearStore().isSuccess()) {
         ATH_MSG_WARNING("Cannot clear Store");
      }
      std::unique_ptr<AthenaAttributeList> athAttrList(new AthenaAttributeList());
      athAttrList->extend("eventRef", "string");
      (*athAttrList)["eventRef"].data<std::string>() = std::string((char*)tokenStr);
      SG::WriteHandle<AthenaAttributeList> wh(m_attrListKey.value(), eventStore()->name());
      if (!wh.record(std::move(athAttrList)).isSuccess()) {
         delete (char*)tokenStr; tokenStr = nullptr;
         ATH_MSG_ERROR("Cannot record AttributeList to StoreGate " << StoreID::storeName(eventStore()->storeID()));
         return(StatusCode::FAILURE);
      }
      Token token;
      token.fromString(std::string((char*)tokenStr));
      delete (char*)tokenStr; tokenStr = nullptr;
      Guid guid = token.dbID();
      if (guid != m_guid && m_processMetadata.value()) {
         if (m_evtCount >= 0 && m_guid != Guid::null()) {
            // Fire EndInputFile incident
            FileIncident endInputFileIncident(name(), "EndInputFile", "FID:" + m_guid.toString(), m_guid.toString());
            m_incidentSvc->fireIncident(endInputFileIncident);
         }
         m_guid = guid;
         FileIncident beginInputFileIncident(name(), "BeginInputFile", "FID:" + m_guid.toString(), m_guid.toString());
         m_incidentSvc->fireIncident(beginInputFileIncident);
      }
      return(StatusCode::SUCCESS);
   }
   for (const auto& tool : m_helperTools) {
      if (!tool->preNext().isSuccess()) {
         ATH_MSG_WARNING("Failed to preNext() " << tool->name());
      }
   }
   for (;;) {
      // Handle possible file transition
      StatusCode sc = nextHandleFileTransition(ctxt);
      if (sc.isRecoverable()) {
        continue; // handles empty files
      }
      if (sc.isFailure()) {
         return StatusCode::FAILURE;
      }
      // Increase event count
      ++m_evtCount;
      if (!m_counterTool.empty() && !m_counterTool->preNext().isSuccess()) {
         ATH_MSG_WARNING("Failed to preNext() CounterTool.");
      }
      if( m_evtCount > m_skipEvents
          && (m_skipEventRanges.empty() || m_evtCount < m_skipEventRanges.front().first))
      {
         if (!m_eventStreamingTool.empty() && m_eventStreamingTool->isServer()) {
            std::string token = m_headerIterator->eventRef().toString();
            StatusCode sc = m_eventStreamingTool->putEvent(m_evtCount - 1, token.c_str(), token.length() + 1, 0);
            while (sc.isRecoverable()) {
               while (m_athenaPoolCnvSvc->readData().isSuccess()) {
                  ATH_MSG_VERBOSE("Called last readData, while putting next event in next()");
               }
               // Nothing to do right now, trigger alternative (e.g. caching) here? Currently just fast loop.
               sc = m_eventStreamingTool->putEvent(m_evtCount - 1, token.c_str(), token.length() + 1, 0);
            }
            if (!sc.isSuccess()) {
               ATH_MSG_ERROR("Cannot put Event " << m_evtCount - 1 << " to AthenaSharedMemoryTool");
               return(StatusCode::FAILURE);
            }
         } else {
            if (!m_isSecondary.value()) {
               if (!eventStore()->clearStore().isSuccess()) {
                  ATH_MSG_WARNING("Cannot clear Store");
               }
               if (!recordAttributeList().isSuccess()) {
                  ATH_MSG_ERROR("Failed to record AttributeList.");
                  return(StatusCode::FAILURE);
               }
            }
         }
         StatusCode status = StatusCode::SUCCESS;
         for (const auto& tool : m_helperTools) {
            StatusCode toolStatus = tool->postNext();
            if (toolStatus.isRecoverable()) {
               ATH_MSG_INFO("Request skipping event from: " << tool->name());
               if (status.isSuccess()) {
                  status = StatusCode::RECOVERABLE;
               }
            } else if (toolStatus.isFailure()) {
               ATH_MSG_WARNING("Failed to postNext() " << tool->name());
               status = StatusCode::FAILURE;
            }
         }
         if (status.isRecoverable()) {
            ATH_MSG_INFO("skipping event " << m_evtCount);
         } else if (status.isFailure()) {
            ATH_MSG_WARNING("Failed to postNext() HelperTool.");
         } else {
            if (!m_counterTool.empty() && !m_counterTool->postNext().isSuccess()) {
               ATH_MSG_WARNING("Failed to postNext() CounterTool.");
            }
            break;
         }
      } else {
         while( !m_skipEventRanges.empty() && m_evtCount >= m_skipEventRanges.front().second ) {
            m_skipEventRanges.erase(m_skipEventRanges.begin());
         }
         ATH_MSG_INFO("skipping event " << m_evtCount);
      }
   }
   return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode EventSelectorAthenaPool::next(IEvtSelector::Context& ctxt, int jump) const {
   if (jump > 0) {
      for (int i = 0; i < jump; i++) {
         ATH_CHECK(next(ctxt));
      }
      return(StatusCode::SUCCESS);
   }
   return(StatusCode::FAILURE);
}
//________________________________________________________________________________
StatusCode EventSelectorAthenaPool::nextHandleFileTransition(IEvtSelector::Context& ctxt) const
{
   // Check if we're at the end of file
   if (m_headerIterator == nullptr || m_headerIterator->next() == 0) {
      m_headerIterator = nullptr;
      // Close previous collection.
      delete m_poolCollectionConverter; m_poolCollectionConverter = nullptr;

      // zero the current DB ID (m_guid) before disconnect() to indicate it is no longer in use
      const SG::SourceID old_guid = m_guid.toString();
      m_guid = Guid::null();
      disconnectIfFinished( old_guid );

      // Open next file from inputCollections list.
      m_inputCollectionsIterator++;
      // Create PoolCollectionConverter for input file
      m_poolCollectionConverter = getCollectionCnv(true);
      if (m_poolCollectionConverter == nullptr) {
         // Return end iterator
         ctxt = *m_endIter;
         // This is not a real failure but a Gaudi way of handling "end of job"
         return StatusCode::FAILURE;
      }
      // Get DataHeader iterator
      m_headerIterator = &m_poolCollectionConverter->executeQuery();

      // Return RECOVERABLE to mark we should still continue
      return StatusCode::RECOVERABLE;
   }

   Guid guid;
   int tech = 0;
   if (m_refName.value().empty()) {
      guid = m_headerIterator->eventRef().dbID();
      tech = m_headerIterator->eventRef().technology();
   } else {
      Token token;
      m_headerIterator->currentRow().tokenList()[m_refName.value() + "_ref"].setData(&token);
      guid = token.dbID();
      tech = token.technology();
   }
   if (guid != m_guid) {
      // we are starting reading from a new DB. Check if the old one needs to be retired
      if (m_guid != Guid::null()) {
         // zero the current DB ID (m_guid) before trying disconnect() to indicate it is no longer in use
         const SG::SourceID old_guid = m_guid.toString();
         m_guid = Guid::null();
         disconnectIfFinished( old_guid );
      }
      m_guid = guid;
      // Fire BeginInputFile incident if current InputCollection is a payload file;
      // otherwise, ascertain whether the pointed-to file is reachable before firing any incidents and/or proceeding
      if (m_collectionType.value() == "ImplicitROOT") {
         // For now, we can only deal with input metadata from POOL files, but we know we have a POOL file here
         if (!m_athenaPoolCnvSvc->setInputAttributes(*m_inputCollectionsIterator).isSuccess()) {
               ATH_MSG_ERROR("Failed to set input attributes.");
               return(StatusCode::FAILURE);
         }
         if (m_processMetadata.value()) {
            FileIncident beginInputFileIncident(name(), "BeginInputFile", *m_inputCollectionsIterator, "FID:" + m_guid.toString());
            m_incidentSvc->fireIncident(beginInputFileIncident);
         }
      } else {
         // Check if File is BS
         if (tech != 0x00001000 && m_processMetadata.value()) {
            FileIncident beginInputFileIncident(name(), "BeginInputFile", "FID:" + m_guid.toString());
            m_incidentSvc->fireIncident(beginInputFileIncident);
         }
      }
   }  // end if (guid != m_guid)
   return StatusCode::SUCCESS;
}
//________________________________________________________________________________
StatusCode EventSelectorAthenaPool::previous(IEvtSelector::Context& /*ctxt*/) const {
   ATH_MSG_ERROR("previous() not implemented");
   return(StatusCode::FAILURE);
}
//________________________________________________________________________________
StatusCode EventSelectorAthenaPool::previous(IEvtSelector::Context& ctxt, int jump) const {
   if (jump > 0) {
      for (int i = 0; i < jump; i++) {
         ATH_CHECK(previous(ctxt));
      }
      return(StatusCode::SUCCESS);
   }
   return(StatusCode::FAILURE);
}
//________________________________________________________________________________
StatusCode EventSelectorAthenaPool::last(IEvtSelector::Context& ctxt) const {
   if (ctxt.identifier() == m_endIter->identifier()) {
      ATH_MSG_DEBUG("last(): Last event in InputStream.");
      return(StatusCode::SUCCESS);
   }
   return(StatusCode::FAILURE);
}
//________________________________________________________________________________
StatusCode EventSelectorAthenaPool::rewind(IEvtSelector::Context& /*ctxt*/) const {
   ATH_MSG_ERROR("EventSelectorAthenaPool::rewind() not implemented");
   return(StatusCode::FAILURE);
}
//________________________________________________________________________________
StatusCode EventSelectorAthenaPool::createAddress(const IEvtSelector::Context& /*ctxt*/,
		IOpaqueAddress*& iop) const {
   std::string tokenStr;
   SG::ReadHandle<AthenaAttributeList> attrList(m_attrListKey.value(), eventStore()->name());
   if (attrList.isValid()) {
      try {
         if (m_refName.value().empty()) {
            tokenStr = (*attrList)["eventRef"].data<std::string>();
            ATH_MSG_DEBUG("found AthenaAttribute, name = eventRef = " << tokenStr);
         } else {
            tokenStr = (*attrList)[m_refName.value() + "_ref"].data<std::string>();
            ATH_MSG_DEBUG("found AthenaAttribute, name = " << m_refName.value() << "_ref = " << tokenStr);
         }
      } catch (std::exception &e) {
         ATH_MSG_ERROR(e.what());
         return(StatusCode::FAILURE);
      }
   } else {
      ATH_MSG_WARNING("Cannot find AthenaAttribute, key = " << m_attrListKey.value());
      tokenStr = m_poolCollectionConverter->retrieveToken(m_headerIterator, m_refName.value());
   }
   Token* token = new Token;
   token->fromString(tokenStr);
   iop = new TokenAddress(POOL_StorageType, ClassID_traits<DataHeader>::ID(), "", "EventSelector", IPoolSvc::kInputStream, token);
   return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode EventSelectorAthenaPool::releaseContext(IEvtSelector::Context*& /*ctxt*/) const {
   return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode EventSelectorAthenaPool::resetCriteria(const std::string& /*criteria*/,
		IEvtSelector::Context& /*ctxt*/) const {
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode EventSelectorAthenaPool::seek(Context& /*ctxt*/, int evtNum) const {
   long newColl = findEvent(evtNum);
   if (newColl == -1 && evtNum >= m_firstEvt[m_curCollection] && evtNum < m_evtCount - 1) {
      newColl = m_curCollection;
   }
   if (newColl == -1) {
      m_headerIterator = nullptr;
      ATH_MSG_INFO("seek: Reached end of Input.");
      fireEndFileIncidents(true);
      return(StatusCode::RECOVERABLE);
   }
   if (newColl != m_curCollection) {
      if (!m_keepInputFilesOpen.value() && m_poolCollectionConverter != nullptr) {
         m_poolCollectionConverter->disconnectDb().ignore();
      }
      delete m_poolCollectionConverter; m_poolCollectionConverter = nullptr;
      m_curCollection = newColl;
      try {
         ATH_MSG_DEBUG("Seek to item: \""
	         <<  m_inputCollectionsProp.value()[m_curCollection]
	         << "\" from the collection list.");
         // Reset input collection iterator to the right place
         m_inputCollectionsIterator = m_inputCollectionsProp.value().begin();
         m_inputCollectionsIterator += m_curCollection;
         m_poolCollectionConverter = new PoolCollectionConverter(m_collectionType.value() + ":" + m_collectionTree.value(),
	         m_inputCollectionsProp.value()[m_curCollection],
	         m_query.value(),
	         m_athenaPoolCnvSvc->getPoolSvc());
         if (!m_poolCollectionConverter->initialize().isSuccess()) {
            m_headerIterator = nullptr;
            ATH_MSG_ERROR("seek: Unable to initialize PoolCollectionConverter.");
            return(StatusCode::FAILURE);
         }
         // Create DataHeader iterators
         m_headerIterator = &m_poolCollectionConverter->executeQuery();
         EventContextAthenaPool* beginIter = new EventContextAthenaPool(this);
         next(*beginIter).ignore();
         ATH_MSG_DEBUG("Token " << m_headerIterator->eventRef().toString());
      } catch (std::exception &e) {
         m_headerIterator = nullptr;
         ATH_MSG_ERROR(e.what());
         return(StatusCode::FAILURE);
      }
   }

   pool::IPositionSeek* is = dynamic_cast<pool::IPositionSeek*>(m_headerIterator);
   if (is == nullptr) {
      ATH_MSG_ERROR("Container does not allow seeking.");
      return(StatusCode::FAILURE);
   }
   if (is->seek(evtNum - m_firstEvt[m_curCollection]) == 0) {
      m_headerIterator = nullptr;
      ATH_MSG_ERROR("Did not find event, evtNum = " << evtNum);
      return(StatusCode::FAILURE);
   } else {
      m_evtCount = evtNum + 1;
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
int EventSelectorAthenaPool::curEvent (const Context& /*ctxt*/) const {
  return(m_evtCount);
}
//__________________________________________________________________________
// Search for event number evtNum.
// Return the index of the collection containing it, or -1 if not found.
// Note: passing -1 for evtNum will always yield failure,
// but this can be used to force filling in the entire m_numEvt array.
int EventSelectorAthenaPool::findEvent(int evtNum) const {
   for (std::size_t i = 0, imax = m_numEvt.size(); i < imax; i++) {
      if (m_numEvt[i] == -1) {
         PoolCollectionConverter pcc(m_collectionType.value() + ":" + m_collectionTree.value(),
	         m_inputCollectionsProp.value()[i],
	         m_query.value(),
	         m_athenaPoolCnvSvc->getPoolSvc());
         if (!pcc.initialize().isSuccess()) {
            break;
         }
         int collection_size = 0;
         if (pcc.isValid()) {
            pool::ICollectionCursor* hi = &pcc.executeQuery();
            ICollectionSize* cs = dynamic_cast<ICollectionSize*>(hi);
            if (cs == nullptr) {
               break;
            }
            collection_size = cs->size();
         }
         if (i > 0) {
            m_firstEvt[i] = m_firstEvt[i - 1] + m_numEvt[i - 1];
         } else {
            m_firstEvt[i] = 0;
         }
         m_numEvt[i] = collection_size;
      }
      if (evtNum >= m_firstEvt[i] && evtNum < m_firstEvt[i] + m_numEvt[i]) {
         return(i);
      }
   }
   return(-1);
}

//________________________________________________________________________________
StatusCode EventSelectorAthenaPool::makeServer(int num) {
   if (num < 0) {
      if (m_athenaPoolCnvSvc->makeServer(num - 1).isFailure()) {
         ATH_MSG_ERROR("Failed to switch AthenaPoolCnvSvc to output DataStreaming server");
      }
      return(StatusCode::SUCCESS);
   }
   if (m_athenaPoolCnvSvc->makeServer(num + 1).isFailure()) {
      ATH_MSG_ERROR("Failed to switch AthenaPoolCnvSvc to input DataStreaming server");
      return(StatusCode::FAILURE);
   }
   if (m_eventStreamingTool.empty()) {
      return(StatusCode::SUCCESS);
   }
   m_processMetadata = false;
   ATH_MSG_DEBUG("makeServer: " << m_eventStreamingTool << " = " << num);
   return(m_eventStreamingTool->makeServer(1));
}

//________________________________________________________________________________
StatusCode EventSelectorAthenaPool::makeClient(int num) {
   if (m_athenaPoolCnvSvc->makeClient(num + 1).isFailure()) {
      ATH_MSG_ERROR("Failed to switch AthenaPoolCnvSvc to DataStreaming client");
      return(StatusCode::FAILURE);
   }
   if (m_eventStreamingTool.empty()) {
      return(StatusCode::SUCCESS);
   }
   ATH_MSG_DEBUG("makeClient: " << m_eventStreamingTool << " = " << num);
   return(m_eventStreamingTool->makeClient(0));
}

//________________________________________________________________________________
StatusCode EventSelectorAthenaPool::share(int evtnum) {
   if (!m_eventStreamingTool.empty() && m_eventStreamingTool->isClient()) {
      StatusCode sc = m_eventStreamingTool->lockEvent(evtnum);
      while (sc.isRecoverable()) {
         usleep(1000);
         sc = m_eventStreamingTool->lockEvent(evtnum);
      }
// Send stop client and wait for restart
      if (sc.isFailure()) {
         if (m_athenaPoolCnvSvc->makeClient(0).isFailure()) {
            return(StatusCode::FAILURE);
         }
         sc = m_eventStreamingTool->lockEvent(evtnum);
         while (sc.isRecoverable() || sc.isFailure()) {
            usleep(1000);
            sc = m_eventStreamingTool->lockEvent(evtnum);
         }
//FIXME
         if (m_athenaPoolCnvSvc->makeClient(1).isFailure()) {
            return(StatusCode::FAILURE);
         }
      }
      return(sc);
   }
   return(StatusCode::FAILURE);
}

//________________________________________________________________________________
StatusCode EventSelectorAthenaPool::readEvent(int maxevt) {
   if (m_eventStreamingTool.empty()) {
      ATH_MSG_ERROR("No AthenaSharedMemoryTool configured for readEvent()");
      return(StatusCode::FAILURE);
   }
   ATH_MSG_VERBOSE("Called read Event " << maxevt);
   IEvtSelector::Context* ctxt = new EventContextAthenaPool(this);
   for (int i = 0; i < maxevt || maxevt == -1; ++i) {
      if (!next(*ctxt).isSuccess()) {
         if (m_evtCount == -1) {
            ATH_MSG_VERBOSE("Called read Event and read last event from input: " << i);
            break;
         }
         ATH_MSG_ERROR("Cannot read Event " << m_evtCount - 1 << " into AthenaSharedMemoryTool");
         delete ctxt; ctxt = nullptr;
         return(StatusCode::FAILURE);
      } else {
         ATH_MSG_VERBOSE("Called next, read Event " << m_evtCount - 1);
      }
   }
   delete ctxt; ctxt = nullptr;
   // End of file, wait for last event to be taken
   StatusCode sc = m_eventStreamingTool->putEvent(0, 0, 0, 0);
   while (sc.isRecoverable()) {
      while (m_athenaPoolCnvSvc->readData().isSuccess()) {
         ATH_MSG_VERBOSE("Called last readData, while marking last event in readEvent()");
      }
      usleep(1000);
      sc = m_eventStreamingTool->putEvent(0, 0, 0, 0);
   }
   if (!sc.isSuccess()) {
      ATH_MSG_ERROR("Cannot put last Event marker to AthenaSharedMemoryTool");
      return(StatusCode::FAILURE);
   } else {
      sc = m_athenaPoolCnvSvc->readData();
      while (sc.isSuccess() || sc.isRecoverable()) {
         sc = m_athenaPoolCnvSvc->readData();
      }
      ATH_MSG_DEBUG("Failed last readData -> Clients are stopped, after marking last event in readEvent()");
   }
   return(StatusCode::SUCCESS);
}

//__________________________________________________________________________
int EventSelectorAthenaPool::size(Context& /*ctxt*/) const {
   // Fetch sizes of all collections.
   findEvent(-1);
   int sz = 0;
   for (std::size_t i = 0, imax = m_numEvt.size(); i < imax; i++) {
      sz += m_numEvt[i];
   }
   return(sz);
}
//__________________________________________________________________________
PoolCollectionConverter* EventSelectorAthenaPool::getCollectionCnv(bool throwIncidents) const {
   while (m_inputCollectionsIterator != m_inputCollectionsProp.value().end()) {
      if (m_curCollection != 0) {
         m_numEvt[m_curCollection] = m_evtCount - m_firstEvt[m_curCollection];
         m_curCollection++;
         m_firstEvt[m_curCollection] = m_evtCount;
      }
      ATH_MSG_DEBUG("Try item: \"" << *m_inputCollectionsIterator << "\" from the collection list.");
      PoolCollectionConverter* pCollCnv = new PoolCollectionConverter(m_collectionType.value() + ":" + m_collectionTree.value(),
	      *m_inputCollectionsIterator,
	      m_query.value(),
	      m_athenaPoolCnvSvc->getPoolSvc());
      StatusCode status = pCollCnv->initialize();
      if (!status.isSuccess()) {
         // Close previous collection.
         delete pCollCnv; pCollCnv = nullptr;
         if (!status.isRecoverable()) {
            ATH_MSG_ERROR("Unable to initialize PoolCollectionConverter.");
            throw GaudiException("Unable to read: " + *m_inputCollectionsIterator, name(), StatusCode::FAILURE);
         } else {
            ATH_MSG_ERROR("Unable to open: " << *m_inputCollectionsIterator);
            throw GaudiException("Unable to open: " + *m_inputCollectionsIterator, name(), StatusCode::FAILURE);
         }
      } else {
         if (!pCollCnv->isValid().isSuccess()) {
            delete pCollCnv; pCollCnv = nullptr;
            ATH_MSG_DEBUG("No events found in: " << *m_inputCollectionsIterator << " skipped!!!");
            if (throwIncidents && m_processMetadata.value()) {
               FileIncident beginInputFileIncident(name(), "BeginInputFile", *m_inputCollectionsIterator);
               m_incidentSvc->fireIncident(beginInputFileIncident);
               FileIncident endInputFileIncident(name(), "EndInputFile", "eventless " + *m_inputCollectionsIterator);
               m_incidentSvc->fireIncident(endInputFileIncident);
            }
            m_athenaPoolCnvSvc->getPoolSvc()->disconnectDb(*m_inputCollectionsIterator).ignore();
            m_inputCollectionsIterator++;
         } else {
            return(pCollCnv);
         }
      }
   }
   return(nullptr);
}
//__________________________________________________________________________
StatusCode EventSelectorAthenaPool::recordAttributeList() const {
   // Get access to AttributeList
   ATH_MSG_DEBUG("Get AttributeList from the collection");
   // MN: accessing only attribute list, ignoring token list
   const coral::AttributeList& attrList = m_headerIterator->currentRow().attributeList();
   ATH_MSG_DEBUG("AttributeList size " << attrList.size());
   std::unique_ptr<AthenaAttributeList> athAttrList(new AthenaAttributeList(attrList));
   // Fill the new attribute list
   ATH_CHECK(fillAttributeList(athAttrList.get(), "", false));
   // Write the AttributeList
   SG::WriteHandle<AthenaAttributeList> wh(m_attrListKey.value(), eventStore()->name());
   if (!wh.record(std::move(athAttrList)).isSuccess()) {
      ATH_MSG_ERROR("Cannot record AttributeList to StoreGate " << StoreID::storeName(eventStore()->storeID()));
      return(StatusCode::FAILURE);
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode EventSelectorAthenaPool::fillAttributeList(coral::AttributeList *attrList, const std::string &suffix, bool copySource) const
{
   const pool::TokenList& tokenList = m_headerIterator->currentRow().tokenList();
   for (pool::TokenList::const_iterator iter = tokenList.begin(), last = tokenList.end(); iter != last; ++iter) {
      attrList->extend(iter.tokenName() + suffix, "string");
      (*attrList)[iter.tokenName() + suffix].data<std::string>() = iter->toString();
      ATH_MSG_DEBUG("record AthenaAttribute, name = " << iter.tokenName() + suffix << " = " << iter->toString() << ".");
   }

   std::string eventRef = "eventRef";
   if (m_isSecondary.value()) {
      eventRef.append(suffix);
   }
   attrList->extend(eventRef, "string");
   (*attrList)[eventRef].data<std::string>() = m_headerIterator->eventRef().toString();
   ATH_MSG_DEBUG("record AthenaAttribute, name = " + eventRef + " = " <<  m_headerIterator->eventRef().toString() << ".");

   if (copySource) {
      const coral::AttributeList& sourceAttrList = m_headerIterator->currentRow().attributeList();
      for (const auto &attr : sourceAttrList) {
         attrList->extend(attr.specification().name() + suffix, attr.specification().type());
         (*attrList)[attr.specification().name() + suffix] = attr;
      }
   }

   return StatusCode::SUCCESS;
}
//__________________________________________________________________________
StatusCode EventSelectorAthenaPool::io_reinit() {
   ATH_MSG_INFO("I/O reinitialization...");
   if (m_poolCollectionConverter != nullptr) {
      m_poolCollectionConverter->disconnectDb().ignore();
      delete m_poolCollectionConverter; m_poolCollectionConverter = nullptr;
   }
   m_headerIterator = nullptr;
   ServiceHandle<IIoComponentMgr> iomgr("IoComponentMgr", name());
   if (!iomgr.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Could not retrieve IoComponentMgr !");
      return(StatusCode::FAILURE);
   }
   if (!iomgr->io_hasitem(this)) {
      ATH_MSG_FATAL("IoComponentMgr does not know about myself !");
      return(StatusCode::FAILURE);
   }
   if (!m_eventStreamingTool.empty() && m_eventStreamingTool->isClient()) {
      return(this->reinit());
   }
   std::vector<std::string> inputCollections = m_inputCollectionsProp.value();
   std::set<std::size_t> updatedIndexes;
   for (std::size_t i = 0, imax = m_inputCollectionsProp.value().size(); i < imax; i++) {
      if (updatedIndexes.find(i) != updatedIndexes.end()) continue;
      std::string savedName = inputCollections[i];
      std::string &fname = inputCollections[i];
      if (!iomgr->io_contains(this, fname)) {
         ATH_MSG_ERROR("IoComponentMgr does not know about [" << fname << "] !");
         return(StatusCode::FAILURE);
      }
      if (!iomgr->io_retrieve(this, fname).isSuccess()) {
         ATH_MSG_FATAL("Could not retrieve new value for [" << fname << "] !");
         return(StatusCode::FAILURE);
      }
      if (savedName != fname) {
         ATH_MSG_DEBUG("Mapping value for [" << savedName << "] to [" << fname << "]");
         m_athenaPoolCnvSvc->getPoolSvc()->renamePfn(savedName, fname);
      }
      updatedIndexes.insert(i);
      for (std::size_t j = i + 1; j < imax; j++) {
         if (inputCollections[j] == savedName) {
            inputCollections[j] = fname;
            updatedIndexes.insert(j);
         }
      }
   }
   // all good... copy over.
   m_inputCollectionsProp = inputCollections; // triggers reinit()
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode EventSelectorAthenaPool::io_finalize() {
   ATH_MSG_INFO("I/O finalization...");
   if (m_poolCollectionConverter != nullptr) {
      m_poolCollectionConverter->disconnectDb().ignore();
      delete m_poolCollectionConverter; m_poolCollectionConverter = nullptr;
   }
   return(StatusCode::SUCCESS);
}

//__________________________________________________________________________
/* Listen to IncidentType::BeginProcessing and EndProcessing
   Maintain counters of how many events from a given file are being processed.
   Files are identified by SG::SourceID (string GUID).
   When there are no more events from a file, see if it can be closed.
*/
void EventSelectorAthenaPool::handle(const Incident& inc)
{
   SG::SourceID fid;
   if ( Atlas::hasExtendedEventContext(inc.context()) ) {
     fid = Atlas::getExtendedEventContext(inc.context()).proxy()->sourceID();
   }
   if( fid.empty() ) {
      ATH_MSG_WARNING("could not read event source ID from incident event context");
      return;
   }

   ATH_MSG_DEBUG("**  MN Incident handler " << inc.type() << " Event source ID=" << fid );
   if( inc.type() == IncidentType::BeginProcessing ) {
      // increment the events-per-file counter for FID
      m_activeEventsPerSource[fid]++;
   } else if( inc.type() == IncidentType::EndProcessing ) {
      m_activeEventsPerSource[fid]--;
      disconnectIfFinished( fid );
   }
   if( msgLvl(MSG::DEBUG) ) {
      for( auto& source: m_activeEventsPerSource )
         msg(MSG::DEBUG) << "SourceID: " << source.first << " active events: " << source.second << endmsg;
   }
}

//__________________________________________________________________________
/* Disconnect APR Database identifieed by a SG::SourceID when it is no longer in use:
   m_guid is not pointing to it and there are no events from it being processed
   (if the EventLoopMgr was not firing Begin/End incidents, this will just close the DB)
*/
bool EventSelectorAthenaPool::disconnectIfFinished( const SG::SourceID &fid ) const
{
   if( m_activeEventsPerSource[fid] <= 0 && m_guid != fid ) {
      // Explicitly disconnect file corresponding to old FID to release memory
      if( !m_keepInputFilesOpen.value() ) {
         // Assume that the end of collection file indicates the end of payload file.
         if (m_processMetadata.value()) {
            FileIncident endInputFileIncident(name(), "EndInputFile", "FID:" + fid, fid);
            m_incidentSvc->fireIncident(endInputFileIncident);
         }
         ATH_MSG_INFO("Disconnecting input sourceID: " << fid );
         m_athenaPoolCnvSvc->getPoolSvc()->disconnectDb("FID:" + fid, IPoolSvc::kInputStream).ignore();
         m_activeEventsPerSource.erase( fid );
         return true;
      }
   }
   return false;
}

//__________________________________________________________________________
void EventSelectorAthenaPool::syncEventCount(int count) const
{
   m_evtCount = count;
}
