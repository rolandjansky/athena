/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDETDUMMYPLOTS
#define INDETPHYSVALMONITORING_INDETDUMMYPLOTS
/**
 * @file InDetDummyPlots.cxx
 *
 **/



// local includes
#include "InDetPlotBase.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTracking/TrackParticle.h"
// std includes
#include <string>

// fwd includes
class TH1;
class TProfile;
class TH2;

///class holding dummy plots
class InDetDummyPlots: public InDetPlotBase {
public:
  InDetDummyPlots(InDetPlotBase* pParent, const std::string& dirName);

  void lepton_fill(const xAOD::TruthParticle& truth, float weight);
  void prim_photon_fill(const xAOD::TruthParticle& truth);
  void brem_photon_fill(const xAOD::TruthParticle& truth);
  void BT_fill(const xAOD::TruthParticle& truth, float weight);
  void track_vs_truth(const xAOD::TrackParticle& track, const xAOD::TruthParticle& truth, float tmp);
  void minDR(float min_dR, float prod_rad, float prob, double BIDPt);
  void algoEfficiency(double radius, int SiSPweight, int TRTSeededweight, int TRTStandaloneweight, int other_weight);
  void track_author(std::bitset<xAOD::NumberOfTrackRecoInfo> authorset);


private:
  TH1* m_lepton_disappearance_radius;

  TProfile* m_low_Pt_lepton_frac;

  TH1* m_nOut_of_lepdeath;

  TH2* m_brem_spectrum;
  TH2* m_energy_remaining;
  TH2* m_energy_remaining_vs_eta;
  TH2* m_energy_remaining_vs_prodR_TRT_barrel;
  TH2* m_energy_remaining_vs_prodR_type_A_endcap;
  TH2* m_energy_remaining_vs_prodR_type_A_and_B;
  TH2* m_energy_remaining_vs_prodR_type_B_endcap;
  TH2* m_primary_photon_eta_vs_conversion_radius;
  TH2* m_primary_photon_pt_vs_decay_radius;
  TH1* m_primary_photon_pt;
  TH2* m_brem_photon_eta_vs_radius_of_conversion;
  TH2* m_brem_photon_pt_vs_radius_of_origin_of_conversion;

  TH2* m_truthMatchProbability_vs_delta_R;
  TH1* m_minimum_delta_R;
  TH1* m_minimum_delta_R_2;
  TH1* m_minimum_delta_R_3;
  TH1* m_minimum_delta_R_not_found;
  TH1* m_minimum_delta_R_2_not_found;
  TH1* m_minimum_delta_R_3_not_found;
  TH1* m_delta_inverse_pt;
  
  TProfile* m_SiSPSeededFinder_eff_vs_radius;
  TProfile* m_TRTSeeded_eff_vs_radius;
  TProfile* m_TRTStandalone_eff_vs_radius;
  TProfile* m_combo_eff_vs_radius;
  TProfile* m_exclotherAlgo_eff_vs_radius;
  TProfile* m_otherAlgo_eff_vs_radius;

  TH1* m_track_author_use;


  // plot base has nop default implementation of this; we use it to book the histos
  void initializePlots();
  void finalizePlots();
};

#endif
