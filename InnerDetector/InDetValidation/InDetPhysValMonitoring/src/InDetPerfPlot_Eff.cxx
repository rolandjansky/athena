/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetPerfPlot_Eff.h"
#include "TrkValHistUtils/EfficiencyPurityCalculator.h"
#include "xAODTruth/TruthParticle.h"

using namespace TMath;

InDetPerfPlot_Eff::InDetPerfPlot_Eff(InDetPlotBase* pParent, const std::string & sDir):
InDetPlotBase(pParent, sDir),
m_trackeff_vs_eta{},
m_trackeff_vs_pt{},
m_trackeff_vs_phi{},
m_trackeff_vs_d0{},
m_trackeff_vs_z0{},
m_trackeff_vs_R{},
m_trackeff_vs_Z{},
m_trackinjeteff_vs_dr_gr_j100{}
{
  //nop
}


void 
InDetPerfPlot_Eff::initializePlots() {
  book(m_trackeff_vs_eta, "trackeff_vs_eta");
  book(m_trackeff_vs_pt, "trackeff_vs_pt");
  book(m_trackeff_vs_phi, "trackeff_vs_phi");
  book(m_trackeff_vs_d0, "trackeff_vs_d0");
  book(m_trackeff_vs_z0, "trackeff_vs_z0");
  book(m_trackeff_vs_R, "trackeff_vs_R");
  book(m_trackeff_vs_Z, "trackeff_vs_Z");

  book(m_trackinjeteff_vs_dr_gr_j100, "trackinjeteff_vs_dr_gr_j100");
}

void InDetPerfPlot_Eff::pro_fill(const xAOD::TruthParticle & truth, float weight){
  double eta = truth.eta();
  double pt = truth.pt()/1000.; //convert MeV to GeV
  double phi = truth.phi();
  double d0 = truth.auxdata<float>("d0");
  double z0 = truth.auxdata<float>("z0");
  double R = truth.auxdata<float>("prodR");
  double Z = truth.auxdata<float>("prodZ");

  m_trackeff_vs_eta->Fill(eta, weight);
  m_trackeff_vs_pt->Fill(pt, weight);
  m_trackeff_vs_phi->Fill(phi, weight);
  m_trackeff_vs_d0->Fill(d0, weight);
  m_trackeff_vs_z0->Fill(z0, weight);
  m_trackeff_vs_R->Fill(R, weight);
  m_trackeff_vs_Z->Fill(Z, weight);
}

void InDetPerfPlot_Eff::jet_fill(const xAOD::TrackParticle& track, const xAOD::Jet& jet, float weight){
  //double trketa = track.eta(); //unused
  //double trkphi = track.phi(); //unused

  double dR = jet.p4().DeltaR( track.p4() );

  m_trackinjeteff_vs_dr_gr_j100->Fill(dR, weight);

}

void InDetPerfPlot_Eff::finalizePlots() {
}

