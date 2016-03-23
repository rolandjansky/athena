/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// VCalibrationCalculator.h
// Prepared 08-Jan-2004 Bill Seligman

// This is an abstract base for a calibration hit calculator in LArG4.

// A "calculator" is used in much the same way as a hand-held
// calculator might be.  The user supplies a value and hits 'Enter'
// (i.e., invokes the Process() method).  Then they read off whatever
// values are of interest.

#ifndef LArG4_VCalibrationCalculator_H
#define LArG4_VCalibrationCalculator_H

#include "globals.hh"

#include <vector>

// Forward declaractions:
class LArG4Identifier;
class G4Step;

namespace LArG4 {

  class VCalibrationCalculator {
  public:
    
    virtual ~VCalibrationCalculator() {};
    
    // The Process method returns a boolean value.  If it's true, the
    // hit can be used by Geant4; if it's false, there's something wrong
    // with the energy deposit and it should be ignored.

    // For calibration work, most of the time we want the calculator
    // to determine both the energy and the identifier.  However,
    // sometimes we want it calculate only the identifier (for escaped
    // energy), or only the energy (no known application yet, but you
    // can never tell).  Use the enum to control any special
    // processing.

    enum eCalculatorProcessing { kEnergyAndID, kOnlyEnergy, kOnlyID };

    virtual G4bool Process (const G4Step*, 
			    const eCalculatorProcessing = kEnergyAndID) = 0;
    
    // The cell identifier determined by the Process method.
    virtual const LArG4Identifier& identifier() const = 0;
    
    // The calibration energies as determined by the Process method for
    // the current G4Step.  Units are the native G4 unit of energy.
    virtual const std::vector<G4double>& energies() const = 0;
  };

} // namespace LArG4

#endif // LArG4_VCalibrationCalculator_H
