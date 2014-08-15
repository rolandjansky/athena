/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkValHistUtils/ImpactPlots.h"

namespace Trk{

void ImpactPlots::initializePlots(){

  z0     = Book1D("z0", "z0;Signed Impact Parameter (mm);Entries / 0.5 mm", 80, -20, 20);
//  z0sig  = Book1D("z0sig","z0sig,;Signed Impact Parameter Significance; Entries / 0.1", 100, 0, 10);
  d0     = Book1D("d0", "d0;Signed Impact Parameter (mm);Entries / 0.5 mm", 80, -20, 20);
//  d0sig  = Book1D("d0sig","d0sig,;Signed Impact Parameter Significance; Entries / 0.1", 100, 0, 10);

}

void ImpactPlots::fill(const xAOD::TrackParticle& trkprt) {
  z0->Fill(trkprt.d0());
  d0->Fill(trkprt.z0());
}

}
