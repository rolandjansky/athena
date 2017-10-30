/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LArWheelCalculator_Impl_FanCalculatorFactory_H__
#define __LArWheelCalculator_Impl_FanCalculatorFactory_H__

// FanCalculator factory
// calculator creation depends on sagging mode and wheel/module calo

#include "IFanCalculator.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"

class LArWheelCalculator;

namespace LArWheelCalculator_Impl
{

  /// @todo Why is this a class???
  class FanCalculatorFactory
  {
    public:
      static IFanCalculator* Create(bool isSaggingOn, bool isModule,
                                    LArWheelCalculator* lwc,
                                    IRDBAccessSvc* rdbAccess,
                                    const DecodeVersionKey & larVersionKey);
  };

}

#endif // __LArWheelCalculator_Impl_FanCalculatorFactory_H__
