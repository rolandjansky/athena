/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */

/**
 * @file   GsfExtrapolator.h
 * @date   Tuesday 25th January 2005
 * @author Tom Athkinson, Anthony Morley, Christos Anastopoulos
 * Extrapolation of MultiComponentState class. It is an
 * AlgTool inheriting from the IMultiStateExtrapolator class
 */

#ifndef TrkGsfExtrapolator_H
#define TrkGsfExtrapolator_H

#include "TrkGaussianSumFilter/IMultiStateExtrapolator.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "Gaudi/Accumulators.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkExInterfaces/INavigator.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkGaussianSumFilter/IMaterialMixtureConvolution.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkParameters/TrackParameters.h"

#include <memory>
#include <string>
#include <vector>

namespace Trk {

class Layer;
class Surface;
class TrackingVolume;
class TrackingGeometry;
class TrackStateOnSurface;
class MaterialProperties;
/** @class GsfExtrapolator */

class GsfExtrapolator final
  : public AthAlgTool
  , virtual public IMultiStateExtrapolator
{

public:
  /** Constructor with AlgTool parameters */
  GsfExtrapolator(const std::string&, const std::string&, const IInterface*);

  /** Destructor */
  virtual ~GsfExtrapolator() override final;

  /** AlgTool initialise method */
  virtual StatusCode initialize() override final;

  /** AlgTool finalise method */
  virtual StatusCode finalize() override final;

  /** Configured AlgTool extrapolation method (1) */
  virtual MultiComponentState extrapolate(
    const EventContext& ctx,
    Cache&,
    const MultiComponentState&,
    const Surface&,
    PropDirection direction,
    const BoundaryCheck& boundaryCheck,
    ParticleHypothesis particleHypothesis =
      nonInteracting) const override final;

  /** Configured AlgTool extrapolation without material effects method (2) */
  virtual MultiComponentState extrapolateDirectly(
    const EventContext& ctx,
    const MultiComponentState&,
    const Surface&,
    PropDirection direction,
    const BoundaryCheck& boundaryCheck,
    ParticleHypothesis particleHypothesis =
      nonInteracting) const override final;

private:
  /** Implementation of main extrapolation method*/
  MultiComponentState extrapolateImpl(
    const EventContext& ctx,
    Cache& cache,
    const MultiComponentState&,
    const Surface&,
    PropDirection direction,
    const BoundaryCheck& boundaryCheck,
    ParticleHypothesis particleHypothesis) const;

  /** Implementation of extrapolation without material effects*/
  MultiComponentState extrapolateDirectlyImpl(
    const EventContext& ctx,
    const MultiComponentState&,
    const Surface&,
    PropDirection direction = anyDirection,
    const BoundaryCheck& boundaryCheck = true,
    ParticleHypothesis particleHypothesis = nonInteracting) const;

  /** Two primary private extrapolation methods
    - extrapolateToVolumeBoundary : extrapolates to the
    exit of the destination tracking volume
    The exit layer surface will be hit in this method.

    - extrapolateInsideVolume : extrapolates to the destination surface in
    the final tracking volume
    */
  void extrapolateToVolumeBoundary(const EventContext& ctx,
                                   Cache& cache,
                                   const MultiComponentState&,
                                   const Layer*,
                                   const TrackingVolume&,
                                   PropDirection direction,
                                   ParticleHypothesis particleHypothesis) const;

  MultiComponentState extrapolateInsideVolume(
    const EventContext& ctx,
    Cache& cache,
    const MultiComponentState&,
    const Surface&,
    const Layer*,
    const TrackingVolume&,
    PropDirection direction,
    const BoundaryCheck& boundaryCheck,
    ParticleHypothesis particleHypothesis) const;

  /** Additional private extrapolation methods */

  /** Layer stepping, stopping at the last layer before destination */
  MultiComponentState extrapolateFromLayerToLayer(
    const EventContext& ctx,
    Cache& cache,
    const MultiComponentState&,
    const TrackingVolume&,
    const Layer* startLayer,
    const Layer* destinationLayer,
    PropDirection direction,
    ParticleHypothesis particleHypothesis) const;

  /** Single extrapolation step to an intermediate layer */
  MultiComponentState extrapolateToIntermediateLayer(
    const EventContext& ctx,
    Cache& cache,
    const MultiComponentState&,
    const Layer&,
    const TrackingVolume&,
    PropDirection direction,
    ParticleHypothesis particleHypothesis,
    bool perpendicularCheck = true) const;

  /** Final extrapolation step to a destination layer */
  MultiComponentState extrapolateToDestinationLayer(
    const EventContext& ctx,
    Cache& cache,
    const MultiComponentState&,
    const Surface&,
    const Layer&,
    const Layer*,
    PropDirection direction,
    const BoundaryCheck& boundaryCheck,
    ParticleHypothesis particleHypothesis) const;

  /** Method to initialise navigation parameters including starting state, layer
   * and volume, and destination volume */
  void initialiseNavigation(
    const EventContext& ctx,
    Cache& cache,
    const MultiComponentState& initialState,
    const Surface& surface,
    const Layer*& associatedLayer,
    const TrackingVolume*& currentVolume,
    const TrackingVolume*& destinationVolume,
    std::unique_ptr<TrackParameters>& referenceParameters,
    PropDirection direction) const;

  ToolHandle<IPropagator> m_propagator{ this, "Propagator", "", "" };
  ToolHandle<INavigator> m_navigator{ this,
                                      "Navigator",
                                      "Trk::Navigator/Navigator",
                                      "" };
  ToolHandle<IMaterialMixtureConvolution> m_materialUpdator{
    this,
    "GsfMaterialConvolution",
    "Trk::GsfMaterialMixtureConvolution/GsfMaterialMixtureConvolution",
    ""
  };

  //!< Switch to turn on/off surface based material effects
  bool m_fastField;
  Trk::MagneticFieldProperties m_fieldProperties;
};

} // end namespace Trk

#endif
