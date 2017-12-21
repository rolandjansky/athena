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

namespace Trk {
  class ITrackSelectorTool;
}

namespace Muon {
  class IMuonSystemExtensionTool;
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
  bool m_doSiliconForwardMuons;

  float m_extThreshold;
  
  SG::ReadHandleKeyArray<xAOD::TrackParticleContainer> m_indetTrackParticleLocation;
  SG::ReadHandleKey<xAOD::TrackParticleContainer>  m_indetForwardTrackParticleLocation;
  SG::WriteHandleKey<InDetCandidateCollection> m_candidateCollectionName;
  ToolHandle <Trk::ITrackSelectorTool> m_trackSelector;
  ToolHandle <Trk::ITrackSelectorTool> m_forwardTrackSelector;
  ToolHandle <Trk::ITrackSelectorTool> m_currentTrackSelector;
  ToolHandle <Muon::IMuonSystemExtensionTool> m_muonSystemExtensionTool;

  void create(const xAOD::TrackParticleContainer& indetTrackParticles,
	      InDetCandidateCollection& outputContainer,
	      bool flagCandidateAsSiAssociated=false) const;
  StatusCode create(const SG::ReadHandleKey<xAOD::TrackParticleContainer>& location,
		    std::unique_ptr<InDetCandidateCollection>& collection,
		    bool flagCandidateAsSiAssociate=false) const;

  bool isValidTrackParticle(const xAOD::TrackParticle* const tp) const;
  void printTrackParticleInfo(const xAOD::TrackParticle* const tp, const std::string& what) const;

  int getCount(const xAOD::TrackParticle& tp, xAOD::SummaryType type) const;
};


#endif
