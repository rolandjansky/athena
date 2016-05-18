/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArG4H6COLDTCMod0CalibCalculator_H
#define LArG4H6COLDTCMod0CalibCalculator_H


#include "LArG4Code/VCalibrationCalculator.h"
#include "LArG4H6COLDTCMod0Calculator.h"

#include "LArG4Code/LArG4Identifier.h"
#include "CaloG4Sim/SimulationEnergies.h"

#include "globals.hh"

#include <vector>

class LArG4H6COLDTCMod0CalibCalculator : public LArG4::VCalibrationCalculator {

public:

      LArG4H6COLDTCMod0CalibCalculator();
      virtual ~LArG4H6COLDTCMod0CalibCalculator();

      virtual G4bool Process(const G4Step*, const eCalculatorProcessing p = kEnergyAndID);
      virtual const LArG4Identifier& identifier() const {return m_identifier;}
      virtual const LArG4Identifier& identifier(int i=0) const {
               if (i!=0) throw std::range_error("Multiple hits not yet implemented");
                   return m_identifier;
      }
      virtual const std::vector<G4double>& energies() const { return m_energies; }

    private:
      LArG4Identifier m_identifier;
      std::vector<G4double> m_energies;

      LArG4H6COLDTCMod0Calculator* m_Calculator;

      CaloG4::SimulationEnergies m_energyCalculator;

};

#endif
