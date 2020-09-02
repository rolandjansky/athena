///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// RootNtupleOutputMetadataTool.h 
// Header file for class Athena::RootNtupleOutputMetadataTool
// Author Peter van Gemmeren <gemmeren@anl.gov>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENAROOTCOMPS_ROOTNTUPLEOUTPUTMETADATATOOL_H
#define ATHENAROOTCOMPS_ROOTNTUPLEOUTPUTMETADATATOOL_H 1

/** @file Athena::RootNtupleOutputMetadataTool.h
 *  @brief Tool for adding d3pd style metadata to output.
 *  @author Jack Cranshaw <gemmeren@anl.gov>
 **/


// fwk
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "AthenaKernel/IAthenaOutputTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

class StoreGateSvc;
class IConversionSvc;
class IClassIDSvc;

namespace Athena {
/** @class Athena::RootNtupleOutputMetadataTool
 *  @brief This is the AthenaRoot version of AthenaServices/AthenaOutputStreamTool.
 **/
class RootNtupleOutputMetadataTool 
  : public extends<::AthAlgTool, IIncidentListener, ::IAthenaOutputTool>
{

public:
  /// Standard AlgTool Constructor
  RootNtupleOutputMetadataTool(const std::string& type,
                       const std::string& name,
                       const IInterface* parent);

  /// Destructor
  virtual ~RootNtupleOutputMetadataTool();

  /// Gaudi AlgTool Interface method implementations:
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode stop() override;
  /// AthenaOutputTool Interface method implementations:
  virtual StatusCode postInitialize() override;
  virtual StatusCode preFinalize() override;
  virtual StatusCode preExecute() override;
  virtual StatusCode preStream() override;
  virtual StatusCode postExecute() override;
  /// Incident service handle listening for Begin/End InputFile incidents
  virtual void handle(const Incident& incident) override;
    
  /// Connect to the output stream
  ///   Must writeMetadata BEFORE streaming
  ///   Only specify "outputName" if one wants to override jobOptions
  StatusCode copyMetadata();
  StatusCode writeMetadata();

private:
  /// Default constructor:
  RootNtupleOutputMetadataTool(); //< not implemented
  RootNtupleOutputMetadataTool(const RootNtupleOutputMetadataTool&); //< not implemented
  RootNtupleOutputMetadataTool& operator=(const RootNtupleOutputMetadataTool&); //< not implemented

  StatusCode addMetadata(const std::string& key, const void* obj, const std::type_info& ti);

private:
  /// ServiceHandle to the data store service
  ServiceHandle<StoreGateSvc>   m_imetaStore;
  ServiceHandle<StoreGateSvc>   m_ometaStore;
  /// ServiceHandle to clid service
  ServiceHandle<IClassIDSvc>    m_clidSvc;

  /// Name of the stream name
  std::string m_streamName;
  std::set<std::string> m_treesWritten;

  /// Name of the output name
  std::string m_outputName;

  /// Name of the output tuple
  std::string m_tupleName;

  bool m_metaWritten;
};

}//> end namespace Athena

#endif
