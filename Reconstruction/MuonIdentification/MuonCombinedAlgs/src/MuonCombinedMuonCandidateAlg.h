/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDALGS_MUONCOMBINEDMUONCANDIDATEALG_H
#define MUONCOMBINEDALGS_MUONCOMBINEDMUONCANDIDATEALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "MuonCombinedEvent/MuonCandidateCollection.h"
#include "MuonCombinedToolInterfaces/IMuonCandidateTool.h"
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
  ToolHandle<MuonCombined::IMuonCandidateTool> m_muonCandidateTool{this,"MuonCandidateTool","MuonCombined::MuonCandidateTool/MuonCandidateTool","Muon Candidate Tool"};
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_muonTrackParticleLocation{this,"MuonSpectrometerTrackParticleLocation","MuonSpectrometerTrackParticles","MS Track Particle collection"};
  SG::WriteHandleKey<MuonCandidateCollection> m_candidateCollectionName{this,"MuonCandidateLocation","MuonCandidates","Muon candidate collection"};

};


#endif
