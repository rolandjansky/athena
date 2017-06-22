/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4H6SD_LArG4H6COLDTCMod0CalibCalculator_H
#define LARG4H6SD_LArG4H6COLDTCMod0CalibCalculator_H


#include "LArG4Code/LArCalibCalculatorSvcImp.h"
#include "LArG4Code/LArG4Identifier.h"
#include "CaloG4Sim/SimulationEnergies.h"
#include "globals.hh"
#include <vector>

class ILArCalculatorSvc;
class G4Step;

class LArG4H6COLDTCMod0CalibCalculator : public LArCalibCalculatorSvcImp {

public:

  LArG4H6COLDTCMod0CalibCalculator(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize() override final;
  virtual ~LArG4H6COLDTCMod0CalibCalculator();

  virtual G4bool Process(const G4Step*, LArG4Identifier & identifier,
                         std::vector<G4double> & energies,
                         const LArG4::eCalculatorProcessing p = LArG4::kEnergyAndID) const override final;

private:
  ServiceHandle<ILArCalculatorSvc> m_Calculator; //LArG4H6COLDTCMod0Calculator

  CaloG4::SimulationEnergies m_energyCalculator;

};

#endif
