/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LArWheelCalculator_Impl_ModuleFanCalculator_H__
#define __LArWheelCalculator_Impl_ModuleFanCalculator_H__

#include "IFanCalculator.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"

class LArWheelCalculator;

namespace LArWheelCalculator_Impl
{

  /// This is an interface of distance calculation to parts of the LAr endcap.
  class ModuleFanCalculator : public IFanCalculator
  {
    public:
      ModuleFanCalculator(LArWheelCalculator* lwc,
                          IRDBAccessSvc* rdbAccess,
                          const DecodeVersionKey & larVersionKey);

      // geometry methods:
      virtual double DistanceToTheNearestFan(CLHEP::Hep3Vector &p, int & out_fan_number) const;
      virtual int PhiGapNumberForWheel(int i) const;
      virtual std::pair<int, int> GetPhiGapAndSide(const CLHEP::Hep3Vector &p) const;

      inline const LArWheelCalculator *lwc() const { return m_lwc; };

    private:
      LArWheelCalculator* m_lwc;

  };

}

#endif // __LArWheelCalculator_Impl_ModuleFanCalculator_H__
