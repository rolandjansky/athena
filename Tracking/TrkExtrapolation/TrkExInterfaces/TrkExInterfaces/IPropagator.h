/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IPropagator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXINTERFACES_IPROPAGATOR_H
#define TRKEXINTERFACES_IPROPAGATOR_H

// Gaudi
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ThreadLocalContext.h"

// Trk
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkExInterfaces/HelperStructs.h"
#include "TrkExUtils/ExtrapolationCache.h"
#include "TrkExUtils/TargetSurfaces.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"
#include "TrkNeutralParameters/NeutralParameters.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/BoundaryCheck.h"
#include "TrkExUtils/IntersectionSolution.h" //typedef
// STL
#include <utility>

namespace Trk {

class Surface;
class CylinderBounds;
class MagneticFieldProperties;
class TransportJacobian;
class TrackStateOnSurface;

/** typedef for return type TrackParameters, pathlength */
typedef std::pair<const TrackParameters*, double> TrackParametersWithPath;

/** typedef for input surfaces, boundary check */
typedef std::pair<const Surface*, BoundaryCheck> DestSurf;

/** Interface ID for IPropagators*/
static const InterfaceID IID_IPropagator("IPropagator", 1, 0);

/** @class IPropagator
  Interface class IPropagators
  It inherits from IAlgTool.
  */
class IPropagator : virtual public IAlgTool
{
public:
  /**Virtual destructor*/
  virtual ~IPropagator() = default;

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID() { return IID_IPropagator; }

  /** Main propagation method for NeutralParameters */
  virtual std::unique_ptr<NeutralParameters> propagate(
    const NeutralParameters& parameters,
    const Surface& sf,
    PropDirection dir,
    const BoundaryCheck& bcheck,
    bool returnCurv = false) const = 0;

  /** Main propagation method without transport jacobian production*/
  virtual std::unique_ptr<TrackParameters> propagate(
    const EventContext& ctx,
    const TrackParameters& parm,
    const Surface& sf,
    PropDirection dir,
    const BoundaryCheck& bcheck,
    const MagneticFieldProperties& mprop,
    ParticleHypothesis particle = pion,
    bool returnCurv = false,
    const TrackingVolume* tVol = nullptr) const = 0;

  /** Propagate parameters and covariance with search of closest surface */
  virtual std::unique_ptr<TrackParameters> propagate(
    const EventContext& ctx,
    const TrackParameters& parm,
    std::vector<DestSurf>& sfs,
    PropDirection dir,
    const MagneticFieldProperties& mprop,
    ParticleHypothesis particle,
    std::vector<unsigned int>& solutions,
    double& path,
    bool usePathLim = false,
    bool returnCurv = false,
    const TrackingVolume* tVol = nullptr) const = 0;

  /** Propagate parameters and covariance with search of closest surface 
   * time included*/
  virtual std::unique_ptr<TrackParameters> propagateT(
    const EventContext& ctx,
    const TrackParameters& parm,
    std::vector<DestSurf>& sfs,
    PropDirection dir,
    const MagneticFieldProperties& mprop,
    ParticleHypothesis particle,
    std::vector<unsigned int>& solutions,
    PathLimit& pathLim,
    TimeLimit& timeLim,
    bool returnCurv,
    const TrackingVolume* tVol,
    std::vector<Trk::HitInfo>*& hitVector) const;

  /** Propagate parameters and covariance with search of closest surface and
   * material collection */
  virtual Trk::ExtrapolationCode propagate(
    const EventContext& ctx,
    Trk::ExCellCharged& eCell,
    Trk::TargetSurfaces& sfs,
    Trk::TargetSurfaceVector& solutions) const;
  
  /** Propagation interface:
    The propagation method with internal material collection. The propagator
    finds the closest surface.
    */
  virtual std::unique_ptr<TrackParameters> propagateM(
    const EventContext& ctx,
    const TrackParameters& parm,
    std::vector<DestSurf>& sfs,
    PropDirection dir,
    const MagneticFieldProperties& mprop,
    ParticleHypothesis particle,
    std::vector<unsigned int>& solutions,
    std::vector<const Trk::TrackStateOnSurface*>*& matstates,
    std::vector<std::pair<std::unique_ptr<Trk::TrackParameters>, int>>* intersections,
    double& path,
    bool usePathLim = false,
    bool returnCurv = false,
    const TrackingVolume* tVol = nullptr,
    Trk::ExtrapolationCache* cache = nullptr) const;

  /** Main propagation method with transport jacobian production*/
  virtual std::unique_ptr<TrackParameters> propagate(
    const EventContext& ctx,
    const TrackParameters& parm,
    const Surface& sf,
    PropDirection dir,
    const BoundaryCheck& bcheck,
    const MagneticFieldProperties& mprop,
    TransportJacobian*& jacob,
    double& pathLength,
    ParticleHypothesis particle = pion,
    bool returnCurv = false,
    const TrackingVolume* tVol = nullptr) const = 0;

  /** Main propagation method for parameters only. Without transport jacobian production*/
  virtual std::unique_ptr<TrackParameters> propagateParameters(
    const EventContext& ctx,
    const TrackParameters& parm,
    const Surface& sf,
    PropDirection dir,
    const BoundaryCheck& bcheck,
    const MagneticFieldProperties& mprop,
    ParticleHypothesis particle = pion,
    bool returnCurv = false,
    const TrackingVolume* tVol = nullptr) const = 0;

  /** Main propagation method for parameters only with transport jacobian production*/
  virtual std::unique_ptr<TrackParameters> propagateParameters(
    const EventContext& ctx,
    const TrackParameters& parm,
    const Surface& sf,
    PropDirection dir,
    const BoundaryCheck& bcheck,
    const MagneticFieldProperties& mprop,
    TransportJacobian*& jacob,
    ParticleHypothesis particle = pion,
    bool returnCurv = false,
    const TrackingVolume* tVol = nullptr) const = 0;

  /** Intersection interface:
     The intersection interface might be used by the material service as well to
     estimate the surfaces (sensitive and nonesensitive) while propagation
    */
  virtual const IntersectionSolution* intersect(
    const EventContext& ctx,
    const TrackParameters& parm,
    const Surface& sf,
    const MagneticFieldProperties& mprop,
    ParticleHypothesis particle = pion,
    const TrackingVolume* tVol = nullptr) const = 0;

  /** Intersection and Intersector interface:
   */
  virtual const TrackSurfaceIntersection* intersectSurface(
    const EventContext& ctx,
    const Surface& surface,
    const TrackSurfaceIntersection* trackIntersection,
    const double qOverP,
    const MagneticFieldProperties& mft,
    ParticleHypothesis particle) const;

  /** GlobalPositions list interface:
     This is used mostly in pattern recognition in the road finder, the
     propagation direction is intrinsically given by the sign of the stepSize.

     To avoid memory fragmentation in multiple use of pattern recognition
     processes and respecting the possible iterative filling of the positions
     list, the list of GlobalPositions is given by reference through the
     signature and a void method has been chosen.
     */
  virtual void globalPositions(const EventContext& ctx,
                               std::list<Amg::Vector3D>& positionslist,
                               const TrackParameters& parm,
                               const MagneticFieldProperties& mprop,
                               const CylinderBounds& cylbo,
                               double stepSize,
                               ParticleHypothesis particle = pion,
                               const TrackingVolume* tVol = nullptr) const = 0;

  /** Validation Action:
    Can be implemented optionally, outside access to internal validation steps
  */
  virtual void validationAction() const {}


};

} // end of namespace

#include "TrkExInterfaces/IPropagator.icc"

#endif // TRKEXINTERFACES_PROPAGATOR_H

