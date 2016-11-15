/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Navigator.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

// Gaudi Kernel
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
// StoreGate
#include "StoreGate/StoreGateSvc.h"
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
// Validation mode - TTree includes
#include "TTree.h"
#include "GaudiKernel/ITHistSvc.h"
// Data Model
#include "AthContainers/DataVector.h"
// Amg
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

Trk::MagneticFieldProperties Trk::Navigator::s_zeroMagneticField(Trk::NoField);

// constructor
Trk::Navigator::Navigator(const std::string &t, const std::string &n, const IInterface *p) :
  AthAlgTool(t, n, p),
  m_trackingGeometry(0),
  m_trackingGeometrySvc("AtlasTrackingGeometrySvc", n),
  m_trackingGeometryName("AtlasTrackingGeometry"),
  m_insideVolumeTolerance(1. * Gaudi::Units::mm),
  m_isOnSurfaceTolerance(0.005 * Gaudi::Units::mm),
  m_useStraightLineApproximation(false),
  m_searchWithDistance(true),
  m_validationMode(false),
  m_validationTreeName("NavigatorValidation"),
  m_validationTreeDescription("Boundary Surface hits"),
  m_validationTreeFolder("/val/NavigationValidation"),
  m_validationTree(0),
  m_boundariesCounter(0),
  m_boundaries{},
  m_boundaryHitX{},
  m_boundaryHitY{},
  m_boundaryHitR{},
  m_boundaryHitZ{},
  m_forwardCalls(0),
  m_forwardFirstBoundSwitch(0),
  m_forwardSecondBoundSwitch(0),
  m_forwardThirdBoundSwitch(0),
  m_backwardCalls(0),
  m_backwardFirstBoundSwitch(0),
  m_backwardSecondBoundSwitch(0),
  m_backwardThirdBoundSwitch(0),
  m_outsideVolumeCase(0),
  m_sucessfulBackPropagation(0),
  m_fastField(false) {
  declareInterface<INavigator>(this);
  // steering of algorithms
  declareProperty("TrackingGeometrySvc", m_trackingGeometrySvc);
  declareProperty("InsideVolumeTolerance", m_insideVolumeTolerance);
  declareProperty("IsOnSurfaceTolerance", m_isOnSurfaceTolerance);
  declareProperty("UseStraightLineApproximation", m_useStraightLineApproximation);
  // closest parameter search with new Surface::distance method
  declareProperty("SearchWithDistanceToSurface", m_searchWithDistance);
  // switch for Validation mode
  declareProperty("ValidationMode", m_validationMode);
  declareProperty("ValidationTreeName", m_validationTreeName);
  declareProperty("ValidationTreeDescription", m_validationTreeDescription);
  declareProperty("ValidationTreeFolder", m_validationTreeFolder);
  // Magnetic field properties
  declareProperty("MagneticFieldProperties", m_fastField);
}

// destructor
Trk::Navigator::~Navigator() {
}

// Athena standard methods
// initialize
StatusCode
Trk::Navigator::initialize() {
  // Initialize StatusCode
  //StatusCode s = StatusCode::SUCCESS;

  // get the TrackingGeometrySvc
  if (m_trackingGeometrySvc.retrieve().isSuccess()) {
    ATH_MSG_INFO("Successfully retrieved " << m_trackingGeometrySvc);
    m_trackingGeometryName = m_trackingGeometrySvc->trackingGeometryName();
  } else {
    ATH_MSG_WARNING("Couldn't retrieve " << m_trackingGeometrySvc << ". ");
    ATH_MSG_WARNING(" -> Trying to retrieve default '" << m_trackingGeometryName << "' from DetectorStore.");
  }

  // the validation setup ----------------------------------------------------------------------------------
  if (m_validationMode) {
    // create the new Tree
    m_validationTree = new TTree(m_validationTreeName.c_str(), m_validationTreeDescription.c_str());

    // counter for boundary surfaces
    m_validationTree->Branch("BoundaySurfacesHit", &m_boundaries, "boundshit/I");
    m_validationTree->Branch("BoundaryHitX", m_boundaryHitX, "boundshitx[boundshit]/F");
    m_validationTree->Branch("BoundaryHitY", m_boundaryHitY, "boundshity[boundshit]/F");
    m_validationTree->Branch("BoundaryHitR", m_boundaryHitR, "boundshitz[boundshit]/F");
    m_validationTree->Branch("BoundaryHitZ", m_boundaryHitZ, "boundshitr[boundshit]/F");

    // now register the Tree
    ITHistSvc *tHistSvc = nullptr;
    if (service("THistSvc", tHistSvc).isFailure()) {
      ATH_MSG_ERROR("Could not find Hist Service -> Switching ValidationMode Off !");
      delete m_validationTree;
      m_validationTree = nullptr;
    }
    if ((tHistSvc->regTree(m_validationTreeFolder, m_validationTree)).isFailure()) {
      ATH_MSG_ERROR("Could not register the validation Tree -> Switching ValidationMode Off !");
      delete m_validationTree;
      m_validationTree = nullptr;
    }
  } // ------------- end of validation mode -----------------------------------------------------------------
  m_fieldProperties = m_fastField ? Trk::MagneticFieldProperties(Trk::FastField) : Trk::MagneticFieldProperties(
    Trk::FullField);

  ATH_MSG_DEBUG("initialize() successful");
  return StatusCode::SUCCESS;
}

const Trk::TrackingVolume *
Trk::Navigator::volume(const Amg::Vector3D &gp) const {
  if (!m_trackingGeometry) {
    updateTrackingGeometry();
  }
  return(m_trackingGeometry->lowestTrackingVolume(gp));
}

const Trk::TrackingVolume *
Trk::Navigator::highestVolume() const {
  if (!m_trackingGeometry) {
    updateTrackingGeometry();
  }
  return(m_trackingGeometry->highestTrackingVolume());
}

const Trk::BoundarySurface<Trk::TrackingVolume> *
Trk::Navigator::nextBoundarySurface(const Trk::IPropagator &prop,
                                    const Trk::TrackParameters &parms,
                                    Trk::PropDirection dir) const {
  const Trk::TrackingVolume *trackingVolume = volume(parms.position());

  if (trackingVolume) {
    return(nextBoundarySurface(prop, parms, dir, *trackingVolume));
  }
  return 0;
}

const Trk::BoundarySurface<Trk::TrackingVolume> *
Trk::Navigator::nextBoundarySurface(const Trk::IPropagator &prop,
                                    const Trk::TrackParameters &parms,
                                    Trk::PropDirection dir,
                                    const Trk::TrackingVolume &vol) const {
  if (!m_trackingGeometry) {
    updateTrackingGeometry();
  }

  // get the surface accessor
  const Trk::ObjectAccessor &surfAcc =
    vol.boundarySurfaceAccessor(parms.position(), dir * parms.momentum().normalized());
  // initialize the currentBoundary surface
  const Trk::BoundarySurface<Trk::TrackingVolume> *currentBoundary = 0;
  bool outsideVolume = surfAcc.inverseRetrieval();
  // attempt counter
  int tryBoundary = 0;

  // set the prop direction according to inverseRetrieval result
  Trk::PropDirection searchDir = dir;
  if (outsideVolume) {
    searchDir = (dir == Trk::alongMomentum) ? Trk::oppositeMomentum : Trk::alongMomentum;
  }

  // debug version
  ATH_MSG_VERBOSE("  [N] Starting parameters are :" << parms);

  // loop over the the boundary surfaces according to the accessor type
  for (surfAcc.begin(); surfAcc.end(); surfAcc.operator ++ ()) {
    ++tryBoundary;
    // ----------------- output to screen if outputLevel() says so --------
    ATH_MSG_VERBOSE("  [N] " << tryBoundary << ". try - BoundarySurface " << surfAcc.accessor()
                             << " of Volume: '" << vol.volumeName() << "'.");
    // get the boundary Surface according to the surfaceAccessor
    currentBoundary = vol.boundarySurface(surfAcc);
    const Trk::Surface &currentSurface = currentBoundary->surfaceRepresentation();

    const Trk::TrackParameters *trackPar = 0;
    // do either RungeKutta (always after first unsuccessful try) or straight line
    trackPar = (!m_useStraightLineApproximation || tryBoundary > 1) ?
               prop.propagateParameters(parms,
                                        currentSurface,
                                        searchDir, true,
                                        // vol) :
                                        m_fieldProperties) :
               prop.propagateParameters(parms,
                                        currentSurface,
                                        searchDir, true,
                                        s_zeroMagneticField);

    if (trackPar) {
      ATH_MSG_VERBOSE("  [N] --> next BoundarySurface found with Parameters: " << *trackPar);
      // ----------------- output to screen if outputLevel() says so --------

      // ----------------- record if in validation mode ----------------------
      if (m_validationMode && m_boundariesCounter < TRKEXTOOLS_MAXNAVSTEPS) {
        const Amg::Vector3D &posOnBoundary = trackPar->position();
        m_boundaryHitX[m_boundariesCounter] = posOnBoundary.x();
        m_boundaryHitY[m_boundariesCounter] = posOnBoundary.y();
        m_boundaryHitR[m_boundariesCounter] = posOnBoundary.perp();
        m_boundaryHitZ[m_boundariesCounter] = posOnBoundary.z();
        m_boundariesCounter++;
      } // ------------------------------------------------------------------

      delete trackPar;
      return currentBoundary;
    }
  }

  // check the insideStatus
  bool insideVolume = vol.inside(parms.position());
  if (!insideVolume) {
    ++m_outsideVolumeCase;
  }

  return 0;
}

const Trk::NavigationCell
Trk::Navigator::nextTrackingVolume(const Trk::IPropagator &prop,
                                   const Trk::TrackParameters &parms,
                                   Trk::PropDirection dir,
                                   const Trk::TrackingVolume &vol) const {
  if (!m_trackingGeometry) {
    updateTrackingGeometry();
  }

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
  const Trk::ObjectAccessor &surfAcc =
    vol.boundarySurfaceAccessor(parms.position(), dir * parms.momentum().normalized());
  // the object accessor already solved the outside question
  bool outsideVolume = surfAcc.inverseRetrieval();
  // initialize the boundary pointer / tracking volume pointer
  const Trk::BoundarySurface<Trk::TrackingVolume> *currentBoundary = 0;
  const Trk::TrackingVolume *nextVolume = 0;

  // debug version
  ATH_MSG_VERBOSE("  [N] Starting parameters are : " << parms);
  ATH_MSG_VERBOSE(
    "  [N] This corresponds to [r,z] = [ " << parms.position().perp() << ", " << parms.position().z() << "]");
  ATH_MSG_VERBOSE("  [N] Boundary Surface accessor : " << surfAcc);

  // set the prop direction according to inverseRetrieval result
  Trk::PropDirection searchDir = dir;
  if (outsideVolume) {
    ATH_MSG_VERBOSE("  [N] Parameters have been flagged as being outside !");
    searchDir = (dir == Trk::alongMomentum) ? Trk::oppositeMomentum : Trk::alongMomentum;
  }

  // loop over boundary surfaces
  int tryBoundary = 0;
  for (surfAcc.begin(); surfAcc.end(); surfAcc.operator ++ ()) {
    ++tryBoundary;
    // get the boundary surface associated to the surfaceAccessor
    currentBoundary = vol.boundarySurface(surfAcc);

    // ----------------- output to screen if outputLevel() says so --------
    if (!currentBoundary) {
      ATH_MSG_WARNING("  [N] " << tryBoundary << ". try - BoundarySurface " << surfAcc.accessor()
                               << " of Volume: '" << vol.volumeName() << "' NOT FOUND.");
      continue;
    } else {
      ATH_MSG_VERBOSE("  [N] " << tryBoundary << ". try - BoundarySurface " << surfAcc.accessor()
                               << " of Volume: '" << vol.volumeName() << "'.");
    }

    const Trk::Surface &currentSurface = currentBoundary->surfaceRepresentation();
    // try the propagation
    const Trk::TrackParameters *trackPar = 0;
    // do either RungeKutta (always after first unsuccessful try) or straight line
    if (!currentSurface.isOnSurface(parms.position(), true, 0., 0.)) {
      trackPar = (!m_useStraightLineApproximation || tryBoundary > 1) ?
                 prop.propagateParameters(parms,
                                          currentSurface,
                                          searchDir, true,
                                          // vol) :
                                          m_fieldProperties) :
                 prop.propagateParameters(parms,
                                          currentSurface,
                                          searchDir, true,
                                          s_zeroMagneticField);
    } else {
      trackPar = parms.clone();
    }
    if (trackPar) {
      // the next volume pointer
      nextVolume = currentBoundary->attachedVolume(trackPar->position(), trackPar->momentum().normalized(), dir);
      // ----------------- output to screen if outputLevel() says so --------
      if (msgLvl(MSG::VERBOSE)) {
        ATH_MSG_VERBOSE("  [N] --> next BoundarySurface found with Parameters: " << *trackPar);
        ATH_MSG_VERBOSE(
          "  [N] This corresponds to [r,z] = [ " << trackPar->position().perp() << ", " << trackPar->position().z() <<
        "]");

        // log of the boundary surface
        currentBoundary->debugInfo(msg(MSG::VERBOSE));
        ATH_MSG_VERBOSE("  [N] --> Quering the BoundarySurface for the associated TrackingVolume: ");
        ATH_MSG_VERBOSE('\t' << '\t' << (nextVolume ? nextVolume->volumeName() : "None"));
      }


      // ----------------- record if in validation mode ----------------------
      if (m_validationMode && m_boundariesCounter < TRKEXTOOLS_MAXNAVSTEPS) {
        const Amg::Vector3D &posOnBoundary = trackPar->position();
        m_boundaryHitX[m_boundariesCounter] = posOnBoundary.x();
        m_boundaryHitY[m_boundariesCounter] = posOnBoundary.y();
        m_boundaryHitR[m_boundariesCounter] = posOnBoundary.perp();
        m_boundaryHitZ[m_boundariesCounter] = posOnBoundary.z();
        m_boundariesCounter++;
      } // ------------------------------------------------------------------

      return Trk::NavigationCell(nextVolume, trackPar, Trk::BoundarySurfaceFace(surfAcc.accessor()));
    }

    // ---------------------------------------------------
    if (!first && searchDir == Trk::alongMomentum) {
      ++m_forwardFirstBoundSwitch;
      first = true;
    }else if (!second && searchDir == Trk::alongMomentum) {
      ++m_forwardSecondBoundSwitch;
      second = true;
    }else if (searchDir == Trk::alongMomentum) {
      ++m_forwardThirdBoundSwitch;
    }else if (!first && searchDir == Trk::oppositeMomentum) {
      ++m_backwardFirstBoundSwitch;
      first = true;
    }else if (!second && searchDir == Trk::oppositeMomentum) {
      ++m_backwardSecondBoundSwitch;
      second = true;
    }else if (searchDir == Trk::oppositeMomentum) {
      ++m_backwardThirdBoundSwitch;
    }
    // ---------------------------------------------------
  }

  // return what you have : no idea
  return Trk::NavigationCell(0, 0);
}

const Trk::NavigationCell
Trk::Navigator::nextDenseTrackingVolume(
  const Trk::IPropagator &prop,
  const Trk::TrackParameters &parms,
  const Trk::Surface *destinationSurface,
  Trk::PropDirection dir,
  Trk::ParticleHypothesis particle,
  const Trk::TrackingVolume &vol,
  double &path) const {
  ATH_MSG_DEBUG(
    "  [N] nextDenseTrackingVolume() to volume '" << vol.volumeName() << "', starting from " << parms.position());

  if (!m_trackingGeometry) {
    updateTrackingGeometry();
  }

  Trk::NavigationCell solution(0, 0);
  const Trk::TrackParameters *currPar = &parms;

  double tol = 0.001;
  path = 0.;

  // check position : if volume exit return trivial solution
  const Trk::TrackingVolume *nextVolume = 0;
  if (atVolumeBoundary(currPar, &vol, dir, nextVolume, tol) && nextVolume != (&vol)) {
    if (!nextVolume) {
      Amg::Vector3D gp = currPar->position();
      const Trk::TrackingVolume *currStatic = m_trackingGeometry->lowestStaticTrackingVolume(gp);
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

  const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > > bounds = vol.boundarySurfaces();
  for (unsigned int ib = 0; ib < bounds.size(); ib++) {
    const Trk::Surface *nextSurface = &((bounds[ib].getPtr())->surfaceRepresentation());
    surfaces->push_back(std::pair<const Trk::Surface *, Trk::BoundaryCheck>(nextSurface, true));
  }

  // propagate
  std::vector<unsigned int> solutions;
  // const Trk::TrackParameters* nextPar = prop.propagate(parms,*surfaces,dir,vol,particle,solutions,path);
  const Trk::TrackParameters *nextPar = prop.propagate(parms, *surfaces, dir, m_fieldProperties, particle, solutions,
                                                       path, false, false, &vol);
  // if (nextPar) throwIntoGarbageBin(nextPar);
  if (nextPar) {
    Amg::Vector3D gp = nextPar->position();
    if (destinationSurface && destinationSurface == &nextPar->associatedSurface()) {
      solution = Trk::NavigationCell(&vol, nextPar);
    } else if (atVolumeBoundary(nextPar, &vol, dir, nextVolume, tol)) {
      if (!nextVolume) {
        // detached volume boundary or world boundary : resolve
        const Trk::TrackingVolume *currStatic = m_trackingGeometry->lowestStaticTrackingVolume(gp);
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
Trk::Navigator::atVolumeBoundary(const Trk::TrackParameters *parms, const Trk::TrackingVolume *vol,
                                 Trk::PropDirection dir, const Trk::TrackingVolume * &nextVol, double tol) const {
  bool isAtBoundary = false;

  nextVol = 0;
  if (!vol) {
    return isAtBoundary;
  }
  const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > > bounds = vol->boundarySurfaces();
  for (unsigned int ib = 0; ib < bounds.size(); ib++) {
    const Trk::Surface &surf = (bounds[ib].getPtr())->surfaceRepresentation();
    if (surf.isOnSurface(parms->position(), true, tol, tol)) {
      // isAtBoundary = true;
      // const Trk::TrackingVolume* attachedVol =
      //  (bounds[ib].getPtr())->attachedVolume(parms->position(),parms->momentum(),dir);
      // if (!nextVol && attachedVol ) nextVol = attachedVol;

      // sanity check to enforce the desired tolerance
      Trk::DistanceSolution distSol = surf.straightLineDistanceEstimate(parms->position(),
                                                                        dir * parms->momentum().unit());
      if (distSol.currentDistance(false) < tol && distSol.numberOfSolutions() > 0) {
        isAtBoundary = true;
        const Trk::TrackingVolume *attachedVol = (bounds[ib].getPtr())->attachedVolume(
          parms->position(), parms->momentum(), dir);
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

  /*  for debugging
     ATH_MSG_DEBUG("navigator particle R,phi,z, momentum:"<<
        parms->position().perp()<<","<<parms->position().phi()<<","<<parms->position().z() <<","<<parms->momentum());
     ATH_MSG_DEBUG("navigator static volume position:"<< vol->center());
     const Trk::CylinderVolumeBounds* cyl = dynamic_cast<const Trk::CylinderVolumeBounds*> (&(vol->volumeBounds()));
     if (cyl) ATH_MSG_DEBUG("---> cylinder volume
        dimensions:"<<cyl->innerRadius()<<","<<cyl->outerRadius()<<","<<cyl->halflengthZ());

     for (unsigned int ib=0; ib< bounds.size(); ib++ ){
     const Trk::Surface& surf = (bounds[ib].getPtr())->surfaceRepresentation();
     Trk::DistanceSolution distSol = surf.straightLineDistanceEstimate(parms->position(),
                                      dir*parms->momentum().unit());
     ATH_MSG_DEBUG("---> decomposed boundary surface position, normal, current
        distance:"<<ib<<","<<surf.center()<<","<<surf.normal()<<","<<distSol.currentDistance(false));
     ATH_MSG_DEBUG("---> estimated distance to (first solution):boundary
        check:"<<distSol.numberOfSolutions()<<","<<distSol.first()<<":"<<
            surf.isOnSurface(parms->position()+distSol.first()*dir*parms->momentum().unit(),true,tol,tol));
     if (distSol.numberOfSolutions()>1)
      ATH_MSG_DEBUG("---> estimated distance to (second solution):boundary check:" << distSol.second()<< ","<<
              surf.isOnSurface(parms->position()+distSol.second()*dir*parms->momentum().unit(),true,tol,tol));
     }
   */

  return isAtBoundary;
}

const Trk::TrackParameters *
Trk::Navigator::closestParameters(const Trk::Track &trk,
                                  const Trk::Surface &sf,
                                  const Trk::IPropagator *propptr) const {
  if (!m_trackingGeometry) {
    updateTrackingGeometry();
  }

  // -- corresponds to Extrapolator::m_searchLevel = 2/3 - search with Propagation
  if (propptr && !m_searchWithDistance) {
    const Trk::TrackParameters *closestTrackParameters = 0;
    // const Trk::TrackingVolume*         highestVolume = (m_trackingGeometry->highestTrackingVolume());

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
        return 0;
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
  const Trk::TrackParameters *closestTrackParameters = 0;


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
  if (!measuredParameters.size()) {
    return 0;
  }

  if (m_searchWithDistance) {
    // loop over the track parameters and get the distance
    std::vector<const Trk::TrackParameters *>::const_iterator tpIter = measuredParameters.begin();
    std::vector<const Trk::TrackParameters *>::const_iterator tpIterEnd = measuredParameters.end();
    // set a maximum distance
    double closestDistance = 10e10;
    const Trk::TrackParameters *currentClosestParameters = 0;

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
  const Trk::PerigeeSurface *persf = 0;
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

void
Trk::Navigator::validationAction() const {
  // first record the values
  if (m_validationTree) {
    m_boundaries = long(m_boundariesCounter);
    m_validationTree->Fill();
    // then reset
    m_boundariesCounter = 0;
  }
}

// finalize
StatusCode
Trk::Navigator::finalize() {
  m_trackingGeometry = 0;

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

  ATH_MSG_INFO("finalize() successful");
  return StatusCode::SUCCESS;
}

const Trk::TrackingGeometry *
Trk::Navigator::trackingGeometry() const {
  if (!m_trackingGeometry) {
    updateTrackingGeometry();
  }
  return m_trackingGeometry;
}

StatusCode
Trk::Navigator::updateTrackingGeometry() const {
  // -------------------- public TrackingGeometry (from DetectorStore) ----------------------------
  // get the DetectorStore
  StoreGateSvc *detectorStore = 0;
  StatusCode s = service("DetectorStore", detectorStore);

  if (s.isFailure()) {
    ATH_MSG_FATAL("DetectorStore service not found!");
    throw Trk::NavigatorException();
  }

  s = detectorStore->retrieve(m_trackingGeometry, m_trackingGeometryName);
  if (s.isFailure()) {
    ATH_MSG_FATAL("Could not retrieve TrackingGeometry '" << m_trackingGeometryName << "' from DetectorStore.");
    ATH_MSG_FATAL("  - probably the chosen layout is not supported / no cool tag exists. ");
    throw Trk::NavigatorException();
  }
  return s;
}
