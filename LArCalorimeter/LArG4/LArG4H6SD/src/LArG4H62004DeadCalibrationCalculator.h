/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArG4H62004DeadCalibrationCalculator_H
#define LArG4H62004DeadCalibrationCalculator_H

#include "LArG4Code/VCalibrationCalculator.h"
#include "LArG4Code/LArG4Identifier.h"
#include "CaloG4Sim/SimulationEnergies.h"

#include "globals.hh"

#include <vector>


class LArG4H62004DeadCalibrationCalculator : public LArG4::VCalibrationCalculator {
public:

      LArG4H62004DeadCalibrationCalculator();
      virtual ~LArG4H62004DeadCalibrationCalculator();


      virtual G4bool Process (const G4Step* step,
                              const eCalculatorProcessing p = kEnergyAndID);

      // The cell identifier determined by the Process method.
      virtual const LArG4Identifier& identifier() const { return m_identifier; }

      // The calibration energies as determined by the Process method for
      // the current G4Step.  Units are the native G4 unit of energy.
      virtual const std::vector<G4double>& energies() const { return m_energies; }

private:
      LArG4Identifier m_identifier;
      std::vector<G4double> m_energies;

      // Energy calculator
      CaloG4::SimulationEnergies m_energyCalculator;

};
#endif
