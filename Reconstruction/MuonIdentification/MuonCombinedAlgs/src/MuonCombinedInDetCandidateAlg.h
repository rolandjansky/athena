/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDALGS_MUONCOMBINEDINDETCANDIDATEALG_H
#define MUONCOMBINEDALGS_MUONCOMBINEDINDETCANDIDATEALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>

namespace MuonCombined {
  class IInDetCandidateTool;
}

class MuonCombinedInDetCandidateAlg : public AthAlgorithm
{
 public:
  MuonCombinedInDetCandidateAlg(const std::string& name, ISvcLocator* pSvcLocator);

  ~MuonCombinedInDetCandidateAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  ToolHandle<MuonCombined::IInDetCandidateTool> m_indetCandidateTool;
  ToolHandle<MuonCombined::IInDetCandidateTool> m_indetForwardCandidateTool;
  std::string m_indetTrackParticleLocation;
  std::string m_indetForwardTrackParticleLocation;
  std::string m_candidateCollectionName;
  bool m_doSiliconForwardMuons;

};


#endif
