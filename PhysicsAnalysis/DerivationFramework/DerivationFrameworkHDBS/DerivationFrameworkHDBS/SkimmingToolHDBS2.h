/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SkimmingToolHDBS2.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_SKIMMINGTOOLHDBS2_H
#define DERIVATIONFRAMEWORK_SKIMMINGTOOLHDBS2_H 1

#include<string>
#include<vector>
#include<map>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/ReadHandleKey.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"

// xAOD header files
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "TrigDecisionTool/TrigDecisionTool.h"

//Vertexing
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"

namespace DerivationFramework {

  /** @class SkimmingToolHDBS2
      @author Susumu.Oda@cern.ch
  */
  class SkimmingToolHDBS2 : public AthAlgTool, public IAugmentationTool, public ISkimmingTool {

  public:
    /** Constructor with parameters */
    SkimmingToolHDBS2( const std::string& t, const std::string& n, const IInterface* p );

    /** Destructor */
    ~SkimmingToolHDBS2();

    // Athena algtool's Hooks
    virtual StatusCode  initialize() final;
    virtual StatusCode  finalize() final;

    /** Check that the current event passes this filter */
    virtual bool eventPassesFilter() const final;

    /** Add new branches as augmentation */
    virtual StatusCode addBranches() const final;

  private:
    enum {
      NUMBER_OF_MERGED_JET_TYPES = 3
    };

    ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;
    //vertexing stuff
    ToolHandle < Trk::IVertexFitter > m_iVertexFitter;
    Trk::TrkVKalVrtFitter* m_VKVFitter;

    mutable unsigned int m_ntot;
    mutable unsigned int m_npass;

    bool m_skipTriggerRequirement;
    bool m_doVertexing;
    bool m_doVertexing_el_withOrigInDetTrack;
    bool m_doVertexing_mu_withInDetForwTrack;

    mutable std::vector<const xAOD::Electron*> m_goodElectrons;
    mutable std::vector<const xAOD::Muon*> m_goodMuons;
    mutable std::vector<const xAOD::Jet*> m_goodJets;
    mutable std::vector<const xAOD::Jet*> m_goodMergedJets[NUMBER_OF_MERGED_JET_TYPES];
    mutable std::vector<const xAOD::Photon*> m_goodPhotons;
    mutable std::vector<const xAOD::TrackParticle*> m_goodTracks;
    mutable std::vector<const xAOD::TrackParticle*> m_goodTracksLoose;
    mutable std::vector<const xAOD::Photon*> m_goodPhotonsForW;
    mutable std::vector<const xAOD::TrackParticle*> m_goodTracksForW;

    std::string m_filterType;

    SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey;

    SG::ReadHandleKey<xAOD::ElectronContainer> m_electronSGKey;
    SG::ReadHandleKey<xAOD::MuonContainer> m_muonSGKey;
    SG::ReadHandleKey<xAOD::JetContainer> m_jetSGKey;
    SG::ReadHandleKey<xAOD::JetContainer> m_mergedJetSGKey[NUMBER_OF_MERGED_JET_TYPES];
    SG::ReadHandleKey<xAOD::PhotonContainer> m_photonSGKey;
    SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trackSGKey;

    std::string m_vxContainerName;

    unsigned int m_nLeptons;
    unsigned int m_nElectrons;
    unsigned int m_nMuons;
    unsigned int m_nJets;
    unsigned int m_nMergedJets[NUMBER_OF_MERGED_JET_TYPES];
    unsigned int m_nPhotons;
    unsigned int m_nTracks;

    std::string m_electronQual;
    std::string m_muonQual;
    std::string m_tightElectronQual;
    std::string m_tightMuonQual;
    std::string m_vertexingElectronQual;
    std::string m_vertexingMuonQual;
    std::string m_jetQual;
    std::string m_mergedJetQual[NUMBER_OF_MERGED_JET_TYPES];
    std::string m_photonQual;

    std::string m_primaryElectronQual4L;

    std::vector<std::string> m_trigger2L;
    std::vector<std::string> m_trigger4L;
    std::vector<std::string> m_triggerTP;
    std::vector<std::string> m_trigger2L2Q;
    std::vector<std::string> m_triggerJPSI;
    std::vector<std::string> m_triggerPHI;
    std::vector<std::string> m_triggerRHO;
    std::vector<std::string> m_triggerWPIGAMMA;

    double m_electronEtCut;
    double m_muonPtCut;
    double m_tightElectronEtCut;
    double m_tightMuonPtCut;
    double m_vertexingElectronEtCut;
    double m_vertexingMuonPtCut;
    double m_jetPtCut;
    double m_mergedJetPtCut[NUMBER_OF_MERGED_JET_TYPES];
    double m_photonPtCut;
    double m_trackPtCut;
    double m_trackPtCutLoose;
    double m_diTrackPtCut;
    double m_trackPtForWCut;
    double m_photonPtForWCut;

    double m_electronEtaCut;
    double m_muonEtaCut;
    double m_caloMuonEtaCut;
    double m_jetEtaCut;
    double m_mergedJetEtaCut[NUMBER_OF_MERGED_JET_TYPES];
    double m_photonEtaCut;

    double m_invariantMassCut;
    double m_invariantMassJpsiLowCut;
    double m_invariantMassJpsiUpCut;
    double m_invariantMassUpsilonLowCut;
    double m_invariantMassUpsilonUpCut;
    double m_invariantMassPhiLowCut;
    double m_invariantMassPhiUpCut;
    double m_invariantMassRhoLowCut;
    double m_invariantMassRhoUpCut;
    double m_invariantMassKshortLowCut;
    double m_invariantMassKshortUpCut;
    double m_invariantMassKstarLowCut;
    double m_invariantMassKstarUpCut;
    double m_invariantMassD0LowCut;
    double m_invariantMassD0UpCut;
    double m_invariantMassWLowCut;
    double m_leadingElectronEtCut;
    double m_leadingMuonPtCut;
    double m_diMuonPtCut;

    double m_dRElectronJetCut;

    bool m_checkEventError;
    bool m_defaultElectronFourMomentum;

    bool m_DFCommonJetFourMomentum;

    bool m_requireTightLeptons;

    void fillElectrons() const;
    void fillMuons() const;
    void fillJets() const;
    void fillMergedJets() const;
    void fillPhotons() const;
    void fillTracks() const;

    bool checkElectronQuality(const xAOD::Electron *el, const bool isTight=false, const bool isVertexing=false) const;
    bool checkMuonQuality(const xAOD::Muon *mu, const bool isTight=false, const bool isVertexing=false) const;
    bool checkJetQuality(const xAOD::Jet *jet) const;
    bool checkMergedJetQuality(const xAOD::Jet *jet, const unsigned int type) const;
    bool checkPhotonQuality(const xAOD::Photon *ph) const;
    bool checkPhotonQualityForW(const xAOD::Photon *ph) const;
    bool checkTrackQuality(const xAOD::TrackParticle *trk) const;
    bool checkTrackQualityLoose(const xAOD::TrackParticle *trk) const;
    bool checkTrackQualityForW(const xAOD::TrackParticle *trk) const;

    bool check2L(const bool onlyVertexing=false) const;
    bool check4L(const bool onlyVertexing=false) const;
    bool checkTP() const;
    bool check2L2Q() const;
    bool checkJPSI() const;
    bool checkPHI() const;
    bool checkRHO() const;
    bool checkWPIGAMMA() const;

    bool formQuads(std::vector<const xAOD::IParticle*> *leptons, bool doAllCombinations=false, std::vector<std::vector<const xAOD::IParticle*> *>* quads=0) const;
    void doVertexing(std::vector< std::vector<const xAOD::IParticle*> *> *quads) const;

    TLorentzVector electronFourMomentum(const xAOD::Electron *el) const;
    TLorentzVector muonFourMomentum(const xAOD::Muon *mu) const;
    TLorentzVector jetFourMomentum(const xAOD::Jet *jet) const;

    double getDeltaR(const double eta1, const double phi1, const double eta2, const double phi2) const;

    static const double s_MZ;
    static const double s_MKplus;
    static const double s_MPiplus;
  };

}

#endif // DERIVATIONFRAMEWORK_SKIMMINGTOOLHDBS2_H
