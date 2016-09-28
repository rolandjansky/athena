/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ByteStreamEventStorageOutputSvc.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIoComponentMgr.h"

#include "EventStorage/EventStorageRecords.h"
#include "EventStorage/RawFileName.h"
#include "EventStorage/SimpleFileName.h"

#include "ByteStreamData/RawEvent.h"
#include "ByteStreamData/ByteStreamMetadataContainer.h"
#include "ByteStreamData/ByteStreamUserMetadata.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/TagInfo.h"

#include "StoreGate/StoreGateSvc.h"

#include "ByteStreamCnvSvcLegacy/offline_eformat/old/util.h"

#include "ByteStreamDataWriter.h"

#include <boost/shared_ptr.hpp>
#include <stdlib.h>

// Constructor.
ByteStreamEventStorageOutputSvc::ByteStreamEventStorageOutputSvc(const std::string& name, ISvcLocator* svcloc) :
		ByteStreamOutputSvc(name,svcloc),
	m_totalEventCounter(0),
        m_attlistsvc("ByteStreamAttListMetadataSvc", name)
{
   declareProperty("OutputDirectory", m_inputDir);
   // a set of fields for making up filename
   declareProperty("ProjectTag",      m_projectTag);
   declareProperty("AppName",         m_appName);
   declareProperty("FileTag",         m_fileTag);
   declareProperty("StreamType",      m_streamType = "Single");
   declareProperty("StreamName",      m_streamName = "Stream");
   declareProperty("LumiBlockNumber", m_lumiBlockNumber = 0);
   declareProperty("RunNumber",       m_run = 0);

   // This is used by ByteStreamCnvSvc when multiple streams are written out.
   declareProperty("BSOutputStreamName", m_bsOutputStreamName = name);
   // or just give a simple filename
   // this will be the filename if it is non-empty
   declareProperty("SimpleFileName",  m_simpleFileName);

   // flag for dumping all fragments to logfile
   declareProperty("DumpFlag", m_dump = false);

   // flag for writing eventless files
   declareProperty("WriteEventlessFiles", m_writeEventless = true);

   // flag for compressing events
   declareProperty("CompressEvents", m_compressEvents = false);

   declareProperty("MaxFileMB", m_maxFileMB = 10000);
   declareProperty("MaxFileNE", m_maxFileNE = 100000);
   declareProperty("AttributeListKeys", m_keys);

   declareProperty("EformatVersion", m_eformatVersion = "current",
                   "Version of the event format data, use \"v40\" or \"run1\" "
                   "for run1, \"current\" for most current version (default).");
   declareProperty("EventStorageVersion", m_eventStorageVersion = "current",
                   "Version of the ByteStream file data, use \"v5\" or \"run1\" "
                   "for run1, \"current\" for most current version (default).");
}
//__________________________________________________________________________
ByteStreamEventStorageOutputSvc::~ByteStreamEventStorageOutputSvc() {
}
//__________________________________________________________________________
StatusCode ByteStreamEventStorageOutputSvc::initialize() {
   ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);
   if (!ByteStreamOutputSvc::initialize().isSuccess()) {
      ATH_MSG_FATAL("Cannot initialize ByteStreamOutputSvc base class.");
      return(StatusCode::FAILURE);
   }

   // register this service for 'I/O' events
   ServiceHandle<IIoComponentMgr> iomgr("IoComponentMgr", name());
   if (!iomgr.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Could not retrieve IoComponentMgr !");
      return(StatusCode::FAILURE);
   }
   if (!iomgr->io_register(this).isSuccess()) {
      ATH_MSG_FATAL("Could not register myself with the IoComponentMgr !");
      return(StatusCode::FAILURE);
   }

   // Register output file's name with the I/O manager
   if (!m_simpleFileName.value().empty()) {
      if (!iomgr->io_register(this, IIoComponentMgr::IoMode::WRITE, m_simpleFileName.value()).isSuccess()) {
         ATH_MSG_FATAL("could not register [" << m_simpleFileName.value() << "] for output !");
         return(StatusCode::FAILURE);
      } else {
         ATH_MSG_VERBOSE("io_register[" << this->name() << "](" << m_simpleFileName.value() << ") [ok]");
      }
   }

   // Retrieve AttListSvc
   if (!m_attlistsvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get metadata AttListSvc.");
      return(StatusCode::FAILURE);
   }

   // validate m_eformatVersion and m_eventStorageVersion
   const char* choices_ef[] = {"current", "v40", "run1"};
   if (std::find(std::begin(choices_ef), std::end(choices_ef), m_eformatVersion.value()) == std::end(choices_ef)) {
       ATH_MSG_FATAL("Unexpected value for EformatVersion property: " << m_eformatVersion);
       return(StatusCode::FAILURE);
   }
   const char* choices_es[] = {"current", "v5", "run1"};
   if (std::find(std::begin(choices_es), std::end(choices_es), m_eventStorageVersion.value()) == std::end(choices_es)) {
       ATH_MSG_FATAL("Unexpected value for EventStorageVersion property: " << m_eventStorageVersion);
       return(StatusCode::FAILURE);
   }
   ATH_MSG_INFO("eformat version to use: \"" << m_eformatVersion.value() << "\"");
   ATH_MSG_INFO("event storage (BS) version to use: \"" << m_eventStorageVersion.value() << "\"");

   return(this->reinit());

}
//__________________________________________________________________________
StatusCode ByteStreamEventStorageOutputSvc::reinit() {
   ATH_MSG_INFO("Reinitialization...");
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode ByteStreamEventStorageOutputSvc::stop() {
   // Check whether anything has been written && whether the user wants metadata only files
   bool dWok = false; 
   if (m_dataWriter == 0 && m_writeEventless) {
      const ByteStreamMetadataContainer* metaDataCont = 0;
      const ByteStreamMetadata* metaData = 0;
      ServiceHandle<StoreGateSvc> mds("MetaDataStore", name());
      StatusCode status = mds.retrieve();
      if (!status.isFailure()) {
         StatusCode stat = mds->retrieve(metaDataCont);
         if (stat.isSuccess()) metaData = *(metaDataCont->begin());
      }
      // Try to write metadata to file
      dWok = initDataWriterContents(0,metaData);
      if (!dWok) ATH_MSG_WARNING("Could not write Metadata for eventless file");
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode ByteStreamEventStorageOutputSvc::finalize() {
   // clean up
   ATH_MSG_DEBUG("deleting DataWriter");
   m_dataWriter.reset();
   ATH_MSG_INFO("number of events written: " << m_totalEventCounter);
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
// Open the first input file and read the first event.
bool ByteStreamEventStorageOutputSvc::initDataWriter() {
   // Retrieve EventInfo to get run number, detector mask and event type
   const EventInfo* evtInfo = 0;
   ServiceHandle<StoreGateSvc> sg("StoreGateSvc", name());
   if (sg.retrieve().isFailure()) {
      ATH_MSG_ERROR("Cannot get StoreGateSvc");
      return(false);
   }
   if (sg->retrieve(evtInfo).isFailure() || evtInfo == 0) {
      ATH_MSG_ERROR("Cannot retrieve EventInfo");
      return(false);
   }
   const ByteStreamMetadata* metaData = 0;
   ServiceHandle<StoreGateSvc> mds("InputMetaDataStore", name());
   StatusCode status = mds.retrieve();
   if (!status.isFailure()) {
      status = mds->retrieve(metaData);
      if (status.isFailure()) ATH_MSG_INFO("No ByteStreamMetadata in InputMetaDataStore");
   }
   // Now try to write metadata to file
   return initDataWriterContents(evtInfo,metaData);
}
//__________________________________________________________________________
// Open the first input file and read the first event.
bool ByteStreamEventStorageOutputSvc::initDataWriterContents(const EventInfo* evtInfo, 
                                                             const ByteStreamMetadata* metaData)
{
   // Initialize parameters
   EventStorage::run_parameters_record runPara = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
   EventStorage::freeMetaDataStrings freeMetaDataStrings;
   runPara.detector_mask_LS=0xFFFFFFFFFFFFFFFFULL;
   runPara.detector_mask_MS=0xFFFFFFFFFFFFFFFFULL;
   int run = 0;
   int lumiNum = 0;
   // The heirarchy of run/lumiblock number, GNARR
   // 1) User override
   // 2) Event data
   // 3) File metadata
   // 4) default = unknown = 0
   if (m_run != 0) {
      run = m_run;
   } else if (evtInfo != 0) {
      run = evtInfo->event_ID()->run_number();
   } else if (metaData != 0) {
      run = metaData->getRunNumber();
   } 
   runPara.run_number = run;

   if (m_lumiBlockNumber != 0) {
      lumiNum = m_lumiBlockNumber;
   } else if (evtInfo != 0) {
      lumiNum = evtInfo->event_ID()->lumi_block();
   } else if (metaData != 0) {
      lumiNum = metaData->getLumiBlock();
   }

   if (evtInfo != 0) {   
      uint64_t result = evtInfo->event_ID()->detector_mask1();
      result = result << 32;
      result |= evtInfo->event_ID()->detector_mask0();
      runPara.detector_mask_LS = result;
      result = evtInfo->event_ID()->detector_mask3();
      result = result << 32;
      result |= evtInfo->event_ID()->detector_mask2();
      runPara.detector_mask_MS = result;
      
      freeMetaDataStrings.push_back(evtInfo->event_type()->EventType::typeToString());
   } else {
      runPara.detector_mask_LS=0xFFFFFFFFFFFFFFFFULL;
      runPara.detector_mask_MS=0xFFFFFFFFFFFFFFFFULL;
   }
   if (metaData != 0) {
      runPara.max_events   = metaData->getMaxEvents();
      runPara.rec_enable   = metaData->getRecEnable();
      runPara.trigger_type = metaData->getTriggerType();
      runPara.beam_type    = metaData->getBeamType();
      runPara.beam_energy  = metaData->getBeamEnergy();
      for (EventStorage::freeMetaDataStrings::const_iterator iter = metaData->getFreeMetaDataStrings().begin(),
		      iterEnd = metaData->getFreeMetaDataStrings().end(); iter != iterEnd; iter++) {
         if((*iter).find("Compression=") == std::string::npos) {
            freeMetaDataStrings.push_back(*iter);
         }
      }
   } else {
      ATH_MSG_DEBUG("Cannot retrieve MetaData");
      metaData = 0;
      runPara.max_events = 0;
      runPara.rec_enable = 0;
      runPara.trigger_type = 0;
      runPara.beam_type = 0;
      runPara.beam_energy = 0;
   }
   const TagInfo* tagInfo = 0;
   ServiceHandle<StoreGateSvc> ds("DetectorStore", name());
   if (ds.retrieve().isSuccess()) {
      if (ds->retrieve(tagInfo).isFailure() || tagInfo == 0) {
         ATH_MSG_DEBUG("Cannot retrieve TagInfo");
      } else {
         std::string tagName, tagValue;
         if (metaData == 0) { // FIXME: Set TriggerType, BeamType?
            tagName = "beam_energy";
            tagInfo->findTag(tagName, tagValue);
            runPara.beam_type = atof(tagValue.c_str());
         }
         tagName = "GeoAtlas";
         tagInfo->findTag(tagName, tagValue);
         freeMetaDataStrings.push_back(tagName + ": " + tagValue);
         tagName = "IOVDbGlobalTag";
         tagInfo->findTag(tagName, tagValue);
         freeMetaDataStrings.push_back(tagName + ": " + tagValue);
      }
   }

   // Finally, check storegate for meta strings
   this->checkForUserMetadata(freeMetaDataStrings);


   std::string fileNameCore;
   EventStorage::CompressionType compression = m_compressEvents ? EventStorage::ZLIB : EventStorage::NONE;
   int eventStorageVersion = 0;
   if (m_eventStorageVersion.value() == "v5" || m_eventStorageVersion.value() == "run1") {
       eventStorageVersion = 5;
   }
   if (!m_simpleFileName.value().empty()) {
      fileNameCore = m_simpleFileName.value();
      boost::shared_ptr<EventStorage::SimpleFileName> sfn(new EventStorage::SimpleFileName(m_simpleFileName.value()));
      m_dataWriter = ByteStreamDataWriter::makeWriter(eventStorageVersion,
                         m_inputDir.value(), sfn, runPara, m_projectTag.value(),
                         m_streamType.value(), m_streamName.value(),
                         m_streamType.value() + "_" + m_streamName.value(),
                         lumiNum, m_appName.value(), freeMetaDataStrings,
                         m_maxFileNE.value(), m_maxFileMB.value(), compression);
   } else {
      // construct file name
      daq::RawFileName fileNameObj(m_projectTag.value(), run, m_streamType.value(), m_streamName.value(), lumiNum, m_appName.value());
      fileNameCore = fileNameObj.fileNameCore();
      m_dataWriter = ByteStreamDataWriter::makeWriter(eventStorageVersion,
                         m_inputDir.value(), fileNameCore, runPara, freeMetaDataStrings,
                         m_maxFileNE.value(), m_maxFileMB.value(), compression);
   }
   if (!m_dataWriter->good()) {
      ATH_MSG_ERROR("Unable to initialize file");
      return(false);
   } else {
      ATH_MSG_DEBUG("initialized file for with name " << fileNameCore);
   }
   return(true);
}

void ByteStreamEventStorageOutputSvc::checkForUserMetadata(EventStorage::freeMetaDataStrings& freeMetaDataStrings)
{
   ATH_MSG_DEBUG("checkForUserMetadata");

   // Check for attributeLists with user metadata
   if (m_keys.size()>0) {
      StatusCode sc = m_attlistsvc->toBSMetadata(m_keys);
      if (sc.isFailure()) {
         msg() << MSG::WARNING << "Conversion failed for AttributeList service" << endreq;
      }
   }

   ServiceHandle<StoreGateSvc> ds("DetectorStore", name());
   if (ds.retrieve().isSuccess()) {
      const DataHandle<ByteStream::FreeMetadata> userfmdF;
      const DataHandle<ByteStream::FreeMetadata> userfmdL;
      StatusCode sc = ds->retrieve( userfmdF, userfmdL);
      if (sc.isSuccess() && userfmdF != userfmdL) {
         ATH_MSG_DEBUG("Found ByteStreamUserMetadata in DetectorStore");
         for (; userfmdF != userfmdL; ++userfmdF) {
            for (ByteStream::FreeMetadata::const_iterator it = userfmdF->begin(); 
                 it != userfmdF->end(); 
                 ++it) {
               ATH_MSG_DEBUG("Processing " << userfmdF.key() + "::" << it->toString());
               freeMetaDataStrings.push_back("UMD" + ByteStreamFreeMetadataString::foldersep() + userfmdF.key() + ByteStreamFreeMetadataString::foldersep() + it->toString());
            }
         }
      }
      else {
         ATH_MSG_WARNING("Did not find /ByteStream/Output/FreeMetadataStrings in detstore. Skipping.");
      }
   }
   return;
}

//__________________________________________________________________________
// Read the next event.
bool ByteStreamEventStorageOutputSvc::putEvent(RawEvent* re) {
   if (!m_dataWriter) {
      if (!initDataWriter()) {
         ATH_MSG_ERROR("Failed to initialize DataWriter");
         return(false);
      }
   }
   uint32_t size = re->fragment_size_word();
   // write
   OFFLINE_FRAGMENTS_NAMESPACE::PointerType st;
   re->start(st);
   ATH_MSG_DEBUG("event size =  " << size << "  start = " << st);

   // convert to different version
   bool deleteBuffer = false;
   if (m_eformatVersion.value() == "v40" or m_eformatVersion.value() == "run1") {
       // allocate some extra space just in case
       uint32_t bufSize = size + 128;
       auto buf = new OFFLINE_FRAGMENTS_NAMESPACE::DataType[bufSize];

       // This builds no-checksum headers, should use the same
       // checksum type as original event
       size = offline_eformat::old::convert_to_40(st, buf, bufSize);
       if (size == 0) {
           // not enough space in buffer
           ATH_MSG_ERROR("Failed to convert event, buffer is too small");
           delete [] buf;
           return false;
       }

       st = buf;
       deleteBuffer = true;
       ATH_MSG_DEBUG("event size after conversion =  " << size << "  version = " << st[3]);
   }

   if (m_dataWriter->putData(sizeof(OFFLINE_FRAGMENTS_NAMESPACE::DataType) * size,
	   reinterpret_cast<void*>(const_cast<OFFLINE_FRAGMENTS_NAMESPACE::DataType*>(st))) != EventStorage::DWOK) {
      ATH_MSG_ERROR("Failed to write event to DataWriter");
      if (deleteBuffer) delete [] st;
      return(false);
   }
   ++m_totalEventCounter;
   if (deleteBuffer) delete [] st;
   return(true);
}
//__________________________________________________________________________
StatusCode ByteStreamEventStorageOutputSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
   if (ByteStreamOutputSvc::interfaceID().versionMatch(riid)) {
      *ppvInterface = dynamic_cast<ByteStreamOutputSvc*>(this);
   } else {
      // Interface is not directly available: try out a base class
      return(::AthService::queryInterface(riid, ppvInterface));
   }
   addRef();
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode ByteStreamEventStorageOutputSvc::io_reinit() {
   ATH_MSG_INFO("I/O reinitialization...");
   ServiceHandle<IIoComponentMgr> iomgr("IoComponentMgr", name());
   if (!iomgr.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Could not retrieve IoComponentMgr !");
      return(StatusCode::FAILURE);
   }
   if (!iomgr->io_hasitem(this)) {
      ATH_MSG_FATAL("IoComponentMgr does not know about myself !");
      return(StatusCode::FAILURE);
   }
   if (!m_simpleFileName.value().empty()) {
      std::string outputFile = m_simpleFileName.value();
      ATH_MSG_INFO("I/O reinitialization, file = "  << outputFile);
      std::string &fname = outputFile;
      if (!iomgr->io_contains(this, fname)) {
         ATH_MSG_ERROR("IoComponentMgr does not know about [" << fname << "] !");
         return(StatusCode::FAILURE);
      }
      if (!iomgr->io_retrieve(this, fname).isSuccess()) {
         ATH_MSG_FATAL("Could not retrieve new value for [" << fname << "] !");
         return(StatusCode::FAILURE);
      }
      // all good... copy over.
      // modify directory
      m_inputDir.setValue(outputFile.substr(0, outputFile.find_last_of("/")));
      // FIXME: modify file name, not done for now because of IoUtils.update_io_registry vs. merge conflict.
      //m_simpleFileName.setValue(outputFile.substr(outputFile.find_last_of("/") + 1));
   }
   ATH_MSG_DEBUG("Deleting DataWriter");
   m_dataWriter.reset();
   return(this->reinit());
}
