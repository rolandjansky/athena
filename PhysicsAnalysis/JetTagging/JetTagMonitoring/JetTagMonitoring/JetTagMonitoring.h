/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// *******************************************************
//
// NAME:     JetTagMonitoring.h
// PACKAGE:  PhysicsAnalysis/JetTagging/JetTagMonitoring
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro@ge.infn.it
//
// *******************************************************

#ifndef JETTAGMONITORING_H
#define JETTAGMONITORING_H

#include <vector>
#include <string>
#include <bitset>

#include <map>
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "ITrackToVertex/ITrackToVertex.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"

#include "xAODJet/Jet.h"

class TH1F_LW;
class TH2F_LW;
class StoreGateSvc;
class RunSummary;
class LWHist;

namespace Trk {
  class VxCandidate;
}
namespace Analysis {
  class TrackSelector;
}

/**
 * @brief Class for monitoring variables related to offline flavour tagging.
 *
 * @author Andrea Coccaro <Andrea.Coccaro@ge.infn.it>
 *
 * This class answers monitoring needs related to offline flavour tagging:
 *
 * - btagging variables;
 * - jet/tracks related variables;
 * - muon/electron related variables;
 * - global variables of interest for flavour tagging.
 */


class JetTagMonitoring : public ManagedMonitorToolBase {

  friend class RunSummary;

public:

    JetTagMonitoring(const std::string & type, const std::string & name, const IInterface* parent);

    ~JetTagMonitoring();

    virtual StatusCode initialize();

    virtual StatusCode bookHistograms();
    virtual StatusCode fillHistograms();
    virtual StatusCode procHistograms( );


private:

    enum Jet_t { goodJet, badJet, suspectJet };

    virtual StatusCode registerHist (MonGroup& theGroup, TH1* h1);
    virtual StatusCode registerHist (MonGroup& theGroup, LWHist* h1);

    bool applyKinematicCuts(const xAOD::Jet *jet);
    Jet_t applyQualityCuts(const xAOD::Jet *jet);

    void fillJetHistograms();
    void fillGoodJetHistos(const xAOD::Jet *jet);
    void fillSuspectJetHistos(const xAOD::Jet *jet);
    void fillDetailedHistograms(const xAOD::Jet *jet);
    //void fillElectronHistograms(const xAOD::Jet *jet);
    //void fillMuonHistograms(const xAOD::Jet *jet);
    void fillTrackInJetHistograms(const xAOD::Jet *jet);
    void fillBadTrackBits(const std::bitset<17> failedCuts, double eta, double phi);
    //void fillBadZone(int zone, double w);
    bool isGoodJet(const xAOD::Jet *jet);

    ServiceHandle<StoreGateSvc> m_storeGate;


    ToolHandle< Analysis::TrackSelector > m_trackSelectorTool;
    ToolHandle<Reco::ITrackToVertex> m_trackToVertexTool;
    /* ToolHandle<InDet::IInDetTestBLayerTool> m_blayerTool; */
    bool m_histogramsCreated;

    /* const Trk::VxCandidate* m_priVtx; */
    const xAOD::Vertex* m_priVtx;

    /** @brief String to retrieve JetContainer from StoreGate. */
    std::string m_jetName;
    /** @brief String to retrieve TrackParticleContainer from StoreGate. */
    std::string m_trackParticleName;
    /** @brief String to retrieve PrimaryVertexContainer from StoreGate. */
    std::string m_primaryVertexName;
    /** @brief DQ cuts switcher. */
    bool m_do_cuts;
    double m_trk_d0_min_cut;
    double m_trk_d0_max_cut;
    unsigned int m_pri_vtx_trk_min_cut;
    double m_jet_pt_cut;
    double m_jet_eta_cut;
    unsigned int m_trk_n;
    double m_sv1ip3d_weight_cut;
    double m_mv2c20_50_weight_cut;
    double m_mv2c20_70_weight_cut;
    double m_mv2c20_80_weight_cut;    

    /** @brief Master kill if no tools found. */
    bool m_switch_off;

    /** @brief Use Analysis::TrackSelector. */
    bool m_use_trackselector;

    /** @brief Use Analysis::TrigDecisionTool. */
    bool m_use_trigdectool;


    /** @brief To monitor number of tracks used to evaluate IP2D weight. */
    TH1F_LW* m_tag_ip2d_n;
    /** @brief To monitor jet likelihood value if coming from b decay for IP2D tagger. */
    TH1F_LW* m_tag_ip2d_b;
    /** @brief To monitor jet likelihood value if coming from u decay for IP2D tagger. */
    TH1F_LW* m_tag_ip2d_u;
    /** @brief To monitor jet likelihood value if coming from c decay for IP2D tagger. */
    TH1F_LW* m_tag_ip2d_c;
     /** @brief To monitor jet log likelihood ratio for IP2D tagger. */
    TH1F_LW* m_tag_ip2d_llr;   
    /** @brief To monitor number of tracks used to evaluate IP3D weight. */   
    TH1F_LW* m_tag_ip3d_n;
    /** @brief To monitor jet likelihood value if coming from b decay for IP3D tagger. */
    TH1F_LW* m_tag_ip3d_b;
    /** @brief To monitor jet likelihood value if coming from u decay for IP3D tagger. */
    TH1F_LW* m_tag_ip3d_u;
    /** @brief To monitor jet likelihood value if coming from c decay for IP3D tagger. */
    TH1F_LW* m_tag_ip3d_c;   
     /** @brief To monitor jet log likelihood ratio for IP3D tagger. */
    TH1F_LW* m_tag_ip3d_llr;     
    /** @brief To monitor jet 3D significance of SV0 tagger. */
    TH1F_LW* m_tag_sv0_sig3d;
    /** @brief To monitor jet likelihood value if coming from b decay for SV1 tagger. */
    TH1F_LW* m_tag_sv1_b;
    /** @brief To monitor jet likelihood value if coming from u decay for SV1 tagger. */
    TH1F_LW* m_tag_sv1_u;
     /** @brief To monitor jet likelihood value if coming from c decay for SV1 tagger. */
    TH1F_LW* m_tag_sv1_c;    
      /** @brief To monitor jet log likelihood ratio for SV1 tagger. */
    TH1F_LW* m_tag_sv1_llr;    
    /** @brief To monitor log likelihood ratio for JetFitter. */
    TH1F_LW* m_tag_jetfitter_llr;    
    /** @brief To monitor log likelihood ratio for combined tagger JetFitterCOMBNN. */
    TH1F_LW* m_tag_jfcnn_llr;
    
    /** @brief To monitor likelihood weight based on combined tagger (IP3D+SV1). */
    TH1F_LW* m_tag_sv1ip3d_w;
    TH1F_LW* m_tag_sv1ip3d_w_sj;
   
    /** @brief To monitor likelihood weight based on MV2c20 tagger. */
    TH1F_LW* m_tag_mv2c20_w;
    TH1F_LW* m_tag_mv2c20_w_sj;
    /** @brief To monitor number of Jets. */
    TH1F_LW* m_jet_n;
    /** @brief To monitor number of TrackParticles. */
    TH1F_LW* m_trackParticle_n;

    /** @brief To monitor number of primary vertices. */
    TH1F_LW* m_global_nPrimVtx;
    /** @brief To monitor x of primary vertex. */
    TH1F_LW* m_global_xPrimVtx;
    /** @brief To monitor y of primary vertex. */
    TH1F_LW* m_global_yPrimVtx;
    /** @brief To monitor z of primary vertex. */
    TH1F_LW* m_global_zPrimVtx;

    /** @brief To monitor number of b layer hits in TrackParticle. */
    TH1F_LW* m_global_BLayerHits;
    /** @brief To monitor number of pixel hits in TrackParticle. */
    TH1F_LW* m_global_PixelHits;
    /** @brief To monitor number of SCT hits in TrackParticle. */
    TH1F_LW* m_global_SCTHits;
    /** @brief To monitor number of pixel+SCT hits in TrackParticle. */
    TH1F_LW* m_global_SiHits;
    /** @brief To monitor number of TRT hits in TrackParticle. */
    TH1F_LW* m_global_TRTHits;

    /** @brief To monitor eta of the reconstructed jet. */
    TH1F_LW* m_jet_eta;
    /** @brief To monitor phi of the reconstructed jet. */
    TH1F_LW* m_jet_phi;
    /** @brief To monitor Et of the reconstructed jet. */
    TH1F_LW* m_jet_et;

    /** @brief To monitor number of tracks in a jet. */
    TH1F_LW* m_jet_tracks_n;
    /** @brief To monitor Pt of tracks in a jet. */
    TH1F_LW* m_jet_tracks_pt;
    /** @brief To monitor eta of tracks in a jet. */
    TH1F_LW* m_jet_tracks_eta;
    /** @brief To monitor phi of tracks in a jet. */
    TH1F_LW* m_jet_tracks_phi;
    /** @brief To monitor number of b layer hits in a jet. */
    TH1F_LW* m_jet_tracks_BLayerHits;
    /** @brief To monitor number of pixel hits in a jet. */
    TH1F_LW* m_jet_tracks_PixelHits;
    /** @brief To monitor number of SCT hits in a jet. */
    TH1F_LW* m_jet_tracks_SCTHits;
    /** @brief To monitor number of muons in a jet. */
    TH1F_LW* m_jet_muons_n;
    /** @brief To monitor Pt of muons in a jet. */
    TH1F_LW* m_jet_muons_pt;

    //* electron variables per jet *//
    /** @brief To monitor number of electrons in a jet. */
    TH1F_LW* m_jet_electrons_n;
    /** @brief To monitor Pt of electrons in a jet. */
    TH1F_LW* m_jet_electrons_pt;

    TH1F_LW* m_trigPassed;
    TH1F_LW* m_cutflow;

    TH1F_LW* m_cutflow_jet;

    TH1F_LW* m_priVtx_trks;

    /** @brief 2D map of track selector efficiency. */
    TH2F_LW* m_track_selector_eff;
    TH2F_LW* m_track_selector_eff_LS;
    TH2F_LW* m_track_selector_suspect;
    TH2F_LW* m_track_selector_all;
    TH2F_LW* m_track_selector_all_LS;

    /** @brief 2D map of jets at various cuts. */
    TH2F_LW* m_jet_2D_all;
    TH2F_LW* m_jet_2D_good;
    TH2F_LW* m_jet_2D_kinematic;
    TH2F_LW* m_jet_2D_kinematic_LS;
    TH2F_LW* m_jet_2D_quality;
    TH2F_LW* m_jet_2D_suspect;

    /** @brief 2D map of tag rates. */
    TH2F_LW* m_sv1ip3d_tag_pos_rate_2D;
    TH2F_LW* m_sv1ip3d_tag_neg_rate_2D;

    TH2F_LW* m_mv2c20_tag_50_2D;
    TH2F_LW* m_mv2c20_tag_70_2D;
    TH2F_LW* m_mv2c20_tag_80_2D;   
    TH2F_LW* m_mv2c20_tag_80_2D_LS;    

    enum Cuts_t { pTMin, d0Max, z0Max, sigd0Max, sigz0Max, etaMax,
                  nHitBLayer, deadBLayer, nHitPix, nHitSct, nHitSi, nHitTrt, nHitTrtHighE,
                  fitChi2, fitProb, fitChi2OnNdfMax, numCuts
                };


    /** @brief 2D map of TrackSelectorTool cuts */
    TH2F_LW* m_tracks_all_2D;
    TH2F_LW* m_tracks_pTMin_2D;
    TH2F_LW* m_tracks_d0Max_2D;
    TH2F_LW* m_tracks_z0Max_2D;
    TH2F_LW* m_tracks_sigd0Max_2D;
    TH2F_LW* m_tracks_sigz0Max_2D;
    TH2F_LW* m_tracks_etaMax_2D;
    TH2F_LW* m_tracks_nHitBLayer_2D;
    TH2F_LW* m_tracks_deadBLayer_2D;
    TH2F_LW* m_tracks_nHitPix_2D;
    TH2F_LW* m_tracks_nHitSct_2D;
    TH2F_LW* m_tracks_nHitSi_2D;
    TH2F_LW* m_tracks_nHitTrt_2D;
    TH2F_LW* m_tracks_nHitTrtHighE_2D;
    TH2F_LW* m_tracks_fitChi2_2D;
    TH2F_LW* m_tracks_fitProb_2D;
    TH2F_LW* m_tracks_fitChi2OnNdfMax_2D;

    TH2F_LW* m_tracks_all_2D_LS;
    TH2F_LW* m_tracks_pTMin_2D_LS;
    TH2F_LW* m_tracks_d0Max_2D_LS;
    TH2F_LW* m_tracks_z0Max_2D_LS;
    TH2F_LW* m_tracks_sigd0Max_2D_LS;
    TH2F_LW* m_tracks_sigz0Max_2D_LS;
    TH2F_LW* m_tracks_etaMax_2D_LS;
    TH2F_LW* m_tracks_nHitBLayer_2D_LS;
    TH2F_LW* m_tracks_deadBLayer_2D_LS;
    TH2F_LW* m_tracks_nHitPix_2D_LS;
    TH2F_LW* m_tracks_nHitSct_2D_LS;
    TH2F_LW* m_tracks_nHitSi_2D_LS;
    TH2F_LW* m_tracks_nHitTrt_2D_LS;
    TH2F_LW* m_tracks_nHitTrtHighE_2D_LS;
    TH2F_LW* m_tracks_fitChi2_2D_LS;
    TH2F_LW* m_tracks_fitProb_2D_LS;
    TH2F_LW* m_tracks_fitChi2OnNdfMax_2D_LS;

    TH1F_LW* m_efficiency;

    unsigned int m_lumiBlockNum;

    MonGroup* m_monGr_shift;
    MonGroup* m_monGr_LowStat;

    //--------------Pixel stuff--------------------------------------
    ServiceHandle <IInDetConditionsSvc> m_pixelCondSummarySvc;

    bool m_doModules;
    bool m_doOffline;
    bool m_isNewLumiBlock;
    bool m_doLumiBlock;
    bool m_majorityDisabled;
    bool m_doStatus;

};

#endif

