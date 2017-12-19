/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "DistanceCalculatorFactory.h"

#include "DistanceCalculatorSaggingOff.h"
#include "DistanceCalculatorSaggingOn.h"

namespace LArWheelCalculator_Impl
{

  IDistanceCalculator* DistanceCalculatorFactory::Create(const std::string & sagging_opt,
                                                         LArWheelCalculator* lwc,
                                                         IRDBAccessSvc* rdbAccess,
                                                         const DecodeVersionKey & larVersionKey)
  {
    // the same condition is in LArWheelCalculator constructor
    bool SaggingOn = (sagging_opt != "" && sagging_opt != "off")? true: false;

    if (SaggingOn) {
      return new DistanceCalculatorSaggingOn(sagging_opt, lwc, rdbAccess, larVersionKey);
    } else {
      return new DistanceCalculatorSaggingOff(lwc, rdbAccess, larVersionKey);
    }
  }

}
