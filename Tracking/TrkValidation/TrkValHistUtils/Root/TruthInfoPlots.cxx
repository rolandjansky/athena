/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkValHistUtils/TruthInfoPlots.h"

namespace Trk {
  void
  TruthInfoPlots::init() {
    truthType = nullptr;
    origin = nullptr;
  }

  void
  TruthInfoPlots::initializePlots() {
    truthType = Book1D("truthType", "truthType;truthType;Entries", 50, -0.5, 49.5);
    origin = Book1D("truthOrigin", "truthOrigin;truthOrigin;Entries", 40, -0.5, 39.5);
  }

  void
  TruthInfoPlots::fill(const xAOD::TruthParticle &truthprt, float weight ) {
    if (truthprt.isAvailable<int>("truthType")) {
      truthType->Fill(truthprt.auxdata< int >("truthType"), weight);
    }
    if (truthprt.isAvailable<int>("truthOrigin")) {
      origin->Fill(truthprt.auxdata< int >("truthOrigin"), weight);
    }
  }
}
