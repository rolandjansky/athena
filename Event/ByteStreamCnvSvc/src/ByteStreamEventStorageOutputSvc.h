/* Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration */

#ifndef EVENT_BYTESTREAMCNVSVC_SRC_BYTESTREAMEVENTSTORAGEOUTPUTSVC_H_
#define EVENT_BYTESTREAMCNVSVC_SRC_BYTESTREAMEVENTSTORAGEOUTPUTSVC_H_

/** @file ByteStreamEventStorageOutputSvc.h
 *  @brief This file contains the class definition for the
 *         ByteStreamEventStorageOutputSvc class.
 *
 *  Writes out BS events to a file, the most obvious thing to do. Used to test 
 *  running trigger code in offline athena and to write DRAW output, for example
 *    * Trigger test:    test_trig_data_v1Dev_writeBS_build.py.
 *    * DRAW production:
 *        from ByteStreamCnvSvc import WriteByteStream
 *        StreamBSFileOutput = WriteByteStream.getStream("EventStorage",
 *                                                       "StreamBSFileOutput")
 *
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  @author Frank Berghaus <fberghaus@anl.gov>
 **/
#include <memory>
#include <mutex>
#include <string>

#include "ByteStreamDataWriter.h"
#include "ByteStreamCnvSvc/ByteStreamOutputSvc.h"

#include "AthenaKernel/SlotSpecificObj.h"
#include "ByteStreamData/ByteStreamMetadataContainer.h"
#include "ByteStreamData/RawEvent.h"
#include "EventStorage/EventStorageRecords.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"

#include "GaudiKernel/extends.h"
#include "GaudiKernel/IIoComponent.h"
#include "GaudiKernel/IIoComponentMgr.h" 
#include "GaudiKernel/ServiceHandle.h"


class EventInfo;
class ByteStreamMetadata;


/** @class ByteStreamEventStorageOutputSvc
 *  @brief This class provides the services for writing FullEventFragment into
 *         file.
 *  This class implements the interface ByteStreamOutputSvc for the conversion
 *  service to write the output to a file.
 **/
class ByteStreamEventStorageOutputSvc :
    public extends< ByteStreamOutputSvc, IIoComponent > {
 public:
  using extends::extends;

  /// Constructors:
  ByteStreamEventStorageOutputSvc(
      const std::string& name, ISvcLocator* pSvcLocator);

   /// Destructor.
  virtual ~ByteStreamEventStorageOutputSvc() {}

  /// Required of all Gaudi Services
  StatusCode initialize() override;
  StatusCode stop() override;
  StatusCode finalize() override;
  StatusCode
      queryInterface(const InterfaceID& riid, void** ppvInterface) override;


  /// Implementation of the ByteStreamOutputSvc interface method putEvent.
  bool putEvent(RawEvent* re) override;
  bool putEvent(RawEvent* re, const EventContext& ctx) override;

  // Callback method to reinitialize the internal state of the component
  // for I/O purposes (e.g. upon @c fork(2))
  StatusCode io_reinit() override;

 private:  // properties
  //!< directory for the data files
  Gaudi::Property<std::string> m_inputDir{this, "OutputDirectory", "",
    "directory for the data files"};

  //!< Project Tag
  Gaudi::Property<std::string> m_projectTag{this, "ProjectTag", "",
    "part of filename"};

  //!< Application Name
  Gaudi::Property<std::string> m_appName{this, "AppName", "",
    "part of filename"};

  //!< File Tag
  Gaudi::Property<std::string> m_fileTag{this, "FileTag", "",
    "part of filename"};

  //!< stream type
  Gaudi::Property<std::string> m_streamType{this, "StreamType",  "",
    "part of filename"};

  //!< stream name
  Gaudi::Property<std::string> m_streamName{this, "StreamName", "",
    "part of filename"};

  //!< eformat event version to produce, "v40" for run1, or "current"
  Gaudi::Property<std::string> m_eformatVersion{this, "EformatVersion",
    "current", "Version of the event format data, use \"v40\" or \"run1\" "
    "for run1, \"current\" for most current version (default)."};

  //!< EventStorage BS version to produce, "v5" for run1, or "current"
  Gaudi::Property<std::string> m_eventStorageVersion{this,
    "EventStorageVersion", "current", "Version of the ByteStream file data, "
        "use \"v5\" or \"run1\" for run1, \"current\" for most current version "
        "(default)."};

  //!< stream name for multiple output
  Gaudi::Property<std::string> m_bsOutputStreamName{this, "BSOutputStreamName",
    name(), "stream name for muliple output"};

  //! use this string for filename, not from the "AgreedFileName"
  Gaudi::Property<std::string> m_simpleFileName{this, "SimpleFileName", "",
    "for filename not from AgreedFileName"};

  //!< luminosity block number
  Gaudi::Property<int> m_lumiBlockNumber{this, "LumiBlockNumber", 0,
    "part of filename"};

  //!< run number
  Gaudi::Property<int> m_run{this, "RunNumber", 0, "part of filename"};

  //!< Dump fragments
  Gaudi::Property<bool> m_dump{this, "DumpFlag", false, "dump fragments"};

  //!< Write eventless files
  Gaudi::Property<bool> m_writeEventless{this, "WriteEventlessFiles", true,
    ""};

  //!< Compress events
  Gaudi::Property<bool> m_compressEvents{this, "CompressEvents", false, ""};

  //!< number of MB per file
  Gaudi::Property<unsigned int> m_maxFileMB{this, "MaxFileMB", 10000,
    "MB per file"};

  //!< number of events per file
  Gaudi::Property<unsigned int> m_maxFileNE{this, "MaxFileNE", 100000,
    "evens per file"};

  SG::ReadHandleKey< xAOD::EventInfo > m_eventInfoKey{this, "EventInfoKey",
    "EventInfo", "Key for EventInfo object in event store"};

  SG::ReadHandleKey< ByteStreamMetadataContainer > m_byteStreamMetadataKey{this,
    "ByteStreamMetadataKey", "MetaDataStore+ByteStreamMetadata",
    "Key for bytestream metadata object in metadata store"};

  ServiceHandle< IIoComponentMgr > m_ioMgr{"IoComponentMgr", name()};

 private:  // data
  //! number of event counter
  int m_totalEventCounter{0};

  bool m_isRun1{false};

  //! pointer to DataWriter
  std::unique_ptr<ByteStreamDataWriter> m_dataWriter;

  //! mutex to lock data writer during initialization or writing
  std::mutex m_dataWriterMutex;

 private:  // internal helper functions
  /// reinitialize the service when a @c fork() occured/was-issued
  StatusCode reinit();

  /// initialize EventStorage's DataWriter
  bool initDataWriter(const EventContext* ctx = nullptr);
  bool initDataWriterContents(
      const xAOD::EventInfo*, const ByteStreamMetadata*);
  const ByteStreamMetadata * getByteStreamMetadata();
  const ByteStreamMetadata * getByteStreamMetadata(const EventContext& ctx);

  struct EventCache {
    inline void releaseEvent() {
      buffer.reset(nullptr);
      size = 0;
    }

    //! Underlying data structure
    std::unique_ptr< uint32_t[] > buffer{nullptr};

    unsigned int size{0};
  };

  //! Cache of event data for each slot
  SG::SlotSpecificObj<EventCache> m_eventCache;

  /// Create DataWriter parameters from job properties
  void updateDataWriterParameters(DataWriterParameters&) const;

  /// Create DataWriter parameters from EventInfo
  void updateDataWriterParameters(DataWriterParameters& params,
                                  const xAOD::EventInfo& evtInfo) const;

  /// Create DataWriter parameters from MetaData
  void updateDataWriterParameters(DataWriterParameters& params,
                                  const ByteStreamMetadata& metaData) const;

};

#endif  // EVENT_BYTESTREAMCNVSVC_SRC_BYTESTREAMEVENTSTORAGEOUTPUTSVC_H_
