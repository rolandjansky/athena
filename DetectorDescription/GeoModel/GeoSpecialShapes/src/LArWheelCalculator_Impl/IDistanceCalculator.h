/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LArWheelCalculator_Impl_IDistanceCalculator_H__
#define __LArWheelCalculator_Impl_IDistanceCalculator_H__

// This is an interface of distance calculation to parts of the LAr endcap.

#include "CLHEP/Vector/ThreeVector.h"

namespace LArWheelCalculator_Impl {
  class IDistanceCalculator {
    public:
	virtual ~IDistanceCalculator() {};
  // geometry methods:

	virtual double DistanceToTheNeutralFibre(const CLHEP::Hep3Vector &p, int fan_number) const = 0;  // depends on sagging flag
	virtual CLHEP::Hep3Vector NearestPointOnNeutralFibre(const CLHEP::Hep3Vector &p, int fan_number) const = 0; // depends on sagging flag
	virtual double AmplitudeOfSurface(const CLHEP::Hep3Vector& p, int side, int fan_number) const = 0;  // depends on sagging flag
  };

}
#endif // __LArWheelCalculator_Impl_IDistanceCalculator_H__

