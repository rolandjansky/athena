/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "ByteStreamCnvSvc/IByteStreamFreeMetadataSvc.h"
#include "ByteStreamData/RawEvent.h"

#include "GaudiKernel/IIoComponent.h"
#include "GaudiKernel/ServiceHandle.h"

class EventInfo;
class ByteStreamMetadata;

/** @classByteStreamEventStorageOutputSvc
 *  @brief This class provides the services for writing FullEventFragment into file.
 *  This class implements the interface ByteStreamOutputSvc for the conversion service to write the output.
 **/
class ByteStreamEventStorageOutputSvc : public ByteStreamOutputSvc, virtual public IIoComponent {
public:
   /// Constructors:
   ByteStreamEventStorageOutputSvc(const std::string& name, ISvcLocator* svcloc);

   /// Destructor.
   virtual ~ByteStreamEventStorageOutputSvc();

   /// Required of all Gaudi Services
   virtual StatusCode initialize();
   /// Required of all Gaudi Services
   virtual StatusCode stop();
   virtual StatusCode finalize();

   /// Required of all Gaudi services:  see Gaudi documentation for details
   StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

   /// Implementation of the ByteStreamOutputSvc interface method putEvent.
   virtual bool putEvent(RawEvent* re);

   /// Callback method to reinitialize the internal state of the component for I/O purposes (e.g. upon @c fork(2))
   virtual StatusCode io_reinit();

private: // internal member functions
   /// reinitialize the service when a @c fork() occured/was-issued
   StatusCode reinit();
   void checkForUserMetadata(EventStorage::freeMetaDataStrings& freeMetaDataStrings);

private: // data
   int                m_totalEventCounter; //!< number of event counter
   std::unique_ptr<ByteStreamDataWriter> m_dataWriter; //!< pointer to DataWriter

private: // properties
   StringProperty        m_inputDir;       //!< directory for the data files
   StringProperty        m_projectTag;     //!< Project Tag
   StringProperty        m_appName;        //!< Application Name
   StringProperty        m_fileTag;        //!< File Tag
   StringProperty        m_streamType;     //!< stream type
   StringProperty        m_streamName;     //!< stream name
   IntegerProperty       m_lumiBlockNumber;//!< luminosity block number
   IntegerProperty       m_run;	           //!< run number

   BooleanProperty       m_dump;	   //!< Dump fragments
   BooleanProperty       m_writeEventless; //!< Write eventless files
   BooleanProperty       m_compressEvents; //!< Compress events

   UnsignedIntegerProperty m_maxFileMB;    //!< number of MB per file
   UnsignedIntegerProperty m_maxFileNE;    //!< number of events per file

   StringProperty     m_eformatVersion;    //!< eformat event version to produce, "v40" for run1, or "current"
   StringProperty     m_eventStorageVersion;  //!< EventStorage BS version to produce, "v5" for run1, or "current"
   StringProperty     m_bsOutputStreamName;//!< stream name for multiple output
   StringProperty     m_simpleFileName;    //!< use this string for filename, not from the "AgreedFileName"
   std::vector<std::string> m_keys;

   ServiceHandle<IByteStreamFreeMetadataSvc> m_attlistsvc;

private: // internal helper functions
   /// initialize EventStorage's DataWriter
   bool initDataWriter();
   bool initDataWriterContents(const EventInfo*, const ByteStreamMetadata* );
};

#endif
