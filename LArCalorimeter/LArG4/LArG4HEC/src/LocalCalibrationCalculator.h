/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArG4_HEC_LocalCalibrationCalculator_H
#define LArG4_HEC_LocalCalibrationCalculator_H

#include "LArG4Code/LArCalibCalculatorSvcImp.h"
#include "LArG4Code/LArG4Identifier.h"
#include "LArG4HEC/ILocalGeometry.h"
#include "CaloG4Sim/SimulationEnergies.h"

#include "globals.hh"

#include <vector>

// Forward declaractions:
class G4Step;

namespace LArG4 {

  namespace HEC {


    class LocalCalibrationCalculator : public LArCalibCalculatorSvcImp {
    public:

      LocalCalibrationCalculator(const std::string& name, ISvcLocator *pSvcLocatorconst);
      void GeometryTypeUpdateHandler(Gaudi::Details::PropertyBase&);
      StatusCode initialize() override final;
      virtual ~LocalCalibrationCalculator();

      virtual G4bool Process (const G4Step* step, LArG4Identifier & identifier,
                              std::vector<G4double> & energies,
                              const eCalculatorProcessing process = kEnergyAndID) const override final;
   private:

      ServiceHandle<ILocalGeometry> m_geometryCalculator;

      eLocalGeometryType m_geometryType;
      StringProperty m_strgeometryType;

      CaloG4::SimulationEnergies m_energyCalculator;

    };

  } // namespace HEC

} // namespace LArG4

#endif // LArG4_HEC_LocalCalibrationCalculator_H
