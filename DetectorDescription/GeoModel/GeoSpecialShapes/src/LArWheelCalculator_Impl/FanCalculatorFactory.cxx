/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "FanCalculatorFactory.h"
#include "ModuleFanCalculator.h"
#include "WheelFanCalculator.h"

namespace LArWheelCalculator_Impl
{

  IFanCalculator* FanCalculatorFactory::Create(bool isSaggingOn, bool isModule,
                                               LArWheelCalculator* lwc)

  {
    if (isModule) {
      return new ModuleFanCalculator(lwc);
    }
    if (isSaggingOn) {
      return new WheelFanCalculator<SaggingOn_t>(lwc);
    } else {
      return new WheelFanCalculator<SaggingOff_t>(lwc);
    }
  }

}
