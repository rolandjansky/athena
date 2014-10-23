/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Aug-2008: M.Fincke  First implementation for SLHC MiniFCal

#ifndef MiniFCALCalibrationCalculator_H
#define MiniFCALCalibrationCalculator_H

#include "CaloG4Sim/SimulationEnergies.h"
#include "LArG4Code/VCalibrationCalculator.h"
#include "LArG4Code/LArG4Identifier.h"
#include "LArG4MiniFCAL/MiniFCALAssignIdentifier.h"

#include "globals.hh"

#include <vector>

// Forward declaractions:
class G4Step;

//namespace CaloG4 {
//  class SimulationEnergies;
//}

namespace LArG4 {

  namespace MiniFCAL {


    class MiniFCALCalibrationCalculator : public VCalibrationCalculator {
    public:
    
      MiniFCALCalibrationCalculator(const eMiniFCALAssignIdentifierType type = kActive);
      virtual ~MiniFCALCalibrationCalculator();
    
      virtual G4bool Process (const G4Step* step, 
			      const eCalculatorProcessing p = kEnergyAndID);
    
      virtual const LArG4Identifier& identifier() const { return m_identifier; }
    
      virtual const std::vector<G4double>& energies() const { return m_energies; }

    private:
      LArG4Identifier m_identifier;

      std::vector<G4double> m_energies;

      MiniFCALAssignIdentifier* m_geometryCalculator;

      eMiniFCALAssignIdentifierType m_geometryType;

      CaloG4::SimulationEnergies m_energyCalculator;

    };

  } // namespace MiniFCAL

} // namespace LArG4

#endif // MiniFCALCalibrationCalculator_H
