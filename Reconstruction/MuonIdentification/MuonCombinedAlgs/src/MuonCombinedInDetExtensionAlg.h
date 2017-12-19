/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDALGS_MUONCOMBINEDINDETEXTENSIONALG_H
#define MUONCOMBINEDALGS_MUONCOMBINEDINDETEXTENSIONALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/MMPrepDataContainer.h"
#include "MuonPrepRawData/sTgcPrepDataContainer.h"
#include <string>

namespace MuonCombined {
  class IMuonCombinedInDetExtensionTool;
}

class MuonCombinedInDetExtensionAlg : public AthAlgorithm
{
 public:
  MuonCombinedInDetExtensionAlg(const std::string& name, ISvcLocator* pSvcLocator);

  ~MuonCombinedInDetExtensionAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  ToolHandleArray<MuonCombined::IMuonCombinedInDetExtensionTool> m_muonCombinedInDetExtensionTools;
  SG::ReadHandleKey<InDetCandidateCollection> m_indetCandidateCollectionName{this,"InDetCandidateLocation","InDetCandidates","ID candidates"};
  SG::ReadHandleKey<Muon::MdtPrepDataContainer> m_MDT_ContainerName{this,"MDTPrepDataLocation","MDT_DriftCircles","MDT prep data"};
  SG::ReadHandleKey<Muon::CscPrepDataContainer> m_CSC_ContainerName{this,"CSCPrepDataLocation","CSC_Clusters","CSC prep data"};
  SG::ReadHandleKey<Muon::RpcPrepDataContainer> m_RPC_ContainerName{this,"RPCPrepDataLocation","RPC_Measurements","RPC prep data"};
  SG::ReadHandleKey<Muon::TgcPrepDataContainer> m_TGC_ContainerName{this,"TGCPrepDataLocation","TGC_Measurements","TGC prep data"};
  SG::ReadHandleKey<Muon::sTgcPrepDataContainer> m_sTGC_ContainerName{this,"sTGCPrepDataLocation","sTGC_Clusters","sTGC prep data"};
  SG::ReadHandleKey<Muon::MMPrepDataContainer> m_MM_ContainerName{this,"MMPrepDataLocation","MM_Clusters","MM prep data"};
  bool m_usePRDs;
};


#endif
