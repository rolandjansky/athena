/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IExtrapolator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXINTERFACES_IEXTRAPOLATOR_H
#define TRKEXINTERFACES_IEXTRAPOLATOR_H

// Gaudi
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ThreadLocalContext.h"

// Trk
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkExUtils/ExtrapolationCache.h"
#include "TrkExUtils/MaterialUpdateMode.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkNeutralParameters/NeutralParameters.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkParametersBase/ParametersBase.h"
#include "TrkSurfaces/BoundaryCheck.h"

// xAOD
#include "xAODTracking/NeutralParticle.h"
#include "xAODTracking/TrackParticle.h"
// STL
#include <memory>
#include <utility>

namespace Trk {

static const InterfaceID IID_IExtrapolator("IExtrapolator", 1, 0);

/**
 @class IExtrapolator

 Interface class for the extrapolation AlgTool, it inherits from IAlgTool
 Detailed information about private members and member functions can be found in
 the actual implementation class Extrapolator which inherits from this one.

 The ownership of the returned parameters is passed to caller.

 @author Andreas.Salzburger@cern.ch
 @author Christos Anastopoulos (Athena MT)
 */

class IPropagator;
class INavigator;
class IMaterialUpdater;
class Track;
class Surface;
class TrackingVolume;
class TrackingGeometry;
class TransportJacobian;
class TrackStateOnSurface;
class Layer;
class Volume;

class IExtrapolator : virtual public IAlgTool
{
public:
  /** Virtual destructor */
  virtual ~IExtrapolator() {}

  /** AlgTool interface methods */
  static const InterfaceID& interfaceID() { return IID_IExtrapolator; }

  /** Extrapolate Neutral xAOD particle to surface.
   * Starts from the perigee parameters. */
  virtual std::unique_ptr<NeutralParameters> extrapolate(
    const xAOD::NeutralParticle& xnParticle,
    const Surface& sf,
    PropDirection dir = anyDirection,
    const BoundaryCheck& bcheck = true) const = 0;

  /** Main extrapolation Interface starting from neutral parameters 
   * and aiming at surface.*/
  virtual std::unique_ptr<NeutralParameters> extrapolate(
    const NeutralParameters& parameters,
    const Surface& sf,
    PropDirection dir = anyDirection,
    const BoundaryCheck& bcheck = true) const = 0;

  /** Extrapolate Charged xAOD particle to surface
   * Starts from the perigee parameters.*/
  virtual std::unique_ptr<TrackParameters> extrapolate(
    const EventContext& ctx,
    const xAOD::TrackParticle& particleBase,
    const Surface& sf,
    PropDirection dir = anyDirection,
    const BoundaryCheck& bcheck = true,
    ParticleHypothesis particle = pion,
    MaterialUpdateMode matupmode = addNoise) const = 0;

  /** Main extrapolation interface starting from charged parameters and aiming
   * at Surface*/
  virtual std::unique_ptr<TrackParameters> extrapolate(
    const EventContext& ctx,
    const TrackParameters& parm,
    const Surface& sf,
    PropDirection dir = anyDirection,
    const BoundaryCheck& bcheck = true,
    ParticleHypothesis particle = pion,
    MaterialUpdateMode matupmode = addNoise,
    Trk::ExtrapolationCache* cache = nullptr) const = 0;

  /** Extrapolation method where a step-wise navigation to the
    destination surface is performed. Returns a vector
    parameters on all detector elements. Used mainly by the 
    hole-search*/
  virtual std::vector<std::unique_ptr<TrackParameters>>
  extrapolateStepwise(const EventContext& ctx,
                      const TrackParameters& parm,
                      const Surface& sf,
                      PropDirection dir = anyDirection,
                      const BoundaryCheck& bcheck = true,
                      ParticleHypothesis particle = pion) const = 0;
  
  /** Main extrapolation interface starting from a Trk::Track and aiming
   * at Surface. It uses the navigator to find the closest parameters
   * of the track to the surface. */
  virtual std::unique_ptr<TrackParameters> extrapolate(
    const EventContext& ctx,
    const Track& trk,
    const Surface& sf,
    PropDirection dir = anyDirection,
    const BoundaryCheck& bcheck = true,
    ParticleHypothesis particle = pion,
    MaterialUpdateMode matupmode = addNoise,
    Trk::ExtrapolationCache* cache = nullptr) const = 0;

  /** Extrapolate directly: Forwards directly the call to the 
   * configured "Global" propagator. No navigation and no 
   * material effecs. Useful when we need fast propagation
   * without these. */
  virtual std::unique_ptr<TrackParameters> extrapolateDirectly(
    const EventContext& ctx,
    const TrackParameters& parm,
    const Surface& sf,
    PropDirection dir = anyDirection,
    const BoundaryCheck& bcheck = true,
    ParticleHypothesis particle = pion) const = 0;


  /** extrapolateBlindly like step-wise extrapolation, but without
   * a destination surface.  Blind inside the given tracking Volume (boundaryVol),
   * if none is given the reference surface for destination is used
   */
  virtual std::vector<std::unique_ptr<TrackParameters>>
  extrapolateBlindly(const EventContext& ctx,
                     const TrackParameters& parm,
                     PropDirection dir = anyDirection,
                     const BoundaryCheck& bcheck = true,
                     ParticleHypothesis particle = pion,
                     const Volume* boundaryVol = nullptr) const = 0;

  /** Extrapolation to the next active layer with material collection*/
  virtual std::pair<std::unique_ptr<TrackParameters>, const Trk::Layer*>
  extrapolateToNextActiveLayerM(
    const EventContext& ctx,
    const TrackParameters& parm,
    PropDirection dir,
    const BoundaryCheck& bcheck,
    std::vector<const Trk::TrackStateOnSurface*>& material,
    ParticleHypothesis particle = pion,
    MaterialUpdateMode matupmode = addNoise) const = 0;

  /** Extrapolation to volume :*/
  virtual std::unique_ptr<TrackParameters> extrapolateToVolume(
    const EventContext& ctx,
    const TrackParameters& parm,
    const Trk::TrackingVolume& vol,
    PropDirection dir = anyDirection,
    ParticleHypothesis particle = pion) const = 0;

  /** Extrapolate to a destination surface,
   * while collecting all the material
   * layers in between. Useful for chi2 based tracking.
   */
  virtual std::vector<const TrackStateOnSurface*>* extrapolateM(
    const EventContext& ctx,
    const TrackParameters& parameters,
    const Surface& sf,
    PropDirection dir,
    const BoundaryCheck& bcheck,
    ParticleHypothesis particle = pion,
    Trk::ExtrapolationCache* cache = nullptr) const = 0;

  /** Extrapolation method collecting intersections with subdetector
   * boundaries and active volumes/layers. Destination
   * (subdetector boundary) : geoID (+ entry, -exit) ( default MS exit )
   * Employs the STEP propagator, used in the ParticleCaloExtension
   * mainly for muons and Particle Flow.
   */
  virtual std::unique_ptr<
    std::vector<std::pair<std::unique_ptr<Trk::TrackParameters>, int>>>
  extrapolate(const EventContext& ctx,
              const Trk::TrackParameters& parm,
              Trk::PropDirection dir,
              Trk::ParticleHypothesis particle,
              std::vector<const Trk::TrackStateOnSurface*>*& material,
              int destination = 3) const = 0;

  /** Return the TrackingGeometry used by the Extrapolator (forwards information
   * from Navigator) */
  virtual const TrackingGeometry* trackingGeometry() const = 0;

  /** Validation Action*/
  virtual void validationAction() const = 0;

  /** Access the subPropagator to the given volume*/
  virtual const IPropagator* subPropagator(
    const TrackingVolume& tvol) const = 0;
};
} // end of namespace

#endif // TRKEXINTERFACES_ITRKEXTRAPOLATOR_H

