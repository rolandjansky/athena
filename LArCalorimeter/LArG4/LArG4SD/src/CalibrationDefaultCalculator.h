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

#include "LArG4Code/LArCalibCalculatorSvcImp.h"
#include "LArG4Code/LArG4Identifier.h"

#include "CaloG4Sim/SimulationEnergies.h"

#include "globals.hh"

#include <vector>

// Forward declaractions:
class G4Step;

namespace LArG4 {

  class CalibrationDefaultCalculator : public LArCalibCalculatorSvcImp {
  public:

    CalibrationDefaultCalculator(const std::string& name, ISvcLocator *pSvcLocator);
    StatusCode initialize() {return StatusCode::SUCCESS;}
    StatusCode finalize() {return StatusCode::SUCCESS;}
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

    virtual G4bool Process (const G4Step* step, LArG4Identifier & identifier,
                  std::vector<G4double> & energies,
                  const eCalculatorProcessing p = kEnergyAndID) const override final;

  private:


    // The usual calibration energy calculator.
    CaloG4::SimulationEnergies m_energyCalculator;

    // hardcoded numbers
    const double m_oneOverDeta = 10.;       //   1/Deta = 1./0.1 = 10.
    const double m_oneOverDphi = 32./M_PI;  //   1/Dphi
    const int    m_phiBinMax = 63;
    const double m_rhoAlignmentSafety = 50.*CLHEP::mm;
    const double m_zAlignmentSafety = 100.*CLHEP::mm;
    const double m_rhoCalorOut = 3885.*CLHEP::mm - m_rhoAlignmentSafety;
    const double m_zCalorOut = 6100.*CLHEP::mm - m_zAlignmentSafety;
    const double m_rhoInDetOut = 1150.*CLHEP::mm + m_rhoAlignmentSafety;
    const double m_zInDetOut = 3511.*CLHEP::mm + m_zAlignmentSafety;
    const double m_startZFCal1 = 4668.5 - m_zAlignmentSafety;
    const double m_z1BeforeFCal = 4225.5 + m_zAlignmentSafety; //
    const double m_z2BeforeFCal = 4557.5 + m_zAlignmentSafety; //


  };

} // namespace LArG4

#endif // LArG4_CalibrationDefaultCalculator_H
