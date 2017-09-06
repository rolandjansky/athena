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
#include "registerKeys.h"

#include "AthenaPoolCnvSvc/IAthenaPoolCnvSvc.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "DBDataModel/CollectionMetadata.h"
#include "PersistentDataModel/Token.h"
#include "PersistentDataModel/DataHeader.h"
#include "PoolSvc/IPoolSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ActiveStoreSvc.h"

#include "AthenaKernel/IAthenaIPCTool.h"
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
	m_satelliteOid1(0LL),
	m_athenaPoolCnvSvc("AthenaPoolCnvSvc", name),
	m_chronoStatSvc("ChronoStatSvc", name),
	m_clidSvc("ClassIDSvc", name),
	m_incidentSvc("IncidentSvc", name),
	m_helperTools(this),
	m_counterTool("", this),
	m_eventStreamingTool("", this),
        m_curCollection(0),
	m_evtCount(0),
	m_firedIncident(false) {
   declareProperty("BackNavigation",      m_backNavigationFlag = false);
   declareProperty("ProcessMetadata",     m_processMetadata = true);
   declareProperty("ShowSizeStatistics",  m_showSizeStat = false);
   declareProperty("CollectionType",      m_collectionType = "ImplicitROOT");
   declareProperty("CollectionTree",      m_collectionTree = "POOLContainer");
   declareProperty("Connection",          m_connection);
   declareProperty("RefName",             m_refName);
   declareProperty("AttributeListKey",    m_attrListKey = "Input");
   declareProperty("InputCollections",    m_inputCollectionsProp);
   declareProperty("Query",               m_query = "");
   declareProperty("SkipBadFiles",        m_skipBadFiles = false);
   declareProperty("KeepInputFilesOpen",  m_keepInputFilesOpen = false);
   declareProperty("SkipEvents",          m_skipEvents = 0);
   declareProperty("SkipEventSequence",   m_skipEventSequenceProp);
   declareProperty("HelperTools",         m_helperTools);
   declareProperty("CounterTool",         m_counterTool);
   declareProperty("SharedMemoryTool",    m_eventStreamingTool);

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
	   m_collectionType.value() != "SeekableROOT" &&
	   m_collectionType.value() != "ImplicitROOT") {
      ATH_MSG_FATAL("EventSelector.CollectionType must be one of: ExplicitROOT, SeekableROOT, ImplicitROOT (default)");
      return(StatusCode::FAILURE);
   }

   // Retrieve ClassIDSvc
   if (!m_clidSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get " << m_clidSvc.typeAndName() << ".");
      return(StatusCode::FAILURE);
   }
   // Retrieve ChronoStatSvc
   if (!m_chronoStatSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get " << m_chronoStatSvc.typeAndName() << ".");
      return(StatusCode::FAILURE);
   }
   // Get IncidentSvc
   if (!m_incidentSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get " << m_incidentSvc.typeAndName() << ".");
      return(StatusCode::FAILURE);
   }
   // Get AthenaPoolCnvSvc
   if (!m_athenaPoolCnvSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get " << m_athenaPoolCnvSvc.typeAndName() << ".");
      return(StatusCode::FAILURE);
   }
   // Get TagMetaDataStore
   if (!m_tagDataStore.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get " << m_tagDataStore.typeAndName() << ".");
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
   for (std::vector<std::string>::const_iterator iter = propVal.begin(), last = propVal.end(); iter != last; iter++) {
      if (*iter == m_athenaPoolCnvSvc.type()) { foundCnvSvc = true; }
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
   ATH_MSG_INFO("reinitialization...");

   // reset markers
   m_numEvt.resize(m_inputCollectionsProp.value().size(), -1);
   m_firstEvt.resize(m_inputCollectionsProp.value().size(), -1);
   m_guid = Guid::null();

   // Initialize InputCollectionsIterator
   m_inputCollectionsIterator = m_inputCollectionsProp.value().begin();
   m_curCollection = 0;
   if (!m_firstEvt.empty()) {
     m_firstEvt[0] = 0;
   }
   m_evtCount = 0;
   if (!m_eventStreamingTool.empty() && m_eventStreamingTool->isClient()) {
      ATH_MSG_INFO("Done reinitialization for shared reader client");
      return(StatusCode::SUCCESS);
   }
   bool retError = false;
   for (std::vector<ToolHandle<IAthenaSelectorTool> >::iterator iter = m_helperTools.begin(),
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
      if (m_processMetadata.value()) {
	m_inputCollectionsIterator = m_inputCollectionsProp.value().end();
	if(m_inputCollectionsProp.value().size()>0) m_inputCollectionsIterator--;
	//NOTE (wb may 2016): this will make the FirstInputFile incident correspond to last file in the collection ... if want it to be first file then move iterator to begin and then move above two lines below this incident firing
         bool isPayload = m_collectionType.value() == "SeekableROOT" || m_collectionType.value() == "ImplicitROOT";
         if (isPayload && !m_firedIncident && m_inputCollectionsProp.value().size()>0) {
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
   if (m_poolCollectionConverter != 0) {
      // Reset iterators and apply new query
      m_poolCollectionConverter->disconnectDb().ignore();
      delete m_poolCollectionConverter; m_poolCollectionConverter = 0;
   }
   m_inputCollectionsIterator = m_inputCollectionsProp.value().begin();
   m_curCollection = 0;
   if (!m_eventStreamingTool.empty() && m_eventStreamingTool->isClient()) {
      return(StatusCode::SUCCESS);
   }
   m_poolCollectionConverter = getCollectionCnv(true);
   if (m_poolCollectionConverter == 0) {
      ATH_MSG_INFO("No Events found in any Input Collections");
      m_inputCollectionsIterator = m_inputCollectionsProp.value().end();
      if(m_inputCollectionsProp.value().size()>0) {
        m_inputCollectionsIterator--; //leave iterator in state of last input file
        if (m_processMetadata.value()) {
          // Fire first BeginTagFile incident
          FileIncident beginTagFileIncident(name(), "BeginTagFile", *m_inputCollectionsIterator);
          m_incidentSvc->fireIncident(beginTagFileIncident);
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
   if (!m_eventStreamingTool.empty() && m_eventStreamingTool->isClient()) {
      return(StatusCode::SUCCESS);
   }
   IEvtSelector::Context* ctxt(0);
   if (!releaseContext(ctxt).isSuccess()) {
      ATH_MSG_WARNING("Cannot release context");
   }
   return(StatusCode::SUCCESS);
}


void EventSelectorAthenaPool::fireEndFileIncidents(bool isLastFile, bool fireEndTagIncident) const {
   if (m_processMetadata.value()) {
      if (m_evtCount >= 0) {
            // Assume that the end of collection file indicates the end of payload file.
            if (m_guid != Guid::null()) {
               // Fire EndInputFile incident
               FileIncident endInputFileIncident(name(), "EndInputFile", "FID:" + m_guid.toString());
               m_incidentSvc->fireIncident(endInputFileIncident);
            }
            // Fire EndTagFile incident if not out of files (maybe we should make it fire then as well?)
            if(fireEndTagIncident) {
               if(m_inputCollectionsIterator!=m_inputCollectionsProp.value().end()) {
                  FileIncident endTagFileIncident(name(), "EndTagFile", *m_inputCollectionsIterator);
                  m_incidentSvc->fireIncident(endTagFileIncident);
                  if (!m_tagDataStore->clearStore().isSuccess()) {
                     ATH_MSG_WARNING("Unable to clear tag MetaData Proxies");
                  }
               }
            }
      }
      // Fire LastInputFile incident
      if (isLastFile && m_firedIncident) {
         FileIncident lastInputFileIncident(name(), "LastInputFile", "end");
         m_incidentSvc->fireIncident(lastInputFileIncident);
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
      for (std::vector<ToolHandle<IAthenaSelectorTool> >::iterator iter = m_helperTools.begin(),
             last = m_helperTools.end(); iter != last; iter++) {
        if (!(*iter)->preFinalize().isSuccess()) {
          ATH_MSG_WARNING("Failed to preFinalize() " << (*iter)->name());
        }
      }
   }
   delete m_beginIter; m_beginIter = 0;
   delete m_endIter;   m_endIter   = 0;
   m_headerIterator = 0;
   if (m_poolCollectionConverter != 0) {
      delete m_poolCollectionConverter; m_poolCollectionConverter = 0;
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
   // Release TagMetaDataStore
   if (!m_tagDataStore.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release " << m_tagDataStore.typeAndName() << ".");
   }
   // Release AthenaPoolCnvSvc
   if (!m_athenaPoolCnvSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release " << m_athenaPoolCnvSvc.typeAndName() << ".");
   }
   // Release IncidentSvc
   if (!m_incidentSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release " << m_incidentSvc.typeAndName() << ".");
   }
   // Release ChronoStatSvc
   if (!m_chronoStatSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release " << m_chronoStatSvc.typeAndName() << ".");
   }
   // Release ClassIDSvc
   if (!m_clidSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release " << m_clidSvc.typeAndName() << ".");
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
   std::lock_guard<CallMutex> lockGuard(m_callLock);
   if (!m_eventStreamingTool.empty() && m_eventStreamingTool->isClient()) {
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
      void* tokenStr = 0;
      unsigned int status = 0;
      if (!m_eventStreamingTool->getLockedEvent(&tokenStr, status).isSuccess()) {
         ATH_MSG_FATAL("Cannot get NextEvent from AthenaSharedMemoryTool");
         delete (char*)tokenStr; tokenStr = 0;
         return(StatusCode::FAILURE);
      }
      AthenaAttributeList* athAttrList = new AthenaAttributeList();
      if (!eventStore()->record(athAttrList, m_attrListKey.value()).isSuccess()) {
         ATH_MSG_ERROR("Cannot record AttributeList to StoreGate.");
         delete (char*)tokenStr; tokenStr = 0;
         delete athAttrList; athAttrList = 0;
         return(StatusCode::FAILURE);
      }
      athAttrList->extend("eventRef", "string");
      (*athAttrList)["eventRef"].data<std::string>() = std::string((char*)tokenStr);
      Token token;
      token.fromString(std::string((char*)tokenStr));
      delete (char*)tokenStr; tokenStr = 0;
      Guid guid = token.dbID();
      if (guid != m_guid && m_processMetadata.value()) {
         if (m_guid != Guid::null()) {
	   fireEndFileIncidents(false,true/*NOTE: Unclear if we should fire EndTagFile in this code (will buttinger - 26/02/2015)*/);
         }
         m_guid = guid;
         FileIncident beginInputFileIncident(name(), "BeginInputFile", "FID:" + m_guid.toString());
         m_incidentSvc->fireIncident(beginInputFileIncident);
      }
      return(StatusCode::SUCCESS);
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
	 fireEndFileIncidents(false,true);
         if (m_guid != Guid::null()) {
            // Explicitly disconnect file corresponding to old m_guid to avoid holding on to large blocks of memory
            if (!m_keepInputFilesOpen.value()) {
               m_athenaPoolCnvSvc->getPoolSvc()->disconnectDb("FID:" + m_guid.toString(), IPoolSvc::kInputStream).ignore();
            }
            m_guid = Guid::null();
         }
         // Open next file from inputCollections list.
         m_inputCollectionsIterator++;
         // Create PoolCollectionConverter for input file
         m_poolCollectionConverter = getCollectionCnv(true);
         if (m_poolCollectionConverter == 0) {
	   fireEndFileIncidents(true,false/*doesnt fire the EndTagFile incident, already fired in call above*/); //fires LastInputFile incident
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
         //NOTE: to self (wb): we should only get here if we are processing a collection file (files containing dataheaders which reference other files!)
         if (m_guid != Guid::null()) {
	   fireEndFileIncidents(false,false/*does not fire EndTagFile when switching files within a collection file*/);
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
         // Using Satellite DataHeader?
         std::string::size_type p_slash = m_collectionTree.value().find('/');
         if (p_slash != std::string::npos) {
            const std::string tree = m_collectionTree.value().substr(0, p_slash);
            const std::string satellite = m_collectionTree.value().substr(p_slash + 1);
            const Token* satelliteToken = m_athenaPoolCnvSvc->getPoolSvc()->getToken("FID:" + m_headerIterator->eventRef().dbID().toString(), tree + "(" + satellite + "/DataHeader)", 0);
            if (satelliteToken != 0) {
               m_satelliteOid1 = satelliteToken->oid().first;
            } else {
               m_satelliteOid1 = 0;
            }
            delete satelliteToken; satelliteToken = 0;
         } else {
            m_satelliteOid1 = 0;
         }
      }  // end if (guid != m_guid)
      ++m_evtCount;
      if (!m_counterTool.empty() && !m_counterTool->preNext().isSuccess()) {
         ATH_MSG_WARNING("Failed to preNext() CounterTool.");
      }
      if (m_evtCount > m_skipEvents && (m_skipEventSequence.empty() || m_evtCount != m_skipEventSequence.front())) {
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
            if (!recordAttributeList().isSuccess()) {
               ATH_MSG_ERROR("Failed to record AttributeList.");
               return(StatusCode::FAILURE);
            }
         }
         StatusCode status = StatusCode::SUCCESS;
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
            if (!m_counterTool.empty() && !m_counterTool->postNext().isSuccess()) {
               ATH_MSG_WARNING("Failed to postNext() CounterTool.");
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
   iop = new GenericAddress(POOL_StorageType, ClassID_traits<DataHeader>::ID(), tokenStr, "EventSelector");
   return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode EventSelectorAthenaPool::releaseContext(IEvtSelector::Context*& /*ctxt*/) const {
   if (!m_eventStreamingTool.empty() && m_eventStreamingTool->isClient()) {
      return(StatusCode::SUCCESS);
   }
   return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode EventSelectorAthenaPool::resetCriteria(const std::string& /*criteria*/,
		IEvtSelector::Context& /*ctxt*/) const {
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode EventSelectorAthenaPool::seek(int evtNum) {
   long newColl = findEvent(evtNum);
   if (newColl == -1 && evtNum >= m_firstEvt[m_curCollection] && evtNum < m_evtCount - 1) {
      newColl = m_curCollection;
   }
   if (newColl == -1) {
      m_headerIterator = 0;
      ATH_MSG_INFO("seek: Reached end of Input.");
      fireEndFileIncidents(true,true);
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
	         m_athenaPoolCnvSvc->getPoolSvc());
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
         int collection_size = 0;
         if (pcc.isValid()) {
            pool::ICollectionCursor* hi = &pcc.executeQuery();
            ICollectionSize* cs = dynamic_cast<ICollectionSize*>(hi);
            if (cs == 0) {
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
      //return(StatusCode::FAILURE);
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
   ATH_MSG_VERBOSE("Called read Event " << maxevt);
   IEvtSelector::Context* ctxt = new EventContextAthenaPool(this);
   for (int i = 0; i < maxevt || maxevt == -1; ++i) {
      if (!next(*ctxt).isSuccess()) {
         if (m_evtCount == -1) {
            ATH_MSG_VERBOSE("Called read Event and read last event from input: " << i);
            break;
         }
         ATH_MSG_ERROR("Cannot read Event " << m_evtCount - 1 << " into AthenaSharedMemoryTool");
         delete ctxt; ctxt = 0;
         return(StatusCode::FAILURE);
      } else {
         ATH_MSG_VERBOSE("Called next, read Event " << m_evtCount - 1);
      }
   }
   delete ctxt; ctxt = 0;
   // End of file, wait for last event to be taken
   StatusCode sc = m_eventStreamingTool->putEvent(0, 0, 0, 0);
   while (sc.isRecoverable()) {
      while (m_athenaPoolCnvSvc->readData().isSuccess()) {
         ATH_MSG_VERBOSE("Called last readData, while marking last event in readEvent()");
      }
// Nothing to do right now, trigger alternative (e.g. caching) here? Currently just fast loop.
      sc = m_eventStreamingTool->putEvent(0, 0, 0, 0);
   }
   if (!sc.isSuccess()) {
      ATH_MSG_ERROR("Cannot put last Event marker to AthenaSharedMemoryTool");
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
	      m_athenaPoolCnvSvc->getPoolSvc());
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
   // Using Satellite DataHeader?
   if (m_satelliteOid1 > 0) {
      Token satelliteToken;
      m_headerIterator->eventRef().set(&satelliteToken);
      satelliteToken.setOid(Token::OID_t(m_satelliteOid1, m_headerIterator->eventRef().oid().second));
      (*athAttrList)["eventRef"].data<std::string>() = satelliteToken.toString();
   } else {
      (*athAttrList)["eventRef"].data<std::string>() = m_headerIterator->eventRef().toString();
   }
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
   if (m_poolCollectionConverter != 0) {
      m_poolCollectionConverter->disconnectDb().ignore();
      delete m_poolCollectionConverter; m_poolCollectionConverter = 0;
   }
   return(StatusCode::SUCCESS);
}
