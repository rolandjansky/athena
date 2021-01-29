/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/IAlgTool.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkFitterUtils/KalmanMatEffectsController.h"
#include "TrkParameters/TrackParameters.h"

namespace Trk {

class DNA_MaterialEffects; //!< object managing the DNA output
class MeasurementBase;     //!< Measured position/parameters from ROT
class IExtrapolator;       //!< Extrapolation Tool interface
class IUpdator;            //!< interface for Estimator updating

static const InterfaceID
  InterfaceID_IDynamicNoiseAdjustor("IDynamicNoiseAdjustor", 1, 0);

/** @class IDynamicNoiseAdjustor
    provides the interface to encapsulate the adjustment
    of momentum and error inside a Kalman Filter. This tool
    needs to use the same propagation and filtering tools
    as the top-level Kalman filter, therefore these tools
    are passed through the public configure() method.
  */

class IDynamicNoiseAdjustor : virtual public IAlgTool
{
public:
  enum DnaStrategy
  {
    unidentifiedStrategy = 0,
    //!< regular successful DNA activity, no special treatment
    regularSuccessfulDNA = 1000,
    //!< special treatment for pixels, only in backward direction
    backwardPixelTreatment = 1500,
    //!< no good minimum, possible special treatment
    noGoodMinimum = 2000,
    //!< geometric cuts not satisfied. Mostly TRT, needs special treatment
    geomCutsNotSatisfied = 2003,
    //!< low chi2, no canonical DNA needed, but may need some special treatment
    undevelopedTrack = 2004
  };

  struct State
  {
    State()
      : eventContext(Gaudi::Hive::currentContext())
    {}
    State(const EventContext& ctx)
      : eventContext(ctx)
    {}
    ~State() = default;
    const EventContext& eventContext;

    DnaStrategy currentDnaStrategy = unidentifiedStrategy;
  };

  /** Via the configure method track fitters set up the
      DNA tool to use the their instances of extrapolator
      and measurement updator */
  virtual StatusCode configureWithTools(IExtrapolator*, const IUpdator*) = 0;

  /** DNA_Adjust looks at the quality of the prediction (1st track
      par) with respect to the target measurement, and possibly
      re-extrapolates from the start (2nd track par) to calculate
      the noise adjustment. The predicted parameters (1st par)
      are modified. */
  virtual const DNA_MaterialEffects* DNA_Adjust(
    State& state,
    const TrackParameters*& predPar,    // predicted into next layer
    const TrackParameters*& updatedPar, // previously updated
    const MeasurementBase* fittableMeasurement,
    const KalmanMatEffectsController& matEff,
    PropDirection direction,
    const DNA_MaterialEffects* forwardMEF = nullptr) const = 0;
};

} // end of namespace

#endif // TRK_DYNAMICNOISEADJUSTOR_H
