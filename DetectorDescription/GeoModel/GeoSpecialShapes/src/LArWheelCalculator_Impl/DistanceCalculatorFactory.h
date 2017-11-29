/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LArWheelCalculator_Impl_DistanceCalculatorFactory_H__
#define __LArWheelCalculator_Impl_DistanceCalculatorFactory_H__

// DistanceCalculator factory
// calculator creation depends on sagging mode

#include <string>
#include "IDistanceCalculator.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"

class LArWheelCalculator;

namespace LArWheelCalculator_Impl
{
  /// @todo Why is this a class???
  class DistanceCalculatorFactory
  {
    public:
      static IDistanceCalculator* Create(const std::string & sagging_opt,
                                         LArWheelCalculator* lwc,
                                         IRDBAccessSvc* rdbAccess,
                                         const DecodeVersionKey & larVersionKey);
  };

}

#endif // __LArWheelCalculator_Impl_DistanceCalculatorFactory_H__
