/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LArWheelCalculator_Impl_DistanceCalculatorSaggingOff_H__
#define __LArWheelCalculator_Impl_DistanceCalculatorSaggingOff_H__

// This class implements details of distance calculation to parts of the LAr endcap without sagging corrections.

#include "IDistanceCalculator.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"

class LArWheelCalculator;

namespace LArWheelCalculator_Impl {
  class DistanceCalculatorSaggingOff : public IDistanceCalculator {
    public:
	  DistanceCalculatorSaggingOff(LArWheelCalculator* lwc, IRDBAccessSvc* rdbAccess, const DecodeVersionKey & larVersionKey);

  // geometry methods:
	  virtual double DistanceToTheNeutralFibre(const CLHEP::Hep3Vector &p) const;
	  virtual double DistanceToTheNeutralFibre_ref(const CLHEP::Hep3Vector &p) const;
	  virtual CLHEP::Hep3Vector NearestPointOnNeutralFibre(const CLHEP::Hep3Vector &p) const;
	  virtual CLHEP::Hep3Vector NearestPointOnNeutralFibre_ref(const CLHEP::Hep3Vector &p) const;
	  virtual double AmplitudeOfSurface(const CLHEP::Hep3Vector& P, int side) const;

  // Return the calculator:
	  inline const LArWheelCalculator *lwc() const { return m_lwc; };
    private:

	LArWheelCalculator* m_lwc;
	double m_EndQuarterWave;
  };

}
#endif // __LArWheelCalculator_Impl_IDistanceCalculatorOff_H__

