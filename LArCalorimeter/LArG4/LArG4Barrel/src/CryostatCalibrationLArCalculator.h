/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// LArG4::BarrelCryostat::CalibrationLArCalculator
// Prepared 13-Aug-2004 Bill Seligman

// This class calculates the values needed for calibration hits in the
// simulation.

// A "calculator" is used in much the same way as a hand-held
// calculator might be.  The user supplies a value and hits 'Enter'
// (i.e., invokes the Process() method).  Then they read off whatever
// values are of interest.

// This particular class is meant to be be used for general LAr
// regions within the barrel cryostat.  The liquid-argon volumes
// typically "fill in the cracks" between all other volumes; this
// calculator determines the identifiers for such "cracks".

#ifndef LArG4_BarrelCryostat_CalibrationLArCalculator_H
#define LArG4_BarrelCryostat_CalibrationLArCalculator_H

#include "LArG4Code/LArCalibCalculatorSvcImp.h"
#include "LArG4Code/LArG4Identifier.h"
#include "LArG4Code/LArVG4DetectorParameters.h"

#include "CaloG4Sim/SimulationEnergies.h"

#include "globals.hh"

#include <vector>

// Forward declaractions:
class G4Step;


// Note the use of nested namespaces (mainly to prevent long names
// like LArG4HECCalibrationCalculator).  This class is contained in
// the namespace LArG4::BarrelCryostat.

namespace LArG4 {

  namespace BarrelCryostat {

    class CalibrationLArCalculator : public LArCalibCalculatorSvcImp {
    public:

      CalibrationLArCalculator(const std::string& name, ISvcLocator *pSvcLocator);
      StatusCode initialize() override final;
      virtual ~CalibrationLArCalculator();

      // The Process method returns a boolean value.  If it's true, the
      // hit can be used by Geant4; if it's false, there's something wrong
      // with the energy deposit and it should be ignored.

      // For calibration work, most of the time we want the calculator
      // to determine both the energy and the identifier.  However,
      // sometimes we want it calculate only the identifier (for
      // escaped energy), or only the energy (no known application
      // yet, but you can never tell).  Use the enum (defined in
      // VCalibrationCalculator.h) to control any special processing.
      virtual G4bool Process (const G4Step* step, LArG4Identifier & identifier,
                              std::vector<G4double> & energies,
                              const eCalculatorProcessing process = kEnergyAndID) const override final;

    private:
      // Energy calculator
      CaloG4::SimulationEnergies m_energyCalculator;

      // Access to parameters.
      const LArVG4DetectorParameters* m_parameters;

      ServiceHandle<ILArCalibCalculatorSvc> m_defaultCalculator;

    };

  } // namespace BarrelCryostat

} // namespace LArG4

#endif // LArG4_BarrelCryostat_CalibrationLArCalculator_H
