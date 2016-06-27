/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Hide multi-threading classes from builds without G4MT
#ifdef G4MULTITHREADED

#include "G4AtlasUserWorkerThreadInitialization.h"
#include "G4AtlasWorkerRunManager.h"

// Geant4 includes. They might not be stictly necessary.
#include "globals.hh" // gives the G4Exception
#include "G4AutoLock.hh"

// Mersenne twister engine used in ATLAS
#include "CLHEP/Random/MTwistEngine.h"

// Using Geant4's mutex, but could easily use another.
namespace {
  G4Mutex rngCreateMutex = G4MUTEX_INITIALIZER;
}

void G4AtlasUserWorkerThreadInitialization::
SetupRNGEngine(const CLHEP::HepRandomEngine* aNewRNG) const
{
  // Current behavior in G4 base class is to use a lock.
  G4AutoLock l(&rngCreateMutex);

  // A Call to this just forces the creation to defaults.
  // Not sure how this fits into the ATLAS model...
  G4Random::getTheEngine();
  
  // Try casting to determine RNG engine type
  CLHEP::HepRandomEngine* retRNG = 0;
  if ( dynamic_cast<const CLHEP::MTwistEngine*>(aNewRNG) ) {
     retRNG = new CLHEP::MTwistEngine;
  }
  else if ( dynamic_cast<const CLHEP::HepJamesRandom*>(aNewRNG) ) {
     retRNG = new CLHEP::HepJamesRandom;
  }
  else if ( dynamic_cast<const CLHEP::RanecuEngine*>(aNewRNG) ) {
     retRNG = new CLHEP::RanecuEngine;
  }
  else if ( dynamic_cast<const CLHEP::Ranlux64Engine*>(aNewRNG) ) {
     retRNG = new CLHEP::Ranlux64Engine;
  }
  else if ( dynamic_cast<const CLHEP::MTwistEngine*>(aNewRNG) ) {
     retRNG = new CLHEP::MTwistEngine;
  }
  else if ( dynamic_cast<const CLHEP::DualRand*>(aNewRNG) ) {
     retRNG = new CLHEP::DualRand;
  }
  else if ( dynamic_cast<const CLHEP::RanluxEngine*>(aNewRNG) ) {
     retRNG = new CLHEP::RanluxEngine;
  }
  else if ( dynamic_cast<const CLHEP::RanshiEngine*>(aNewRNG) ) {
     retRNG = new CLHEP::RanshiEngine;
  }
  else {
     // TODO: think about using proper ATLAS messaging and error handling..?
     G4ExceptionDescription msg;
     msg << " Unknown type of RNG Engine - " << G4endl
         << " Can cope only with MTwistEngine, HepJamesRandom, Ranecu, Ranlux64,"
         << " MTwistEngine, DualRand, Ranlux or Ranshi."
         << G4endl
         << " Cannot clone this type of RNG engine, as required for this thread"
         << G4endl
         << " Aborting " << G4endl;
     G4Exception("G4UserWorkerInitializition::SetupRNGEngine()",
                 "Run10099", FatalException, msg);
  }

  // Set the new engine. I suspect ATLAS seed information is lost :(
  G4Random::setTheEngine( retRNG );
}

G4WorkerRunManager* G4AtlasUserWorkerThreadInitialization::
CreateWorkerRunManager() const
{
  // TODO: maybe better to just use the pseudo-singleton mechanism
  return G4AtlasWorkerRunManager::GetG4AtlasWorkerRunManager();
}

#endif // G4MULTITHREADED
