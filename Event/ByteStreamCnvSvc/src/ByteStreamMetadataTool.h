/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMMETADATATOOL_H
#define BYTESTREAMMETADATATOOL_H

/** @file ByteStreamMetadataTool.h
 *  @brief This file contains the class definition for the ByteStreamMetadataTool class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  @author Frank Berghaus <fberghaus@anl.gov>
 *  $Id: $
 **/

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/SourceID.h"
#include "AthenaKernel/IMetaDataTool.h"

#include <string>
#include <set>

class StoreGateSvc;


/** @class ByteStreamMetadataTool
 *  @brief This class provides the MetaDataTool for ByteStreamMetadata objects
 **/
class ByteStreamMetadataTool
:         public ::AthAlgTool,
  virtual public IMetaDataTool
{
public: 
  /// Standard Service Constructor
  ByteStreamMetadataTool(const std::string& type, const std::string& name,
      const IInterface* parent);


  /// Destructor
  virtual ~ByteStreamMetadataTool();


  /// Gaudi Service Interface method implementations:
  StatusCode initialize();
  StatusCode finalize  ();


  /// Incident service handle listening for BeginInputFile and EndInputFile.
  virtual StatusCode beginInputFile();
  virtual StatusCode beginInputFile(const SG::SourceID&);
  virtual StatusCode metaDataStop  ();
  virtual StatusCode metaDataStop  (const SG::SourceID&);
  virtual StatusCode endInputFile  ();
  virtual StatusCode endInputFile  (const SG::SourceID&);


private:
  typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
  StoreGateSvc_t m_metadataStore;
  StoreGateSvc_t m_inputStore;

  std::set<std::string> keysFromInput() const;
};

#endif // BYTESTREAMMETADATATOOL_H
