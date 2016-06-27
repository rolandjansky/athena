/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASALG_G4ATLASUSERWORKERTHREADINITIALIZATION_H
#define G4ATLASALG_G4ATLASUSERWORKERTHREADINITIALIZATION_H

// Hide multi-threading classes from builds without G4MT
#ifdef G4MULTITHREADED

#include "G4UserWorkerThreadInitialization.hh"

/// @brief ATLAS custom class for thread initialization functionality
///
/// What we currently override from the G4 base class:
/// * Thread-private random number generator engine
/// * Creation of a worker run manager (G4AtlasWorkerRunManager)
///
/// TODO: do we need messaging?
/// TODO: make this work better with HepRndm Engine.
///       Currently seeds will not be set correctly!
///
/// @author Steve Farrell <Steven.Farrell@cern.ch>
///
class G4AtlasUserWorkerThreadInitialization : public G4UserWorkerThreadInitialization {

public:

  /// @brief Override thread-local setup of RNG engine.
  /// This currently just clones the master's engine.
  /// Assumes we're just using a MTwistEngine for now.
  void SetupRNGEngine(const CLHEP::HepRandomEngine* aRNGEngine) const;

  /// @brief Create ATLAS worker run manager
  G4WorkerRunManager* CreateWorkerRunManager() const;

}; // class G4AtlasUserWorkerThreadInitialization

#endif // G4MULTITHREADED

#endif
