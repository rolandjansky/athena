
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDALGS_MUONCOMBINEDINDETCANDIDATEALG_H
#define MUONCOMBINEDALGS_MUONCOMBINEDINDETCANDIDATEALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"
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
  
  SG::ReadHandleKeyArray<xAOD::TrackParticleContainer> m_indetTrackParticleLocation;
  SG::ReadHandleKey<xAOD::TrackParticleContainer>  m_indetForwardTrackParticleLocation;
  // SG::ReadHandleKey<InDetCandidateCollection> m_indetCandidateCollectionName;
  SG::WriteHandleKey<InDetCandidateCollection> m_candidateCollectionName;

  bool m_doSiliconForwardMuons;

};


#endif
