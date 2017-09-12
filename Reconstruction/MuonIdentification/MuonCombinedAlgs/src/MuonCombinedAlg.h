/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDALGS_MUONCOMBINEDALG_H
#define MUONCOMBINEDALGS_MUONCOMBINEDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include "MuonCombinedEvent/MuonCandidateCollection.h"
#include "MuonCombinedToolInterfaces/IMuonCombinedTool.h"
#include <string>

namespace MuonCombined {
  class IMuonCombinedTool;
}

class MuonCombinedAlg : public AthAlgorithm
{
 public:
  MuonCombinedAlg(const std::string& name, ISvcLocator* pSvcLocator);

  ~MuonCombinedAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  ToolHandle<MuonCombined::IMuonCombinedTool> m_muonCombinedTool{this,"MuonCombinedTool","MuonCombined::MuonCombinedTool/MuonCombinedTool","muon combined tool"};
  SG::ReadHandleKey<InDetCandidateCollection> m_indetCandidateCollectionName{this,"InDetCandidateLocation","InDetCandidates","name of ID candidate collection"};
  SG::ReadHandleKey<MuonCandidateCollection> m_muonCandidateCollectionName{this,"MuonCandidateLocation","MuonCandidates","name of muon candidate collection"};

};


#endif
