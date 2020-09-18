/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Navigator.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
// Trk inlcudes
#include "TrkExTools/Navigator.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkEventUtils/TrkParametersComparisonFunction.h"
#include "TrkExUtils/IntersectionSolution.h"
#include "TrkExUtils/RungeKuttaUtils.h"
#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"
#include "TrkDetDescrInterfaces/IGeometryBuilder.h"
#include "TrkDetDescrUtils/ObjectAccessor.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkVolumes/BoundarySurface.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"
// Data Model
#include "AthContainers/DataVector.h"
// Amg
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

namespace{
const Trk::MagneticFieldProperties s_zeroMagneticField(Trk::NoField);
}

// constructor
Trk::Navigator::Navigator(const std::string &t, const std::string &n, const IInterface *p) :
  AthAlgTool(t, n, p),
  m_trackingGeometrySvc("AtlasTrackingGeometrySvc", n),
  m_trackingGeometryName("AtlasTrackingGeometry"),
  m_insideVolumeTolerance(1. * Gaudi::Units::mm),
  m_isOnSurfaceTolerance(0.005 * Gaudi::Units::mm),
  m_useStraightLineApproximation(false),
  m_searchWithDistance(true),
  m_fastField(false),
  m_validationMode(false),
  m_forwardCalls{},
  m_forwardFirstBoundSwitch{},
  m_forwardSecondBoundSwitch{},
  m_forwardThirdBoundSwitch{},
  m_backwardCalls{},
  m_backwardFirstBoundSwitch{},
  m_backwardSecondBoundSwitch{},
  m_backwardThirdBoundSwitch{},
  m_outsideVolumeCase{},
  m_sucessfulBackPropagation{}
  {
  declareInterface<INavigator>(this);
  // steering of algorithms
  declareProperty("TrackingGeometrySvc", m_trackingGeometrySvc);
  declareProperty("InsideVolumeTolerance", m_insideVolumeTolerance);
  declareProperty("IsOnSurfaceTolerance", m_isOnSurfaceTolerance);
  declareProperty("UseStraightLineApproximation", m_useStraightLineApproximation);
  // closest parameter search with new Surface::distance method
  declareProperty("SearchWithDistanceToSurface", m_searchWithDistance);
  // Magnetic field properties
  declareProperty("MagneticFieldProperties", m_fastField);
  declareProperty("ValidationMode", m_validationMode);
  }

// destructor
Trk::Navigator::~Navigator() {
}

// Athena standard methods
// initialize
StatusCode
Trk::Navigator::initialize() {


  //We can use conditions when the key is not empty
  m_useConditions=!m_trackingGeometryReadKey.key().empty();

  // get the TrackingGeometrySvc
  if (!m_useConditions) {
    if (m_trackingGeometrySvc.retrieve().isSuccess()) {
      ATH_MSG_DEBUG("Successfully retrieved " << m_trackingGeometrySvc);
      m_trackingGeometryName = m_trackingGeometrySvc->trackingGeometryName();
    } else {
      ATH_MSG_WARNING("Couldn't retrieve " << m_trackingGeometrySvc << ". ");
      ATH_MSG_WARNING(" -> Trying to retrieve default '"
                      << m_trackingGeometryName << "' from DetectorStore.");
    }
  }

  ATH_CHECK(m_trackingGeometryReadKey.initialize(m_useConditions));

  m_fieldProperties = m_fastField
                        ? Trk::MagneticFieldProperties(Trk::FastField)
                        : Trk::MagneticFieldProperties(Trk::FullField);

  //This is no-op for the Navigator only relevant for
  //derivated Validation for now
  validationInitialize();
  return StatusCode::SUCCESS;
}

const Trk::TrackingVolume*
Trk::Navigator::volume(const EventContext& ctx, const Amg::Vector3D& gp) const
{
  return (trackingGeometry(ctx)->lowestTrackingVolume(gp));
}

const Trk::TrackingVolume*
Trk::Navigator::highestVolume(const EventContext& ctx) const
{
  return (trackingGeometry(ctx)->highestTrackingVolume());
}

const Trk::BoundarySurface<Trk::TrackingVolume>*
Trk::Navigator::nextBoundarySurface(const EventContext& ctx,
                                    const Trk::IPropagator& prop,
                                    const Trk::TrackParameters& parms,
                                    Trk::PropDirection dir) const
{
  const Trk::TrackingVolume* trackingVolume = volume(parms.position());

  if (trackingVolume) {
    return (nextBoundarySurface(ctx,prop, parms, dir, *trackingVolume));
  }
  return nullptr;
}

const Trk::BoundarySurface<Trk::TrackingVolume>*
Trk::Navigator::nextBoundarySurface(const EventContext& ctx,
                                    const Trk::IPropagator& prop,
                                    const Trk::TrackParameters& parms,
                                    Trk::PropDirection dir,
                                    const Trk::TrackingVolume& vol) const
{
  // get the surface accessor
  Trk::ObjectAccessor surfAcc = vol.boundarySurfaceAccessor(
    parms.position(), dir * parms.momentum().normalized());
  // initialize the currentBoundary surface
  const Trk::BoundarySurface<Trk::TrackingVolume>* currentBoundary = nullptr;
  bool outsideVolume = surfAcc.inverseRetrieval();
  // attempt counter
  int tryBoundary = 0;

  // set the prop direction according to inverseRetrieval result
  Trk::PropDirection searchDir = dir;
  if (outsideVolume) {
    searchDir =
      (dir == Trk::alongMomentum) ? Trk::oppositeMomentum : Trk::alongMomentum;
  }

  // debug version
  ATH_MSG_VERBOSE("g  [N] Starting parameters are :" << parms);

  // loop over the the boundary surfaces according to the accessor type
  for (const Trk::ObjectAccessor::value_type& surface_id : surfAcc) {
    ++tryBoundary;
    // ----------------- output to screen if outputLevel() says so --------
    ATH_MSG_VERBOSE("  [N] " << tryBoundary << ". try - BoundarySurface "
                             << surface_id << " of Volume: '"
                             << vol.volumeName() << "'.");
    // get the boundary Surface according to the surfaceAccessor
    currentBoundary = vol.boundarySurface(surface_id);
    const Trk::Surface& currentSurface =
      currentBoundary->surfaceRepresentation();

    const Trk::TrackParameters* trackPar = nullptr;
    // do either RungeKutta (always after first unsuccessful try) or straight
    // line
    trackPar =
      (!m_useStraightLineApproximation || tryBoundary > 1)
        ? prop.propagateParameters(
            ctx, parms, currentSurface, searchDir, true, m_fieldProperties)
        : prop.propagateParameters(
            ctx, parms, currentSurface, searchDir, true, s_zeroMagneticField);

    if (trackPar) {
      ATH_MSG_VERBOSE(
        "  [N] --> next BoundarySurface found with Parameters: " << *trackPar);
      validationFill(trackPar);
      delete trackPar;
      return currentBoundary;
    }
  }

  // check the insideStatus
  bool insideVolume = vol.inside(parms.position());
  if (!insideVolume) {
    ++m_outsideVolumeCase;
  }

  return nullptr;
}

Trk::NavigationCell
Trk::Navigator::nextTrackingVolume(const EventContext& ctx,
                                   const Trk::IPropagator& prop,
                                   const Trk::TrackParameters& parms,
                                   Trk::PropDirection dir,
                                   const Trk::TrackingVolume& vol) const
{

  // ---------------------------------------------------
  if (dir == Trk::alongMomentum) {
    ++m_forwardCalls;
  } else {
    ++m_backwardCalls;
  }

  bool first = false;
  bool second = false;

  // ---------------------------------------------------
  // get the object accessor from the Volume
  Trk::ObjectAccessor surfAcc = vol.boundarySurfaceAccessor(
    parms.position(), dir * parms.momentum().normalized());
  // the object accessor already solved the outside question
  bool outsideVolume = surfAcc.inverseRetrieval();
  // initialize the boundary pointer / tracking volume pointer
  const Trk::BoundarySurface<Trk::TrackingVolume>* currentBoundary = nullptr;
  const Trk::TrackingVolume* nextVolume = nullptr;

  // debug version
  ATH_MSG_VERBOSE("  [N] Starting parameters are : " << parms);
  ATH_MSG_VERBOSE("  [N] This corresponds to [r,z] = [ "
                  << parms.position().perp() << ", " << parms.position().z()
                  << "]");
  ATH_MSG_VERBOSE("  [N] Boundary Surface accessor : " << surfAcc);

  // set the prop direction according to inverseRetrieval result
  Trk::PropDirection searchDir = dir;
  if (outsideVolume) {
    ATH_MSG_VERBOSE("  [N] Parameters have been flagged as being outside !");
    searchDir =
      (dir == Trk::alongMomentum) ? Trk::oppositeMomentum : Trk::alongMomentum;
  }

  // loop over boundary surfaces
  int tryBoundary = 0;

  /* local counted to increment in the loop*/
  auto forwardFirstBoundSwitch = m_forwardFirstBoundSwitch.buffer();
  auto forwardSecondBoundSwitch = m_forwardSecondBoundSwitch.buffer();
  auto forwardThirdBoundSwitch = m_forwardThirdBoundSwitch.buffer();
  auto backwardFirstBoundSwitch = m_backwardFirstBoundSwitch.buffer();
  auto backwardSecondBoundSwitch = m_backwardSecondBoundSwitch.buffer();
  auto backwardThirdBoundSwitch = m_backwardThirdBoundSwitch.buffer();

  for (const Trk::ObjectAccessor::value_type& surface_id : surfAcc) {
    ++tryBoundary;
    // get the boundary surface associated to the surfaceAccessor
    currentBoundary = vol.boundarySurface(surface_id);

    // ----------------- output to screen if outputLevel() says so --------
    if (!currentBoundary) {
      ATH_MSG_WARNING("  [N] " << tryBoundary << ". try - BoundarySurface "
                               << surface_id << " of Volume: '"
                               << vol.volumeName() << "' NOT FOUND.");
      continue;
    }
      ATH_MSG_VERBOSE("  [N] " << tryBoundary << ". try - BoundarySurface "
                               << surface_id << " of Volume: '"
                               << vol.volumeName() << "'.");


    const Trk::Surface& currentSurface =
      currentBoundary->surfaceRepresentation();
    // try the propagation
    const Trk::TrackParameters* trackPar = nullptr;
    // do either RungeKutta (always after first unsuccessful try) or straight
    // line
    if (!currentSurface.isOnSurface(parms.position(), true, 0., 0.)) {
      trackPar =
        (!m_useStraightLineApproximation || tryBoundary > 1)
          ? prop.propagateParameters(
              ctx, parms, currentSurface, searchDir, true, m_fieldProperties)
          : prop.propagateParameters(
              ctx, parms, currentSurface, searchDir, true, s_zeroMagneticField);
    } else {
      trackPar = parms.clone();
    }
    if (trackPar) {
      // the next volume pointer
      nextVolume = currentBoundary->attachedVolume(
        trackPar->position(), trackPar->momentum().normalized(), dir);
      // ----------------- output to screen if outputLevel() says so --------
      if (msgLvl(MSG::VERBOSE)) {
        ATH_MSG_VERBOSE("  [N] --> next BoundarySurface found with Parameters: "
                        << *trackPar);
        ATH_MSG_VERBOSE("  [N] This corresponds to [r,z] = [ "
                        << trackPar->position().perp() << ", "
                        << trackPar->position().z() << "]");

        // log of the boundary surface
        currentBoundary->debugInfo(msg(MSG::VERBOSE));
        ATH_MSG_VERBOSE("[N] --> Quering the BoundarySurface for the "
                        "associated TrackingVolume: ");
        ATH_MSG_VERBOSE(
          '\t' << '\t' << (nextVolume ? nextVolume->volumeName() : "None"));
      }

      validationFill(trackPar);
      return Trk::NavigationCell(
        nextVolume, trackPar, Trk::BoundarySurfaceFace(surface_id));
    }

    // ---------------------------------------------------
    if (!first && searchDir == Trk::alongMomentum) {
      ++forwardFirstBoundSwitch;
      first = true;
    } else if (!second && searchDir == Trk::alongMomentum) {
      ++forwardSecondBoundSwitch;
      second = true;
    } else if (searchDir == Trk::alongMomentum) {
      ++forwardThirdBoundSwitch;
    } else if (!first && searchDir == Trk::oppositeMomentum) {
      ++backwardFirstBoundSwitch;
      first = true;
    } else if (!second && searchDir == Trk::oppositeMomentum) {
      ++backwardSecondBoundSwitch;
      second = true;
    } else if (searchDir == Trk::oppositeMomentum) {
      ++backwardThirdBoundSwitch;
    }
    // ---------------------------------------------------
  }
  // return what you have : no idea
  return Trk::NavigationCell(nullptr, nullptr);
}

Trk::NavigationCell
Trk::Navigator::nextDenseTrackingVolume(
  const EventContext& ctx,
  const Trk::IPropagator &prop,
  const Trk::TrackParameters &parms,
  const Trk::Surface *destinationSurface,
  Trk::PropDirection dir,
  Trk::ParticleHypothesis particle,
  const Trk::TrackingVolume &vol,
  double &path) const {
  ATH_MSG_DEBUG("  [N] nextDenseTrackingVolume() to volume '"
                << vol.volumeName() << "', starting from " << parms.position());


  Trk::NavigationCell solution(nullptr, nullptr);
  const Trk::TrackParameters *currPar = &parms;

  double tol = 0.001;
  path = 0.;

  // check position : if volume exit return trivial solution
  const Trk::TrackingVolume *nextVolume = nullptr;
  if (atVolumeBoundary(currPar, &vol, dir, nextVolume, tol) && nextVolume != (&vol)) {
    if (!nextVolume) {
      const Amg::Vector3D& gp = currPar->position();
      const Trk::TrackingVolume *currStatic = trackingGeometry(ctx)->lowestStaticTrackingVolume(gp);
      if (&vol != currStatic) {
        nextVolume = currStatic;
      }
    }
    return Trk::NavigationCell(nextVolume, currPar);
  }

  // prepare vector of boundary surfaces + destination surface
  std::vector<std::pair<const Trk::Surface *, Trk::BoundaryCheck> > *surfaces =
    new std::vector<std::pair<const Trk::Surface *, Trk::BoundaryCheck> >;

  if (destinationSurface) {
    surfaces->push_back(std::pair<const Trk::Surface *, Trk::BoundaryCheck>(destinationSurface, false));
  }

  const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > > &bounds = vol.boundarySurfaces();
  for (unsigned int ib = 0; ib < bounds.size(); ib++) {
    const Trk::Surface *nextSurface = &((bounds[ib].get())->surfaceRepresentation());
    surfaces->push_back(std::pair<const Trk::Surface *, Trk::BoundaryCheck>(nextSurface, true));
  }

  // propagate
  std::vector<unsigned int> solutions;
  // const Trk::TrackParameters* nextPar = prop.propagate(parms,*surfaces,dir,vol,particle,solutions,path);
  const Trk::TrackParameters* nextPar = prop.propagate(ctx,
                                                       parms,
                                                       *surfaces,
                                                       dir,
                                                       m_fieldProperties,
                                                       particle,
                                                       solutions,
                                                       path,
                                                       false,
                                                       false,
                                                       &vol);
  // if (nextPar) throwIntoGarbageBin(nextPar);
  if (nextPar) {
    Amg::Vector3D gp = nextPar->position();
    if (destinationSurface && destinationSurface == &nextPar->associatedSurface()) {
      solution = Trk::NavigationCell(&vol, nextPar);
    } else if (atVolumeBoundary(nextPar, &vol, dir, nextVolume, tol)) {
      if (!nextVolume) {
        // detached volume boundary or world boundary : resolve
        const Trk::TrackingVolume *currStatic = trackingGeometry(ctx)->lowestStaticTrackingVolume(gp);
        if (&vol != currStatic) {
          nextVolume = currStatic;
        }
      }
      solution = Trk::NavigationCell(nextVolume, nextPar);
    }
  }
  surfaces->clear();
  delete surfaces;

  return solution;
}

bool
Trk::Navigator::atVolumeBoundary(const Trk::TrackParameters* parms,
                                 const Trk::TrackingVolume* vol,
                                 Trk::PropDirection dir,
                                 const Trk::TrackingVolume*& nextVol,
                                 double tol) const
{
  bool isAtBoundary = false;

  nextVol = nullptr;
  if (!vol) {
    return isAtBoundary;
  }
  const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > > &bounds = vol->boundarySurfaces();
  for (unsigned int ib = 0; ib < bounds.size(); ib++) {
    const Trk::Surface &surf = (bounds[ib].get())->surfaceRepresentation();
    if (surf.isOnSurface(parms->position(), true, tol, tol)) {
      // isAtBoundary = true;
      // const Trk::TrackingVolume* attachedVol =
      //  (bounds[ib].get())->attachedVolume(parms->position(),parms->momentum(),dir);
      // if (!nextVol && attachedVol ) nextVol = attachedVol;

      // sanity check to enforce the desired tolerance
      Trk::DistanceSolution distSol = surf.straightLineDistanceEstimate(parms->position(),
                                                                        dir * parms->momentum().unit());
      if (distSol.currentDistance(false) < tol && distSol.numberOfSolutions() > 0) {
        isAtBoundary = true;
        const Trk::TrackingVolume* attachedVol =
          (bounds[ib].get())
            ->attachedVolume(parms->position(), parms->momentum(), dir);
        if (!nextVol && attachedVol) {
          nextVol = attachedVol;
        }
        // double good solution indicate tangential intersection : revert the attached volumes
        if (distSol.numberOfSolutions() > 1 && fabs(distSol.first()) < tol && fabs(distSol.second()) < tol) {
          ATH_MSG_WARNING("navigator detects tangential intersection: switch of volumes reverted ");
          if (nextVol) {
            ATH_MSG_WARNING(vol->volumeName() << "->" << nextVol->volumeName() << "->" << vol->volumeName());
          }
          isAtBoundary = false;
          // revert attached volume
          nextVol = vol;
        }
      }
    }
  }

  return isAtBoundary;
}

const Trk::TrackParameters*
Trk::Navigator::closestParameters(const Trk::Track& trk,
                                  const Trk::Surface& sf,
                                  const Trk::IPropagator* propptr) const
{

  // -- corresponds to Extrapolator::m_searchLevel = 2/3 - search with Propagation
  if (propptr && !m_searchWithDistance) {
    const Trk::TrackParameters *closestTrackParameters = nullptr;

    double distanceToSurface = 10e10;

    // policy change --- only measured parameters are taken
    DataVector<const TrackParameters>::const_iterator it = trk.trackParameters()->begin();

    for (; it != trk.trackParameters()->end(); ++it) {
      // change in policy --- only measured parameters are taken
      const Trk::TrackParameters *mtp = *it;
      if (!mtp || !mtp->covariance()) {
        continue;
      }

      // const Trk::IntersectionSolution* interSolutions =  propptr->intersect(**it, sf, *highestVolume);
      const Trk::IntersectionSolution *interSolutions = propptr->intersect(**it, sf, m_fieldProperties);
      if (!interSolutions) {
        return nullptr;
      }
      double currentDistance = fabs(((*interSolutions)[2])->pathlength());
      if (currentDistance < distanceToSurface) {
        // assign new distance to surface
        distanceToSurface = currentDistance;
        // set current TrackParmaters as closest
        closestTrackParameters = *it;
      }
      delete interSolutions;
    }
    return closestTrackParameters;
  }

  // -- corresponds to Extrapolator::m_searchLevel = 1 - search with dedicated algorithms for cylinder/sl/perigee
  // surface
  const Trk::TrackParameters *closestTrackParameters = nullptr;


  // policy change --- only measured parameters are taken
  DataVector<const TrackParameters>::const_iterator it = trk.trackParameters()->begin();
  std::vector<const Trk::TrackParameters *> measuredParameters;
  measuredParameters.reserve(trk.trackParameters()->size());
  for (; it != trk.trackParameters()->end(); ++it) {
    // dynamic cast the Measured ones
    const Trk::TrackParameters *mtp = *it;
    if (!mtp || !mtp->covariance()) {
      continue;
    }
    measuredParameters.push_back(*it);
  }

  // new policy --- take only measured parameters
  if (measuredParameters.empty()) {
    return nullptr;
  }

  if (m_searchWithDistance) {
    // loop over the track parameters and get the distance
    std::vector<const Trk::TrackParameters *>::const_iterator tpIter = measuredParameters.begin();
    std::vector<const Trk::TrackParameters *>::const_iterator tpIterEnd = measuredParameters.end();
    // set a maximum distance
    double closestDistance = 10e10;
    const Trk::TrackParameters *currentClosestParameters = nullptr;

    for (; tpIter != tpIterEnd; ++tpIter) {
      // forward-backward solution
      Amg::Vector3D tpDirection = (*tpIter)->momentum().normalized();

      Trk::DistanceSolution currentDistance = sf.straightLineDistanceEstimate((*tpIter)->position(), tpDirection);
      if (currentDistance.numberOfSolutions() > 0) {
        // get the one/two solution(s)
        double firstDistance = fabs(currentDistance.first());
        double secondDistance = currentDistance.numberOfSolutions() >
                                1 ? fabs(currentDistance.second()) : firstDistance;
        // now do the check
        if (firstDistance < closestDistance || secondDistance < closestDistance) {
          currentClosestParameters = (*tpIter);
          closestDistance = firstDistance <= secondDistance ? firstDistance : secondDistance;
        }
      }
    }

    // return what has shown to be closest
    return currentClosestParameters;
  }



  const Trk::CylinderSurface *ccsf = dynamic_cast<const Trk::CylinderSurface *>(&sf);
  if (ccsf) {
    Trk::TrkParametersComparisonFunction tParFinderCylinder(ccsf->bounds().r());
    closestTrackParameters =
      *(std::min_element(measuredParameters.begin(), measuredParameters.end(), tParFinderCylinder));

    return closestTrackParameters;
  }

  const Trk::StraightLineSurface *slsf = dynamic_cast<const Trk::StraightLineSurface *>(&sf);
  const Trk::PerigeeSurface *persf = nullptr;
  if (!slsf) {
    persf = dynamic_cast<const Trk::PerigeeSurface *>(&sf);
  }

  if (slsf || persf) {
    Trk::TrkParametersComparisonFunction tParFinderLine(sf.center(), sf.transform().rotation().col(2));
    closestTrackParameters = *(std::min_element(measuredParameters.begin(), measuredParameters.end(), tParFinderLine));

    return closestTrackParameters;
  }

  Trk::TrkParametersComparisonFunction tParFinderCenter(sf.center());
  closestTrackParameters = *(std::min_element(measuredParameters.begin(), measuredParameters.end(), tParFinderCenter));

  return closestTrackParameters;
}

// finalize
StatusCode
Trk::Navigator::finalize() {

  if (msgLvl(MSG::DEBUG)) {
    ATH_MSG_DEBUG("[N] " << name() << " Perfomance Statistics : --------------------------------");
    ATH_MSG_DEBUG("     -> Number of forward calls        : " << m_forwardCalls);
    ATH_MSG_DEBUG("        - 1st forward bound switch     : " << m_forwardFirstBoundSwitch);
    ATH_MSG_DEBUG("        - 2nd forward bound switch     : " << m_forwardSecondBoundSwitch);
    ATH_MSG_DEBUG("        - 3rd forward bound switch     : " << m_forwardThirdBoundSwitch);
    ATH_MSG_DEBUG("     -> Number of backward calls       : " << m_backwardCalls);
    ATH_MSG_DEBUG("        - 1st backward bound switch    : " << m_backwardFirstBoundSwitch);
    ATH_MSG_DEBUG("        - 2nd backward bound switch    : " << m_backwardSecondBoundSwitch);
    ATH_MSG_DEBUG("        - 3rd backward bound switch    : " << m_backwardThirdBoundSwitch);
    ATH_MSG_DEBUG("     -> Number of outsideVolume cases  : " << m_outsideVolumeCase);
    ATH_MSG_DEBUG("        - successfully recovered       : " << m_sucessfulBackPropagation);
    ATH_MSG_DEBUG(" ---------------------------------------------------------------------");
  }

  return StatusCode::SUCCESS;
}

const Trk::TrackingGeometry*
Trk::Navigator::trackingGeometry(const EventContext& ctx) const
{
  if (!m_useConditions) {
    const TrackingGeometry* trackingGeometry = nullptr;
    if (detStore()
          ->retrieve(trackingGeometry, m_trackingGeometryName)
          .isFailure()) {
      ATH_MSG_FATAL("Could not retrieve TrackingGeometry from DetectorStore.");
      throw Trk::NavigatorException();
    }
    return trackingGeometry;
  } else {
    SG::ReadCondHandle<TrackingGeometry> handle(m_trackingGeometryReadKey, ctx);
    if (!handle.isValid()) {
      ATH_MSG_FATAL("Could not retrieve TrackingGeometry from DetectorStore.");
      throw Trk::NavigatorException();
    }
    return handle.cptr();
  }
}

