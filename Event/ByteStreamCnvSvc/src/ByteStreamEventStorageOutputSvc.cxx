/* Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration */
#include "ByteStreamEventStorageOutputSvc.h"

#include <stdexcept>
#include <stdlib.h>
#include <sstream>

#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>

#include "ByteStreamDataWriter.h"

#include "AthenaKernel/StoreID.h"

#include "ByteStreamCnvSvcLegacy/offline_eformat/old/util.h"
#include "ByteStreamData/RawEvent.h"

#include "EventStorage/EventStorageRecords.h"
#include "EventStorage/RawFileName.h"
#include "EventStorage/SimpleFileName.h"

#include "xAODEventInfo/EventInfo.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIoComponentMgr.h"

#include "StoreGate/ReadHandle.h"


ByteStreamEventStorageOutputSvc::ByteStreamEventStorageOutputSvc(
    const std::string& name, ISvcLocator* pSvcLocator)
  : ByteStreamOutputSvc(name, pSvcLocator) {
}


StatusCode
ByteStreamEventStorageOutputSvc::initialize() {
  ATH_MSG_INFO("Initializing " << name() << " - package version "
               << PACKAGE_VERSION);

  ATH_CHECK(ByteStreamOutputSvc::initialize());

  ATH_CHECK(m_eventInfoKey.initialize());
  ATH_CHECK(m_byteStreamMetadataKey.initialize());

  // register this service for 'I/O' events
  ATH_CHECK(m_ioMgr.retrieve());
  ATH_CHECK(m_ioMgr->io_register(this));

  // Register output file's name with the I/O manager
  if (!m_simpleFileName.empty()) {
    ATH_CHECK(m_ioMgr->io_register(this, IIoComponentMgr::IoMode::WRITE,
                                   m_simpleFileName));
    ATH_MSG_VERBOSE("io_register[" << this->name() << "]("
                    << m_simpleFileName << ") [ok]");
  }

  // validate m_eformatVersion
  const std::vector< std::string > choices_ef{"current", "v40", "run1"};
  if (std::find(choices_ef.begin(), choices_ef.end(), m_eformatVersion)
      == choices_ef.end()) {
    ATH_MSG_FATAL("Unexpected value for EformatVersion property: "
                  << m_eformatVersion);
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("eformat version to use: \"" << m_eformatVersion << "\"");

  // validate m_eventStorageVersion
  const std::vector< std::string > choices_es{"current", "v5", "run1"};
  if (std::find(choices_es.begin(), choices_es.end(), m_eventStorageVersion)
      == choices_es.end()) {
    ATH_MSG_FATAL("Unexpected value for EventStorageVersion property: "
                  << m_eventStorageVersion);
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("event storage (BS) version to use: \""
               << m_eventStorageVersion << "\"");

  m_isRun1 = (m_eformatVersion == "v40" or m_eformatVersion == "run1");

  ATH_CHECK(reinit());

  return StatusCode::SUCCESS;
}


StatusCode
ByteStreamEventStorageOutputSvc::reinit() {
  ATH_MSG_INFO("Reinitialization...");
  return StatusCode::SUCCESS;
}


StatusCode
ByteStreamEventStorageOutputSvc::stop() {
  // Check whether anything has been written and whether the user wants metadata
  // only files
  if (m_dataWriter == 0 and m_writeEventless) {
    const ByteStreamMetadata* metaData = getByteStreamMetadata();

    // Try to write metadata to eventless file
    bool dWok = initDataWriterContents(nullptr, metaData);
    if (!dWok) ATH_MSG_WARNING("Could not write Metadata for eventless file");
  }

  return StatusCode::SUCCESS;
}


StatusCode
ByteStreamEventStorageOutputSvc::finalize() {
  // clean up
  ATH_MSG_DEBUG("deleting DataWriter");
  m_dataWriter.reset();
  ATH_MSG_INFO("number of events written: " << m_totalEventCounter);
  return StatusCode::SUCCESS;
}


bool
ByteStreamEventStorageOutputSvc::initDataWriter(const EventContext* ctx) {
  // Called on first event. Reads run parameters first event and/or first event
  const xAOD::EventInfo* eventInfo = ctx == nullptr
      ? SG::get(m_eventInfoKey)
      : SG::get(m_eventInfoKey, *ctx);
  if (eventInfo == nullptr) ATH_MSG_WARNING("failed to retrieve EventInfo");

  const ByteStreamMetadata* metaData = ctx == nullptr ? getByteStreamMetadata() : getByteStreamMetadata(*ctx);
  if (metaData == nullptr)
    ATH_MSG_WARNING("failed to retrieve ByteStreamMetaData");

  // Now open a file for writing from retrieved parameters
  return initDataWriterContents(eventInfo, metaData);
}


bool
ByteStreamEventStorageOutputSvc::initDataWriterContents(
    const xAOD::EventInfo* evtInfo,
    const ByteStreamMetadata* metaData) {
  // check that we have sufficient information to do what we need
  if (evtInfo or metaData)
    ATH_MSG_DEBUG("Looking up data writer parameters");
  else
    throw std::runtime_error("Cannot write data without run parameters");

  // The heirarchy of run/lumiblock number, GNARR
  //
  //   1) User override
  //   2) Event data
  //   3) File metadata
  //   4) default = unknown = 0
  //
  // Go from 4 to 1 and overwrite
  DataWriterParameters params; 
  if (metaData != nullptr) updateDataWriterParameters(params, *metaData);
  if (evtInfo != nullptr) updateDataWriterParameters(params, *evtInfo);
  updateDataWriterParameters(params);

  m_dataWriter = ByteStreamDataWriter::makeWriter(params);

  bool result = m_dataWriter->good();
  if (result)
    ATH_MSG_DEBUG("initialized output stream to file with name "
                  << params.fileNameCore);
  else
    ATH_MSG_ERROR("Unable to initialize file");

  return result;
}


bool
ByteStreamEventStorageOutputSvc::putEvent(RawEvent* re) {
  // Read the next event.
  return putEvent(re, Gaudi::Hive::currentContext());
}


bool
ByteStreamEventStorageOutputSvc::putEvent(
    RawEvent* re, const EventContext& ctx) {
  // Read the next event.
  using OFFLINE_FRAGMENTS_NAMESPACE::DataType;
  using OFFLINE_FRAGMENTS_NAMESPACE::PointerType;

  EventCache* cache = m_eventCache.get(ctx);
  cache->releaseEvent();

  // we need the size and the start of the event to give to the data writer
  cache->size = re->fragment_size_word();
  ATH_MSG_DEBUG("event size = " << cache->size << ", start = " << re->start());

  if (m_isRun1) {
    // convert to current eformat
    // allocate some extra space just in case
    ATH_MSG_DEBUG("converting Run 1 format ");

    cache->size += 128;
    cache->buffer = std::make_unique< DataType[] >(cache->size);
    ATH_MSG_DEBUG("created buffer 0x"
                  << std::hex << cache->buffer.get() << std::dec);

    // This builds no-checksum headers, should use the same
    // checksum type as original event
    cache->size = offline_eformat::old::convert_to_40(
        re->start(), cache->buffer.get(), cache->size);
    ATH_MSG_DEBUG("filled buffer");

    if (cache->size == 0) {
      // not enough space in buffer
      ATH_MSG_ERROR("Failed to convert event, buffer is too small");
      return false;
    }

    ATH_MSG_DEBUG("event size after conversion =  " << cache->size
                  << "  version = " << cache->buffer.get()[3]);

  } else {
    cache->buffer = std::make_unique< DataType[] >(cache->size);
    std::copy(re->start(), re->start() + cache->size, cache->buffer.get());
  }

  {
    // multiple data writers concurrently sounds like a bad idea
    std::lock_guard< std::mutex > lock(m_dataWriterMutex);

    // make sure the data writer is ready
    ATH_MSG_DEBUG("looking up data writer");
    if (!m_dataWriter) {
      if (!initDataWriter(&ctx)) {
        ATH_MSG_ERROR("Failed to initialize DataWriter");
        return false;
      }
    }

    // write event to disk
    EventStorage::DWError write_result = m_dataWriter->putData(
        sizeof(DataType) * cache->size,
        reinterpret_cast< void* >(cache->buffer.get()));

    // Report success or failure
    if (write_result != EventStorage::DWOK) {
      ATH_MSG_ERROR("Failed to write event to DataWriter");
      return false;
    }
    ++m_totalEventCounter;
  }

  return true;
}


StatusCode
ByteStreamEventStorageOutputSvc::io_reinit() {
  ATH_MSG_INFO("I/O reinitialization...");

  if (!m_ioMgr->io_hasitem(this)) {
    ATH_MSG_FATAL("IoComponentMgr does not know about myself !");
    return StatusCode::FAILURE;
  }

  if (!m_simpleFileName.empty()) {
    std::string outputFile = m_simpleFileName;
    ATH_MSG_INFO("I/O reinitialization, file = "  << outputFile);
    std::string &fname = outputFile;
    if (!m_ioMgr->io_contains(this, fname)) {
      ATH_MSG_ERROR("IoComponentMgr does not know about [" << fname << "] !");
      return(StatusCode::FAILURE);
    }
    if (!m_ioMgr->io_retrieve(this, fname).isSuccess()) {
      ATH_MSG_FATAL("Could not retrieve new value for [" << fname << "] !");
      return(StatusCode::FAILURE);
    }
    // all good... copy over.
    // modify directory
    m_inputDir.setValue(outputFile.substr(0, outputFile.find_last_of("/")));
    // FIXME: modify file name, not done for now because of
    // IoUtils.update_io_registry vs. merge conflict.
    //m_simpleFileName.setValue(
    //    outputFile.substr(outputFile.find_last_of("/") + 1));
  }
  ATH_MSG_DEBUG("Deleting DataWriter");
  m_dataWriter.reset();

  ATH_CHECK(reinit());

  return StatusCode::SUCCESS;
}


const ByteStreamMetadata *
ByteStreamEventStorageOutputSvc::getByteStreamMetadata()
{
  SG::ReadHandle<ByteStreamMetadataContainer> metaDataCont (m_byteStreamMetadataKey);

  if (!metaDataCont.isValid()) return nullptr;

  if (metaDataCont->size() > 1)
    ATH_MSG_WARNING("Multiple run parameters in MetaDataStore. "
                    "Bytestream format only supports one. Arbitrarily "
                    "choosing first.");

  return metaDataCont->front();
}


const ByteStreamMetadata *
ByteStreamEventStorageOutputSvc::getByteStreamMetadata(
    const EventContext& ctx)
{
  SG::ReadHandle<ByteStreamMetadataContainer> metaDataCont (m_byteStreamMetadataKey, ctx);

  if (!metaDataCont.isValid()) return nullptr;

  if (metaDataCont->size() > 1)
    ATH_MSG_WARNING("Multiple run parameters in MetaDataStore. "
                    "Bytestream format only supports one. Arbitrarily "
                    "choosing first.");

  return metaDataCont->front();
}


void
ByteStreamEventStorageOutputSvc::updateDataWriterParameters(
    DataWriterParameters& params) const {

  if (m_eventStorageVersion == "v5" or m_eventStorageVersion == "run1")
    params.version = 5;
  else params.version = 0;

  params.writingPath = m_inputDir;

  if (m_run != 0) params.rPar.run_number = m_run;
  ATH_MSG_DEBUG("Run number: " << params.rPar.run_number);

  if (m_lumiBlockNumber != 0) params.lumiBlockNumber = m_lumiBlockNumber;
  ATH_MSG_DEBUG("LB number: " << params.lumiBlockNumber);

  if (!m_streamType.empty()) params.streamType = m_streamType;
  if (!m_streamName.empty()) params.streamName = m_streamName;

  if (params.streamType.empty()) params.streamType = "Single";
  if (params.streamName.empty()) params.streamName = "Stream";

  params.stream = params.streamType + "_" + params.streamName;

  if (!m_projectTag.empty()) params.project = m_projectTag;

  params.applicationName = m_appName;

  if (!m_simpleFileName.empty()) {
    // set up for simple file name
    boost::shared_ptr<EventStorage::SimpleFileName> simple_file_name(
        new EventStorage::SimpleFileName(m_simpleFileName));
    params.theFNCB = simple_file_name;
  } else {
    // set up for production file name
    daq::RawFileName fileNameObj(
        params.project,
        params.rPar.run_number,
        params.streamType,
        params.streamName,
        params.lumiBlockNumber,
        params.applicationName);
    params.fileNameCore = fileNameObj.fileNameCore();
  }

  params.compression = m_compressEvents
      ? EventStorage::ZLIB
      : EventStorage::NONE;

  params.maxFileMB = m_maxFileMB;
  params.maxFileNE = params.rPar.max_events = m_maxFileNE;
}


void
ByteStreamEventStorageOutputSvc::updateDataWriterParameters(
    DataWriterParameters& params, const xAOD::EventInfo& eventInfo) const {
  ATH_MSG_DEBUG("Parsing run parameters from EventInfo" << eventInfo);
  
  params.rPar.run_number = eventInfo.runNumber();
  params.lumiBlockNumber = eventInfo.lumiBlock();

  for (const xAOD::EventInfo::StreamTag& tag : eventInfo.streamTags())
    if(!tag.type().empty()) {
      params.streamType = tag.type();
      break;
    }

  for (const xAOD::EventInfo::StreamTag& tag : eventInfo.streamTags())
    if (!tag.name().empty()) {
      params.streamName = tag.name();
      break;
    }

  for (const auto& tag : eventInfo.detDescrTags())
    params.fmdStrings.push_back(tag.first + ' ' + tag.second);

  params.rPar.trigger_type = eventInfo.level1TriggerType();
  params.rPar.detector_mask_LS = eventInfo.detectorMask();
  params.rPar.detector_mask_MS = eventInfo.detectorMaskExt();

  std::string event_type = "Event type: sim/data - ";
  if (eventInfo.eventType(xAOD::EventInfo::EventType::IS_SIMULATION))
    event_type += "is sim";
  else event_type += "is data";

  event_type += " , testbeam/atlas - ";
  if (eventInfo.eventType(xAOD::EventInfo::EventType::IS_TESTBEAM))
    event_type += "is testbeam";
  else event_type += "is atlas";

  event_type += " , calibration/physics - ";
  if (eventInfo.eventType(xAOD::EventInfo::EventType::IS_CALIBRATION))
    event_type += "is calibration";
  else event_type += "is physics";

  params.fmdStrings.push_back(event_type);
}


void
ByteStreamEventStorageOutputSvc::updateDataWriterParameters(
    DataWriterParameters& params, const ByteStreamMetadata& metaData) const {
  ATH_MSG_DEBUG("Parsing run parameters from metadata:\n" << metaData); 

  params.rPar.run_number = metaData.getRunNumber();
  params.lumiBlockNumber = metaData.getLumiBlock();

  const std::string stream = metaData.getStream();
  const std::string::size_type split = stream.find('_');

  if (split != std::string::npos and params.streamType.empty())
    params.streamType = stream.substr(0,split);

  if (split != std::string::npos and params.streamName.empty())
    params.streamName = stream.substr(split+1);

  params.project = metaData.getProject();
  params.maxFileNE = params.rPar.max_events = metaData.getMaxEvents();

  params.rPar.rec_enable = metaData.getRecEnable();
  params.rPar.trigger_type = metaData.getTriggerType();
  params.rPar.beam_type = metaData.getBeamType();
  if (metaData.getBeamEnergy() != 0)
    params.rPar.beam_energy = metaData.getBeamEnergy();

  params.rPar.detector_mask_LS = metaData.getDetectorMask(); 
  params.rPar.detector_mask_MS = metaData.getDetectorMask2();

  for (const std::string& fmd : metaData.getFreeMetaDataStrings())
    params.fmdStrings.push_back(fmd);
  // if(fmd.find("Compression=") == std::string::npos)
}


StatusCode
ByteStreamEventStorageOutputSvc::queryInterface(
    const InterfaceID& riid, void** ppvInterface) {
  if (ByteStreamOutputSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface = dynamic_cast<ByteStreamOutputSvc*>(this);
  } else {
    // Interface is not directly available: try out a base class
    ATH_CHECK(::AthService::queryInterface(riid, ppvInterface));
  }
  addRef();
  return StatusCode::SUCCESS;
}
