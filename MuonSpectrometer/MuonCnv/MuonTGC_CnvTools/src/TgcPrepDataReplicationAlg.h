/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TgcPrepDataReplicationAlg_h
#define TgcPrepDataReplicationAlg_h

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonTGC_CnvTools/ITgcPrepDataReplicationTool.h"

namespace Muon 
{

  class TgcPrepDataReplicationAlg:public AthAlgorithm
  {
  public:
    TgcPrepDataReplicationAlg(const std::string& name, ISvcLocator* pSvcLocator); // Constructor
    
    StatusCode initialize();
    StatusCode finalize();
    StatusCode execute();
    
  private:
    /** ITgcPrepDataReplicationTool.h */
    ToolHandle<ITgcPrepDataReplicationTool> m_tool{this, "Tool", "Muon::TgcPrepDataReplicationTool"};
  };
} // end of namespace 

#endif // TgcPrepDataReplicationAlg_h
