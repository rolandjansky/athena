/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   IMultiStateExtrapolator.h
 * @date   Tuesday 25th January 2005
 * @author Anthony Morley, Christos Anastopoulos
 * Abstract base class for extrapolation of a MultiComponentState
 */


#ifndef TrkIMultiStateExtrapolator_H
#define TrkIMultiStateExtrapolator_H

#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"

#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventPrimitives/PropDirection.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/BoundaryCheck.h"

#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/IAlgTool.h"

#include <memory>
#include <vector>

namespace Trk {

class IPropagator;
class Surface;
class Track;
class TrackingVolume;
class TrackStateOnSurface;

static const InterfaceID IID_IMultiStateExtrapolator("IMultiStateExtrapolator",
                                                     1,
                                                     0);

class IMultiStateExtrapolator : virtual public IAlgTool
{
public:
  /** Virtual destructor */
  virtual ~IMultiStateExtrapolator() = default;

  /** AlgTool interface method */
  static const InterfaceID& interfaceID()
  {
    return IID_IMultiStateExtrapolator;
  };

  /** Configured AlgTool extrapolation method (1) */
  virtual MultiComponentState extrapolate(
    const EventContext& ctx,
    const MultiComponentState&,
    const Surface&,
    PropDirection direction = anyDirection,
    const BoundaryCheck& boundaryCheck = true,
    ParticleHypothesis particleHypothesis = nonInteracting) const = 0;

  /** Configured AlgTool extrapolation without material effects method (2) */
  virtual MultiComponentState extrapolateDirectly(
    const EventContext& ctx,
    const MultiComponentState&,
    const Surface&,
    PropDirection direction = anyDirection,
    const BoundaryCheck& boundaryCheck = true,
    ParticleHypothesis particleHypothesis = nonInteracting) const = 0;

  virtual std::unique_ptr<std::vector<const Trk::TrackStateOnSurface*>>
  extrapolateM(const EventContext& ctx,
               const MultiComponentState&,
               const Surface&,
               PropDirection dir = anyDirection,
               const BoundaryCheck& bcheck = true,
               ParticleHypothesis particle = nonInteracting) const = 0;
};

} // end trk namespace

#endif
