/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Aug-2008: M.Fincke  First implementation for SLHC MiniFCal

#ifndef MiniFCALCalibrationCalculator_H
#define MiniFCALCalibrationCalculator_H

#include "CaloG4Sim/SimulationEnergies.h"
#include "LArG4Code/LArCalibCalculatorSvcImp.h"
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


    class MiniFCALCalibrationCalculator : public LArCalibCalculatorSvcImp {
    public:

      MiniFCALCalibrationCalculator(const std::string& name, ISvcLocator *pSvcLocator);
      StatusCode initialize();
      StatusCode finalize() {return StatusCode::SUCCESS;}
      virtual ~MiniFCALCalibrationCalculator();

      virtual G4bool Process (const G4Step* step, LArG4Identifier & identifier,
                              std::vector<G4double> & energies,
                              const eCalculatorProcessing process = kEnergyAndID) const override final;

    private:
      MiniFCALAssignIdentifier* m_geometryCalculator;

      eMiniFCALAssignIdentifierType m_geometryType;
      std::string m_strgeometryType;

      CaloG4::SimulationEnergies m_energyCalculator;

    };

  } // namespace MiniFCAL

} // namespace LArG4

#endif // MiniFCALCalibrationCalculator_H
