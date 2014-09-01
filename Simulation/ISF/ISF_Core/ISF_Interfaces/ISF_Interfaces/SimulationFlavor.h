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
    FatrasSim = 1,
    Geant4Sim = 2,
    FastCaloSim = 3
  };

}

#endif 
