/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// VEscapedEnergyProcessing
// 13-Jul-2004 William Seligman

#ifndef CALOG4SIM_VESCAPEDENERGYPROCESSING_H
#define CALOG4SIM_VESCAPEDENERGYPROCESSING_H

#include "G4Step.hh"
#include "globals.hh"

namespace CaloG4
{
  /**
   * @class VEscapedEnergyProcessing
   * @brief Virtual interface for escaped energy processor classes.
   *
   * The SimulationEnergies class provides a common procedure for
   * categorizing the energy deposited in a given G4Step.  However,
   * different detectors have different scheme for handling one of the
   * categories: escaped energy.
   *
   * The issue is that, if a particle's energy is lost by escaping the
   * simulation, you don't want to record that energy in the volume it
   * escapes; you want to record that energy in the volume in which the
   * particle was created.  Neutrinos are a good example of this.
   *
   * In effect, the SimulationEnergies class has to issue an "interrupt"
   * to some other volume than the current G4Step, telling that other
   * volume to accumulate the energy of the escaped particle.  The Tile
   * Simulation and the LAr simulation handle this interrupt
   * differently, since they organize sensitive detectors in a different
   * way.
   *
   * This interface "hides" the different escaped-energy processing
   * required by the different detectors in the simulation.
   *
   */

  class VEscapedEnergyProcessing
  {

  public:

    /// Virtual destructor
    virtual ~VEscapedEnergyProcessing() {}

    virtual G4bool Process( G4Step* fakeStep ) = 0;
  };

} // namespace CaloG4

#endif
