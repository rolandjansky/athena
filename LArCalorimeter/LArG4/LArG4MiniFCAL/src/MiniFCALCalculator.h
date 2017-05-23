/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MiniFCALCalculator.hh

// Revision history:

// Aug-2008: M.Fincke  First implementation for SLHC MiniFCal

#ifndef LARG4MINIFCAL_MINIFCALCALCULATOR_H
#define LARG4MINIFCAL_MINIFCALCALCULATOR_H

#include "LArG4Code/LArG4Identifier.h"
#include "LArG4Code/LArCalculatorSvcImp.h"
#include <stdexcept>

// Forward declarations.
class G4Step;
class MiniFCALAssignIdentifier;
namespace LArG4 {
  namespace MiniFCAL {
     class MiniFCALAssignIdentifier;
  }
}


class MiniFCALCalculator : public LArCalculatorSvcImp {
public:

  MiniFCALCalculator(const std::string& name, ISvcLocator * pSvcLocator);
  StatusCode initialize();

  /////////////////////////////////////////////

  virtual G4float OOTcut() const override final { return m_OOTcut; }
  virtual void SetOutOfTimeCut(G4double c) { m_OOTcut = c; } //FIXME public but not part of interface class


  virtual G4bool Process(const G4Step*, std::vector<LArHitData>&) const override final;

  virtual G4bool isInTime(G4double hitTime) const override final
  {
    return !(hitTime > m_OOTcut); //FIXME should we be checking the absolute value of hdata[0].time here?
  }

private:

  LArG4::MiniFCAL::MiniFCALAssignIdentifier *m_Geometry;

  G4float  m_OOTcut;
};

#endif // LARG4MINIFCAL_MINIFCALCALCULATOR_H
