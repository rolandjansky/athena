/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "FanCalculatorFactory.h"
#include "ModuleFanCalculator.h"
#include "WheelFanCalculator.h"

namespace LArWheelCalculator_Impl
{

  IFanCalculator* FanCalculatorFactory::Create(bool isSaggingOn, bool isModule,
                                               LArWheelCalculator* lwc,
                                               IRDBAccessSvc* rdbAccess,
                                               const DecodeVersionKey& larVersionKey)
  {
    if (isModule) {
      return new ModuleFanCalculator(lwc, rdbAccess, larVersionKey);
    }
    if (isSaggingOn) {
      return new WheelFanCalculator<SaggingOn_t>(lwc, rdbAccess, larVersionKey);
    } else {
      return new WheelFanCalculator<SaggingOff_t>(lwc, rdbAccess, larVersionKey);
    }
  }

}
