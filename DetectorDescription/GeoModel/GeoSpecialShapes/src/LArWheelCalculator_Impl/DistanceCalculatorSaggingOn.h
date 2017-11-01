/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LArWheelCalculator_Impl_DistanceCalculatorSaggingOn_H__
#define __LArWheelCalculator_Impl_DistanceCalculatorSaggingOn_H__

#include "DistanceCalculatorSaggingOff.h"

class LArWheelCalculator;

namespace LArWheelCalculator_Impl
{

  /// @class DistanceCalculatorSaggingOn
  /// @brief Implements details of distance calculation to parts of the
  /// LAr endcap with sagging taken into account.
  ///
  class DistanceCalculatorSaggingOn : public DistanceCalculatorSaggingOff
  {

    public:

      typedef DistanceCalculatorSaggingOff parent;

      /// Constructor
      DistanceCalculatorSaggingOn(const std::string& saggingOptions,
                                  LArWheelCalculator* lwc,
                                  IRDBAccessSvc* rdbAccess,
                                  const DecodeVersionKey & larVersionKey);

      /// @name Geometry methods
      /// @{
      virtual double DistanceToTheNeutralFibre(const CLHEP::Hep3Vector &p, int fan_number) const;
      virtual CLHEP::Hep3Vector NearestPointOnNeutralFibre(const CLHEP::Hep3Vector &p, int fan_number) const;
      virtual double AmplitudeOfSurface(const CLHEP::Hep3Vector& P, int side, int fan_number) const;
      /// @}

    private:

      double get_sagging(const CLHEP::Hep3Vector &P, int fan_number) const;
      void init_sagging_parameters();

      std::vector<std::vector<double> > m_sagging_parameter;
      std::string m_saggingOptions;

  };

}

#endif // __LArWheelCalculator_Impl_IDistanceCalculatorOn_H__
