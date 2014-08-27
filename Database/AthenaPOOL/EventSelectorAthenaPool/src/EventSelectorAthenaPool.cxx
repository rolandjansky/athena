/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file EventSelectorAthenaPool.cxx
 *  @brief This file contains the implementation for the EventSelectorAthenaPool class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: EventSelectorAthenaPool.cxx,v 1.226 2009-05-20 18:04:13 gemmeren Exp $
 **/

#include "EventSelectorAthenaPool.h"
#include "EventContextAthenaPool.h"
#include "PoolCollectionConverter.h"
#include "AthenaKernel/IAthenaIPCTool.h"
#include "registerKeys.h"

#include "AthenaPoolCnvSvc/IAthenaPoolCnvSvc.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "DBDataModel/CollectionMetadata.h"
#include "PersistentDataModel/Token.h"
#include "PersistentDataModel/DataHeader.h"
#include "PoolSvc/IPoolSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ActiveStoreSvc.h"

#include "AthenaKernel/ICollectionSize.h"
#include "AthenaKernel/IEventSeek.h"
#include "AthenaKernel/IClassIDSvc.h"

// Framework
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/FileIncident.h"
#include "GaudiKernel/IChronoStatSvc.h"
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
	::AthService(name, pSvcLocator),
	m_beginIter(0),
	m_endIter(0),
	m_activeStoreSvc("ActiveStoreSvc", name),
	m_tagDataStore("StoreGateSvc/TagMetaDataStore", name),
	m_poolCollectionConverter(0),
	m_headerIterator(0),
	m_guid(),
	m_athenaPoolCnvSvc("AthenaPoolCnvSvc", name),
	m_chronoStatSvc("ChronoStatSvc", name),
	m_clidSvc("ClassIDSvc", name),
	m_incidentSvc("IncidentSvc", name),
	m_helperTools(this),
	m_counterTool("", this),
	m_sharedMemoryTool("", this),
	m_evtCount(0),
	m_firedIncident(false) {
   declareProperty("BackNavigation",      m_backNavigationFlag = false);
   declareProperty("ProcessMetadata",     m_processMetadata = true);
   declareProperty("ShowSizeStatistics",  m_showSizeStat = false);
   declareProperty("CollectionType",      m_collectionType = "ImplicitROOT");
   declareProperty("CollectionTree",      m_collectionTree = "POOLContainer");
   declareProperty("Connection",          m_connection);
   declareProperty("RefName",             m_refName);
   declareProperty("DerivedRefName",      m_derRefName);
   declareProperty("AttributeListKey",    m_attrListKey = "Input");
   declareProperty("InputCollections",    m_inputCollectionsProp);
   declareProperty("Query",               m_query = "");
   declareProperty("SkipBadFiles",        m_skipBadFiles = false);
   declareProperty("KeepInputFilesOpen",  m_keepInputFilesOpen = false);
   declareProperty("SkipEvents",          m_skipEvents = 0);
   declareProperty("SkipEventSequence",   m_skipEventSequenceProp);

   declareProperty("HelperTools",         m_helperTools);
   declareProperty("CounterTool",         m_counterTool);
   declareProperty("SharedMemoryTool",    m_sharedMemoryTool);

   // RunNumber, OldRunNumber and OverrideRunNumberFromInput are used
   // to override the run number coming in on the input stream
   declareProperty("RunNumber",           m_runNo = 0);
   m_runNo.verifier().setLower(0);
   declareProperty("OldRunNumber",        m_oldRunNo = 0);
   m_oldRunNo.verifier().setLower(0);
   declareProperty("OverrideRunNumberFromInput",   m_overrideRunNumberFromInput = false);
   // The following properties are only for compatibility with
   // McEventSelector and are not really used anywhere
   declareProperty("EventsPerRun",        m_eventsPerRun = 1000000);
   m_eventsPerRun.verifier().setLower(0);
   declareProperty("FirstEvent",          m_firstEventNo = 0);
   m_firstEventNo.verifier().setLower(0);
   declareProperty("FirstLB",             m_firstLBNo = 0);
   m_firstLBNo.verifier().setLower(0);
   declareProperty("EventsPerLB",         m_eventsPerLB = 1000);
   m_eventsPerLB.verifier().setLower(0);
   declareProperty("InitialTimeStamp",    m_initTimeStamp = 0);
   m_initTimeStamp.verifier().setLower(0);
   declareProperty("TimeStampInterval",   m_timeStampInterval = 0);
   declareProperty("OverrideRunNumber",   m_overrideRunNumber = false);
   declareProperty("OverrideEventNumber", m_overrideEventNumber = false);
   declareProperty("OverrideTimeStamp",   m_overrideTimeStamp = false);

   m_inputCollectionsProp.declareUpdateHandler(&EventSelectorAthenaPool::inputCollectionsHandler, this);
}
//________________________________________________________________________________
void EventSelectorAthenaPool::inputCollectionsHandler(Property&) {
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
//________________________________________________________________________________
StatusCode EventSelectorAthenaPool::initialize() {
   ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);
   if (!::AthService::initialize().isSuccess()) {
      ATH_MSG_FATAL("Cannot initialize AthService base class.");
      return(StatusCode::FAILURE);
   }

   // Check for input collection
   if (m_inputCollectionsProp.value().size() == 0) {
      ATH_MSG_FATAL("Use the property: EventSelector.InputCollections = "
		      << "[ \"<collectionName>\" ] (list of collections)");
      return(StatusCode::FAILURE);
   }
   m_skipEventSequence = m_skipEventSequenceProp.value();
   std::sort(m_skipEventSequence.begin(), m_skipEventSequence.end());

   // CollectionType must be one of:
   //   ExplicitROOT, ExplicitMySQL, ExplicitMySQLlt, ExplicitRAL, SeekableROOT, ImplicitROOT
   if (m_collectionType.value() != "ExplicitROOT" &&
	   m_collectionType.value() != "ExplicitMySQL" &&
	   m_collectionType.value() != "ExplicitMySQLlt" &&
	   m_collectionType.value() != "ExplicitRAL" &&
	   m_collectionType.value() != "SeekableROOT" &&
	   m_collectionType.value() != "ImplicitROOT") {
      ATH_MSG_FATAL("EventSelector.CollectionType must be one of: "
		<< "ExplicitROOT, ExplicitMySQL, ExplicitMySQLlt, ExplicitRAL, SeekableROOT, ImplicitROOT (default)");
      return(StatusCode::FAILURE);
   }

   // Retrieve ClassIDSvc
   if (!m_clidSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get ClassIDSvc.");
      return(StatusCode::FAILURE);
   }
   // Retrieve ChronoStatSvc
   if (!m_chronoStatSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get ChronoStatSvc.");
      return(StatusCode::FAILURE);
   }
   // Get IncidentSvc
   if (!m_incidentSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get IncidentSvc.");
      return(StatusCode::FAILURE);
   }
   // Get AthenaPoolCnvSvc
   if (!m_athenaPoolCnvSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get AthenaPoolCnvSvc");
      return(StatusCode::FAILURE);
   }
   // Get TagMetaDataStore
   if (!m_tagDataStore.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get TagMetaDataStore.");
      return(StatusCode::FAILURE);
   }
   // Get CounterTool (if configured)
   if (!m_counterTool.empty()) {
      if (!m_counterTool.retrieve().isSuccess()) {
         ATH_MSG_FATAL("Cannot get CounterTool.");
         return(StatusCode::FAILURE);
      }
   }
   // Get HelperTools
   if (!m_helperTools.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get " << m_helperTools);
      return(StatusCode::FAILURE);
   }
   // Get SharedMemoryTool (if configured)
   if (!m_sharedMemoryTool.empty()) {
      if (!m_sharedMemoryTool.retrieve().isSuccess()) {
         ATH_MSG_FATAL("Cannot get AthenaSharedMemoryTool");
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
   ATH_MSG_INFO("reinitialization...");

   // reset markers
   m_numEvt.resize(m_inputCollectionsProp.value().size(), -1);
   m_firstEvt.resize(m_inputCollectionsProp.value().size(), -1);
   m_guid == Guid::null();

   // Initialize InputCollectionsIterator
   m_inputCollectionsIterator = m_inputCollectionsProp.value().begin();
   m_curCollection = 0;
   m_firstEvt[0] = 0;
   m_evtCount = 0;
   bool retError = false;
   for (std::vector<ToolHandle<IAthenaSelectorTool> >::const_iterator iter = m_helperTools.begin(),
		   last = m_helperTools.end(); iter != last; iter++) {
      if (!(*iter)->postInitialize().isSuccess()) {
         ATH_MSG_FATAL("Failed to postInitialize() " << (*iter)->name());
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
   if (m_poolCollectionConverter == 0) {
      ATH_MSG_INFO("No Events found in any Input Collections");
      return(StatusCode::SUCCESS);
   }
   // Check for valid header name
   if (!m_refName.value().empty()) {
      if (m_derRefName.value().empty()) {
         m_derRefName = m_refName.value();
      }
      if (m_collectionType.value() == "ExplicitROOT"
		      || m_collectionType.value() == "ExplicitMySQL"
		      || m_collectionType.value() == "ExplicitMySQLlt"
		      || m_collectionType.value() == "ExplicitRAL") {
         if (!m_poolCollectionConverter->checkRefName(m_refName.value()).isSuccess()) {
            ATH_MSG_FATAL("Ref name: " << m_refName.value() << " is not valid ");
            return(StatusCode::FAILURE);
         } else {
            ATH_MSG_INFO("Using collection ref name: " << m_refName.value());
         }
      } else {
         ATH_MSG_INFO("Using implicit collection, ignore ref name: " << m_refName.value());
      }
   } else if (m_collectionType.value() == "ExplicitROOT"
		   || m_collectionType.value() == "ExplicitMySQL"
		   || m_collectionType.value() == "ExplicitMySQLlt"
		   || m_collectionType.value() == "ExplicitRAL") {
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
   while (m_headerIterator == 0 || m_headerIterator->next() == 0) { // no selected events
      if (m_poolCollectionConverter != 0) {
         m_poolCollectionConverter->disconnectDb().ignore();
         delete m_poolCollectionConverter; m_poolCollectionConverter = 0;
      }
      m_inputCollectionsIterator++;
      m_poolCollectionConverter = getCollectionCnv();
      if (m_poolCollectionConverter != 0) {
         m_headerIterator = &m_poolCollectionConverter->executeQuery();
      } else {
         break;
      }
   }
   if (m_poolCollectionConverter == 0 || m_headerIterator == 0) { // no event selected in any collection
      m_inputCollectionsIterator = m_inputCollectionsProp.value().begin();
      m_curCollection = 0;
      m_poolCollectionConverter = getCollectionCnv();
      if (m_poolCollectionConverter == 0) {
         return(StatusCode::SUCCESS);
      }
      m_headerIterator = &m_poolCollectionConverter->selectAll();
      while (m_headerIterator == 0 || m_headerIterator->next() == 0) { // empty collection
         if (m_poolCollectionConverter != 0) {
            m_poolCollectionConverter->disconnectDb().ignore();
            delete m_poolCollectionConverter; m_poolCollectionConverter = 0;
         }
         m_inputCollectionsIterator++;
         m_poolCollectionConverter = getCollectionCnv();
         if (m_poolCollectionConverter != 0) {
            m_headerIterator = &m_poolCollectionConverter->selectAll();
         } else {
            break;
         }
      }
   }
   if (m_poolCollectionConverter == 0 || m_headerIterator == 0) {
      return(StatusCode::SUCCESS);
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
      guid = m_headerIterator->currentRow().tokenList()[m_refName.value() + "_ref"].dbID();
      tech = token.technology();
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
   if (m_poolCollectionConverter != 0) {
      // Reset iterators and apply new query
      m_poolCollectionConverter->disconnectDb().ignore();
      delete m_poolCollectionConverter; m_poolCollectionConverter = 0;
   }
   m_inputCollectionsIterator = m_inputCollectionsProp.value().begin();
   m_curCollection = 0;
   m_poolCollectionConverter = getCollectionCnv(true);
   if (m_poolCollectionConverter == 0) {
      ATH_MSG_INFO("No Events found in any Input Collections");
      m_inputCollectionsIterator--;
      if (m_processMetadata.value()) {
         // Fire first BeginTagFile incident
         FileIncident beginTagFileIncident(name(), "BeginTagFile", *m_inputCollectionsIterator);
         m_incidentSvc->fireIncident(beginTagFileIncident);
         bool isPayload = m_collectionType.value() == "SeekableROOT" || m_collectionType.value() == "ImplicitROOT";
         if (isPayload && !m_firedIncident) {
            FileIncident firstInputFileIncident(name(), "FirstInputFile", *m_inputCollectionsIterator);
            m_incidentSvc->fireIncident(firstInputFileIncident);
            m_firedIncident = true;
         }
      }
      delete m_beginIter; m_beginIter = 0;
      m_beginIter = new EventContextAthenaPool(this);
      delete m_endIter;   m_endIter   = 0;
      m_endIter = new EventContextAthenaPool(0);
      return(StatusCode::SUCCESS);
   }
   if (m_processMetadata.value()) {
      // Get Collection Metadata into Tag Data Store
      bool isPayload = m_collectionType.value() == "SeekableROOT" || m_collectionType.value() == "ImplicitROOT";
      if (!isPayload) {
         CollectionMetadata* metadataMap = new CollectionMetadata(m_poolCollectionConverter->retrieveMetadata());
         if (!m_tagDataStore->record(metadataMap, "CollectionMetadata").isSuccess()) {
            ATH_MSG_ERROR("Cannot record Meatdata map to TagDataStore.");
            delete metadataMap; metadataMap = 0;
            return(StatusCode::FAILURE);
         }
      }
      // Fire first BeginTagFile incident
      FileIncident beginTagFileIncident(name(), "BeginTagFile", *m_inputCollectionsIterator);
      m_incidentSvc->fireIncident(beginTagFileIncident);
   }
   m_headerIterator = &m_poolCollectionConverter->executeQuery(/*m_query.value()*/);
   delete m_beginIter; m_beginIter = 0;
   m_beginIter = new EventContextAthenaPool(this);
   delete m_endIter;   m_endIter   = 0;
   m_endIter = new EventContextAthenaPool(0);
   return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode EventSelectorAthenaPool::stop() {
   if (m_processMetadata.value()) {
      if (m_evtCount >= 0) {
         if (m_headerIterator == 0 || m_headerIterator->next() == 0) {
            // Assume that the end of collection file indicates the end of payload file.
            if (m_guid != Guid::null()) {
               // Fire EndInputFile incident
               FileIncident endInputFileIncident(name(), "EndInputFile", "FID:" + m_guid.toString());
               m_incidentSvc->fireIncident(endInputFileIncident);
            }
            // Fire EndTagFile incident
            FileIncident endTagFileIncident(name(), "EndTagFile", *m_inputCollectionsIterator);
            m_incidentSvc->fireIncident(endTagFileIncident);
            if (!m_tagDataStore->clearStore().isSuccess()) {
               ATH_MSG_WARNING("Unable to clear tag MetaData Proxies");
            }
         }
      }
      // Fire LastInputFile incident
      if (m_firedIncident) {
         FileIncident lastInputFileIncident(name(), "LastInputFile", "end");
         m_incidentSvc->fireIncident(lastInputFileIncident);
         m_firedIncident = false;
      }
   }
   return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode EventSelectorAthenaPool::finalize() {
   if (!m_counterTool.empty()) {
      if (!m_counterTool->preFinalize().isSuccess()) {
         ATH_MSG_WARNING("Failed to preFinalize() CounterTool");
      }
   }
   for (std::vector<ToolHandle<IAthenaSelectorTool> >::const_iterator iter = m_helperTools.begin(),
		   last = m_helperTools.end(); iter != last; iter++) {
      if (!(*iter)->preFinalize().isSuccess()) {
         ATH_MSG_WARNING("Failed to preFinalize() " << (*iter)->name());
      }
   }
   delete m_beginIter; m_beginIter = 0;
   delete m_endIter;   m_endIter   = 0;
   m_headerIterator = 0;
   if (m_poolCollectionConverter != 0) {
      delete m_poolCollectionConverter; m_poolCollectionConverter = 0;
   }
   // Release AthenaSharedMemoryTool
   if (!m_sharedMemoryTool.empty()) {
      if (!m_sharedMemoryTool.release().isSuccess()) {
         ATH_MSG_WARNING("Cannot release AthenaSharedMemoryTool");
      }
   }
   // Release CounterTool
   if (!m_counterTool.empty()) {
      if (!m_counterTool.release().isSuccess()) {
         ATH_MSG_WARNING("Cannot release CounterTool.");
      }
   }
   // Release HelperTools
   if (!m_helperTools.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release " << m_helperTools);
   }
   // Release TagMetaDataStore
   if (!m_tagDataStore.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release TagMetaDataStore.");
   }
   // Release AthenaPoolCnvSvc
   if (!m_athenaPoolCnvSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release AthenaPoolCnvSvc.");
   }
   // Release IncidentSvc
   if (!m_incidentSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release IncidentSvc.");
   }
   // Release ChronoStatSvc
   if (!m_chronoStatSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release ChronoStatSvc.");
   }
   // Release ClassIDSvc
   if (!m_clidSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release ClassIDSvc.");
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
   } else if (riid == IEventSeek::interfaceID()) {
      *ppvInterface = dynamic_cast<IEventSeek*>(this);
   } else if (riid == ICollectionSize::interfaceID()) {
      *ppvInterface = dynamic_cast<ICollectionSize*>(this);
   } else if (riid == IEventShare::interfaceID()) {
      *ppvInterface = dynamic_cast<IEventShare*>(this);
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
   if (!m_sharedMemoryTool.empty() && m_sharedMemoryTool->isClient()) {
      if (eventStore()->transientContains<AthenaAttributeList>(m_attrListKey.value())) {
         const DataHandle<AthenaAttributeList> oldAttrList;
         if (!eventStore()->retrieve(oldAttrList, m_attrListKey.value()).isSuccess()) {
            ATH_MSG_ERROR("Cannot retrieve old AttributeList from StoreGate.");
            return(StatusCode::FAILURE);
         }
         if (!eventStore()->removeDataAndProxy(oldAttrList.cptr()).isSuccess()) {
            ATH_MSG_ERROR("Cannot remove old AttributeList from StoreGate.");
            return(StatusCode::FAILURE);
         }
      }
      void* token = 0;
      unsigned int status = 0;
      if (!m_sharedMemoryTool->getLockedEvent(&token, status).isSuccess()) {
         ATH_MSG_FATAL("Cannot get NextEvent from AthenaSharedMemoryTool");
         delete (char*)token; token = 0;
         return(StatusCode::FAILURE);
      }
      AthenaAttributeList* athAttrList = new AthenaAttributeList();
      if (!eventStore()->record(athAttrList, m_attrListKey.value()).isSuccess()) {
         ATH_MSG_ERROR("Cannot record AttributeList to StoreGate.");
         delete (char*)token; token = 0;
         delete athAttrList; athAttrList = 0;
         return(StatusCode::FAILURE);
      }
      athAttrList->extend("eventRef", "string");
      (*athAttrList)["eventRef"].data<std::string>() = std::string((char*)token);
      delete (char*)token; token = 0;
      return(m_sharedMemoryTool->unlockEvent());
   }
   for (std::vector<ToolHandle<IAthenaSelectorTool> >::const_iterator iter = m_helperTools.begin(),
		   last = m_helperTools.end(); iter != last; iter++) {
      if (!(*iter)->preNext().isSuccess()) {
         ATH_MSG_WARNING("Failed to preNext() " << (*iter)->name());
      }
   }
   for (;;) {
      if (m_headerIterator == 0 || m_headerIterator->next() == 0) {
         IEvtSelector::Context* rIt = const_cast<IEvtSelector::Context*>(&ctxt);
         m_headerIterator = 0;
         // Close previous collection.
         if (!m_keepInputFilesOpen.value() && m_poolCollectionConverter != 0) {
            m_poolCollectionConverter->disconnectDb().ignore();
         }
         delete m_poolCollectionConverter; m_poolCollectionConverter = 0;
         // Assume that the end of collection file indicates the end of payload file.
         if (m_guid != Guid::null()) {
            if (m_processMetadata.value()) {
               // Fire EndInputFile incident
               FileIncident endInputFileIncident(name(), "EndInputFile", "FID:" + m_guid.toString());
               m_incidentSvc->fireIncident(endInputFileIncident);
            }
            // Explicitly disconnect file corresponding to old m_guid to avoid holding on to large blocks of memory
            if (!m_keepInputFilesOpen.value()) {
               m_athenaPoolCnvSvc->getPoolSvc()->disconnectDb("FID:" + m_guid.toString(), IPoolSvc::kInputStream).ignore();
            }
            m_guid = Guid::null();
         }
         if (m_processMetadata.value()) {
            // Fire EndTagFile incident
            FileIncident endTagFileIncident(name(), "EndTagFile", *m_inputCollectionsIterator);
            m_incidentSvc->fireIncident(endTagFileIncident);
            if (!m_tagDataStore->clearStore().isSuccess()) {
               ATH_MSG_WARNING("Unable to clear tag MetaData Proxies");
            }
         }
         // Open next file from inputCollections list.
         m_inputCollectionsIterator++;
         // Create PoolCollectionConverter for input file
         m_poolCollectionConverter = getCollectionCnv(true);
         if (m_poolCollectionConverter == 0) {
            // Return end iterator
            *rIt = *m_endIter;
            m_evtCount = -1;
            return(StatusCode::FAILURE);
         }
         // Get DataHeader iterator
         m_headerIterator = &m_poolCollectionConverter->executeQuery();
         if (m_processMetadata.value()) {
            // Fire BeginTagFile incident
            bool isPayload = m_collectionType.value() == "SeekableROOT" || m_collectionType.value() == "ImplicitROOT";
            if (!isPayload) {
               // Get Collection Metadata into Tag Data Store
               CollectionMetadata* metadataMap = new CollectionMetadata(m_poolCollectionConverter->retrieveMetadata());
               if (!m_tagDataStore->record(metadataMap, "CollectionMetadata").isSuccess()) {
                  ATH_MSG_ERROR("Cannot record Meatdata map to TagDataStore.");
                  delete metadataMap; metadataMap = 0;
                  return(StatusCode::FAILURE);
               }
            }
            FileIncident beginTagFileIncident(name(), "BeginTagFile", *m_inputCollectionsIterator);
            m_incidentSvc->fireIncident(beginTagFileIncident);
         }
         continue; // handles empty files
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
         if (m_guid != Guid::null()) {
            if (m_processMetadata.value()) {
               // Fire EndInputFile incident
               FileIncident endInputFileIncident(name(), "EndInputFile", "FID:" + m_guid.toString());
               m_incidentSvc->fireIncident(endInputFileIncident);
            }
            // Explicitly disconnect file corresponding to old m_guid to avoid holding on to large blocks of memory
            if (!m_keepInputFilesOpen.value()) {
               m_athenaPoolCnvSvc->getPoolSvc()->disconnectDb("FID:" + m_guid.toString(), IPoolSvc::kInputStream).ignore();
            }
         }
         m_guid = guid;
         // Fire BeginInputFile incident if current InputCollection is a payload file;
         // otherwise, ascertain whether the pointed-to file is reachable before firing any incidents and/or proceeding
         bool isPayload = m_collectionType.value() == "SeekableROOT" || m_collectionType.value() == "ImplicitROOT";
         if (isPayload) {
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
      ++m_evtCount;
      if (!m_counterTool.empty()) {
         if (!m_counterTool->preNext().isSuccess()) {
            ATH_MSG_WARNING("Failed to preNext() CounterTool.");
         }
      }
      if (m_evtCount > m_skipEvents && (m_skipEventSequence.empty() || m_evtCount != m_skipEventSequence.front())) {
         StatusCode status = recordAttributeList();
         if (!status.isSuccess()) {
            ATH_MSG_WARNING("Failed to record AttributeList.");
         }
         for (std::vector<ToolHandle<IAthenaSelectorTool> >::const_iterator iter = m_helperTools.begin(),
		         last = m_helperTools.end(); iter != last; iter++) {
            StatusCode toolStatus = (*iter)->postNext();
            if (toolStatus.isRecoverable()) {
               ATH_MSG_INFO("Request skipping event from: " << (*iter)->name());
               if (status.isSuccess()) {
                  status = StatusCode::RECOVERABLE;
               }
            } else if (toolStatus.isFailure()) {
               ATH_MSG_WARNING("Failed to postNext() " << (*iter)->name());
               status = StatusCode::FAILURE;
            }
         }
         if (status.isRecoverable()) {
            ATH_MSG_INFO("skipping event " << m_evtCount);
	    m_incidentSvc->fireIncident(Incident(name(), "SkipEvent"));
         } else if (status.isFailure()) {
            ATH_MSG_WARNING("Failed to postNext() HelperTool.");
         } else {
            if (!m_counterTool.empty()) {
               if (!m_counterTool->postNext().isSuccess()) {
                  ATH_MSG_WARNING("Failed to postNext() CounterTool.");
               }
            }
            break;
         }
      } else {
         if (!m_skipEventSequence.empty() && m_evtCount == m_skipEventSequence.front()) {
            m_skipEventSequence.erase(m_skipEventSequence.begin());
         }
         ATH_MSG_INFO("skipping event " << m_evtCount);
         m_incidentSvc->fireIncident(Incident(name(), "SkipEvent"));
      }
   }
   return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode EventSelectorAthenaPool::next(IEvtSelector::Context& ctxt, int jump) const {
   if (jump > 0) {
      for (int i = 0; i < jump; i++) {
         if (!next(ctxt).isSuccess()) {
            return(StatusCode::FAILURE);
         }
      }
      return(StatusCode::SUCCESS);
   }
   return(StatusCode::FAILURE);
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
         if (!previous(ctxt).isSuccess()) {
            return(StatusCode::FAILURE);
         }
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
   if (eventStore()->transientContains<AthenaAttributeList>(m_attrListKey.value())) {
      const DataHandle<AthenaAttributeList> attrList;
      if (!eventStore()->retrieve(attrList, m_attrListKey.value()).isSuccess()) {
         ATH_MSG_ERROR("Cannot retrieve AttributeList from StoreGate.");
         return(StatusCode::FAILURE);
      }
      try {
         if (m_derRefName.value().empty()) {
            tokenStr = (*attrList)["eventRef"].data<std::string>();
            ATH_MSG_DEBUG("found AthenaAttribute, name = eventRef = " << tokenStr);
         } else {
            tokenStr = (*attrList)[m_derRefName.value() + "_ref"].data<std::string>();
            ATH_MSG_DEBUG("found AthenaAttribute, name = " << m_derRefName.value() << "_ref = " << tokenStr);
         }
      } catch (std::exception &e) {
         ATH_MSG_ERROR(e.what());
         return(StatusCode::FAILURE);
      }
   } else {
      ATH_MSG_WARNING("Cannot find AthenaAttribute, key = " << m_attrListKey.value());
      tokenStr = m_poolCollectionConverter->retrieveToken(m_headerIterator, m_refName.value());
   }
   iop = new GenericAddress(POOL_StorageType, ClassID_traits<DataHeader>::ID(), tokenStr, "EventSelector");
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
StatusCode EventSelectorAthenaPool::seek(int evtNum) {
   // Exit early if we are not capable of seeking.
   // Otherwise, we ge a confusing error message.
   if (dynamic_cast<pool::IPositionSeek*>(m_headerIterator) == 0) {
      ATH_MSG_ERROR("seek: Container does not allow seeking.");
      return(StatusCode::FAILURE);
   }
   long newColl = findEvent(evtNum);
   if (newColl == -1 && evtNum >= m_firstEvt[m_curCollection] && evtNum < m_evtCount) {
      newColl = m_curCollection;
   }
   if (newColl == -1) {
      m_headerIterator = 0;
      ATH_MSG_INFO("seek: Reached end of Input.");
      return(StatusCode::RECOVERABLE);
   }
   if (newColl != m_curCollection) {
      if (!m_keepInputFilesOpen.value() && m_poolCollectionConverter != 0) {
         m_poolCollectionConverter->disconnectDb().ignore();
      }
      delete m_poolCollectionConverter; m_poolCollectionConverter = 0;
      m_curCollection = newColl;
      try {
         ATH_MSG_DEBUG("Seek to item: \""
	         <<  m_inputCollectionsProp.value()[m_curCollection]
	         << "\" from the collection list.");
         // Reset input collection iterator to the right place
         m_inputCollectionsIterator = m_inputCollectionsProp.value().begin();
         m_inputCollectionsIterator += m_curCollection;
         m_poolCollectionConverter = new PoolCollectionConverter(msgSvc(),
	         m_collectionType.value() + ":" + m_collectionTree.value(),
	         m_connection.value(),
	         m_inputCollectionsProp.value()[m_curCollection],
	         m_query.value(),
	         m_athenaPoolCnvSvc->getPoolSvc(),
	         IPoolSvc::kInputStream);
         if (!m_poolCollectionConverter->initialize().isSuccess()) {
            m_headerIterator = 0;
            ATH_MSG_ERROR("seek: Unable to initialize PoolCollectionConverter.");
            return(StatusCode::FAILURE);
         }
         // Create DataHeader iterators
         m_headerIterator = &m_poolCollectionConverter->executeQuery();
         delete m_beginIter; m_beginIter = 0;
         m_beginIter = new EventContextAthenaPool(this);
         next(*m_beginIter).ignore();
         ATH_MSG_DEBUG("Token " << m_headerIterator->eventRef().toString());
      } catch (std::exception &e) {
         m_headerIterator = 0;
         ATH_MSG_ERROR(e.what());
         return(StatusCode::FAILURE);
      }
   }

   pool::IPositionSeek* is = dynamic_cast<pool::IPositionSeek*>(m_headerIterator);
   if (is == 0) {
      ATH_MSG_ERROR("Container does not allow seeking.");
      return(StatusCode::FAILURE);
   }
   if (is->seek(evtNum - m_firstEvt[m_curCollection]) == 0) {
      m_headerIterator = 0;
      ATH_MSG_ERROR("Did not find event, evtNum = " << evtNum);
      return(StatusCode::FAILURE);
   } else {
      m_evtCount = evtNum + 1;
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
int EventSelectorAthenaPool::curEvent() const {
  return(m_evtCount);
}
//__________________________________________________________________________
// Search for event number evtNum.
// Return the index of the collection containing it, or -1 if not found.
// Note: passing -1 for evtNum will always yield failure,
// but this can be used to force filling in the entire m_numEvt array.
int EventSelectorAthenaPool::findEvent(int evtNum) {
   for (std::size_t i = 0, imax = m_numEvt.size(); i < imax; i++) {
      if (m_numEvt[i] == -1) {
         PoolCollectionConverter pcc(msgSvc(),
	         m_collectionType.value() + ":" + m_collectionTree.value(),
	         m_connection.value(),
	         m_inputCollectionsProp.value()[i],
	         m_query.value(),
	         m_athenaPoolCnvSvc->getPoolSvc());
         if (!pcc.initialize().isSuccess()) {
            break;
         }
         pool::ICollectionCursor* hi = &pcc.executeQuery();
         ICollectionSize* cs = dynamic_cast<ICollectionSize*>(hi);
         if (cs == 0) {
            break;
         }
         if (i > 0) {
            m_firstEvt[i] = m_firstEvt[i - 1] + m_numEvt[i - 1];
         } else {
            m_firstEvt[i] = 0;
         }
         m_numEvt[i] = cs->size();
      }
      if (evtNum >= m_firstEvt[i] && evtNum < m_firstEvt[i] + m_numEvt[i]) {
         return(i);
      }
   }
   return(-1);
}

//________________________________________________________________________________
StatusCode EventSelectorAthenaPool::makeServer(int /*num*/) {
   if (m_sharedMemoryTool.empty()) {
      return(StatusCode::FAILURE);
   }
   return(m_sharedMemoryTool->makeServer());
}

//________________________________________________________________________________
StatusCode EventSelectorAthenaPool::makeClient(int /*num*/) {
   if (m_sharedMemoryTool.empty()) {
      return(StatusCode::FAILURE);
   }
   return(m_sharedMemoryTool->makeClient());
}

//________________________________________________________________________________
StatusCode EventSelectorAthenaPool::share(int evtnum) {
   if (m_sharedMemoryTool.empty()) {
      return(StatusCode::FAILURE);
   }
   if (m_sharedMemoryTool->isClient()) {
      return(m_sharedMemoryTool->lockEvent(evtnum));
   }
   return(StatusCode::FAILURE);
}

//________________________________________________________________________________
StatusCode EventSelectorAthenaPool::readEvent(int maxevt) {
   if (m_sharedMemoryTool.empty()) {
      return(StatusCode::FAILURE);
   }
   for (int i = 0; i < maxevt; ++i) {
      if(m_headerIterator == 0 || m_headerIterator->next() == 0){
         // End of file, wait for last event to be taken
         if (!m_sharedMemoryTool->putEvent(0, 0, 0, 0).isSuccess()) {
            ATH_MSG_ERROR("Cannot put last Event marker to AthenaSharedMemoryTool");
         }
         return(StatusCode::FAILURE);
      }
      ++m_evtCount;
      if (m_evtCount <= m_skipEvents) {
         ATH_MSG_DEBUG("Skipping event " << m_evtCount - 1);
         continue;
      }
      if (!m_skipEventSequence.empty() && m_evtCount == m_skipEventSequence.front()) {
         m_skipEventSequence.erase(m_skipEventSequence.begin());
         ATH_MSG_DEBUG("Skipping event " << m_evtCount - 1);
         continue;
      }
      if (m_sharedMemoryTool->isServer()) {
         std::string token = m_headerIterator->eventRef().toString();
         if (!m_sharedMemoryTool->putEvent(m_evtCount - 1, token.c_str(), token.length() + 1, 0).isSuccess()) {
            ATH_MSG_ERROR("Cannot put Event " << m_evtCount - 1 << " to AthenaSharedMemoryTool");
            return(StatusCode::FAILURE);
         }
      }
   }
   return(StatusCode::SUCCESS);
}

//__________________________________________________________________________
int EventSelectorAthenaPool::size() {
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
      PoolCollectionConverter* pCollCnv = new PoolCollectionConverter(msgSvc(),
	      m_collectionType.value() + ":" + m_collectionTree.value(),
	      m_connection.value(),
	      *m_inputCollectionsIterator,
	      m_query.value(),
	      m_athenaPoolCnvSvc->getPoolSvc(),
	      IPoolSvc::kInputStream);
      StatusCode status = pCollCnv->initialize();
      if (!status.isSuccess()) {
         // Close previous collection.
         delete pCollCnv; pCollCnv = 0;
         if (!status.isRecoverable()) {
            ATH_MSG_ERROR("Unable to initialize PoolCollectionConverter.");
 	    FileIncident inputFileError(name(), "FailInputFile", *m_inputCollectionsIterator);
 	    m_incidentSvc->fireIncident(inputFileError);
            throw GaudiException("Unable to read: " + *m_inputCollectionsIterator, name(), StatusCode::FAILURE);
         } else {
            ATH_MSG_ERROR("Unable to open: " << *m_inputCollectionsIterator);
 	    FileIncident inputFileError(name(), "FailInputFile", *m_inputCollectionsIterator);
 	    m_incidentSvc->fireIncident(inputFileError);
            throw GaudiException("Unable to open: " + *m_inputCollectionsIterator, name(), StatusCode::FAILURE);
         }
      } else {
         if (!pCollCnv->isValid().isSuccess()) {
            delete pCollCnv; pCollCnv = 0;
            ATH_MSG_DEBUG("No events found in: " << *m_inputCollectionsIterator << " skipped!!!");
            if (throwIncidents && m_processMetadata.value()) {
               FileIncident beginInputFileIncident(name(), "BeginInputFile", *m_inputCollectionsIterator);
               m_incidentSvc->fireIncident(beginInputFileIncident);
               FileIncident endInputFileIncident(name(), "EndInputFile", "eventless " + *m_inputCollectionsIterator);
               m_incidentSvc->fireIncident(endInputFileIncident);
            }
            m_athenaPoolCnvSvc->getPoolSvc()->disconnectDb(*m_inputCollectionsIterator, IPoolSvc::kInputStream).ignore();
            m_inputCollectionsIterator++;
         } else {
            return(pCollCnv);
         }
      }
   }
   return(0);
}
//__________________________________________________________________________
StatusCode EventSelectorAthenaPool::recordAttributeList() const {
   // Get access to AttributeList
   ATH_MSG_DEBUG("Get AttributeList from the collection");
   // MN: accessing only attribute list, ignoring token list
   const coral::AttributeList& attrList = m_headerIterator->currentRow().attributeList();
   ATH_MSG_DEBUG("AttributeList size " << attrList.size());
   if (eventStore()->transientContains<AthenaAttributeList>(m_attrListKey.value())) {
      const DataHandle<AthenaAttributeList> oldAttrList;
      if (!eventStore()->retrieve(oldAttrList, m_attrListKey.value()).isSuccess()) {
         ATH_MSG_ERROR("Cannot retrieve old AttributeList from StoreGate.");
         return(StatusCode::FAILURE);
      }
      if (!eventStore()->removeDataAndProxy(oldAttrList.cptr()).isSuccess()) {
         ATH_MSG_ERROR("Cannot remove old AttributeList from StoreGate.");
         return(StatusCode::FAILURE);
      }
   }
   AthenaAttributeList* athAttrList = new AthenaAttributeList(attrList);
   if (!eventStore()->record(athAttrList, m_attrListKey.value()).isSuccess()) {
      ATH_MSG_ERROR("Cannot record AttributeList to StoreGate.");
      delete athAttrList; athAttrList = 0;
      return(StatusCode::FAILURE);
   }
   const pool::TokenList& tokenList = m_headerIterator->currentRow().tokenList();
   for (pool::TokenList::const_iterator iter = tokenList.begin(), last = tokenList.end(); iter != last; ++iter) {
      athAttrList->extend(iter.tokenName(), "string");
      (*athAttrList)[iter.tokenName()].data<std::string>() = iter->toString();
      ATH_MSG_DEBUG("record AthenaAttribute, name = " << iter.tokenName() << " = " << iter->toString() << ".");
   }
   athAttrList->extend("eventRef", "string");
   (*athAttrList)["eventRef"].data<std::string>() = m_headerIterator->eventRef().toString();
   ATH_MSG_DEBUG("record AthenaAttribute, name = eventRef = " << m_headerIterator->eventRef().toString() << ".");
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode EventSelectorAthenaPool::io_reinit() {
   ATH_MSG_INFO("I/O reinitialization...");
   if (m_poolCollectionConverter != 0) {
      m_poolCollectionConverter->disconnectDb().ignore();
      delete m_poolCollectionConverter; m_poolCollectionConverter = 0;
   }
   m_headerIterator = 0;
   ServiceHandle<IIoComponentMgr> iomgr("IoComponentMgr", name());
   if (!iomgr.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Could not retrieve IoComponentMgr !");
      return(StatusCode::FAILURE);
   }
   if (!iomgr->io_hasitem(this)) {
      ATH_MSG_FATAL("IoComponentMgr does not know about myself !");
      return(StatusCode::FAILURE);
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
   if (m_poolCollectionConverter != 0) {
      m_poolCollectionConverter->disconnectDb().ignore();
      delete m_poolCollectionConverter; m_poolCollectionConverter = 0;
   }
   return(StatusCode::SUCCESS);
}
