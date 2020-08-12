/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


/**
 * @file   IForwardGsfFitter.h
 * @date   Thursday 8th January 2009
 * @author Anthony Morley, Christos Anastopoulos
 * @brief  Abstract interface for the forward GSF fitter
 */

#ifndef TrkIForwardGsfFitter_H
#define TrkIForwardGsfFitter_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkFitterUtils/FitterTypes.h"
#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"
#include "TrkGaussianSumFilter/IMultiStateExtrapolator.h"
#include "TrkParameters/TrackParameters.h"
#include "GaudiKernel/EventContext.h"

#include <memory>
namespace Trk {

class IMultiStateMeasurementUpdator;
class IRIO_OnTrackCreator;
class Surface;

static const InterfaceID InterfaceID_ForwardGsfFitter("ForwardGsfFitter", 1, 0);

class IForwardGsfFitter : virtual public IAlgTool
{

public:
  /** AlgTool interface method */
  static const InterfaceID& interfaceID()
  {
    return InterfaceID_ForwardGsfFitter;
  };

  /** Virtual destructor */
  virtual ~IForwardGsfFitter() = default;

  /** Configure the forward GSF fitter
      - Configure the extrapolator
      - Configure the measurement updator
      - Configure the RIO_OnTrack creator */
  virtual StatusCode configureTools(
    const ToolHandle<Trk::IMultiStateExtrapolator>&,
    const ToolHandle<Trk::IRIO_OnTrackCreator>&) = 0;

  /** Forward GSF fit using PrepRawData */
  virtual std::unique_ptr<ForwardTrajectory> fitPRD(
    const EventContext& ctx,
    Trk::IMultiStateExtrapolator::Cache&,
    const PrepRawDataSet&,
    const TrackParameters&,
    const ParticleHypothesis particleHypothesis = nonInteracting) const = 0;

  /** Forward GSF fit using MeasurementSet */
  virtual std::unique_ptr<ForwardTrajectory> fitMeasurements(
    const EventContext& ctx,
    Trk::IMultiStateExtrapolator::Cache&,
    const MeasurementSet&,
    const TrackParameters&,
    const ParticleHypothesis particleHypothesis = nonInteracting) const = 0;

  /** The interface will later be extended so that the initial state can be
   * additionally a MultiComponentState object!
   */
};

} // end Trk namespace

#endif
