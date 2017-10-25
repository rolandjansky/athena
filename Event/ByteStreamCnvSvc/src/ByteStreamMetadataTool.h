/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMMETADATATOOL_H
#define BYTESTREAMMETADATATOOL_H

/** @file ByteStreamMetadataTool.h
 *  @brief This file contains the class definition for the ByteStreamMetadataTool class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: $
 **/

#include "GaudiKernel/ServiceHandle.h"
//#include "GaudiKernel/IIncidentListener.h"
//#include "AthenaKernel/IFileTransition.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaPoolKernel/IMetaDataTool.h"

#include <string>

class StoreGateSvc;

/** @class ByteStreamMetadataTool
 *  @brief This class provides the MetaDataTool for ByteStreamMetadata objects
 **/
class ByteStreamMetadataTool : public ::AthAlgTool,
	virtual public IMetaDataTool {
public: 
   /// Standard Service Constructor
   ByteStreamMetadataTool(const std::string& type, const std::string& name, const IInterface* parent);
   /// Destructor
   virtual ~ByteStreamMetadataTool();

   /// Gaudi Service Interface method implementations:
   StatusCode initialize();
   StatusCode finalize();

   /// Incident service handle listening for BeginInputFile and EndInputFile.
   //void handle(const Incident& incident);
   virtual StatusCode beginInputFile();
   virtual StatusCode endInputFile();
   virtual StatusCode metaDataStop();

private:
   typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
   StoreGateSvc_t m_pMetaDataStore;
   StoreGateSvc_t m_pInputStore;
};

#endif
