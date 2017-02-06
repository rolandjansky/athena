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
private:
  TH1* m_lepton_disappearance_radius;

  TProfile* m_low_Pt_lepton_frac;

  TH1* m_nOut_of_lepdeath;

  TH2* m_brem_spectrum;
  TH2* m_energy_remaining;
  TH2* m_energy_remaining_vs_eta;
  TH2* m_energy_remaining_vs_prodR_low_eta;
  TH2* m_energy_remaining_vs_prodR_medium_eta;
  TH2* m_energy_remaining_vs_prodR_high_eta;
  TH2* m_primary_photon_eta_vs_conversion_radius;
  TH2* m_primary_photon_pt_vs_decay_radius;
  TH1* m_primary_photon_pt;
  TH2* m_brem_photon_eta_vs_radius_of_conversion;
  TH2* m_brem_photon_pt_vs_radius_of_origin_of_conversion;

  // plot base has nop default implementation of this; we use it to book the histos
  void initializePlots();
  void finalizePlots();
};

#endif
