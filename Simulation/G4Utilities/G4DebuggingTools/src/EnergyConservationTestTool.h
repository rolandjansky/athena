/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_G4UA__ENERGYCONSERVATIONTESTTOOL_H
#define G4DEBUGGINGTOOLS_G4UA__ENERGYCONSERVATIONTESTTOOL_H

#include "G4AtlasTools/UserActionToolBase.h"
#include "EnergyConservationTest.h"

namespace G4UA
{

  class EnergyConservationTestTool : public UserActionToolBase<EnergyConservationTest>
  {
  public:
    EnergyConservationTestTool(const std::string& type, const std::string& name,const IInterface* parent);
  protected:
    virtual std::unique_ptr<EnergyConservationTest>
    makeAndFillAction(G4AtlasUserActions&) override final;
  }; // class EnergyConservationTestTool

} // namespace G4UA

#endif
