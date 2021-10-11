/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LArG4H6WarmTCCalculator_H__
#define __LArG4H6WarmTCCalculator_H__

#include "LArG4Code/LArCalculatorSvcImp.h"
#include "LArG4Code/LArG4Identifier.h"

// Forward declarations.
class G4Step;

class LArG4H6WarmTCCalculator : public LArCalculatorSvcImp
{
public:

  LArG4H6WarmTCCalculator(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~LArG4H6WarmTCCalculator() {};
  /////////////////////////////////////////////
  // The interface for ILArCalculatorSvc.

  virtual G4float OOTcut() const override final { return m_OOTcut; }

  virtual G4bool Process(const G4Step*, std::vector<LArHitData>&) const override final;

  virtual G4bool isInTime(G4double hitTime) const override final
  {
    return !(hitTime > m_OOTcut);
  }

private:
  bool m_isX = false;
  bool m_isABS = false;
};

#endif
