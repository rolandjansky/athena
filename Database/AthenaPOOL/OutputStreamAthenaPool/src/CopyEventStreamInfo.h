/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COPYEVENTSTREAMINFO_H
#define COPYEVENTSTREAMINFO_H

/** @file CopyEventStreamInfo.h
 *  @brief This file contains the class definition for the CopyEventStreamInfo class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "GaudiKernel/ServiceHandle.h"

#include "AthenaBaseComps/AthAlgTool.h"

#include "AthenaKernel/IMetaDataTool.h"
#include "AthenaKernel/IMetaDataSvc.h"

#include <string>

class StoreGateSvc;

/** @class CopyEventStreamInfo 
 *  @brief This class provides an algorithm to make the EventStreamInfo object and update it.
 **/
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
   virtual StatusCode beginInputFile(const SG::SourceID& = "Serial");
 
   /// Function called when the currently open input file got completely
   /// processed
   virtual StatusCode endInputFile(const SG::SourceID& = "Serial");

   /// Function called when the tool should write out its metadata
   virtual StatusCode metaDataStop();

private:
   /// Key, the StoreGate key for the EventStreamInfo object.
   StringProperty m_key;

   /// Access to output MetaDataStore through MetaDataSvc (using MetaContainers)
   ServiceHandle<IMetaDataSvc> m_metaDataSvc;
   /// MetaDataStore for input 
   ServiceHandle<StoreGateSvc> m_inputMetaDataStore;
};
#endif
