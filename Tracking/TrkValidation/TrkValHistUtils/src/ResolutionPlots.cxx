/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkValHistUtils/ResolutionPlots.h"

namespace Trk{

void ResolutionPlots::initializePlots(){

  Res_pT     = Book1D("Res_pT","Res_pT;(pT-pTtruth)/pTtruth;Entries",100,-0.5,0.5);
  Res_eta    = Book1D("Res_eta","Res_eta;eta-etatruth;Entries",100,-0.02,0.02);
  Res_phi    = Book1D("Res_phi","Res_phi;phi-phitruth;Entries",100,-0.005,0.005);


}

void ResolutionPlots::fill(const xAOD::TrackParticle& trkprt, const xAOD::TruthParticle& truthprt) {

  Res_pT->Fill((trkprt.pt() - truthprt.pt())/truthprt.pt());
  Res_eta->Fill(trkprt.eta() - truthprt.eta());
  Res_phi->Fill(trkprt.phi() - truthprt.phi());

}

}
