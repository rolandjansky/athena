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

#include <Gaudi/Accumulators.h>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkExInterfaces/IEnergyLossUpdator.h"
#include "TrkExInterfaces/IMultipleScatteringUpdator.h"
#include "TrkExInterfaces/INavigator.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkGaussianSumFilter/IMaterialMixtureConvolution.h"
#include "TrkGaussianSumFilter/IMultiStateExtrapolator.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkParameters/TrackParameters.h"

#include "GeoPrimitives/GeoPrimitives.h"

#include "TrkGeometry/MagneticFieldProperties.h"
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
    PropDirection direction = anyDirection,
    const BoundaryCheck& boundaryCheck = true,
    ParticleHypothesis particleHypothesis =
      nonInteracting) const override final;

  /** Configured AlgTool extrapolation without material effects method (2) */
  virtual MultiComponentState extrapolateDirectly(
    const EventContext& ctx,
    const MultiComponentState&,
    const Surface&,
    PropDirection direction = anyDirection,
    const BoundaryCheck& boundaryCheck = true,
    ParticleHypothesis particleHypothesis =
      nonInteracting) const override final;

  virtual std::unique_ptr<std::vector<const Trk::TrackStateOnSurface*>>
  extrapolateM(const EventContext& ctx,
               const MultiComponentState&,
               const Surface& sf,
               PropDirection dir,
               const BoundaryCheck& bcheck,
               ParticleHypothesis particle) const override final;

private:
  /** These are the methods that do the actual heavy lifting when
   * extrapolating*/
  MultiComponentState extrapolateImpl(
    const EventContext& ctx,
    Cache& cache,
    const IPropagator&,
    const MultiComponentState&,
    const Surface&,
    PropDirection direction = anyDirection,
    const BoundaryCheck& boundaryCheck = true,
    ParticleHypothesis particleHypothesis = nonInteracting) const;

  /** Configured AlgTool extrapolation without material effects method (2) */
  MultiComponentState extrapolateDirectlyImpl(
    const EventContext& ctx,
    const IPropagator&,
    const MultiComponentState&,
    const Surface&,
    PropDirection direction = anyDirection,
    const BoundaryCheck& boundaryCheck = true,
    ParticleHypothesis particleHypothesis = nonInteracting) const;

  /** Two primary private extrapolation methods
    - extrapolateToVolumeBoundary - extrapolates to the exit of the destination
    tracking volume
    - Exit layer surface will be hit in this method.
    - extrapolateInsideVolume     - extrapolates to the destination surface in
    the final tracking volume
    */
  void extrapolateToVolumeBoundary(
    const EventContext& ctx,
    Cache& cache,
    const IPropagator&,
    const MultiComponentState&,
    const Layer*,
    const TrackingVolume&,
    PropDirection direction = anyDirection,
    ParticleHypothesis particleHypothesis = nonInteracting) const;

  MultiComponentState extrapolateInsideVolume(
    const EventContext& ctx,
    Cache& cache,
    const IPropagator&,
    const MultiComponentState&,
    const Surface&,
    const Layer*,
    const TrackingVolume&,
    PropDirection direction = anyDirection,
    const BoundaryCheck& boundaryCheck = true,
    ParticleHypothesis particleHypothesis = nonInteracting) const;

  /** Additional private extrapolation methods */

  /** Layer stepping, stopping at the last layer before destination */
  MultiComponentState extrapolateFromLayerToLayer(
    const EventContext& ctx,
    Cache& cache,
    const IPropagator&,
    const MultiComponentState&,
    const TrackingVolume&,
    const Layer* startLayer,
    const Layer* destinationLayer = nullptr,
    PropDirection direction = anyDirection,
    ParticleHypothesis particleHypothesis = nonInteracting) const;

  /** Single extrapolation step to an intermediate layer */
  MultiComponentState extrapolateToIntermediateLayer(
    const EventContext& ctx,
    Cache& cache,
    const IPropagator&,
    const MultiComponentState&,
    const Layer&,
    const TrackingVolume&,
    PropDirection direction = anyDirection,
    ParticleHypothesis particleHypothesis = nonInteracting,
    bool perpendicularCheck = true) const;

  /** Final extrapolation step to a destination layer */
  MultiComponentState extrapolateToDestinationLayer(
    const EventContext& ctx,
    Cache& cache,
    const IPropagator&,
    const MultiComponentState&,
    const Surface&,
    const Layer&,
    const Layer*,
    PropDirection direction = anyDirection,
    const BoundaryCheck& boundaryCheck = true,
    ParticleHypothesis particleHypothesis = nonInteracting) const;

  /** Method to initialise navigation parameters including starting state, layer
   * and volume, and destination volume */
  void initialiseNavigation(
    const EventContext& ctx,
    Cache& cache,
    const IPropagator& propagator,
    const MultiComponentState& initialState,
    const Surface& surface,
    const Layer*& associatedLayer,
    const TrackingVolume*& currentVolume,
    const TrackingVolume*& destinationVolume,
    std::unique_ptr<TrackParameters>& referenceParameters,
    PropDirection direction) const;

  /** Add material to vector*/
  void addMaterialtoVector(
    Cache& cache,
    const Trk::Layer* nextLayer,
    const Trk::TrackParameters* nextPar,
    PropDirection direction = anyDirection,
    ParticleHypothesis particleHypothesis = nonInteracting) const;

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
  ToolHandle<IMultipleScatteringUpdator> m_msupdators{
    this,
    "MultipleScatteringUpdator",
    "Trk::MultipleScatteringUpdator/AtlasMultipleScatteringUpdator",
    ""
  };
  ToolHandle<IEnergyLossUpdator> m_elossupdators{
    this,
    "EnergyLossUpdator",
    "Trk::EnergyLossUpdator/AtlasEnergyLossUpdator",
    ""
  };

  //!< Switch to turn on/off surface based material effects
  bool m_surfaceBasedMaterialEffects;
  bool m_fastField;
  Trk::MagneticFieldProperties m_fieldProperties;
};

} // end namespace Trk

#endif
