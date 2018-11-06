/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EscapedEnergyProcessing
// 15-Jul-2004 William Seligman

#ifndef LArG4SD_EscapedEnergyProcessing_H
#define LArG4SD_EscapedEnergyProcessing_H

// The SimulationEnergies class provides a common procedure for
// categorizing the energy deposited in a given G4Step.  However,
// special processing is required for escaped energy.

// The issue is that, if a particle's energy is lost by escaping the
// simulation, you don't want to record that energy in the volume it
// escapes; you want to record that energy in the volume in which the
// particle was created.  Neutrinos are a good example of this.

// In effect, the SimulationEnergies class has to issue an "interrupt"
// to some other volume than the current G4Step, telling that other
// volume to accumulate the energy of the escaped particle.

// This class contains the processing to handle the escaped energy
// processing for volumes in the LArG4 portion of the simulation.

#include "CaloG4Sim/VEscapedEnergyProcessing.h"

#include "G4TouchableHandle.hh"
#include "G4ThreeVector.hh"
#include "G4Step.hh"
#include "globals.hh"

class LArG4CalibSD;

class EscapedEnergyProcessing : public CaloG4::VEscapedEnergyProcessing
{
public:
  EscapedEnergyProcessing(LArG4CalibSD* SD);
  virtual ~EscapedEnergyProcessing();

  // Method: The G4TouchableHandle to the volume in which "point" is
  // located; the value of "point" itself in case additional
  // processing is necessary, and the amount of escaped energy.

  virtual G4bool Process( G4Step* fakeStep );

private:
  // Local pointer to the default SD that this should put hits into
  LArG4CalibSD* m_defaultSD;
};


#endif // LArG4_EscapedEnergyProcessing_H
