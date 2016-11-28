/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkValHistUtils/ImpactPlots.h"

namespace Trk {
  void
  ImpactPlots::init() {
    z0 = NULL;
    z0sig = NULL;
    d0 = NULL;
    d0sig = NULL;
    d0_small = NULL;
  }

  void
  ImpactPlots::initializePlots() {
    z0 = Book1D("z0", "z0;Signed Impact Parameter z0 (mm);Entries / 5 mm", 80, -200, 200);
//  z0sig  = Book1D("z0sig","z0sig,;Signed Impact Parameter Significance; Entries / 0.1", 100, 0, 10);
    d0 = Book1D("d0", "d0;Signed Impact Parameter d0 (mm);Entries / 0.025 mm", 80, -1, 1);
//  d0sig  = Book1D("d0sig","d0sig,;Signed Impact Parameter Significance; Entries / 0.1", 100, 0, 10);
    d0_small = Book1D("d0_small", "d0;Signed Impact Parameter d0 (mm);Entries / 0.0025 mm", 80, -0.1, 0.1);
  }

  void
  ImpactPlots::fill(const xAOD::TrackParticle &trkprt) {
    d0->Fill(trkprt.d0());
    d0_small->Fill(trkprt.d0());
    z0->Fill(trkprt.z0());
  }
}
