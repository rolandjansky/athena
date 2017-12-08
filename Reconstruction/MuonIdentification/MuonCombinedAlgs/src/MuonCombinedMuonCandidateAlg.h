/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDALGS_MUONCOMBINEDMUONCANDIDATEALG_H
#define MUONCOMBINEDALGS_MUONCOMBINEDMUONCANDIDATEALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>

namespace MuonCombined {
  class IMuonCandidateTool;
}

class MuonCombinedMuonCandidateAlg : public AthAlgorithm
{
 public:
  MuonCombinedMuonCandidateAlg(const std::string& name, ISvcLocator* pSvcLocator);

  ~MuonCombinedMuonCandidateAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  ToolHandle<MuonCombined::IMuonCandidateTool> m_muonCandidateTool;
  std::string m_muonTrackParticleLocation;
  std::string m_candidateCollectionName;

};


#endif
