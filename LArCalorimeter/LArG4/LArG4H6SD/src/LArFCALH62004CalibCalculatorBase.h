/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArFCALH62004CalibCalculatorBase_H
#define LArFCALH62004CalibCalculatorBase_H

#include "LArG4Code/LArCalibCalculatorSvcImp.h"
#include "LArG4Code/LArG4Identifier.h"
#include "CaloG4Sim/SimulationEnergies.h"
#include "globals.hh"
#include <vector>

// Forward declaration for namespace CaloG4.
class G4Step;
class FCAL_ChannelMap;
class IRDBRecordset;

class LArFCALH62004CalibCalculatorBase : public LArCalibCalculatorSvcImp {

public:
  LArFCALH62004CalibCalculatorBase(const std::string& name, ISvcLocator * pSvcLocator);
  virtual StatusCode initialize() override final;
  virtual ~LArFCALH62004CalibCalculatorBase();

  virtual G4bool Process (const G4Step* step, LArG4Identifier & identifier,
                          std::vector<G4double> & energies,
                          const LArG4::eCalculatorProcessing p = LArG4::kEnergyAndID) const override final;

protected:

  G4double m_deltaX;
  G4double m_deltaY;

  G4int m_FCalSampling;

private:

  // Energy calculator
  CaloG4::SimulationEnergies m_energyCalculator;

  FCAL_ChannelMap   *m_ChannelMap;

  const IRDBRecordset* m_fcalMod;
  float m_Zshift;

};


#endif    // LArG4_HEC_CalibrationCalculator_H
