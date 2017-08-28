/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TCCPlots.h"
#include <cmath>

using CLHEP::GeV;

TCCPlots::TCCPlots(TCCPlotsBase* pParent, const std::string& sDir, const std::string& collection) :
  TCCPlotsBase(pParent, sDir),
  m_collectionType (collection),
  m_eventWeight    (1.0),
  m_jet_n                           (nullptr),
  m_jet_pt                          (nullptr),
  m_jet_eta                         (nullptr),
  m_jet_phi                         (nullptr),
  m_jet_m                           (nullptr),
  m_jet_m_2leadings                 (nullptr),
  m_jet_pt_2leadings                (nullptr),
  m_jet_eta_2leadings               (nullptr),
  m_jet_phi_2leadings               (nullptr),
  m_jet_m_leading                   (nullptr),
  m_jet_pt_leading                  (nullptr),
  m_jet_eta_leading                 (nullptr),
  m_jet_phi_leading                 (nullptr),
  m_jet_m_subleading                (nullptr),
  m_jet_pt_subleading               (nullptr),
  m_jet_eta_subleading              (nullptr),
  m_jet_phi_subleading              (nullptr),
  m_jet_deltaR                      (nullptr),
  m_jet_deltaR_matched              (nullptr),
  m_jet_response_m                  (nullptr),
  m_jet_response_m_pt               (nullptr),
  m_jet_response_pt                 (nullptr),
  m_jet_response_eta                (nullptr),
  m_jet_response_phi                (nullptr),
  m_jet_response_m_2leadings        (nullptr),
  m_jet_response_m_pt_2leadings     (nullptr),
  m_jet_response_pt_2leadings       (nullptr),
  m_jet_response_eta_2leadings      (nullptr),
  m_jet_response_phi_2leadings      (nullptr),
  m_jet_response_m_leading          (nullptr),
  m_jet_response_m_pt_leading       (nullptr),
  m_jet_response_pt_leading         (nullptr),
  m_jet_response_eta_leading        (nullptr),
  m_jet_response_phi_leading        (nullptr),
  m_jet_response_m_subleading       (nullptr),
  m_jet_response_m_pt_subleading    (nullptr),
  m_jet_response_pt_subleading      (nullptr),
  m_jet_response_eta_subleading     (nullptr),
  m_jet_response_phi_subleading     (nullptr),
  m_jet_response_add_eta            (nullptr),
  m_jet_response_add_eta_2leadings  (nullptr),
  m_jet_response_add_eta_leading    (nullptr),
  m_jet_response_add_eta_subleading (nullptr),
  m_jet_response_add_eta_pt            (nullptr),
  m_jet_response_add_eta_pt_2leadings  (nullptr),
  m_jet_response_add_eta_pt_leading    (nullptr),
  m_jet_response_add_eta_pt_subleading (nullptr),
  m_jet_response_m_npv              (nullptr),
  m_jet_response_m_npv_2leadings    (nullptr),
  m_jet_response_m_npv_leading      (nullptr),
  m_jet_response_m_npv_subleading   (nullptr),
  m_jet_response_d2_pt              (nullptr),
  m_jet_response_d2_pt_2leadings    (nullptr),
  m_jet_response_d2_pt_leading      (nullptr),
  m_jet_response_d2_pt_subleading   (nullptr),
  m_jet_response_add_d2_pt            (nullptr),
  m_jet_response_add_d2_pt_2leadings  (nullptr),
  m_jet_response_add_d2_pt_leading    (nullptr),
  m_jet_response_add_d2_pt_subleading (nullptr),
  m_jet_pseudoresponse_m            (nullptr),
  m_jet_pseudoresponse_pt           (nullptr),
  m_jet_pseudoresponse_m_2leadings  (nullptr),
  m_jet_pseudoresponse_pt_2leadings (nullptr),
  m_jet_pseudoresponse_m_leading    (nullptr),
  m_jet_pseudoresponse_pt_leading   (nullptr),
  m_jet_pseudoresponse_m_subleading (nullptr),
  m_jet_pseudoresponse_pt_subleading(nullptr),
  m_jet_pseudoresponse_notcalib_pt           (nullptr),
  m_jet_pseudoresponse_notcalib_pt_2leadings (nullptr),
  m_jet_pseudoresponse_notcalib_pt_leading   (nullptr),
  m_jet_pseudoresponse_notcalib_pt_subleading(nullptr),
  m_jet_d2                          (nullptr),
  m_jet_d2_2leadings                (nullptr),
  m_jet_d2_leading                  (nullptr),
  m_jet_d2_subleading               (nullptr),
  m_jet_d2_massCut                  (nullptr),
  m_jet_d2_2leadings_massCut        (nullptr),
  m_jet_d2_leading_massCut          (nullptr),
  m_jet_d2_subleading_massCut       (nullptr),
  m_jet_response_d2                 (nullptr),
  m_jet_response_d2_2leadings       (nullptr),
  m_jet_response_d2_leading         (nullptr),
  m_jet_response_d2_subleading      (nullptr),
  m_jet_response_add_d2             (nullptr),
  m_jet_response_add_d2_2leadings   (nullptr),
  m_jet_response_add_d2_leading     (nullptr),
  m_jet_response_add_d2_subleading  (nullptr),
  m_jet_resolution_m                (nullptr),
  m_jet_resolution_m_2leadings      (nullptr),
  m_jet_resolution_m_leading        (nullptr),
  m_jet_resolution_m_subleading     (nullptr),
  m_jet_resolution_m_npv            (nullptr),
  m_jet_resolution_m_npv_2leadings  (nullptr),
  m_jet_resolution_m_npv_leading    (nullptr),
  m_jet_resolution_m_npv_subleading (nullptr),
  m_jet_median_width_m_npv            (nullptr),
  m_jet_median_width_m_npv_2leadings  (nullptr),
  m_jet_median_width_m_npv_leading    (nullptr),
  m_jet_median_width_m_npv_subleading (nullptr),  
  m_jet_resolution_d2                 (nullptr),
  m_jet_resolution_d2_2leadings       (nullptr),
  m_jet_resolution_d2_leading         (nullptr),
  m_jet_resolution_d2_subleading      (nullptr),
  m_jet_resolution_add_d2             (nullptr),
  m_jet_resolution_add_d2_2leadings   (nullptr),
  m_jet_resolution_add_d2_leading     (nullptr),
  m_jet_resolution_add_d2_subleading  (nullptr),
  m_jet_mopt_pt_response_m                   (nullptr),  
  m_jet_mopt_pt_response_m_2leadings         (nullptr),  
  m_jet_mopt_pt_response_m_leading           (nullptr),  
  m_jet_mopt_pt_response_m_subleading        (nullptr),  
  m_jet_mopt_pt_response_pt                  (nullptr),  
  m_jet_mopt_pt_response_pt_2leadings        (nullptr),  
  m_jet_mopt_pt_response_pt_leading          (nullptr),  
  m_jet_mopt_pt_response_pt_subleading       (nullptr),  
  m_jet_mopt_pt_response_phi                 (nullptr),  
  m_jet_mopt_pt_response_phi_2leadings       (nullptr),  
  m_jet_mopt_pt_response_phi_leading         (nullptr),  
  m_jet_mopt_pt_response_phi_subleading      (nullptr),  
  m_jet_mopt_pt_response_eta                 (nullptr),  
  m_jet_mopt_pt_response_eta_2leadings       (nullptr),  
  m_jet_mopt_pt_response_eta_leading         (nullptr),  
  m_jet_mopt_pt_response_eta_subleading      (nullptr),  
  m_jet_mopt_pt_response_d2                  (nullptr),  
  m_jet_mopt_pt_response_d2_2leadings        (nullptr),  
  m_jet_mopt_pt_response_d2_leading          (nullptr),  
  m_jet_mopt_pt_response_d2_subleading       (nullptr),  
  m_jet_resolution_mopt_pt_m                 (nullptr),  
  m_jet_resolution_mopt_pt_m_2leadings       (nullptr),  
  m_jet_resolution_mopt_pt_m_leading         (nullptr),  
  m_jet_resolution_mopt_pt_m_subleading      (nullptr),  
  m_jet_resolution_mopt_pt_pt                (nullptr),  
  m_jet_resolution_mopt_pt_pt_2leadings      (nullptr),  
  m_jet_resolution_mopt_pt_pt_leading        (nullptr),  
  m_jet_resolution_mopt_pt_pt_subleading     (nullptr),  
  m_jet_resolution_mopt_pt_phi               (nullptr),  
  m_jet_resolution_mopt_pt_phi_2leadings     (nullptr),  
  m_jet_resolution_mopt_pt_phi_leading       (nullptr),  
  m_jet_resolution_mopt_pt_phi_subleading    (nullptr),  
  m_jet_resolution_mopt_pt_eta               (nullptr),  
  m_jet_resolution_mopt_pt_eta_2leadings     (nullptr),  
  m_jet_resolution_mopt_pt_eta_leading       (nullptr),  
  m_jet_resolution_mopt_pt_eta_subleading    (nullptr),  
  m_jet_resolution_mopt_pt_d2                (nullptr),  
  m_jet_resolution_mopt_pt_d2_2leadings      (nullptr),  
  m_jet_resolution_mopt_pt_d2_leading        (nullptr),  
  m_jet_resolution_mopt_pt_d2_subleading     (nullptr),
  m_trk_d0                                   (nullptr),
  m_trk_z0                                   (nullptr),
  m_trk_theta                                (nullptr),
  m_trk_phi                                  (nullptr),
  m_trk_qOverP                               (nullptr),
  m_trk_d0_eta                               (nullptr),
  m_trk_z0_eta                               (nullptr),
  m_trk_theta_eta                            (nullptr),
  m_trk_phi_eta                              (nullptr),
  m_trk_qOverP_eta                           (nullptr),
  m_trk_d0_pt                                (nullptr),
  m_trk_z0_pt                                (nullptr),
  m_trk_theta_pt                             (nullptr),
  m_trk_phi_pt                               (nullptr),
  m_trk_qOverP_pt                            (nullptr),
  m_trk_caloEntryEta                         (nullptr),
  m_trk_caloEntryEtaOverEta_eta              (nullptr),
  m_trk_caloEntryEtaOverEta_pt               (nullptr),
  m_trk_caloEntryUncEta_caloEntryEta         (nullptr),
  m_trk_caloEntryUncEta_eta                  (nullptr),
  m_trk_caloEntryUncEta_pt                   (nullptr),
  m_trk_caloEntryUncTheta_eta                (nullptr),
  m_trk_caloEntryUncTheta_pt                 (nullptr),
  m_trk_caloEntryPhi                         (nullptr),
  m_trk_caloEntryPhiOverPhi_eta              (nullptr),
  m_trk_caloEntryPhiOverPhi_pt               (nullptr),
  m_trk_caloEntryUncPhi_caloEntryPhi         (nullptr),
  m_trk_caloEntryUncPhi_eta                  (nullptr),
  m_trk_caloEntryUncPhi_pt                   (nullptr),
  m_trk_caloEntryUncTot_eta                  (nullptr),
  m_trk_caloEntryUncTot_pt                   (nullptr),
  m_trk_perigeeEta                           (nullptr),
  m_trk_perigeeUncEta_eta                    (nullptr),
  m_trk_perigeeUncEta_pt                     (nullptr),
  m_trk_perigeeUncTheta_eta                  (nullptr),
  m_trk_perigeeUncTheta_pt                   (nullptr),
  m_trk_perigeePhi                           (nullptr),
  m_trk_perigeeUncPhi_eta                    (nullptr),
  m_trk_perigeeUncPhi_pt                     (nullptr),
  m_trk_perigeeUncTot_eta                    (nullptr),
  m_trk_perigeeUncTot_pt                     (nullptr),
  m_trk_perigeeUncEta_caloUncEta             (nullptr),
  m_trk_perigeeUncTheta_caloUncTheta         (nullptr),
  m_trk_perigeeUncPhi_caloUncPhi             (nullptr),
  m_trk_perigeeUncTot_caloUncTot             (nullptr),
  m_trk_prodRadius                           (nullptr),
  m_trk_perigeeUncEta_prodRadius             (nullptr),
  m_trk_perigeeUncTheta_prodRadius           (nullptr),
  m_trk_perigeeUncPhi_prodRadius             (nullptr), 
  m_trk_perigeeUncTot_prodRadius             (nullptr), 
  m_trk_caloEntryUncEta_prodRadius           (nullptr), 
  m_trk_caloEntryUncTheta_prodRadius         (nullptr), 
  m_trk_caloEntryUncPhi_prodRadius           (nullptr), 
  m_trk_caloEntryUncTot_prodRadius           (nullptr),
  m_trk_delta_perigeeEta_caloEntryEta        (nullptr),
  m_trk_delta_perigeePhi_caloEntryPhi        (nullptr),
  m_trk_delta_perigeeEta_caloEntryEta_eta    (nullptr),
  m_trk_delta_perigeePhi_caloEntryPhi_eta    (nullptr),
  m_trk_delta_perigeeEta_caloEntryEta_pt     (nullptr),
  m_trk_delta_perigeePhi_caloEntryPhi_pt     (nullptr),
  m_trk_delta_trackEta_caloEntryEta          (nullptr),
  m_trk_delta_trackPhi_caloEntryPhi          (nullptr),
  m_trk_delta_trackEta_caloEntryEta_eta      (nullptr),
  m_trk_delta_trackPhi_caloEntryPhi_eta      (nullptr),
  m_trk_delta_trackEta_caloEntryEta_pt       (nullptr),
  m_trk_delta_trackPhi_caloEntryPhi_pt       (nullptr),
  m_trk_delta_trackEta_caloEntryEtaCorr      (nullptr),
  m_trk_delta_trackPhi_caloEntryPhiCorr      (nullptr),
  m_trk_delta_trackEta_caloEntryEtaCorr_eta  (nullptr),
  m_trk_delta_trackPhi_caloEntryPhiCorr_eta  (nullptr),
  m_trk_delta_trackEta_caloEntryEtaCorr_pt   (nullptr),
  m_trk_delta_trackPhi_caloEntryPhiCorr_pt   (nullptr),
  m_trk_delta_caloEntryEta_caloEntryEtaCorr    (nullptr),
  m_trk_delta_caloEntryPhi_caloEntryPhiCorr    (nullptr),
  m_trk_delta_caloEntryEta_caloEntryEtaCorr_eta(nullptr),
  m_trk_delta_caloEntryPhi_caloEntryPhiCorr_eta(nullptr),
  m_trk_delta_caloEntryEta_caloEntryEtaCorr_pt (nullptr),
  m_trk_delta_caloEntryPhi_caloEntryPhiCorr_pt (nullptr),
  m_clusters_eta                               (nullptr),
  m_clusters_matched_eta                       (nullptr),
  m_clusters_notMatched_eta                    (nullptr),
  m_clusters_matchedFraction_eta               (nullptr),
  m_clusters_notMatchedFraction_eta            (nullptr),
  m_clusters_width                             (nullptr),
  m_clusters_width_eta                         (nullptr),
  m_clusters_energy                            (nullptr),
  m_clusters_matched_energy                    (nullptr),
  m_clusters_notMatched_energy                 (nullptr),
  m_clusters_matchedFraction_energy            (nullptr),
  m_clusters_notMatchedFraction_energy         (nullptr),
  m_clusters_width_energy                      (nullptr),
  m_clusters_etacut_eta                        (nullptr),
  m_clusters_etacut_width                      (nullptr),
  m_clusters_etacut_width_eta                  (nullptr),
  m_clusters_etacut_energy                     (nullptr),
  m_clusters_etacut_width_energy               (nullptr),          
  m_clusters_matched_eta_fix_and_var           (nullptr),
  m_clusters_matched_eta_fix_or_var            (nullptr),
  m_clusters_matched_eta_fix                   (nullptr),
  m_clusters_matched_eta_notfix                (nullptr),
  m_clusters_matched_eta_var                   (nullptr),
  m_clusters_matched_eta_notvar                (nullptr),
  m_clusters_matched_eta_onlyvar               (nullptr),
  m_clusters_matched_eta_onlyfix               (nullptr),
  m_clusters_matched_eta_none                  (nullptr),
  m_clusters_matchedFraction_eta_fix_and_var   (nullptr),
  m_clusters_matchedFraction_eta_fix_or_var    (nullptr),
  m_clusters_matchedFraction_eta_fix           (nullptr),
  m_clusters_matchedFraction_eta_notfix        (nullptr),
  m_clusters_matchedFraction_eta_var           (nullptr),
  m_clusters_matchedFraction_eta_notvar        (nullptr),
  m_clusters_matchedFraction_eta_onlyvar       (nullptr),
  m_clusters_matchedFraction_eta_onlyfix       (nullptr),
  m_clusters_matchedFraction_eta_none          (nullptr),
  m_clusters_abs_eta                               (nullptr),
  m_clusters_abs_matched_eta                       (nullptr),
  m_clusters_abs_notMatched_eta                    (nullptr),
  m_clusters_abs_matchedFraction_eta               (nullptr),
  m_clusters_abs_notMatchedFraction_eta            (nullptr),
  m_clusters_abs_width                             (nullptr),
  m_clusters_abs_width_eta                         (nullptr),
  m_clusters_abs_energy                            (nullptr),
  m_clusters_abs_matched_energy                    (nullptr),
  m_clusters_abs_notMatched_energy                 (nullptr),
  m_clusters_abs_matchedFraction_energy            (nullptr),
  m_clusters_abs_notMatchedFraction_energy         (nullptr),
  m_clusters_abs_width_energy                      (nullptr),
  m_clusters_etacut_abs_eta                        (nullptr), 
  m_clusters_etacut_abs_width                      (nullptr),
  m_clusters_etacut_abs_width_eta                  (nullptr),
  m_clusters_etacut_abs_energy                     (nullptr),
  m_clusters_etacut_abs_width_energy               (nullptr),
  m_clusters_abs_matched_eta_fix_and_var           (nullptr),
  m_clusters_abs_matched_eta_fix_or_var            (nullptr),
  m_clusters_abs_matched_eta_fix                   (nullptr),
  m_clusters_abs_matched_eta_notfix                (nullptr),
  m_clusters_abs_matched_eta_var                   (nullptr),
  m_clusters_abs_matched_eta_notvar                (nullptr),
  m_clusters_abs_matched_eta_onlyvar               (nullptr),
  m_clusters_abs_matched_eta_onlyfix               (nullptr),
  m_clusters_abs_matched_eta_none                  (nullptr),
  m_clusters_abs_matchedFraction_eta_fix_and_var   (nullptr),
  m_clusters_abs_matchedFraction_eta_fix_or_var    (nullptr),
  m_clusters_abs_matchedFraction_eta_fix           (nullptr),
  m_clusters_abs_matchedFraction_eta_notfix        (nullptr),
  m_clusters_abs_matchedFraction_eta_var           (nullptr),
  m_clusters_abs_matchedFraction_eta_notvar        (nullptr),
  m_clusters_abs_matchedFraction_eta_onlyvar       (nullptr),
  m_clusters_abs_matchedFraction_eta_onlyfix       (nullptr),
  m_clusters_abs_matchedFraction_eta_none          (nullptr),
  m_clusters_matched_pv0_eta_fix_and_var           (nullptr),
  m_clusters_matched_pv0_eta_fix_or_var            (nullptr),
  m_clusters_matched_pv0_eta_fix                   (nullptr),
  m_clusters_matched_pv0_eta_notfix                (nullptr),
  m_clusters_matched_pv0_eta_var                   (nullptr),
  m_clusters_matched_pv0_eta_notvar                (nullptr),
  m_clusters_matched_pv0_eta_onlyvar               (nullptr),
  m_clusters_matched_pv0_eta_onlyfix               (nullptr),
  m_clusters_matched_pv0_eta_none                  (nullptr),
  m_clusters_abs_matched_pv0_eta_fix_and_var       (nullptr),
  m_clusters_abs_matched_pv0_eta_fix_or_var        (nullptr),
  m_clusters_abs_matched_pv0_eta_fix               (nullptr),
  m_clusters_abs_matched_pv0_eta_notfix            (nullptr),
  m_clusters_abs_matched_pv0_eta_var               (nullptr),
  m_clusters_abs_matched_pv0_eta_notvar            (nullptr),
  m_clusters_abs_matched_pv0_eta_onlyvar           (nullptr),
  m_clusters_abs_matched_pv0_eta_onlyfix           (nullptr),
  m_clusters_abs_matched_pv0_eta_none              (nullptr),
  m_clusters_matchedpv0Fraction_eta_fix_and_var    (nullptr),
  m_clusters_matchedpv0Fraction_eta_fix_or_var     (nullptr),
  m_clusters_matchedpv0Fraction_eta_fix            (nullptr),
  m_clusters_matchedpv0Fraction_eta_notfix         (nullptr),
  m_clusters_matchedpv0Fraction_eta_var            (nullptr),
  m_clusters_matchedpv0Fraction_eta_notvar         (nullptr),
  m_clusters_matchedpv0Fraction_eta_onlyvar        (nullptr),
  m_clusters_matchedpv0Fraction_eta_onlyfix        (nullptr),
  m_clusters_matchedpv0Fraction_eta_none           (nullptr),
  m_clusters_abs_matchedpv0Fraction_eta_fix_and_var(nullptr),
  m_clusters_abs_matchedpv0Fraction_eta_fix_or_var (nullptr),
  m_clusters_abs_matchedpv0Fraction_eta_fix        (nullptr),
  m_clusters_abs_matchedpv0Fraction_eta_notfix     (nullptr),
  m_clusters_abs_matchedpv0Fraction_eta_var        (nullptr),
  m_clusters_abs_matchedpv0Fraction_eta_notvar     (nullptr),
  m_clusters_abs_matchedpv0Fraction_eta_onlyvar    (nullptr),
  m_clusters_abs_matchedpv0Fraction_eta_onlyfix    (nullptr),
  m_clusters_abs_matchedpv0Fraction_eta_none       (nullptr),
  m_clusters_matched_pvx_eta_fix_and_var           (nullptr),
  m_clusters_matched_pvx_eta_fix_or_var            (nullptr),
  m_clusters_matched_pvx_eta_fix                   (nullptr),
  m_clusters_matched_pvx_eta_notfix                (nullptr),
  m_clusters_matched_pvx_eta_var                   (nullptr),
  m_clusters_matched_pvx_eta_notvar                (nullptr),
  m_clusters_matched_pvx_eta_onlyvar               (nullptr),
  m_clusters_matched_pvx_eta_onlyfix               (nullptr),
  m_clusters_matched_pvx_eta_none                  (nullptr),
  m_clusters_abs_matched_pvx_eta_fix_and_var       (nullptr),
  m_clusters_abs_matched_pvx_eta_fix_or_var        (nullptr),
  m_clusters_abs_matched_pvx_eta_fix               (nullptr),
  m_clusters_abs_matched_pvx_eta_notfix            (nullptr),
  m_clusters_abs_matched_pvx_eta_var               (nullptr),
  m_clusters_abs_matched_pvx_eta_notvar            (nullptr),
  m_clusters_abs_matched_pvx_eta_onlyvar           (nullptr),
  m_clusters_abs_matched_pvx_eta_onlyfix           (nullptr),
  m_clusters_abs_matched_pvx_eta_none              (nullptr),
  m_clusters_matchedpvxFraction_eta_fix_and_var    (nullptr),
  m_clusters_matchedpvxFraction_eta_fix_or_var     (nullptr),
  m_clusters_matchedpvxFraction_eta_fix            (nullptr),
  m_clusters_matchedpvxFraction_eta_notfix         (nullptr),
  m_clusters_matchedpvxFraction_eta_var            (nullptr),
  m_clusters_matchedpvxFraction_eta_notvar         (nullptr),
  m_clusters_matchedpvxFraction_eta_onlyvar        (nullptr),
  m_clusters_matchedpvxFraction_eta_onlyfix        (nullptr),
  m_clusters_matchedpvxFraction_eta_none           (nullptr),
  m_clusters_abs_matchedpvxFraction_eta_fix_and_var(nullptr),
  m_clusters_abs_matchedpvxFraction_eta_fix_or_var (nullptr),
  m_clusters_abs_matchedpvxFraction_eta_fix        (nullptr),
  m_clusters_abs_matchedpvxFraction_eta_notfix     (nullptr),
  m_clusters_abs_matchedpvxFraction_eta_var        (nullptr),
  m_clusters_abs_matchedpvxFraction_eta_notvar     (nullptr),
  m_clusters_abs_matchedpvxFraction_eta_onlyvar    (nullptr),
  m_clusters_abs_matchedpvxFraction_eta_onlyfix    (nullptr),
  m_clusters_abs_matchedpvxFraction_eta_none       (nullptr),
  m_clusters_pt_fraction_e                         (nullptr),
  m_clusters_PV0_pt_fraction_e                     (nullptr),
  m_clusters_PVX_pt_fraction_e                     (nullptr),
  m_clusters_rejected_pt_fraction_e                (nullptr),
  m_clusters_all_neutral_eta                       (nullptr),
  m_clusters_all_neutral_e                         (nullptr),
  m_clusters_all_neutral_PV0_eta                   (nullptr),
  m_clusters_all_neutral_PV0_e                     (nullptr),
  m_clusters_all_neutral_notPV0_eta                (nullptr),
  m_clusters_all_neutral_notPV0_e                  (nullptr),
  m_clusters_all_neutral_neutral_eta               (nullptr),
  m_clusters_all_neutral_neutral_e                 (nullptr),
  m_trk_total_eta                                           (nullptr),
  m_trk_total_pt                                            (nullptr),
  m_trk_total_clusters_eta                                  (nullptr),
  m_trk_total_clusters_pt                                   (nullptr),
  m_trk_notMatching_deltar_fix_eta                          (nullptr),
  m_trk_notMatching_deltar_fix_phi                          (nullptr),
  m_trk_notMatching_deltar_fix_pt                           (nullptr),
  m_trk_notMatching_deltar_fix_caloEntryUncTot_eta          (nullptr),
  m_trk_notMatching_deltar_fix_caloEntryUncTot_pt           (nullptr),
  m_trk_notMatching_deltar_fix_caloEntryUncTot_prodRadius   (nullptr),
  m_trk_notMatching_deltar_var_eta                          (nullptr),
  m_trk_notMatching_deltar_var_phi                          (nullptr),
  m_trk_notMatching_deltar_var_pt                           (nullptr),
  m_trk_notMatching_deltar_var_caloEntryUncTot_eta          (nullptr),
  m_trk_notMatching_deltar_var_caloEntryUncTot_pt           (nullptr),
  m_trk_notMatching_deltar_var_caloEntryUncTot_prodRadius   (nullptr),
  m_trk_notMatching_deltar_none_eta                         (nullptr),
  m_trk_notMatching_deltar_none_phi                         (nullptr),
  m_trk_notMatching_deltar_none_pt                          (nullptr),
  m_trk_notMatching_deltar_none_caloEntryUncTot_eta         (nullptr),
  m_trk_notMatching_deltar_none_caloEntryUncTot_pt          (nullptr),
  m_trk_notMatching_deltar_none_caloEntryUncTot_prodRadius  (nullptr),
  m_trk_matching_deltar_fix_eta                             (nullptr),
  m_trk_matching_deltar_fix_pt                              (nullptr),
  m_trk_matching_deltar_var_eta                             (nullptr),
  m_trk_matching_deltar_var_pt                              (nullptr),
  m_trk_matching_deltar_onlyfix_eta                         (nullptr),
  m_trk_matching_deltar_onlyfix_pt                          (nullptr),
  m_trk_matching_deltar_onlyvar_eta                         (nullptr),
  m_trk_matching_deltar_onlyvar_pt                          (nullptr),
  m_trk_matching_deltar_fix_or_var_eta                      (nullptr),
  m_trk_matching_deltar_fix_or_var_pt                       (nullptr),
  m_trk_matching_deltar_fix_and_var_eta                     (nullptr),
  m_trk_matching_deltar_fix_and_var_pt                      (nullptr),
  m_trk_matching_deltar_or_clusters_eta                     (nullptr),
  m_trk_matching_deltar_or_clusters_phi                     (nullptr),
  m_trk_matching_deltar_or_clusters_pt                      (nullptr),
  m_trk_matching_deltar_and_clusters_eta                    (nullptr),
  m_trk_matching_deltar_and_clusters_phi                    (nullptr),
  m_trk_matching_deltar_and_clusters_pt                     (nullptr),
  m_trk_matching_deltar_fix_only_clusters_eta               (nullptr),
  m_trk_matching_deltar_fix_only_clusters_phi               (nullptr),
  m_trk_matching_deltar_fix_only_clusters_pt                (nullptr),
  m_trk_matching_deltar_var_only_clusters_eta               (nullptr),
  m_trk_matching_deltar_var_only_clusters_phi               (nullptr),
  m_trk_matching_deltar_var_only_clusters_pt                (nullptr),
  m_trk_matchedFraction_eta_fix_and_var                     (nullptr),
  m_trk_matchedFraction_eta_fix_or_var                      (nullptr),
  m_trk_matchedFraction_eta_fix                             (nullptr),
  m_trk_matchedFraction_eta_notfix                          (nullptr),
  m_trk_matchedFraction_eta_var                             (nullptr),
  m_trk_matchedFraction_eta_notvar                          (nullptr),
  m_trk_matchedFraction_eta_onlyvar                         (nullptr),
  m_trk_matchedFraction_eta_onlyfix                         (nullptr),
  m_trk_matchedFraction_eta_none                            (nullptr),
  m_trk_matchedFraction_pt_fix_and_var                      (nullptr),
  m_trk_matchedFraction_pt_fix_or_var                       (nullptr),
  m_trk_matchedFraction_pt_fix                              (nullptr),
  m_trk_matchedFraction_pt_notfix                           (nullptr),
  m_trk_matchedFraction_pt_var                              (nullptr),
  m_trk_matchedFraction_pt_notvar                           (nullptr),
  m_trk_matchedFraction_pt_onlyvar                          (nullptr),
  m_trk_matchedFraction_pt_onlyfix                          (nullptr),
  m_trk_matchedFraction_pt_none                             (nullptr),
  m_trk_pv0_total_eta                                       (nullptr),
  m_trk_pv0_total_pt                                        (nullptr),
  m_trk_pv0_caloEntryUncTot_eta                             (nullptr),
  m_trk_pv0_caloEntryUncTot_pt                              (nullptr),
  m_trk_pv0_caloEntryUncTot_truthpt                         (nullptr), 
  m_trk_pv0_total_clusters_eta                              (nullptr),
  m_trk_pv0_total_clusters_pt                               (nullptr),
  m_trk_pv0_matching_deltar_fix_eta                         (nullptr),
  m_trk_pv0_matching_deltar_fix_pt                          (nullptr),
  m_trk_pv0_matching_deltar_var_eta                         (nullptr),
  m_trk_pv0_matching_deltar_var_pt                          (nullptr),
  m_trk_pv0_notMatching_deltar_fix_eta                      (nullptr),
  m_trk_pv0_notMatching_deltar_fix_pt                       (nullptr),
  m_trk_pv0_notMatching_deltar_var_eta                      (nullptr),
  m_trk_pv0_notMatching_deltar_var_pt                       (nullptr),
  m_trk_pv0_notMatching_deltar_none_eta                     (nullptr),
  m_trk_pv0_notMatching_deltar_none_pt                      (nullptr),
  m_trk_pv0_matching_deltar_fix_and_var_eta                 (nullptr),
  m_trk_pv0_matching_deltar_fix_and_var_pt                  (nullptr),
  m_trk_pv0_matching_deltar_fix_or_var_eta                  (nullptr),
  m_trk_pv0_matching_deltar_fix_or_var_pt                   (nullptr),
  m_trk_pv0_matching_deltar_onlyfix_eta                     (nullptr),
  m_trk_pv0_matching_deltar_onlyfix_pt                      (nullptr),
  m_trk_pv0_matching_deltar_onlyvar_eta                     (nullptr),
  m_trk_pv0_matching_deltar_onlyvar_pt                      (nullptr),
  m_trk_pv0_matchedFraction_eta_fix_and_var                 (nullptr),
  m_trk_pv0_matchedFraction_eta_fix_or_var                  (nullptr),
  m_trk_pv0_matchedFraction_eta_fix                         (nullptr),
  m_trk_pv0_matchedFraction_eta_notfix                      (nullptr),
  m_trk_pv0_matchedFraction_eta_var                         (nullptr),
  m_trk_pv0_matchedFraction_eta_notvar                      (nullptr),
  m_trk_pv0_matchedFraction_eta_onlyvar                     (nullptr),
  m_trk_pv0_matchedFraction_eta_onlyfix                     (nullptr),
  m_trk_pv0_matchedFraction_eta_none                        (nullptr),
  m_trk_pv0_matchedFraction_pt_fix_and_var                  (nullptr),
  m_trk_pv0_matchedFraction_pt_fix_or_var                   (nullptr),
  m_trk_pv0_matchedFraction_pt_fix                          (nullptr),
  m_trk_pv0_matchedFraction_pt_notfix                       (nullptr),
  m_trk_pv0_matchedFraction_pt_var                          (nullptr),
  m_trk_pv0_matchedFraction_pt_notvar                       (nullptr),
  m_trk_pv0_matchedFraction_pt_onlyvar                      (nullptr),
  m_trk_pv0_matchedFraction_pt_onlyfix                      (nullptr),
  m_trk_pv0_matchedFraction_pt_none                         (nullptr),
  m_trk_pv0_caloEntryUncTot_eta_0SiHits                     (nullptr), 
  m_trk_pv0_caloEntryUncTot_pt_0SiHits                      (nullptr),
  m_trk_pv0_caloEntryUncTot_rFirstHit_0SiHits               (nullptr),
  m_trk_pv0_caloEntryUncTot_nSiHits                         (nullptr),
  m_trk_pv0_caloEntryUncTot_rFirstHit                       (nullptr),
  m_tcc_pt                                                  (nullptr),
  m_tcc_pt_etacut                                           (nullptr),
  m_tcc_phi                                                 (nullptr),
  m_tcc_eta                                                 (nullptr),
  m_tcc_eta_ptcut                                           (nullptr),
  m_tcc_m                                                   (nullptr),
  m_tcc_taste                                               (nullptr),
  m_tcc_taste_pt                                            (nullptr),
  m_tcc_taste_pt_etacut                                     (nullptr),
  m_tcc_taste_eta                                           (nullptr),
  m_tcc_taste_eta_ptcut                                     (nullptr),
  m_tcc_pt_truth_pt                                         (nullptr),
  m_tcc_pt_response                                         (nullptr),
  m_tcc_pt_track_pt                                         (nullptr),
  m_tcc_pt_pseudoresponse                                   (nullptr),
  m_tcc_N_M                                                 (nullptr),
  m_tcc_N_M_pv0                                             (nullptr),
  m_trk_tcc_reco_pt_truth_pt                                (nullptr),
  m_trk_tcc_reco_pt_response                                (nullptr) {
}

void TCCPlots::setEventWeight(const float& weight) {
  m_eventWeight = weight;
}

void TCCPlots::setJetPtBinning(const std::vector<float>& bins) {
  m_jetPtBins = bins;
}

void TCCPlots::setJetMassOverPtBinning(const std::vector<float>& bins) {
  m_jetMassOverPtBins = bins;
}

void TCCPlots::setTrackPtBinning(const std::vector<float>& bins) {
  m_trackPtBins = bins;
}

void TCCPlots::setTrackProdRadiusBinning(const std::vector<float>& bins) {
  m_trackProdRadiusBins = bins;
}

void TCCPlots::initializePlots() {
  
  if (m_collectionType!="")
    std::cout << "INFO: TCCPlots configured for " << m_collectionType << " type collection" << std::endl;    
  
  if (m_collectionType == "jets") {
    book(m_jet_n                                     , "jet_n"                                    );
    book(m_jet_pt                                    , "jet_pt"                                   );
    book(m_jet_eta                                   , "jet_eta"                                  );
    book(m_jet_phi                                   , "jet_phi"                                  );
    book(m_jet_m                                     , "jet_m"                                    );
    book(m_jet_m_2leadings                           , "jet_m_2leadings"                          );
    book(m_jet_pt_2leadings                          , "jet_pt_2leadings"                         );
    book(m_jet_eta_2leadings                         , "jet_eta_2leadings"                        );
    book(m_jet_phi_2leadings                         , "jet_phi_2leadings"                        );
    book(m_jet_m_leading                             , "jet_m_leading"                            );
    book(m_jet_pt_leading                            , "jet_pt_leading"                           );
    book(m_jet_eta_leading                           , "jet_eta_leading"                          );
    book(m_jet_phi_leading                           , "jet_phi_leading"                          );
    book(m_jet_m_subleading                          , "jet_m_subleading"                         );
    book(m_jet_pt_subleading                         , "jet_pt_subleading"                        );
    book(m_jet_eta_subleading                        , "jet_eta_subleading"                       );
    book(m_jet_phi_subleading                        , "jet_phi_subleading"                       );
    book(m_jet_deltaR                                , "jet_deltaR"                               );
    book(m_jet_deltaR_matched                        , "jet_deltaR_matched"                       );
    book(m_jet_response_m                            , "jet_response_m"                           );
    book(m_jet_response_m_pt                         , "jet_response_m_pt"                        );
    book(m_jet_response_pt                           , "jet_response_pt"                          );
    book(m_jet_response_m_2leadings                  , "jet_response_m_2leadings"                 );
    book(m_jet_response_m_pt_2leadings               , "jet_response_m_pt_2leadings"              );
    book(m_jet_response_pt_2leadings                 , "jet_response_pt_2leadings"                );
    book(m_jet_response_m_leading                    , "jet_response_m_leading"                   );
    book(m_jet_response_m_pt_leading                 , "jet_response_m_pt_leading"                );
    book(m_jet_response_pt_leading                   , "jet_response_pt_leading"                  );
    book(m_jet_response_m_subleading                 , "jet_response_m_subleading"                );
    book(m_jet_response_m_pt_subleading              , "jet_response_m_pt_subleading"             );
    book(m_jet_response_pt_subleading                , "jet_response_pt_subleading"               );
    book(m_jet_response_eta                          , "jet_response_eta"                         );
    book(m_jet_response_phi                          , "jet_response_phi"                         );
    book(m_jet_response_eta_2leadings                , "jet_response_eta_2leadings"               );
    book(m_jet_response_phi_2leadings                , "jet_response_phi_2leadings"               );
    book(m_jet_response_eta_leading                  , "jet_response_eta_leading"                 );
    book(m_jet_response_phi_leading                  , "jet_response_phi_leading"                 );
    book(m_jet_response_eta_subleading               , "jet_response_eta_subleading"              );
    book(m_jet_response_phi_subleading               , "jet_response_phi_subleading"              );
    book(m_jet_response_add_eta                      , "jet_response_add_eta"                     );
    book(m_jet_response_add_eta_2leadings            , "jet_response_add_eta_2leadings"           );
    book(m_jet_response_add_eta_leading              , "jet_response_add_eta_leading"             );
    book(m_jet_response_add_eta_subleading           , "jet_response_add_eta_subleading"          );
    book(m_jet_response_add_eta_pt                   , "jet_response_add_eta_pt"                  );
    book(m_jet_response_add_eta_pt_2leadings         , "jet_response_add_eta_pt_2leadings"        );
    book(m_jet_response_add_eta_pt_leading           , "jet_response_add_eta_pt_leading"          );
    book(m_jet_response_add_eta_pt_subleading        , "jet_response_add_eta_pt_subleading"       );
    book(m_jet_response_m_npv                        , "jet_response_m_npv"                       );
    book(m_jet_response_m_npv_2leadings              , "jet_response_m_npv_2leadings"             );
    book(m_jet_response_m_npv_leading                , "jet_response_m_npv_leading"               );
    book(m_jet_response_m_npv_subleading             , "jet_response_m_npv_subleading"            );
    book(m_jet_response_d2_pt                        , "jet_response_d2_pt"                       );
    book(m_jet_response_d2_pt_2leadings              , "jet_response_d2_pt_2leadings"             );
    book(m_jet_response_d2_pt_leading                , "jet_response_d2_pt_leading"               );
    book(m_jet_response_d2_pt_subleading             , "jet_response_d2_pt_subleading"            );
    book(m_jet_response_add_d2_pt                    , "jet_response_add_d2_pt"                   );
    book(m_jet_response_add_d2_pt_2leadings          , "jet_response_add_d2_pt_2leadings"         );
    book(m_jet_response_add_d2_pt_leading            , "jet_response_add_d2_pt_leading"           );
    book(m_jet_response_add_d2_pt_subleading         , "jet_response_add_d2_pt_subleading"        );
    book(m_jet_pseudoresponse_m                      , "jet_pseudoresponse_m"                     );
    book(m_jet_pseudoresponse_pt                     , "jet_pseudoresponse_pt"                    );
    book(m_jet_pseudoresponse_m_2leadings            , "jet_pseudoresponse_m_2leadings"           );
    book(m_jet_pseudoresponse_pt_2leadings           , "jet_pseudoresponse_pt_2leadings"          );
    book(m_jet_pseudoresponse_m_leading              , "jet_pseudoresponse_m_leading"             );
    book(m_jet_pseudoresponse_pt_leading             , "jet_pseudoresponse_pt_leading"            );
    book(m_jet_pseudoresponse_m_subleading           , "jet_pseudoresponse_m_subleading"          );
    book(m_jet_pseudoresponse_pt_subleading          , "jet_pseudoresponse_pt_subleading"         );
    book(m_jet_pseudoresponse_notcalib_pt            , "jet_pseudoresponse_notcalib_pt"           );
    book(m_jet_pseudoresponse_notcalib_pt_2leadings  , "jet_pseudoresponse_notcalib_pt_2leadings" );
    book(m_jet_pseudoresponse_notcalib_pt_leading    , "jet_pseudoresponse_notcalib_pt_leading"   );
    book(m_jet_pseudoresponse_notcalib_pt_subleading , "jet_pseudoresponse_notcalib_pt_subleading");
    book(m_jet_d2                                    , "jet_d2"                                   );
    book(m_jet_d2_2leadings                          , "jet_d2_2leadings"                         );
    book(m_jet_d2_leading                            , "jet_d2_leading"                           );
    book(m_jet_d2_subleading                         , "jet_d2_subleading"                        );
    book(m_jet_d2_massCut                            , "jet_d2_massCut"                           );
    book(m_jet_d2_2leadings_massCut                  , "jet_d2_2leadings_massCut"                 );
    book(m_jet_d2_leading_massCut                    , "jet_d2_leading_massCut"                   );
    book(m_jet_d2_subleading_massCut                 , "jet_d2_subleading_massCut"                );
    book(m_jet_response_d2                           , "jet_response_d2"                          );
    book(m_jet_response_d2_2leadings                 , "jet_response_d2_2leadings"                );
    book(m_jet_response_d2_leading                   , "jet_response_d2_leading"                  );
    book(m_jet_response_d2_subleading                , "jet_response_d2_subleading"               ); 
    book(m_jet_response_add_d2                       , "jet_response_add_d2"                      );
    book(m_jet_response_add_d2_2leadings             , "jet_response_add_d2_2leadings"            );
    book(m_jet_response_add_d2_leading               , "jet_response_add_d2_leading"              );
    book(m_jet_response_add_d2_subleading            , "jet_response_add_d2_subleading"           ); 
    book(m_jet_resolution_m                          , "jet_resolution_m"                         );
    book(m_jet_resolution_m_2leadings                , "jet_resolution_m_2leadings"               );
    book(m_jet_resolution_m_leading                  , "jet_resolution_m_leading"                 );
    book(m_jet_resolution_m_subleading               , "jet_resolution_m_subleading"              );
    book(m_jet_resolution_m_npv                      , "jet_resolution_m_npv"                     );
    book(m_jet_resolution_m_npv_2leadings            , "jet_resolution_m_npv_2leadings"           );
    book(m_jet_resolution_m_npv_leading              , "jet_resolution_m_npv_leading"             );
    book(m_jet_resolution_m_npv_subleading           , "jet_resolution_m_npv_subleading"          );
    book(m_jet_resolution_d2                         , "jet_resolution_d2"                        );
    book(m_jet_resolution_d2_2leadings               , "jet_resolution_d2_2leadings"              );
    book(m_jet_resolution_d2_leading                 , "jet_resolution_d2_leading"                );
    book(m_jet_resolution_d2_subleading              , "jet_resolution_d2_subleading"             );
    book(m_jet_resolution_add_d2                     , "jet_resolution_add_d2"                    );
    book(m_jet_resolution_add_d2_2leadings           , "jet_resolution_add_d2_2leadings"          );
    book(m_jet_resolution_add_d2_leading             , "jet_resolution_add_d2_leading"            );
    book(m_jet_resolution_add_d2_subleading          , "jet_resolution_add_d2_subleading"         );
    book(m_jet_median_width_m_npv                    , "jet_median_width_m_npv"                   );
    book(m_jet_median_width_m_npv_2leadings          , "jet_median_width_m_npv_2leadings"         );
    book(m_jet_median_width_m_npv_leading            , "jet_median_width_m_npv_leading"           );
    book(m_jet_median_width_m_npv_subleading         , "jet_median_width_m_npv_subleading"        );
    book(m_jet_mopt_pt_response_m                    , "jet_mopt_pt_response_m"                   );
    book(m_jet_mopt_pt_response_m_2leadings          , "jet_mopt_pt_response_m_2leadings"         );
    book(m_jet_mopt_pt_response_m_leading            , "jet_mopt_pt_response_m_leading"           );
    book(m_jet_mopt_pt_response_m_subleading         , "jet_mopt_pt_response_m_subleading"        );
    book(m_jet_mopt_pt_response_pt                   , "jet_mopt_pt_response_pt"                  );
    book(m_jet_mopt_pt_response_pt_2leadings         , "jet_mopt_pt_response_pt_2leadings"        );
    book(m_jet_mopt_pt_response_pt_leading           , "jet_mopt_pt_response_pt_leading"          );
    book(m_jet_mopt_pt_response_pt_subleading        , "jet_mopt_pt_response_pt_subleading"       );
    book(m_jet_mopt_pt_response_phi                  , "jet_mopt_pt_response_phi"                 );
    book(m_jet_mopt_pt_response_phi_2leadings        , "jet_mopt_pt_response_phi_2leadings"       );
    book(m_jet_mopt_pt_response_phi_leading          , "jet_mopt_pt_response_phi_leading"         );
    book(m_jet_mopt_pt_response_phi_subleading       , "jet_mopt_pt_response_phi_subleading"      );
    book(m_jet_mopt_pt_response_eta                  , "jet_mopt_pt_response_eta"                 );
    book(m_jet_mopt_pt_response_eta_2leadings        , "jet_mopt_pt_response_eta_2leadings"       );
    book(m_jet_mopt_pt_response_eta_leading          , "jet_mopt_pt_response_eta_leading"         );
    book(m_jet_mopt_pt_response_eta_subleading       , "jet_mopt_pt_response_eta_subleading"      );
    book(m_jet_mopt_pt_response_d2                   , "jet_mopt_pt_response_d2"                  );
    book(m_jet_mopt_pt_response_d2_2leadings         , "jet_mopt_pt_response_d2_2leadings"        );
    book(m_jet_mopt_pt_response_d2_leading           , "jet_mopt_pt_response_d2_leading"          );
    book(m_jet_mopt_pt_response_d2_subleading        , "jet_mopt_pt_response_d2_subleading"       ); 
    book(m_jet_resolution_mopt_pt_m                  , "jet_resolution_mopt_pt_m"                 );
    book(m_jet_resolution_mopt_pt_m_2leadings        , "jet_resolution_mopt_pt_m_2leadings"       );
    book(m_jet_resolution_mopt_pt_m_leading          , "jet_resolution_mopt_pt_m_leading"         );
    book(m_jet_resolution_mopt_pt_m_subleading       , "jet_resolution_mopt_pt_m_subleading"      );
    book(m_jet_resolution_mopt_pt_pt                 , "jet_resolution_mopt_pt_pt"                );
    book(m_jet_resolution_mopt_pt_pt_2leadings       , "jet_resolution_mopt_pt_pt_2leadings"      );
    book(m_jet_resolution_mopt_pt_pt_leading         , "jet_resolution_mopt_pt_pt_leading"        );
    book(m_jet_resolution_mopt_pt_pt_subleading      , "jet_resolution_mopt_pt_pt_subleading"     );
    book(m_jet_resolution_mopt_pt_phi                , "jet_resolution_mopt_pt_phi"               );
    book(m_jet_resolution_mopt_pt_phi_2leadings      , "jet_resolution_mopt_pt_phi_2leadings"     );
    book(m_jet_resolution_mopt_pt_phi_leading        , "jet_resolution_mopt_pt_phi_leading"       );
    book(m_jet_resolution_mopt_pt_phi_subleading     , "jet_resolution_mopt_pt_phi_subleading"    );
    book(m_jet_resolution_mopt_pt_eta                , "jet_resolution_mopt_pt_eta"               );
    book(m_jet_resolution_mopt_pt_eta_2leadings      , "jet_resolution_mopt_pt_eta_2leadings"     );
    book(m_jet_resolution_mopt_pt_eta_leading        , "jet_resolution_mopt_pt_eta_leading"       );
    book(m_jet_resolution_mopt_pt_eta_subleading     , "jet_resolution_mopt_pt_eta_subleading"    );
    book(m_jet_resolution_mopt_pt_d2                 , "jet_resolution_mopt_pt_d2"                );
    book(m_jet_resolution_mopt_pt_d2_2leadings       , "jet_resolution_mopt_pt_d2_2leadings"      );
    book(m_jet_resolution_mopt_pt_d2_leading         , "jet_resolution_mopt_pt_d2_leading"        );
    book(m_jet_resolution_mopt_pt_d2_subleading      , "jet_resolution_mopt_pt_d2_subleading"     );     
 
  } else if (m_collectionType == "tracks") {
   // book histograms for tracks 
    book(m_trk_d0                           , "trk_d0"                           );
    book(m_trk_z0                           , "trk_z0"                           );
    book(m_trk_theta                        , "trk_theta"                        );
    book(m_trk_phi                          , "trk_phi"                          );
    book(m_trk_qOverP                       , "trk_qOverP"                       );
    book(m_trk_d0_eta                       , "trk_d0_eta"                       );
    book(m_trk_z0_eta                       , "trk_z0_eta"                       );
    book(m_trk_theta_eta                    , "trk_theta_eta"                    );
    book(m_trk_phi_eta                      , "trk_phi_eta"                      );
    book(m_trk_qOverP_eta                   , "trk_qOverP_eta"                   );
    book(m_trk_d0_pt                        , "trk_d0_pt"                        );
    book(m_trk_z0_pt                        , "trk_z0_pt"                        );
    book(m_trk_theta_pt                     , "trk_theta_pt"                     );
    book(m_trk_phi_pt                       , "trk_phi_pt"                       );
    book(m_trk_qOverP_pt                    , "trk_qOverP_pt"                    );
    book(m_trk_caloEntryEta                 , "trk_caloEntryEta"                 );
    book(m_trk_caloEntryEtaOverEta_eta      , "trk_caloEntryEtaOverEta_eta"      );
    book(m_trk_caloEntryEtaOverEta_pt       , "trk_caloEntryEtaOverEta_pt"       );
    book(m_trk_caloEntryUncEta_caloEntryEta , "trk_caloEntryUncEta_caloEntryEta" );
    book(m_trk_caloEntryUncEta_eta          , "trk_caloEntryUncEta_eta"          );
    book(m_trk_caloEntryUncEta_pt           , "trk_caloEntryUncEta_pt"           );
    book(m_trk_caloEntryUncTheta_eta        , "trk_caloEntryUncTheta_eta"        );
    book(m_trk_caloEntryUncTheta_pt         , "trk_caloEntryUncTheta_pt"         );
    book(m_trk_caloEntryPhi                 , "trk_caloEntryPhi"                 );
    book(m_trk_caloEntryPhiOverPhi_eta      , "trk_caloEntryPhiOverPhi_eta"      );
    book(m_trk_caloEntryPhiOverPhi_pt       , "trk_caloEntryPhiOverPhi_pt"       );
    book(m_trk_caloEntryUncPhi_caloEntryPhi , "trk_caloEntryUncPhi_caloEntryPhi" );
    book(m_trk_caloEntryUncPhi_eta          , "trk_caloEntryUncPhi_eta"          );
    book(m_trk_caloEntryUncPhi_pt           , "trk_caloEntryUncPhi_pt"           );
    book(m_trk_caloEntryUncTot_eta          , "trk_caloEntryUncTot_eta"          );
    book(m_trk_caloEntryUncTot_pt           , "trk_caloEntryUncTot_pt"           );
    book(m_trk_perigeeEta                   , "trk_perigeeEta"                   );
    book(m_trk_perigeeUncEta_eta            , "trk_perigeeUncEta_eta"            );
    book(m_trk_perigeeUncEta_pt             , "trk_perigeeUncEta_pt"             );
    book(m_trk_perigeeUncTheta_eta          , "trk_perigeeUncTheta_eta"          );
    book(m_trk_perigeeUncTheta_pt           , "trk_perigeeUncTheta_pt"           );
    book(m_trk_perigeePhi                   , "trk_perigeePhi"                   );
    book(m_trk_perigeeUncPhi_eta            , "trk_perigeeUncPhi_eta"            );
    book(m_trk_perigeeUncPhi_pt             , "trk_perigeeUncPhi_pt"             );
    book(m_trk_perigeeUncTot_eta            , "trk_perigeeUncTot_eta"            );
    book(m_trk_perigeeUncTot_pt             , "trk_perigeeUncTot_pt"             );
    book(m_trk_perigeeUncEta_caloUncEta     , "trk_perigeeUncEta_caloUncEta"     );
    book(m_trk_perigeeUncTheta_caloUncTheta , "trk_perigeeUncTheta_caloUncTheta" );
    book(m_trk_perigeeUncPhi_caloUncPhi     , "trk_perigeeUncPhi_caloUncPhi"     );
    book(m_trk_perigeeUncTot_caloUncTot     , "trk_perigeeUncTot_caloUncTot"     );
    book(m_trk_prodRadius                   , "trk_prodRadius"                   );
    book(m_trk_perigeeUncEta_prodRadius     , "trk_perigeeUncEta_prodRadius"     ); 
    book(m_trk_perigeeUncTheta_prodRadius   , "trk_perigeeUncTheta_prodRadius"   ); 
    book(m_trk_perigeeUncPhi_prodRadius     , "trk_perigeeUncPhi_prodRadius"     );
    book(m_trk_perigeeUncTot_prodRadius     , "trk_perigeeUncTot_prodRadius"     );
    book(m_trk_caloEntryUncEta_prodRadius   , "trk_caloEntryUncEta_prodRadius"   );
    book(m_trk_caloEntryUncTheta_prodRadius , "trk_caloEntryUncTheta_prodRadius" );
    book(m_trk_caloEntryUncPhi_prodRadius   , "trk_caloEntryUncPhi_prodRadius"   );
    book(m_trk_caloEntryUncTot_prodRadius   , "trk_caloEntryUncTot_prodRadius"   );
    book(m_trk_delta_perigeeEta_caloEntryEta, "trk_delta_perigeeEta_caloEntryEta");
    book(m_trk_delta_perigeePhi_caloEntryPhi, "trk_delta_perigeePhi_caloEntryPhi");
    book(m_trk_delta_perigeeEta_caloEntryEta_eta    , "trk_delta_perigeeEta_caloEntryEta_eta"   );   
    book(m_trk_delta_perigeePhi_caloEntryPhi_eta    , "trk_delta_perigeePhi_caloEntryPhi_eta"   );   
    book(m_trk_delta_perigeeEta_caloEntryEta_pt     , "trk_delta_perigeeEta_caloEntryEta_pt"    );    
    book(m_trk_delta_perigeePhi_caloEntryPhi_pt     , "trk_delta_perigeePhi_caloEntryPhi_pt"    ); 
    book(m_trk_delta_trackEta_caloEntryEta          , "trk_delta_trackEta_caloEntryEta"         );
    book(m_trk_delta_trackPhi_caloEntryPhi          , "trk_delta_trackPhi_caloEntryPhi"         );
    book(m_trk_delta_trackEta_caloEntryEta_eta      , "trk_delta_trackEta_caloEntryEta_eta"     );
    book(m_trk_delta_trackPhi_caloEntryPhi_eta      , "trk_delta_trackPhi_caloEntryPhi_eta"     );
    book(m_trk_delta_trackEta_caloEntryEta_pt       , "trk_delta_trackEta_caloEntryEta_pt"      );
    book(m_trk_delta_trackPhi_caloEntryPhi_pt       , "trk_delta_trackPhi_caloEntryPhi_pt"      );
    book(m_trk_delta_trackEta_caloEntryEtaCorr      , "trk_delta_trackEta_caloEntryEtaCorr"     );
    book(m_trk_delta_trackPhi_caloEntryPhiCorr      , "trk_delta_trackPhi_caloEntryPhiCorr"     );
    book(m_trk_delta_trackEta_caloEntryEtaCorr_eta  , "trk_delta_trackEta_caloEntryEtaCorr_eta" );
    book(m_trk_delta_trackPhi_caloEntryPhiCorr_eta  , "trk_delta_trackPhi_caloEntryPhiCorr_eta" );
    book(m_trk_delta_trackEta_caloEntryEtaCorr_pt   , "trk_delta_trackEta_caloEntryEtaCorr_pt"  );
    book(m_trk_delta_trackPhi_caloEntryPhiCorr_pt   , "trk_delta_trackPhi_caloEntryPhiCorr_pt"  );
    book(m_trk_delta_caloEntryEta_caloEntryEtaCorr      , "trk_delta_caloEntryEta_caloEntryEtaCorr"     );
    book(m_trk_delta_caloEntryPhi_caloEntryPhiCorr      , "trk_delta_caloEntryPhi_caloEntryPhiCorr"     );
    book(m_trk_delta_caloEntryEta_caloEntryEtaCorr_eta  , "trk_delta_caloEntryEta_caloEntryEtaCorr_eta" );
    book(m_trk_delta_caloEntryPhi_caloEntryPhiCorr_eta  , "trk_delta_caloEntryPhi_caloEntryPhiCorr_eta" );
    book(m_trk_delta_caloEntryEta_caloEntryEtaCorr_pt   , "trk_delta_caloEntryEta_caloEntryEtaCorr_pt"  );
    book(m_trk_delta_caloEntryPhi_caloEntryPhiCorr_pt   , "trk_delta_caloEntryPhi_caloEntryPhiCorr_pt"  );
        
    book(m_trk_total_eta                                          , "trk_total_eta"                                         );
    book(m_trk_total_pt                                           , "trk_total_pt"                                          );
    book(m_trk_total_clusters_eta                                 , "trk_total_clusters_eta"                                );
    book(m_trk_total_clusters_pt                                  , "trk_total_clusters_pt"                                 );
    book(m_trk_notMatching_deltar_fix_eta                         , "trk_notMatching_deltar_fix_eta"                        );
    book(m_trk_notMatching_deltar_fix_phi                         , "trk_notMatching_deltar_fix_phi"                        );
    book(m_trk_notMatching_deltar_fix_pt                          , "trk_notMatching_deltar_fix_pt"                         );
    book(m_trk_notMatching_deltar_fix_caloEntryUncTot_eta         , "trk_notMatching_deltar_fix_caloEntryUncTot_eta"        );
    book(m_trk_notMatching_deltar_fix_caloEntryUncTot_pt          , "trk_notMatching_deltar_fix_caloEntryUncTot_pt"         );
    book(m_trk_notMatching_deltar_fix_caloEntryUncTot_prodRadius  , "trk_notMatching_deltar_fix_caloEntryUncTot_prodRadius" );
    book(m_trk_notMatching_deltar_var_eta                         , "trk_notMatching_deltar_var_eta"                        );
    book(m_trk_notMatching_deltar_var_phi                         , "trk_notMatching_deltar_var_phi"                        );
    book(m_trk_notMatching_deltar_var_pt                          , "trk_notMatching_deltar_var_pt"                         );
    book(m_trk_notMatching_deltar_var_caloEntryUncTot_eta         , "trk_notMatching_deltar_var_caloEntryUncTot_eta"        );
    book(m_trk_notMatching_deltar_var_caloEntryUncTot_pt          , "trk_notMatching_deltar_var_caloEntryUncTot_pt"         );
    book(m_trk_notMatching_deltar_var_caloEntryUncTot_prodRadius  , "trk_notMatching_deltar_var_caloEntryUncTot_prodRadius" );
    book(m_trk_notMatching_deltar_none_eta                        , "trk_notMatching_deltar_none_eta"                       );
    book(m_trk_notMatching_deltar_none_phi                        , "trk_notMatching_deltar_none_phi"                       );
    book(m_trk_notMatching_deltar_none_pt                         , "trk_notMatching_deltar_none_pt"                        );
    book(m_trk_notMatching_deltar_none_caloEntryUncTot_eta        , "trk_notMatching_deltar_none_caloEntryUncTot_eta"       );
    book(m_trk_notMatching_deltar_none_caloEntryUncTot_pt         , "trk_notMatching_deltar_none_caloEntryUncTot_pt"        );
    book(m_trk_notMatching_deltar_none_caloEntryUncTot_prodRadius , "trk_notMatching_deltar_none_caloEntryUncTot_prodRadius");
    book(m_trk_matching_deltar_fix_eta                            , "trk_matching_deltar_fix_eta"                           );
    book(m_trk_matching_deltar_fix_pt                             , "trk_matching_deltar_fix_pt"                            );
    book(m_trk_matching_deltar_var_eta                            , "trk_matching_deltar_var_eta"                           );
    book(m_trk_matching_deltar_var_pt                             , "trk_matching_deltar_var_pt"                            );
    book(m_trk_matching_deltar_onlyfix_eta                        , "trk_matching_deltar_onlyfix_eta"                       );
    book(m_trk_matching_deltar_onlyfix_pt                         , "trk_matching_deltar_onlyfix_pt"                        );
    book(m_trk_matching_deltar_onlyvar_eta                        , "trk_matching_deltar_onlyvar_eta"                       );
    book(m_trk_matching_deltar_onlyvar_pt                         , "trk_matching_deltar_onlyvar_pt"                        );
    book(m_trk_matching_deltar_fix_or_var_eta                     , "trk_matching_deltar_fix_or_var_eta"                    );
    book(m_trk_matching_deltar_fix_or_var_pt                      , "trk_matching_deltar_fix_or_var_pt"                     );
    book(m_trk_matching_deltar_fix_and_var_eta                    , "trk_matching_deltar_fix_and_var_eta"                   );
    book(m_trk_matching_deltar_fix_and_var_pt                     , "trk_matching_deltar_fix_and_var_pt"                    );
    book(m_trk_matching_deltar_or_clusters_eta                    , "trk_matching_deltar_or_clusters_eta"                   );
    book(m_trk_matching_deltar_or_clusters_phi                    , "trk_matching_deltar_or_clusters_phi"                   );
    book(m_trk_matching_deltar_or_clusters_pt                     , "trk_matching_deltar_or_clusters_pt"                    );
    book(m_trk_matching_deltar_and_clusters_eta                   , "trk_matching_deltar_and_clusters_eta"                  );
    book(m_trk_matching_deltar_and_clusters_phi                   , "trk_matching_deltar_and_clusters_phi"                  );
    book(m_trk_matching_deltar_and_clusters_pt                    , "trk_matching_deltar_and_clusters_pt"                   );
    book(m_trk_matching_deltar_fix_only_clusters_eta              , "trk_matching_deltar_fix_only_clusters_eta"             );
    book(m_trk_matching_deltar_fix_only_clusters_phi              , "trk_matching_deltar_fix_only_clusters_phi"             );
    book(m_trk_matching_deltar_fix_only_clusters_pt               , "trk_matching_deltar_fix_only_clusters_pt"              );
    book(m_trk_matching_deltar_var_only_clusters_eta              , "trk_matching_deltar_var_only_clusters_eta"             );
    book(m_trk_matching_deltar_var_only_clusters_phi              , "trk_matching_deltar_var_only_clusters_phi"             );
    book(m_trk_matching_deltar_var_only_clusters_pt               , "trk_matching_deltar_var_only_clusters_pt"              );
    book(m_trk_matchedFraction_eta_fix_and_var                    , "trk_matchedFraction_eta_fix_and_var"                   );
    book(m_trk_matchedFraction_eta_fix_or_var                     , "trk_matchedFraction_eta_fix_or_var"                    );
    book(m_trk_matchedFraction_eta_fix                            , "trk_matchedFraction_eta_fix"                           );
    book(m_trk_matchedFraction_eta_notfix                         , "trk_matchedFraction_eta_notfix"                        );
    book(m_trk_matchedFraction_eta_var                            , "trk_matchedFraction_eta_var"                           );
    book(m_trk_matchedFraction_eta_notvar                         , "trk_matchedFraction_eta_notvar"                        );
    book(m_trk_matchedFraction_eta_onlyvar                        , "trk_matchedFraction_eta_onlyvar"                       );
    book(m_trk_matchedFraction_eta_onlyfix                        , "trk_matchedFraction_eta_onlyfix"                       );
    book(m_trk_matchedFraction_eta_none                           , "trk_matchedFraction_eta_none"                          );
    book(m_trk_matchedFraction_pt_fix_and_var                     , "trk_matchedFraction_pt_fix_and_var"                    );
    book(m_trk_matchedFraction_pt_fix_or_var                      , "trk_matchedFraction_pt_fix_or_var"                     );
    book(m_trk_matchedFraction_pt_fix                             , "trk_matchedFraction_pt_fix"                            );
    book(m_trk_matchedFraction_pt_notfix                          , "trk_matchedFraction_pt_notfix"                         );
    book(m_trk_matchedFraction_pt_var                             , "trk_matchedFraction_pt_var"                            );
    book(m_trk_matchedFraction_pt_notvar                          , "trk_matchedFraction_pt_notvar"                         );
    book(m_trk_matchedFraction_pt_onlyvar                         , "trk_matchedFraction_pt_onlyvar"                        );
    book(m_trk_matchedFraction_pt_onlyfix                         , "trk_matchedFraction_pt_onlyfix"                        );
    book(m_trk_matchedFraction_pt_none                            , "trk_matchedFraction_pt_none"                           );
        
    book(m_trk_pv0_total_eta                                      , "trk_pv0_total_eta"                           );
    book(m_trk_pv0_total_pt                                       , "trk_pv0_total_pt"                            );
    book(m_trk_pv0_caloEntryUncTot_eta                            , "trk_pv0_caloEntryUncTot_eta"                 );
    book(m_trk_pv0_caloEntryUncTot_pt                             , "trk_pv0_caloEntryUncTot_pt"                  );
    book(m_trk_pv0_caloEntryUncTot_truthpt                        , "trk_pv0_caloEntryUncTot_truthpt"             );
    book(m_trk_pv0_total_clusters_eta                             , "trk_pv0_total_clusters_eta"                  );
    book(m_trk_pv0_total_clusters_pt                              , "trk_pv0_total_clusters_pt"                   );
    book(m_trk_pv0_matching_deltar_fix_eta                        , "trk_pv0_matching_deltar_fix_eta"             );
    book(m_trk_pv0_matching_deltar_fix_pt                         , "trk_pv0_matching_deltar_fix_pt"              );
    book(m_trk_pv0_matching_deltar_var_eta                        , "trk_pv0_matching_deltar_var_eta"             );
    book(m_trk_pv0_matching_deltar_var_pt                         , "trk_pv0_matching_deltar_var_pt"              );
    book(m_trk_pv0_notMatching_deltar_fix_eta                     , "trk_pv0_notMatching_deltar_fix_eta"          );
    book(m_trk_pv0_notMatching_deltar_fix_pt                      , "trk_pv0_notMatching_deltar_fix_pt"           );
    book(m_trk_pv0_notMatching_deltar_var_eta                     , "trk_pv0_notMatching_deltar_var_eta"          );
    book(m_trk_pv0_notMatching_deltar_var_pt                      , "trk_pv0_notMatching_deltar_var_pt"           );
    book(m_trk_pv0_notMatching_deltar_none_eta                    , "trk_pv0_notMatching_deltar_none_eta"         );
    book(m_trk_pv0_notMatching_deltar_none_pt                     , "trk_pv0_notMatching_deltar_none_pt"          );
    book(m_trk_pv0_matching_deltar_fix_and_var_eta                , "trk_pv0_matching_deltar_fix_and_var_eta"     );
    book(m_trk_pv0_matching_deltar_fix_and_var_pt                 , "trk_pv0_matching_deltar_fix_and_var_pt"      );
    book(m_trk_pv0_matching_deltar_fix_or_var_eta                 , "trk_pv0_matching_deltar_fix_or_var_eta"      );
    book(m_trk_pv0_matching_deltar_fix_or_var_pt                  , "trk_pv0_matching_deltar_fix_or_var_pt"       );
    book(m_trk_pv0_matching_deltar_onlyfix_eta                    , "trk_pv0_matching_deltar_onlyfix_eta"         );
    book(m_trk_pv0_matching_deltar_onlyfix_pt                     , "trk_pv0_matching_deltar_onlyfix_pt"          );
    book(m_trk_pv0_matching_deltar_onlyvar_eta                    , "trk_pv0_matching_deltar_onlyvar_eta"         );
    book(m_trk_pv0_matching_deltar_onlyvar_pt                     , "trk_pv0_matching_deltar_onlyvar_pt"          );
    book(m_trk_pv0_matchedFraction_eta_fix_and_var                , "trk_pv0_matchedFraction_eta_fix_and_var"     );
    book(m_trk_pv0_matchedFraction_eta_fix_or_var                 , "trk_pv0_matchedFraction_eta_fix_or_var"      );
    book(m_trk_pv0_matchedFraction_eta_fix                        , "trk_pv0_matchedFraction_eta_fix"             );
    book(m_trk_pv0_matchedFraction_eta_notfix                     , "trk_pv0_matchedFraction_eta_notfix"          );
    book(m_trk_pv0_matchedFraction_eta_var                        , "trk_pv0_matchedFraction_eta_var"             );
    book(m_trk_pv0_matchedFraction_eta_notvar                     , "trk_pv0_matchedFraction_eta_notvar"          );
    book(m_trk_pv0_matchedFraction_eta_onlyvar                    , "trk_pv0_matchedFraction_eta_onlyvar"         );
    book(m_trk_pv0_matchedFraction_eta_onlyfix                    , "trk_pv0_matchedFraction_eta_onlyfix"         );
    book(m_trk_pv0_matchedFraction_eta_none                       , "trk_pv0_matchedFraction_eta_none"            );
    book(m_trk_pv0_matchedFraction_pt_fix_and_var                 , "trk_pv0_matchedFraction_pt_fix_and_var"      );
    book(m_trk_pv0_matchedFraction_pt_fix_or_var                  , "trk_pv0_matchedFraction_pt_fix_or_var"       );
    book(m_trk_pv0_matchedFraction_pt_fix                         , "trk_pv0_matchedFraction_pt_fix"              );
    book(m_trk_pv0_matchedFraction_pt_notfix                      , "trk_pv0_matchedFraction_pt_notfix"           );
    book(m_trk_pv0_matchedFraction_pt_var                         , "trk_pv0_matchedFraction_pt_var"              );
    book(m_trk_pv0_matchedFraction_pt_notvar                      , "trk_pv0_matchedFraction_pt_notvar"           );
    book(m_trk_pv0_matchedFraction_pt_onlyvar                     , "trk_pv0_matchedFraction_pt_onlyvar"          );
    book(m_trk_pv0_matchedFraction_pt_onlyfix                     , "trk_pv0_matchedFraction_pt_onlyfix"          );
    book(m_trk_pv0_matchedFraction_pt_none                        , "trk_pv0_matchedFraction_pt_none"             );
    
    book(m_trk_pv0_caloEntryUncTot_eta_0SiHits                    , "trk_pv0_caloEntryUncTot_eta_0SiHits"         );
    book(m_trk_pv0_caloEntryUncTot_pt_0SiHits                     , "trk_pv0_caloEntryUncTot_pt_0SiHits"          );
    book(m_trk_pv0_caloEntryUncTot_rFirstHit_0SiHits              , "trk_pv0_caloEntryUncTot_rFirstHit_0SiHits"   );
    book(m_trk_pv0_caloEntryUncTot_nSiHits                        , "trk_pv0_caloEntryUncTot_nSiHits"             );
    book(m_trk_pv0_caloEntryUncTot_rFirstHit                      , "trk_pv0_caloEntryUncTot_rFirstHit"           );        
        
  } else if (m_collectionType == "clusters") {
    
    book(m_clusters_eta                            , "clusters_eta"                   );
    book(m_clusters_matched_eta                    , "clusters_matched_eta"           );
    book(m_clusters_notMatched_eta                 , "clusters_notMatched_eta"        );
    book(m_clusters_matchedFraction_eta            , "clusters_matchedFraction_eta"   );
    book(m_clusters_notMatchedFraction_eta         , "clusters_notMatchedFraction_eta");
    book(m_clusters_matchedFraction_energy         , "clusters_matchedFraction_energy"   );
    book(m_clusters_notMatchedFraction_energy      , "clusters_notMatchedFraction_energy");
    book(m_clusters_width                          , "clusters_width"                 );
    book(m_clusters_width_eta                      , "clusters_width_eta"             );
    book(m_clusters_energy                         , "clusters_energy"                );
    book(m_clusters_abs_energy                     , "clusters_abs_energy"            );
    book(m_clusters_width_energy                   , "clusters_width_energy"          );
    book(m_clusters_abs_width_energy               , "clusters_abs_width_energy"      );
    book(m_clusters_matched_energy                 , "clusters_matched_energy"        );
    book(m_clusters_notMatched_energy              , "clusters_notMatched_energy"     );
    book(m_clusters_matched_eta_fix_and_var        , "clusters_matched_eta_fix_and_var"          );
    book(m_clusters_matched_eta_fix_or_var         , "clusters_matched_eta_fix_or_var"           );
    book(m_clusters_matched_eta_fix                , "clusters_matched_eta_fix"                  );
    book(m_clusters_matched_eta_notfix             , "clusters_matched_eta_notfix"               );
    book(m_clusters_matched_eta_var                , "clusters_matched_eta_var"                  );
    book(m_clusters_matched_eta_notvar             , "clusters_matched_eta_notvar"               );
    book(m_clusters_matched_eta_onlyvar            , "clusters_matched_eta_onlyvar"              );
    book(m_clusters_matched_eta_onlyfix            , "clusters_matched_eta_onlyfix"              );
    book(m_clusters_matched_eta_none               , "clusters_matched_eta_none"                 );
    book(m_clusters_matchedFraction_eta_fix_and_var, "clusters_matchedFraction_eta_fix_and_var"  );
    book(m_clusters_matchedFraction_eta_fix_or_var , "clusters_matchedFraction_eta_fix_or_var"   );
    book(m_clusters_matchedFraction_eta_fix        , "clusters_matchedFraction_eta_fix"          );
    book(m_clusters_matchedFraction_eta_notfix     , "clusters_matchedFraction_eta_notfix"       );
    book(m_clusters_matchedFraction_eta_var        , "clusters_matchedFraction_eta_var"          );
    book(m_clusters_matchedFraction_eta_notvar     , "clusters_matchedFraction_eta_notvar"       );
    book(m_clusters_matchedFraction_eta_onlyvar    , "clusters_matchedFraction_eta_onlyvar"      );
    book(m_clusters_matchedFraction_eta_onlyfix    , "clusters_matchedFraction_eta_onlyfix"      );
    book(m_clusters_matchedFraction_eta_none       , "clusters_matchedFraction_eta_none"         );    
    
    book(m_clusters_abs_eta                            , "clusters_abs_eta"                   );
    book(m_clusters_abs_matched_eta                    , "clusters_abs_matched_eta"           );
    book(m_clusters_abs_notMatched_eta                 , "clusters_abs_notMatched_eta"        );
    book(m_clusters_abs_matchedFraction_eta            , "clusters_abs_matchedFraction_eta"   );
    book(m_clusters_abs_notMatchedFraction_eta         , "clusters_abs_notMatchedFraction_eta");
    book(m_clusters_abs_matched_energy                 , "clusters_abs_matched_energy"        );
    book(m_clusters_abs_notMatched_energy              , "clusters_abs_notMatched_energy"     );
    book(m_clusters_abs_matchedFraction_energy         , "clusters_abs_matchedFraction_energy"   );
    book(m_clusters_abs_notMatchedFraction_energy      , "clusters_abs_notMatchedFraction_energy");
    book(m_clusters_abs_width                          , "clusters_abs_width"                 );
    book(m_clusters_abs_width_eta                      , "clusters_abs_width_eta"             );
    book(m_clusters_abs_matched_eta_fix_and_var        , "clusters_abs_matched_eta_fix_and_var"          );
    book(m_clusters_abs_matched_eta_fix_or_var         , "clusters_abs_matched_eta_fix_or_var"           );
    book(m_clusters_abs_matched_eta_fix                , "clusters_abs_matched_eta_fix"                  );
    book(m_clusters_abs_matched_eta_notfix             , "clusters_abs_matched_eta_notfix"               );
    book(m_clusters_abs_matched_eta_var                , "clusters_abs_matched_eta_var"                  );
    book(m_clusters_abs_matched_eta_notvar             , "clusters_abs_matched_eta_notvar"               );
    book(m_clusters_abs_matched_eta_onlyvar            , "clusters_abs_matched_eta_onlyvar"              );
    book(m_clusters_abs_matched_eta_onlyfix            , "clusters_abs_matched_eta_onlyfix"              );
    book(m_clusters_abs_matched_eta_none               , "clusters_abs_matched_eta_none"                 );
    book(m_clusters_abs_matchedFraction_eta_fix_and_var, "clusters_abs_matchedFraction_eta_fix_and_var"  );
    book(m_clusters_abs_matchedFraction_eta_fix_or_var , "clusters_abs_matchedFraction_eta_fix_or_var"   );
    book(m_clusters_abs_matchedFraction_eta_fix        , "clusters_abs_matchedFraction_eta_fix"          );
    book(m_clusters_abs_matchedFraction_eta_notfix     , "clusters_abs_matchedFraction_eta_notfix"       );
    book(m_clusters_abs_matchedFraction_eta_var        , "clusters_abs_matchedFraction_eta_var"          );
    book(m_clusters_abs_matchedFraction_eta_notvar     , "clusters_abs_matchedFraction_eta_notvar"       );
    book(m_clusters_abs_matchedFraction_eta_onlyvar    , "clusters_abs_matchedFraction_eta_onlyvar"      );
    book(m_clusters_abs_matchedFraction_eta_onlyfix    , "clusters_abs_matchedFraction_eta_onlyfix"      );
    book(m_clusters_abs_matchedFraction_eta_none       , "clusters_abs_matchedFraction_eta_none"         );
    
    book(m_clusters_matched_pv0_eta_fix_and_var           , "clusters_matched_pv0_eta_fix_and_var"              );
    book(m_clusters_matched_pv0_eta_fix_or_var            , "clusters_matched_pv0_eta_fix_or_var"               );
    book(m_clusters_matched_pv0_eta_fix                   , "clusters_matched_pv0_eta_fix"                      );
    book(m_clusters_matched_pv0_eta_notfix                , "clusters_matched_pv0_eta_notfix"                   );
    book(m_clusters_matched_pv0_eta_var                   , "clusters_matched_pv0_eta_var"                      );
    book(m_clusters_matched_pv0_eta_notvar                , "clusters_matched_pv0_eta_notvar"                   );
    book(m_clusters_matched_pv0_eta_onlyvar               , "clusters_matched_pv0_eta_onlyvar"                  );
    book(m_clusters_matched_pv0_eta_onlyfix               , "clusters_matched_pv0_eta_onlyfix"                  );
    book(m_clusters_matched_pv0_eta_none                  , "clusters_matched_pv0_eta_none"                     );
    book(m_clusters_abs_matched_pv0_eta_fix_and_var       , "clusters_abs_matched_pv0_eta_fix_and_var"          );
    book(m_clusters_abs_matched_pv0_eta_fix_or_var        , "clusters_abs_matched_pv0_eta_fix_or_var"           );
    book(m_clusters_abs_matched_pv0_eta_fix               , "clusters_abs_matched_pv0_eta_fix"                  );
    book(m_clusters_abs_matched_pv0_eta_notfix            , "clusters_abs_matched_pv0_eta_notfix"               );
    book(m_clusters_abs_matched_pv0_eta_var               , "clusters_abs_matched_pv0_eta_var"                  );
    book(m_clusters_abs_matched_pv0_eta_notvar            , "clusters_abs_matched_pv0_eta_notvar"               );
    book(m_clusters_abs_matched_pv0_eta_onlyvar           , "clusters_abs_matched_pv0_eta_onlyvar"              );
    book(m_clusters_abs_matched_pv0_eta_onlyfix           , "clusters_abs_matched_pv0_eta_onlyfix"              );
    book(m_clusters_abs_matched_pv0_eta_none              , "clusters_abs_matched_pv0_eta_none"                 );
    book(m_clusters_matchedpv0Fraction_eta_fix_and_var    , "clusters_matchedpv0Fraction_eta_fix_and_var"       );
    book(m_clusters_matchedpv0Fraction_eta_fix_or_var     , "clusters_matchedpv0Fraction_eta_fix_or_var"        );
    book(m_clusters_matchedpv0Fraction_eta_fix            , "clusters_matchedpv0Fraction_eta_fix"               );
    book(m_clusters_matchedpv0Fraction_eta_notfix         , "clusters_matchedpv0Fraction_eta_notfix"            );
    book(m_clusters_matchedpv0Fraction_eta_var            , "clusters_matchedpv0Fraction_eta_var"               );
    book(m_clusters_matchedpv0Fraction_eta_notvar         , "clusters_matchedpv0Fraction_eta_notvar"            );
    book(m_clusters_matchedpv0Fraction_eta_onlyvar        , "clusters_matchedpv0Fraction_eta_onlyvar"           );
    book(m_clusters_matchedpv0Fraction_eta_onlyfix        , "clusters_matchedpv0Fraction_eta_onlyfix"           );
    book(m_clusters_matchedpv0Fraction_eta_none           , "clusters_matchedpv0Fraction_eta_none"              );
    book(m_clusters_abs_matchedpv0Fraction_eta_fix_and_var, "clusters_abs_matchedpv0Fraction_eta_fix_and_var"   );
    book(m_clusters_abs_matchedpv0Fraction_eta_fix_or_var , "clusters_abs_matchedpv0Fraction_eta_fix_or_var"    );
    book(m_clusters_abs_matchedpv0Fraction_eta_fix        , "clusters_abs_matchedpv0Fraction_eta_fix"           );
    book(m_clusters_abs_matchedpv0Fraction_eta_notfix     , "clusters_abs_matchedpv0Fraction_eta_notfix"        );
    book(m_clusters_abs_matchedpv0Fraction_eta_var        , "clusters_abs_matchedpv0Fraction_eta_var"           );
    book(m_clusters_abs_matchedpv0Fraction_eta_notvar     , "clusters_abs_matchedpv0Fraction_eta_notvar"        );
    book(m_clusters_abs_matchedpv0Fraction_eta_onlyvar    , "clusters_abs_matchedpv0Fraction_eta_onlyvar"       );
    book(m_clusters_abs_matchedpv0Fraction_eta_onlyfix    , "clusters_abs_matchedpv0Fraction_eta_onlyfix"       );
    book(m_clusters_abs_matchedpv0Fraction_eta_none       , "clusters_abs_matchedpv0Fraction_eta_none"          );
    
    
    book(m_clusters_matched_pvx_eta_fix_and_var           , "clusters_matched_pvx_eta_fix_and_var"              );
    book(m_clusters_matched_pvx_eta_fix_or_var            , "clusters_matched_pvx_eta_fix_or_var"               );
    book(m_clusters_matched_pvx_eta_fix                   , "clusters_matched_pvx_eta_fix"                      );
    book(m_clusters_matched_pvx_eta_notfix                , "clusters_matched_pvx_eta_notfix"                   );
    book(m_clusters_matched_pvx_eta_var                   , "clusters_matched_pvx_eta_var"                      );
    book(m_clusters_matched_pvx_eta_notvar                , "clusters_matched_pvx_eta_notvar"                   );
    book(m_clusters_matched_pvx_eta_onlyvar               , "clusters_matched_pvx_eta_onlyvar"                  );
    book(m_clusters_matched_pvx_eta_onlyfix               , "clusters_matched_pvx_eta_onlyfix"                  );
    book(m_clusters_matched_pvx_eta_none                  , "clusters_matched_pvx_eta_none"                     );
    book(m_clusters_abs_matched_pvx_eta_fix_and_var       , "clusters_abs_matched_pvx_eta_fix_and_var"          );
    book(m_clusters_abs_matched_pvx_eta_fix_or_var        , "clusters_abs_matched_pvx_eta_fix_or_var"           );
    book(m_clusters_abs_matched_pvx_eta_fix               , "clusters_abs_matched_pvx_eta_fix"                  );
    book(m_clusters_abs_matched_pvx_eta_notfix            , "clusters_abs_matched_pvx_eta_notfix"               );
    book(m_clusters_abs_matched_pvx_eta_var               , "clusters_abs_matched_pvx_eta_var"                  );
    book(m_clusters_abs_matched_pvx_eta_notvar            , "clusters_abs_matched_pvx_eta_notvar"               );
    book(m_clusters_abs_matched_pvx_eta_onlyvar           , "clusters_abs_matched_pvx_eta_onlyvar"              );
    book(m_clusters_abs_matched_pvx_eta_onlyfix           , "clusters_abs_matched_pvx_eta_onlyfix"              );
    book(m_clusters_abs_matched_pvx_eta_none              , "clusters_abs_matched_pvx_eta_none"                 );
    book(m_clusters_matchedpvxFraction_eta_fix_and_var    , "clusters_matchedpvxFraction_eta_fix_and_var"       );
    book(m_clusters_matchedpvxFraction_eta_fix_or_var     , "clusters_matchedpvxFraction_eta_fix_or_var"        );
    book(m_clusters_matchedpvxFraction_eta_fix            , "clusters_matchedpvxFraction_eta_fix"               );
    book(m_clusters_matchedpvxFraction_eta_notfix         , "clusters_matchedpvxFraction_eta_notfix"            );
    book(m_clusters_matchedpvxFraction_eta_var            , "clusters_matchedpvxFraction_eta_var"               );
    book(m_clusters_matchedpvxFraction_eta_notvar         , "clusters_matchedpvxFraction_eta_notvar"            );
    book(m_clusters_matchedpvxFraction_eta_onlyvar        , "clusters_matchedpvxFraction_eta_onlyvar"           );
    book(m_clusters_matchedpvxFraction_eta_onlyfix        , "clusters_matchedpvxFraction_eta_onlyfix"           );
    book(m_clusters_matchedpvxFraction_eta_none           , "clusters_matchedpvxFraction_eta_none"              );
    book(m_clusters_abs_matchedpvxFraction_eta_fix_and_var, "clusters_abs_matchedpvxFraction_eta_fix_and_var"   );
    book(m_clusters_abs_matchedpvxFraction_eta_fix_or_var , "clusters_abs_matchedpvxFraction_eta_fix_or_var"    );
    book(m_clusters_abs_matchedpvxFraction_eta_fix        , "clusters_abs_matchedpvxFraction_eta_fix"           );
    book(m_clusters_abs_matchedpvxFraction_eta_notfix     , "clusters_abs_matchedpvxFraction_eta_notfix"        );
    book(m_clusters_abs_matchedpvxFraction_eta_var        , "clusters_abs_matchedpvxFraction_eta_var"           );
    book(m_clusters_abs_matchedpvxFraction_eta_notvar     , "clusters_abs_matchedpvxFraction_eta_notvar"        );
    book(m_clusters_abs_matchedpvxFraction_eta_onlyvar    , "clusters_abs_matchedpvxFraction_eta_onlyvar"       );
    book(m_clusters_abs_matchedpvxFraction_eta_onlyfix    , "clusters_abs_matchedpvxFraction_eta_onlyfix"       );
    book(m_clusters_abs_matchedpvxFraction_eta_none       , "clusters_abs_matchedpvxFraction_eta_none"          );
    
    
    book(m_clusters_pt_fraction_e                      ,  "clusters_pt_fraction_e"          );
    book(m_clusters_PV0_pt_fraction_e                  ,  "clusters_PV0_pt_fraction_e"      );
    book(m_clusters_PVX_pt_fraction_e                  ,  "clusters_PVX_pt_fraction_e"      );
    book(m_clusters_rejected_pt_fraction_e             ,  "clusters_rejected_pt_fraction_e" );    
    
    book(m_clusters_all_neutral_eta                    , "clusters_all_neutral_eta"         );
    book(m_clusters_all_neutral_e                      , "clusters_all_neutral_e"           );
    book(m_clusters_all_neutral_PV0_eta                , "clusters_all_neutral_PV0_eta"     );
    book(m_clusters_all_neutral_PV0_e                  , "clusters_all_neutral_PV0_e"       );
    book(m_clusters_all_neutral_notPV0_eta             , "clusters_all_neutral_notPV0_eta"  );
    book(m_clusters_all_neutral_notPV0_e               , "clusters_all_neutral_notPV0_e"    );
    book(m_clusters_all_neutral_neutral_eta            , "clusters_all_neutral_neutral_eta"  );
    book(m_clusters_all_neutral_neutral_e              , "clusters_all_neutral_neutral_e"    );
    
    book(m_clusters_etacut_eta                         , "clusters_etacut_eta"               );
    book(m_clusters_etacut_width                       , "clusters_etacut_width"             );
    book(m_clusters_etacut_width_eta                   , "clusters_etacut_width_eta"         );
    book(m_clusters_etacut_energy                      , "clusters_etacut_energy"            );
    book(m_clusters_etacut_width_energy                , "clusters_etacut_width_energy"      );
    book(m_clusters_etacut_abs_eta                     , "clusters_etacut_abs_eta"           );
    book(m_clusters_etacut_abs_width                   , "clusters_etacut_abs_width"         );
    book(m_clusters_etacut_abs_width_eta               , "clusters_etacut_abs_width_eta"     );
    book(m_clusters_etacut_abs_energy                  , "clusters_etacut_abs_energy"        );
    book(m_clusters_etacut_abs_width_energy            , "clusters_etacut_abs_width_energy"  );
    
  } else if (m_collectionType == "tccs") {
    book(m_tcc_pt                         , "tcc_pt"                               );
    book(m_tcc_pt_etacut                  , "tcc_pt_etacut"                        );
    book(m_tcc_phi                        , "tcc_phi"                              );
    book(m_tcc_eta                        , "tcc_eta"                              );
    book(m_tcc_eta_ptcut                  , "tcc_eta_ptcut"                        );
    book(m_tcc_m                          , "tcc_m"                                );
    book(m_tcc_taste                      , "tcc_taste"                            );
    book(m_tcc_taste_pt                   , "tcc_taste_pt"                         );
    book(m_tcc_taste_pt_etacut            , "tcc_taste_pt_etacut"                  );
    book(m_tcc_taste_eta                  , "tcc_taste_eta"                        );
    book(m_tcc_taste_eta_ptcut            , "tcc_taste_eta_ptcut"                  );
    book(m_tcc_pt_pseudoresponse          , "tcc_pt_pseudoresponse"                );
    book(m_tcc_pt_track_pt                , "tcc_pt_track_pt"                      );
    book(m_tcc_pt_truth_pt                , "tcc_pt_truth_pt"                      );
    book(m_tcc_pt_response                , "tcc_pt_response"                      );
    book(m_tcc_N_M                        , "tcc_N_M"                              );
    book(m_tcc_N_M_pv0                    , "tcc_N_M_pv0"                          );
    
    book(m_trk_tcc_reco_pt_truth_pt                                  , "trk_tcc_reco_pt_truth_pt"                                 );
    book(m_trk_tcc_reco_pt_response                                  , "trk_tcc_reco_pt_response"                                 );
        
  } else
    std::cout << "WARNING: TCCPlots configured with unkown collection. Please check if you are using jets or tracks!" << std::endl;   
  
  resizeHistograms();
}

void TCCPlots::fill(const xAOD::JetContainer& jets) {
  fillHisto(m_jet_n , jets.size(), m_eventWeight); 
}

void TCCPlots::fill(const xAOD::Jet& jet) {
  fillHisto(m_jet_pt , jet.pt()/GeV, m_eventWeight);
  fillHisto(m_jet_eta, jet.eta()   , m_eventWeight);
  fillHisto(m_jet_phi, jet.phi()   , m_eventWeight);
  fillHisto(m_jet_m  , jet.m()/GeV , m_eventWeight);
}

void TCCPlots::fill(const xAOD::Jet& jet, const xAOD::Jet& truth) {
  fillHisto(m_jet_deltaR , jet.p4().DeltaR(truth.p4()), m_eventWeight);
}

void TCCPlots::fillResponse(const xAOD::Jet& jet, const xAOD::Jet& truth) {
  fillHisto(m_jet_deltaR_matched , jet.p4().DeltaR(truth.p4()), m_eventWeight);
  fillHisto(m_jet_response_m     , jet.m() /truth.m()         , m_eventWeight);
  fillHisto(m_jet_response_pt    , jet.pt()/truth.pt()        , m_eventWeight);
  fillHisto(m_jet_response_eta   , jet.eta()/truth.eta()      , m_eventWeight);
  fillHisto(m_jet_response_phi   , jet.phi()/truth.phi()      , m_eventWeight);
  
  fillHisto(m_jet_response_add_eta, jet.eta() - truth.eta()   , m_eventWeight);
  fillHisto(m_jet_response_add_eta_pt, truth.pt()/GeV, jet.eta() - truth.eta(), m_eventWeight);
  
  fillHisto(m_jet_response_m_pt  , truth.pt()/GeV, jet.m()/truth.m() , m_eventWeight);
    
  static SG::AuxElement::Accessor<float> accD2("D2");
  if (accD2.isAvailable(jet) and accD2.isAvailable(truth)) {
    fillHisto(m_jet_response_d2   , accD2(jet)/accD2(truth), m_eventWeight);  
    fillHisto(m_jet_response_d2_pt, truth.pt()/GeV, accD2(jet)/accD2(truth) , m_eventWeight);
    fillHisto(m_jet_response_add_d2   , accD2(jet) - accD2(truth), m_eventWeight);  
    fillHisto(m_jet_response_add_d2_pt, truth.pt()/GeV, accD2(jet) - accD2(truth) , m_eventWeight);
  }
}

void TCCPlots::fillResponseNPV(const xAOD::Jet& jet, const xAOD::Jet& truth, int npv) {
  fillHisto(m_jet_response_m_npv  , npv, jet.m()/truth.m() , m_eventWeight);
}

void TCCPlots::fillResponseNoPtNoMassCuts(const xAOD::Jet& jet, const xAOD::Jet& truth) {
  fillHisto(m_jet_mopt_pt_response_m     , truth.pt()/GeV, truth.m()/truth.pt(), jet.m() /truth.m()   , m_eventWeight);
  fillHisto(m_jet_mopt_pt_response_pt    , truth.pt()/GeV, truth.m()/truth.pt(), jet.pt()/truth.pt()  , m_eventWeight);
  fillHisto(m_jet_mopt_pt_response_eta   , truth.pt()/GeV, truth.m()/truth.pt(), jet.eta()/truth.eta(), m_eventWeight);
  fillHisto(m_jet_mopt_pt_response_phi   , truth.pt()/GeV, truth.m()/truth.pt(), jet.phi()/truth.phi(), m_eventWeight);
    
  static SG::AuxElement::Accessor<float> accD2("D2");
  if (accD2.isAvailable(jet) and accD2.isAvailable(truth))
    fillHisto(m_jet_mopt_pt_response_d2 , truth.pt()/GeV, truth.m()/truth.pt(), accD2(jet)/accD2(truth), m_eventWeight);
}

void TCCPlots::fillPseudoResponse(const xAOD::Jet& jet, const xAOD::Jet& calo) {
  fillHisto(m_jet_pseudoresponse_m     , jet.m() /calo.m() , m_eventWeight);
  fillHisto(m_jet_pseudoresponse_pt    , jet.pt()/calo.pt(), m_eventWeight);
  
  static SG::AuxElement::Accessor<float> ptAcc("JetConstitScaleMomentum_pt");
  if (ptAcc.isAvailable(calo)) {
    fillHisto(m_jet_pseudoresponse_notcalib_pt, jet.pt()/ptAcc(calo), m_eventWeight);
  }
}

void TCCPlots::fillMoments(const xAOD::Jet& jet) {
  static SG::AuxElement::Accessor<float> accD2("D2");
  if (accD2.isAvailable(jet))
    fillHisto(m_jet_d2, accD2(jet), m_eventWeight);
}

void TCCPlots::fillMomentsWithMassCut(const xAOD::Jet& jet) {
  static SG::AuxElement::Accessor<float> accD2("D2");
  if (accD2.isAvailable(jet))
    fillHisto(m_jet_d2_massCut, accD2(jet), m_eventWeight);
}

void TCCPlots::fillMomentsLeading(const xAOD::Jet& jet) {
  static SG::AuxElement::Accessor<float> accD2("D2");
  if (accD2.isAvailable(jet)) {
    fillHisto(m_jet_d2_leading       , accD2(jet), m_eventWeight);
    fillHisto(m_jet_d2_2leadings     , accD2(jet), m_eventWeight);
  }
}

void TCCPlots::fillMomentsLeadingWithMassCut(const xAOD::Jet& jet) {
  static SG::AuxElement::Accessor<float> accD2("D2");
  if (accD2.isAvailable(jet)) {
    fillHisto(m_jet_d2_leading_massCut       , accD2(jet), m_eventWeight);
    fillHisto(m_jet_d2_2leadings_massCut     , accD2(jet), m_eventWeight);
  }
}

void TCCPlots::fillMomentsSubLeading(const xAOD::Jet& jet) {
  static SG::AuxElement::Accessor<float> accD2("D2");
  if (accD2.isAvailable(jet)) {
    fillHisto(m_jet_d2_subleading    , accD2(jet), m_eventWeight);
    fillHisto(m_jet_d2_2leadings     , accD2(jet), m_eventWeight);
  }
}

void TCCPlots::fillMomentsSubLeadingWithMassCut(const xAOD::Jet& jet) {
  static SG::AuxElement::Accessor<float> accD2("D2");
  if (accD2.isAvailable(jet)) {
    fillHisto(m_jet_d2_subleading_massCut    , accD2(jet), m_eventWeight);
    fillHisto(m_jet_d2_2leadings_massCut     , accD2(jet), m_eventWeight);
  }
}

void TCCPlots::fillResponseLeading(const xAOD::Jet& jet, const xAOD::Jet& truth) {
  fillHisto(m_jet_response_m_leading     , jet.m()  /truth.m()  , m_eventWeight);
  fillHisto(m_jet_response_pt_leading    , jet.pt() /truth.pt() , m_eventWeight);
  fillHisto(m_jet_response_m_2leadings   , jet.m()  /truth.m()  , m_eventWeight);
  fillHisto(m_jet_response_pt_2leadings  , jet.pt() /truth.pt() , m_eventWeight);
  fillHisto(m_jet_response_eta_leading   , jet.eta()/truth.eta(), m_eventWeight);
  fillHisto(m_jet_response_phi_leading   , jet.phi()/truth.phi(), m_eventWeight);
  fillHisto(m_jet_response_eta_2leadings , jet.eta()/truth.eta(), m_eventWeight);
  fillHisto(m_jet_response_phi_2leadings , jet.phi()/truth.phi(), m_eventWeight);
  
  fillHisto(m_jet_response_add_eta_leading  , jet.eta() - truth.eta()   , m_eventWeight);
  fillHisto(m_jet_response_add_eta_2leadings, jet.eta() - truth.eta()   , m_eventWeight);
  
  fillHisto(m_jet_response_add_eta_pt_leading  , truth.pt()/GeV, jet.eta() - truth.eta(), m_eventWeight);
  fillHisto(m_jet_response_add_eta_pt_2leadings, truth.pt()/GeV, jet.eta() - truth.eta(), m_eventWeight);
  
  fillHisto(m_jet_response_m_pt_2leadings  , truth.pt()/GeV, jet.m()/truth.m(), m_eventWeight);
  fillHisto(m_jet_response_m_pt_leading    , truth.pt()/GeV, jet.m()/truth.m(), m_eventWeight);

  static SG::AuxElement::Accessor<float> accD2("D2");
  if (accD2.isAvailable(jet) and accD2.isAvailable(truth)) {
    fillHisto(m_jet_response_d2_2leadings , accD2(jet)/accD2(truth), m_eventWeight);
    fillHisto(m_jet_response_d2_leading   , accD2(jet)/accD2(truth), m_eventWeight);
    fillHisto(m_jet_response_d2_pt_2leadings  , truth.pt()/GeV, accD2(jet)/accD2(truth), m_eventWeight);
    fillHisto(m_jet_response_d2_pt_leading    , truth.pt()/GeV, accD2(jet)/accD2(truth), m_eventWeight);
    fillHisto(m_jet_response_add_d2_2leadings , accD2(jet) - accD2(truth), m_eventWeight);
    fillHisto(m_jet_response_add_d2_leading   , accD2(jet) - accD2(truth), m_eventWeight);
    fillHisto(m_jet_response_add_d2_pt_2leadings  , truth.pt()/GeV, accD2(jet) - accD2(truth), m_eventWeight);
    fillHisto(m_jet_response_add_d2_pt_leading    , truth.pt()/GeV, accD2(jet) - accD2(truth), m_eventWeight);
  }
}

void TCCPlots::fillResponseLeadingNPV(const xAOD::Jet& jet, const xAOD::Jet& truth, int npv) {
  fillHisto(m_jet_response_m_npv_2leadings  , npv, jet.m()/truth.m() , m_eventWeight);
  fillHisto(m_jet_response_m_npv_leading    , npv, jet.m()/truth.m() , m_eventWeight);
}

void TCCPlots::fillResponseNoPtNoMassCutsLeading(const xAOD::Jet& jet, const xAOD::Jet& truth) {
  fillHisto(m_jet_mopt_pt_response_m_leading       , truth.pt()/GeV, truth.m()/truth.pt(), jet.m() /truth.m()   , m_eventWeight);
  fillHisto(m_jet_mopt_pt_response_m_2leadings     , truth.pt()/GeV, truth.m()/truth.pt(), jet.m() /truth.m()   , m_eventWeight);
  fillHisto(m_jet_mopt_pt_response_pt_leading      , truth.pt()/GeV, truth.m()/truth.pt(), jet.pt()/truth.pt()  , m_eventWeight);
  fillHisto(m_jet_mopt_pt_response_pt_2leadings    , truth.pt()/GeV, truth.m()/truth.pt(), jet.pt()/truth.pt()  , m_eventWeight);
  fillHisto(m_jet_mopt_pt_response_eta_leading     , truth.pt()/GeV, truth.m()/truth.pt(), jet.eta()/truth.eta(), m_eventWeight);
  fillHisto(m_jet_mopt_pt_response_eta_2leadings   , truth.pt()/GeV, truth.m()/truth.pt(), jet.eta()/truth.eta(), m_eventWeight);
  fillHisto(m_jet_mopt_pt_response_phi_leading     , truth.pt()/GeV, truth.m()/truth.pt(), jet.phi()/truth.phi(), m_eventWeight);
  fillHisto(m_jet_mopt_pt_response_phi_2leadings   , truth.pt()/GeV, truth.m()/truth.pt(), jet.phi()/truth.phi(), m_eventWeight);
  
    
  static SG::AuxElement::Accessor<float> accD2("D2");
  if (accD2.isAvailable(jet) and accD2.isAvailable(truth)) {
    fillHisto(m_jet_mopt_pt_response_d2_leading   , truth.pt()/GeV, truth.m()/truth.pt(), accD2(jet)/accD2(truth), m_eventWeight);
    fillHisto(m_jet_mopt_pt_response_d2_2leadings , truth.pt()/GeV, truth.m()/truth.pt(), accD2(jet)/accD2(truth), m_eventWeight);
  }
}

void TCCPlots::fillLeading(const xAOD::Jet& jet) {
  fillHisto(m_jet_m_2leadings   , jet.m() /GeV, m_eventWeight);
  fillHisto(m_jet_m_leading     , jet.m() /GeV, m_eventWeight);
  fillHisto(m_jet_pt_2leadings  , jet.pt()/GeV, m_eventWeight);
  fillHisto(m_jet_pt_leading    , jet.pt()/GeV, m_eventWeight);
  fillHisto(m_jet_eta_2leadings , jet.eta()   , m_eventWeight);
  fillHisto(m_jet_eta_leading   , jet.eta()   , m_eventWeight);
  fillHisto(m_jet_phi_2leadings , jet.phi()   , m_eventWeight);
  fillHisto(m_jet_phi_leading   , jet.phi()   , m_eventWeight);
}

void TCCPlots::fillResponseSubLeading(const xAOD::Jet& jet, const xAOD::Jet& truth) {
  fillHisto(m_jet_response_m_subleading   , jet.m()  /truth.m()  , m_eventWeight);
  fillHisto(m_jet_response_pt_subleading  , jet.pt() /truth.pt() , m_eventWeight);
  fillHisto(m_jet_response_m_2leadings    , jet.m()  /truth.m()  , m_eventWeight);
  fillHisto(m_jet_response_pt_2leadings   , jet.pt() /truth.pt() , m_eventWeight);
  fillHisto(m_jet_response_eta_subleading , jet.eta()/truth.eta(), m_eventWeight);
  fillHisto(m_jet_response_phi_subleading , jet.phi()/truth.phi(), m_eventWeight);
  fillHisto(m_jet_response_eta_2leadings  , jet.eta()/truth.eta(), m_eventWeight);
  fillHisto(m_jet_response_phi_2leadings  , jet.phi()/truth.phi(), m_eventWeight);
  
  fillHisto(m_jet_response_add_eta_subleading, jet.eta() - truth.eta()   , m_eventWeight);
  fillHisto(m_jet_response_add_eta_2leadings , jet.eta() - truth.eta()   , m_eventWeight);
  
  fillHisto(m_jet_response_add_eta_pt_subleading, truth.pt()/GeV, jet.eta() - truth.eta(), m_eventWeight);
  fillHisto(m_jet_response_add_eta_pt_2leadings , truth.pt()/GeV, jet.eta() - truth.eta(), m_eventWeight);
  
  fillHisto(m_jet_response_m_pt_2leadings  , truth.pt()/GeV, jet.m()/truth.m(), m_eventWeight);
  fillHisto(m_jet_response_m_pt_subleading , truth.pt()/GeV, jet.m()/truth.m(), m_eventWeight);

  static SG::AuxElement::Accessor<float> accD2("D2");
  if (accD2.isAvailable(jet) and accD2.isAvailable(truth)) {
    fillHisto(m_jet_response_d2_2leadings    , accD2(jet)/accD2(truth), m_eventWeight);
    fillHisto(m_jet_response_d2_subleading   , accD2(jet)/accD2(truth), m_eventWeight);
    fillHisto(m_jet_response_d2_pt_2leadings  , truth.pt()/GeV, accD2(jet)/accD2(truth), m_eventWeight);
    fillHisto(m_jet_response_d2_pt_subleading , truth.pt()/GeV, accD2(jet)/accD2(truth), m_eventWeight);
    fillHisto(m_jet_response_add_d2_2leadings    , accD2(jet) - accD2(truth), m_eventWeight);
    fillHisto(m_jet_response_add_d2_subleading   , accD2(jet) - accD2(truth), m_eventWeight);
    fillHisto(m_jet_response_add_d2_pt_2leadings  , truth.pt()/GeV, accD2(jet) - accD2(truth), m_eventWeight);
    fillHisto(m_jet_response_add_d2_pt_subleading , truth.pt()/GeV, accD2(jet) - accD2(truth), m_eventWeight);
  }
}

void TCCPlots::fillResponseSubLeadingNPV(const xAOD::Jet& jet, const xAOD::Jet& truth, int npv) {
  fillHisto(m_jet_response_m_npv_2leadings  , npv, jet.m()/truth.m() , m_eventWeight);
  fillHisto(m_jet_response_m_npv_subleading , npv, jet.m()/truth.m() , m_eventWeight);
}

void TCCPlots::fillResponseNoPtNoMassCutsSubLeading(const xAOD::Jet& jet, const xAOD::Jet& truth) {
  fillHisto(m_jet_mopt_pt_response_m_subleading    , truth.pt()/GeV, truth.m()/truth.pt(), jet.m() /truth.m()   , m_eventWeight);
  fillHisto(m_jet_mopt_pt_response_m_2leadings     , truth.pt()/GeV, truth.m()/truth.pt(), jet.m() /truth.m()   , m_eventWeight);
  fillHisto(m_jet_mopt_pt_response_pt_subleading   , truth.pt()/GeV, truth.m()/truth.pt(), jet.pt()/truth.pt()  , m_eventWeight);
  fillHisto(m_jet_mopt_pt_response_pt_2leadings    , truth.pt()/GeV, truth.m()/truth.pt(), jet.pt()/truth.pt()  , m_eventWeight);
  fillHisto(m_jet_mopt_pt_response_eta_subleading  , truth.pt()/GeV, truth.m()/truth.pt(), jet.eta()/truth.eta(), m_eventWeight);
  fillHisto(m_jet_mopt_pt_response_eta_2leadings   , truth.pt()/GeV, truth.m()/truth.pt(), jet.eta()/truth.eta(), m_eventWeight);
  fillHisto(m_jet_mopt_pt_response_phi_subleading  , truth.pt()/GeV, truth.m()/truth.pt(), jet.phi()/truth.phi(), m_eventWeight);
  fillHisto(m_jet_mopt_pt_response_phi_2leadings   , truth.pt()/GeV, truth.m()/truth.pt(), jet.phi()/truth.phi(), m_eventWeight);
    
  static SG::AuxElement::Accessor<float> accD2("D2");
  if (accD2.isAvailable(jet) and accD2.isAvailable(truth)) {
    fillHisto(m_jet_mopt_pt_response_d2_subleading, truth.pt()/GeV, truth.m()/truth.pt(), accD2(jet)/accD2(truth), m_eventWeight);
    fillHisto(m_jet_mopt_pt_response_d2_2leadings , truth.pt()/GeV, truth.m()/truth.pt(), accD2(jet)/accD2(truth), m_eventWeight);
  }
}

void TCCPlots::fillSubLeading(const xAOD::Jet& jet) {
  fillHisto(m_jet_m_2leadings    , jet.m() /GeV, m_eventWeight);
  fillHisto(m_jet_m_subleading   , jet.m() /GeV, m_eventWeight);
  fillHisto(m_jet_pt_2leadings   , jet.pt()/GeV, m_eventWeight);
  fillHisto(m_jet_pt_subleading  , jet.pt()/GeV, m_eventWeight);
  fillHisto(m_jet_eta_2leadings  , jet.eta()   , m_eventWeight);
  fillHisto(m_jet_eta_subleading , jet.eta()   , m_eventWeight);
  fillHisto(m_jet_phi_2leadings  , jet.phi()   , m_eventWeight);
  fillHisto(m_jet_phi_subleading , jet.phi()   , m_eventWeight);
}

void TCCPlots::fillPseudoResponseLeading(const xAOD::Jet& jet, const xAOD::Jet& calo) {
  fillHisto(m_jet_pseudoresponse_m_leading     , jet.m() /calo.m() , m_eventWeight);
  fillHisto(m_jet_pseudoresponse_pt_leading    , jet.pt()/calo.pt(), m_eventWeight);
  fillHisto(m_jet_pseudoresponse_m_2leadings   , jet.m() /calo.m() , m_eventWeight);
  fillHisto(m_jet_pseudoresponse_pt_2leadings  , jet.pt()/calo.pt(), m_eventWeight);
  
  static SG::AuxElement::Accessor<float> ptAcc("JetConstitScaleMomentum_pt");
  if (ptAcc.isAvailable(calo)) {
    fillHisto(m_jet_pseudoresponse_notcalib_pt_leading  , jet.pt()/ptAcc(calo), m_eventWeight);
    fillHisto(m_jet_pseudoresponse_notcalib_pt_2leadings, jet.pt()/ptAcc(calo), m_eventWeight);
  }
}

void TCCPlots::fillPseudoResponseSubLeading(const xAOD::Jet& jet, const xAOD::Jet& calo) {
  fillHisto(m_jet_pseudoresponse_m_subleading   , jet.m() /calo.m() , m_eventWeight);
  fillHisto(m_jet_pseudoresponse_pt_subleading  , jet.pt()/calo.pt(), m_eventWeight);
  fillHisto(m_jet_pseudoresponse_m_2leadings    , jet.m() /calo.m() , m_eventWeight);
  fillHisto(m_jet_pseudoresponse_pt_2leadings   , jet.pt()/calo.pt(), m_eventWeight);
  
  static SG::AuxElement::Accessor<float> ptAcc("JetConstitScaleMomentum_pt");
  if (ptAcc.isAvailable(calo)) {
    fillHisto(m_jet_pseudoresponse_notcalib_pt_subleading, jet.pt()/ptAcc(calo), m_eventWeight);
    fillHisto(m_jet_pseudoresponse_notcalib_pt_2leadings , jet.pt()/ptAcc(calo), m_eventWeight);
  }
}
      
void TCCPlots::fillTrackParametersAllPt(const xAOD::TrackParticle& track) {  
  fillHisto(m_trk_d0_pt      , track.pt()/GeV, track.d0()    , m_eventWeight);
  fillHisto(m_trk_z0_pt      , track.pt()/GeV, track.z0()    , m_eventWeight);
  fillHisto(m_trk_theta_pt   , track.pt()/GeV, track.theta() , m_eventWeight);
  fillHisto(m_trk_phi_pt     , track.pt()/GeV, track.phi()   , m_eventWeight);
  fillHisto(m_trk_qOverP_pt  , track.pt()/GeV, track.qOverP(), m_eventWeight);
}

void TCCPlots::fillCaloEntryInfoAllPt(const xAOD::TrackParticle& track) {    
  static SG::AuxElement::Accessor< float > acc_caloEntryPosEta( "CaloEntryPosEta" );
  static SG::AuxElement::Accessor< float > acc_caloEntryUncEta( "CaloEntryUncEta" );
  static SG::AuxElement::Accessor< float > acc_caloEntryUncTheta( "CaloEntryUncTheta" );
  if (acc_caloEntryPosEta.isAvailable(track)) {
    float caloEntryEta = acc_caloEntryPosEta(track);
    fillHisto(m_trk_caloEntryEtaOverEta_pt , track.pt()/GeV , caloEntryEta/track.eta(), m_eventWeight);
  }
  
  if (acc_caloEntryUncEta.isAvailable(track)) {
    float caloEntryUncEta = acc_caloEntryUncEta(track);
    fillHisto(m_trk_caloEntryUncEta_pt          , track.pt()/GeV  , caloEntryUncEta, m_eventWeight);
  }
  if (acc_caloEntryUncTheta.isAvailable(track)) {
    float caloEntryUncTheta = acc_caloEntryUncTheta(track);
    fillHisto(m_trk_caloEntryUncTheta_pt          , track.pt()/GeV  , caloEntryUncTheta, m_eventWeight);
  }  
  
  static SG::AuxElement::Accessor< float > acc_caloEntryPosPhi( "CaloEntryPosPhi" );
  static SG::AuxElement::Accessor< float > acc_caloEntryUncPhi( "CaloEntryUncPhi" );
  if (acc_caloEntryPosPhi.isAvailable(track)) {
    float caloEntryPhi = acc_caloEntryPosPhi(track);
    fillHisto(m_trk_caloEntryPhiOverPhi_pt , track.pt()/GeV , caloEntryPhi/track.phi(), m_eventWeight);
  }
  
  if (acc_caloEntryUncPhi.isAvailable(track)) {
    float caloEntryUncPhi = acc_caloEntryUncPhi(track);
    fillHisto(m_trk_caloEntryUncPhi_pt          , track.pt()/GeV  , caloEntryUncPhi, m_eventWeight);
  }
  
  if (acc_caloEntryUncEta.isAvailable(track) and acc_caloEntryUncPhi.isAvailable(track)) {
    float caloEntryUncEta = acc_caloEntryUncEta(track);
    float caloEntryUncPhi = acc_caloEntryUncPhi(track);
    float totUnc = sqrt(caloEntryUncEta*caloEntryUncEta+caloEntryUncPhi*caloEntryUncPhi);
    fillHisto(m_trk_caloEntryUncTot_pt          , track.pt()/GeV  , totUnc, m_eventWeight);    
  }
}

void TCCPlots::fillPerigeeInfoAllPt(const xAOD::TrackParticle& track) {
  
  static SG::AuxElement::Accessor< float > acc_perigeePosEta( "PerigeePosEta" );
  static SG::AuxElement::Accessor< float > acc_perigeeUncEta( "PerigeeUncEta" );
  static SG::AuxElement::Accessor< float > acc_perigeeUncTheta( "PerigeeUncTheta" );
  if (acc_perigeeUncEta.isAvailable(track)) {
    float perigeeUncEta = acc_perigeeUncEta(track);
    fillHisto(m_trk_perigeeUncEta_pt          , track.pt()/GeV  , perigeeUncEta, m_eventWeight);
  }
  if (acc_perigeeUncTheta.isAvailable(track)) {
    float perigeeUncTheta = acc_perigeeUncTheta(track);
    fillHisto(m_trk_perigeeUncTheta_pt          , track.pt()/GeV  , perigeeUncTheta, m_eventWeight);
  }
  
  static SG::AuxElement::Accessor< float > acc_perigeePosPhi( "PerigeePosPhi" );
  static SG::AuxElement::Accessor< float > acc_perigeeUncPhi( "PerigeeUncPhi" );
  if (acc_perigeeUncPhi.isAvailable(track)) {
    float perigeeUncPhi = acc_perigeeUncPhi(track);
    fillHisto(m_trk_perigeeUncPhi_pt          , track.pt()/GeV  , perigeeUncPhi, m_eventWeight);
  }
  
  if (acc_perigeeUncEta.isAvailable(track) and acc_perigeeUncPhi.isAvailable(track)) {
    float perigeeUncEta = acc_perigeeUncEta(track);
    float perigeeUncPhi = acc_perigeeUncPhi(track);
    float totUnc = sqrt(perigeeUncEta*perigeeUncEta+perigeeUncPhi*perigeeUncPhi);
    fillHisto(m_trk_perigeeUncTot_pt          , track.pt()/GeV  , totUnc, m_eventWeight);
  }
}

void TCCPlots::fillPerigeeVsCaloEntryAllPt(const xAOD::TrackParticle& track) {
  static SG::AuxElement::Accessor< float > acc_perigeeEta( "PerigeePosEta" );
  static SG::AuxElement::Accessor< float > acc_perigeePhi( "PerigeePosPhi" );
  static SG::AuxElement::Accessor< float > acc_caloEntryEta( "CaloEntryPosEta" );
  static SG::AuxElement::Accessor< float > acc_caloEntryPhi( "CaloEntryPosPhi" );
  
  if (acc_perigeeEta.isAvailable(track) and acc_perigeePhi.isAvailable(track) and acc_caloEntryEta.isAvailable(track) and acc_caloEntryPhi.isAvailable(track)) {
    float delta_eta = acc_caloEntryEta(track) - acc_perigeeEta(track);
    float delta_phi = acc_caloEntryPhi(track) - acc_perigeePhi(track);
    
    if (delta_phi > +M_PI) delta_phi -= 2. * M_PI;
    if (delta_phi < -M_PI) delta_phi += 2. * M_PI;
    
    fillHisto(m_trk_delta_perigeeEta_caloEntryEta_pt        , track.pt()/GeV   , delta_eta   , m_eventWeight);
    fillHisto(m_trk_delta_perigeePhi_caloEntryPhi_pt        , track.pt()/GeV   , delta_phi   , m_eventWeight);
    
    
    delta_eta = acc_caloEntryEta(track) - track.eta();
    delta_phi = acc_caloEntryPhi(track) - track.phi();
    
    if (delta_phi > +M_PI) delta_phi -= 2. * M_PI;
    if (delta_phi < -M_PI) delta_phi += 2. * M_PI;
               
    fillHisto(m_trk_delta_trackEta_caloEntryEta_pt , track.pt()/GeV, delta_eta, m_eventWeight);
    fillHisto(m_trk_delta_trackPhi_caloEntryPhi_pt , track.pt()/GeV, delta_phi, m_eventWeight);
  }
  
  static SG::AuxElement::Accessor< int > acc_caloEntryCorr( "Corrected" );
  static SG::AuxElement::Accessor< float > acc_caloEntryEtaCorr( "CaloEntryPosEtaCorr" );
  static SG::AuxElement::Accessor< float > acc_caloEntryPhiCorr( "CaloEntryPosPhiCorr" );
  
  if (acc_caloEntryCorr.isAvailable(track) and acc_caloEntryCorr(track)>0) {
    float delta_eta = acc_caloEntryEtaCorr(track) - track.eta();
    float delta_phi = acc_caloEntryPhiCorr(track) - track.phi();
    
    if (delta_phi > +M_PI) delta_phi -= 2. * M_PI;    
    if (delta_phi < -M_PI) delta_phi += 2. * M_PI;
               
    fillHisto(m_trk_delta_trackEta_caloEntryEtaCorr_pt, track.pt()/GeV   , delta_eta, m_eventWeight);
    fillHisto(m_trk_delta_trackPhi_caloEntryPhiCorr_pt, track.pt()/GeV   , delta_phi, m_eventWeight);
  }
  
  if (acc_caloEntryEta.isAvailable(track) and acc_caloEntryPhi.isAvailable(track) and acc_caloEntryCorr.isAvailable(track) and acc_caloEntryCorr(track)>0) {
    float delta_eta = acc_caloEntryEtaCorr(track) - acc_caloEntryEta(track);
    float delta_phi = acc_caloEntryPhiCorr(track) - acc_caloEntryPhi(track);
    
    if (delta_phi > +M_PI) delta_phi -= 2. * M_PI;    
    if (delta_phi < -M_PI) delta_phi += 2. * M_PI;
               
    fillHisto(m_trk_delta_caloEntryEta_caloEntryEtaCorr_pt, track.pt()/GeV   , delta_eta, m_eventWeight);
    fillHisto(m_trk_delta_caloEntryPhi_caloEntryPhiCorr_pt, track.pt()/GeV   , delta_phi, m_eventWeight);
  }  
}

void TCCPlots::fillTrackParameters(const xAOD::TrackParticle& track) {
  fillHisto(m_trk_d0      , track.d0()    , m_eventWeight);
  fillHisto(m_trk_z0      , track.z0()    , m_eventWeight);
  fillHisto(m_trk_theta   , track.theta() , m_eventWeight);
  fillHisto(m_trk_phi     , track.phi()   , m_eventWeight);
  fillHisto(m_trk_qOverP  , track.qOverP(), m_eventWeight);  
  
  fillHisto(m_trk_d0_eta      , track.eta(), track.d0()    , m_eventWeight);
  fillHisto(m_trk_z0_eta      , track.eta(), track.z0()    , m_eventWeight);
  fillHisto(m_trk_theta_eta   , track.eta(), track.theta() , m_eventWeight);
  fillHisto(m_trk_phi_eta     , track.eta(), track.phi()   , m_eventWeight);
  fillHisto(m_trk_qOverP_eta  , track.eta(), track.qOverP(), m_eventWeight);  
  
}

void TCCPlots::fillCaloEntryInfo(const xAOD::TrackParticle& track) {    
  static SG::AuxElement::Accessor< float > acc_caloEntryPosEta( "CaloEntryPosEta" );
  static SG::AuxElement::Accessor< float > acc_caloEntryUncEta( "CaloEntryUncEta" );
  static SG::AuxElement::Accessor< float > acc_caloEntryUncTheta( "CaloEntryUncTheta" );
  if (acc_caloEntryPosEta.isAvailable(track)) {
    float caloEntryEta = acc_caloEntryPosEta(track);
    fillHisto(m_trk_caloEntryEta           , caloEntryEta , m_eventWeight);
    fillHisto(m_trk_caloEntryEtaOverEta_eta, track.eta()    , caloEntryEta/track.eta(), m_eventWeight);
  }
  
  if (acc_caloEntryPosEta.isAvailable(track) and acc_caloEntryUncEta.isAvailable(track)) {
    float caloEntryEta    = acc_caloEntryPosEta(track);
    float caloEntryUncEta = acc_caloEntryUncEta(track);
    fillHisto(m_trk_caloEntryUncEta_caloEntryEta, caloEntryEta    , caloEntryUncEta, m_eventWeight);
    fillHisto(m_trk_caloEntryUncEta_eta         , track.eta()     , caloEntryUncEta, m_eventWeight);
  }
  if (acc_caloEntryUncTheta.isAvailable(track)) {
    float caloEntryUncTheta = acc_caloEntryUncTheta(track);
    fillHisto(m_trk_caloEntryUncTheta_eta         , track.eta()     , caloEntryUncTheta, m_eventWeight);
  }  
  
  static SG::AuxElement::Accessor< float > acc_caloEntryPosPhi( "CaloEntryPosPhi" );
  static SG::AuxElement::Accessor< float > acc_caloEntryUncPhi( "CaloEntryUncPhi" );
  if (acc_caloEntryPosPhi.isAvailable(track)) {
    float caloEntryPhi = acc_caloEntryPosPhi(track);
    fillHisto(m_trk_caloEntryPhi           , caloEntryPhi, m_eventWeight);
    fillHisto(m_trk_caloEntryPhiOverPhi_eta, track.eta()    , caloEntryPhi/track.phi(), m_eventWeight);
  }
  
  if (acc_caloEntryPosPhi.isAvailable(track) and acc_caloEntryUncPhi.isAvailable(track)) {
    float caloEntryPhi    = acc_caloEntryPosPhi(track);
    float caloEntryUncPhi = acc_caloEntryUncPhi(track);
    fillHisto(m_trk_caloEntryUncPhi_caloEntryPhi, caloEntryPhi, caloEntryUncPhi, m_eventWeight);
    fillHisto(m_trk_caloEntryUncPhi_eta         , track.eta() , caloEntryUncPhi, m_eventWeight);
  }
  
  if (acc_caloEntryUncEta.isAvailable(track) and acc_caloEntryUncPhi.isAvailable(track)) {
    float caloEntryUncEta = acc_caloEntryUncEta(track);
    float caloEntryUncPhi = acc_caloEntryUncPhi(track);
    float totUnc = sqrt(caloEntryUncEta*caloEntryUncEta+caloEntryUncPhi*caloEntryUncPhi);
    fillHisto(m_trk_caloEntryUncTot_eta         , track.eta() , totUnc, m_eventWeight);
    static SG::AuxElement::Accessor< float > acc_prodRadius( "ProductionRadius" );
    if (acc_prodRadius.isAvailable(track)) {
      fillHisto(m_trk_caloEntryUncEta_prodRadius, acc_prodRadius(track), caloEntryUncEta, m_eventWeight);
      fillHisto(m_trk_caloEntryUncPhi_prodRadius, acc_prodRadius(track), caloEntryUncPhi, m_eventWeight);
      fillHisto(m_trk_caloEntryUncTot_prodRadius, acc_prodRadius(track), totUnc         , m_eventWeight);
      if (acc_caloEntryUncTheta.isAvailable(track))
	fillHisto(m_trk_caloEntryUncTheta_prodRadius, acc_prodRadius(track),  acc_caloEntryUncTheta(track), m_eventWeight);
    }    
  }
}

void TCCPlots::fillPerigeeInfo(const xAOD::TrackParticle& track) {
  
  static SG::AuxElement::Accessor< float > acc_perigeePosEta( "PerigeePosEta" );
  static SG::AuxElement::Accessor< float > acc_perigeeUncEta( "PerigeeUncEta" );
  static SG::AuxElement::Accessor< float > acc_perigeeUncTheta( "PerigeeUncTheta" );
  if (acc_perigeePosEta.isAvailable(track)) {
    float perigeeEta = acc_perigeePosEta(track);
    fillHisto(m_trk_perigeeEta           , perigeeEta, m_eventWeight);
  }
  if (acc_perigeeUncEta.isAvailable(track)) {
    float perigeeUncEta = acc_perigeeUncEta(track);
    fillHisto(m_trk_perigeeUncEta_eta         , track.eta()     , perigeeUncEta, m_eventWeight);
  }
  if (acc_perigeeUncTheta.isAvailable(track)) {
    float perigeeUncTheta = acc_perigeeUncTheta(track);
    fillHisto(m_trk_perigeeUncTheta_eta         , track.eta()     , perigeeUncTheta, m_eventWeight);
  }
  
  static SG::AuxElement::Accessor< float > acc_perigeePosPhi( "PerigeePosPhi" );
  static SG::AuxElement::Accessor< float > acc_perigeeUncPhi( "PerigeeUncPhi" );
  if (acc_perigeePosPhi.isAvailable(track)) {
    float perigeePhi = acc_perigeePosPhi(track);
    fillHisto(m_trk_perigeePhi           , perigeePhi, m_eventWeight);
  }
  
  if (acc_perigeeUncPhi.isAvailable(track)) {
    float perigeeUncPhi = acc_perigeeUncPhi(track);
    fillHisto(m_trk_perigeeUncPhi_eta         , track.eta() , perigeeUncPhi, m_eventWeight);
  }
  
  if (acc_perigeeUncEta.isAvailable(track) and acc_perigeeUncPhi.isAvailable(track)) {
    float perigeeUncEta = acc_perigeeUncEta(track);
    float perigeeUncPhi = acc_perigeeUncPhi(track);
    float totUnc = sqrt(perigeeUncEta*perigeeUncEta+perigeeUncPhi*perigeeUncPhi);
    fillHisto(m_trk_perigeeUncTot_eta         , track.eta() , totUnc, m_eventWeight);
    static SG::AuxElement::Accessor< float > acc_prodRadius( "ProductionRadius" );
    if (acc_prodRadius.isAvailable(track)) {
      fillHisto(m_trk_perigeeUncEta_prodRadius, acc_prodRadius(track), perigeeUncEta, m_eventWeight);
      fillHisto(m_trk_perigeeUncPhi_prodRadius, acc_prodRadius(track), perigeeUncPhi, m_eventWeight);
      fillHisto(m_trk_perigeeUncTot_prodRadius, acc_prodRadius(track), totUnc       , m_eventWeight);
      fillHisto(m_trk_prodRadius              , acc_prodRadius(track), m_eventWeight);
      if (acc_perigeeUncTheta.isAvailable(track))
	fillHisto(m_trk_perigeeUncTheta_prodRadius, acc_prodRadius(track),  acc_perigeeUncTheta(track), m_eventWeight);
    }
  }
}

void TCCPlots::fillPerigeeVsCaloEntry(const xAOD::TrackParticle& track) {
  static SG::AuxElement::Accessor< float > acc_perigeeUncEta( "PerigeeUncEta" );
  static SG::AuxElement::Accessor< float > acc_perigeeUncTheta( "PerigeeUncTheta" );
  static SG::AuxElement::Accessor< float > acc_perigeeUncPhi( "PerigeeUncPhi" );
  static SG::AuxElement::Accessor< float > acc_caloEntryUncEta( "CaloEntryUncEta" );
  static SG::AuxElement::Accessor< float > acc_caloEntryUncTheta( "CaloEntryUncTheta" );
  static SG::AuxElement::Accessor< float > acc_caloEntryUncPhi( "CaloEntryUncPhi" );
  
  if (acc_perigeeUncEta.isAvailable(track) and acc_perigeeUncPhi.isAvailable(track) and acc_perigeeUncTheta.isAvailable(track) 
      and acc_caloEntryUncEta.isAvailable(track) and acc_caloEntryUncPhi.isAvailable(track) and acc_caloEntryUncTheta.isAvailable(track)) {
    float caloEntryUncEta = acc_caloEntryUncEta(track);
    float CaloEntryUncTheta = acc_caloEntryUncTheta(track);
    float caloEntryUncPhi = acc_caloEntryUncPhi(track);
    float caloTotUnc = sqrt(caloEntryUncEta*caloEntryUncEta+caloEntryUncPhi*caloEntryUncPhi);
    float perigeeUncEta = acc_perigeeUncEta(track);  
    float perigeeUncTheta = acc_perigeeUncTheta(track);
    float perigeeUncPhi = acc_perigeeUncPhi(track);
    float perigeeTotUnc = sqrt(perigeeUncEta*perigeeUncEta+perigeeUncPhi*perigeeUncPhi);
    fillHisto(m_trk_perigeeUncEta_caloUncEta    , caloEntryUncEta  , perigeeUncEta  , m_eventWeight);
    fillHisto(m_trk_perigeeUncTheta_caloUncTheta, CaloEntryUncTheta, perigeeUncTheta, m_eventWeight);
    fillHisto(m_trk_perigeeUncPhi_caloUncPhi    , caloEntryUncPhi  , perigeeUncPhi  , m_eventWeight);
    fillHisto(m_trk_perigeeUncTot_caloUncTot    , caloTotUnc       , perigeeTotUnc  , m_eventWeight);
  }
  
  static SG::AuxElement::Accessor< float > acc_perigeeEta( "PerigeePosEta" );
  static SG::AuxElement::Accessor< float > acc_perigeePhi( "PerigeePosPhi" );
  static SG::AuxElement::Accessor< float > acc_caloEntryEta( "CaloEntryPosEta" );
  static SG::AuxElement::Accessor< float > acc_caloEntryPhi( "CaloEntryPosPhi" );
  
  if (acc_perigeeEta.isAvailable(track) and acc_perigeePhi.isAvailable(track) and acc_caloEntryEta.isAvailable(track) and acc_caloEntryPhi.isAvailable(track)) {
    float delta_eta = acc_caloEntryEta(track) - acc_perigeeEta(track);
    float delta_phi = acc_caloEntryPhi(track) - acc_perigeePhi(track);
    
    if (delta_phi > +M_PI) delta_phi -= 2. * M_PI;
    if (delta_phi < -M_PI) delta_phi += 2. * M_PI;
               
    fillHisto(m_trk_delta_perigeeEta_caloEntryEta, delta_eta, m_eventWeight);
    fillHisto(m_trk_delta_perigeePhi_caloEntryPhi, delta_phi, m_eventWeight);
    
    fillHisto(m_trk_delta_perigeeEta_caloEntryEta_eta       , track.eta()  , delta_eta   , m_eventWeight);
    fillHisto(m_trk_delta_perigeePhi_caloEntryPhi_eta       , track.eta()  , delta_phi   , m_eventWeight);
    
    delta_eta = acc_caloEntryEta(track) - track.eta();
    delta_phi = acc_caloEntryPhi(track) - track.phi();
    
    if (delta_phi > +M_PI) delta_phi -= 2. * M_PI;
    if (delta_phi < -M_PI) delta_phi += 2. * M_PI;
               
    fillHisto(m_trk_delta_trackEta_caloEntryEta    , delta_eta, m_eventWeight);
    fillHisto(m_trk_delta_trackPhi_caloEntryPhi    , delta_phi, m_eventWeight);
    fillHisto(m_trk_delta_trackEta_caloEntryEta_eta, track.eta()   , delta_eta, m_eventWeight);
    fillHisto(m_trk_delta_trackPhi_caloEntryPhi_eta, track.eta()   , delta_phi, m_eventWeight);
  }
  
  static SG::AuxElement::Accessor< int > acc_caloEntryCorr( "Corrected" );
  static SG::AuxElement::Accessor< float > acc_caloEntryEtaCorr( "CaloEntryPosEtaCorr" );
  static SG::AuxElement::Accessor< float > acc_caloEntryPhiCorr( "CaloEntryPosPhiCorr" );
  
  if (acc_caloEntryCorr.isAvailable(track) and acc_caloEntryCorr(track)>0) {
    float delta_eta = acc_caloEntryEtaCorr(track) - track.eta();
    float delta_phi = acc_caloEntryPhiCorr(track) - track.phi();
    
    if (delta_phi > +M_PI) delta_phi -= 2. * M_PI;    
    if (delta_phi < -M_PI) delta_phi += 2. * M_PI;
               
    fillHisto(m_trk_delta_trackEta_caloEntryEtaCorr    , delta_eta, m_eventWeight);
    fillHisto(m_trk_delta_trackPhi_caloEntryPhiCorr    , delta_phi, m_eventWeight);
    fillHisto(m_trk_delta_trackEta_caloEntryEtaCorr_eta, track.eta()   , delta_eta, m_eventWeight);
    fillHisto(m_trk_delta_trackPhi_caloEntryPhiCorr_eta, track.eta()   , delta_phi, m_eventWeight);
  }
  
 
  if (acc_caloEntryEta.isAvailable(track) and acc_caloEntryPhi.isAvailable(track) and acc_caloEntryCorr.isAvailable(track) and acc_caloEntryCorr(track)>0) {
    float delta_eta = acc_caloEntryEtaCorr(track) - acc_caloEntryEta(track);
    float delta_phi = acc_caloEntryPhiCorr(track) - acc_caloEntryPhi(track);
          
    if (delta_phi > +M_PI) delta_phi -= 2. * M_PI;    
    if (delta_phi < -M_PI) delta_phi += 2. * M_PI;
          
    fillHisto(m_trk_delta_caloEntryEta_caloEntryEtaCorr    , delta_eta, m_eventWeight);
    fillHisto(m_trk_delta_caloEntryPhi_caloEntryPhiCorr    , delta_phi, m_eventWeight);
    fillHisto(m_trk_delta_caloEntryEta_caloEntryEtaCorr_eta, track.eta()   , delta_eta, m_eventWeight);
    fillHisto(m_trk_delta_caloEntryPhi_caloEntryPhiCorr_eta, track.eta()   , delta_phi, m_eventWeight);
  }  
  
}

void TCCPlots::fillMatching(const xAOD::TrackParticle& track) {
  // I should fill stats for the tracks 
  static SG::AuxElement::Accessor< int > acc_assTool( "ParticleCaloClusterAssociationTool" );
  if (not acc_assTool.isAvailable(track)) return;
  if (acc_assTool(track)==0) return;
  
  // tracks w/ extrapolation
  
  fillHisto(m_trk_total_eta, track.eta()   , m_eventWeight);
  fillHisto(m_trk_total_pt , track.pt()/GeV, m_eventWeight);
      
  static SG::AuxElement::Accessor< float > acc_caloEntryUncEta( "CaloEntryUncEta" );
  static SG::AuxElement::Accessor< float > acc_caloEntryUncPhi( "CaloEntryUncPhi" );
  static SG::AuxElement::Accessor< float > acc_prodRadius( "ProductionRadius" );
  
  if (acc_caloEntryUncEta.isAvailable(track) and acc_caloEntryUncPhi.isAvailable(track) and acc_prodRadius.isAvailable(track)) {
    float caloEntryUncEta = acc_caloEntryUncEta(track);
    float caloEntryUncPhi = acc_caloEntryUncPhi(track);
    float totUnc = sqrt(caloEntryUncEta*caloEntryUncEta+caloEntryUncPhi*caloEntryUncPhi);
    
  //   static SG::AuxElement::Accessor< std::vector<int> > acc_matched( "IsMatched" );
  //   static SG::AuxElement::Accessor< std::vector<int> > acc_mode( "MatchingMode" );
  
    static SG::AuxElement::Accessor< int > acc_allClusters( "AllClusters" );
    static SG::AuxElement::Accessor< int > acc_clusters_dr_fix      ( "Match_deltaR_fixed"        );
    static SG::AuxElement::Accessor< int > acc_clusters_dr_var      ( "Match_deltaR_variable"     );
    
    fillHisto(m_trk_total_clusters_eta, track.eta()   , acc_allClusters(track), m_eventWeight);
    fillHisto(m_trk_total_clusters_pt , track.pt()/GeV, acc_allClusters(track), m_eventWeight);
      
    if (acc_clusters_dr_fix(track)==0) {    
      fillHisto(m_trk_notMatching_deltar_fix_eta , track.eta()   , m_eventWeight);
      fillHisto(m_trk_notMatching_deltar_fix_phi , track.phi()   , m_eventWeight);
      fillHisto(m_trk_notMatching_deltar_fix_pt  , track.pt()/GeV, m_eventWeight);
      fillHisto(m_trk_notMatching_deltar_fix_caloEntryUncTot_eta, track.eta()   , totUnc, m_eventWeight);
      fillHisto(m_trk_notMatching_deltar_fix_caloEntryUncTot_pt , track.pt()/GeV, totUnc, m_eventWeight);
      fillHisto(m_trk_notMatching_deltar_fix_caloEntryUncTot_prodRadius, acc_prodRadius(track), totUnc       , m_eventWeight);
    } else {
      fillHisto(m_trk_matching_deltar_fix_eta , track.eta()   , m_eventWeight);
      fillHisto(m_trk_matching_deltar_fix_pt  , track.pt()/GeV, m_eventWeight);
    }
    
    if (acc_clusters_dr_var(track)==0) {    
      fillHisto(m_trk_notMatching_deltar_var_eta , track.eta()   , m_eventWeight);
      fillHisto(m_trk_notMatching_deltar_var_phi , track.phi()   , m_eventWeight);
      fillHisto(m_trk_notMatching_deltar_var_pt  , track.pt()/GeV, m_eventWeight);
      fillHisto(m_trk_notMatching_deltar_var_caloEntryUncTot_eta, track.eta()   , totUnc, m_eventWeight);
      fillHisto(m_trk_notMatching_deltar_var_caloEntryUncTot_pt , track.pt()/GeV, totUnc, m_eventWeight);
      fillHisto(m_trk_notMatching_deltar_var_caloEntryUncTot_prodRadius, acc_prodRadius(track), totUnc       , m_eventWeight);  
    } else {
      fillHisto(m_trk_matching_deltar_var_eta , track.eta()   , m_eventWeight);
      fillHisto(m_trk_matching_deltar_var_pt  , track.pt()/GeV, m_eventWeight);
    }
    
    if (acc_clusters_dr_fix(track)==0 and acc_clusters_dr_var(track)==0) {    
      fillHisto(m_trk_notMatching_deltar_none_eta , track.eta()   , m_eventWeight);
      fillHisto(m_trk_notMatching_deltar_none_phi , track.phi()   , m_eventWeight);
      fillHisto(m_trk_notMatching_deltar_none_pt  , track.pt()/GeV, m_eventWeight);
      fillHisto(m_trk_notMatching_deltar_none_caloEntryUncTot_eta, track.eta()   , totUnc, m_eventWeight);
      fillHisto(m_trk_notMatching_deltar_none_caloEntryUncTot_pt , track.pt()/GeV, totUnc, m_eventWeight);
      fillHisto(m_trk_notMatching_deltar_none_caloEntryUncTot_prodRadius, acc_prodRadius(track), totUnc       , m_eventWeight);
    }
    
    if (acc_clusters_dr_fix(track)>0 and acc_clusters_dr_var(track)>0) {
      fillHisto(m_trk_matching_deltar_fix_and_var_eta  , track.eta()   , m_eventWeight);
      fillHisto(m_trk_matching_deltar_fix_and_var_pt   , track.pt()/GeV, m_eventWeight);
    }
    
    if (acc_clusters_dr_fix(track)>0 or acc_clusters_dr_var(track)>0) {
      fillHisto(m_trk_matching_deltar_fix_or_var_eta   , track.eta()   , m_eventWeight);
      fillHisto(m_trk_matching_deltar_fix_or_var_pt    , track.pt()/GeV, m_eventWeight);
    }
    
    if (acc_clusters_dr_fix(track)>0 and acc_clusters_dr_var(track)==0) {
      fillHisto(m_trk_matching_deltar_onlyfix_eta      , track.eta()   , m_eventWeight);
      fillHisto(m_trk_matching_deltar_onlyfix_pt       , track.pt()/GeV, m_eventWeight);
    }
    
    if (acc_clusters_dr_fix(track)==0 and acc_clusters_dr_var(track)>0) {
      fillHisto(m_trk_matching_deltar_onlyvar_eta      , track.eta()   , m_eventWeight);
      fillHisto(m_trk_matching_deltar_onlyvar_pt       , track.pt()/GeV, m_eventWeight);
    }
    
    static SG::AuxElement::Accessor< int > acc_clusters_dr_or       ( "Match_deltaR"             );
    static SG::AuxElement::Accessor< int > acc_clusters_dr_and      ( "Match_deltaR_both"        );
    static SG::AuxElement::Accessor< int > acc_clusters_dr_fix_only ( "Match_deltaR_fixedOnly"   );
    static SG::AuxElement::Accessor< int > acc_clusters_dr_var_only ( "Match_deltaR_variableOnly");
    
    // matching dr fixed or dr variable
    fillHisto(m_trk_matching_deltar_or_clusters_eta , track.eta()   , acc_clusters_dr_or(track), m_eventWeight);
    fillHisto(m_trk_matching_deltar_or_clusters_phi , track.phi()   , acc_clusters_dr_or(track), m_eventWeight);
    fillHisto(m_trk_matching_deltar_or_clusters_pt  , track.pt()/GeV, acc_clusters_dr_or(track), m_eventWeight);
    
    // matching dr fixed and dr variable
    fillHisto(m_trk_matching_deltar_and_clusters_eta , track.eta()   , acc_clusters_dr_and(track), m_eventWeight);
    fillHisto(m_trk_matching_deltar_and_clusters_phi , track.phi()   , acc_clusters_dr_and(track), m_eventWeight);
    fillHisto(m_trk_matching_deltar_and_clusters_pt  , track.pt()/GeV, acc_clusters_dr_and(track), m_eventWeight);
      
    //matching only dr fix
    fillHisto(m_trk_matching_deltar_fix_only_clusters_eta , track.eta()   , acc_clusters_dr_fix_only(track), m_eventWeight);
    fillHisto(m_trk_matching_deltar_fix_only_clusters_phi , track.phi()   , acc_clusters_dr_fix_only(track), m_eventWeight);
    fillHisto(m_trk_matching_deltar_fix_only_clusters_pt  , track.pt()/GeV, acc_clusters_dr_fix_only(track), m_eventWeight);
    
    //matching only dr variable
    fillHisto(m_trk_matching_deltar_var_only_clusters_eta , track.eta()   , acc_clusters_dr_var_only(track), m_eventWeight);
    fillHisto(m_trk_matching_deltar_var_only_clusters_phi , track.phi()   , acc_clusters_dr_var_only(track), m_eventWeight);
    fillHisto(m_trk_matching_deltar_var_only_clusters_pt  , track.pt()/GeV, acc_clusters_dr_var_only(track), m_eventWeight);
  }     
  
  static SG::AuxElement::Accessor< int > acc_trk_PV0 ( "IsPV0Compatible" );
  if (acc_trk_PV0.isAvailable(track) and acc_trk_PV0(track)==1) {
    
    fillHisto(m_trk_pv0_total_eta, track.eta()   , m_eventWeight);
    fillHisto(m_trk_pv0_total_pt , track.pt()/GeV, m_eventWeight);
    
    if (acc_caloEntryUncEta.isAvailable(track) and acc_caloEntryUncPhi.isAvailable(track) and acc_prodRadius.isAvailable(track)) {
      float caloEntryUncEta = acc_caloEntryUncEta(track);
      float caloEntryUncPhi = acc_caloEntryUncPhi(track);
      float totUnc = sqrt(caloEntryUncEta*caloEntryUncEta+caloEntryUncPhi*caloEntryUncPhi);
      
      fillHisto(m_trk_pv0_caloEntryUncTot_eta, track.eta()   , totUnc, m_eventWeight);
      fillHisto(m_trk_pv0_caloEntryUncTot_pt , track.pt()/GeV, totUnc, m_eventWeight);
     
      uint8_t nPixHits (0);
      uint8_t nSCTHits (0);
      
      if (track.summaryValue(nPixHits, xAOD::numberOfPixelHits) and track.summaryValue(nSCTHits, xAOD::numberOfSCTHits)) {
	int siHits = nPixHits + nSCTHits;
	if (siHits == 0) {
	  fillHisto(m_trk_pv0_caloEntryUncTot_eta_0SiHits      , track.eta()   , totUnc, m_eventWeight);
	  fillHisto(m_trk_pv0_caloEntryUncTot_pt_0SiHits       , track.pt()/GeV, totUnc, m_eventWeight);
	  fillHisto(m_trk_pv0_caloEntryUncTot_rFirstHit_0SiHits, track.radiusOfFirstHit(), totUnc, m_eventWeight);
	}
	fillHisto(m_trk_pv0_caloEntryUncTot_nSiHits, siHits, totUnc, m_eventWeight);
      }

      fillHisto(m_trk_pv0_caloEntryUncTot_rFirstHit, track.radiusOfFirstHit(), totUnc, m_eventWeight); 
                  
      static SG::AuxElement::Accessor< int > acc_allClusters     ( "AllClusters"           );
      static SG::AuxElement::Accessor< int > acc_clusters_dr_fix ( "Match_deltaR_fixed"    );
      static SG::AuxElement::Accessor< int > acc_clusters_dr_var ( "Match_deltaR_variable" );
      
      fillHisto(m_trk_pv0_total_clusters_eta, track.eta()   , acc_allClusters(track), m_eventWeight);
      fillHisto(m_trk_pv0_total_clusters_pt , track.pt()/GeV, acc_allClusters(track), m_eventWeight);
      
      if (acc_clusters_dr_fix(track)>0) {    
	fillHisto(m_trk_pv0_matching_deltar_fix_eta , track.eta()   , m_eventWeight);
	fillHisto(m_trk_pv0_matching_deltar_fix_pt  , track.pt()/GeV, m_eventWeight);
      } else {
	fillHisto(m_trk_pv0_notMatching_deltar_fix_eta , track.eta()   , m_eventWeight);
	fillHisto(m_trk_pv0_notMatching_deltar_fix_pt  , track.pt()/GeV, m_eventWeight);	
      }
      
      if (acc_clusters_dr_var(track)>0) {    
	fillHisto(m_trk_pv0_matching_deltar_var_eta , track.eta()   , m_eventWeight);
	fillHisto(m_trk_pv0_matching_deltar_var_pt  , track.pt()/GeV, m_eventWeight);
      } else {
	fillHisto(m_trk_pv0_notMatching_deltar_var_eta , track.eta()   , m_eventWeight);
	fillHisto(m_trk_pv0_notMatching_deltar_var_pt  , track.pt()/GeV, m_eventWeight);	
      }
      
      if (acc_clusters_dr_fix(track)==0 and acc_clusters_dr_var(track)==0) {    
	fillHisto(m_trk_pv0_notMatching_deltar_none_eta , track.eta()   , m_eventWeight);
	fillHisto(m_trk_pv0_notMatching_deltar_none_pt  , track.pt()/GeV, m_eventWeight);
      }
      
      if (acc_clusters_dr_fix(track)>0 and acc_clusters_dr_var(track)>0) {
	fillHisto(m_trk_pv0_matching_deltar_fix_and_var_eta  , track.eta()   , m_eventWeight);
	fillHisto(m_trk_pv0_matching_deltar_fix_and_var_pt   , track.pt()/GeV, m_eventWeight);
      }
      
      if (acc_clusters_dr_fix(track)>0 or acc_clusters_dr_var(track)>0) {
	fillHisto(m_trk_pv0_matching_deltar_fix_or_var_eta   , track.eta()   , m_eventWeight);
	fillHisto(m_trk_pv0_matching_deltar_fix_or_var_pt    , track.pt()/GeV, m_eventWeight);
      }
      
      if (acc_clusters_dr_fix(track)>0 and acc_clusters_dr_var(track)==0) {
	fillHisto(m_trk_pv0_matching_deltar_onlyfix_eta      , track.eta()   , m_eventWeight);
	fillHisto(m_trk_pv0_matching_deltar_onlyfix_pt       , track.pt()/GeV, m_eventWeight);
      }
      
      if (acc_clusters_dr_fix(track)==0 and acc_clusters_dr_var(track)>0) {
	fillHisto(m_trk_pv0_matching_deltar_onlyvar_eta      , track.eta()   , m_eventWeight);
	fillHisto(m_trk_pv0_matching_deltar_onlyvar_pt       , track.pt()/GeV, m_eventWeight);
      }
    }
  }
}

void TCCPlots::fillClusterEtaCut(const xAOD::CaloCluster& cluster) {
  // here I fill the clusters
  static SG::AuxElement::Accessor< int > acc_assTool( "ParticleCaloClusterAssociationTool" );
  if (not acc_assTool.isAvailable(cluster)) return;
  if (acc_assTool(cluster)==0) return;
  
  static SG::AuxElement::Accessor< float > acc_eta( "ClusterEta" );
  static SG::AuxElement::Accessor< float > acc_unc( "ClusterUnc" );
  
  static SG::AuxElement::Accessor< float > acc_weight( "ClusterWeight" );
  static SG::AuxElement::Accessor< float > acc_calE( "calE" );
   
  fillHisto(m_clusters_etacut_eta                     , acc_eta(cluster), acc_weight(cluster)*m_eventWeight);
  fillHisto(m_clusters_etacut_abs_eta                 , acc_eta(cluster), m_eventWeight);
  fillHisto(m_clusters_etacut_width                   , acc_unc(cluster), acc_weight(cluster)*m_eventWeight);
  fillHisto(m_clusters_etacut_abs_width               , acc_unc(cluster), m_eventWeight);
  fillHisto(m_clusters_etacut_width_eta               , acc_eta(cluster), acc_unc(cluster), acc_weight(cluster)*m_eventWeight);
  fillHisto(m_clusters_etacut_abs_width_eta           , acc_eta(cluster), acc_unc(cluster), m_eventWeight);
  if (acc_calE.isAvailable(cluster)) {
    fillHisto(m_clusters_etacut_energy                , cluster.e()/GeV, acc_weight(cluster)*m_eventWeight);
    fillHisto(m_clusters_etacut_abs_energy            , cluster.e()/GeV, m_eventWeight);
    fillHisto(m_clusters_etacut_width_energy          , cluster.e()/GeV, acc_unc(cluster), acc_weight(cluster)*m_eventWeight);
    fillHisto(m_clusters_etacut_abs_width_energy      , cluster.e()/GeV, acc_unc(cluster), m_eventWeight);  
  }
}

void TCCPlots::fillCluster(const xAOD::CaloCluster& cluster) {
  // here I fill the clusters
  static SG::AuxElement::Accessor< int > acc_assTool( "ParticleCaloClusterAssociationTool" );
  if (not acc_assTool.isAvailable(cluster)) return;
  if (acc_assTool(cluster)==0) return;
  
  static SG::AuxElement::Accessor< float > acc_eta( "ClusterEta" );
  static SG::AuxElement::Accessor< float > acc_unc( "ClusterUnc" );
  
  static SG::AuxElement::Accessor< float > acc_weight( "ClusterWeight" );
  static SG::AuxElement::Accessor< float > acc_calE( "calE" );
   
  fillHisto(m_clusters_eta                     , acc_eta(cluster), acc_weight(cluster)*m_eventWeight);
  fillHisto(m_clusters_abs_eta                 , acc_eta(cluster), m_eventWeight);
  fillHisto(m_clusters_width                   , acc_unc(cluster), acc_weight(cluster)*m_eventWeight);
  fillHisto(m_clusters_abs_width               , acc_unc(cluster), m_eventWeight);
  fillHisto(m_clusters_width_eta               , acc_eta(cluster), acc_unc(cluster), acc_weight(cluster)*m_eventWeight);
  fillHisto(m_clusters_abs_width_eta           , acc_eta(cluster), acc_unc(cluster), m_eventWeight);
  if (acc_calE.isAvailable(cluster)) {
    fillHisto(m_clusters_energy                , cluster.e()/GeV, acc_weight(cluster)*m_eventWeight);
    fillHisto(m_clusters_abs_energy            , cluster.e()/GeV, m_eventWeight);
    fillHisto(m_clusters_width_energy          , cluster.e()/GeV, acc_unc(cluster), acc_weight(cluster)*m_eventWeight);
    fillHisto(m_clusters_abs_width_energy      , cluster.e()/GeV, acc_unc(cluster), m_eventWeight);  
  }
  
  if (fabs(cluster.eta())<2.5) {
    fillHisto(m_clusters_etacut_eta                     , acc_eta(cluster), acc_weight(cluster)*m_eventWeight);
    fillHisto(m_clusters_etacut_abs_eta                 , acc_eta(cluster), m_eventWeight);
    fillHisto(m_clusters_etacut_width                   , acc_unc(cluster), acc_weight(cluster)*m_eventWeight);
    fillHisto(m_clusters_etacut_abs_width               , acc_unc(cluster), m_eventWeight);
    fillHisto(m_clusters_etacut_width_eta               , acc_eta(cluster), acc_unc(cluster), acc_weight(cluster)*m_eventWeight);
    fillHisto(m_clusters_etacut_abs_width_eta           , acc_eta(cluster), acc_unc(cluster), m_eventWeight);
    if (acc_calE.isAvailable(cluster)) {
      fillHisto(m_clusters_etacut_energy                , cluster.e()/GeV, acc_weight(cluster)*m_eventWeight);
      fillHisto(m_clusters_etacut_abs_energy            , cluster.e()/GeV, m_eventWeight);
      fillHisto(m_clusters_etacut_width_energy          , cluster.e()/GeV, acc_unc(cluster), acc_weight(cluster)*m_eventWeight);
      fillHisto(m_clusters_etacut_abs_width_energy      , cluster.e()/GeV, acc_unc(cluster), m_eventWeight);  
    }
  }  
  
  static SG::AuxElement::Accessor< int > acc_matched( "ClusterMatched" );
  if (acc_matched(cluster)==1) {
    fillHisto(m_clusters_matched_eta             , acc_eta(cluster), acc_weight(cluster)*m_eventWeight);
    fillHisto(m_clusters_abs_matched_eta         , acc_eta(cluster), m_eventWeight);
    if (acc_calE.isAvailable(cluster)) {
      fillHisto(m_clusters_matched_energy             , cluster.e()/GeV, acc_weight(cluster)*m_eventWeight);
      fillHisto(m_clusters_abs_matched_energy         , cluster.e()/GeV, m_eventWeight);    
    }
  } else {
    fillHisto(m_clusters_notMatched_eta          , acc_eta(cluster), acc_weight(cluster)*m_eventWeight);  
    fillHisto(m_clusters_abs_notMatched_eta      , acc_eta(cluster), m_eventWeight);  
    if (acc_calE.isAvailable(cluster)) {
      fillHisto(m_clusters_notMatched_energy          , cluster.e()/GeV, acc_weight(cluster)*m_eventWeight);  
      fillHisto(m_clusters_abs_notMatched_energy      , cluster.e()/GeV, m_eventWeight);  
    }
  }
  
  static SG::AuxElement::Accessor< int > acc_dr_fix_match( "ClusterMatchedFixedDeltaR" );
  static SG::AuxElement::Accessor< int > acc_dr_var_match ( "ClusterMatchedVariableDeltaR" );
  
  
  if (acc_dr_fix_match(cluster)==1 and acc_dr_var_match(cluster)==1) {
    fillHisto(m_clusters_matched_eta_fix_and_var      , acc_eta(cluster), acc_weight(cluster)*m_eventWeight);
    fillHisto(m_clusters_abs_matched_eta_fix_and_var  , acc_eta(cluster), m_eventWeight);
  }
  
  if (acc_dr_fix_match(cluster)==1 or acc_dr_var_match(cluster)==1) {
    fillHisto(m_clusters_matched_eta_fix_or_var      , acc_eta(cluster), acc_weight(cluster)*m_eventWeight);
    fillHisto(m_clusters_abs_matched_eta_fix_or_var  , acc_eta(cluster), m_eventWeight);
  }
  
  if (acc_dr_fix_match(cluster)==1) {
    fillHisto(m_clusters_matched_eta_fix         , acc_eta(cluster), acc_weight(cluster)*m_eventWeight);
    fillHisto(m_clusters_abs_matched_eta_fix     , acc_eta(cluster), m_eventWeight);
  } else {
    fillHisto(m_clusters_matched_eta_notfix      , acc_eta(cluster), acc_weight(cluster)*m_eventWeight);
    fillHisto(m_clusters_abs_matched_eta_notfix  , acc_eta(cluster), m_eventWeight);
  }
  
  if (acc_dr_var_match(cluster)==1) {
    fillHisto(m_clusters_matched_eta_var         , acc_eta(cluster), acc_weight(cluster)*m_eventWeight);
    fillHisto(m_clusters_abs_matched_eta_var     , acc_eta(cluster), m_eventWeight);
  } else {
    fillHisto(m_clusters_matched_eta_notvar      , acc_eta(cluster), acc_weight(cluster)*m_eventWeight);
    fillHisto(m_clusters_abs_matched_eta_notvar  , acc_eta(cluster), m_eventWeight);
  }
      
  if (acc_dr_fix_match(cluster)==0 and acc_dr_var_match(cluster)==1) {
    fillHisto(m_clusters_matched_eta_onlyvar     , acc_eta(cluster), acc_weight(cluster)*m_eventWeight);
    fillHisto(m_clusters_abs_matched_eta_onlyvar , acc_eta(cluster), m_eventWeight);
  }
  
  if (acc_dr_fix_match(cluster)==1 and acc_dr_var_match(cluster)==0) {
    fillHisto(m_clusters_matched_eta_onlyfix     , acc_eta(cluster), acc_weight(cluster)*m_eventWeight);
    fillHisto(m_clusters_abs_matched_eta_onlyfix , acc_eta(cluster), m_eventWeight);
  }
  
  if (acc_dr_fix_match(cluster)==0 and acc_dr_var_match(cluster)==0) {
    fillHisto(m_clusters_matched_eta_none        , acc_eta(cluster), acc_weight(cluster)*m_eventWeight);
    fillHisto(m_clusters_abs_matched_eta_none    , acc_eta(cluster), m_eventWeight);
  }
  
  // match with tracks from PV0
  static SG::AuxElement::Accessor< int > acc_dr_fix_PV0( "FixedMatchPV0" );
  static SG::AuxElement::Accessor< int > acc_dr_var_PV0( "VarMatchPV0"   );
  
  if (acc_dr_fix_PV0.isAvailable(cluster) and acc_dr_var_PV0.isAvailable(cluster)) {
    bool matchFix = (acc_dr_fix_match(cluster)==1 and acc_dr_fix_PV0(cluster)==1);
    bool matchVar = (acc_dr_var_match(cluster)==1 and acc_dr_var_PV0(cluster)==1);
    
    if (matchFix and matchVar) {
      fillHisto(m_clusters_matched_pv0_eta_fix_and_var      , acc_eta(cluster), acc_weight(cluster)*m_eventWeight);
      fillHisto(m_clusters_abs_matched_pv0_eta_fix_and_var  , acc_eta(cluster), m_eventWeight);
    }
    
    if (matchFix or matchVar) {
      fillHisto(m_clusters_matched_pv0_eta_fix_or_var      , acc_eta(cluster), acc_weight(cluster)*m_eventWeight);
      fillHisto(m_clusters_abs_matched_pv0_eta_fix_or_var  , acc_eta(cluster), m_eventWeight);
    }
    
    if (matchFix) {
      fillHisto(m_clusters_matched_pv0_eta_fix         , acc_eta(cluster), acc_weight(cluster)*m_eventWeight);
      fillHisto(m_clusters_abs_matched_pv0_eta_fix     , acc_eta(cluster), m_eventWeight);
    } else {
      fillHisto(m_clusters_matched_pv0_eta_notfix      , acc_eta(cluster), acc_weight(cluster)*m_eventWeight);
      fillHisto(m_clusters_abs_matched_pv0_eta_notfix  , acc_eta(cluster), m_eventWeight);
    }
    
    if (matchVar) {
      fillHisto(m_clusters_matched_pv0_eta_var         , acc_eta(cluster), acc_weight(cluster)*m_eventWeight);
      fillHisto(m_clusters_abs_matched_pv0_eta_var     , acc_eta(cluster), m_eventWeight);
    } else {
      fillHisto(m_clusters_matched_pv0_eta_notvar      , acc_eta(cluster), acc_weight(cluster)*m_eventWeight);
      fillHisto(m_clusters_abs_matched_pv0_eta_notvar  , acc_eta(cluster), m_eventWeight);
    }
    
    if (not matchFix and matchVar) {
      fillHisto(m_clusters_matched_pv0_eta_onlyvar     , acc_eta(cluster), acc_weight(cluster)*m_eventWeight);
      fillHisto(m_clusters_abs_matched_pv0_eta_onlyvar , acc_eta(cluster), m_eventWeight);
    }
    
    if (matchFix and not matchVar) {
      fillHisto(m_clusters_matched_pv0_eta_onlyfix     , acc_eta(cluster), acc_weight(cluster)*m_eventWeight);
      fillHisto(m_clusters_abs_matched_pv0_eta_onlyfix , acc_eta(cluster), m_eventWeight);
    }
    
    if (not matchFix and not matchVar) {
      fillHisto(m_clusters_matched_pv0_eta_none        , acc_eta(cluster), acc_weight(cluster)*m_eventWeight);
      fillHisto(m_clusters_abs_matched_pv0_eta_none    , acc_eta(cluster), m_eventWeight);
    }
  }  
  
  // match with tracks from PVX
  static SG::AuxElement::Accessor< int > acc_dr_fix_PVX( "FixedMatchPVX" );
  static SG::AuxElement::Accessor< int > acc_dr_var_PVX( "VarMatchPVX"   );
  
  if (acc_dr_fix_PVX.isAvailable(cluster) and acc_dr_var_PVX.isAvailable(cluster)
    and acc_dr_fix_PV0.isAvailable(cluster) and acc_dr_var_PV0.isAvailable(cluster)) {
    bool matchFix = (acc_dr_fix_match(cluster)==1 and acc_dr_fix_PVX(cluster)==1 and acc_dr_fix_PV0(cluster)==0);
    bool matchVar = (acc_dr_var_match(cluster)==1 and acc_dr_var_PVX(cluster)==1 and acc_dr_var_PV0(cluster)==0);
    
    if (matchFix and matchVar) {
      fillHisto(m_clusters_matched_pvx_eta_fix_and_var      , acc_eta(cluster), acc_weight(cluster)*m_eventWeight);
      fillHisto(m_clusters_abs_matched_pvx_eta_fix_and_var  , acc_eta(cluster), m_eventWeight);
    }
    
    if (matchFix or matchVar) {
      fillHisto(m_clusters_matched_pvx_eta_fix_or_var      , acc_eta(cluster), acc_weight(cluster)*m_eventWeight);
      fillHisto(m_clusters_abs_matched_pvx_eta_fix_or_var  , acc_eta(cluster), m_eventWeight);
    }
    
    if (matchFix) {
      fillHisto(m_clusters_matched_pvx_eta_fix         , acc_eta(cluster), acc_weight(cluster)*m_eventWeight);
      fillHisto(m_clusters_abs_matched_pvx_eta_fix     , acc_eta(cluster), m_eventWeight);
    } else {
      fillHisto(m_clusters_matched_pvx_eta_notfix      , acc_eta(cluster), acc_weight(cluster)*m_eventWeight);
      fillHisto(m_clusters_abs_matched_pvx_eta_notfix  , acc_eta(cluster), m_eventWeight);
    }
    
    if (matchVar) {
      fillHisto(m_clusters_matched_pvx_eta_var         , acc_eta(cluster), acc_weight(cluster)*m_eventWeight);
      fillHisto(m_clusters_abs_matched_pvx_eta_var     , acc_eta(cluster), m_eventWeight);
    } else {
      fillHisto(m_clusters_matched_pvx_eta_notvar      , acc_eta(cluster), acc_weight(cluster)*m_eventWeight);
      fillHisto(m_clusters_abs_matched_pvx_eta_notvar  , acc_eta(cluster), m_eventWeight);
    }
    
    if (not matchFix and matchVar) {
      fillHisto(m_clusters_matched_pvx_eta_onlyvar     , acc_eta(cluster), acc_weight(cluster)*m_eventWeight);
      fillHisto(m_clusters_abs_matched_pvx_eta_onlyvar , acc_eta(cluster), m_eventWeight);
    }
    
    if (matchFix and not matchVar) {
      fillHisto(m_clusters_matched_pvx_eta_onlyfix     , acc_eta(cluster), acc_weight(cluster)*m_eventWeight);
      fillHisto(m_clusters_abs_matched_pvx_eta_onlyfix , acc_eta(cluster), m_eventWeight);
    }
    
    if (not matchFix and not matchVar) {
      fillHisto(m_clusters_matched_pvx_eta_none        , acc_eta(cluster), acc_weight(cluster)*m_eventWeight);
      fillHisto(m_clusters_abs_matched_pvx_eta_none    , acc_eta(cluster), m_eventWeight);
    }
  }  
  
  static SG::AuxElement::Accessor< int > acc_neutral( "isNeutral" );
  static SG::AuxElement::Accessor< float > acc_pt_fraction( "ptFraction" );
  static SG::AuxElement::Accessor< int > acc_rejected( "isRejected" );
  static SG::AuxElement::Accessor< int > acc_isPV0( "isPV0" );
  static SG::AuxElement::Accessor< int > acc_isPVX( "isPVX" );
  
  if (acc_calE.isAvailable(cluster) and acc_neutral.isAvailable(cluster) and acc_neutral(cluster)==1) {
    fillHisto(m_clusters_pt_fraction_e, cluster.e()/GeV, acc_pt_fraction(cluster), m_eventWeight);
    if (acc_isPV0(cluster)==1)
      fillHisto(m_clusters_PV0_pt_fraction_e, cluster.e()/GeV, acc_pt_fraction(cluster), m_eventWeight);
    else
      fillHisto(m_clusters_PVX_pt_fraction_e, cluster.e()/GeV, acc_pt_fraction(cluster), m_eventWeight);
    if (acc_rejected(cluster)==1)
      fillHisto(m_clusters_rejected_pt_fraction_e, cluster.e()/GeV, acc_pt_fraction(cluster), m_eventWeight);
  
    fillHisto(m_clusters_all_neutral_e           , cluster.e()/GeV , m_eventWeight);
    if (acc_isPV0(cluster)==1)
      fillHisto(m_clusters_all_neutral_PV0_e     , cluster.e()/GeV , m_eventWeight);
    else if (acc_isPVX(cluster)==1)                                                        
      fillHisto(m_clusters_all_neutral_notPV0_e  , cluster.e()/GeV , m_eventWeight);
    else 
      fillHisto(m_clusters_all_neutral_neutral_e , cluster.e()/GeV , m_eventWeight);
    
    fillHisto(m_clusters_all_neutral_eta          , acc_eta(cluster), m_eventWeight);
    if (acc_isPV0(cluster)==1)
      fillHisto(m_clusters_all_neutral_PV0_eta    , acc_eta(cluster), m_eventWeight);
    else if (acc_isPVX(cluster)==1)
      fillHisto(m_clusters_all_neutral_notPV0_eta , acc_eta(cluster), m_eventWeight);
    else
      fillHisto(m_clusters_all_neutral_neutral_eta, acc_eta(cluster), m_eventWeight);
    
  }  
}

void TCCPlots::fillTCCptCut(const xAOD::TrackCaloCluster& tcc) {
    fillHisto(m_tcc_eta_ptcut      , tcc.eta()   , m_eventWeight);
    fillHisto(m_tcc_taste_eta_ptcut, tcc.eta(), tcc.taste(), m_eventWeight);
}

void TCCPlots::fillTCCetaCut(const xAOD::TrackCaloCluster& tcc) {
    fillHisto(m_tcc_pt_etacut      , tcc.pt()/GeV, m_eventWeight);
    fillHisto(m_tcc_taste_pt_etacut, tcc.pt()/GeV, tcc.taste(), m_eventWeight);
}

void TCCPlots::fillTCC(const xAOD::TrackCaloCluster& tcc, std::vector<const xAOD::TrackParticle*>& alltracksPV0) {
    fillHisto(m_tcc_eta  , tcc.eta()   , m_eventWeight);
    fillHisto(m_tcc_phi  , tcc.phi()   , m_eventWeight);
    fillHisto(m_tcc_pt   , tcc.pt()/GeV, m_eventWeight);
    fillHisto(m_tcc_m    , tcc.m()/GeV , m_eventWeight);
    fillHisto(m_tcc_taste, tcc.taste() , m_eventWeight);
    fillHisto(m_tcc_taste_eta, tcc.eta()     , tcc.taste(), m_eventWeight);
    fillHisto(m_tcc_taste_pt , tcc.pt()/GeV  , tcc.taste(), m_eventWeight);
    
    const xAOD::TrackParticle* track = getTrackParticlePtr(tcc);
    if (track) {
      fillHisto(m_tcc_pt_pseudoresponse, tcc.pt()/track->pt()          , m_eventWeight);
      fillHisto(m_tcc_pt_track_pt, tcc.pt()/GeV, track->pt()/GeV , m_eventWeight);
      
      const xAOD::TruthParticle* assTruth = getTruthPtr(*track);
      if (assTruth) {
	fillHisto(m_tcc_pt_response, tcc.pt()/assTruth->pt()          , m_eventWeight);
	fillHisto(m_tcc_pt_truth_pt, tcc.pt()/GeV, assTruth->pt()/GeV , m_eventWeight);
	
        fillHisto(m_trk_tcc_reco_pt_truth_pt, track->pt()/GeV, assTruth->pt()/GeV, m_eventWeight);
        fillHisto(m_trk_tcc_reco_pt_response, track->pt()/assTruth->pt()         , m_eventWeight);
      }
      
      
      //- There is one track matching a given number of clusters , N : size of the caloClusterLinks()
      //- Each of those clusters matches a given number of tracks, M : size of MatchingTracks
      //std::cout << "Size of the cluster vector: "<< tcc.caloClusterLinks().size() << std::endl;    
      
      static SG::AuxElement::Accessor< float > acc_unc( "ClusterUnc" );

      if (tcc.caloClusterLinks().size()) {
	int N = tcc.caloClusterLinks().size();
// 	std::cout << "N = " << N << std::endl;
	for (size_t c = 0; c < tcc.caloClusterLinks().size(); ++c) {
	  const xAOD::CaloCluster* cluster = *(tcc.caloClusterLinks().at(c));
// 	  std::cout << "cluster " << c << " --> " << cluster->eta() << ", " << cluster->phi() << "   ----   " << acc_unc(*cluster) << std::endl;
	  static SG::AuxElement::Accessor< std::vector<ElementLink<xAOD::TrackParticleContainer>> > acc_links( "MatchingTracks" );
	  if (acc_links.isAvailable(*cluster)) {
	    int M = acc_links(*cluster).size();
	    //std::cout << "Size of the track vector: "<< acc_links(*cluster).size() << std::endl;  
	    fillHisto(m_tcc_N_M, N, M, m_eventWeight);
	    int MPV0 = 0;
	    for (size_t t = 0; t < acc_links(*cluster).size(); ++t) {
	      const xAOD::TrackParticle* track = *(acc_links(*cluster).at(t));
	      if (std::find(alltracksPV0.begin(), alltracksPV0.end(), track) != alltracksPV0.end()) {
		MPV0++;
// 		std::cout << "PV0 track " << t << " --> " << track->pt() << ", " << track->eta() << ", " << track->phi() << ", " << track->m() << std::endl;
	      } 
// 		else std::cout << "PVX track " << t << " --> " << track->pt() << ", " << track->eta() << ", " << track->phi() << ", " << track->m() << std::endl; 
	    }
	    fillHisto(m_tcc_N_M_pv0, N, MPV0, m_eventWeight);
// 	    std::cout << "M PV0  = " << MPV0 << " ---- M = " << M << std::endl;
	  }
	} // for caloClusterLinks
      } // if caloClusterLinks().size
    }
}

void TCCPlots::finalizePlots() {
  if (m_collectionType!= "")
    std::cout << "Finalising " << m_collectionType << " in folder " << m_folder << std::endl;
  
  if (m_collectionType == "jets") {
    
    std::vector < TH2* > th2f = { m_jet_response_m_pt, m_jet_response_m_pt_2leadings, 
      m_jet_response_m_pt_leading, m_jet_response_m_pt_subleading, 
      m_jet_response_d2_pt, m_jet_response_d2_pt_2leadings, 
      m_jet_response_d2_pt_leading, m_jet_response_d2_pt_subleading,
      m_jet_response_add_d2_pt, m_jet_response_add_d2_pt_2leadings, 
      m_jet_response_add_d2_pt_leading, m_jet_response_add_d2_pt_subleading,
      m_jet_response_add_eta_pt, m_jet_response_add_eta_pt_2leadings, 
      m_jet_response_add_eta_pt_leading, m_jet_response_add_eta_pt_subleading      
    };
      
    std::vector < TH3* > th3f = { m_jet_mopt_pt_response_m, m_jet_mopt_pt_response_m_2leadings,
      m_jet_mopt_pt_response_m_leading, m_jet_mopt_pt_response_m_subleading,
      m_jet_mopt_pt_response_pt, m_jet_mopt_pt_response_pt_2leadings,
      m_jet_mopt_pt_response_pt_leading, m_jet_mopt_pt_response_pt_subleading,
      m_jet_mopt_pt_response_eta, m_jet_mopt_pt_response_eta_2leadings, 
      m_jet_mopt_pt_response_eta_leading, m_jet_mopt_pt_response_eta_subleading, 
      m_jet_mopt_pt_response_phi, m_jet_mopt_pt_response_phi_2leadings,
      m_jet_mopt_pt_response_phi_leading, m_jet_mopt_pt_response_phi_subleading,
      m_jet_mopt_pt_response_d2, m_jet_mopt_pt_response_d2_2leadings,
      m_jet_mopt_pt_response_d2_leading, m_jet_mopt_pt_response_d2_subleading };
      
    for ( auto& histo : th2f ) {
      for (int j = 1; j <= histo->GetNbinsY(); j++)
	for (int i = 1; i <= histo->GetNbinsX(); i++) 
	  histo->SetBinContent(i,j,histo->GetBinContent(i,j)/(histo->GetXaxis()->GetBinWidth(i)*histo->GetYaxis()->GetBinWidth(j)));
    }
    
    for ( auto& histo : th3f ) {
      for (int k = 1; k <= histo->GetNbinsZ(); k++)
	for (int j = 1; j <= histo->GetNbinsY(); j++)
	  for (int i = 1; i <= histo->GetNbinsX(); i++) 
	    histo->SetBinContent(i,j,k,histo->GetBinContent(i,j,k)/(histo->GetXaxis()->GetBinWidth(i)*histo->GetYaxis()->GetBinWidth(j)*histo->GetZaxis()->GetBinWidth(k)));
    }

    make_median(m_jet_response_m_pt                      , m_jet_resolution_m            );
    make_median(m_jet_response_m_pt_2leadings            , m_jet_resolution_m_2leadings  );  
    make_median(m_jet_response_m_pt_leading              , m_jet_resolution_m_leading    );
    make_median(m_jet_response_m_pt_subleading           , m_jet_resolution_m_subleading );
    
    make_median(m_jet_response_d2_pt                     , m_jet_resolution_d2            );
    make_median(m_jet_response_d2_pt_2leadings           , m_jet_resolution_d2_2leadings  );  
    make_median(m_jet_response_d2_pt_leading             , m_jet_resolution_d2_leading    );
    make_median(m_jet_response_d2_pt_subleading          , m_jet_resolution_d2_subleading );
    
    make_median_add(m_jet_response_add_d2_pt             , m_jet_resolution_add_d2            );
    make_median_add(m_jet_response_add_d2_pt_2leadings   , m_jet_resolution_add_d2_2leadings  );  
    make_median_add(m_jet_response_add_d2_pt_leading     , m_jet_resolution_add_d2_leading    );
    make_median_add(m_jet_response_add_d2_pt_subleading  , m_jet_resolution_add_d2_subleading );
    
    make_median(m_jet_mopt_pt_response_m                 , m_jet_resolution_mopt_pt_m               );
    make_median(m_jet_mopt_pt_response_m_2leadings       , m_jet_resolution_mopt_pt_m_2leadings     );
    
    make_median(m_jet_mopt_pt_response_m_leading         , m_jet_resolution_mopt_pt_m_leading       );
    make_median(m_jet_mopt_pt_response_m_subleading      , m_jet_resolution_mopt_pt_m_subleading    );
    
    make_median(m_jet_mopt_pt_response_pt                , m_jet_resolution_mopt_pt_pt              );
    make_median(m_jet_mopt_pt_response_pt_2leadings      , m_jet_resolution_mopt_pt_pt_2leadings    );
    make_median(m_jet_mopt_pt_response_pt_leading        , m_jet_resolution_mopt_pt_pt_leading      );
    make_median(m_jet_mopt_pt_response_pt_subleading     , m_jet_resolution_mopt_pt_pt_subleading   );
    
    make_median(m_jet_mopt_pt_response_eta               , m_jet_resolution_mopt_pt_eta             );
    make_median(m_jet_mopt_pt_response_eta_2leadings     , m_jet_resolution_mopt_pt_eta_2leadings   );
    make_median(m_jet_mopt_pt_response_eta_leading       , m_jet_resolution_mopt_pt_eta_leading     );
    make_median(m_jet_mopt_pt_response_eta_subleading    , m_jet_resolution_mopt_pt_eta_subleading  );
    
    make_median(m_jet_mopt_pt_response_phi               , m_jet_resolution_mopt_pt_phi             );
    make_median(m_jet_mopt_pt_response_phi_2leadings     , m_jet_resolution_mopt_pt_phi_2leadings   );
    make_median(m_jet_mopt_pt_response_phi_leading       , m_jet_resolution_mopt_pt_phi_leading     );
    make_median(m_jet_mopt_pt_response_phi_subleading    , m_jet_resolution_mopt_pt_phi_subleading  );
    
    make_median(m_jet_mopt_pt_response_d2                , m_jet_resolution_mopt_pt_d2              );
    make_median(m_jet_mopt_pt_response_d2_2leadings      , m_jet_resolution_mopt_pt_d2_2leadings    );
    make_median(m_jet_mopt_pt_response_d2_leading        , m_jet_resolution_mopt_pt_d2_leading      );
    make_median(m_jet_mopt_pt_response_d2_subleading     , m_jet_resolution_mopt_pt_d2_subleading   );  
    
    make_median(m_jet_response_m_npv                      , m_jet_resolution_m_npv            , m_jet_median_width_m_npv           );
    make_median(m_jet_response_m_npv_2leadings            , m_jet_resolution_m_npv_2leadings  , m_jet_median_width_m_npv_2leadings );  
    make_median(m_jet_response_m_npv_leading              , m_jet_resolution_m_npv_leading    , m_jet_median_width_m_npv_leading   );
    make_median(m_jet_response_m_npv_subleading           , m_jet_resolution_m_npv_subleading , m_jet_median_width_m_npv_subleading);
        
  
  } else if (m_collectionType == "tracks") {
    //pt + prod radius th2
    std::vector < TH2* > th2f = { m_trk_d0_pt, m_trk_z0_pt, m_trk_theta_pt, m_trk_phi_pt, m_trk_qOverP_pt, 
      m_trk_caloEntryEtaOverEta_pt, m_trk_caloEntryUncEta_pt, m_trk_caloEntryUncTheta_pt, m_trk_caloEntryPhiOverPhi_pt,
      m_trk_caloEntryUncPhi_pt, m_trk_caloEntryUncTot_pt, m_trk_pv0_caloEntryUncTot_pt, m_trk_pv0_caloEntryUncTot_truthpt,
      m_trk_pv0_caloEntryUncTot_pt_0SiHits, m_trk_perigeeUncEta_pt, m_trk_perigeeUncTheta_pt, 
      m_trk_perigeeUncPhi_pt, m_trk_perigeeUncTot_pt, m_trk_delta_perigeeEta_caloEntryEta_pt, 
      m_trk_delta_perigeePhi_caloEntryPhi_pt, m_trk_delta_trackEta_caloEntryEta_pt, m_trk_delta_trackPhi_caloEntryPhi_pt,
      m_trk_delta_trackEta_caloEntryEtaCorr_pt, m_trk_delta_trackPhi_caloEntryPhiCorr_pt, 
      m_trk_delta_caloEntryEta_caloEntryEtaCorr_pt, m_trk_delta_caloEntryPhi_caloEntryPhiCorr_pt,
      m_trk_matching_deltar_or_clusters_pt, m_trk_matching_deltar_and_clusters_pt, 
      m_trk_matching_deltar_fix_only_clusters_pt, m_trk_matching_deltar_var_only_clusters_pt, m_trk_total_clusters_pt,
      m_trk_notMatching_deltar_fix_caloEntryUncTot_pt, m_trk_notMatching_deltar_var_caloEntryUncTot_pt, 
      m_trk_notMatching_deltar_none_caloEntryUncTot_pt,
      m_trk_perigeeUncEta_prodRadius, m_trk_perigeeUncTheta_prodRadius, m_trk_perigeeUncPhi_prodRadius,
      m_trk_perigeeUncTot_prodRadius, m_trk_caloEntryUncEta_prodRadius, m_trk_caloEntryUncTheta_prodRadius,
      m_trk_caloEntryUncPhi_prodRadius, m_trk_caloEntryUncTot_prodRadius, 
      m_trk_notMatching_deltar_fix_caloEntryUncTot_prodRadius, m_trk_notMatching_deltar_var_caloEntryUncTot_prodRadius, 
      m_trk_notMatching_deltar_none_caloEntryUncTot_prodRadius };

    //pt + prod radius th1
    std::vector < TH1* > th1f = { m_trk_total_pt, m_trk_notMatching_deltar_fix_pt,
      m_trk_notMatching_deltar_var_pt, m_trk_notMatching_deltar_none_pt, m_trk_prodRadius,
      m_trk_matching_deltar_fix_and_var_pt, m_trk_matching_deltar_fix_or_var_pt, 
      m_trk_matching_deltar_fix_pt, m_trk_matching_deltar_var_pt, 
      m_trk_matching_deltar_onlyfix_pt, m_trk_matching_deltar_onlyvar_pt,
      m_trk_pv0_total_pt, m_trk_pv0_notMatching_deltar_fix_pt,
      m_trk_pv0_notMatching_deltar_var_pt, m_trk_pv0_notMatching_deltar_none_pt,
      m_trk_pv0_matching_deltar_fix_and_var_pt, m_trk_pv0_matching_deltar_fix_or_var_pt, 
      m_trk_pv0_matching_deltar_fix_pt, m_trk_pv0_matching_deltar_var_pt, 
      m_trk_pv0_matching_deltar_onlyfix_pt, m_trk_pv0_matching_deltar_onlyvar_pt,
    };
      
     
    for ( auto& histo : th2f ) {
      for (int j = 1; j <= histo->GetNbinsY(); j++)
	for (int i = 1; i <= histo->GetNbinsX(); i++) 
	  histo->SetBinContent(i,j,histo->GetBinContent(i,j)/(histo->GetXaxis()->GetBinWidth(i)*histo->GetYaxis()->GetBinWidth(j)));
    }
     
    for ( auto& histo : th1f ) {
      for (int i = 1; i <= histo->GetNbinsX(); i++)
	histo->SetBinContent(i,histo->GetBinContent(i)/histo->GetBinWidth(i));       
    }
    
    m_trk_matchedFraction_eta_fix_and_var ->Divide( m_trk_matching_deltar_fix_and_var_eta    , m_trk_total_eta, 1., 1., "B");
    m_trk_matchedFraction_eta_fix_or_var  ->Divide( m_trk_matching_deltar_fix_or_var_eta     , m_trk_total_eta, 1., 1., "B");
    m_trk_matchedFraction_eta_fix         ->Divide( m_trk_matching_deltar_fix_eta            , m_trk_total_eta, 1., 1., "B");
    m_trk_matchedFraction_eta_notfix      ->Divide( m_trk_notMatching_deltar_fix_eta         , m_trk_total_eta, 1., 1., "B");
    m_trk_matchedFraction_eta_var         ->Divide( m_trk_matching_deltar_var_eta            , m_trk_total_eta, 1., 1., "B");
    m_trk_matchedFraction_eta_notvar      ->Divide( m_trk_notMatching_deltar_var_eta         , m_trk_total_eta, 1., 1., "B");
    m_trk_matchedFraction_eta_onlyvar     ->Divide( m_trk_matching_deltar_onlyvar_eta        , m_trk_total_eta, 1., 1., "B");
    m_trk_matchedFraction_eta_onlyfix     ->Divide( m_trk_matching_deltar_onlyfix_eta        , m_trk_total_eta, 1., 1., "B");
    m_trk_matchedFraction_eta_none        ->Divide( m_trk_notMatching_deltar_none_eta        , m_trk_total_eta, 1., 1., "B");
    
    m_trk_matchedFraction_pt_fix_and_var  ->Divide( m_trk_matching_deltar_fix_and_var_pt     , m_trk_total_pt , 1., 1., "B");
    m_trk_matchedFraction_pt_fix_or_var   ->Divide( m_trk_matching_deltar_fix_or_var_pt      , m_trk_total_pt , 1., 1., "B");
    m_trk_matchedFraction_pt_fix          ->Divide( m_trk_matching_deltar_fix_pt             , m_trk_total_pt , 1., 1., "B");
    m_trk_matchedFraction_pt_notfix       ->Divide( m_trk_notMatching_deltar_fix_pt          , m_trk_total_pt , 1., 1., "B");
    m_trk_matchedFraction_pt_var          ->Divide( m_trk_matching_deltar_var_pt             , m_trk_total_pt , 1., 1., "B");
    m_trk_matchedFraction_pt_notvar       ->Divide( m_trk_notMatching_deltar_var_pt          , m_trk_total_pt , 1., 1., "B");
    m_trk_matchedFraction_pt_onlyvar      ->Divide( m_trk_matching_deltar_onlyvar_pt         , m_trk_total_pt , 1., 1., "B");
    m_trk_matchedFraction_pt_onlyfix      ->Divide( m_trk_matching_deltar_onlyfix_pt         , m_trk_total_pt , 1., 1., "B");
    m_trk_matchedFraction_pt_none         ->Divide( m_trk_notMatching_deltar_none_pt         , m_trk_total_pt , 1., 1., "B");
    
    m_trk_pv0_matchedFraction_eta_fix_and_var ->Divide( m_trk_pv0_matching_deltar_fix_and_var_eta    , m_trk_pv0_total_eta, 1., 1., "B");
    m_trk_pv0_matchedFraction_eta_fix_or_var  ->Divide( m_trk_pv0_matching_deltar_fix_or_var_eta     , m_trk_pv0_total_eta, 1., 1., "B");
    m_trk_pv0_matchedFraction_eta_fix         ->Divide( m_trk_pv0_matching_deltar_fix_eta            , m_trk_pv0_total_eta, 1., 1., "B");
    m_trk_pv0_matchedFraction_eta_notfix      ->Divide( m_trk_pv0_notMatching_deltar_fix_eta         , m_trk_pv0_total_eta, 1., 1., "B");
    m_trk_pv0_matchedFraction_eta_var         ->Divide( m_trk_pv0_matching_deltar_var_eta            , m_trk_pv0_total_eta, 1., 1., "B");
    m_trk_pv0_matchedFraction_eta_notvar      ->Divide( m_trk_pv0_notMatching_deltar_var_eta         , m_trk_pv0_total_eta, 1., 1., "B");
    m_trk_pv0_matchedFraction_eta_onlyvar     ->Divide( m_trk_pv0_matching_deltar_onlyvar_eta        , m_trk_pv0_total_eta, 1., 1., "B");
    m_trk_pv0_matchedFraction_eta_onlyfix     ->Divide( m_trk_pv0_matching_deltar_onlyfix_eta        , m_trk_pv0_total_eta, 1., 1., "B");
    m_trk_pv0_matchedFraction_eta_none        ->Divide( m_trk_pv0_notMatching_deltar_none_eta        , m_trk_pv0_total_eta, 1., 1., "B");
         
    m_trk_pv0_matchedFraction_pt_fix_and_var  ->Divide( m_trk_pv0_matching_deltar_fix_and_var_pt     , m_trk_pv0_total_pt , 1., 1., "B");
    m_trk_pv0_matchedFraction_pt_fix_or_var   ->Divide( m_trk_pv0_matching_deltar_fix_or_var_pt      , m_trk_pv0_total_pt , 1., 1., "B");
    m_trk_pv0_matchedFraction_pt_fix          ->Divide( m_trk_pv0_matching_deltar_fix_pt             , m_trk_pv0_total_pt , 1., 1., "B");
    m_trk_pv0_matchedFraction_pt_notfix       ->Divide( m_trk_pv0_notMatching_deltar_fix_pt          , m_trk_pv0_total_pt , 1., 1., "B");
    m_trk_pv0_matchedFraction_pt_var          ->Divide( m_trk_pv0_matching_deltar_var_pt             , m_trk_pv0_total_pt , 1., 1., "B");
    m_trk_pv0_matchedFraction_pt_notvar       ->Divide( m_trk_pv0_notMatching_deltar_var_pt          , m_trk_pv0_total_pt , 1., 1., "B");
    m_trk_pv0_matchedFraction_pt_onlyvar      ->Divide( m_trk_pv0_matching_deltar_onlyvar_pt         , m_trk_pv0_total_pt , 1., 1., "B");
    m_trk_pv0_matchedFraction_pt_onlyfix      ->Divide( m_trk_pv0_matching_deltar_onlyfix_pt         , m_trk_pv0_total_pt , 1., 1., "B");
    m_trk_pv0_matchedFraction_pt_none         ->Divide( m_trk_pv0_notMatching_deltar_none_pt         , m_trk_pv0_total_pt , 1., 1., "B");
    
  } else if (m_collectionType == "clusters") {
    
    m_clusters_matchedFraction_eta             ->Divide(m_clusters_matched_eta             , m_clusters_eta, 1., 1., "B");
    m_clusters_notMatchedFraction_eta          ->Divide(m_clusters_notMatched_eta          , m_clusters_eta, 1., 1., "B");
    m_clusters_matchedFraction_energy          ->Divide(m_clusters_matched_energy          , m_clusters_energy, 1., 1., "B");
    m_clusters_notMatchedFraction_energy       ->Divide(m_clusters_notMatched_energy       , m_clusters_energy, 1., 1., "B");
    m_clusters_matchedFraction_eta_fix_and_var ->Divide(m_clusters_matched_eta_fix_and_var , m_clusters_eta, 1., 1., "B");
    m_clusters_matchedFraction_eta_fix_or_var  ->Divide(m_clusters_matched_eta_fix_or_var  , m_clusters_eta, 1., 1., "B");
    m_clusters_matchedFraction_eta_fix         ->Divide(m_clusters_matched_eta_fix         , m_clusters_eta, 1., 1., "B");
    m_clusters_matchedFraction_eta_notfix      ->Divide(m_clusters_matched_eta_notfix      , m_clusters_eta, 1., 1., "B");
    m_clusters_matchedFraction_eta_var         ->Divide(m_clusters_matched_eta_var         , m_clusters_eta, 1., 1., "B");
    m_clusters_matchedFraction_eta_notvar      ->Divide(m_clusters_matched_eta_notvar      , m_clusters_eta, 1., 1., "B");
    m_clusters_matchedFraction_eta_onlyvar     ->Divide(m_clusters_matched_eta_onlyvar     , m_clusters_eta, 1., 1., "B");
    m_clusters_matchedFraction_eta_onlyfix     ->Divide(m_clusters_matched_eta_onlyfix     , m_clusters_eta, 1., 1., "B");
    m_clusters_matchedFraction_eta_none        ->Divide(m_clusters_matched_eta_none        , m_clusters_eta, 1., 1., "B");
            
    m_clusters_abs_matchedFraction_eta             ->Divide(m_clusters_abs_matched_eta             , m_clusters_abs_eta, 1., 1., "B");
    m_clusters_abs_notMatchedFraction_eta          ->Divide(m_clusters_abs_notMatched_eta          , m_clusters_abs_eta, 1., 1., "B");
    m_clusters_abs_matchedFraction_energy          ->Divide(m_clusters_abs_matched_energy          , m_clusters_abs_energy, 1., 1., "B");
    m_clusters_abs_notMatchedFraction_energy       ->Divide(m_clusters_abs_notMatched_energy       , m_clusters_abs_energy, 1., 1., "B");
    m_clusters_abs_matchedFraction_eta_fix_and_var ->Divide(m_clusters_abs_matched_eta_fix_and_var , m_clusters_abs_eta, 1., 1., "B");
    m_clusters_abs_matchedFraction_eta_fix_or_var  ->Divide(m_clusters_abs_matched_eta_fix_or_var  , m_clusters_abs_eta, 1., 1., "B");
    m_clusters_abs_matchedFraction_eta_fix         ->Divide(m_clusters_abs_matched_eta_fix         , m_clusters_abs_eta, 1., 1., "B");
    m_clusters_abs_matchedFraction_eta_notfix      ->Divide(m_clusters_abs_matched_eta_notfix      , m_clusters_abs_eta, 1., 1., "B");
    m_clusters_abs_matchedFraction_eta_var         ->Divide(m_clusters_abs_matched_eta_var         , m_clusters_abs_eta, 1., 1., "B");
    m_clusters_abs_matchedFraction_eta_notvar      ->Divide(m_clusters_abs_matched_eta_notvar      , m_clusters_abs_eta, 1., 1., "B");
    m_clusters_abs_matchedFraction_eta_onlyvar     ->Divide(m_clusters_abs_matched_eta_onlyvar     , m_clusters_abs_eta, 1., 1., "B");
    m_clusters_abs_matchedFraction_eta_onlyfix     ->Divide(m_clusters_abs_matched_eta_onlyfix     , m_clusters_abs_eta, 1., 1., "B");
    m_clusters_abs_matchedFraction_eta_none        ->Divide(m_clusters_abs_matched_eta_none        , m_clusters_abs_eta, 1., 1., "B");
    
    m_clusters_matchedpv0Fraction_eta_fix_and_var ->Divide(m_clusters_matched_pv0_eta_fix_and_var , m_clusters_eta, 1., 1., "B");
    m_clusters_matchedpv0Fraction_eta_fix_or_var  ->Divide(m_clusters_matched_pv0_eta_fix_or_var  , m_clusters_eta, 1., 1., "B");
    m_clusters_matchedpv0Fraction_eta_fix         ->Divide(m_clusters_matched_pv0_eta_fix         , m_clusters_eta, 1., 1., "B");
    m_clusters_matchedpv0Fraction_eta_notfix      ->Divide(m_clusters_matched_pv0_eta_notfix      , m_clusters_eta, 1., 1., "B");
    m_clusters_matchedpv0Fraction_eta_var         ->Divide(m_clusters_matched_pv0_eta_var         , m_clusters_eta, 1., 1., "B");
    m_clusters_matchedpv0Fraction_eta_notvar      ->Divide(m_clusters_matched_pv0_eta_notvar      , m_clusters_eta, 1., 1., "B");
    m_clusters_matchedpv0Fraction_eta_onlyvar     ->Divide(m_clusters_matched_pv0_eta_onlyvar     , m_clusters_eta, 1., 1., "B");
    m_clusters_matchedpv0Fraction_eta_onlyfix     ->Divide(m_clusters_matched_pv0_eta_onlyfix     , m_clusters_eta, 1., 1., "B");
    m_clusters_matchedpv0Fraction_eta_none        ->Divide(m_clusters_matched_pv0_eta_none        , m_clusters_eta, 1., 1., "B");
    
    m_clusters_abs_matchedpv0Fraction_eta_fix_and_var ->Divide(m_clusters_abs_matched_pv0_eta_fix_and_var , m_clusters_abs_eta, 1., 1., "B");
    m_clusters_abs_matchedpv0Fraction_eta_fix_or_var  ->Divide(m_clusters_abs_matched_pv0_eta_fix_or_var  , m_clusters_abs_eta, 1., 1., "B");
    m_clusters_abs_matchedpv0Fraction_eta_fix         ->Divide(m_clusters_abs_matched_pv0_eta_fix         , m_clusters_abs_eta, 1., 1., "B");
    m_clusters_abs_matchedpv0Fraction_eta_notfix      ->Divide(m_clusters_abs_matched_pv0_eta_notfix      , m_clusters_abs_eta, 1., 1., "B");
    m_clusters_abs_matchedpv0Fraction_eta_var         ->Divide(m_clusters_abs_matched_pv0_eta_var         , m_clusters_abs_eta, 1., 1., "B");
    m_clusters_abs_matchedpv0Fraction_eta_notvar      ->Divide(m_clusters_abs_matched_pv0_eta_notvar      , m_clusters_abs_eta, 1., 1., "B");
    m_clusters_abs_matchedpv0Fraction_eta_onlyvar     ->Divide(m_clusters_abs_matched_pv0_eta_onlyvar     , m_clusters_abs_eta, 1., 1., "B");
    m_clusters_abs_matchedpv0Fraction_eta_onlyfix     ->Divide(m_clusters_abs_matched_pv0_eta_onlyfix     , m_clusters_abs_eta, 1., 1., "B");
    m_clusters_abs_matchedpv0Fraction_eta_none        ->Divide(m_clusters_abs_matched_pv0_eta_none        , m_clusters_abs_eta, 1., 1., "B");
    
    m_clusters_matchedpvxFraction_eta_fix_and_var ->Divide(m_clusters_matched_pvx_eta_fix_and_var , m_clusters_eta, 1., 1., "B");
    m_clusters_matchedpvxFraction_eta_fix_or_var  ->Divide(m_clusters_matched_pvx_eta_fix_or_var  , m_clusters_eta, 1., 1., "B");
    m_clusters_matchedpvxFraction_eta_fix         ->Divide(m_clusters_matched_pvx_eta_fix         , m_clusters_eta, 1., 1., "B");
    m_clusters_matchedpvxFraction_eta_notfix      ->Divide(m_clusters_matched_pvx_eta_notfix      , m_clusters_eta, 1., 1., "B");
    m_clusters_matchedpvxFraction_eta_var         ->Divide(m_clusters_matched_pvx_eta_var         , m_clusters_eta, 1., 1., "B");
    m_clusters_matchedpvxFraction_eta_notvar      ->Divide(m_clusters_matched_pvx_eta_notvar      , m_clusters_eta, 1., 1., "B");
    m_clusters_matchedpvxFraction_eta_onlyvar     ->Divide(m_clusters_matched_pvx_eta_onlyvar     , m_clusters_eta, 1., 1., "B");
    m_clusters_matchedpvxFraction_eta_onlyfix     ->Divide(m_clusters_matched_pvx_eta_onlyfix     , m_clusters_eta, 1., 1., "B");
    m_clusters_matchedpvxFraction_eta_none        ->Divide(m_clusters_matched_pvx_eta_none        , m_clusters_eta, 1., 1., "B");
    
    m_clusters_abs_matchedpvxFraction_eta_fix_and_var ->Divide(m_clusters_abs_matched_pvx_eta_fix_and_var , m_clusters_abs_eta, 1., 1., "B");
    m_clusters_abs_matchedpvxFraction_eta_fix_or_var  ->Divide(m_clusters_abs_matched_pvx_eta_fix_or_var  , m_clusters_abs_eta, 1., 1., "B");
    m_clusters_abs_matchedpvxFraction_eta_fix         ->Divide(m_clusters_abs_matched_pvx_eta_fix         , m_clusters_abs_eta, 1., 1., "B");
    m_clusters_abs_matchedpvxFraction_eta_notfix      ->Divide(m_clusters_abs_matched_pvx_eta_notfix      , m_clusters_abs_eta, 1., 1., "B");
    m_clusters_abs_matchedpvxFraction_eta_var         ->Divide(m_clusters_abs_matched_pvx_eta_var         , m_clusters_abs_eta, 1., 1., "B");
    m_clusters_abs_matchedpvxFraction_eta_notvar      ->Divide(m_clusters_abs_matched_pvx_eta_notvar      , m_clusters_abs_eta, 1., 1., "B");
    m_clusters_abs_matchedpvxFraction_eta_onlyvar     ->Divide(m_clusters_abs_matched_pvx_eta_onlyvar     , m_clusters_abs_eta, 1., 1., "B");
    m_clusters_abs_matchedpvxFraction_eta_onlyfix     ->Divide(m_clusters_abs_matched_pvx_eta_onlyfix     , m_clusters_abs_eta, 1., 1., "B");
    m_clusters_abs_matchedpvxFraction_eta_none        ->Divide(m_clusters_abs_matched_pvx_eta_none        , m_clusters_abs_eta, 1., 1., "B");
  
  } else if (m_collectionType == "tccs") {
    std::vector < TH2* > th2f = { m_tcc_taste_pt, m_tcc_taste_pt_etacut,
      m_tcc_pt_track_pt, m_trk_tcc_reco_pt_truth_pt, m_tcc_pt_truth_pt };

    //pt + prod radius th1
    std::vector < TH1* > th1f = { m_tcc_pt, m_tcc_pt_etacut };

    for ( auto& histo : th2f ) {
      for (int j = 1; j <= histo->GetNbinsY(); j++)
	for (int i = 1; i <= histo->GetNbinsX(); i++)
	  histo->SetBinContent(i,j,histo->GetBinContent(i,j)/(histo->GetXaxis()->GetBinWidth(i)*histo->GetYaxis()->GetBinWidth(j)));
    }
     
    for ( auto& histo : th1f ) {
      for (int i = 1; i <= histo->GetNbinsX(); i++)
	histo->SetBinContent(i,histo->GetBinContent(i)/histo->GetBinWidth(i));       
    }
  } else return;
  
}

void TCCPlots::make_median(TH2* h2_response, TH1* h1_resolution, TH1* h1_median){
  for (int i=1; i<=h2_response->GetNbinsX(); i++){
    TH1F* hold = new TH1F("","", h2_response->GetNbinsY(),
			  h2_response->GetYaxis()->GetBinCenter(1)-0.5*h2_response->GetYaxis()->GetBinWidth(1),
			  h2_response->GetYaxis()->GetBinCenter(h2_response->GetNbinsY())+0.5*h2_response->GetYaxis()->GetBinWidth(h2_response->GetNbinsY()));
    for (int j=1; j<=h2_response->GetNbinsY(); j++){
      hold->SetBinContent(j,h2_response->GetBinContent(i,j));
    }
    if (hold->Integral()==0.) continue;
    Double_t prob,quant16,quant84,median;
    prob=.5;
    hold->GetQuantiles(1,&median,&prob);
    prob=.84;
    hold->GetQuantiles(1,&quant84,&prob);
    prob=.16;
    hold->GetQuantiles(1,&quant16,&prob);
    h1_resolution->SetBinContent(i,0.5*fabs(quant84-quant16)/median);
    if (h1_median) {
      h1_median->SetBinContent(i,median);
      h1_median->SetBinError(i,h1_resolution->GetBinContent(i));
    }
  }
}

void TCCPlots::make_median_add(TH2* h2_response, TH1* h1_resolution){
  for (int i=1; i<=h2_response->GetNbinsX(); i++){
    TH1F* hold = new TH1F("","", h2_response->GetNbinsY(),
			  h2_response->GetYaxis()->GetBinCenter(1)-0.5*h2_response->GetYaxis()->GetBinWidth(1),
			  h2_response->GetYaxis()->GetBinCenter(h2_response->GetNbinsY())+0.5*h2_response->GetYaxis()->GetBinWidth(h2_response->GetNbinsY()));
    for (int j=1; j<=h2_response->GetNbinsY(); j++){
      hold->SetBinContent(j,h2_response->GetBinContent(i,j));
    }
    if (hold->Integral()==0.) continue;
    Double_t prob,quant16,quant84,median;
    prob=.5;
    hold->GetQuantiles(1,&median,&prob);
//     prob=.84;
    prob=0.75;
    hold->GetQuantiles(1,&quant84,&prob);
//     prob=.16;
    prob=0.25;
    hold->GetQuantiles(1,&quant16,&prob);
    h1_resolution->SetBinContent(i,0.5*fabs(quant84-quant16));
  }
}

void TCCPlots::make_median(TH3* h3_response, TH2* h2_resolution){
  for (int i=1; i<=h3_response->GetNbinsX(); i++) {
    for (int j=1; j<=h3_response->GetNbinsY(); j++) {
      TH1F* hold = new TH1F("","", 
			    h3_response->GetNbinsZ(), 
			    h3_response->GetZaxis()->GetBinCenter(1)-0.5*h3_response->GetZaxis()->GetBinWidth(1), 
			    h3_response->GetZaxis()->GetBinCenter(h3_response->GetNbinsZ())+0.5*h3_response->GetZaxis()->GetBinWidth(h3_response->GetNbinsZ()));
      for (int k=1; k<=h3_response->GetNbinsZ(); k++){
	hold->SetBinContent(k,h3_response->GetBinContent(i,j,k));
      }
      if (hold->Integral()==0.) continue;
      Double_t prob,quant16,quant84,median;
      prob=.5;
      hold->GetQuantiles(1,&median,&prob);
      prob=.84;
      hold->GetQuantiles(1,&quant84,&prob);
      prob=.16;
      hold->GetQuantiles(1,&quant16,&prob);
      h2_resolution->SetBinContent(i,j,0.5*fabs(quant84-quant16)/median);
    }
  }
}

void TCCPlots::resizeHistograms() {
  
  if (m_collectionType == "tracks") {
    
    if (m_trackPtBins.size()>0) {
     
      const float* ptBins = m_trackPtBins.data();
      int           nBins = m_trackPtBins.size()-1;
      
      m_trk_d0_pt                                      ->GetXaxis()->Set(nBins, ptBins);
      m_trk_z0_pt                                      ->GetXaxis()->Set(nBins, ptBins);
      m_trk_theta_pt                                   ->GetXaxis()->Set(nBins, ptBins);
      m_trk_phi_pt                                     ->GetXaxis()->Set(nBins, ptBins);
      m_trk_qOverP_pt                                  ->GetXaxis()->Set(nBins, ptBins);    
      m_trk_caloEntryEtaOverEta_pt                     ->GetXaxis()->Set(nBins, ptBins);    
      m_trk_caloEntryUncEta_pt                         ->GetXaxis()->Set(nBins, ptBins);    
      m_trk_caloEntryUncTheta_pt                       ->GetXaxis()->Set(nBins, ptBins);    
      m_trk_caloEntryPhiOverPhi_pt                     ->GetXaxis()->Set(nBins, ptBins);    
      m_trk_caloEntryUncPhi_pt                         ->GetXaxis()->Set(nBins, ptBins);    
      m_trk_caloEntryUncTot_pt                         ->GetXaxis()->Set(nBins, ptBins); 
      m_trk_pv0_caloEntryUncTot_pt                     ->GetXaxis()->Set(nBins, ptBins); 
      m_trk_pv0_caloEntryUncTot_truthpt                ->GetXaxis()->Set(nBins, ptBins);
      m_trk_pv0_caloEntryUncTot_pt_0SiHits             ->GetXaxis()->Set(nBins, ptBins); 
      m_trk_perigeeUncEta_pt                           ->GetXaxis()->Set(nBins, ptBins);    
      m_trk_perigeeUncTheta_pt                         ->GetXaxis()->Set(nBins, ptBins);    
      m_trk_perigeeUncPhi_pt                           ->GetXaxis()->Set(nBins, ptBins);    
      m_trk_perigeeUncTot_pt                           ->GetXaxis()->Set(nBins, ptBins);    
      m_trk_delta_perigeeEta_caloEntryEta_pt           ->GetXaxis()->Set(nBins, ptBins);
      m_trk_delta_perigeePhi_caloEntryPhi_pt           ->GetXaxis()->Set(nBins, ptBins);
      m_trk_delta_trackEta_caloEntryEta_pt             ->GetXaxis()->Set(nBins, ptBins);
      m_trk_delta_trackPhi_caloEntryPhi_pt             ->GetXaxis()->Set(nBins, ptBins);
      m_trk_delta_trackEta_caloEntryEtaCorr_pt         ->GetXaxis()->Set(nBins, ptBins);
      m_trk_delta_trackPhi_caloEntryPhiCorr_pt         ->GetXaxis()->Set(nBins, ptBins);
      m_trk_delta_caloEntryEta_caloEntryEtaCorr_pt     ->GetXaxis()->Set(nBins, ptBins);
      m_trk_delta_caloEntryPhi_caloEntryPhiCorr_pt     ->GetXaxis()->Set(nBins, ptBins);
      
      m_trk_total_pt                                   ->GetXaxis()->Set(nBins, ptBins);
      m_trk_total_clusters_pt                          ->GetXaxis()->Set(nBins, ptBins);
      m_trk_notMatching_deltar_fix_pt                  ->GetXaxis()->Set(nBins, ptBins);
      m_trk_notMatching_deltar_fix_caloEntryUncTot_pt  ->GetXaxis()->Set(nBins, ptBins);
      m_trk_notMatching_deltar_var_pt                  ->GetXaxis()->Set(nBins, ptBins);
      m_trk_notMatching_deltar_var_caloEntryUncTot_pt  ->GetXaxis()->Set(nBins, ptBins);
      m_trk_notMatching_deltar_none_pt                 ->GetXaxis()->Set(nBins, ptBins);
      m_trk_notMatching_deltar_none_caloEntryUncTot_pt ->GetXaxis()->Set(nBins, ptBins);
      
      m_trk_matching_deltar_or_clusters_pt             ->GetXaxis()->Set(nBins, ptBins);
      m_trk_matching_deltar_and_clusters_pt            ->GetXaxis()->Set(nBins, ptBins);
      m_trk_matching_deltar_fix_only_clusters_pt       ->GetXaxis()->Set(nBins, ptBins);
      m_trk_matching_deltar_var_only_clusters_pt       ->GetXaxis()->Set(nBins, ptBins);
      
      m_trk_matching_deltar_fix_and_var_pt             ->GetXaxis()->Set(nBins, ptBins);
      m_trk_matching_deltar_fix_or_var_pt              ->GetXaxis()->Set(nBins, ptBins);
      m_trk_matching_deltar_fix_pt                     ->GetXaxis()->Set(nBins, ptBins);
      m_trk_matching_deltar_var_pt                     ->GetXaxis()->Set(nBins, ptBins);
      m_trk_matching_deltar_onlyfix_pt                 ->GetXaxis()->Set(nBins, ptBins);
      m_trk_matching_deltar_onlyvar_pt                 ->GetXaxis()->Set(nBins, ptBins);
              
      m_trk_matchedFraction_pt_fix_and_var             ->GetXaxis()->Set(nBins, ptBins);         
      m_trk_matchedFraction_pt_fix_or_var              ->GetXaxis()->Set(nBins, ptBins);         
      m_trk_matchedFraction_pt_fix                     ->GetXaxis()->Set(nBins, ptBins);         
      m_trk_matchedFraction_pt_notfix                  ->GetXaxis()->Set(nBins, ptBins);         
      m_trk_matchedFraction_pt_var                     ->GetXaxis()->Set(nBins, ptBins);         
      m_trk_matchedFraction_pt_notvar                  ->GetXaxis()->Set(nBins, ptBins);         
      m_trk_matchedFraction_pt_onlyvar                 ->GetXaxis()->Set(nBins, ptBins);         
      m_trk_matchedFraction_pt_onlyfix                 ->GetXaxis()->Set(nBins, ptBins);         
      m_trk_matchedFraction_pt_none                    ->GetXaxis()->Set(nBins, ptBins);

      m_trk_pv0_total_pt                               ->GetXaxis()->Set(nBins, ptBins);
      m_trk_pv0_total_clusters_pt                      ->GetXaxis()->Set(nBins, ptBins);
      m_trk_pv0_notMatching_deltar_fix_pt              ->GetXaxis()->Set(nBins, ptBins);
      m_trk_pv0_notMatching_deltar_var_pt              ->GetXaxis()->Set(nBins, ptBins);
      m_trk_pv0_notMatching_deltar_none_pt             ->GetXaxis()->Set(nBins, ptBins);
      
      m_trk_pv0_matching_deltar_fix_and_var_pt         ->GetXaxis()->Set(nBins, ptBins);
      m_trk_pv0_matching_deltar_fix_or_var_pt          ->GetXaxis()->Set(nBins, ptBins);
      m_trk_pv0_matching_deltar_fix_pt                 ->GetXaxis()->Set(nBins, ptBins);
      m_trk_pv0_matching_deltar_var_pt                 ->GetXaxis()->Set(nBins, ptBins);
      m_trk_pv0_matching_deltar_onlyfix_pt             ->GetXaxis()->Set(nBins, ptBins);
      m_trk_pv0_matching_deltar_onlyvar_pt             ->GetXaxis()->Set(nBins, ptBins);
              
      m_trk_pv0_matchedFraction_pt_fix_and_var         ->GetXaxis()->Set(nBins, ptBins);         
      m_trk_pv0_matchedFraction_pt_fix_or_var          ->GetXaxis()->Set(nBins, ptBins);         
      m_trk_pv0_matchedFraction_pt_fix                 ->GetXaxis()->Set(nBins, ptBins);         
      m_trk_pv0_matchedFraction_pt_notfix              ->GetXaxis()->Set(nBins, ptBins);         
      m_trk_pv0_matchedFraction_pt_var                 ->GetXaxis()->Set(nBins, ptBins);         
      m_trk_pv0_matchedFraction_pt_notvar              ->GetXaxis()->Set(nBins, ptBins);         
      m_trk_pv0_matchedFraction_pt_onlyvar             ->GetXaxis()->Set(nBins, ptBins);         
      m_trk_pv0_matchedFraction_pt_onlyfix             ->GetXaxis()->Set(nBins, ptBins);         
      m_trk_pv0_matchedFraction_pt_none                ->GetXaxis()->Set(nBins, ptBins);
     
    }
    
    if (m_trackProdRadiusBins.size()>0) {
      const float* radiusBins = m_trackProdRadiusBins.data();
      int               nBins = m_trackProdRadiusBins.size()-1;
      
      m_trk_prodRadius                                         ->GetXaxis()->Set(nBins, radiusBins);
      m_trk_perigeeUncEta_prodRadius                           ->GetXaxis()->Set(nBins, radiusBins);
      m_trk_perigeeUncTheta_prodRadius                         ->GetXaxis()->Set(nBins, radiusBins);
      m_trk_perigeeUncPhi_prodRadius                           ->GetXaxis()->Set(nBins, radiusBins);
      m_trk_perigeeUncTot_prodRadius                           ->GetXaxis()->Set(nBins, radiusBins);
      m_trk_caloEntryUncEta_prodRadius                         ->GetXaxis()->Set(nBins, radiusBins);
      m_trk_caloEntryUncTheta_prodRadius                       ->GetXaxis()->Set(nBins, radiusBins);
      m_trk_caloEntryUncPhi_prodRadius                         ->GetXaxis()->Set(nBins, radiusBins);
      m_trk_caloEntryUncTot_prodRadius                         ->GetXaxis()->Set(nBins, radiusBins);
      
      m_trk_notMatching_deltar_fix_caloEntryUncTot_prodRadius  ->GetXaxis()->Set(nBins, radiusBins);
      m_trk_notMatching_deltar_var_caloEntryUncTot_prodRadius  ->GetXaxis()->Set(nBins, radiusBins);
      m_trk_notMatching_deltar_none_caloEntryUncTot_prodRadius ->GetXaxis()->Set(nBins, radiusBins);
      
    }
  } else if (m_collectionType == "jets")  {
        
    if (m_jetPtBins.size()>0) {
      
      float* ptBins = m_jetPtBins.data();
      int     nBins = m_jetPtBins.size()-1;
       
      m_jet_response_m_pt             ->GetXaxis()->Set(nBins, ptBins);
      m_jet_response_m_pt_2leadings   ->GetXaxis()->Set(nBins, ptBins);
      m_jet_response_m_pt_leading     ->GetXaxis()->Set(nBins, ptBins);
      m_jet_response_m_pt_subleading  ->GetXaxis()->Set(nBins, ptBins);
      m_jet_response_d2_pt            ->GetXaxis()->Set(nBins, ptBins);
      m_jet_response_d2_pt_2leadings  ->GetXaxis()->Set(nBins, ptBins);
      m_jet_response_d2_pt_leading    ->GetXaxis()->Set(nBins, ptBins);
      m_jet_response_d2_pt_subleading ->GetXaxis()->Set(nBins, ptBins);
      m_jet_response_add_d2_pt             ->GetXaxis()->Set(nBins, ptBins);
      m_jet_response_add_d2_pt_2leadings   ->GetXaxis()->Set(nBins, ptBins);
      m_jet_response_add_d2_pt_leading     ->GetXaxis()->Set(nBins, ptBins);
      m_jet_response_add_d2_pt_subleading  ->GetXaxis()->Set(nBins, ptBins);
      m_jet_response_add_eta_pt            ->GetXaxis()->Set(nBins, ptBins);
      m_jet_response_add_eta_pt_2leadings  ->GetXaxis()->Set(nBins, ptBins);
      m_jet_response_add_eta_pt_leading    ->GetXaxis()->Set(nBins, ptBins);
      m_jet_response_add_eta_pt_subleading ->GetXaxis()->Set(nBins, ptBins);      
      m_jet_resolution_m                   ->GetXaxis()->Set(nBins, ptBins);          
      m_jet_resolution_m_2leadings         ->GetXaxis()->Set(nBins, ptBins);
      m_jet_resolution_m_leading           ->GetXaxis()->Set(nBins, ptBins);
      m_jet_resolution_m_subleading        ->GetXaxis()->Set(nBins, ptBins);
      m_jet_resolution_d2                  ->GetXaxis()->Set(nBins, ptBins);          
      m_jet_resolution_d2_2leadings        ->GetXaxis()->Set(nBins, ptBins);
      m_jet_resolution_d2_leading          ->GetXaxis()->Set(nBins, ptBins);
      m_jet_resolution_d2_subleading       ->GetXaxis()->Set(nBins, ptBins);
      m_jet_resolution_add_d2              ->GetXaxis()->Set(nBins, ptBins);          
      m_jet_resolution_add_d2_2leadings    ->GetXaxis()->Set(nBins, ptBins);
      m_jet_resolution_add_d2_leading      ->GetXaxis()->Set(nBins, ptBins);
      m_jet_resolution_add_d2_subleading   ->GetXaxis()->Set(nBins, ptBins);      
    }
    
    if (m_jetPtBins.size()>0 and m_jetMassOverPtBins.size()>0) {
      
      float* ptBins = m_jetPtBins.data();
      int   nBinsPt = m_jetPtBins.size()-1;
      
      float* massOverPtBins = m_jetMassOverPtBins.data();
      int   nBinsMassOverPt = m_jetMassOverPtBins.size()-1;
      
      
      m_jet_mopt_pt_response_m                ->GetXaxis()->Set(nBinsPt, ptBins);
      m_jet_mopt_pt_response_m                ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins);
      m_jet_mopt_pt_response_m_2leadings      ->GetXaxis()->Set(nBinsPt, ptBins);
      m_jet_mopt_pt_response_m_2leadings      ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins);
      m_jet_mopt_pt_response_m_leading        ->GetXaxis()->Set(nBinsPt, ptBins);
      m_jet_mopt_pt_response_m_leading        ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins);
      m_jet_mopt_pt_response_m_subleading     ->GetXaxis()->Set(nBinsPt, ptBins);
      m_jet_mopt_pt_response_m_subleading     ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins);
                                              
      m_jet_mopt_pt_response_pt               ->GetXaxis()->Set(nBinsPt, ptBins); 
      m_jet_mopt_pt_response_pt               ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins); 
      m_jet_mopt_pt_response_pt_2leadings     ->GetXaxis()->Set(nBinsPt, ptBins); 
      m_jet_mopt_pt_response_pt_2leadings     ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins); 
      m_jet_mopt_pt_response_pt_leading       ->GetXaxis()->Set(nBinsPt, ptBins); 
      m_jet_mopt_pt_response_pt_leading       ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins); 
      m_jet_mopt_pt_response_pt_subleading    ->GetXaxis()->Set(nBinsPt, ptBins); 
      m_jet_mopt_pt_response_pt_subleading    ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins); 
                                              
      m_jet_mopt_pt_response_eta              ->GetXaxis()->Set(nBinsPt, ptBins);  
      m_jet_mopt_pt_response_eta              ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins);  
      m_jet_mopt_pt_response_eta_2leadings    ->GetXaxis()->Set(nBinsPt, ptBins);  
      m_jet_mopt_pt_response_eta_2leadings    ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins);  
      m_jet_mopt_pt_response_eta_leading      ->GetXaxis()->Set(nBinsPt, ptBins);  
      m_jet_mopt_pt_response_eta_leading      ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins);  
      m_jet_mopt_pt_response_eta_subleading   ->GetXaxis()->Set(nBinsPt, ptBins);  
      m_jet_mopt_pt_response_eta_subleading   ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins);  
                                              
      m_jet_mopt_pt_response_phi              ->GetXaxis()->Set(nBinsPt, ptBins);  
      m_jet_mopt_pt_response_phi              ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins);  
      m_jet_mopt_pt_response_phi_2leadings    ->GetXaxis()->Set(nBinsPt, ptBins);  
      m_jet_mopt_pt_response_phi_2leadings    ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins);  
      m_jet_mopt_pt_response_phi_leading      ->GetXaxis()->Set(nBinsPt, ptBins);  
      m_jet_mopt_pt_response_phi_leading      ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins);  
      m_jet_mopt_pt_response_phi_subleading   ->GetXaxis()->Set(nBinsPt, ptBins);  
      m_jet_mopt_pt_response_phi_subleading   ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins);  
                                              
      m_jet_mopt_pt_response_d2               ->GetXaxis()->Set(nBinsPt, ptBins); 
      m_jet_mopt_pt_response_d2               ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins); 
      m_jet_mopt_pt_response_d2_2leadings     ->GetXaxis()->Set(nBinsPt, ptBins); 
      m_jet_mopt_pt_response_d2_2leadings     ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins); 
      m_jet_mopt_pt_response_d2_leading       ->GetXaxis()->Set(nBinsPt, ptBins); 
      m_jet_mopt_pt_response_d2_leading       ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins); 
      m_jet_mopt_pt_response_d2_subleading    ->GetXaxis()->Set(nBinsPt, ptBins); 
      m_jet_mopt_pt_response_d2_subleading    ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins); 
                                              
      m_jet_resolution_mopt_pt_m              ->GetXaxis()->Set(nBinsPt, ptBins);
      m_jet_resolution_mopt_pt_m              ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins);
      m_jet_resolution_mopt_pt_m_2leadings    ->GetXaxis()->Set(nBinsPt, ptBins);
      m_jet_resolution_mopt_pt_m_2leadings    ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins);
      m_jet_resolution_mopt_pt_m_leading      ->GetXaxis()->Set(nBinsPt, ptBins);
      m_jet_resolution_mopt_pt_m_leading      ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins);
      m_jet_resolution_mopt_pt_m_subleading   ->GetXaxis()->Set(nBinsPt, ptBins);
      m_jet_resolution_mopt_pt_m_subleading   ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins);
                                              
      m_jet_resolution_mopt_pt_pt             ->GetXaxis()->Set(nBinsPt, ptBins); 
      m_jet_resolution_mopt_pt_pt             ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins); 
      m_jet_resolution_mopt_pt_pt_2leadings   ->GetXaxis()->Set(nBinsPt, ptBins); 
      m_jet_resolution_mopt_pt_pt_2leadings   ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins); 
      m_jet_resolution_mopt_pt_pt_leading     ->GetXaxis()->Set(nBinsPt, ptBins); 
      m_jet_resolution_mopt_pt_pt_leading     ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins); 
      m_jet_resolution_mopt_pt_pt_subleading  ->GetXaxis()->Set(nBinsPt, ptBins);
      m_jet_resolution_mopt_pt_pt_subleading  ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins);
                                              
      m_jet_resolution_mopt_pt_eta            ->GetXaxis()->Set(nBinsPt, ptBins);  
      m_jet_resolution_mopt_pt_eta            ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins);  
      m_jet_resolution_mopt_pt_eta_2leadings  ->GetXaxis()->Set(nBinsPt, ptBins);  
      m_jet_resolution_mopt_pt_eta_2leadings  ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins);  
      m_jet_resolution_mopt_pt_eta_leading    ->GetXaxis()->Set(nBinsPt, ptBins);  
      m_jet_resolution_mopt_pt_eta_leading    ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins);  
      m_jet_resolution_mopt_pt_eta_subleading ->GetXaxis()->Set(nBinsPt, ptBins); 
      m_jet_resolution_mopt_pt_eta_subleading ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins); 
                                              
      m_jet_resolution_mopt_pt_phi            ->GetXaxis()->Set(nBinsPt, ptBins);  
      m_jet_resolution_mopt_pt_phi            ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins);  
      m_jet_resolution_mopt_pt_phi_2leadings  ->GetXaxis()->Set(nBinsPt, ptBins);  
      m_jet_resolution_mopt_pt_phi_2leadings  ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins);  
      m_jet_resolution_mopt_pt_phi_leading    ->GetXaxis()->Set(nBinsPt, ptBins);  
      m_jet_resolution_mopt_pt_phi_leading    ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins);  
      m_jet_resolution_mopt_pt_phi_subleading ->GetXaxis()->Set(nBinsPt, ptBins); 
      m_jet_resolution_mopt_pt_phi_subleading ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins); 
                                              
      m_jet_resolution_mopt_pt_d2             ->GetXaxis()->Set(nBinsPt, ptBins); 
      m_jet_resolution_mopt_pt_d2             ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins); 
      m_jet_resolution_mopt_pt_d2_2leadings   ->GetXaxis()->Set(nBinsPt, ptBins); 
      m_jet_resolution_mopt_pt_d2_2leadings   ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins); 
      m_jet_resolution_mopt_pt_d2_leading     ->GetXaxis()->Set(nBinsPt, ptBins); 
      m_jet_resolution_mopt_pt_d2_leading     ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins); 
      m_jet_resolution_mopt_pt_d2_subleading  ->GetXaxis()->Set(nBinsPt, ptBins);
      m_jet_resolution_mopt_pt_d2_subleading  ->GetYaxis()->Set(nBinsMassOverPt, massOverPtBins);
    }
  } else if (m_collectionType == "tccs") {
    if (m_trackPtBins.size()>0) {  
      float* ptBins = m_trackPtBins.data();
      int     nBins = m_trackPtBins.size()-1;
      
      m_tcc_pt              ->GetXaxis()->Set(nBins, ptBins);
      m_tcc_pt_etacut       ->GetXaxis()->Set(nBins, ptBins);
      m_tcc_taste_pt        ->GetXaxis()->Set(nBins, ptBins);
      m_tcc_taste_pt_etacut ->GetXaxis()->Set(nBins, ptBins);
      m_tcc_pt_track_pt     ->GetXaxis()->Set(nBins, ptBins);
      m_tcc_pt_track_pt     ->GetYaxis()->Set(nBins, ptBins);
      m_tcc_pt_truth_pt     ->GetXaxis()->Set(nBins, ptBins);
      m_tcc_pt_truth_pt     ->GetYaxis()->Set(nBins, ptBins);
             
      m_trk_tcc_reco_pt_truth_pt  ->GetXaxis()->Set(nBins, ptBins); 
      m_trk_tcc_reco_pt_truth_pt  ->GetYaxis()->Set(nBins, ptBins); 
              
    }
  } else return;
}

   
   
   