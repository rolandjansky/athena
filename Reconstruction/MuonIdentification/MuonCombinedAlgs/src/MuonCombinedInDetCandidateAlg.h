/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDALGS_MUONCOMBINEDINDETCANDIDATEALG_H
#define MUONCOMBINEDALGS_MUONCOMBINEDINDETCANDIDATEALG_H

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include "MuonRecToolInterfaces/IMuonSystemExtensionTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "xAODTracking/TrackParticleContainer.h"

class MuonCombinedInDetCandidateAlg : public AthReentrantAlgorithm {
  public:
    MuonCombinedInDetCandidateAlg(const std::string& name, ISvcLocator* pSvcLocator);
    ~MuonCombinedInDetCandidateAlg() = default;

    StatusCode initialize();
    StatusCode execute(const EventContext& ctx) const;

  private:
    bool m_doSiliconForwardMuons;

    float m_extThreshold;

    SG::ReadHandleKeyArray<xAOD::TrackParticleContainer> m_indetTrackParticleLocation;
    SG::ReadHandleKey<xAOD::TrackParticleContainer>      m_indetForwardTrackParticleLocation;
    SG::WriteHandleKey<InDetCandidateCollection>         m_candidateCollectionName;
    ToolHandle<Trk::ITrackSelectorTool>                  m_trackSelector{
        this, "TrackSelector", "InDet::InDetDetailedTrackSelectorTool/MuonCombinedInDetDetailedTrackSelectorTool",
        "Track selector tool"};
    ToolHandle<Trk::ITrackSelectorTool> m_forwardTrackSelector{
        this, "InDetForwardTrackSelector",
        "InDet::InDetDetailedTrackSelectorTool/MuonCombinedInDetDetailedForwardTrackSelectorTool",
        "Forward track selector tool"};
    ToolHandle<Muon::IMuonSystemExtensionTool> m_muonSystemExtensionTool{
        this, "MuonSystemExtensionTool", "Muon::MuonSystemExtensionTool/MuonSystemExtensionTool",
        "Muon system extension tool"};

    void       create(const ToolHandle<Trk::ITrackSelectorTool>& currentTrackSelector,
                      const xAOD::TrackParticleContainer& indetTrackParticles, InDetCandidateCollection& outputContainer,
                      bool flagCandidateAsSiAssociated = false) const;
    StatusCode create(const EventContext& ctx, const ToolHandle<Trk::ITrackSelectorTool>& currentTrackSelector,
                      const SG::ReadHandleKey<xAOD::TrackParticleContainer>& location,
                      std::unique_ptr<InDetCandidateCollection>&             collection,
                      bool                                                   flagCandidateAsSiAssociate = false) const;

    bool isValidTrackParticle(const ToolHandle<Trk::ITrackSelectorTool>& currentTrackSelector,
                              const xAOD::TrackParticle* const           tp) const;
    void printTrackParticleInfo(const xAOD::TrackParticle* const tp, const std::string& what) const;

    int getCount(const xAOD::TrackParticle& tp, xAOD::SummaryType type) const;
};


#endif
