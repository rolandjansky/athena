/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PFCALCRADIALENERGYPROFILES
#define PFCALCRADIALENERGYPROFILES

#include "AsgMessaging/AsgMessaging.h"
#include "GaudiKernel/Bootstrap.h"
#include "eflowRec/PFData.h"

class PFCalcRadialEnergyProfiles : public asg::AsgMessaging
{

public:
  PFCalcRadialEnergyProfiles()
    : asg::AsgMessaging("PFCalcRadialEnergyProfiles")
  {
  }

  ~PFCalcRadialEnergyProfiles() = default;

  void calculate(const PFData& data) const;
};
#endif
