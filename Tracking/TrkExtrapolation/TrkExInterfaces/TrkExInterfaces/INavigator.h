/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// INavigator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRINTERFACES_INAVIGATOR_H
#define TRKDETDESCRINTERFACES_INAVIGATOR_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"
// GeoPrimitives
#include "GeoPrimitives/GeoPrimitives.h"
// Trk
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkVolumes/BoundarySurface.h"
#include "TrkVolumes/BoundarySurfaceFace.h"
// STL
#include <utility>

namespace Trk {

class IPropagator;
class Surface;
class Track;
// class TrackParameters;
class TrackingVolume;
class TrackingGeometry;
class DetachedTrackingVolume;

/**useful struct for a single navigation cell*/
struct NavigationCell
{

  const TrackingVolume* nextVolume;
  const TrackParameters* parametersOnBoundary;
  BoundarySurfaceFace exitFace;
  /** Constructor */
  NavigationCell(const TrackingVolume* nVol,
                 const TrackParameters* lPar,
                 BoundarySurfaceFace face = undefinedFace)
    : nextVolume(nVol)
    , parametersOnBoundary(lPar)
    , exitFace(face)
  {}
};

typedef std::pair<int, const NavigationCell*> IdNavigationCell;

/** Interface ID for INavigator */
static const InterfaceID IID_INavigator("INavigator", 1, 0);

/** @class INavigator
   Interface class for the navigation AlgTool, it inherits from IAlgTool
   Detailed information about private members and member functions can be
   found in the actual implementation class Trk::Navigator which inherits from
   this one.

   @author Andreas.Salzburger@cern.ch
  */
class INavigator : virtual public IAlgTool
{
public:
  /**Virtual destructor*/
  virtual ~INavigator() {}

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID() { return IID_INavigator; }

  /** INavigator interface method - returns the TrackingGeometry used for
   * navigation */
  virtual const TrackingGeometry* trackingGeometry(
    const EventContext& ctx) const = 0;

  /** INavigator interface method - global search for the Volume one is in */
  virtual const TrackingVolume* volume(const EventContext& ctx,
                                       const Amg::Vector3D& gp) const = 0;

  /** INavigator interface method - forward hightes TrackingVolume */
  virtual const TrackingVolume* highestVolume(
    const EventContext& ctx) const = 0;

  /** INavigator interface method - getting the closest TrackParameters from a
   * Track to a Surface */
  virtual const TrackParameters* closestParameters(
    const Track& trk,
    const Surface& sf,
    const IPropagator* prop = 0) const = 0;


  /** INavigator method to resolve navigation at boundary */
  virtual bool atVolumeBoundary(const Trk::TrackParameters* parms,
                                const Trk::TrackingVolume* vol,
                                Trk::PropDirection dir,
                                const Trk::TrackingVolume*& nextVol,
                                double tol) const = 0;

  /** Validation Action:
       Can be implemented optionally, outside access to internal validation
     steps */
  virtual void validationAction() const = 0;


  /** INavigator interface method - getting the next BoundarySurface not knowing
   * the Volume*/
  virtual const BoundarySurface<TrackingVolume>* nextBoundarySurface(
    const EventContext& ctx,
    const IPropagator& prop,
    const TrackParameters& parms,
    PropDirection dir) const = 0;

  /** INavigator interface method - getting the next BoundarySurface when
   * knowing the Volume*/
  virtual const BoundarySurface<TrackingVolume>* nextBoundarySurface(
    const EventContext& ctx,
    const IPropagator& prop,
    const TrackParameters& parms,
    PropDirection dir,
    const TrackingVolume& vol) const = 0;

  /** INavigator interface method - - getting the next Volume and the parameter
   * for the next Navigation */
  virtual NavigationCell nextTrackingVolume(
    const EventContext& ctx,
    const IPropagator& prop,
    const TrackParameters& parms,
    PropDirection dir,
    const TrackingVolume& vol) const = 0;

  /** INavigator interface method - getting the next Volume and the parameter
    for the next Navigation
    - contains full loop over volume boundaries
  */
  virtual NavigationCell nextDenseTrackingVolume(const EventContext& ctx,
                                                 const IPropagator& prop,
                                                 const TrackParameters& parms,
                                                 const Surface* destination,
                                                 PropDirection dir,
                                                 ParticleHypothesis particle,
                                                 const TrackingVolume& vol,
                                                 double& path) const = 0;

   /*
    * Methods without explicit Event Context. To be removed
    * when clients get migrated
    */

  /** INavigator interface method - returns the TrackingGeometry used for
   * navigation */
  virtual const TrackingGeometry* trackingGeometry() const;

  /** INavigator interface method - global search for the Volume one is in */
  virtual const TrackingVolume* volume(const Amg::Vector3D& gp) const;

  /** INavigator interface method - forward hightes TrackingVolume */
  virtual const TrackingVolume* highestVolume() const;

  /** INavigator interface method - getting the next BoundarySurface not knowing
   * the Volume*/
  virtual const BoundarySurface<TrackingVolume>* nextBoundarySurface(
    const IPropagator& prop,
    const TrackParameters& parms,
    PropDirection dir) const;

  /** INavigator interface method - getting the next BoundarySurface when
   * knowing the Volume*/
  virtual const BoundarySurface<TrackingVolume>* nextBoundarySurface(
    const IPropagator& prop,
    const TrackParameters& parms,
    PropDirection dir,
    const TrackingVolume& vol) const;

  /** INavigator interface method - - getting the next Volume and the parameter
   * for the next Navigation */
  virtual NavigationCell nextTrackingVolume(
    const IPropagator& prop,
    const TrackParameters& parms,
    PropDirection dir,
    const TrackingVolume& vol) const;

  /** INavigator interface method - getting the next Volume and the parameter
    for the next Navigation
    - contains full loop over volume boundaries
  */
  virtual NavigationCell nextDenseTrackingVolume(const IPropagator& prop,
                                                 const TrackParameters& parms,
                                                 const Surface* destination,
                                                 PropDirection dir,
                                                 ParticleHypothesis particle,
                                                 const TrackingVolume& vol,
                                                 double& path) const;

};

} // end of namespace
#include "TrkExInterfaces/INavigator.icc"

#endif // TRKDETDESCRINTERFACES_INAVIGATOR_H

