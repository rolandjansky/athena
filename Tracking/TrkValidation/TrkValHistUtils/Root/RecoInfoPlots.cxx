/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkValHistUtils/RecoInfoPlots.h"
#include "TMath.h"

namespace Trk {
  void
  RecoInfoPlots::init() {
    trackfitchi2 = nullptr;
    trackfitndof = nullptr;
    trackcon = nullptr;
    trackchi2prob = nullptr;
  }

  void
  RecoInfoPlots::initializePlots() {
    trackfitchi2 = Book1D("tchi2" + m_sType, m_sType + " Track fit chi2;Track Fit Chi2;Entries", 100, 0., 100.);
    trackfitndof = Book1D("tndof" + m_sType, m_sType + " Track fit ndof;Number of degrees of freedom;Entries", 101,
                          -0.5, 100.5);
    trackcon = Book1D("chi2ndof" + m_sType, m_sType + " Track fit chi2/ndof;Track fit chi2/ndof;Entries", 100, 0, 5);
    trackchi2prob = Book1D("chi2prob" + m_sType, m_sType + " Track fit chi2 probability;Track fit p(#chi^2);Entries",
                           20, 0., 1.);
  }

  void
  RecoInfoPlots::fill(const xAOD::TrackParticle &trkprt, float weight) {
    float chi2 = trkprt.chiSquared();
    int ndf = trkprt.numberDoF();

    trackfitchi2->Fill(chi2,weight);
    trackfitndof->Fill(ndf,weight);
    trackcon->Fill(chi2 / ndf, weight);
    trackchi2prob->Fill(TMath::Prob(chi2, ndf), weight);
  }
}
