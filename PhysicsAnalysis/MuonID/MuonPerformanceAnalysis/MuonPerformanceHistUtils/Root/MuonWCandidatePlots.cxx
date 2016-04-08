/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPerformanceHistUtils/MuonWCandidatePlots.h"
#include "xAODTracking/TrackingPrimitives.h"
#include <TLorentzVector.h>

MuonWCandidatePlots::MuonWCandidatePlots(PlotBase *pParent, std::string sDir):PlotBase(pParent, sDir)
									     , me_t(NULL)
									     , me_x(NULL)
									     , me_y(NULL)									      
									     , mu_pt(NULL)
									     , mu_eta(NULL)
									     , mu_phi(NULL)
									     , w_pt(NULL)
									     , w_phi(NULL)
									     , w_mt(NULL)
{}

void MuonWCandidatePlots::initializePlots()
{
  me_t = Book1D("me_T","ME_T;ME_T [GeV];Entries",200,0.,200.);
  me_x = Book1D("me_X","ME_X;ME_X [GeV];Entries",200,0.,200.);
  me_y = Book1D("me_Y","ME_Y;ME_Y [GeV];Entries",200,0.,200.);

  mu_pt  = Book1D("mu_pt","mu_pt;p_{T,#mu} [GeV];Entries",200,0.,200.);
  mu_eta = Book1D("mu_eta","mu_eta;#eta_{#mu};Entries",64,-3.2,3.2);
  mu_phi = Book1D("mu_phi","mu_phi;#phi_{#mu};Entries",60,-3.0,3.0);

  w_pt   = Book1D("w_pt","w_pt;p_{T,W} [GeV];Entries",200,0.,200.);
  w_phi  = Book1D("w_phi","w_phi;#phi_{W};Entries",64,-3.2,3.2);
  w_mt   = Book1D("w_mt","w_mt;m_{T,W} [GeV];Entries",200,0.,200.);
}


void MuonWCandidatePlots::fill(const xAOD::Muon& mu, const xAOD::MissingET& met)
{
  me_t->Fill(met.met()/1000.);
  me_x->Fill(met.mpx()/1000.);
  me_y->Fill(met.mpy()/1000.);

  mu_pt->Fill(mu.pt()/1000.);
  mu_eta->Fill(mu.eta());
  mu_phi->Fill(mu.phi());

  TLorentzVector metP4;
  metP4.SetPxPyPzE(met.mpx(), met.mpy(), 0.0, met.met());
  TLorentzVector muP4 = mu.p4();
  TLorentzVector mutP4; 
  mutP4.SetPxPyPzE(muP4.Px(), muP4.Py(), 0.0, muP4.Pt());
  TLorentzVector Wcand = mutP4+metP4;
  
  w_pt->Fill(Wcand.Pt()/1000.);
  w_mt->Fill(Wcand.M()/1000.);
  w_phi->Fill(Wcand.Phi());
}
  

