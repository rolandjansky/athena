/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaValidation/ParticlePlots.h"
#include <typeinfo>

namespace egammaMonitoring {

  ParticlePlots::ParticlePlots(){}
  
  void ParticlePlots::initializePlots() {
    
    pT_prtcl   = new TH1D("pT_prtcl"  , ";p_{T} [GeV]; Track p_{T} Events",  200,            0,         200);
    eta_prtcl  = new TH1D("eta_prtcl" , ";#eta; Track #eta Events"        ,  200,           -3,           3);
    phi_prtcl  = new TH1D("phi_prtcl" , ";#phi; Track #phi Events"        ,  100, -TMath::Pi(), TMath::Pi());
    
  } // initializePlots
  
  void ParticlePlots::fill(const xAOD::IParticle& egamma) {
    
    if((egamma.pt())/1000. > 0) pT_prtcl->Fill((egamma.pt())/1000.);
    eta_prtcl->Fill(egamma.eta());
    phi_prtcl->Fill(egamma.phi());

  } // fill
  
} // namespace
