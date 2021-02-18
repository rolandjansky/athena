/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_KINKTRKZMUMUTAGTOOL_H
#define DERIVATIONFRAMEWORK_KINKTRKZMUMUTAGTOOL_H 1

#include<string>
#include<vector>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigObjectMatching/TrigMatchTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "xAODMissingET/MissingETContainer.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

namespace DerivationFramework {

  /** @class KinkTrkZmumuTagTool
      @author shimpei.yamamoto@cern.ch
     */
  class KinkTrkZmumuTagTool : public AthAlgTool, public IAugmentationTool {
   
  public: 
    /** Constructor with parameters */
    KinkTrkZmumuTagTool( const std::string& t, const std::string& n, const IInterface* p );
   
    /** Destructor */
    ~KinkTrkZmumuTagTool();
   
    // Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();
 
    /** Check that the current event passes this filter */
    virtual StatusCode addBranches() const;

  private:
    bool passTrigger(const std::vector<std::string>& triggers) const;
    bool checkTagMuon(const xAOD::Muon *muon) const;
    bool checkMSTrack(const xAOD::TrackParticle *track) const;
    bool checkMuonTrackPair(const xAOD::Muon *muon, const xAOD::TrackParticle *track) const;
    bool passMuonQuality(const xAOD::Muon *muon) const;
    bool passMSTrackQuality(const xAOD::TrackParticle *track) const;
    bool passMuonTrigMatch(const xAOD::Muon *muon, const std::vector<std::string>& triggers) const;

    ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;
    ToolHandle<TrigMatchTool> m_trigMatchTool;
    std::vector<std::string> m_trigNames;
    float m_trigMatchDeltaR;
    bool m_doTrigMatch;

    ToolHandle<CP::IMuonSelectionTool> m_muonSelectionTool;
    SG::ReadHandleKey<xAOD::MuonContainer> m_muonSGKey { this, "MuonContainerKey", "Muons", ""};
    std::vector<std::string> m_muonIDKeys;
    float m_muonPtCut;
    float m_muonEtaMax;

    SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trackSGKey { this, "TrackContainerKey", "MuonSpectrometerTracks", ""};
    float m_trackPtCut;
    float m_trackEtaMax;

    float m_diMuonMassLow;
    float m_diMuonMassHigh;
    float m_dPhiMax;
    bool m_doOppositeSignReq;

    std::string m_sgKeyPrefix;


  }; 
 
}
#endif
