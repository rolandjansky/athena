/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PFCALCRADIALENERGYPROFILES
#define PFCALCRADIALENERGYPROFILES

#include "AthenaBaseComps/AthMessaging.h"
#include "eflowRec/PFData.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/ISvcLocator.h"

class PFCalcRadialEnergyProfiles : public AthMessaging {

  public:
    PFCalcRadialEnergyProfiles()  : AthMessaging(Gaudi::svcLocator()->service<IMessageSvc>("MessageSvc"),"PFCalcRadialEnergyProfiles ") {};
    ~PFCalcRadialEnergyProfiles() {};

    void calculate(const PFData& data);

};
#endif