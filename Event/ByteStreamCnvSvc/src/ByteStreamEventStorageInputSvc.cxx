/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ByteStreamEventStorageInputSvc.h"

#include "DumpFrags.h"
#include "ByteStreamData/ByteStreamMetadataContainer.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"
#include "EventStorage/pickDataReader.h"
#include "EventStorage/DataReader.h"

#include "Gaudi/Property.h"

#include "PersistentDataModel/DataHeader.h"
#include "PersistentDataModel/Token.h"
#include "StoreGate/StoreGateSvc.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventAuxInfo.h"

#include "eformat/HeaderMarker.h"
#include "eformat/SourceIdentifier.h"
#include "eformat/Issue.h"
#include "eformat/Problem.h"
#include "eformat/Version.h"
#include "eformat/Status.h"
#include "eformat/old/util.h"

#include <cstdio>
#include <string>
#include <vector>
#include <unistd.h>


/******************************************************************************/
// Constructor.
ByteStreamEventStorageInputSvc::ByteStreamEventStorageInputSvc(
    const std::string& name, ISvcLocator* pSvcLocator)
  : ByteStreamInputSvc(name, pSvcLocator)
  , m_readerMutex()
  , m_eventsCache()
  , m_reader()
  , m_evtOffsets()
  , m_evtInFile(0)
  , m_evtFileOffset(0)
  , m_fileGUID("")
  , m_storeGate    ("StoreGateSvc", name)
  , m_inputMetadata("StoreGateSvc/InputMetaDataStore", name)
  , m_robProvider  ("ROBDataProviderSvc", name)
  , m_sequential   (this, "EnableSequential",   false, "")
  , m_dump         (this, "DumpFlag",           false, "Dump fragments")
  , m_wait         (this, "WaitSecs",              0., "Seconds to wait if input is in wait state")
  , m_valEvent     (this, "ValidateEvent",       true, "switch on check_tree when reading events")
  , m_eventInfoKey (this, "EventInfoKey", "EventInfo", "Key of EventInfo in metadata store")
{
  assert(pSvcLocator != nullptr);

  declareProperty("EventStore",    m_storeGate);
  declareProperty("MetaDataStore", m_inputMetadata);
}


/******************************************************************************/
ByteStreamEventStorageInputSvc::~ByteStreamEventStorageInputSvc()
{}


/******************************************************************************/
StatusCode
ByteStreamEventStorageInputSvc::initialize()
{
  ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);

  ATH_CHECK(ByteStreamInputSvc::initialize());
  ATH_CHECK(m_inputMetadata.retrieve());
  ATH_CHECK(m_storeGate.retrieve());
  ATH_CHECK(m_robProvider.retrieve());

  return(StatusCode::SUCCESS);
}


/******************************************************************************/
StatusCode
ByteStreamEventStorageInputSvc::stop()
{
  ATH_MSG_DEBUG("Calling ByteStreamInputSvc::stop()");
  // close moved to EventSelector for explicit coupling with incident
  //if (m_reader != 0) closeBlockIterator(false);
  return(StatusCode::SUCCESS);
}


/******************************************************************************/
StatusCode
ByteStreamEventStorageInputSvc::finalize() {

  ATH_CHECK(m_storeGate.release());
  ATH_CHECK(m_robProvider.release()); 
  ATH_CHECK(m_inputMetadata.release());

  return(ByteStreamInputSvc::finalize());
}


/******************************************************************************/
// Can't inline this because it is called via pointer to the base class
long
ByteStreamEventStorageInputSvc::positionInBlock()
{
  return m_evtOffsets.size();
}


/******************************************************************************/
StatusCode
ByteStreamEventStorageInputSvc::loadMetadata()
{
  // default goes into ByteStreamMetadata
  auto bsmdc = std::make_unique<ByteStreamMetadataContainer>();
  bsmdc->push_back(std::make_unique<ByteStreamMetadata>(*m_reader));
  ATH_MSG_DEBUG("ByteStreamMetadata:\n" << *(bsmdc->front()));

  return m_inputMetadata->record(std::move(bsmdc), "ByteStreamMetadata");
}


/******************************************************************************/
const RawEvent*
ByteStreamEventStorageInputSvc::previousEvent()
{
  std::lock_guard<std::mutex> lock(m_readerMutex);
  const EventContext context{Gaudi::Hive::currentContext()};

  // Load data buffer from file
  char *buf;
  unsigned int eventSize;
  if (readerReady()) {
    //get current event position (cast to long long until native tdaq implementation)
    m_evtInFile--;
    m_evtFileOffset = m_evtOffsets.at(m_evtInFile);
    DRError ecode = m_reader->getData(eventSize, &buf, m_evtOffsets.at(m_evtInFile - 1));

    if (DRWAIT == ecode && m_wait > 0) {
      do {
        // wait for n seconds
        ATH_MSG_DEBUG("Waiting for input:  " << m_wait << " seconds");
        int result = usleep(static_cast<int>(m_wait * 1e6));
        if (result != 0) {
          ATH_MSG_ERROR("System Error while running sleep");
          return nullptr;
        }
      } while(m_reader->getData(eventSize, &buf, m_evtFileOffset) == DRWAIT);
    } else if (DROK != ecode) {
      ATH_MSG_ERROR("Error reading previous event");
      throw ByteStreamExceptions::readError();
    }
    ATH_MSG_DEBUG("Event Size " << eventSize);
  }
  else {
    ATH_MSG_ERROR("DataReader not ready. Need to getBlockIterator first");
    return 0;
  }

  EventCache* cache = m_eventsCache.get(context);
  // initialize before building RawEvent
  cache->releaseEvent();

  // Use buffer to build FullEventFragment
  try {
    buildFragment(cache, buf, eventSize, true);
  }
  catch (...) {
    // rethrow any exceptions
    throw;
  }

  if (cache->rawEvent == nullptr) {
    ATH_MSG_ERROR("Failure to build fragment");
    return nullptr;
  }

  // Set it for the data provider
  m_robProvider->setNextEvent(context, cache->rawEvent.get());
  m_robProvider->setEventStatus(context, cache->eventStatus);

  // dump
  if (m_dump) {
    DumpFrags::dump(cache->rawEvent.get());
  }
  ATH_MSG_DEBUG( "switched to previous event in slot " << context);
  return(cache->rawEvent.get());
}


/******************************************************************************/
// Read the next event.
const RawEvent*
ByteStreamEventStorageInputSvc::nextEvent() {

  std::lock_guard<std::mutex> lock( m_readerMutex );
  const EventContext context{ Gaudi::Hive::currentContext() };

  // Load data buffer from file
  char *buf;
  unsigned int eventSize;
  if (readerReady()) {
    DRError ecode;
    // Check if have moved back from high water mark
    m_evtInFile++; // increment iterator
    if (m_evtInFile+1 > m_evtOffsets.size()) { 
      //get current event position (cast to long long until native tdaq implementation)
      ATH_MSG_DEBUG("nextEvent _above_ high water mark");
      m_evtFileOffset = static_cast<long long>(m_reader->getPosition());
      m_evtOffsets.push_back(m_evtFileOffset);
      ecode = m_reader->getData(eventSize, &buf);
    } else {
      // Load from previous offset
      ATH_MSG_DEBUG("nextEvent below high water mark");
      m_evtFileOffset = m_evtOffsets.at(m_evtInFile - 1);
      ecode = m_reader->getData(eventSize, &buf, m_evtFileOffset);
    }

    if (DRWAIT == ecode && m_wait > 0) {
      do {
        // wait for n seconds
        ATH_MSG_DEBUG("Waiting for input:  " << m_wait << " seconds");
        int result = usleep(static_cast<int>(m_wait * 1e6));
        if (result != 0) {
          ATH_MSG_ERROR("System Error while running sleep");
          return 0;
        }
      } while(m_reader->getData(eventSize, &buf) == DRWAIT);
    } else if (DROK != ecode) {
      ATH_MSG_ERROR("Error reading next event");
      throw ByteStreamExceptions::readError();
    }
    ATH_MSG_DEBUG("Event Size " << eventSize);

  } else {
    ATH_MSG_ERROR("DataReader not ready. Need to getBlockIterator first");
    return 0;
  }

  EventCache* cache = m_eventsCache.get(context);

  // initialize before building RawEvent
  cache->releaseEvent();

  // Use buffer to build FullEventFragment
  try {
    buildFragment(cache, buf, eventSize, true);
  }
  catch (...) {
    // rethrow any exceptions
    throw;
  }

  if (cache->rawEvent == nullptr) {
    ATH_MSG_ERROR("Failure to build fragment");
    return nullptr;
  }

  // Set it for the data provider
  m_robProvider->setNextEvent(context, cache->rawEvent.get());
  m_robProvider->setEventStatus(context, cache->eventStatus);

  //++m_totalEventCounter;

  // dump
  if (m_dump) {
    DumpFrags::dump(cache->rawEvent.get());
  }
  ATH_MSG_DEBUG("switched to next event in slot " << context);
  return(cache->rawEvent.get());
}


/******************************************************************************/
void
ByteStreamEventStorageInputSvc::validateEvent()
{
  const EventContext& context{Gaudi::Hive::currentContext()};
  const RawEvent* const event = m_eventsCache.get(context)->rawEvent.get();
  m_eventsCache.get(context)->eventStatus = validateEvent(event);
}


/******************************************************************************/
unsigned
ByteStreamEventStorageInputSvc::validateEvent(const RawEvent* const rawEvent) const
{
  unsigned int status = 0;
  if (m_valEvent) {
    // check validity
    std::vector<eformat::FragmentProblem> problems;
    rawEvent->problems(problems);

    if(!problems.empty()) {
      status += 0x01000000;

      // bad event
      ATH_MSG_WARNING("Failed to create FullEventFragment");
      for(const auto& problem : problems)
        ATH_MSG_WARNING(eformat::helper::FragmentProblemDictionary.string(problem));

      throw ByteStreamExceptions::badFragmentData();
    }

    if(!ROBFragmentCheck(rawEvent)) {
      status += 0x02000000;

      // bad event
      ATH_MSG_ERROR("Skipping bad event");
      throw ByteStreamExceptions::badFragmentData();
    }
  } else {
    ATH_MSG_DEBUG("Processing event without validating.");
  }
  return status;
}


/******************************************************************************/
void
ByteStreamEventStorageInputSvc::buildFragment(
    EventCache* cache, char* data, uint32_t eventSize,
    bool validate) const
{
  using OFFLINE_FRAGMENTS_NAMESPACE::DataType;
  using OFFLINE_FRAGMENTS_NAMESPACE::PointerType;
  DataType* fragment = reinterpret_cast<DataType*>(data);

  if (validate) {
    // check fragment type
    const DataType headWord = fragment[0];
    ATH_MSG_DEBUG("First word of the fragment " << MSG::hex << headWord << MSG::dec);
    // format version
    const DataType formatVersion = eformat::helper::Version(fragment[3]).major_version();
    ATH_MSG_DEBUG("Format version" << MSG::hex << formatVersion << MSG::dec);
    // error message
    if((formatVersion != eformat::MAJOR_DEFAULT_VERSION) && 
       (formatVersion != eformat::MAJOR_V24_VERSION) && 
       (formatVersion != eformat::MAJOR_V30_VERSION) && 
       (formatVersion != eformat::MAJOR_V40_VERSION) && 
       (formatVersion != eformat::MAJOR_V31_VERSION) ) {
      ATH_MSG_ERROR("unsupported Format Version : "
          << MSG::hex << formatVersion << MSG::dec);
    }

    if(eformat::FULL_EVENT == headWord || 0xcc1234cc == headWord) { // ROS = 0xcc1234cc
      try {
        // convert old fragment
        if(formatVersion != eformat::MAJOR_DEFAULT_VERSION) {
          // 100 for increase of data-size due to header conversion
          const uint32_t newEventSize = eventSize + 1000;
          DataType* newFragment  = new DataType[newEventSize];
          eformat::old::convert(fragment, newFragment, newEventSize);

          // delete old fragment
          delete [] fragment; fragment = nullptr;

          // set new pointer
          fragment = newFragment;
        }
      } catch (const eformat::Issue& ex) {
        // bad event
        ATH_MSG_WARNING(ex.what());
        ATH_MSG_ERROR("Skipping bad event");
        throw ByteStreamExceptions::badFragment();
      }
    } else {
      // Unknown fragment
      ATH_MSG_FATAL("Unknown Header work in input fragment " << MSG::hex << headWord);
      throw ByteStreamExceptions::badFragment();
    }
  }

  // This is a FullEventFragment
  // make a new FEFrag in memory from it
  cache->eventStatus = 0;
  if(fragment[5] > 0) {
    cache->eventStatus += eformat::helper::Status(fragment[6]).specific();
    cache->eventStatus += (eformat::helper::Status(fragment[6]).generic() & 0x000000ff) << 16;
  }

  // This is a FullEventFragment
  // make a new RawEvent in memory from it
  cache->rawEvent = std::make_unique<RawEvent>(fragment);
  ATH_MSG_DEBUG("Made an FullEventFragment from ES " << fragment);
}


/******************************************************************************/
StatusCode
ByteStreamEventStorageInputSvc::generateDataHeader()
{
  // get file GUID
  m_fileGUID = m_reader->GUID();

  // reader returns -1 when end of the file is reached
  if(m_evtFileOffset != -1) {
    ATH_MSG_DEBUG("ByteStream File GUID:              " << m_fileGUID);
    ATH_MSG_DEBUG("ByteStream Event Position in File: " << m_evtFileOffset);

    // To accomodate for skipEvents option in EventSelector
    // While skipping BS event Selector does not return SUCCESS code,
    // just advances silently through events. So SG content is not refreshed
    // Lets do refresh of the event header here
    std::string key = "ByteStreamDataHeader";
    ATH_CHECK(deleteEntry<DataHeader>(key));

    // Created data header element with BS provenance information
    std::unique_ptr<DataHeaderElement> dataHeaderElement = makeBSProvenance();
    // Create data header itself
    std::unique_ptr<DataHeader> dataHeader = std::make_unique<DataHeader>();
    // Declare header primary
    dataHeader->setStatus(DataHeader::Input);
    //add the data header elenebt self reference to the object vector
    dataHeader->insert(*std::move(dataHeaderElement));

    // Clean up EventInfo from the previous event
    key = m_eventInfoKey.value();
    ATH_CHECK(deleteEntry<xAOD::EventInfo>(key));
    // Now add ref to xAOD::EventInfo
    std::unique_ptr<IOpaqueAddress> iopx = std::make_unique<ByteStreamAddress>(
        ClassID_traits<xAOD::EventInfo>::ID(), key, "");
    ATH_CHECK(m_storeGate->recordAddress(key, iopx.release()));
    const SG::DataProxy* ptmpx = m_storeGate->transientProxy(
        ClassID_traits<xAOD::EventInfo>::ID(), key);
    if (ptmpx != nullptr) {
      DataHeaderElement element(ptmpx, 0, key);
      dataHeader->insert(element);
    }

    // Clean up auxiliary EventInfo from the previous event
    key = m_eventInfoKey.value() + "Aux.";
    ATH_CHECK(deleteEntry<xAOD::EventAuxInfo>(key));
    // Now add ref to xAOD::EventAuxInfo
    std::unique_ptr<IOpaqueAddress> iopaux = std::make_unique<ByteStreamAddress>(
        ClassID_traits<xAOD::EventAuxInfo>::ID(), key, "");
    ATH_CHECK(m_storeGate->recordAddress(key, iopaux.release()));
    const SG::DataProxy* ptmpaux = m_storeGate->transientProxy(
        ClassID_traits<xAOD::EventAuxInfo>::ID(), key);
    if (ptmpaux !=0) {
      DataHeaderElement element(ptmpaux, 0, key);
      dataHeader->insert(element);
    }

    // Record new data header.Boolean flags will allow it's deletionin case
    // of skipped events.
    ATH_CHECK(m_storeGate->record<DataHeader>(dataHeader.release(),
          "ByteStreamDataHeader", true, false, true));
  }
  return StatusCode::SUCCESS;
}


/******************************************************************************/
void
ByteStreamEventStorageInputSvc::EventCache::releaseEvent()
{
  // cleanup parts of previous event and re-init them
  if(rawEvent) {
    OFFLINE_FRAGMENTS_NAMESPACE::PointerType fragment = rawEvent->start();
    delete [] fragment; fragment = nullptr;
    rawEvent.reset(nullptr);
    eventStatus = 0;
  }
}


/******************************************************************************/
ByteStreamEventStorageInputSvc::EventCache::~EventCache()
{
  releaseEvent();
}


/******************************************************************************/
void
ByteStreamEventStorageInputSvc::closeBlockIterator(bool clearMetadata)
{
  if (clearMetadata) {
    ATH_MSG_WARNING("Clearing input metadata store");
    StatusCode status = m_inputMetadata->clearStore();
    if (!status.isSuccess()) {
      ATH_MSG_WARNING("Unable to clear Input MetaData Proxies");
    }
  }

  if (!readerReady()) {
    ATH_MSG_INFO("No more events in this run, high water mark for this file = "
        << m_evtOffsets.size()-1);
  }

  m_reader.reset();
}


/******************************************************************************/
bool
ByteStreamEventStorageInputSvc::setSequentialRead()
{
  // enable SequenceReading
  m_reader->enableSequenceReading();
  return true;
}


/******************************************************************************/
bool
ByteStreamEventStorageInputSvc::ready() const
{
  return readerReady();
}


/******************************************************************************/
std::pair<long,std::string>
ByteStreamEventStorageInputSvc::getBlockIterator(const std::string fileName)
{
  // open the file
  if(m_reader != 0) closeBlockIterator();

  m_reader = std::unique_ptr<EventStorage::DataReader>(pickDataReader(fileName));

  if(m_reader == nullptr) {
    ATH_MSG_ERROR("Failed to open file " << fileName);
    closeBlockIterator();
    return std::make_pair(-1,"END"); 
  }

  // Initilaize offset vector
  m_evtOffsets.resize(m_reader->eventsInFile(), -1);
  m_evtOffsets.clear();

  // Get ByteStream Metadata into Input MetaData Store
  // (include begin Input File Incident)
  if (loadMetadata().isSuccess()) {
    ATH_MSG_DEBUG("Recorded ByteStreamMetadata in InputMetaDataStore");
  } else {
    ATH_MSG_ERROR("Unable to record ByteStreamMetadata in InputMetaDataStore");
    return std::make_pair(-1, "FAIL");
  }

  m_evtInFile = 0;

  // enable sequentialReading if multiple files
  if(m_sequential) {
    bool test = setSequentialRead();
    if (!test) return std::make_pair(-1,"SEQ");
  }

  ATH_MSG_INFO("Picked valid file: " << m_reader->fileName());
  // initialize offsets and counters
  m_evtOffsets.push_back(static_cast<long long>(m_reader->getPosition()));
  return std::make_pair(m_reader->eventsInFile(), m_reader->GUID());
}


/******************************************************************************/
bool
ByteStreamEventStorageInputSvc::readerReady() const
{
  bool eofFlag(false);

  if (m_reader) eofFlag = m_reader->endOfFile();
  else {
    ATH_MSG_INFO("eformat reader object not initialized");
    return false;
  }

  bool moreEvent = m_reader->good();

  return (!eofFlag) && moreEvent;
}


/******************************************************************************/
bool
ByteStreamEventStorageInputSvc::ROBFragmentCheck(const RawEvent* re) const
{
  bool     allOK  = true;
  uint32_t total  = re->nchildren();
  uint32_t lastId = 0;
  std::vector<eformat::FragmentProblem> problems;

  for (size_t i = 0; i<total; ++i) {
    OFFLINE_FRAGMENTS_NAMESPACE::PointerType fp;
    re->child(fp, i);

    OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment f(fp);
    lastId = f.source_id();

    problems.clear();
    f.problems(problems);
    if(!problems.empty()) {
      allOK = false;
      for(const auto& problem : problems) {
        ATH_MSG_WARNING("Failed to create ROBFragment id = " << lastId << ", "
            << eformat::helper::SourceIdentifier(lastId).human()  << " : "
            << eformat::helper::FragmentProblemDictionary.string(problem));
      }
    }
  }

  return allOK;
}


/******************************************************************************/
void
ByteStreamEventStorageInputSvc::setEvent(void* data, unsigned int eventStatus)
{
  const EventContext context{Gaudi::Hive::currentContext()};
  return setEvent(context, data, eventStatus);
}


/******************************************************************************/
void
ByteStreamEventStorageInputSvc::setEvent(const EventContext& context,
    void* data, unsigned int eventStatus)
{
  using OFFLINE_FRAGMENTS_NAMESPACE::DataType;
  EventCache* cache = m_eventsCache.get(context);
  cache->releaseEvent();

  DataType* fragment = reinterpret_cast<DataType*>(data);
  cache->rawEvent = std::make_unique<RawEvent>(fragment);
  cache->eventStatus = eventStatus;

  // Set it for the data provider
  m_robProvider->setNextEvent(context, cache->rawEvent.get());
  m_robProvider->setEventStatus(context, cache->eventStatus);

  // Build a DH for use by other components
  StatusCode rec_sg = generateDataHeader();
  if (rec_sg != StatusCode::SUCCESS) {
    ATH_MSG_ERROR("Fail to record BS DataHeader in StoreGate. Skipping events?! "
        << rec_sg);
  }
}


/******************************************************************************/
const
RawEvent* ByteStreamEventStorageInputSvc::currentEvent() const
{
  const EventContext context{Gaudi::Hive::currentContext()};
  return m_eventsCache.get(context)->rawEvent.get();
}


/******************************************************************************/
unsigned int
ByteStreamEventStorageInputSvc::currentEventStatus() const
{
  const EventContext context{Gaudi::Hive::currentContext()};
  return m_eventsCache.get(context)->eventStatus;
}


/******************************************************************************/
StatusCode
ByteStreamEventStorageInputSvc::queryInterface(const InterfaceID& riid,
    void** ppvInterface)
{
  if(ByteStreamInputSvc::interfaceID().versionMatch(riid))
    *ppvInterface = dynamic_cast<ByteStreamInputSvc*>(this);
  else // Interface is not directly available: try out a base class
    return(::AthService::queryInterface(riid, ppvInterface));
 
  addRef();
  return(StatusCode::SUCCESS);
}


/******************************************************************************/
std::unique_ptr<DataHeaderElement>
ByteStreamEventStorageInputSvc::makeBSProvenance() const
{
    std::unique_ptr<Token> token = std::make_unique<Token>();
    token->setDb(m_fileGUID);
    token->setTechnology(0x00001000);
    token->setOid(Token::OID_t(0LL, m_evtFileOffset));

    // note: passing ownership of token to DataHeaderElement
    return std::make_unique<DataHeaderElement>(ClassID_traits<DataHeader>::ID(),
        "StreamRAW", token.release());
}

