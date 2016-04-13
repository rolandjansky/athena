/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTruthBase_TruthStrategyUtils_H
#define MCTruthBase_TruthStrategyUtils_H

/// @brief This file contains helper functions for truth strategies.

// STL includes
#include <vector>
#include <memory>

// HepMC includes
//#include "HepMC/GenVertex.h"
//#include "HepMC/GenParticle.h"

// HepMC forward declarations
namespace HepMC
{
  class GenVertex;
  class GenParticle;
}

// Geant4 forward declarations
class G4Track;
class G4StepPoint;


namespace TruthStrategyUtils
{

  /// @brief Convert step-point into a new vertex.
  ///
  /// Constructs and returns a new GenVertex from the space-time 4-vector of
  /// the G4StepPoint argument. Caller owns the returned vertex.
  ///
  std::unique_ptr<HepMC::GenVertex> stepPoint2Vertex(const G4StepPoint* stepPoint);


  /// @brief Convert track into new particle.
  ///
  /// Constructs and returns a new GenParticle from the track kinematics and
  /// PDG ID of the provided G4Track. Caller owns the returned particle.
  ///
  std::unique_ptr<HepMC::GenParticle> track2Particle(const G4Track* track);


  /// @brief Construct and save a truth vertex from list of secondaries.
  ///
  /// This function is intended to be invoked by the truth strategies once a
  /// truth vertex has passed all checks and ready to be saved in the MC truth.
  /// It takes a primary track, the current step, and a list of new secondary
  /// tracks. It constructs a GenVertex, fills it with GenParticles, and then
  /// saves the results to the GenEvent.
  ///
  /// Implementation adapted from TruthStrategyManager::SaveSecondaryVertex.
  /// This version is safe to use in multi-threading.
  ///
  /// @todo Should this return something more useful??
  ///
  void saveSecondaryVertex(G4Track*, G4StepPoint*,
                           const std::vector<const G4Track*>&);

} // namespace TruthStrategyUtils

#endif
