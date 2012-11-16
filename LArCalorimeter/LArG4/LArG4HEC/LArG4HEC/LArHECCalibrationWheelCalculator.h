/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Jan-2008: (M.Fincke)  To be used for new Module-Geometry 

#ifndef LArG4_HEC_LArHECCalibrationWheelCalculator_H
#define LArG4_HEC_LArHECCalibrationWheelCalculator_H

#include "LArG4Code/VCalibrationCalculator.h"
#include "LArG4Code/LArG4Identifier.h"
#include "LArG4HEC/HECGeometry.h"
#include "CaloG4Sim/SimulationEnergies.h"

#include "globals.hh"

#include <vector>

// Forward declaractions:
class G4Step;

namespace LArG4 {

  namespace HEC {


    class LArHECCalibrationWheelCalculator : public VCalibrationCalculator {
    public:
    
      LArHECCalibrationWheelCalculator(const eHECGeometryType type = kWheelActive);
      virtual ~LArHECCalibrationWheelCalculator();
    
      virtual G4bool Process (const G4Step* step, 
			      const eCalculatorProcessing p = kEnergyAndID);
    
      virtual LArG4Identifier identifier() const { return m_identifier; }
    
      virtual const std::vector<G4double>& energies() const { return m_energies; }

    private:
      LArG4Identifier m_identifier;

      std::vector<G4double> m_energies;

      HECGeometry* m_geometryCalculator;

      eHECGeometryType m_geometryType;

      CaloG4::SimulationEnergies m_energyCalculator;

    };

  } // namespace HEC

} // namespace LArG4

#endif // LArG4_HEC_LArHECCalibrationWheelCalculator_H
