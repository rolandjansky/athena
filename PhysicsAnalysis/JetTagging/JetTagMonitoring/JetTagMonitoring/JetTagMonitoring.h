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
#include "TrigDecisionTool/TrigDecisionTool.h" // added by SARA

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

    void fillJetHistograms();
    void fillGoodJetHistos(const xAOD::Jet *jet);
    void fillSuspectJetHistos(const xAOD::Jet *jet);
    void fillBadJetHistos(const xAOD::Jet *jet);
    void fillDetailedHistograms(const xAOD::Jet *jet, Jet_t taggabilityLabel);
    void fillTrackInJetHistograms(const xAOD::Jet *jet);
    void fillBtagHistograms(const xAOD::Jet *jet, bool fill_top_histos); // added by SARA;
    void fillBadTrackBits(const std::bitset<17> failedCuts, double eta, double phi);
    void fillEffHist(TH1 * h_passed, TH1 * h_all, TH1F_LW * effHist);
    //void fillBadZone(int zone, double w);
    bool passJetQualityCuts(const xAOD::Jet *jet);
    bool passKinematicCuts(const xAOD::Jet *jet);
    Jet_t getTaggabilityLabel(const xAOD::Jet *jet);
    bool isTopEvent(); // added by SARA

    ServiceHandle<StoreGateSvc> m_storeGate;


    ToolHandle< Analysis::TrackSelector > m_trackSelectorTool;
    ToolHandle<Reco::ITrackToVertex> m_trackToVertexTool;
    ToolHandle< Trig::TrigDecisionTool > m_trigDecTool; // added by SARA
    /* ToolHandle<InDet::IInDetTestBLayerTool> m_blayerTool; */
    bool m_histogramsCreated;

    /* const Trk::VxCandidate* m_priVtx; */
    const xAOD::Vertex* m_priVtx = nullptr;

    /** @brief String to retrieve JetContainer from StoreGate. */
    std::string m_jetName;
    /** @brief String to retrieve TrackParticleContainer from StoreGate. */
    std::string m_trackParticleName;
    /** @brief String to retrieve PrimaryVertexContainer from StoreGate. */
    std::string m_primaryVertexName;
    /** @brief String to retrieve ElectronContainer from StoreGate. */
    std::string m_electronName; // added by SARA
    /** @brief String to retrieve MuonContainer from StoreGate. */
    std::string m_muonName; // added by SARA
    /** @brief DQ cuts switcher. */
    bool m_do_cuts;
    double m_trk_d0_min_cut;
    double m_trk_d0_max_cut;
    unsigned int m_pri_vtx_trk_min_cut;
    double m_jet_pt_cut;
    double m_jet_eta_cut;
    unsigned int m_trk_n;
    double m_sv1ip3d_weight_cut;
    double m_mv_60_weight_cut;
    double m_mv_70_weight_cut;
    double m_mv_77_weight_cut;
    double m_mv_85_weight_cut;
    std::string m_mv_algorithmName;
    double m_MuonPtCut; // added by SARA
    double m_MuonEtaCut; // added by SARA
    double m_ElectronPtCut; // added by SARA
    double m_ElectronEtaCut; // added by SARA
    double m_ElectronEtaCrackLowCut; // added by SARA
    double m_ElectronEtaCrackHighCut; // added by SARA
    double m_ElectronTopoEtCone20Cut; // added by SARA
    double m_ElectronPtVarCone20Cut; // added by SARA
    double m_MuonTopoEtCone20Cut; // added by SARA
    double m_MuonPtVarCone20Cut; // added by SARA
    std::string m_ElectronTrigger_2016; // added by SARA
    std::string m_MuonTrigger_2016; // added by SARA
    std::string m_JetTrigger_2016; // added by SARA
    std::string m_ElectronTrigger_2017; // added by SARA
    std::string m_MuonTrigger_2017; // added by SARA
    std::string m_JetTrigger_2017; // added by SARA

    /** @brief Master kill if no tools found. */
    bool m_switch_off;

    /** @brief Use Analysis::TrackSelector. */
    bool m_use_trackselector;

    /** @brief Use Analysis::TrigDecisionTool. */
    bool m_use_trigdectool;
    
    /** @brief To monitor number of tracks used to evaluate IP2D weight. */
    TH1F_LW* m_tag_ip2d_n = nullptr;
    TH1F_LW* m_tag_ip2d_n_sj = nullptr;
    /** @brief To monitor jet likelihood value if coming from b decay for IP2D tagger. */
    TH1F_LW* m_tag_ip2d_b = nullptr;
    TH1F_LW* m_tag_ip2d_b_sj = nullptr;
    /** @brief To monitor jet likelihood value if coming from u decay for IP2D tagger. */
    TH1F_LW* m_tag_ip2d_u = nullptr;
    TH1F_LW* m_tag_ip2d_u_sj = nullptr;
    /** @brief To monitor jet likelihood value if coming from c decay for IP2D tagger. */
    TH1F_LW* m_tag_ip2d_c = nullptr;
    TH1F_LW* m_tag_ip2d_c_sj = nullptr;
     /** @brief To monitor jet log likelihood ratio for IP2D tagger. */
    TH1F_LW* m_tag_ip2d_llr = nullptr;   
    TH1F_LW* m_tag_ip2d_llr_sj = nullptr;   
    /** @brief To monitor number of tracks used to evaluate IP3D weight. */   
    TH1F_LW* m_tag_ip3d_n = nullptr;
    TH1F_LW* m_tag_ip3d_n_sj = nullptr;
    /** @brief To monitor jet likelihood value if coming from b decay for IP3D tagger. */
    TH1F_LW* m_tag_ip3d_b = nullptr;
    TH1F_LW* m_tag_ip3d_b_sj = nullptr;
    /** @brief To monitor jet likelihood value if coming from u decay for IP3D tagger. */
    TH1F_LW* m_tag_ip3d_u = nullptr;
    TH1F_LW* m_tag_ip3d_u_sj = nullptr;
    /** @brief To monitor jet likelihood value if coming from c decay for IP3D tagger. */
    TH1F_LW* m_tag_ip3d_c = nullptr;   
    TH1F_LW* m_tag_ip3d_c_sj = nullptr;   
     /** @brief To monitor jet log likelihood ratio for IP3D tagger. */
    TH1F_LW* m_tag_ip3d_llr = nullptr;     
    TH1F_LW* m_tag_ip3d_llr_sj = nullptr;     
    /** @brief To monitor jet 3D significance of SV0 tagger. */
    TH1F_LW* m_tag_sv0_sig3d = nullptr;
    /** @brief To monitor jet likelihood value if coming from b decay for SV1 tagger. */
    TH1F_LW* m_tag_sv1_b = nullptr;
    TH1F_LW* m_tag_sv1_b_sj = nullptr;
    /** @brief To monitor jet likelihood value if coming from u decay for SV1 tagger. */
    TH1F_LW* m_tag_sv1_u = nullptr;
    TH1F_LW* m_tag_sv1_u_sj = nullptr;
     /** @brief To monitor jet likelihood value if coming from c decay for SV1 tagger. */
    TH1F_LW* m_tag_sv1_c = nullptr;    
    TH1F_LW* m_tag_sv1_c_sj = nullptr;    
      /** @brief To monitor jet log likelihood ratio for SV1 tagger. */
    TH1F_LW* m_tag_sv1_llr = nullptr;    
    TH1F_LW* m_tag_sv1_llr_sj = nullptr;    
    /** @brief To monitor log likelihood ratio for JetFitter. */
    TH1F_LW* m_tag_jetfitter_llr = nullptr;    
    TH1F_LW* m_tag_jetfitter_llr_sj = nullptr;    
    /** @brief To monitor log likelihood ratio for combined tagger JetFitterCOMBNN. */
    TH1F_LW* m_tag_jfcnn_llr = nullptr;
    TH1F_LW* m_tag_jfcnn_llr_sj = nullptr;
    
    /** @brief To monitor likelihood weight based on combined tagger (IP3D+SV1). */
    TH1F_LW* m_tag_sv1ip3d_w = nullptr;
    TH1F_LW* m_tag_sv1ip3d_w_sj = nullptr;
   
    /** @brief To monitor likelihood weight based on Mv tagger. */
    TH1F_LW* m_tag_mv_w = nullptr;
    TH1F_LW* m_tag_mv_w_pT10_20 = nullptr;
    TH1F_LW* m_tag_mv_w_pT20_50 = nullptr;
    TH1F_LW* m_tag_mv_w_pT50_100 = nullptr;
    TH1F_LW* m_tag_mv_w_pT100_200 = nullptr;
    TH1F_LW* m_tag_mv_w_pT200 = nullptr;
    TH1F_LW* m_tag_mv_w_LS = nullptr;
    TH1F_LW* m_tag_mv_w_pT10_20_LS = nullptr;
    TH1F_LW* m_tag_mv_w_pT20_50_LS = nullptr;
    TH1F_LW* m_tag_mv_w_pT50_100_LS = nullptr;
    TH1F_LW* m_tag_mv_w_pT100_200_LS = nullptr;
    TH1F_LW* m_tag_mv_w_pT200_LS = nullptr;

    TH1F_LW* m_tag_mv_w_eta0_05 = nullptr;
    TH1F_LW* m_tag_mv_w_eta05_10 = nullptr;
    TH1F_LW* m_tag_mv_w_eta10_15 = nullptr;
    TH1F_LW* m_tag_mv_w_eta15_20 = nullptr;
    TH1F_LW* m_tag_mv_w_eta20_25 = nullptr;
    TH1F_LW* m_tag_mv_w_phi0_07 = nullptr;
    TH1F_LW* m_tag_mv_w_phi07_14 = nullptr;
    TH1F_LW* m_tag_mv_w_phi14_21 = nullptr;
    TH1F_LW* m_tag_mv_w_phi21_28 = nullptr;
    TH1F_LW* m_tag_mv_w_phi28 = nullptr;
    TH1F_LW* m_tag_mv_w_phi_sum85OP = nullptr;
    TH1F_LW* m_tag_mv_w_phi_sum77OP = nullptr;
    TH1F_LW* m_tag_mv_w_phi_sum70OP = nullptr;
    TH1F_LW* m_tag_mv_w_phi_sum60OP = nullptr;
    TH1F_LW* m_tag_mv_w_phi_sumAll = nullptr;
    TH1F_LW* m_tag_mv_w_phi_frac85OP = nullptr;
    TH1F_LW* m_tag_mv_w_phi_frac77OP = nullptr;
    TH1F_LW* m_tag_mv_w_phi_frac70OP = nullptr;
    TH1F_LW* m_tag_mv_w_phi_frac60OP = nullptr;
    TH1F_LW* m_tag_mv_w_eta_sum85OP = nullptr;
    TH1F_LW* m_tag_mv_w_eta_sum77OP = nullptr;
    TH1F_LW* m_tag_mv_w_eta_sum70OP = nullptr;
    TH1F_LW* m_tag_mv_w_eta_sum60OP = nullptr;
    TH1F_LW* m_tag_mv_w_eta_sumAll = nullptr;
    TH1F_LW* m_tag_mv_w_eta_frac85OP = nullptr;
    TH1F_LW* m_tag_mv_w_eta_frac77OP = nullptr;
    TH1F_LW* m_tag_mv_w_eta_frac70OP = nullptr;
    TH1F_LW* m_tag_mv_w_eta_frac60OP = nullptr;
    TH1F_LW* m_tag_mv_w_sj = nullptr;
    TH1F_LW* m_tag_mv_w_sj_pT10_20 = nullptr;
    TH1F_LW* m_tag_mv_w_sj_pT20_50 = nullptr;
    TH1F_LW* m_tag_mv_w_sj_pT50_100 = nullptr;
    TH1F_LW* m_tag_mv_w_sj_pT100_200 = nullptr;
    TH1F_LW* m_tag_mv_w_sj_pT200 = nullptr;
    TH1F_LW* m_tag_mv_w_sj_eta0_05 = nullptr;
    TH1F_LW* m_tag_mv_w_sj_eta05_10 = nullptr;
    TH1F_LW* m_tag_mv_w_sj_eta10_15 = nullptr;
    TH1F_LW* m_tag_mv_w_sj_eta15_20 = nullptr;
    TH1F_LW* m_tag_mv_w_sj_eta20_25 = nullptr;
    TH1F_LW* m_tag_mv_w_sj_phi0_07 = nullptr;
    TH1F_LW* m_tag_mv_w_sj_phi07_14 = nullptr;
    TH1F_LW* m_tag_mv_w_sj_phi14_21 = nullptr;
    TH1F_LW* m_tag_mv_w_sj_phi21_28 = nullptr;
    TH1F_LW* m_tag_mv_w_sj_phi28 = nullptr;
    TH1F_LW* m_tag_mv_w_sj_phi_sum85OP = nullptr;
    TH1F_LW* m_tag_mv_w_sj_phi_sum77OP = nullptr;
    TH1F_LW* m_tag_mv_w_sj_phi_sum70OP = nullptr;
    TH1F_LW* m_tag_mv_w_sj_phi_sum60OP = nullptr;
    TH1F_LW* m_tag_mv_w_sj_phi_sumAll = nullptr;
    TH1F_LW* m_tag_mv_w_sj_phi_frac85OP = nullptr;
    TH1F_LW* m_tag_mv_w_sj_phi_frac77OP = nullptr;
    TH1F_LW* m_tag_mv_w_sj_phi_frac70OP = nullptr;
    TH1F_LW* m_tag_mv_w_sj_phi_frac60OP = nullptr;
    TH1F_LW* m_tag_mv_w_sj_eta_sum85OP = nullptr;
    TH1F_LW* m_tag_mv_w_sj_eta_sum77OP = nullptr;
    TH1F_LW* m_tag_mv_w_sj_eta_sum70OP = nullptr;
    TH1F_LW* m_tag_mv_w_sj_eta_sum60OP = nullptr;
    TH1F_LW* m_tag_mv_w_sj_eta_sumAll = nullptr;
    TH1F_LW* m_tag_mv_w_sj_eta_frac85OP = nullptr;
    TH1F_LW* m_tag_mv_w_sj_eta_frac77OP = nullptr;
    TH1F_LW* m_tag_mv_w_sj_eta_frac70OP = nullptr;
    TH1F_LW* m_tag_mv_w_sj_eta_frac60OP = nullptr;
    /** @brief To monitor number of Jets. */
    TH1F_LW* m_jet_n = nullptr;
    /** @brief To monitor number of TrackParticles. */
    TH1F_LW* m_trackParticle_n = nullptr;

    /** @brief To monitor number of primary vertices. */
    TH1F_LW* m_global_nPrimVtx = nullptr;
    /** @brief To monitor x of primary vertex. */
    TH1F_LW* m_global_xPrimVtx = nullptr;
    /** @brief To monitor y of primary vertex. */
    TH1F_LW* m_global_yPrimVtx = nullptr;
    /** @brief To monitor z of primary vertex. */
    TH1F_LW* m_global_zPrimVtx = nullptr;

      /** @brief To monitor number of b layer hits in TrackParticle. */
    TH1F_LW* m_global_BLayerHits = nullptr; // IBL hits in Run 2, old B-layer in Run-1
    /** @brief To monitor number of pixel hits in TrackParticle. */
    TH1F_LW* m_global_PixelHits = nullptr;
    /** @brief To monitor number of SCT hits in TrackParticle. */
    TH1F_LW* m_global_SCTHits = nullptr;
    /** @brief To monitor number of pixel+SCT hits in TrackParticle. */
    TH1F_LW* m_global_SiHits = nullptr;
    /** @brief To monitor number of TRT hits in TrackParticle. */
    TH1F_LW* m_global_TRTHits = nullptr;

    /** @brief To monitor eta of the reconstructed jet. */
    TH1F_LW* m_jet_eta = nullptr;
    /** @brief To monitor phi of the reconstructed jet. */
    TH1F_LW* m_jet_phi = nullptr;
    /** @brief To monitor Et of the reconstructed jet. */
    TH1F_LW* m_jet_et = nullptr;

    /** @brief To monitor number of tracks in a jet. */
    TH1F_LW* m_jet_tracks_n = nullptr;
    /** @brief To monitor Pt of tracks in a jet. */
    TH1F_LW* m_jet_tracks_pt = nullptr;
    /** @brief To monitor eta of tracks in a jet. */
    TH1F_LW* m_jet_tracks_eta = nullptr;
    /** @brief To monitor phi of tracks in a jet. */
    TH1F_LW* m_jet_tracks_phi = nullptr;
    /** @brief To monitor d0 of tracks in a jet. */
    TH1F_LW* m_jet_tracks_d0 = nullptr;
    TH1F_LW* m_jet_tracks_d0_LS = nullptr;
    /** @brief To monitor z0 of tracks in a jet. */
    TH1F_LW* m_jet_tracks_z0 = nullptr;
    TH1F_LW* m_jet_tracks_z0_LS = nullptr;
   /** @brief To monitor number of b layer hits in a jet. */
    TH1F_LW* m_jet_tracks_BLayerHits = nullptr; // IBL hits in Run-2, old b-layer in Run-1
    /** @brief To monitor number of pixel hits in a jet. */
    TH1F_LW* m_jet_tracks_PixelHits = nullptr;
    /** @brief To monitor number of SCT hits in a jet. */
    TH1F_LW* m_jet_tracks_SCTHits = nullptr;
    /** @brief To monitor number of muons in a jet. */
    TH1F_LW* m_jet_muons_n = nullptr;
    /** @brief To monitor Pt of muons in a jet. */
    TH1F_LW* m_jet_muons_pt = nullptr;

    //* electron variables per jet *//
    /** @brief To monitor number of electrons in a jet. */
    TH1F_LW* m_jet_electrons_n = nullptr;
    /** @brief To monitor Pt of electrons in a jet. */
    TH1F_LW* m_jet_electrons_pt = nullptr;

    TH1F_LW* m_n_iso_el = nullptr; // added by SARA
    TH1F_LW* m_n_iso_mu = nullptr; // added by SARA

    TH1F_LW* m_trigPassed = nullptr;
    TH1F_LW* m_cutflow = nullptr;

    TH1F_LW* m_cutflow_jet = nullptr;

    TH1F_LW* m_priVtx_trks = nullptr;

    /** @brief 2D map of track selector efficiency. */
    TH2F_LW* m_track_selector_eff = nullptr;
    TH2F_LW* m_track_selector_eff_LS = nullptr;
    TH2F_LW* m_track_selector_suspect = nullptr;
    TH2F_LW* m_track_selector_all = nullptr;
    TH2F_LW* m_track_selector_all_LS = nullptr;

    /** top histograms */
    TH1F_LW* m_jet_top = nullptr; // added by SARA
    TH1F_LW* m_jet_pt_top = nullptr; // added by SARA
    TH1F_LW* m_jet_mv_w_top = nullptr; // added by SARA
    
    TH1F_LW* m_jet_top_tagged = nullptr; // added by SARA
    TH1F_LW* m_jet_pt_top_tagged = nullptr; // added by SARA
    
    TH1F_LW* m_jet_top_eff = nullptr; // added by SARA
    TH1F_LW* m_jet_pt_top_eff = nullptr; // added by SARA
    
    /** @brief 2D map of jets at various cuts. */
    TH2F_LW* m_jet_2D_all = nullptr;
    TH2F_LW* m_jet_2D_good = nullptr;
    TH2F_LW* m_jet_2D_kinematic = nullptr;
    TH2F_LW* m_jet_2D_kinematic_LS = nullptr;
    TH2F_LW* m_jet_2D_quality = nullptr;
    TH2F_LW* m_jet_2D_suspect = nullptr;
    TH2F_LW* m_jet_2D_bad = nullptr;

    /** @brief 2D map of tag rates. */
    TH2F_LW* m_sv1ip3d_tag_pos_rate_2D = nullptr;
    TH2F_LW* m_sv1ip3d_tag_neg_rate_2D = nullptr;

    TH2F_LW* m_mv_tag_60_2D = nullptr;
    TH2F_LW* m_mv_tag_70_2D = nullptr;
    TH2F_LW* m_mv_tag_77_2D = nullptr;   
    TH2F_LW* m_mv_tag_77_2D_LS = nullptr;    

    TH1F_LW* m_tag_mv_w_eta_sum85OP_LS =nullptr;
    TH1F_LW* m_tag_mv_w_eta_sum77OP_LS =nullptr;
    TH1F_LW* m_tag_mv_w_eta_sum70OP_LS =nullptr;
    TH1F_LW* m_tag_mv_w_eta_sum60OP_LS =nullptr;
    TH1F_LW* m_tag_mv_w_eta_sumAll_LS =nullptr;

    enum Cuts_t { pTMin, d0Max, z0Max, sigd0Max, sigz0Max, etaMax,
                  nHitBLayer, deadBLayer, nHitPix, nHitSct, nHitSi, nHitTrt, nHitTrtHighE,
                  fitChi2, fitProb, fitChi2OnNdfMax, numCuts
                };


    /** @brief 2D map of TrackSelectorTool cuts */
    TH2F_LW* m_tracks_all_2D = nullptr;
    TH2F_LW* m_tracks_pTMin_2D = nullptr;
    TH2F_LW* m_tracks_d0Max_2D = nullptr;
    TH2F_LW* m_tracks_z0Max_2D = nullptr;
    TH2F_LW* m_tracks_sigd0Max_2D = nullptr;
    TH2F_LW* m_tracks_sigz0Max_2D = nullptr;
    TH2F_LW* m_tracks_etaMax_2D = nullptr;
    TH2F_LW* m_tracks_nHitBLayer_2D = nullptr;
    TH2F_LW* m_tracks_deadBLayer_2D = nullptr;
    TH2F_LW* m_tracks_nHitPix_2D = nullptr;
    TH2F_LW* m_tracks_nHitSct_2D = nullptr;
    TH2F_LW* m_tracks_nHitSi_2D = nullptr;
    TH2F_LW* m_tracks_nHitTrt_2D = nullptr;
    TH2F_LW* m_tracks_nHitTrtHighE_2D = nullptr;
    TH2F_LW* m_tracks_fitChi2_2D = nullptr;
    TH2F_LW* m_tracks_fitProb_2D = nullptr;
    TH2F_LW* m_tracks_fitChi2OnNdfMax_2D = nullptr;

    TH2F_LW* m_tracks_passedCuts_2D = nullptr;

    TH2F_LW* m_tracks_all_2D_LS = nullptr;
    TH2F_LW* m_tracks_pTMin_2D_LS = nullptr;
    TH2F_LW* m_tracks_d0Max_2D_LS = nullptr;
    TH2F_LW* m_tracks_z0Max_2D_LS = nullptr;
    TH2F_LW* m_tracks_sigd0Max_2D_LS = nullptr;
    TH2F_LW* m_tracks_sigz0Max_2D_LS = nullptr;
    TH2F_LW* m_tracks_etaMax_2D_LS = nullptr;
    TH2F_LW* m_tracks_nHitBLayer_2D_LS = nullptr;
    TH2F_LW* m_tracks_deadBLayer_2D_LS = nullptr;
    TH2F_LW* m_tracks_nHitPix_2D_LS = nullptr;
    TH2F_LW* m_tracks_nHitSct_2D_LS = nullptr;
    TH2F_LW* m_tracks_nHitSi_2D_LS = nullptr;
    TH2F_LW* m_tracks_nHitTrt_2D_LS = nullptr;
    TH2F_LW* m_tracks_nHitTrtHighE_2D_LS = nullptr;
    TH2F_LW* m_tracks_fitChi2_2D_LS = nullptr;
    TH2F_LW* m_tracks_fitProb_2D_LS = nullptr;
    TH2F_LW* m_tracks_fitChi2OnNdfMax_2D_LS = nullptr;

    TH1F_LW* m_efficiency = nullptr;

    unsigned int m_lumiBlockNum = 0;

    MonGroup* m_monGr_shift = nullptr;
    MonGroup* m_monGr_LowStat = nullptr;

    //--------------Pixel stuff--------------------------------------
    ServiceHandle <IInDetConditionsSvc> m_pixelCondSummarySvc;

  //bool m_doModules;
  //bool m_doOffline;
    bool m_isNewLumiBlock;
    bool m_doLumiBlock;
  //bool m_majorityDisabled;
    bool m_doStatus;

};

#endif

