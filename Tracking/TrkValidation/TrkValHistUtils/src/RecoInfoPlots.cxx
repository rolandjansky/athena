/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkValHistUtils/RecoInfoPlots.h"

namespace Trk{

void RecoInfoPlots::initializePlots(){
  trackfitchi2 = Book1D("tchi2","Track fit chi2;Track Fit Chi2;Entries",100,0.,100.);
  trackfitndof = Book1D("tndof","Track fit ndof;Number of degrees of freedom;Entries",51,-0.5,50.5);
  trackcon     = Book1D("chi2ndof", "Track fit chi2/ndof;Track fit chi2/ndof;Entries", 100, 0, 5);

}

void RecoInfoPlots::fill(const xAOD::TrackParticle& trkprt) {
  trackfitchi2->Fill(trkprt.chiSquared());
  trackfitndof->Fill(trkprt.numberDoF());
  trackcon->Fill(trkprt.chiSquared()/trkprt.numberDoF());
}

}
