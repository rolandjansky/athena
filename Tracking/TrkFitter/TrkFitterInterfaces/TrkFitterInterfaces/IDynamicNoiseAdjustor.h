/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// IDynamicNoiseAdjustor.h
//   Header file for interface of DynamicNoiseAdjustor
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Vakhtang.Kartvelishvili at cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_IDYNAMICNOISEADJUSTOR_H
#define TRK_IDYNAMICNOISEADJUSTOR_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkFitterUtils/KalmanMatEffectsController.h"
#include "TrkParameters/TrackParameters.h"


namespace Trk {

  class DNA_MaterialEffects; //!< object managing the DNA output
  class MeasurementBase;     //!< Measured position/parameters from ROT
  class IExtrapolator;       //!< Extrapolation Tool interface
  class IUpdator;            //!< interface for Estimator updating

static const InterfaceID InterfaceID_IDynamicNoiseAdjustor ("IDynamicNoiseAdjustor", 1, 0);

  /** @class IDynamicNoiseAdjustor
      provides the interface to encapsulate the adjustment
      of momentum and error inside a Kalman Filter. This tool
      needs to use the same propagation and filtering tools
      as the top-level Kalman filter, therefore these tools
      are passed through the public configure() method.
    */

  class IDynamicNoiseAdjustor : virtual public IAlgTool {
public:

    /** Via the configure method track fitters set up the
        DNA tool to use the their instances of extrapolator
        and measurement updator */ 
    virtual StatusCode configureWithTools(IExtrapolator*, const IUpdator*) = 0;


    /** DNA_Adjust looks at the quality of the prediction (1st track
        par) with respect to the target measurement, and possibly
        re-extrapolates from the start (2nd track par) to calculate
        the noise adjustment. The predicted parameters (1st par)
        are modified. */
    virtual const DNA_MaterialEffects* DNA_Adjust(const TrackParameters*&,
                                                  const TrackParameters*&,
                                                  const MeasurementBase*,
                                                  const Trk::KalmanMatEffectsController&,
                                                  PropDirection,
                                                  const DNA_MaterialEffects* = nullptr) const = 0;

//    virtual double calculateDna(double, double, double) const = 0;


  };

} // end of namespace

#endif // TRK_DYNAMICNOISEADJUSTOR_H
