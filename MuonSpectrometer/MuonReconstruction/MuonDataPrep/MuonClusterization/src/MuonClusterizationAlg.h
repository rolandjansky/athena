/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCLUSTERIZATIONALG_H
#define MUONCLUSTERIZATIONALG_H

#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

namespace Muon {
  class MuonIdHelperTool;
  class IMuonClusterizationTool;
}

class MuonClusterizationAlg : public AthAlgorithm
{
 public:
  MuonClusterizationAlg(const std::string& name, ISvcLocator* pSvcLocator);

 public:
  virtual ~MuonClusterizationAlg();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 private:

  std::string m_tgcPrdLocationInput;        //!< Location of input TgcPrepData
  std::string m_tgcPrdLocationOutput;       //!< Location of output TgcPrepData

  std::string m_rpcPrdLocationInput;        //!< Location of input RpcPrepData
  std::string m_rpcPrdLocationOutput;       //!< Location of output RpcPrepData

  ToolHandle<Muon::MuonIdHelperTool> m_idHelper;    //!< id helper Tool 
  ToolHandle<Muon::IMuonClusterizationTool> m_clusterTool;    //!< clustering Tool 

};

#endif 
