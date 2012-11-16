/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArG4_HEC_LocalCalibrationCalculator_H
#define LArG4_HEC_LocalCalibrationCalculator_H

#include "LArG4Code/VCalibrationCalculator.h"
#include "LArG4Code/LArG4Identifier.h"
#include "LArG4HEC/LocalGeometry.h"
#include "CaloG4Sim/SimulationEnergies.h"

#include "globals.hh"

#include <vector>

// Forward declaractions:
class G4Step;

namespace LArG4 {

  namespace HEC {


    class LocalCalibrationCalculator : public VCalibrationCalculator {
    public:
    
      LocalCalibrationCalculator(const eLocalGeometryType type = kLocActive);
      virtual ~LocalCalibrationCalculator();
    
      virtual G4bool Process (const G4Step* step, 
			      const eCalculatorProcessing p = kEnergyAndID);
    
      virtual LArG4Identifier identifier() const { return m_identifier; }
    
      virtual const std::vector<G4double>& energies() const { return m_energies; }

    private:
      LArG4Identifier m_identifier;

      std::vector<G4double> m_energies;

      LocalGeometry* m_geometryCalculator;

      eLocalGeometryType m_geometryType;

      CaloG4::SimulationEnergies m_energyCalculator;

    };

  } // namespace HEC

} // namespace LArG4

#endif // LArG4_HEC_LocalCalibrationCalculator_H
