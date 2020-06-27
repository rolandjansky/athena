/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
   */

#ifndef BYTESTREAMEVENTSTORAGEOUTPUTSVC_H
#define BYTESTREAMEVENTSTORAGEOUTPUTSVC_H

/** @file ByteStreamEventStorageOutputSvc.h
 *  @brief This file contains the class definition for the ByteStreamEventStorageOutputSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include <memory>

#include "EventStorage/EventStorageRecords.h"
#include "ByteStreamDataWriter.h"

#include "ByteStreamCnvSvc/ByteStreamOutputSvc.h"
#include "ByteStreamData/RawEvent.h"

#include "GaudiKernel/IIoComponent.h"
#include "GaudiKernel/ServiceHandle.h"


class EventInfo;
class ByteStreamMetadata;


/** @class ByteStreamEventStorageOutputSvc
 *  @brief This class provides the services for writing FullEventFragment into file.
 *  This class implements the interface ByteStreamOutputSvc for the conversion service to write the output.
 **/
class ByteStreamEventStorageOutputSvc
:         public ByteStreamOutputSvc
, virtual public IIoComponent
{
public:
  /// Constructors:
  ByteStreamEventStorageOutputSvc(const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructor.
  virtual ~ByteStreamEventStorageOutputSvc();

  /// Required of all Gaudi Services
  virtual StatusCode initialize() override;
  virtual StatusCode stop      () override;
  virtual StatusCode finalize  () override;

  /// Required of all Gaudi services:  see Gaudi documentation for details
  StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;

  /// Implementation of the ByteStreamOutputSvc interface method putEvent.
  virtual bool putEvent(RawEvent* re) override;
  virtual bool putEvent(RawEvent* re, const EventContext& ctx) override;

  // Callback method to reinitialize the internal state of the component
  // for I/O purposes (e.g. upon @c fork(2))
  virtual StatusCode io_reinit() override;


private: // internal member functions
  /// reinitialize the service when a @c fork() occured/was-issued
  StatusCode reinit();
  void checkForUserMetadata(EventStorage::freeMetaDataStrings& freeMetaDataStrings);

private: // data
  int                                   m_totalEventCounter; //!< number of event counter
  std::unique_ptr<ByteStreamDataWriter> m_dataWriter;        //!< pointer to DataWriter

private: // properties
  Gaudi::Property<std::string>  m_inputDir;       //!< directory for the data files
  Gaudi::Property<std::string>  m_projectTag;     //!< Project Tag
  Gaudi::Property<std::string>  m_appName;        //!< Application Name
  Gaudi::Property<std::string>  m_fileTag;        //!< File Tag
  Gaudi::Property<std::string>  m_streamType;     //!< stream type
  Gaudi::Property<std::string>  m_streamName;     //!< stream name
  Gaudi::Property<int>          m_lumiBlockNumber;//!< luminosity block number
  Gaudi::Property<int>          m_run;	          //!< run number
  Gaudi::Property<bool>         m_dump;	          //!< Dump fragments
  Gaudi::Property<bool>         m_writeEventless; //!< Write eventless files
  Gaudi::Property<bool>         m_compressEvents; //!< Compress events
  Gaudi::Property<unsigned int> m_maxFileMB;      //!< number of MB per file
  Gaudi::Property<unsigned int> m_maxFileNE;      //!< number of events per file
  Gaudi::Property<std::string>  m_eformatVersion; //!< eformat event version to produce, "v40" for run1, or "current"
  Gaudi::Property<std::string>  m_eventStorageVersion;  //!< EventStorage BS version to produce, "v5" for run1, or "current"
  Gaudi::Property<std::string>  m_bsOutputStreamName;//!< stream name for multiple output
  Gaudi::Property<std::string>  m_simpleFileName; //!< use this string for filename, not from the "AgreedFileName"

private: // internal helper functions
  /// initialize EventStorage's DataWriter
  bool initDataWriter();
  bool initDataWriterContents(const EventInfo*, const ByteStreamMetadata* );
};

#endif // BYTESTREAMEVENTSTORAGEOUTPUTSVC_H
