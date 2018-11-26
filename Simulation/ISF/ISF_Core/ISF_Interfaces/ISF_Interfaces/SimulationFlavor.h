/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SimulationFlavor.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef ISFINTERFACES_SIMULATIONFLAVOR_H
#define ISFINTERFACES_SIMULATIONFLAVOR_H 1

//#include <stdint.h>

namespace ISF {

  /** Identifier type for simulation flavor */
  typedef int SimulationFlavor;

  /** enum to make the code more readable */
  enum SimulationDefinitions {
    UndefinedSim = 0,
    ParticleKiller = 1,
    Fatras = 2,
    Geant4 = 3,
    FastCaloSim = 4,
    FastCaloSimV2 = 5,
    Parametric = 6,
    FatrasPileup = 7,
    FastCaloSimPileup = 8,
    NFlavors = 9
  };

}

#endif
