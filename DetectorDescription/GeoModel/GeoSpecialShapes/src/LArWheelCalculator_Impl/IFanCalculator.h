/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LArWheelCalculator_Impl_IFanCalculator_H__
#define __LArWheelCalculator_Impl_IFanCalculator_H__


#include "CLHEP/Vector/ThreeVector.h"

namespace LArWheelCalculator_Impl
{

  // This is an interface of distance calculation to parts of the LAr endcap.
  class IFanCalculator
  {
    public:
      virtual ~IFanCalculator() {};

      // geometry methods:
      virtual double DistanceToTheNearestFan(CLHEP::Hep3Vector &p, int & out_fan_number) const = 0;
      virtual int PhiGapNumberForWheel(int i) const = 0;
      virtual std::pair<int, int> GetPhiGapAndSide(const CLHEP::Hep3Vector &p) const = 0;
  };

}
#endif // __LArWheelCalculator_Impl_IFanCalculator_H__
