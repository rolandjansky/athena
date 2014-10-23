/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CalibrationDefaultCalculator
// Prepared 04-Mar-2004 William Seligman

// The calibration studies rely on every volume in the simulation
// being made into a sensitive detector.  There is a practical
// problem: What if we're still in the middle of developing code, and
// not every volume has been made sensitive yet?  What if we've
// overlooked a volume?

// This class provides a "default behavior" for all energy deposits
// that are not made in a volume that's been made sensitive for
// calibration studies.

#ifndef LArG4_CalibrationDefaultCalculator_H
#define LArG4_CalibrationDefaultCalculator_H

#include "LArG4Code/VCalibrationCalculator.h"
#include "LArG4Code/LArG4Identifier.h"

#include "CaloG4Sim/SimulationEnergies.h"

#include "globals.hh"

#include <vector>

// Forward declaractions:
class G4Step;

namespace LArG4 {

  class CalibrationDefaultCalculator : public VCalibrationCalculator {
  public:
    
    CalibrationDefaultCalculator();
    virtual ~CalibrationDefaultCalculator();
    
    // The Process method returns a boolean value.  If it's true, the
    // hit can be used by Geant4; if it's false, there's something wrong
    // with the energy deposit and it should be ignored.

    // For calibration work, most of the time we want the calculator
    // to determine both the energy and the identifier.  However,
    // sometimes we want it calculate only the identifier (for escaped
    // energy), or only the energy (no known application yet, but you
    // can never tell).  Use the enum to control any special
    // processing.

    virtual G4bool Process (const G4Step*, 
			    const eCalculatorProcessing = kEnergyAndID);
    
    // The cell identifier determined by the Process method.
    virtual const LArG4Identifier& identifier() const { return m_identifier; }
    
    // The calibration energies as determined by the Process method for
    // the current G4Step.  Units are the native G4 unit of energy.
    virtual const std::vector<G4double>& energies() const { return m_energies; }

  private:

    // The results of the calculation.
    LArG4Identifier m_identifier;
    std::vector<G4double> m_energies;

    // The usual calibration energy calculator.
    CaloG4::SimulationEnergies m_energyCalculator;

  };

} // namespace LArG4

#endif // LArG4_CalibrationDefaultCalculator_H
