/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file ByteStreamSingleEventInputSvc.cxx
 *  @brief This file contains the implementation for the ByteStreamSingleEventInputSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "ByteStreamSingleEventInputSvc.h"
#include "DumpFrags.h"
#include "ByteStreamData/ByteStreamMetadata.h"
#include "EventStorage/pickDataReader.h"
#include "EventStorage/EventStorageRecords.h"

#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/FileIncident.h"

#include <unistd.h>

#include "eformat/HeaderMarker.h"
#include "eformat/Version.h"
#include "eformat/old/util.h"

#include "StoreGate/StoreGateSvc.h"

//----------------------------------------------------------------------------------------
ByteStreamSingleEventInputSvc::ByteStreamSingleEventInputSvc(const std::string& name, ISvcLocator* svcloc) :
	::AthService(name, svcloc),
	m_re(0),
	m_reader(0),
	m_mdSvc("StoreGateSvc/InputMetaDataStore", name),
	m_incidentSvc("IncidentSvc", name),
	m_currentFilename() {
   declareProperty("DumpFlag", m_dump = false);
}
//----------------------------------------------------------------------------------------
ByteStreamSingleEventInputSvc::~ByteStreamSingleEventInputSvc() {
   delete m_re; m_re = 0;
   delete m_reader; m_reader = 0;
}
//----------------------------------------------------------------------------------------
StatusCode ByteStreamSingleEventInputSvc::initialize() {
   if (!::AthService::initialize().isSuccess()) {
      ATH_MSG_FATAL("Cannot initialize AthService base class.");
      return(StatusCode::FAILURE);
   }
   if (!m_incidentSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get IncidentSvc.");
      return(StatusCode::FAILURE);
   }
   if (!m_mdSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get InputMetaDataStore.");
      return(StatusCode::FAILURE);
   }
   return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode ByteStreamSingleEventInputSvc::stop() {
   // Fire LastInputFile incident
   FileIncident lastInputFileIncident(name(), "LastInputFile", "end");
   m_incidentSvc->fireIncident(lastInputFileIncident);
   return(StatusCode::SUCCESS);
}
//----------------------------------------------------------------------------------------
StatusCode ByteStreamSingleEventInputSvc::finalize() {
   if (!m_mdSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release InputMetaDataStore.");
   }
   if (!m_incidentSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release IncidentSvc.");
   }
   return(::AthService::finalize());
}
//----------------------------------------------------------------------------------------
const RawEvent* ByteStreamSingleEventInputSvc::getEvent(const std::string& filename, const long long pos) {
   // Some sanity check here. Is the filename empty?
   if (filename.empty()) {
      ATH_MSG_ERROR("ByteStreamSingleEventInputSvc: Empty filename recieved for ByteStream " << filename);
      return(0);
   }
   // Check if the file with given name is already opened
   if (filename != m_currentFilename) {
      if (!m_currentFilename.empty()) {
         FileIncident endInputFileIncident(name(), "EndInputFile", "FID:" + m_reader->GUID(), m_reader->GUID());
         m_incidentSvc->fireIncident(endInputFileIncident);
      }
      if (!m_mdSvc->clearStore().isSuccess()) {
         ATH_MSG_WARNING("Unable to clear Input MetaData Proxies");
      }
      // Delete old reader if exists
      delete m_reader; m_reader = 0;
      // Pick up new reader
      m_reader = pickDataReader(filename);
      // Check if reader pick up was successful. Sanity check mostly.
      if (m_reader == 0){
         ATH_MSG_ERROR("Cannot pick up valid reader for the file : " << filename);
         return(0);
      }
      if (m_currentFilename.empty()) {
         FileIncident firstInputFileIncident(name(), "FirstInputFile", "BSF:" + m_reader->GUID(), m_reader->GUID());
         m_incidentSvc->fireIncident(firstInputFileIncident);
      }
      // Double check of a sort here. Interrogate reader for a file name.
      // Remove later for efficiency and use  just filename?
      ATH_MSG_DEBUG("Picked valid file: " << m_reader->fileName());
      // All was good and a new reader is picked up
      // Call new file as "currently open file"
      m_currentFilename = filename;

      // Get ByteStream Metadata into Input MetaData Store
      std::bitset<64> word1;
      std::bitset<64> word2;
      for (unsigned int i=0; i<64; ++i) {
         word1[i] = m_reader->detectorMask()[i];
         word2[i] = m_reader->detectorMask()[i+64];
      }
      ByteStreamMetadata* metadata = new ByteStreamMetadata(m_reader->runNumber(), m_reader->eventsInFile(),
	      m_reader->maxEvents(), m_reader->recEnable(), m_reader->triggerType(), word1.to_ulong(), word2.to_ulong(),
	      m_reader->beamType(), m_reader->beamEnergy(), m_reader->GUID(), m_reader->stream(),
	      m_reader->projectTag(), m_reader->lumiblockNumber(), m_reader->freeMetaDataStrings());
      if (!m_mdSvc->record(metadata, "ByteStreamMetadata").isSuccess()) {
         delete metadata; metadata = 0;
         ATH_MSG_WARNING("Unable to record Input MetaData for ByteStream");
      }
      FileIncident beginInputFileIncident(name(), "BeginInputFile", "BSF:" + m_reader->GUID(), m_reader->GUID());
      m_incidentSvc->fireIncident(beginInputFileIncident);
   } // current filename check
   char *buf;
   unsigned int eventSize;

   // Temporarily, until tdaq-common switches to "long long int" for event position
   long int tmppos;
   tmppos = (long)pos;
   // Another sanity check for truncation error of conversion of "long long" to "long".
   // Probably never needed after tdaq update, mildly useful before that.
   long long int check_pos;
   check_pos = (long long)tmppos;
   if (check_pos != pos) {
      // Truncation error here
      ATH_MSG_ERROR("Truncation error for event position in file " << pos << ": " << check_pos);
      return(0);
   }
   DRError ecode = m_reader->getData(eventSize, &buf, tmppos);
   if (DROK != ecode) {
      ATH_MSG_ERROR("Error reading event in position: " << pos);
      return(0);
   }
   // Delete the old event
   if (m_re) {
      OFFLINE_FRAGMENTS_NAMESPACE::PointerType st = 0;
      m_re->start(st);
      delete [] st;
      delete m_re; m_re = 0;
   }
   OFFLINE_FRAGMENTS_NAMESPACE::DataType* fragment = reinterpret_cast<OFFLINE_FRAGMENTS_NAMESPACE::DataType*>(buf);
   // Check fragment type
   const uint32_t headWord = fragment[0];
   ATH_MSG_DEBUG("First word of the fragment " << MSG::hex << headWord << MSG::dec);
   // Format version
   const uint32_t formatVersion = eformat::helper::Version(fragment[3]).major_version();
   ATH_MSG_DEBUG("Format version " << MSG::hex << formatVersion << MSG::dec);
   // Error message
   if ((formatVersion != eformat::MAJOR_DEFAULT_VERSION)
	   && (formatVersion != eformat::MAJOR_V24_VERSION)
	   && (formatVersion != eformat::MAJOR_V30_VERSION)
	   && (formatVersion != eformat::MAJOR_V31_VERSION)) {
      ATH_MSG_ERROR("Unsupported Format Version: " << MSG::hex << formatVersion << MSG::dec);
   }
   if (eformat::FULL_EVENT == headWord || 0xcc1234cc == headWord) {// ROS = 0xcc1234cc
      // Convert old ROS fragment
      if (formatVersion != eformat::MAJOR_DEFAULT_VERSION) {
         // 100 for increase of data-size due to header conversion
         uint32_t newRosSize = eventSize + 100;
         OFFLINE_FRAGMENTS_NAMESPACE::DataType* newFragment = new OFFLINE_FRAGMENTS_NAMESPACE::DataType[newRosSize];
         eformat::old::convert(fragment, newFragment, newRosSize);
         // Delete old fragment
         delete [] fragment;
         // Set new pointer
         fragment = newFragment;
      }
      // This is a FullEventFragment make a new FEFrag in memory from it
      m_re = new RawEvent(fragment);
      ATH_MSG_DEBUG("Made an FullEventFragment from ES");
      // Check validity
      try {
         m_re->check_tree();
      } catch (...) {
         // Bad event
         ATH_MSG_ERROR("Failed to create FullEventFragment");
         delete m_re; m_re = 0;
         delete [] fragment;
         return(0);
      }
   } else {
      // Unknown fragment
      ATH_MSG_ERROR("Unknown Header word in input fragment" << MSG::hex << headWord);
      return(0);
   }
   if (m_dump) {
      DumpFrags::dump(m_re);
   }
   return(m_re);
}
//----------------------------------------------------------------------------------------
StatusCode ByteStreamSingleEventInputSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
   if (IByteStreamSingleEventInputSvc::interfaceID().versionMatch(riid)) {
      *ppvInterface = dynamic_cast<IByteStreamSingleEventInputSvc*>(this);
   } else {
      // Interface is not directly available: try out a base class
      return(::AthService::queryInterface(riid, ppvInterface));
   }
   addRef();
   return(StatusCode::SUCCESS);
}
