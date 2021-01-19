/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDETPERFPLOT_EFF
#define INDETPHYSVALMONITORING_INDETPERFPLOT_EFF
/**
 * @file InDetPerfPlot_Eff.cxx
 * @author Gabrel Facini <Gabriel.Facini@cern.ch>
 * Wed Oct 29 09:58:58 CET 2014
 *
 * a lot of this is copied from EfficiencyPlots in the TrkValHistUtils which is dumb
 * the point is that many instances of this will be created so more control of the names
 * is needed.  I don't have permission for that package and time is short...as usual
 **/



// local includes
#include "InDetPlotBase.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"

// std includes
#include <string>

///class holding Pt plots for Inner Detector RTT Validation and implementing fill methods
class InDetPerfPlot_Eff: public InDetPlotBase {
public:
  InDetPerfPlot_Eff(InDetPlotBase* pParent, const std::string& dirName);

  void fill(const xAOD::TruthParticle& truth, const bool isGood);
  void BT_fill(const xAOD::TruthParticle& truth, float weight);
  void mu_fill(const xAOD::TruthParticle& truth, const bool isGood, const unsigned int nMuEvents);

private:
  TEfficiency* m_trackeff_vs_eta;
  TEfficiency* m_trackeff_vs_pt;
  TEfficiency* m_trackeff_vs_phi;
  TEfficiency* m_trackeff_vs_d0;
  TEfficiency* m_trackeff_vs_z0;
  TEfficiency* m_trackeff_vs_R;
  TEfficiency* m_trackeff_vs_Z;

  TEfficiency* m_trackeff_vs_prodR;
  TEfficiency* m_trackeff_vs_prodZ;

  TProfile* m_eff_vs_eta_of_daughters;
  TProfile* m_eff_vs_theta_of_daughters;
  TProfile* m_eff_vs_theta_tan_of_daughters;
  TProfile* m_eff_vs_theta_cotan_of_daughters;
  TProfile* m_eff_vs_phi_of_daughters;
  TProfile* m_eff_vs_phi_sin_of_daughters;
  TProfile* m_eff_vs_phi_cos_of_daughters;
  
  TEfficiency* m_eff_vs_mu;
  TEfficiency* m_eff_vs_mu2;
  TEfficiency* m_eff_vs_mu3;
  TEfficiency* m_eff_vs_muTotal;

  // plot base has nop default implementation of this; we use it to book the histos
  void initializePlots();
  void finalizePlots();
};

#endif
