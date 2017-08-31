/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECVALIDATION_TCCPLOTS_H
#define TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECVALIDATION_TCCPLOTS_H

// std includes
#include <string>
#include <array>

// local includes
#include "TCCPlotsBase.h"
#include "xAODJet/JetContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODPFlow/TrackCaloClusterContainer.h"
#include "xAODTruth/TruthParticleContainer.h"

class TCCPlots: public TCCPlotsBase {
public:
  TCCPlots(TCCPlotsBase* pParent, const std::string& dirName, const std::string& collection = "");
  void fill(const xAOD::JetContainer& jets);
  void fill(const xAOD::Jet& jet);
  void fill(const xAOD::Jet& jet, const xAOD::Jet& truth);
  
  void fillLeading(const xAOD::Jet& jet);
  void fillSubLeading(const xAOD::Jet& jet);
  
  void fillResponse(const xAOD::Jet& jet, const xAOD::Jet& truth);
  void fillResponseLeading(const xAOD::Jet& jet, const xAOD::Jet& truth);
  void fillResponseSubLeading(const xAOD::Jet& jet, const xAOD::Jet& truth);
  void fillResponseNoPtNoMassCuts(const xAOD::Jet& jet, const xAOD::Jet& truth);
  void fillResponseNoPtNoMassCutsLeading(const xAOD::Jet& jet, const xAOD::Jet& truth);
  void fillResponseNoPtNoMassCutsSubLeading(const xAOD::Jet& jet, const xAOD::Jet& truth);
  
  void fillResponseNPV(const xAOD::Jet& jet, const xAOD::Jet& truth, int NPV);
  void fillResponseLeadingNPV(const xAOD::Jet& jet, const xAOD::Jet& truth, int NPV);
  void fillResponseSubLeadingNPV(const xAOD::Jet& jet, const xAOD::Jet& truth, int NPV);
    
  void fillPseudoResponse(const xAOD::Jet& jet, const xAOD::Jet& calo);
  void fillPseudoResponseLeading(const xAOD::Jet& jet, const xAOD::Jet& calo);
  void fillPseudoResponseSubLeading(const xAOD::Jet& jet, const xAOD::Jet& calo);
    
  void fillMoments(const xAOD::Jet& jet);
  void fillMomentsLeading(const xAOD::Jet& jet);
  void fillMomentsSubLeading(const xAOD::Jet& jet);
  void fillMomentsWithMassCut(const xAOD::Jet& jet);
  void fillMomentsLeadingWithMassCut(const xAOD::Jet& jet);
  void fillMomentsSubLeadingWithMassCut(const xAOD::Jet& jet);
  
  void fillTrackParameters(const xAOD::TrackParticle& track);
  void fillCaloEntryInfo(const xAOD::TrackParticle& track);
  void fillPerigeeInfo(const xAOD::TrackParticle& track);
  void fillPerigeeVsCaloEntry(const xAOD::TrackParticle& track);
  
  void fillTrackParametersAllPt(const xAOD::TrackParticle& track);
  void fillCaloEntryInfoAllPt(const xAOD::TrackParticle& track);
  void fillPerigeeInfoAllPt(const xAOD::TrackParticle& track);
  void fillPerigeeVsCaloEntryAllPt(const xAOD::TrackParticle& track);
  
  void fillMatching(const xAOD::TrackParticle& track);
  void fillCluster(const xAOD::CaloCluster& cluster);
  void fillClusterEtaCut(const xAOD::CaloCluster& cluster);
  
  void fillTCC(const xAOD::TrackCaloCluster& tcc, std::vector<const xAOD::TrackParticle*>& alltracksPV0);
  void fillTCCptCut(const xAOD::TrackCaloCluster& tcc);
  void fillTCCetaCut(const xAOD::TrackCaloCluster& tcc);
    
  void make_median(TH2* h2_response, TH1* h1_resolution, TH1* h1_median=nullptr);
  void make_median_add(TH2* h2_response, TH1* h1_resolution);
  void make_median(TH3* h3_response, TH2* h2_resolution);
  
  void setEventWeight(const float& weight);
  void setJetPtBinning(const std::vector<float>& bins);
  void setJetMassOverPtBinning(const std::vector<float>& bins);
  void setTrackPtBinning(const std::vector<float>& bins);
  void setTrackProdRadiusBinning(const std::vector<float>& bins);

  void resizeHistograms();
  
  const xAOD::TruthParticle* getTruthPtr(const xAOD::TrackParticle& trackParticle) const;
  const xAOD::TrackParticle* getTrackParticlePtr(const xAOD::TrackCaloCluster& tcc) const;

  
private:
  std::string m_collectionType             ;
  float       m_eventWeight                ;
  
  std::vector<float> m_jetPtBins           ;
  std::vector<float> m_jetMassOverPtBins   ;
  std::vector<float> m_trackPtBins         ;
  std::vector<float> m_trackProdRadiusBins ;
  
  TH1* m_jet_n                             ;
  TH1* m_jet_pt                            ;
  TH1* m_jet_eta                           ;
  TH1* m_jet_phi                           ;
  TH1* m_jet_m                             ;
  
  TH1* m_jet_m_2leadings                   ;
  TH1* m_jet_pt_2leadings                  ;
  TH1* m_jet_eta_2leadings                 ;
  TH1* m_jet_phi_2leadings                 ;
  
  TH1* m_jet_m_leading                     ;
  TH1* m_jet_pt_leading                    ;
  TH1* m_jet_eta_leading                   ;
  TH1* m_jet_phi_leading                   ;
  
  TH1* m_jet_m_subleading                  ;
  TH1* m_jet_pt_subleading                 ;
  TH1* m_jet_eta_subleading                ;
  TH1* m_jet_phi_subleading                ;
  
  TH1* m_jet_deltaR                        ;
  TH1* m_jet_deltaR_matched                ;
  
  TH1* m_jet_response_m                    ;
  TH2* m_jet_response_m_pt                 ;
  TH1* m_jet_response_pt                   ;
  TH1* m_jet_response_eta                  ;
  TH1* m_jet_response_phi                  ;
  
  TH1* m_jet_response_m_2leadings          ;
  TH2* m_jet_response_m_pt_2leadings       ;
  TH1* m_jet_response_pt_2leadings         ;
  TH1* m_jet_response_eta_2leadings        ;
  TH1* m_jet_response_phi_2leadings        ;
  
  TH1* m_jet_response_m_leading            ;
  TH2* m_jet_response_m_pt_leading         ;
  TH1* m_jet_response_pt_leading           ;
  TH1* m_jet_response_eta_leading          ;
  TH1* m_jet_response_phi_leading          ;
  
  TH1* m_jet_response_m_subleading         ;
  TH2* m_jet_response_m_pt_subleading      ;
  TH1* m_jet_response_pt_subleading        ;
  TH1* m_jet_response_eta_subleading       ;
  TH1* m_jet_response_phi_subleading       ;
  
  TH1* m_jet_response_add_eta              ;
  TH1* m_jet_response_add_eta_2leadings    ;
  TH1* m_jet_response_add_eta_leading      ;
  TH1* m_jet_response_add_eta_subleading   ;
  
  TH2* m_jet_response_add_eta_pt           ;
  TH2* m_jet_response_add_eta_pt_2leadings ;
  TH2* m_jet_response_add_eta_pt_leading   ;
  TH2* m_jet_response_add_eta_pt_subleading;
  
  TH2* m_jet_response_m_npv                ;
  TH2* m_jet_response_m_npv_2leadings      ;
  TH2* m_jet_response_m_npv_leading        ;
  TH2* m_jet_response_m_npv_subleading     ;
      
  TH2* m_jet_response_d2_pt                ;
  TH2* m_jet_response_d2_pt_2leadings      ;
  TH2* m_jet_response_d2_pt_leading        ;
  TH2* m_jet_response_d2_pt_subleading     ;
  
  TH2* m_jet_response_add_d2_pt                ;
  TH2* m_jet_response_add_d2_pt_2leadings      ;
  TH2* m_jet_response_add_d2_pt_leading        ;
  TH2* m_jet_response_add_d2_pt_subleading     ;
  
  TH1* m_jet_pseudoresponse_m              ;
  TH1* m_jet_pseudoresponse_pt             ;
  
  TH1* m_jet_pseudoresponse_m_2leadings    ;
  TH1* m_jet_pseudoresponse_pt_2leadings   ;
  
  TH1* m_jet_pseudoresponse_m_leading      ;
  TH1* m_jet_pseudoresponse_pt_leading     ;
  
  TH1* m_jet_pseudoresponse_m_subleading   ;
  TH1* m_jet_pseudoresponse_pt_subleading  ;
  
  TH1* m_jet_pseudoresponse_notcalib_pt            ;           
  TH1* m_jet_pseudoresponse_notcalib_pt_2leadings  ;
  TH1* m_jet_pseudoresponse_notcalib_pt_leading    ;
  TH1* m_jet_pseudoresponse_notcalib_pt_subleading ;
  
  TH1* m_jet_d2                            ;
  TH1* m_jet_d2_2leadings                  ;
  TH1* m_jet_d2_leading                    ;
  TH1* m_jet_d2_subleading                 ;
  
  TH1* m_jet_d2_massCut                    ;
  TH1* m_jet_d2_2leadings_massCut          ;
  TH1* m_jet_d2_leading_massCut            ;
  TH1* m_jet_d2_subleading_massCut         ;
  
  TH1* m_jet_response_d2                   ;
  TH1* m_jet_response_d2_2leadings         ;
  TH1* m_jet_response_d2_leading           ;
  TH1* m_jet_response_d2_subleading        ;  
  
  TH1* m_jet_response_add_d2                   ;
  TH1* m_jet_response_add_d2_2leadings         ;
  TH1* m_jet_response_add_d2_leading           ;
  TH1* m_jet_response_add_d2_subleading        ;  
  
  TH1* m_jet_resolution_m                  ;
  TH1* m_jet_resolution_m_2leadings        ;
  TH1* m_jet_resolution_m_leading          ;
  TH1* m_jet_resolution_m_subleading       ;

  TH1* m_jet_resolution_m_npv              ;
  TH1* m_jet_resolution_m_npv_2leadings    ;
  TH1* m_jet_resolution_m_npv_leading      ;
  TH1* m_jet_resolution_m_npv_subleading   ;
  
  TH1* m_jet_median_width_m_npv              ;
  TH1* m_jet_median_width_m_npv_2leadings    ;
  TH1* m_jet_median_width_m_npv_leading      ;
  TH1* m_jet_median_width_m_npv_subleading   ;

  TH1* m_jet_resolution_d2              ;
  TH1* m_jet_resolution_d2_2leadings    ;
  TH1* m_jet_resolution_d2_leading      ;
  TH1* m_jet_resolution_d2_subleading   ;  
  
  TH1* m_jet_resolution_add_d2              ;
  TH1* m_jet_resolution_add_d2_2leadings    ;
  TH1* m_jet_resolution_add_d2_leading      ;
  TH1* m_jet_resolution_add_d2_subleading   ;
  
  TH3* m_jet_mopt_pt_response_m                  ;
  TH3* m_jet_mopt_pt_response_m_2leadings        ;
  TH3* m_jet_mopt_pt_response_m_leading          ;
  TH3* m_jet_mopt_pt_response_m_subleading       ;
  TH3* m_jet_mopt_pt_response_pt                 ;
  TH3* m_jet_mopt_pt_response_pt_2leadings       ;
  TH3* m_jet_mopt_pt_response_pt_leading         ;
  TH3* m_jet_mopt_pt_response_pt_subleading      ;
  TH3* m_jet_mopt_pt_response_phi                ;
  TH3* m_jet_mopt_pt_response_phi_2leadings      ;
  TH3* m_jet_mopt_pt_response_phi_leading        ;
  TH3* m_jet_mopt_pt_response_phi_subleading     ;
  TH3* m_jet_mopt_pt_response_eta                ;
  TH3* m_jet_mopt_pt_response_eta_2leadings      ;
  TH3* m_jet_mopt_pt_response_eta_leading        ;
  TH3* m_jet_mopt_pt_response_eta_subleading     ;
  TH3* m_jet_mopt_pt_response_d2                 ;
  TH3* m_jet_mopt_pt_response_d2_2leadings       ;
  TH3* m_jet_mopt_pt_response_d2_leading         ;
  TH3* m_jet_mopt_pt_response_d2_subleading      ;
  
  TH2* m_jet_resolution_mopt_pt_m                ;
  TH2* m_jet_resolution_mopt_pt_m_2leadings      ;
  TH2* m_jet_resolution_mopt_pt_m_leading        ;
  TH2* m_jet_resolution_mopt_pt_m_subleading     ;
  TH2* m_jet_resolution_mopt_pt_pt               ;
  TH2* m_jet_resolution_mopt_pt_pt_2leadings     ;
  TH2* m_jet_resolution_mopt_pt_pt_leading       ;
  TH2* m_jet_resolution_mopt_pt_pt_subleading    ;
  TH2* m_jet_resolution_mopt_pt_phi              ;
  TH2* m_jet_resolution_mopt_pt_phi_2leadings    ;
  TH2* m_jet_resolution_mopt_pt_phi_leading      ;
  TH2* m_jet_resolution_mopt_pt_phi_subleading   ;
  TH2* m_jet_resolution_mopt_pt_eta              ;
  TH2* m_jet_resolution_mopt_pt_eta_2leadings    ;
  TH2* m_jet_resolution_mopt_pt_eta_leading      ;
  TH2* m_jet_resolution_mopt_pt_eta_subleading   ;
  TH2* m_jet_resolution_mopt_pt_d2               ;
  TH2* m_jet_resolution_mopt_pt_d2_2leadings     ;
  TH2* m_jet_resolution_mopt_pt_d2_leading       ;
  TH2* m_jet_resolution_mopt_pt_d2_subleading    ;
  
  //track histograms
  TH1* m_trk_d0                                  ;
  TH1* m_trk_z0                                  ;
  TH1* m_trk_theta                               ;
  TH1* m_trk_phi                                 ;
  TH1* m_trk_qOverP                              ;
  TH2* m_trk_d0_eta                              ;
  TH2* m_trk_z0_eta                              ;
  TH2* m_trk_theta_eta                           ;
  TH2* m_trk_phi_eta                             ;
  TH2* m_trk_qOverP_eta                          ;
  TH2* m_trk_d0_pt                               ;
  TH2* m_trk_z0_pt                               ;
  TH2* m_trk_theta_pt                            ;
  TH2* m_trk_phi_pt                              ;
  TH2* m_trk_qOverP_pt                           ;
  TH1* m_trk_caloEntryEta                        ;
  TH2* m_trk_caloEntryEtaOverEta_eta             ;
  TH2* m_trk_caloEntryEtaOverEta_pt              ;
  TH2* m_trk_caloEntryUncEta_caloEntryEta        ;
  TH2* m_trk_caloEntryUncEta_eta                 ;
  TH2* m_trk_caloEntryUncEta_pt                  ;
  TH2* m_trk_caloEntryUncTheta_eta               ;
  TH2* m_trk_caloEntryUncTheta_pt                ;
  TH1* m_trk_caloEntryPhi                        ;
  TH2* m_trk_caloEntryPhiOverPhi_eta             ;
  TH2* m_trk_caloEntryPhiOverPhi_pt              ;
  TH2* m_trk_caloEntryUncPhi_caloEntryPhi        ;
  TH2* m_trk_caloEntryUncPhi_eta                 ;
  TH2* m_trk_caloEntryUncPhi_pt                  ;
  TH2* m_trk_caloEntryUncTot_eta                 ;
  TH2* m_trk_caloEntryUncTot_pt                  ;
  TH1* m_trk_perigeeEta                          ;
  TH2* m_trk_perigeeUncEta_eta                   ;
  TH2* m_trk_perigeeUncEta_pt                    ;
  TH2* m_trk_perigeeUncTheta_eta                 ;
  TH2* m_trk_perigeeUncTheta_pt                  ;
  TH1* m_trk_perigeePhi                          ;
  TH2* m_trk_perigeeUncPhi_eta                   ;
  TH2* m_trk_perigeeUncPhi_pt                    ;
  TH2* m_trk_perigeeUncTot_eta                   ;
  TH2* m_trk_perigeeUncTot_pt                    ;
  TH2* m_trk_perigeeUncEta_caloUncEta            ;
  TH2* m_trk_perigeeUncTheta_caloUncTheta        ;
  TH2* m_trk_perigeeUncPhi_caloUncPhi            ;
  TH2* m_trk_perigeeUncTot_caloUncTot            ;
  
  TH1* m_trk_prodRadius                          ;
  TH2* m_trk_perigeeUncEta_prodRadius            ;
  TH2* m_trk_perigeeUncTheta_prodRadius          ;
  TH2* m_trk_perigeeUncPhi_prodRadius            ;
  TH2* m_trk_perigeeUncTot_prodRadius            ;
  TH2* m_trk_caloEntryUncEta_prodRadius          ;
  TH2* m_trk_caloEntryUncTheta_prodRadius        ;  
  TH2* m_trk_caloEntryUncPhi_prodRadius          ;
  TH2* m_trk_caloEntryUncTot_prodRadius          ;
  
  TH1* m_trk_delta_perigeeEta_caloEntryEta       ;
  TH1* m_trk_delta_perigeePhi_caloEntryPhi       ;
  TH2* m_trk_delta_perigeeEta_caloEntryEta_eta   ;
  TH2* m_trk_delta_perigeePhi_caloEntryPhi_eta   ;
  TH2* m_trk_delta_perigeeEta_caloEntryEta_pt    ;
  TH2* m_trk_delta_perigeePhi_caloEntryPhi_pt    ;
  
  TH1* m_trk_delta_trackEta_caloEntryEta         ;
  TH1* m_trk_delta_trackPhi_caloEntryPhi         ;
  TH2* m_trk_delta_trackEta_caloEntryEta_eta     ;
  TH2* m_trk_delta_trackPhi_caloEntryPhi_eta     ;
  TH2* m_trk_delta_trackEta_caloEntryEta_pt      ;
  TH2* m_trk_delta_trackPhi_caloEntryPhi_pt      ;
  
  TH1* m_trk_delta_trackEta_caloEntryEtaCorr     ;
  TH1* m_trk_delta_trackPhi_caloEntryPhiCorr     ;
  TH2* m_trk_delta_trackEta_caloEntryEtaCorr_eta ;
  TH2* m_trk_delta_trackPhi_caloEntryPhiCorr_eta ;
  TH2* m_trk_delta_trackEta_caloEntryEtaCorr_pt  ;
  TH2* m_trk_delta_trackPhi_caloEntryPhiCorr_pt  ;
  
  TH1* m_trk_delta_caloEntryEta_caloEntryEtaCorr    ;
  TH1* m_trk_delta_caloEntryPhi_caloEntryPhiCorr    ;
  TH2* m_trk_delta_caloEntryEta_caloEntryEtaCorr_eta;
  TH2* m_trk_delta_caloEntryPhi_caloEntryPhiCorr_eta;
  TH2* m_trk_delta_caloEntryEta_caloEntryEtaCorr_pt ;
  TH2* m_trk_delta_caloEntryPhi_caloEntryPhiCorr_pt ;
      
  TH1* m_clusters_eta                    ;
  TH1* m_clusters_matched_eta            ;
  TH1* m_clusters_notMatched_eta         ;
  TH1* m_clusters_matchedFraction_eta    ;
  TH1* m_clusters_notMatchedFraction_eta ;
  TH1* m_clusters_width                  ;
  TH2* m_clusters_width_eta              ;
  TH1* m_clusters_energy                 ;  
  TH1* m_clusters_matched_energy            ;
  TH1* m_clusters_notMatched_energy         ;
  TH1* m_clusters_matchedFraction_energy    ;
  TH1* m_clusters_notMatchedFraction_energy ;
  TH2* m_clusters_width_energy              ;
  TH1* m_clusters_etacut_eta                 ;
  TH1* m_clusters_etacut_width               ;
  TH2* m_clusters_etacut_width_eta           ;
  TH1* m_clusters_etacut_energy              ;
  TH2* m_clusters_etacut_width_energy        ;  

  TH1* m_clusters_matched_eta_fix_and_var        ;
  TH1* m_clusters_matched_eta_fix_or_var         ;
  TH1* m_clusters_matched_eta_fix                ;
  TH1* m_clusters_matched_eta_notfix             ;
  TH1* m_clusters_matched_eta_var                ;
  TH1* m_clusters_matched_eta_notvar             ;
  TH1* m_clusters_matched_eta_onlyvar            ;
  TH1* m_clusters_matched_eta_onlyfix            ;
  TH1* m_clusters_matched_eta_none               ;
  TH1* m_clusters_matchedFraction_eta_fix_and_var;
  TH1* m_clusters_matchedFraction_eta_fix_or_var ;
  TH1* m_clusters_matchedFraction_eta_fix        ;
  TH1* m_clusters_matchedFraction_eta_notfix     ;
  TH1* m_clusters_matchedFraction_eta_var        ;
  TH1* m_clusters_matchedFraction_eta_notvar     ;
  TH1* m_clusters_matchedFraction_eta_onlyvar    ;
  TH1* m_clusters_matchedFraction_eta_onlyfix    ;
  TH1* m_clusters_matchedFraction_eta_none       ; 
  
  TH1* m_clusters_abs_eta                    ;
  TH1* m_clusters_abs_matched_eta            ;
  TH1* m_clusters_abs_notMatched_eta         ;
  TH1* m_clusters_abs_matchedFraction_eta    ;
  TH1* m_clusters_abs_notMatchedFraction_eta ;
  TH1* m_clusters_abs_width                  ;
  TH2* m_clusters_abs_width_eta              ;
  TH1* m_clusters_abs_energy                 ;  
  TH1* m_clusters_abs_matched_energy            ;
  TH1* m_clusters_abs_notMatched_energy         ;
  TH1* m_clusters_abs_matchedFraction_energy    ;
  TH1* m_clusters_abs_notMatchedFraction_energy ;
  TH2* m_clusters_abs_width_energy           ;
  TH1* m_clusters_etacut_abs_eta             ;
  TH1* m_clusters_etacut_abs_width           ;
  TH2* m_clusters_etacut_abs_width_eta       ;
  TH1* m_clusters_etacut_abs_energy          ;
  TH2* m_clusters_etacut_abs_width_energy    ;
  
  TH1* m_clusters_abs_matched_eta_fix_and_var         ;
  TH1* m_clusters_abs_matched_eta_fix_or_var          ;
  TH1* m_clusters_abs_matched_eta_fix                 ;
  TH1* m_clusters_abs_matched_eta_notfix              ;
  TH1* m_clusters_abs_matched_eta_var                 ;
  TH1* m_clusters_abs_matched_eta_notvar              ;
  TH1* m_clusters_abs_matched_eta_onlyvar             ;
  TH1* m_clusters_abs_matched_eta_onlyfix             ;
  TH1* m_clusters_abs_matched_eta_none                ;
  TH1* m_clusters_abs_matchedFraction_eta_fix_and_var ;
  TH1* m_clusters_abs_matchedFraction_eta_fix_or_var  ;
  TH1* m_clusters_abs_matchedFraction_eta_fix         ;
  TH1* m_clusters_abs_matchedFraction_eta_notfix      ;
  TH1* m_clusters_abs_matchedFraction_eta_var         ;
  TH1* m_clusters_abs_matchedFraction_eta_notvar      ;
  TH1* m_clusters_abs_matchedFraction_eta_onlyvar     ;
  TH1* m_clusters_abs_matchedFraction_eta_onlyfix     ;
  TH1* m_clusters_abs_matchedFraction_eta_none        ;
                                                      
  TH1* m_clusters_matched_pv0_eta_fix_and_var         ;
  TH1* m_clusters_matched_pv0_eta_fix_or_var          ;
  TH1* m_clusters_matched_pv0_eta_fix                 ;
  TH1* m_clusters_matched_pv0_eta_notfix              ;
  TH1* m_clusters_matched_pv0_eta_var                 ;
  TH1* m_clusters_matched_pv0_eta_notvar              ;
  TH1* m_clusters_matched_pv0_eta_onlyvar             ;
  TH1* m_clusters_matched_pv0_eta_onlyfix             ;
  TH1* m_clusters_matched_pv0_eta_none                ;
  TH1* m_clusters_abs_matched_pv0_eta_fix_and_var     ;
  TH1* m_clusters_abs_matched_pv0_eta_fix_or_var      ;
  TH1* m_clusters_abs_matched_pv0_eta_fix             ;
  TH1* m_clusters_abs_matched_pv0_eta_notfix          ;
  TH1* m_clusters_abs_matched_pv0_eta_var             ;
  TH1* m_clusters_abs_matched_pv0_eta_notvar          ;
  TH1* m_clusters_abs_matched_pv0_eta_onlyvar         ;
  TH1* m_clusters_abs_matched_pv0_eta_onlyfix         ;
  TH1* m_clusters_abs_matched_pv0_eta_none            ;
  
  TH1* m_clusters_matchedpv0Fraction_eta_fix_and_var     ;
  TH1* m_clusters_matchedpv0Fraction_eta_fix_or_var      ;
  TH1* m_clusters_matchedpv0Fraction_eta_fix             ;
  TH1* m_clusters_matchedpv0Fraction_eta_notfix          ;
  TH1* m_clusters_matchedpv0Fraction_eta_var             ;
  TH1* m_clusters_matchedpv0Fraction_eta_notvar          ;
  TH1* m_clusters_matchedpv0Fraction_eta_onlyvar         ;
  TH1* m_clusters_matchedpv0Fraction_eta_onlyfix         ;
  TH1* m_clusters_matchedpv0Fraction_eta_none            ;                                                         
  TH1* m_clusters_abs_matchedpv0Fraction_eta_fix_and_var ;
  TH1* m_clusters_abs_matchedpv0Fraction_eta_fix_or_var  ;
  TH1* m_clusters_abs_matchedpv0Fraction_eta_fix         ;
  TH1* m_clusters_abs_matchedpv0Fraction_eta_notfix      ;
  TH1* m_clusters_abs_matchedpv0Fraction_eta_var         ;
  TH1* m_clusters_abs_matchedpv0Fraction_eta_notvar      ;
  TH1* m_clusters_abs_matchedpv0Fraction_eta_onlyvar     ;
  TH1* m_clusters_abs_matchedpv0Fraction_eta_onlyfix     ;
  TH1* m_clusters_abs_matchedpv0Fraction_eta_none        ;
  
  TH1* m_clusters_matched_pvx_eta_fix_and_var         ;
  TH1* m_clusters_matched_pvx_eta_fix_or_var          ;
  TH1* m_clusters_matched_pvx_eta_fix                 ;
  TH1* m_clusters_matched_pvx_eta_notfix              ;
  TH1* m_clusters_matched_pvx_eta_var                 ;
  TH1* m_clusters_matched_pvx_eta_notvar              ;
  TH1* m_clusters_matched_pvx_eta_onlyvar             ;
  TH1* m_clusters_matched_pvx_eta_onlyfix             ;
  TH1* m_clusters_matched_pvx_eta_none                ;
  TH1* m_clusters_abs_matched_pvx_eta_fix_and_var     ;
  TH1* m_clusters_abs_matched_pvx_eta_fix_or_var      ;
  TH1* m_clusters_abs_matched_pvx_eta_fix             ;
  TH1* m_clusters_abs_matched_pvx_eta_notfix          ;
  TH1* m_clusters_abs_matched_pvx_eta_var             ;
  TH1* m_clusters_abs_matched_pvx_eta_notvar          ;
  TH1* m_clusters_abs_matched_pvx_eta_onlyvar         ;
  TH1* m_clusters_abs_matched_pvx_eta_onlyfix         ;
  TH1* m_clusters_abs_matched_pvx_eta_none            ;
  
  TH1* m_clusters_matchedpvxFraction_eta_fix_and_var     ;
  TH1* m_clusters_matchedpvxFraction_eta_fix_or_var      ;
  TH1* m_clusters_matchedpvxFraction_eta_fix             ;
  TH1* m_clusters_matchedpvxFraction_eta_notfix          ;
  TH1* m_clusters_matchedpvxFraction_eta_var             ;
  TH1* m_clusters_matchedpvxFraction_eta_notvar          ;
  TH1* m_clusters_matchedpvxFraction_eta_onlyvar         ;
  TH1* m_clusters_matchedpvxFraction_eta_onlyfix         ;
  TH1* m_clusters_matchedpvxFraction_eta_none            ;                                                         
  TH1* m_clusters_abs_matchedpvxFraction_eta_fix_and_var ;
  TH1* m_clusters_abs_matchedpvxFraction_eta_fix_or_var  ;
  TH1* m_clusters_abs_matchedpvxFraction_eta_fix         ;
  TH1* m_clusters_abs_matchedpvxFraction_eta_notfix      ;
  TH1* m_clusters_abs_matchedpvxFraction_eta_var         ;
  TH1* m_clusters_abs_matchedpvxFraction_eta_notvar      ;
  TH1* m_clusters_abs_matchedpvxFraction_eta_onlyvar     ;
  TH1* m_clusters_abs_matchedpvxFraction_eta_onlyfix     ;
  TH1* m_clusters_abs_matchedpvxFraction_eta_none        ;
    
  TH2* m_clusters_pt_fraction_e                      ;
  TH2* m_clusters_PV0_pt_fraction_e                  ;
  TH2* m_clusters_PVX_pt_fraction_e                  ;
  TH2* m_clusters_rejected_pt_fraction_e             ;
  
  TH1* m_clusters_all_neutral_eta                    ;
  TH1* m_clusters_all_neutral_e                      ;
  TH1* m_clusters_all_neutral_PV0_eta                ;
  TH1* m_clusters_all_neutral_PV0_e                  ;
  TH1* m_clusters_all_neutral_notPV0_eta             ;
  TH1* m_clusters_all_neutral_notPV0_e               ;
  TH1* m_clusters_all_neutral_neutral_eta            ;
  TH1* m_clusters_all_neutral_neutral_e              ;
  
  TH1* m_trk_total_eta                                            ;
  TH1* m_trk_total_pt                                             ;
  TH2* m_trk_total_clusters_eta                                   ;
  TH2* m_trk_total_clusters_pt                                    ;
  TH1* m_trk_notMatching_deltar_fix_eta                           ;
  TH1* m_trk_notMatching_deltar_fix_phi                           ;
  TH1* m_trk_notMatching_deltar_fix_pt                            ;
  TH2* m_trk_notMatching_deltar_fix_caloEntryUncTot_eta           ;
  TH2* m_trk_notMatching_deltar_fix_caloEntryUncTot_pt            ;
  TH2* m_trk_notMatching_deltar_fix_caloEntryUncTot_prodRadius    ;
  TH1* m_trk_notMatching_deltar_var_eta                           ;
  TH1* m_trk_notMatching_deltar_var_phi                           ;
  TH1* m_trk_notMatching_deltar_var_pt                            ;
  TH2* m_trk_notMatching_deltar_var_caloEntryUncTot_eta           ;
  TH2* m_trk_notMatching_deltar_var_caloEntryUncTot_pt            ;
  TH2* m_trk_notMatching_deltar_var_caloEntryUncTot_prodRadius    ;
  TH1* m_trk_notMatching_deltar_none_eta                          ;
  TH1* m_trk_notMatching_deltar_none_phi                          ;
  TH1* m_trk_notMatching_deltar_none_pt                           ;
  TH2* m_trk_notMatching_deltar_none_caloEntryUncTot_eta          ;
  TH2* m_trk_notMatching_deltar_none_caloEntryUncTot_pt           ;
  TH2* m_trk_notMatching_deltar_none_caloEntryUncTot_prodRadius   ;
  TH1* m_trk_matching_deltar_fix_eta                              ;
  TH1* m_trk_matching_deltar_fix_pt                               ;
  TH1* m_trk_matching_deltar_var_eta                              ;
  TH1* m_trk_matching_deltar_var_pt                               ;
  TH1* m_trk_matching_deltar_onlyfix_eta                          ;
  TH1* m_trk_matching_deltar_onlyfix_pt                           ;
  TH1* m_trk_matching_deltar_onlyvar_eta                          ;
  TH1* m_trk_matching_deltar_onlyvar_pt                           ;
  TH1* m_trk_matching_deltar_fix_or_var_eta                       ;
  TH1* m_trk_matching_deltar_fix_or_var_pt                        ;
  TH1* m_trk_matching_deltar_fix_and_var_eta                      ;
  TH1* m_trk_matching_deltar_fix_and_var_pt                       ;
  TH2* m_trk_matching_deltar_or_clusters_eta                      ;
  TH2* m_trk_matching_deltar_or_clusters_phi                      ;
  TH2* m_trk_matching_deltar_or_clusters_pt                       ;
  TH2* m_trk_matching_deltar_and_clusters_eta                     ;
  TH2* m_trk_matching_deltar_and_clusters_phi                     ;
  TH2* m_trk_matching_deltar_and_clusters_pt                      ;
  TH2* m_trk_matching_deltar_fix_only_clusters_eta                ;
  TH2* m_trk_matching_deltar_fix_only_clusters_phi                ;
  TH2* m_trk_matching_deltar_fix_only_clusters_pt                 ;
  TH2* m_trk_matching_deltar_var_only_clusters_eta                ;
  TH2* m_trk_matching_deltar_var_only_clusters_phi                ;
  TH2* m_trk_matching_deltar_var_only_clusters_pt                 ;
  
  TH1* m_trk_matchedFraction_eta_fix_and_var                      ;
  TH1* m_trk_matchedFraction_eta_fix_or_var                       ;
  TH1* m_trk_matchedFraction_eta_fix                              ;
  TH1* m_trk_matchedFraction_eta_notfix                           ;
  TH1* m_trk_matchedFraction_eta_var                              ;
  TH1* m_trk_matchedFraction_eta_notvar                           ;
  TH1* m_trk_matchedFraction_eta_onlyvar                          ;
  TH1* m_trk_matchedFraction_eta_onlyfix                          ;
  TH1* m_trk_matchedFraction_eta_none                             ;
  TH1* m_trk_matchedFraction_pt_fix_and_var                       ;
  TH1* m_trk_matchedFraction_pt_fix_or_var                        ;
  TH1* m_trk_matchedFraction_pt_fix                               ;
  TH1* m_trk_matchedFraction_pt_notfix                            ;
  TH1* m_trk_matchedFraction_pt_var                               ;
  TH1* m_trk_matchedFraction_pt_notvar                            ;
  TH1* m_trk_matchedFraction_pt_onlyvar                           ;
  TH1* m_trk_matchedFraction_pt_onlyfix                           ;
  TH1* m_trk_matchedFraction_pt_none                              ;
  
  TH1* m_trk_pv0_total_eta                                        ;
  TH1* m_trk_pv0_total_pt                                         ;
  TH2* m_trk_pv0_caloEntryUncTot_eta                              ;
  TH2* m_trk_pv0_caloEntryUncTot_pt                               ;
  TH2* m_trk_pv0_caloEntryUncTot_truthpt                          ;
  TH2* m_trk_pv0_total_clusters_eta                               ;
  TH2* m_trk_pv0_total_clusters_pt                                ;
  TH1* m_trk_pv0_matching_deltar_fix_eta                          ;
  TH1* m_trk_pv0_matching_deltar_fix_pt                           ;
  TH1* m_trk_pv0_matching_deltar_var_eta                          ;
  TH1* m_trk_pv0_matching_deltar_var_pt                           ;
  TH1* m_trk_pv0_notMatching_deltar_fix_eta                       ; 
  TH1* m_trk_pv0_notMatching_deltar_fix_pt                        ;
  TH1* m_trk_pv0_notMatching_deltar_var_eta                       ;
  TH1* m_trk_pv0_notMatching_deltar_var_pt                        ;
  TH1* m_trk_pv0_notMatching_deltar_none_eta                      ;
  TH1* m_trk_pv0_notMatching_deltar_none_pt                       ;
  TH1* m_trk_pv0_matching_deltar_fix_and_var_eta                  ;
  TH1* m_trk_pv0_matching_deltar_fix_and_var_pt                   ;
  TH1* m_trk_pv0_matching_deltar_fix_or_var_eta                   ;
  TH1* m_trk_pv0_matching_deltar_fix_or_var_pt                    ;
  TH1* m_trk_pv0_matching_deltar_onlyfix_eta                      ;
  TH1* m_trk_pv0_matching_deltar_onlyfix_pt                       ;
  TH1* m_trk_pv0_matching_deltar_onlyvar_eta                      ;
  TH1* m_trk_pv0_matching_deltar_onlyvar_pt                       ;
  TH1* m_trk_pv0_matchedFraction_eta_fix_and_var                  ;
  TH1* m_trk_pv0_matchedFraction_eta_fix_or_var                   ;
  TH1* m_trk_pv0_matchedFraction_eta_fix                          ;
  TH1* m_trk_pv0_matchedFraction_eta_notfix                       ;
  TH1* m_trk_pv0_matchedFraction_eta_var                          ;
  TH1* m_trk_pv0_matchedFraction_eta_notvar                       ;
  TH1* m_trk_pv0_matchedFraction_eta_onlyvar                      ;
  TH1* m_trk_pv0_matchedFraction_eta_onlyfix                      ;
  TH1* m_trk_pv0_matchedFraction_eta_none                         ;
  TH1* m_trk_pv0_matchedFraction_pt_fix_and_var                   ;
  TH1* m_trk_pv0_matchedFraction_pt_fix_or_var                    ;
  TH1* m_trk_pv0_matchedFraction_pt_fix                           ;
  TH1* m_trk_pv0_matchedFraction_pt_notfix                        ;
  TH1* m_trk_pv0_matchedFraction_pt_var                           ;
  TH1* m_trk_pv0_matchedFraction_pt_notvar                        ;
  TH1* m_trk_pv0_matchedFraction_pt_onlyvar                       ;
  TH1* m_trk_pv0_matchedFraction_pt_onlyfix                       ;
  TH1* m_trk_pv0_matchedFraction_pt_none                          ;
  
  TH2* m_trk_pv0_caloEntryUncTot_eta_0SiHits                      ;
  TH2* m_trk_pv0_caloEntryUncTot_pt_0SiHits                       ;
  TH2* m_trk_pv0_caloEntryUncTot_rFirstHit_0SiHits                ;
  TH2* m_trk_pv0_caloEntryUncTot_nSiHits                          ;
  TH2* m_trk_pv0_caloEntryUncTot_rFirstHit                        ;
    
  TH1* m_tcc_pt                                                   ;
  TH1* m_tcc_pt_etacut                                            ;
  TH1* m_tcc_phi                                                  ;
  TH1* m_tcc_eta                                                  ;
  TH1* m_tcc_eta_ptcut                                            ;
  TH1* m_tcc_m                                                    ;
  TH1* m_tcc_taste                                                ;
  TH2* m_tcc_taste_pt                                             ;
  TH2* m_tcc_taste_pt_etacut                                      ;
  TH2* m_tcc_taste_eta                                            ;
  TH2* m_tcc_taste_eta_ptcut                                      ;
  TH2* m_tcc_pt_truth_pt                                          ;
  TH1* m_tcc_pt_response                                          ;
  TH2* m_tcc_pt_track_pt                                          ;
  TH1* m_tcc_pt_pseudoresponse                                    ;
  TH2* m_tcc_N_M                                                  ;
  TH2* m_tcc_N_M_pv0                                              ;
  
  TH2* m_trk_tcc_reco_pt_truth_pt                                 ;
  TH1* m_trk_tcc_reco_pt_response                                 ;   
      
  // plot base has nop default implementation of this; we use it to book the histos
  void initializePlots();
  void finalizePlots();
  
};

inline const xAOD::TruthParticle* TCCPlots::getTruthPtr(const xAOD::TrackParticle& trackParticle) const {
  typedef ElementLink<xAOD::TruthParticleContainer> ElementTruthLink_t;
  const xAOD::TruthParticle* result(nullptr);
  // 0. is there any truth?
  if (trackParticle.isAvailable<ElementTruthLink_t>("truthParticleLink")) {
    // 1. ..then get link
    const ElementTruthLink_t ptruthContainer = trackParticle.auxdata<ElementTruthLink_t>("truthParticleLink");
    if (ptruthContainer.isValid()) {
      result = *ptruthContainer;
    }
  }
  return result;
}

inline const xAOD::TrackParticle* TCCPlots::getTrackParticlePtr(const xAOD::TrackCaloCluster& tcc) const {
  typedef ElementLink<xAOD::TrackParticleContainer> ElementTruthLink_t;
  const xAOD::TrackParticle* result(nullptr);
  // 0. is there any truth?
  if (tcc.isAvailable<ElementTruthLink_t>("trackParticleLink")) {
    // 1. ..then get link
    const ElementTruthLink_t ptrackContainer = tcc.auxdata<ElementTruthLink_t>("trackParticleLink");
    if (ptrackContainer.isValid()) {
      result = *ptrackContainer;
    }
  }
  return result;
}



#endif // TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECVALIDATION_TCCPLOTS_H