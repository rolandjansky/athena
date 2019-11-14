/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** @file DoubleEventSelectorAthenaPool.cxx
 *  @brief This file contains the implementation for the DoubleEventSelectorAthenaPool class.
 *  @author Peter van Gemmeren <gemmeren      -at- anl.gov>
 *  @author John Detek Chapman <chapman       -at- hep.phy.cam.ac.uk>
 *  @author Miha Muskinja      <miha.muskinja -at- cern.ch>
 **/

#include "DoubleEventSelectorAthenaPool.h"
#include "EventContextAthenaPool.h"
#include "PoolCollectionConverter.h"

#include "AthenaPoolCnvSvc/IAthenaPoolCnvSvc.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "PersistentDataModel/Token.h"
#include "PersistentDataModel/TokenAddress.h"
#include "PersistentDataModel/DataHeader.h"
#include "PoolSvc/IPoolSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ActiveStoreSvc.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

#include "AthenaKernel/IAthenaIPCTool.h"
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
DoubleEventSelectorAthenaPool::DoubleEventSelectorAthenaPool(const std::string& name, ISvcLocator* pSvcLocator) :
  ::AthService(name, pSvcLocator),
  m_activeStoreSvc("ActiveStoreSvc", name),
  m_athenaPoolCnvSvc("AthenaPoolCnvSvc", name),
  m_incidentSvc("IncidentSvc", name),
  m_helperTools(this),
  m_counterTool("", this),
  m_primaryEventStreamingTool("", this),
  m_secondaryEventStreamingTool("", this),
  m_curPrimaryCollection(0),
  m_curSecondaryCollection(0),
  m_evtCount(0) {
  declareProperty("ProcessPrimaryMetadata",     m_processPrimaryMetadata = true);
  declareProperty("ProcessSecondaryMetadata",     m_processSecondaryMetadata = true);
  declareProperty("ShowSizeStatistics",  m_showSizeStat = false);
  declareProperty("CollectionType",      m_collectionType = "ImplicitROOT");
  declareProperty("CollectionTree",      m_collectionTree = "POOLContainer");
  declareProperty("Connection",          m_connection);
  declareProperty("RefName",             m_refName);
  declareProperty("AttributeListKey",    m_attrListKey = "Input");
  declareProperty("SecondaryAttrListSuffix",    m_secondaryAttrListSuffix = "secondary");
  declareProperty("PrimaryInputCollections",    m_primaryInputCollectionsProp);
  declareProperty("SecondaryaryInputCollections",    m_secondaryInputCollectionsProp);
  declareProperty("Query",               m_query = "");
  declareProperty("KeepInputFilesOpen",  m_keepInputFilesOpen = false);
  declareProperty("SkipEvents",          m_skipEvents = 0);
  declareProperty("SkipEventSequence",   m_skipEventSequenceProp);
  declareProperty("HelperTools",         m_helperTools);
  declareProperty("CounterTool",         m_counterTool);

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

  m_primaryInputCollectionsProp.declareUpdateHandler(&DoubleEventSelectorAthenaPool::inputCollectionsHandler, this);
  m_secondaryInputCollectionsProp.declareUpdateHandler(&DoubleEventSelectorAthenaPool::inputCollectionsHandler, this);
}
//________________________________________________________________________________
void DoubleEventSelectorAthenaPool::inputCollectionsHandler(Property&) {
  if (this->FSMState() != Gaudi::StateMachine::OFFLINE) {
    this->reinit().ignore();
  }
}
//________________________________________________________________________________
DoubleEventSelectorAthenaPool::~DoubleEventSelectorAthenaPool() {
}
//________________________________________________________________________________
StoreGateSvc* DoubleEventSelectorAthenaPool::eventStore() const {
  if (m_activeStoreSvc == 0) {
    if (!m_activeStoreSvc.retrieve().isSuccess()) {
      ATH_MSG_ERROR("Cannot get ActiveStoreSvc");
      throw GaudiException("Cannot get ActiveStoreSvc", name(), StatusCode::FAILURE);
    }
  }
  return(m_activeStoreSvc->operator->());
}
//________________________________________________________________________________
StatusCode DoubleEventSelectorAthenaPool::initialize() {
  ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);
  if (!::AthService::initialize().isSuccess()) {
    ATH_MSG_FATAL("Cannot initialize AthService base class.");
    return(StatusCode::FAILURE);
  }
  // Check for input collection
  if (m_primaryInputCollectionsProp.value().empty()) {
    ATH_MSG_FATAL("Use the property: EventSelector.PrimaryInputCollections = "
                  << "[ \"<collectionName>\" ] (list of collections)");
    return(StatusCode::FAILURE);
  }
  // Check for input collection
  if (m_secondaryInputCollectionsProp.value().empty()) {
    ATH_MSG_FATAL("Use the property: EventSelector.SecondaryInputCollections = "
                  << "[ \"<collectionName>\" ] (list of collections)");
    return(StatusCode::FAILURE);
  }
  m_skipEventSequence = m_skipEventSequenceProp.value();
  std::sort(m_skipEventSequence.begin(), m_skipEventSequence.end());
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
  ATH_CHECK(registerInputFileNames(m_primaryInputCollectionsProp.value(), &*iomgr));
  ATH_CHECK(registerInputFileNames(m_secondaryInputCollectionsProp.value(), &*iomgr));

  // Connect to PersistencySvc
  if (!m_athenaPoolCnvSvc->getPoolSvc()->connect(pool::ITransaction::READ, IPoolSvc::kInputStream).isSuccess()) {
    ATH_MSG_FATAL("Cannot connect to POOL PersistencySvc.");
    return(StatusCode::FAILURE);
  }
  return(this->reinit());
}
//________________________________________________________________________________
StatusCode DoubleEventSelectorAthenaPool::registerInputFileNames(const std::vector<std::string>& incol, IIoComponentMgr* iomgr) {
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
  return StatusCode::SUCCESS;
}
//________________________________________________________________________________
StatusCode DoubleEventSelectorAthenaPool::reinit() {
  ATH_MSG_INFO("reinitialization...");
  bool postInitHelperTools(true);
  ATH_CHECK(reinitSingle(m_primaryInputCollectionsProp.value(),
                         m_numPrimaryEvt, m_firstPrimaryEvt,
                         m_primaryGuid,
                         m_primaryInputCollectionsIterator,
                         m_curPrimaryCollection, postInitHelperTools,
                         m_primaryEventStreamingTool,
                         m_primaryPoolCollectionConverter,
                         m_processPrimaryMetadata.value(),
                         m_firedPrimaryIncident,
                         m_primaryHeaderIterator));
  postInitHelperTools = false;
  ATH_CHECK(reinitSingle(m_secondaryInputCollectionsProp.value(),
                         m_numSecondaryEvt, m_firstSecondaryEvt,
                         m_secondaryGuid,
                         m_secondaryInputCollectionsIterator,
                         m_curSecondaryCollection, postInitHelperTools,
                         m_secondaryEventStreamingTool,
                         m_secondaryPoolCollectionConverter,
                         m_processSecondaryMetadata.value(),
                         m_firedSecondaryIncident,
                         m_secondaryHeaderIterator));
  return StatusCode::SUCCESS;
}
//________________________________________________________________________________
StatusCode DoubleEventSelectorAthenaPool::reinitSingle(const std::vector<std::string>& inputCollections,
                                                       std::vector<int>& numEvt,
                                                       std::vector<int>& firstEvt,
                                                       Guid& inputGuid,
                                                       std::vector<std::string>::const_iterator& inputCollectionsIterator,
                                                       long& curCollection,
                                                       bool postInitHelperTools,
                                                       ToolHandle<IAthenaIPCTool>& eventStreamingTool,
                                                       PoolCollectionConverter*& poolCollectionConverter,
                                                       bool processMetadata,
                                                       bool& firedIncident,
                                                       pool::ICollectionCursor*& headerIterator) {
  // reset markers
  numEvt.resize(inputCollections.size(), -1);
  firstEvt.resize(inputCollections.size(), -1);
  inputGuid = Guid::null();

  // Initialize InputCollectionsIterators
  inputCollectionsIterator = inputCollections.begin();
  curCollection = 0;
  if (!firstEvt.empty()) {
    firstEvt[0] = 0;
  }
  m_evtCount = 0;
  if (!eventStreamingTool.empty() && eventStreamingTool->isClient()) {
    ATH_MSG_INFO("Done reinitialization for shared reader client");
    return(StatusCode::SUCCESS);
  }
  if (postInitHelperTools) { // HACK to only do this once for now.
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
  }
  ATH_MSG_INFO("EventSelection with query " << m_query.value());
  // Create PoolCollectionConverters to read the objects in
  poolCollectionConverter = getCollectionCnv(inputCollectionsIterator,
                                             inputCollections,
                                             curCollection, numEvt, firstEvt,
                                             processMetadata);
  if (poolCollectionConverter == nullptr) {
    ATH_MSG_INFO("No Events found in any Input Collections");
    if (processMetadata) {
      inputCollectionsIterator = inputCollections.end();
      if (!inputCollections.empty()) inputCollectionsIterator--;
      //NOTE (wb may 2016): this will make the FirstInputFile incident correspond to last file in the collection ... if want it to be first file then move iterator to begin and then move above two lines below this incident firing
      if (m_collectionType.value() == "ImplicitROOT" && !firedIncident && !inputCollections.empty()) {
        FileIncident firstInputFileIncident(name(), "FirstInputFile", *inputCollectionsIterator);
        m_incidentSvc->fireIncident(firstInputFileIncident);
        firedIncident = true;
      }
    }
    return(StatusCode::SUCCESS);
  }
  // Check for valid header name - TODO Do we need independent refName and collectionType values?
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
    headerIterator = &poolCollectionConverter->executeQuery();
  } catch (std::exception &e) {
    ATH_MSG_FATAL("Cannot open implicit collection - check data/software version.");
    ATH_MSG_ERROR(e.what());
    return(StatusCode::FAILURE);
  }
  while (headerIterator == nullptr || headerIterator->next() == 0) { // no selected events
    if (poolCollectionConverter != nullptr) {
      poolCollectionConverter->disconnectDb().ignore();
      delete poolCollectionConverter; poolCollectionConverter = nullptr;
    }
    inputCollectionsIterator++;
    poolCollectionConverter = getCollectionCnv(inputCollectionsIterator,
                                               inputCollections,
                                               curCollection, numEvt, firstEvt,
                                               processMetadata);
    if (poolCollectionConverter != nullptr) {
      headerIterator = &poolCollectionConverter->executeQuery();
    } else {
      break;
    }
  }
  if (poolCollectionConverter == nullptr || headerIterator == nullptr) { // no event selected in any collection
    inputCollectionsIterator = inputCollections.begin();
    curCollection = 0;
    poolCollectionConverter = getCollectionCnv(inputCollectionsIterator,
                                               inputCollections,
                                               curCollection, numEvt, firstEvt,
                                               processMetadata);
    if (poolCollectionConverter == nullptr) {
      return(StatusCode::SUCCESS);
    }
    headerIterator = &poolCollectionConverter->selectAll();
    while (headerIterator == nullptr || headerIterator->next() == 0) { // empty collection
      if (poolCollectionConverter != nullptr) {
        poolCollectionConverter->disconnectDb().ignore();
        delete poolCollectionConverter; poolCollectionConverter = nullptr;
      }
      inputCollectionsIterator++;
      poolCollectionConverter = getCollectionCnv(inputCollectionsIterator,
                                                 inputCollections,
                                                 curCollection, numEvt, firstEvt,
                                                 processMetadata);
      if (poolCollectionConverter != nullptr) {
        headerIterator = &poolCollectionConverter->selectAll();
      } else {
        break;
      }
    }
  }
  if (poolCollectionConverter == nullptr || headerIterator == nullptr) {
    return(StatusCode::SUCCESS);
  }

  Guid guid;
  int tech = 0;
  if (m_refName.value().empty()) {
    guid = headerIterator->eventRef().dbID();
    tech = headerIterator->eventRef().technology();
  } else {
    guid = headerIterator->currentRow().tokenList()[m_refName.value() + "_ref"].dbID();
    tech = headerIterator->currentRow().tokenList()[m_refName.value() + "_ref"].technology();
  }
  // Check if File is BS, for which Incident is thrown by SingleEventInputSvc
  if (tech != 0x00001000 && processMetadata && !firedIncident) {
    FileIncident firstInputFileIncident(name(), "FirstInputFile", "FID:" + guid.toString(), "FID:" + guid.toString());
    m_incidentSvc->fireIncident(firstInputFileIncident);
    firedIncident = true;
  }
  return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode DoubleEventSelectorAthenaPool::start() {
  if (m_primaryPoolCollectionConverter != nullptr) {
    // Reset iterators and apply new query
    m_primaryPoolCollectionConverter->disconnectDb().ignore();
    delete m_primaryPoolCollectionConverter; m_primaryPoolCollectionConverter = nullptr;
  }
  if (m_secondaryPoolCollectionConverter != nullptr) {
    // Reset iterators and apply new query
    m_secondaryPoolCollectionConverter->disconnectDb().ignore();
    delete m_secondaryPoolCollectionConverter; m_secondaryPoolCollectionConverter = nullptr;
  }
  m_primaryInputCollectionsIterator = m_primaryInputCollectionsProp.value().begin();
  m_curPrimaryCollection = 0;

  m_secondaryInputCollectionsIterator = m_secondaryInputCollectionsProp.value().begin();
  m_curSecondaryCollection = 0;

  m_primaryPoolCollectionConverter = getCollectionCnv(m_primaryInputCollectionsIterator,
                                                      m_primaryInputCollectionsProp.value(),
                                                      m_curPrimaryCollection,
                                                      m_numPrimaryEvt, m_firstPrimaryEvt,
                                                      m_processPrimaryMetadata.value(),true);
  if (m_primaryPoolCollectionConverter == nullptr) {
    ATH_MSG_INFO("No Events found in any Input Collections");
    m_primaryInputCollectionsIterator = m_primaryInputCollectionsProp.value().end();
    if (!m_primaryInputCollectionsProp.value().empty()) {
      m_primaryInputCollectionsIterator--; //leave iterator in state of last input file
    }
  } else {
    m_primaryHeaderIterator = &m_primaryPoolCollectionConverter->executeQuery(/*m_query.value()*/);
  }
  m_secondaryPoolCollectionConverter = getCollectionCnv(m_secondaryInputCollectionsIterator,
                                                        m_secondaryInputCollectionsProp.value(),
                                                        m_curSecondaryCollection,
                                                        m_numSecondaryEvt, m_firstSecondaryEvt,
                                                        m_processSecondaryMetadata.value(),true);
  if (m_secondaryPoolCollectionConverter == nullptr) {
    ATH_MSG_INFO("No Events found in any Input Collections");
    m_secondaryInputCollectionsIterator = m_secondaryInputCollectionsProp.value().end();
    if (!m_secondaryInputCollectionsProp.value().empty()) {
      m_secondaryInputCollectionsIterator--; //leave iterator in state of last input file
    }
  } else {
    m_secondaryHeaderIterator = &m_secondaryPoolCollectionConverter->executeQuery(/*m_query.value()*/);
  }
  delete m_beginIter; m_beginIter = nullptr;
  m_beginIter = new EventContextAthenaPool(this);
  delete m_endIter;   m_endIter   = nullptr;
  m_endIter = new EventContextAthenaPool(nullptr);
  return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode DoubleEventSelectorAthenaPool::stop() {
  IEvtSelector::Context* ctxt(nullptr);
  if (!releaseContext(ctxt).isSuccess()) {
    ATH_MSG_WARNING("Cannot release context");
  }
  return(StatusCode::SUCCESS);
}

//________________________________________________________________________________
void DoubleEventSelectorAthenaPool::fireEndFileIncidents(bool processMetadata, const Guid& inputGuid, bool isLastFile, bool& firedIncident) const {
  if (processMetadata) {
    if (m_evtCount >= 0) {
      // Assume that the end of collection file indicates the end of payload file.
      if (inputGuid != Guid::null()) {
        // Fire EndInputFile incident
         FileIncident endInputFileIncident(name(), "EndInputFile", "FID:" + inputGuid.toString(), inputGuid.toString());
        m_incidentSvc->fireIncident(endInputFileIncident);
      }
    }
    if (isLastFile && firedIncident) {
      firedIncident = false;
    }
  }
}

//________________________________________________________________________________
StatusCode DoubleEventSelectorAthenaPool::finalize() {
  delete m_beginIter; m_beginIter = nullptr;
  delete m_endIter;   m_endIter   = nullptr;
  m_primaryHeaderIterator = nullptr;
  if (m_primaryPoolCollectionConverter != nullptr) {
    delete m_primaryPoolCollectionConverter; m_primaryPoolCollectionConverter = nullptr;
  }
  m_secondaryHeaderIterator = nullptr;
  if (m_secondaryPoolCollectionConverter != nullptr) {
    delete m_secondaryPoolCollectionConverter; m_secondaryPoolCollectionConverter = nullptr;
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
StatusCode DoubleEventSelectorAthenaPool::queryInterface(const InterfaceID& riid, void** ppvInterface) {
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
  } else {
    return(::AthService::queryInterface(riid, ppvInterface));
  }
  addRef();
  return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode DoubleEventSelectorAthenaPool::createContext(IEvtSelector::Context*& ctxt) const {
  ctxt = new EventContextAthenaPool(this);
  return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode DoubleEventSelectorAthenaPool::next(IEvtSelector::Context& ctxt) const {
  std::lock_guard<CallMutex> lockGuard(m_callLock);

  for (const auto& tool : m_helperTools) {
    if (!tool->preNext().isSuccess()) {
      ATH_MSG_WARNING("Failed to preNext() " << tool->name());
    }
  }
  for (;;) {
    if (m_primaryHeaderIterator == nullptr || m_primaryHeaderIterator->next() == 0) {
      m_primaryHeaderIterator = nullptr;
      // Close previous collection.
      if (!m_keepInputFilesOpen.value() && m_primaryPoolCollectionConverter != nullptr) {
        m_primaryPoolCollectionConverter->disconnectDb().ignore();
      }
      delete m_primaryPoolCollectionConverter; m_primaryPoolCollectionConverter = nullptr;
      // Assume that the end of collection file indicates the end of payload file.
      if (m_processPrimaryMetadata.value()) {
        // Fire EndInputFile incident
         FileIncident endInputFileIncident(name(), "EndInputFile", "FID:" + m_primaryGuid.toString(), m_primaryGuid.toString());
        m_incidentSvc->fireIncident(endInputFileIncident);
      }
      // zero the current DB ID (m_primaryGuid) before disconnect() to indicate it is no longer in use
      auto old_guid = m_primaryGuid;
      m_primaryGuid = Guid::null();
      disconnectIfFinished( old_guid.toString() );
      // Open next file from inputCollections list.
      m_primaryInputCollectionsIterator++;
      // Create PoolCollectionConverter for input file
      m_primaryPoolCollectionConverter = getCollectionCnv(m_primaryInputCollectionsIterator,
                                                          m_primaryInputCollectionsProp.value(),
                                                          m_curPrimaryCollection,
                                                          m_numPrimaryEvt, m_firstPrimaryEvt,
                                                          m_processPrimaryMetadata.value(),true);
      if (m_primaryPoolCollectionConverter == nullptr) {
        // Return end iterator
        ctxt = *m_endIter;
        return(StatusCode::FAILURE);
      }
      // Get DataHeader iterator
      m_primaryHeaderIterator = &m_primaryPoolCollectionConverter->executeQuery();
      continue; // handles empty files
    }
    if (m_secondaryHeaderIterator == nullptr || m_secondaryHeaderIterator->next() == 0) {
      m_secondaryHeaderIterator = nullptr;
      // Close previous collection.
      if (!m_keepInputFilesOpen.value() && m_secondaryPoolCollectionConverter != nullptr) {
        m_secondaryPoolCollectionConverter->disconnectDb().ignore();
      }
      delete m_secondaryPoolCollectionConverter; m_secondaryPoolCollectionConverter = nullptr;
      // Assume that the end of collection file indicates the end of payload file.
      if (m_processSecondaryMetadata.value()) {
        // Fire EndInputFile incident
         FileIncident endInputFileIncident(name(), "EndInputFile", "FID:" + m_secondaryGuid.toString(), m_secondaryGuid.toString());
        m_incidentSvc->fireIncident(endInputFileIncident);
      }
      // zero the current DB ID (m_secondaryGuid) before disconnect() to indicate it is no longer in use
      auto old_guid = m_secondaryGuid;
      m_secondaryGuid = Guid::null();
      disconnectIfFinished( old_guid.toString() );
      // Open next file from inputCollections list.
      m_secondaryInputCollectionsIterator++;
      // Create PoolCollectionConverter for input file
      m_secondaryPoolCollectionConverter = getCollectionCnv(m_secondaryInputCollectionsIterator,
                                                            m_secondaryInputCollectionsProp.value(),
                                                            m_curSecondaryCollection,
                                                            m_numSecondaryEvt, m_firstSecondaryEvt,
                                                            m_processSecondaryMetadata.value(),true);
      if (m_secondaryPoolCollectionConverter == nullptr) {
        // Return end iterator
        ctxt = *m_endIter;
        return(StatusCode::FAILURE);
      }
      // Get DataHeader iterator
      m_secondaryHeaderIterator = &m_secondaryPoolCollectionConverter->executeQuery();
      continue; // handles empty files
    }
    Guid guid;
    int tech = 0;
    if (m_refName.value().empty()) {
      guid = m_primaryHeaderIterator->eventRef().dbID();
      tech = m_primaryHeaderIterator->eventRef().technology();
    } else {
      Token token;
      m_primaryHeaderIterator->currentRow().tokenList()[m_refName.value() + "_ref"].setData(&token);
      guid = token.dbID();
      tech = token.technology();
    }
    if (guid != m_primaryGuid) {
      ATH_MSG_DEBUG("guid != m_primaryGuid");
      if (m_primaryGuid != Guid::null()) {
        if (m_processPrimaryMetadata.value()) {
          // Fire EndInputFile incident
           FileIncident endInputFileIncident(name(), "EndInputFile", "FID:" + m_primaryGuid.toString(),  m_primaryGuid.toString());
          m_incidentSvc->fireIncident(endInputFileIncident);
        }
        // zero the current DB ID (m_primaryGuid) before disconnect() to indicate it is no longer in use
        auto old_guid = m_primaryGuid;
        m_primaryGuid = Guid::null();
        disconnectIfFinished(old_guid.toString());
      }
      m_primaryGuid = guid;
      // Fire BeginInputFile incident if current InputCollection is a payload file;
      // otherwise, ascertain whether the pointed-to file is reachable before firing any incidents and/or proceeding
      if (m_collectionType.value() == "ImplicitROOT") {
        // For now, we can only deal with input metadata from POOL files, but we know we have a POOL file here
        if (!m_athenaPoolCnvSvc->setInputAttributes(*m_primaryInputCollectionsIterator).isSuccess()) {
          ATH_MSG_ERROR("Failed to set input attributes.");
          return(StatusCode::FAILURE);
        }
        if (m_processPrimaryMetadata.value()) {
          FileIncident beginInputFileIncident(name(), "BeginInputFile", *m_primaryInputCollectionsIterator, "FID:" + m_primaryGuid.toString());
          m_incidentSvc->fireIncident(beginInputFileIncident);
        }
      } else {
        // Check if File is BS
        if (tech != 0x00001000 && m_processPrimaryMetadata.value()) {
          FileIncident beginInputFileIncident(name(), "BeginInputFile", "FID:" + m_primaryGuid.toString());
          m_incidentSvc->fireIncident(beginInputFileIncident);
        }
      }
    }  // end if (guid != m_primaryGuid)
    if (m_refName.value().empty()) {
      guid = m_secondaryHeaderIterator->eventRef().dbID();
      tech = m_secondaryHeaderIterator->eventRef().technology();
    } else {
      Token token;
      m_secondaryHeaderIterator->currentRow().tokenList()[m_refName.value() + "_ref"].setData(&token);
      guid = token.dbID();
      tech = token.technology();
    }
    if (guid != m_secondaryGuid) {
      ATH_MSG_DEBUG("guid != m_secondaryGuid");
      if (m_secondaryGuid != Guid::null()) {
        if (m_processSecondaryMetadata.value()) {
          // Fire EndInputFile incident
           FileIncident endInputFileIncident(name(), "EndInputFile", "FID:" + m_secondaryGuid.toString(), m_secondaryGuid.toString());
          m_incidentSvc->fireIncident(endInputFileIncident);
        }
        // zero the current DB ID (m_secondaryGuid) before disconnect() to indicate it is no longer in use
        auto old_guid = m_secondaryGuid;
        m_secondaryGuid = Guid::null();
        disconnectIfFinished(old_guid.toString());
      }
      m_secondaryGuid = guid;
      // Fire BeginInputFile incident if current InputCollection is a payload file;
      // otherwise, ascertain whether the pointed-to file is reachable before firing any incidents and/or proceeding
      if (m_collectionType.value() == "ImplicitROOT") {
        // For now, we can only deal with input metadata from POOL files, but we know we have a POOL file here
        if (!m_athenaPoolCnvSvc->setInputAttributes(*m_secondaryInputCollectionsIterator).isSuccess()) {
          ATH_MSG_ERROR("Failed to set input attributes.");
          return(StatusCode::FAILURE);
        }
        if (m_processSecondaryMetadata.value()) {
          FileIncident beginInputFileIncident(name(), "BeginInputFile", *m_secondaryInputCollectionsIterator, "FID:" + m_secondaryGuid.toString());
          m_incidentSvc->fireIncident(beginInputFileIncident);
        }
      } else {
        // Check if File is BS
        if (tech != 0x00001000 && m_processSecondaryMetadata.value()) {
          FileIncident beginInputFileIncident(name(), "BeginInputFile", "FID:" + m_secondaryGuid.toString());
          m_incidentSvc->fireIncident(beginInputFileIncident);
        }
      }
    }  // end if (guid != m_secondaryGuid)
    ++m_evtCount;
    if (!m_counterTool.empty() && !m_counterTool->preNext().isSuccess()) {
      ATH_MSG_WARNING("Failed to preNext() CounterTool.");
    }
    if (m_evtCount > m_skipEvents && (m_skipEventSequence.empty() || m_evtCount != m_skipEventSequence.front())) {
      if (!eventStore()->clearStore().isSuccess()) {
          ATH_MSG_WARNING("Cannot clear Store");
      }

      ATH_MSG_DEBUG("Try recording AttributeLists");
      if (!recordAllAttributeList().isSuccess()) {
        ATH_MSG_ERROR("Failed to record AttributeLists.");
        return(StatusCode::FAILURE);
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
      if (!m_skipEventSequence.empty() && m_evtCount == m_skipEventSequence.front()) {
        m_skipEventSequence.erase(m_skipEventSequence.begin());
      }
      ATH_MSG_INFO("skipping event " << m_evtCount);
    }
  }
  return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode DoubleEventSelectorAthenaPool::next(IEvtSelector::Context& ctxt, int jump) const {
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
StatusCode DoubleEventSelectorAthenaPool::previous(IEvtSelector::Context& /*ctxt*/) const {
  ATH_MSG_ERROR("previous() not implemented");
  return(StatusCode::FAILURE);
}
//________________________________________________________________________________
StatusCode DoubleEventSelectorAthenaPool::previous(IEvtSelector::Context& ctxt, int jump) const {
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
StatusCode DoubleEventSelectorAthenaPool::last(IEvtSelector::Context& ctxt) const {
  if (ctxt.identifier() == m_endIter->identifier()) {
    ATH_MSG_DEBUG("last(): Last event in InputStream.");
    return(StatusCode::SUCCESS);
  }
  return(StatusCode::FAILURE);
}
//________________________________________________________________________________
StatusCode DoubleEventSelectorAthenaPool::rewind(IEvtSelector::Context& /*ctxt*/) const {
  ATH_MSG_ERROR("DoubleEventSelectorAthenaPool::rewind() not implemented");
  return(StatusCode::FAILURE);
}
//________________________________________________________________________________
StatusCode DoubleEventSelectorAthenaPool::createAddress(const IEvtSelector::Context& /*ctxt*/,
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
    tokenStr = m_primaryPoolCollectionConverter->retrieveToken(m_primaryHeaderIterator, m_refName.value());
  }
  Token* token = new Token;
  token->fromString(tokenStr);
  iop = new TokenAddress(POOL_StorageType, ClassID_traits<DataHeader>::ID(), "", "EventSelector", IPoolSvc::kInputStream, token);
  return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode DoubleEventSelectorAthenaPool::releaseContext(IEvtSelector::Context*& /*ctxt*/) const {
  return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode DoubleEventSelectorAthenaPool::resetCriteria(const std::string& /*criteria*/,
                                                        IEvtSelector::Context& /*ctxt*/) const {
  return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode DoubleEventSelectorAthenaPool::seek(Context& /*ctxt*/, int evtNum) const {
  long newColl1 = findEvent(evtNum, m_numPrimaryEvt, m_firstPrimaryEvt, m_primaryInputCollectionsProp.value());
  if (newColl1 == -1 && evtNum >= m_firstPrimaryEvt[m_curPrimaryCollection] && evtNum < m_evtCount - 1) {
    newColl1 = m_curPrimaryCollection;
  }
  if (newColl1 == -1) {
    m_primaryHeaderIterator = nullptr;
    ATH_MSG_INFO("seek: Reached end of Input.");
    fireEndFileIncidents(m_processPrimaryMetadata.value(), m_primaryGuid, true, m_firedPrimaryIncident);
    return(StatusCode::RECOVERABLE);
  }

  long newColl2 = findEvent(evtNum, m_numSecondaryEvt, m_firstSecondaryEvt, m_secondaryInputCollectionsProp.value());
  if (newColl2 == -1 && evtNum >= m_firstSecondaryEvt[m_curSecondaryCollection] && evtNum < m_evtCount - 1) {
    newColl2 = m_curSecondaryCollection;
  }
  if (newColl2 == -1) {
    m_secondaryHeaderIterator = nullptr;
    ATH_MSG_INFO("seek: Reached end of Input.");
    fireEndFileIncidents(m_processSecondaryMetadata.value(), m_secondaryGuid, true, m_firedSecondaryIncident);
    return(StatusCode::RECOVERABLE);
  }

  bool resetPrimary(false);
  bool resetSecondary(false);
  if (newColl1 != m_curPrimaryCollection) {
    resetPrimary=true;
    if (!m_keepInputFilesOpen.value() && m_primaryPoolCollectionConverter != nullptr) {
      m_primaryPoolCollectionConverter->disconnectDb().ignore();
    }
    delete m_primaryPoolCollectionConverter; m_primaryPoolCollectionConverter = nullptr;
    m_curPrimaryCollection = newColl1;
  }
  if (newColl2 != m_curSecondaryCollection) {
    resetSecondary = true;
    if (!m_keepInputFilesOpen.value() && m_secondaryPoolCollectionConverter != nullptr) {
      m_secondaryPoolCollectionConverter->disconnectDb().ignore();
    }
    delete m_secondaryPoolCollectionConverter; m_secondaryPoolCollectionConverter = nullptr;
    m_curSecondaryCollection = newColl2;
  }
  if (resetPrimary || resetSecondary) {
    try {
      if (resetPrimary) {
        ATH_MSG_DEBUG("Seek to item: \""
                      <<  m_primaryInputCollectionsProp.value()[m_curPrimaryCollection]
                      << "\" from the collection list.");
        // Reset input collection iterator to the right place
        m_primaryInputCollectionsIterator = m_primaryInputCollectionsProp.value().begin();
        m_primaryInputCollectionsIterator += m_curPrimaryCollection;
        m_primaryPoolCollectionConverter = new PoolCollectionConverter(m_collectionType.value() + ":" + m_collectionTree.value(),
                                                                       m_primaryInputCollectionsProp.value()[m_curPrimaryCollection],
                                                                       m_query.value(),
                                                                       m_athenaPoolCnvSvc->getPoolSvc());
        if (!m_primaryPoolCollectionConverter->initialize().isSuccess()) {
          m_primaryHeaderIterator = nullptr;
          ATH_MSG_ERROR("seek: Unable to initialize PoolCollectionConverter.");
          return(StatusCode::FAILURE);
        }
        // Create DataHeader iterators
        m_primaryHeaderIterator = &m_primaryPoolCollectionConverter->executeQuery();
      }
      if (resetSecondary) {
        ATH_MSG_DEBUG("Seek to item: \""
                      <<  m_secondaryInputCollectionsProp.value()[m_curSecondaryCollection]
                      << "\" from the collection list.");
        // Reset input collection iterator to the right place
        m_secondaryInputCollectionsIterator = m_secondaryInputCollectionsProp.value().begin();
        m_secondaryInputCollectionsIterator += m_curSecondaryCollection;
        m_secondaryPoolCollectionConverter = new PoolCollectionConverter(m_collectionType.value() + ":" + m_collectionTree.value(),
                                                                         m_secondaryInputCollectionsProp.value()[m_curSecondaryCollection],
                                                                         m_query.value(),
                                                                         m_athenaPoolCnvSvc->getPoolSvc());
        if (!m_secondaryPoolCollectionConverter->initialize().isSuccess()) {
          m_secondaryHeaderIterator = nullptr;
          ATH_MSG_ERROR("seek: Unable to initialize PoolCollectionConverter.");
          return(StatusCode::FAILURE);
        }
        // Create DataHeader iterators
        m_secondaryHeaderIterator = &m_secondaryPoolCollectionConverter->executeQuery();
      }
      delete m_beginIter; m_beginIter = nullptr;
      m_beginIter = new EventContextAthenaPool(this);
      next(*m_beginIter).ignore();
      ATH_MSG_DEBUG("Primary Token " << m_primaryHeaderIterator->eventRef().toString());
      ATH_MSG_DEBUG("Secondary Token " << m_secondaryHeaderIterator->eventRef().toString());
    } catch (std::exception &e) {
      m_primaryHeaderIterator = nullptr;
      m_secondaryHeaderIterator = nullptr;
      ATH_MSG_ERROR(e.what());
      return(StatusCode::FAILURE);
    }
  }

  pool::IPositionSeek* is = dynamic_cast<pool::IPositionSeek*>(m_primaryHeaderIterator);
  if (is == nullptr) {
    ATH_MSG_ERROR("Container does not allow seeking.");
    return(StatusCode::FAILURE);
  }
  if (is->seek(evtNum - m_firstPrimaryEvt[m_curPrimaryCollection]) == 0) {
    m_primaryHeaderIterator = nullptr;
    ATH_MSG_ERROR("Did not find event, evtNum = " << evtNum);
    return(StatusCode::FAILURE);
  } else {
    m_evtCount = evtNum + 1;
  }

  return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
int DoubleEventSelectorAthenaPool::curEvent (const Context& /*ctxt*/) const {
  return(m_evtCount);
}
//__________________________________________________________________________
// Search for event number evtNum.
// Return the index of the collection containing it, or -1 if not found.
// Note: passing -1 for evtNum will always yield failure,
// but this can be used to force filling in the entire m_numPrimaryEvt array.
int DoubleEventSelectorAthenaPool::findEvent(int evtNum, std::vector<int>& numEvt, std::vector<int>& firstEvt, const std::vector<std::string>& inputCollections) const {
  for (std::size_t i = 0, imax = numEvt.size(); i < imax; i++) {
    if (numEvt[i] == -1) {
      PoolCollectionConverter pcc(m_collectionType.value() + ":" + m_collectionTree.value(),
                                  inputCollections[i],
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
        firstEvt[i] = firstEvt[i - 1] + numEvt[i - 1];
      } else {
        firstEvt[i] = 0;
      }
      numEvt[i] = collection_size;
    }
    if (evtNum >= firstEvt[i] && evtNum < firstEvt[i] + numEvt[i]) {
      return(i);
    }
  }
  return(-1);
}

//__________________________________________________________________________
int DoubleEventSelectorAthenaPool::size(Context& /*ctxt*/) const {
  // Fetch sizes of all collections.
  findEvent(-1, m_numPrimaryEvt, m_firstPrimaryEvt, m_primaryInputCollectionsProp.value());
  int sz1 = 0;
  for (std::size_t i = 0, imax = m_numPrimaryEvt.size(); i < imax; i++) {
    sz1 += m_numPrimaryEvt[i];
  }
  findEvent(-1, m_numSecondaryEvt, m_firstSecondaryEvt, m_secondaryInputCollectionsProp.value());
  int sz2 = 0;
  for (std::size_t i = 0, imax = m_numSecondaryEvt.size(); i < imax; i++) {
    sz2 += m_numSecondaryEvt[i];
  }
  if(sz2<sz1) {
    ATH_MSG_WARNING("Fewer secondary input events than primary input events. Expect trouble!");
  }
  return(sz1);
}
//__________________________________________________________________________
PoolCollectionConverter* DoubleEventSelectorAthenaPool::getCollectionCnv(std::vector<std::string>::const_iterator& inputCollectionsIterator,
                                                                         const std::vector<std::string>& inputCollections,
                                                                         long& curCollection, std::vector<int>& numEvt, std::vector<int>& firstEvt,
                                                                         bool processMetadata, bool throwIncidents) const {
  while (inputCollectionsIterator != inputCollections.end()) {
    if (curCollection != 0) {
      numEvt[curCollection] = m_evtCount - firstEvt[curCollection];
      curCollection++;
      firstEvt[curCollection] = m_evtCount;
    }
    ATH_MSG_DEBUG("Try item: \"" << *inputCollectionsIterator << "\" from the collection list.");
    PoolCollectionConverter* pCollCnv = new PoolCollectionConverter(m_collectionType.value() + ":" + m_collectionTree.value(),
                                                                    *inputCollectionsIterator,
                                                                    m_query.value(),
                                                                    m_athenaPoolCnvSvc->getPoolSvc());
    StatusCode status = pCollCnv->initialize();
    if (!status.isSuccess()) {
      // Close previous collection.
      delete pCollCnv; pCollCnv = nullptr;
      if (!status.isRecoverable()) {
        ATH_MSG_ERROR("Unable to initialize PoolCollectionConverter.");
        throw GaudiException("Unable to read: " + *inputCollectionsIterator, name(), StatusCode::FAILURE);
      } else {
        ATH_MSG_ERROR("Unable to open: " << *inputCollectionsIterator);
        throw GaudiException("Unable to open: " + *inputCollectionsIterator, name(), StatusCode::FAILURE);
      }
    } else {
      if (!pCollCnv->isValid().isSuccess()) {
        delete pCollCnv; pCollCnv = nullptr;
        ATH_MSG_DEBUG("No events found in: " << *inputCollectionsIterator << " skipped!!!");
        if (throwIncidents && processMetadata) {
          FileIncident beginInputFileIncident(name(), "BeginInputFile", *inputCollectionsIterator);
          m_incidentSvc->fireIncident(beginInputFileIncident);
          FileIncident endInputFileIncident(name(), "EndInputFile", "eventless " + *inputCollectionsIterator);
          m_incidentSvc->fireIncident(endInputFileIncident);
        }
        m_athenaPoolCnvSvc->getPoolSvc()->disconnectDb(*inputCollectionsIterator).ignore();
        inputCollectionsIterator++;
      } else {
        return(pCollCnv);
      }
    }
  }
  return(nullptr);
}

//__________________________________________________________________________
StatusCode DoubleEventSelectorAthenaPool::recordAllAttributeList() const {
  // Get access to AttributeList
  ATH_MSG_DEBUG("Get AttributeList from the collection");
  // MN: accessing only attribute list, ignoring token list
  const coral::AttributeList& attrList = m_primaryHeaderIterator->currentRow().attributeList();
  ATH_MSG_DEBUG("AttributeList size " << attrList.size());
  auto athAttrList = std::make_unique<AthenaAttributeList>(attrList);
  const pool::TokenList& tokenList = m_primaryHeaderIterator->currentRow().tokenList();
  for (pool::TokenList::const_iterator iter = tokenList.begin(), last = tokenList.end(); iter != last; ++iter) {
    athAttrList->extend(iter.tokenName(), "string");
    (*athAttrList)[iter.tokenName()].data<std::string>() = iter->toString();
    ATH_MSG_DEBUG("record AthenaAttribute, name = " << iter.tokenName() << " = " << iter->toString() << ".");
  }
  athAttrList->extend("eventRef", "string");
  (*athAttrList)["eventRef"].data<std::string>() = m_primaryHeaderIterator->eventRef().toString();

  ATH_MSG_DEBUG("Append secondary attribute list properties to the primary one with a suffix: " << m_secondaryAttrListSuffix.value());
  std::string suffix = "_" + m_secondaryAttrListSuffix.value();

  const pool::TokenList& extraTokenList = m_secondaryHeaderIterator->currentRow().tokenList();
  for (pool::TokenList::const_iterator iter = extraTokenList.begin(), last = extraTokenList.end(); iter != last; ++iter) {
    athAttrList->extend(iter.tokenName() + suffix, "string");
    (*athAttrList)[iter.tokenName() + suffix].data<std::string>() = iter->toString();
    ATH_MSG_DEBUG("record AthenaAttribute, name = " << iter.tokenName() + suffix << " = " << iter->toString() << ".");
  }
  athAttrList->extend("eventRef" + suffix, "string");
  (*athAttrList)["eventRef" + suffix].data<std::string>() = m_secondaryHeaderIterator->eventRef().toString();
  ATH_MSG_DEBUG("record AthenaAttribute, name = eventRef" + suffix + " = " << m_secondaryHeaderIterator->eventRef().toString() << ".");

  // copy all atributes from extra attribute list
  // to the primary attribute list with a suffix 
  athAttrList->extend("hasSecondaryInput", "bool");
  (*athAttrList)["hasSecondaryInput"].data<bool>() = true;
  const coral::AttributeList& extraAttrList = m_secondaryHeaderIterator->currentRow().attributeList();
  for (const auto &attr : extraAttrList) {
    athAttrList->extend(attr.specification().name() + suffix, attr.specification().type());
    (*athAttrList)[attr.specification().name() + suffix] = attr;
  }

  ATH_MSG_DEBUG("record AthenaAttribute, name = eventRef = " << m_primaryHeaderIterator->eventRef().toString() << ".");
  SG::WriteHandle<AthenaAttributeList> wh(m_attrListKey.value(), eventStore()->name());
  if (!wh.record(std::move(athAttrList)).isSuccess()) {
    ATH_MSG_ERROR("Cannot record AttributeList to StoreGate " << StoreID::storeName(eventStore()->storeID()));
    return(StatusCode::FAILURE);
  }
  return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode DoubleEventSelectorAthenaPool::io_reinit() {
  ATH_MSG_INFO("I/O reinitialization...");
  if (m_primaryPoolCollectionConverter != nullptr) {
    m_primaryPoolCollectionConverter->disconnectDb().ignore();
    delete m_primaryPoolCollectionConverter; m_primaryPoolCollectionConverter = nullptr;
  }
  m_primaryHeaderIterator = nullptr;
  if (m_secondaryPoolCollectionConverter != nullptr) {
    m_secondaryPoolCollectionConverter->disconnectDb().ignore();
    delete m_secondaryPoolCollectionConverter; m_secondaryPoolCollectionConverter = nullptr;
  }
  m_secondaryHeaderIterator = nullptr;
  ServiceHandle<IIoComponentMgr> iomgr("IoComponentMgr", name());
  if (!iomgr.retrieve().isSuccess()) {
    ATH_MSG_FATAL("Could not retrieve IoComponentMgr !");
    return(StatusCode::FAILURE);
  }
  if (!iomgr->io_hasitem(this)) {
    ATH_MSG_FATAL("IoComponentMgr does not know about myself !");
    return(StatusCode::FAILURE);
  }
  std::vector<std::string> primaryInputCollections = m_primaryInputCollectionsProp.value();
  std::set<std::size_t> updatedIndexes;
  for (std::size_t i = 0, imax = m_primaryInputCollectionsProp.value().size(); i < imax; i++) {
    if (updatedIndexes.find(i) != updatedIndexes.end()) continue;
    std::string savedName = primaryInputCollections[i];
    std::string &fname = primaryInputCollections[i];
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
      if (primaryInputCollections[j] == savedName) {
        primaryInputCollections[j] = fname;
        updatedIndexes.insert(j);
      }
    }
  }
  // all good... copy over.
  m_primaryInputCollectionsProp = primaryInputCollections; // triggers reinit()
  std::vector<std::string> secondaryInputCollections = m_secondaryInputCollectionsProp.value();
  updatedIndexes.clear();
  for (std::size_t i = 0, imax = m_secondaryInputCollectionsProp.value().size(); i < imax; i++) {
    if (updatedIndexes.find(i) != updatedIndexes.end()) continue;
    std::string savedName = secondaryInputCollections[i];
    std::string &fname = secondaryInputCollections[i];
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
      if (secondaryInputCollections[j] == savedName) {
        secondaryInputCollections[j] = fname;
        updatedIndexes.insert(j);
      }
    }
  }
  // all good... copy over.
  m_secondaryInputCollectionsProp = secondaryInputCollections; // triggers reinit()
  return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode DoubleEventSelectorAthenaPool::io_finalize() {
  ATH_MSG_INFO("I/O finalization...");
  if (m_primaryPoolCollectionConverter != nullptr) {
    m_primaryPoolCollectionConverter->disconnectDb().ignore();
    delete m_primaryPoolCollectionConverter; m_primaryPoolCollectionConverter = nullptr;
  }
  if (m_secondaryPoolCollectionConverter != nullptr) {
    m_secondaryPoolCollectionConverter->disconnectDb().ignore();
    delete m_secondaryPoolCollectionConverter; m_secondaryPoolCollectionConverter = nullptr;
  }
  return(StatusCode::SUCCESS);
}

//__________________________________________________________________________
/* Listen to IncidentType::BeginProcessing and EndProcessing
   Maintain counters of how many events from a given file are being processed.
   Files are identified by SG::SourceID (string GUID).
   When there are no more events from a file, see if it can be closed.
*/
void DoubleEventSelectorAthenaPool::handle(const Incident& inc)
{
  if (not Atlas::hasExtendedEventContext(inc.context()) ) {
    ATH_MSG_WARNING("No extended event context available.");
    return;
  }

  const SGImplSvc *sg = static_cast<SGImplSvc *>(Atlas::getExtendedEventContext(inc.context()).proxy());

  // Primary guid
  SG::SourceID fid1;
  SG::DataProxy* dp1 = sg->proxy(ClassID_traits<DataHeader>::ID(), "EventSelector", true);
  if (dp1) {
    const DataHeader* dh1 = SG::DataProxy_cast<DataHeader> (dp1);
    if (dh1) {
      fid1 =  dh1->begin()->getToken()->dbID().toString();
    }
  }

  if( fid1.empty() ) {
    ATH_MSG_WARNING("could not read event source ID from incident event context with key EventSelector");
    return;
  }

  ATH_MSG_DEBUG("**  MN Incident handler " << inc.type() << " Event source ID=" << fid1 );
  if( inc.type() == IncidentType::BeginProcessing ) {
    // increment the events-per-file counter for FID
    m_activeEventsPerSource[fid1]++;
  } else if( inc.type() == IncidentType::EndProcessing ) {
    m_activeEventsPerSource[fid1]--;
    disconnectIfFinished( fid1 );
  }
  if( msgLvl(MSG::DEBUG) ) {
    for( auto& source: m_activeEventsPerSource )
      msg(MSG::DEBUG) << "SourceID: " << source.first << " active events: " << source.second << endmsg;
  }

  // Secondary guid
  SG::SourceID fid2;
  SG::DataProxy* dp2 = sg->proxy(ClassID_traits<DataHeader>::ID(), "SecondaryEventSelector", true);
  if (dp2) {
    const DataHeader* dh2 = SG::DataProxy_cast<DataHeader> (dp2);
    if (dh2) {
      fid2 =  dh2->begin()->getToken()->dbID().toString();
    }
  }

  if( fid2.empty() ) {
    ATH_MSG_WARNING("could not read event source ID from incident event context with key SecondaryEventSelector");
    return;
  }

  ATH_MSG_DEBUG("**  MN Incident handler " << inc.type() << " Event source ID=" << fid2 );
  if( inc.type() == IncidentType::BeginProcessing ) {
    // increment the events-per-file counter for FID
    m_activeEventsPerSource[fid2]++;
  } else if( inc.type() == IncidentType::EndProcessing ) {
    m_activeEventsPerSource[fid2]--;
    disconnectIfFinished( fid2 );
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
bool DoubleEventSelectorAthenaPool::disconnectIfFinished( SG::SourceID fid ) const
{
   if( m_activeEventsPerSource[fid] <= 0 && m_primaryGuid != fid && m_secondaryGuid != fid ) {
      // Explicitly disconnect file corresponding to old FID to release memory
      if( !m_keepInputFilesOpen.value() ) {
         ATH_MSG_INFO("Disconnecting input sourceID: " << fid );
         m_athenaPoolCnvSvc->getPoolSvc()->disconnectDb("FID:" + fid, IPoolSvc::kInputStream).ignore();
         m_activeEventsPerSource.erase( fid );
         return true;
      }
   }
   return false;
}

/*
  These are needed to satisfy the requirements of the pure virtual functions
  in the interface, but there are no implementations for the DoubleEventSelector.
*/
//________________________________________________________________________________
StatusCode DoubleEventSelectorAthenaPool::makeServer(__attribute__((unused)) int num) {
  ATH_MSG_FATAL("makeServer() not implemented in the DoubleEventSelector");
  return(StatusCode::FAILURE);
}

//________________________________________________________________________________
StatusCode DoubleEventSelectorAthenaPool::makeClient(__attribute__((unused)) int num) {
  ATH_MSG_FATAL("makeClient() not implemented in the DoubleEventSelector");
  return(StatusCode::FAILURE);
}

//________________________________________________________________________________
StatusCode DoubleEventSelectorAthenaPool::share(__attribute__((unused)) int evtnum) {
  ATH_MSG_FATAL("share() not implemented in the DoubleEventSelector");
  return(StatusCode::FAILURE);
}

//________________________________________________________________________________
StatusCode DoubleEventSelectorAthenaPool::readEvent(__attribute__((unused)) int maxevt) {
  ATH_MSG_FATAL("readEvent() not implemented in the DoubleEventSelector");
  return(StatusCode::FAILURE);
}
