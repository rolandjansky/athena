/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArG4H62004DeadCalibrationCalculator_H
#define LArG4H62004DeadCalibrationCalculator_H

#include "LArG4Code/LArCalibCalculatorSvcImp.h"
#include "LArG4Code/LArG4Identifier.h"
#include "CaloG4Sim/SimulationEnergies.h"

#include "globals.hh"

#include <vector>

class G4Step;

class LArG4H62004DeadCalibrationCalculator : public LArCalibCalculatorSvcImp {
public:

      LArG4H62004DeadCalibrationCalculator(const std::string& name, ISvcLocator * pSvcLocator);
      virtual ~LArG4H62004DeadCalibrationCalculator();


      virtual G4bool Process (const G4Step* step, LArG4Identifier & identifier,
                              std::vector<G4double> & energies,
                              const LArG4::eCalculatorProcessing p = LArG4::kEnergyAndID) const override final;

private:
      // Energy calculator
      CaloG4::SimulationEnergies m_energyCalculator;

};
#endif
