/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAM_RPCRAWDATAPROVIDER_H
#define MUONBYTESTREAM_RPCRAWDATAPROVIDER_H

// Base class
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

namespace Muon {
class IMuonRawDataProviderTool;

class RpcRawDataProvider : public AthAlgorithm
{
public:

  //! Constructor.
  RpcRawDataProvider(const std::string &name, ISvcLocator *pSvcLocator);

  //! Initialize
  virtual StatusCode initialize();

  //! Execute
  virtual StatusCode execute();

  //! Finalize
  virtual StatusCode finalize();

  //! Destructur
  ~RpcRawDataProvider();


private:

  ToolHandle<Muon::IMuonRawDataProviderTool>  m_rawDataTool;
};
} // ns end

#endif



