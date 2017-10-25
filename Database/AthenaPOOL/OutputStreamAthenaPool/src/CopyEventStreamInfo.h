/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COPYEVENTSTREAMINFO_H
#define COPYEVENTSTREAMINFO_H
/** @file CopyEventStreamInfo.h
 *  @brief This file contains the class definition for the CopyEventStreamInfo class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
//#include "GaudiKernel/IIncidentListener.h"
#include "AthenaPoolKernel/IMetaDataTool.h"

#include <string>

class StoreGateSvc;

/** @class CopyEventStreamInfo 
 *  @brief This class provides an algorithm to make the EventStreamInfo object and update it.
 **/
//class CopyEventStreamInfo : public ::AthAlgTool, virtual public IMetaDataTool, virtual public IIncidentListener {
class CopyEventStreamInfo : public ::AthAlgTool, virtual public IMetaDataTool {
public:
   /// Standard AlgTool Constructor
   CopyEventStreamInfo(const std::string& type, const std::string& name, const IInterface* parent);
   /// Destructor
   virtual ~CopyEventStreamInfo();

   /// AthAlgTool Interface method implementations:
   StatusCode initialize();
   StatusCode finalize();

   /// Function called when a new input file is opened
   virtual StatusCode beginInputFile();
 
   /// Function called when the currently open input file got completely
   /// processed
   virtual StatusCode endInputFile();

   /// Function called when the tool should write out its metadata
   virtual StatusCode metaDataStop();

   /// Incident service handle listening for BeginInputFile and EndInputFile.
   //void handle(const Incident& incident);

private:
   /// Key, the StoreGate key for the EventStreamInfo object.
   StringProperty m_key;

   /// Pointer to the metadata stores
   ServiceHandle<StoreGateSvc> m_metaDataStore;
   ServiceHandle<StoreGateSvc> m_inputMetaDataStore;
};
#endif
