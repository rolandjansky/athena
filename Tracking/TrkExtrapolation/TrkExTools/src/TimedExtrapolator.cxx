/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TimedExtrapolator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PhysicalConstants.h"
// Trk include
#include "TrkExTools/TimedExtrapolator.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkExInterfaces/IMultipleScatteringUpdator.h"
#include "TrkExInterfaces/IEnergyLossUpdator.h"
#include "TrkExUtils/IntersectionSolution.h"
#include "TrkSurfaces/SurfaceBounds.h"
#include "TrkSurfaces/DiscBounds.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkTrack/Track.h"
#include "TrkGeometry/DetachedTrackingVolume.h"
#include "TrkGeometry/AlignableTrackingVolume.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/CompoundLayer.h"
#include "TrkGeometry/CylinderLayer.h"
#include "TrkGeometry/SubtractedCylinderLayer.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkVolumes/BoundarySurface.h"
#include "TrkVolumes/BoundarySurfaceFace.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkVolumes/Volume.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkEventUtils/TrkParametersComparisonFunction.h"
#include "TrkDetDescrInterfaces/IDynamicLayerCreator.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkDetDescrUtils/GeometrySignature.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkGeometry/MagneticFieldProperties.h"
// for the comparison with a pointer
#include <cstdint>

// Amg
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

// Trk
#include "TrkSurfaces/PlaneSurface.h"

const Trk::ParticleMasses Trk::TimedExtrapolator::s_particleMasses;

// constructor
Trk::TimedExtrapolator::TimedExtrapolator(const std::string &t, const std::string &n, const IInterface *p) :
  AthAlgTool(t, n, p),
  m_propagators(),
  m_stepPropagator("Trk::STEP_Propagator/AtlasSTEP_Propagator"),
  m_navigator("Trk::Navigator/AtlasNavigator"),
  m_updators(),
  m_msupdators(),
  m_elossupdators(),
  // m_dynamicLayerCreator(),
  m_subPropagators(Trk::NumberOfSignatures),
  m_subUpdators(Trk::NumberOfSignatures),
  m_propNames(),
  m_updatNames(),
  m_meotpIndex(0),
  m_configurationLevel(10),
  m_searchLevel(10),
  m_includeMaterialEffects(true),
  m_requireMaterialDestinationHit(false),
  m_stopWithNavigationBreak(false),
  m_stopWithUpdateZero(false),
  m_skipInitialLayerUpdate(false),
  m_referenceMaterial(false),
  m_extendedLayerSearch(true),
  m_initialLayerAttempts(3),
  m_successiveLayerAttempts(1),
  m_tolerance(0.002),
  m_caloMsSecondary(false),
  m_activeOverlap(false),
  m_robustSampling(true),
  m_useDenseVolumeDescription(true),
  m_useMuonMatApprox(false),
  m_checkForCompundLayers(false),
  m_resolveActive(false),
  m_resolveMultilayers(true),
  m_printHelpOutputAtInitialize(false),
  m_printRzOutput(true),
  m_navigationStatistics(false),
  m_navigationBreakDetails(false),
  m_materialEffectsOnTrackValidation(false),
  //  m_cacheLastMatLayer(false),
  m_maxNavigSurf{},
  m_maxNavigVol{},
  m_fastField(false) {
  declareInterface<ITimedExtrapolator>(this);

  // extrapolation steering
  declareProperty("StopWithNavigationBreak", m_stopWithNavigationBreak);
  declareProperty("StopWithUpdateKill", m_stopWithUpdateZero);
  declareProperty("SkipInitialPostUpdate", m_skipInitialLayerUpdate);
  // propagation steering
  declareProperty("Propagators", m_propagators);
  declareProperty("SubPropagators", m_propNames);
  declareProperty("STEP_Propagator", m_stepPropagator);
  // material effects handling
  declareProperty("ApplyMaterialEffects", m_includeMaterialEffects);
  declareProperty("RequireMaterialDestinationHit", m_requireMaterialDestinationHit);
  declareProperty("MaterialEffectsUpdators", m_updators);
  declareProperty("MultipleScatteringUpdators", m_msupdators);
  declareProperty("EnergyLossUpdators", m_elossupdators);
  declareProperty("SubMEUpdators", m_updatNames);
  //  declareProperty("CacheLastMaterialLayer", m_cacheLastMatLayer);
  // general behavior navigation
  declareProperty("SearchLevelClosestParameters", m_searchLevel);
  declareProperty("Navigator", m_navigator);
  declareProperty("UseDenseVolumeDescription", m_useDenseVolumeDescription);
  // muon system specifics
  declareProperty("UseMuonMatApproximation", m_useMuonMatApprox);
  declareProperty("CheckForCompoundLayers", m_checkForCompundLayers);
  declareProperty("ResolveMuonStation", m_resolveActive);
  declareProperty("ResolveMultilayers", m_resolveMultilayers);
  declareProperty("ConsiderMuonStationOverlaps", m_activeOverlap);
  // declareProperty("DynamicLayerCreator",          m_dynamicLayerCreator);
  declareProperty("RobustSampling", m_robustSampling );
  // material & navigation related steering
  declareProperty("MaterialEffectsOnTrackProviderIndex", m_meotpIndex);
  declareProperty("MaterialEffectsOnTrackValidation", m_materialEffectsOnTrackValidation);
  declareProperty("ReferenceMaterial", m_referenceMaterial);
  declareProperty("ExtendedLayerSearch", m_extendedLayerSearch);
  declareProperty("InitialLayerAttempts", m_initialLayerAttempts);
  declareProperty("SuccessiveLayerAttempts", m_successiveLayerAttempts);
  // debug and validation
  declareProperty("HelpOutput", m_printHelpOutputAtInitialize);
  declareProperty("positionOutput", m_printRzOutput);
  declareProperty("NavigationStatisticsOutput", m_navigationStatistics);
  declareProperty("DetailedNavigationOutput", m_navigationBreakDetails);
  declareProperty("Tolerance", m_tolerance);
  declareProperty("CaloMsSecondary", m_caloMsSecondary);
  // Magnetic field properties
  declareProperty("MagneticFieldProperties", m_fastField);
}

// destructor
Trk::TimedExtrapolator::~TimedExtrapolator() {
}

// Athena standard methods
// initialize
StatusCode
Trk::TimedExtrapolator::initialize() {
  m_fieldProperties = m_fastField ? Trk::MagneticFieldProperties(Trk::FastField) : Trk::MagneticFieldProperties(
    Trk::FullField);
  if (m_propagators.empty()) {
    m_propagators.push_back("Trk::RungeKuttaPropagator/DefaultPropagator");
  }
  if (m_updators.empty()) {
    m_updators.push_back("Trk::MaterialEffectsUpdator/DefaultMaterialEffectsUpdator");
  }
  if (m_msupdators.empty()) {
    m_msupdators.push_back("Trk::MultipleScatteringUpdator/AtlasMultipleScatteringUpdator");
  }
  if (m_elossupdators.empty()) {
    m_elossupdators.push_back("Trk::EnergyLossUpdator/AtlasEnergyLossUpdator");
  }

  if (!m_propagators.empty()) {
    if (m_propagators.retrieve().isFailure()) {
      ATH_MSG_FATAL("Failed to retrieve tool " << m_propagators);
      return StatusCode::FAILURE;
    } 
      ATH_MSG_INFO("Retrieved tools " << m_propagators);
    
  }


  // from the number of retrieved propagators set the configurationLevel
  unsigned int validprop = m_propagators.size();

  if (!validprop) {
    ATH_MSG_WARNING("None of the defined propagators could be retrieved!");
    ATH_MSG_WARNING("  Extrapolators jumps back in unconfigured mode, only strategy pattern methods can be used.");
  } else {
    m_configurationLevel = validprop - 1;
    ATH_MSG_VERBOSE("Configuration level automatically set to " << m_configurationLevel);
  }

  // Get the Navigation AlgTools
  if (m_navigator.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_navigator);
    return StatusCode::FAILURE;
  } 
    ATH_MSG_INFO("Retrieved tool " << m_navigator);
  
  // Get the Material Updator
  if (m_includeMaterialEffects && !m_updators.empty()) {
    if (m_updators.retrieve().isFailure()) {
      ATH_MSG_FATAL("None of the defined material updatros could be retrieved!");
      ATH_MSG_FATAL("No multiple scattering and energy loss material update will be done.");
      return StatusCode::FAILURE;
    } 
      ATH_MSG_INFO("Retrieved tools: " << m_updators);
    
  }

  // from the number of retrieved propagators set the configurationLevel
  unsigned int validmeuts = m_updators.size();

  // -----------------------------------------------------------
  // Sanity check 1

  if (m_propNames.empty() && !m_propagators.empty()) {
    ATH_MSG_DEBUG("Inconsistent setup of Extrapolator, no sub-propagators configured, doing it for you. ");
    m_propNames.push_back(m_propagators[0]->name().substr(8, m_propagators[0]->name().size() - 8));
  }

  if (m_updatNames.empty() && !m_updators.empty()) {
    ATH_MSG_DEBUG("Inconsistent setup of Extrapolator, no sub-materialupdators configured, doing it for you. ");
    m_updatNames.push_back(m_updators[0]->name().substr(8, m_updators[0]->name().size() - 8));
  }

  // -----------------------------------------------------------
  // Sanity check 2
  // fill the number of propagator names and updator names up with first one
  while (int(m_propNames.size()) < int(Trk::NumberOfSignatures)) {
    m_propNames.push_back(m_propNames[0]);
  }
  while (int(m_updatNames.size()) < int(Trk::NumberOfSignatures)) {
    m_updatNames.push_back(m_updatNames[0]);
  }
  if (validprop && validmeuts) {
    // Per definition: if configured not found, take the lowest one
    for (unsigned int isign = 0; int(isign) < int(Trk::NumberOfSignatures); ++isign) {
      unsigned int index = 0;

      for (unsigned int iProp = 0; iProp < m_propagators.size(); iProp++) {
        std::string pname = m_propagators[iProp]->name().substr(8, m_propagators[iProp]->name().size() - 8);
        if (m_propNames[isign] == pname) {
          index = iProp;
        }
      }
      ATH_MSG_DEBUG(" subPropagator:" << isign << " pointing to propagator: " << m_propagators[index]->name());
      m_subPropagators[isign] = (index < validprop) ? &(*m_propagators[index]) : &(*m_propagators[Trk::Global]);

      index = 0;
      for (unsigned int iUp = 0; iUp < m_updators.size(); iUp++) {
        std::string uname = m_updators[iUp]->name().substr(8, m_updators[iUp]->name().size() - 8);
        if (m_updatNames[isign] == uname) {
          index = iUp;
        }
      }
      ATH_MSG_DEBUG(" subMEUpdator:" << isign << " pointing to updator: " << m_updators[index]->name());
      m_subUpdators[isign] = (index < validmeuts) ? &(*m_updators[index]) : &(*m_updators[Trk::Global]);
    }
  } else {
    ATH_MSG_FATAL("Configuration Problem of Extrapolator: "
                  << "  -- At least one IPropagator and IMaterialUpdator instance have to be given.! ");
  }


  m_maxNavigSurf = 1000;
  m_maxNavigVol = 50;


  ATH_MSG_INFO("initialize() successful");
  return StatusCode::SUCCESS;
}

// finalize
StatusCode
Trk::TimedExtrapolator::finalize() {
  ATH_MSG_INFO("finalize() successful");
  return StatusCode::SUCCESS;
}

const Trk::TrackParameters *
Trk::TimedExtrapolator::extrapolateWithPathLimit(
  const Trk::TrackParameters &parm,
  Trk::PathLimit &pathLim, Trk::TimeLimit &timeLim,
  Trk::PropDirection dir,
  Trk::ParticleHypothesis particle,
  std::vector<Trk::HitInfo> * &hitInfo,
  Trk::GeometrySignature &nextGeoID,
  const Trk::TrackingVolume *boundaryVol) const {
// extrapolation method intended for simulation of particle decay; collects intersections with active layers
// possible outcomes:1/ returns curvilinear parameters after reaching the maximal path
//                   2/ returns parameters at destination volume boundary
//                   3/ returns 0 ( particle stopped ) but keeps vector of hits

  Trk::TimedExtrapolator::Cache cache(m_maxNavigSurf);
  ATH_MSG_DEBUG(
    "M-[" << ++cache.m_methodSequence << "] extrapolateWithPathLimit(...) " << pathLim.x0Max << ", from " << parm.position());
  ATH_MSG_DEBUG(
    "M-[" << ++cache.m_methodSequence << "] extrapolateWithPathLimit(...): resolve active layers? " << m_resolveActive);

  if (!m_stepPropagator) {
    // Get the STEP_Propagator AlgTool
    if (m_stepPropagator.retrieve().isFailure()) {
      ATH_MSG_ERROR("Failed to retrieve tool " << m_stepPropagator);
      ATH_MSG_ERROR("Configure STEP Propagator for extrapolation with path limit");
      return nullptr;
    } 
      ATH_MSG_INFO("Retrieved tool " << m_stepPropagator);
    
  }

  // reset the path ( in x0 !!)
  cache.m_path = PathLimit(pathLim.x0Max - pathLim.x0Collected, pathLim.process);     // collect material locally

  // initialize hit vector
  cache.m_hitVector = hitInfo;

  // if no input volume, define as highest volume
  // const Trk::TrackingVolume* destVolume = boundaryVol ? boundaryVol : m_navigator->highestVolume();
  cache.m_currentStatic = nullptr;
  if (boundaryVol && !boundaryVol->inside(parm.position(), m_tolerance)) {
    return nullptr;
  }

  // extrapolate to destination volume boundary with path limit
  const Trk::TrackParameters *returnParms = extrapolateToVolumeWithPathLimit(cache,parm, timeLim, dir, particle, nextGeoID,
                                                                             boundaryVol);

  // save actual path on output
  if (cache.m_path.x0Collected > 0.) {
    pathLim.updateMat(cache.m_path.x0Collected, cache.m_path.weightedZ / cache.m_path.x0Collected, cache.m_path.l0Collected);
  }

  if (hitInfo) {
    ATH_MSG_DEBUG(hitInfo->size() << " identified intersections found");
    for (unsigned int ih = 0; ih < hitInfo->size(); ih++) {
      ATH_MSG_DEBUG("R,z,ID:" << (*hitInfo)[ih].trackParms->position().perp() << ","
                              << (*hitInfo)[ih].trackParms->position().z() << ","
                              << (*hitInfo)[ih].detID);
    }
  }

  std::map<const Trk::TrackParameters *, bool>::iterator garbageIter = cache.m_garbageBin.begin();
  std::map<const Trk::TrackParameters *, bool>::iterator garbageEnd = cache.m_garbageBin.end();
  for (; garbageIter != garbageEnd; ++garbageIter) if (garbageIter->first) {
    if(garbageIter->first == returnParms) {
      auto ret=returnParms->clone();
      ATH_MSG_DEBUG("  [+] garbage - at " << positionOutput(garbageIter->first->position())<<" parm="<<garbageIter->first<<" is the return param. Cloning to"<<ret);
      returnParms=ret;
    }
  }

  return returnParms;
}

const Trk::TrackParameters *
Trk::TimedExtrapolator::extrapolateToVolumeWithPathLimit(
  Trk::TimedExtrapolator::Cache &cache,
  const Trk::TrackParameters &parm,
  Trk::TimeLimit &timeLim,
  Trk::PropDirection dir,
  Trk::ParticleHypothesis particle,
  Trk::GeometrySignature &nextGeoID,
  const Trk::TrackingVolume *destVol) const {
  // returns:
  //    A)  curvilinear track parameters if path limit reached
  //    B)  boundary parameters (at destination volume boundary)

  // initialize the return parameters vector
  const Trk::TrackParameters *returnParameters = nullptr;
  const Trk::TrackParameters *currPar = &parm;
  const Trk::TrackingVolume *currVol = nullptr;
  const Trk::TrackingVolume *nextVol = nullptr;
  std::vector<unsigned int> solutions;
  const Trk::TrackingVolume *assocVol = nullptr;
  unsigned int iDest = 0;

  ATH_MSG_DEBUG("  [+] start extrapolateToVolumeWithPathLimit - at " << positionOutput(parm.position())<<" parm="<<&parm);
  // destination volume boundary ?
  if (destVol && m_navigator->atVolumeBoundary(currPar, destVol, dir, nextVol, m_tolerance) && nextVol != destVol) {
    return &parm;
  }

  // if (cache.m_lastMaterialLayer && !cache.m_lastMaterialLayer->isOnLayer(parm.position())) {
  //   cache.m_lastMaterialLayer = nullptr;
  // }
  if (!cache.m_highestVolume) {
    cache.m_highestVolume = m_navigator->highestVolume();
  }

  emptyGarbageBin(cache,&parm);
  // navigation surfaces
  if (cache.m_navigSurfs.capacity() > m_maxNavigSurf) {
    cache.m_navigSurfs.reserve(m_maxNavigSurf);
  }
  cache.m_navigSurfs.clear();

  // target volume may not be part of tracking geometry
  if (destVol) {
    const Trk::TrackingVolume *tgVol = m_navigator->trackingGeometry()->trackingVolume(destVol->volumeName());
    if (!tgVol || tgVol != destVol) {
      const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > >& bounds = destVol->boundarySurfaces();
      for (unsigned int ib = 0; ib < bounds.size(); ib++) {
        const Trk::Surface &surf = (bounds[ib].get())->surfaceRepresentation();
        cache.m_navigSurfs.emplace_back(&surf, true);
      }
      iDest = bounds.size();
    }
  }

  // resolve current position
  bool updateStatic = false;
  Amg::Vector3D gp = parm.position();

  if (!cache.m_currentStatic || !cache.m_currentStatic->inside(gp, m_tolerance)) {
    cache.m_currentStatic = m_navigator->trackingGeometry()->lowestStaticTrackingVolume(gp);
    updateStatic = true;
  }
  if (m_navigator->atVolumeBoundary(currPar, cache.m_currentStatic, dir, nextVol,
                                    m_tolerance) && nextVol != cache.m_currentStatic) {
    // no next volume found --- end of the world
    if (!nextVol) {
      ATH_MSG_DEBUG("  [+] Word boundary reached        - at " << positionOutput(currPar->position()));
      nextGeoID = Trk::GeometrySignature(Trk::Unsigned);
      return currPar->clone();
    }
    cache.m_currentStatic = nextVol;
    updateStatic = true;
  }

  // current frame volume known-retrieve geoID
  nextGeoID = cache.m_currentStatic->geometrySignature();

  // resolve active Calo volumes if hit info required
  if (cache.m_hitVector && nextGeoID == Trk::Calo) {
    const Trk::AlignableTrackingVolume *alignTV = dynamic_cast<const Trk::AlignableTrackingVolume *> (cache.m_currentStatic);
    if (alignTV) {
      Trk::BoundaryTrackParameters boundPar = extrapolateInAlignableTV(cache,*currPar, timeLim, dir, particle, nextGeoID,
                                                                       alignTV);
      const Trk::TrackParameters *aPar = boundPar.trPar;
      if (!aPar) {
        return returnParameters;
      }
      throwIntoGarbageBin(cache,aPar);
      // cache.m_currentStatic = boundPar.exitVol;
      return extrapolateToVolumeWithPathLimit(cache,*aPar, timeLim, dir, particle, nextGeoID, destVol);
    }
  }

  // update if new static volume
  if (updateStatic) {    // retrieve boundaries
    cache.m_staticBoundaries.clear();
    const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > > &bounds =
      cache.m_currentStatic->boundarySurfaces();
    for (unsigned int ib = 0; ib < bounds.size(); ib++) {
      const Trk::Surface &surf = (bounds[ib].get())->surfaceRepresentation();
      cache.m_staticBoundaries.emplace_back(&surf, true);
    }

    cache.m_detachedVols.clear();
    cache.m_detachedBoundaries.clear();
    cache.m_denseVols.clear();
    cache.m_denseBoundaries.clear();
    cache.m_layers.clear();
    cache.m_navigLays.clear();

    // new: ID volumes may have special material layers ( entry layers ) - add them here
    // if (cache.m_currentStatic->entryLayerProvider()) {
    //  const std::vector<const Trk::Layer*>& entryLays = cache.m_currentStatic->entryLayerProvider()->layers();
    //  for (unsigned int i=0; i < entryLays.size(); i++) {
    //	if (entryLays[i]->layerType()>0 || entryLays[i]->layerMaterialProperties()) {
    //	  cache.m_layers.push_back(std::pair<const
    // Trk::Surface*,Trk::BoundaryCheck>(&(entryLays[i]->surfaceRepresentation()),true));
    //	  cache.m_navigLays.push_back(std::pair<const Trk::TrackingVolume*,const Trk::Layer*> (cache.m_currentStatic,entryLays[i])
    // );
    //	  Trk::DistanceSolution distSol = cache.m_layers.back().first->straightLineDistanceEstimate(currPar->position(),
    //  
    //   
    //   
    //                                                                                currPar->momentum().normalized());
    //	}
    // }
    // }

    // detached volume boundaries
    const std::vector<const Trk::DetachedTrackingVolume *> *detVols = cache.m_currentStatic->confinedDetachedVolumes();
    if (detVols) {
      std::vector<const Trk::DetachedTrackingVolume *>::const_iterator iTer = detVols->begin();
      for (; iTer != detVols->end(); ++iTer) {
        // active station ?
        const Trk::Layer *layR = (*iTer)->layerRepresentation();
        bool active = layR && layR->layerType();
        const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > >  &detBounds =
          (*iTer)->trackingVolume()->boundarySurfaces();
        if (active) {
          cache.m_detachedVols.emplace_back(*iTer,
                                                                                                 detBounds.size());
          for (unsigned int ibb = 0; ibb < detBounds.size(); ibb++) {
            const Trk::Surface &surf = (detBounds[ibb].get())->surfaceRepresentation();
            cache.m_detachedBoundaries.emplace_back(&surf, true);
          }
        } else if (cache.m_currentStatic->geometrySignature() != Trk::MS ||
                   !m_useMuonMatApprox || (*iTer)->name().substr((*iTer)->name().size() - 4, 4) == "PERM") {  // retrieve
                                                                                                              // inert
                                                                                                              // detached
                                                                                                              // objects
                                                                                                              // only if
                                                                                                              // needed
          if ((*iTer)->trackingVolume()->zOverAtimesRho() != 0. &&
              (!(*iTer)->trackingVolume()->confinedDenseVolumes() ||
               (*iTer)->trackingVolume()->confinedDenseVolumes()->empty())
              && (!(*iTer)->trackingVolume()->confinedArbitraryLayers() ||
                  (*iTer)->trackingVolume()->confinedArbitraryLayers()->empty())) {
            cache.m_denseVols.emplace_back((*iTer)->trackingVolume(), detBounds.size());
            for (unsigned int ibb = 0; ibb < detBounds.size(); ibb++) {
              const Trk::Surface &surf = (detBounds[ibb].get())->surfaceRepresentation();
              cache.m_denseBoundaries.emplace_back(&surf, true);
            }
          }
          const std::vector<const Trk::Layer *> *confLays = (*iTer)->trackingVolume()->confinedArbitraryLayers();
          if ((*iTer)->trackingVolume()->confinedDenseVolumes() || (confLays && confLays->size() > detBounds.size())) {
            cache.m_detachedVols.emplace_back(*iTer,
                                                                                                  detBounds.size());
            for (unsigned int ibb = 0; ibb < detBounds.size(); ibb++) {
              const Trk::Surface &surf = (detBounds[ibb].get())->surfaceRepresentation();
              cache.m_detachedBoundaries.emplace_back(&surf, true);
            }
          } else if (confLays) {
            std::vector<const Trk::Layer *>::const_iterator lIt = confLays->begin();
            for (; lIt != confLays->end(); ++lIt) {
              cache.m_layers.emplace_back(&((*lIt)->surfaceRepresentation()),
                                                                                     true);
              cache.m_navigLays.emplace_back((*iTer)->trackingVolume(), *lIt);
            }
          }
        }
      }
    }
    cache.m_denseResolved = std::pair<unsigned int, unsigned int> (cache.m_denseVols.size(), cache.m_denseBoundaries.size());
    cache.m_layerResolved = cache.m_layers.size();
  }

  cache.m_navigSurfs.insert(cache.m_navigSurfs.end(), cache.m_staticBoundaries.begin(), cache.m_staticBoundaries.end());

  // resolve the use of dense volumes
  cache.m_dense = (cache.m_currentStatic->geometrySignature() == Trk::MS && m_useMuonMatApprox) ||
            (cache.m_currentStatic->geometrySignature() != Trk::MS && m_useDenseVolumeDescription);

  // reset remaining counters
  cache.m_currentDense = cache.m_dense ?  cache.m_currentStatic : cache.m_highestVolume;
  cache.m_navigBoundaries.clear();
  if (cache.m_denseVols.size() > cache.m_denseResolved.first) {
    cache.m_denseVols.resize(cache.m_denseResolved.first);
  }
  while (cache.m_denseBoundaries.size() > cache.m_denseResolved.second) {
    cache.m_denseBoundaries.pop_back();
  }
  if (cache.m_layers.size() > cache.m_layerResolved) {
    cache.m_navigLays.resize(cache.m_layerResolved);
  }
  while (cache.m_layers.size() > cache.m_layerResolved) {
    cache.m_layers.pop_back();
  }

  // current detached volumes
  // collect : subvolume boundaries, ordered/unordered layers, confined dense volumes
  //////////////////////////////////////////////////////
  // const Trk::DetachedTrackingVolume* currentActive = 0;
  std::vector<std::pair<const Trk::TrackingVolume *, unsigned int> > navigVols;

  gp = currPar->position();
  std::vector<const Trk::DetachedTrackingVolume *> *detVols =
    m_navigator->trackingGeometry()->lowestDetachedTrackingVolumes(gp);
  std::vector<const Trk::DetachedTrackingVolume *>::iterator dIter = detVols->begin();
  for (; dIter != detVols->end(); ++dIter) {
    const Trk::Layer *layR = (*dIter)->layerRepresentation();
    bool active = layR && layR->layerType();
    if (active && !m_resolveActive) {
      continue;
    }
    if (!active && cache.m_currentStatic->geometrySignature() == Trk::MS &&
        m_useMuonMatApprox && (*dIter)->name().substr((*dIter)->name().size() - 4, 4) != "PERM") {
      continue;
    }
    const Trk::TrackingVolume *dVol = (*dIter)->trackingVolume();
    // detached volume exit ?
    bool dExit = m_navigator->atVolumeBoundary(currPar, dVol, dir, nextVol, m_tolerance) && !nextVol;
    if (dExit) {
      continue;
    }
    // inert material
    const std::vector<const Trk::TrackingVolume *> *confinedDense = dVol->confinedDenseVolumes();
    const std::vector<const Trk::Layer *> *confinedLays = dVol->confinedArbitraryLayers();

    if (!active && !confinedDense && !confinedLays) {
      continue;
    }
    const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > > &bounds = dVol->boundarySurfaces();
    if (!active && !confinedDense && confinedLays->size() <= bounds.size()) {
      continue;
    }
    if (confinedDense || confinedLays) {
      navigVols.emplace_back(dVol, bounds.size());
      for (unsigned int ib = 0; ib < bounds.size(); ib++) {
        const Trk::Surface &surf = (bounds[ib].get())->surfaceRepresentation();
        cache.m_navigBoundaries.emplace_back(&surf, true);
      }
      // collect dense volume boundary
      if (confinedDense) {
        std::vector<const Trk::TrackingVolume *>::const_iterator vIter = confinedDense->begin();
        for (; vIter != confinedDense->end(); ++vIter) {
          const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > > &bounds =
            (*vIter)->boundarySurfaces();
          cache.m_denseVols.emplace_back(*vIter, bounds.size());
          for (unsigned int ib = 0; ib < bounds.size(); ib++) {
            const Trk::Surface &surf = (bounds[ib].get())->surfaceRepresentation();
            cache.m_denseBoundaries.emplace_back(&surf, true);
          }
        }
      }
      // collect unordered layers
      if (confinedLays) {
        for (unsigned int il = 0; il < confinedLays->size(); il++) {
          cache.m_layers.emplace_back(&((*confinedLays)[il]->surfaceRepresentation()), true);
          cache.m_navigLays.emplace_back(dVol, (*confinedLays)[il]);
        }
      }
    } else {   // active material
      const Trk::TrackingVolume *detVol = dVol->associatedSubVolume(gp);
      if (!detVol && dVol->confinedVolumes()) {
        std::vector<const Trk::TrackingVolume *> subvols = dVol->confinedVolumes()->arrayObjects();
        for (unsigned int iv = 0; iv < subvols.size(); iv++) {
          if (subvols[iv]->inside(gp, m_tolerance)) {
            detVol = subvols[iv];
            break;
          }
        }
      }

      if (!detVol) {
        detVol = dVol;
      }
      bool vExit = m_navigator->atVolumeBoundary(currPar, detVol, dir, nextVol, m_tolerance) && nextVol != detVol;
      if (vExit && nextVol && nextVol->inside(gp, m_tolerance)) {
        detVol = nextVol;
        vExit = false;
      }
      if (!vExit) {
        const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > > &bounds = detVol->boundarySurfaces();
        navigVols.emplace_back(detVol, bounds.size());
        for (unsigned int ib = 0; ib < bounds.size(); ib++) {
          const Trk::Surface &surf = (bounds[ib].get())->surfaceRepresentation();
          cache.m_navigBoundaries.emplace_back(&surf, true);
        }
        if (detVol->zOverAtimesRho() != 0.) {
          cache.m_denseVols.emplace_back(detVol, bounds.size());
          for (unsigned int ib = 0; ib < bounds.size(); ib++) {
            const Trk::Surface &surf = (bounds[ib].get())->surfaceRepresentation();
            cache.m_denseBoundaries.emplace_back(&surf, true);
          }
        }
        // layers ?
        if (detVol->confinedLayers()) {
          if (m_robustSampling || cache.m_currentStatic->geometrySignature() == Trk::MS) {
            std::vector<const Trk::Layer *> cLays = detVol->confinedLayers()->arrayObjects();
            for (unsigned int i = 0; i < cLays.size(); i++) {
              if (cLays[i]->layerType() > 0 || cLays[i]->layerMaterialProperties()) {
                cache.m_layers.emplace_back(&(cLays[i]->surfaceRepresentation()), true);
                cache.m_navigLays.emplace_back(cache.m_currentStatic,
                                                                                                  cLays[i]);
              }
            }
          } else {
            const Trk::Layer *lay = detVol->associatedLayer(gp);
            // if (lay && ( (*dIter)->layerRepresentation()
            //		 &&(*dIter)->layerRepresentation()->layerType()>0 ) ) currentActive=(*dIter);
            if (lay) {
              cache.m_layers.emplace_back(&(lay->surfaceRepresentation()),
                                                                                     true);
              cache.m_navigLays.emplace_back(detVol, lay);
            }
            const Trk::Layer *nextLayer = detVol->nextLayer(currPar->position(),
                                                            dir * currPar->momentum().normalized(), true);
            if (nextLayer && nextLayer != lay) {
              cache.m_layers.emplace_back(&(nextLayer->surfaceRepresentation()), true);
              cache.m_navigLays.emplace_back(detVol, nextLayer);
            }
          }
        } else if (detVol->confinedArbitraryLayers()) {
          const std::vector<const Trk::Layer *> *layers = detVol->confinedArbitraryLayers();
          for (unsigned int il = 0; il < layers->size(); il++) {
            cache.m_layers.emplace_back(&((*layers)[il]->surfaceRepresentation()), true);
            cache.m_navigLays.emplace_back(detVol, (*layers)[il]);
          }
        }
      }
    }
  }
  delete detVols;

  // confined layers
  if (cache.m_currentStatic->confinedLayers() && updateStatic) {
    // if ( cache.m_currentStatic->confinedLayers() ) {
    if (m_robustSampling || cache.m_currentStatic->geometrySignature() == Trk::MS) {
      std::vector<const Trk::Layer *> cLays = cache.m_currentStatic->confinedLayers()->arrayObjects();
      for (unsigned int i = 0; i < cLays.size(); i++) {
        if (cLays[i]->layerType() > 0 || cLays[i]->layerMaterialProperties()) {
          cache.m_layers.emplace_back(&(cLays[i]->surfaceRepresentation()),
                                                                                 true);
          cache.m_navigLays.emplace_back(cache.m_currentStatic, cLays[i]);
        }
      }
    } else {
      // * this does not work - debug !
      const Trk::Layer *lay = cache.m_currentStatic->associatedLayer(gp);
      // if (!lay) {
      //  lay = cache.m_currentStatic->associatedLayer(gp+m_tolerance*parm.momentum().normalized());
      //	std::cout<<" find input associated layer, second attempt:"<< lay<< std::endl;
      // }
      if (lay) {
        cache.m_layers.emplace_back(&(lay->surfaceRepresentation()),
                                                                               Trk::BoundaryCheck(false));
        cache.m_navigLays.emplace_back(cache.m_currentStatic, lay);
        const Trk::Layer *nextLayer = lay->nextLayer(currPar->position(), dir * currPar->momentum().normalized());
        if (nextLayer && nextLayer != lay) {
          cache.m_layers.emplace_back(&(nextLayer->surfaceRepresentation()),
                                                                                 Trk::BoundaryCheck(false));
          cache.m_navigLays.emplace_back(cache.m_currentStatic,
                                                                                            nextLayer);
        }
        const Trk::Layer *backLayer = lay->nextLayer(currPar->position(), -dir * currPar->momentum().normalized());
        if (backLayer && backLayer != lay) {
          cache.m_layers.emplace_back(&(backLayer->surfaceRepresentation()),
                                                                                 Trk::BoundaryCheck(false));
          cache.m_navigLays.emplace_back(cache.m_currentStatic,
                                                                                            backLayer);
        }
      }
    }
  }

  // cache.m_navigSurfs contains destination surface (if it exists), static volume boundaries
  // complete with TG cache.m_layers/dynamic layers, cache.m_denseBoundaries, cache.m_navigBoundaries, m_detachedBoundaries

  if (!cache.m_layers.empty()) {
    cache.m_navigSurfs.insert(cache.m_navigSurfs.end(), cache.m_layers.begin(), cache.m_layers.end());
  }
  if (!cache.m_denseBoundaries.empty()) {
    cache.m_navigSurfs.insert(cache.m_navigSurfs.end(), cache.m_denseBoundaries.begin(), cache.m_denseBoundaries.end());
  }
  if (!cache.m_navigBoundaries.empty()) {
    cache.m_navigSurfs.insert(cache.m_navigSurfs.end(), cache.m_navigBoundaries.begin(), cache.m_navigBoundaries.end());
  }
  if (!cache.m_detachedBoundaries.empty()) {
    cache.m_navigSurfs.insert(cache.m_navigSurfs.end(), cache.m_detachedBoundaries.begin(), cache.m_detachedBoundaries.end());
  }


  // current dense
  cache.m_currentDense = cache.m_highestVolume;
  if (cache.m_dense && cache.m_denseVols.empty()) {
    cache.m_currentDense = cache.m_currentStatic;
  } else {
    for (unsigned int i = 0; i < cache.m_denseVols.size(); i++) {
      const Trk::TrackingVolume *dVol = cache.m_denseVols[i].first;
      if (dVol->inside(currPar->position(), m_tolerance) && dVol->zOverAtimesRho() != 0.) {
        if (!m_navigator->atVolumeBoundary(currPar, dVol, dir, nextVol, m_tolerance) || nextVol == dVol) {
          cache.m_currentDense = dVol;
        }
      }
    }
  }

  // before propagation, loop over layers and collect hits
  if (cache.m_hitVector) {
    for (unsigned int i = 0; i < cache.m_navigLays.size(); i++) {
      if (cache.m_navigLays[i].second->layerType() > 0 && cache.m_navigLays[i].second->isOnLayer(currPar->position())) {
        if (cache.m_navigLays[i].second->surfaceArray()) {
          // perform the overlap Search on this layer
          ATH_MSG_VERBOSE("  [o] Calling overlapSearch() on input layer.");
          overlapSearch(cache,*m_subPropagators[0], *currPar, *currPar, *cache.m_navigLays[i].second, timeLim.time, dir, true,
                        particle);
        } else {
          ATH_MSG_VERBOSE("  [o] Collecting intersection with active input layer.");
          cache.m_hitVector->push_back(Trk::HitInfo(currPar->clone(), timeLim.time, cache.m_navigLays[i].second->layerType(), 0.));
        }
      } // ------------------------------------------------- Fatras mode off -----------------------------------
    }
  }

  // ready to propagate
  // till: A/ static volume boundary(bcheck=true) , B/ material layer(bcheck=true), C/ destination surface(bcheck=false)
  // update of cache.m_navigSurfs required if I/ entry into new navig volume, II/ exit from currentActive without overlaps

  nextVol = nullptr;
  while (currPar) {
    std::vector<unsigned int> solutions;
    // double time_backup = timeLim.time;
    // double path_backup = cache.m_path.x0Collected;
    ATH_MSG_DEBUG("  [+] Starting propagation at position  " << positionOutput(currPar->position())
                                                             << " (current momentum: " << currPar->momentum().mag() <<
      ")");
    ATH_MSG_DEBUG("  [+] " << cache.m_navigSurfs.size() << " target surfaces in '" << cache.m_currentDense->volumeName() << "'.");      //
                                                                                                                            // verify
                                                                                                                            // that
                                                                                                                            // material
                                                                                                                            // input
                                                                                                                            // makes
                                                                                                                            // sense
    if (!(cache.m_currentDense->inside(currPar->position(), m_tolerance)
          || m_navigator->atVolumeBoundary(currPar, cache.m_currentDense, dir, assocVol, m_tolerance))) {
      cache.m_currentDense = cache.m_highestVolume;
    }
    // const Trk::TrackParameters* nextPar =
    // m_stepPropagator->propagateT(*currPar,cache.m_navigSurfs,dir,*cache.m_currentDense,particle,solutions,cache.m_path,timeLim,true);
    const Trk::TrackParameters *nextPar = m_stepPropagator->propagateT(*currPar, cache.m_navigSurfs, dir, m_fieldProperties,
                                                                       particle, solutions, cache.m_path, timeLim, true,
                                                                       cache.m_currentDense, cache.m_hitVector);
    ATH_MSG_VERBOSE("  [+] Propagation done. ");
    if (nextPar) {
      ATH_MSG_DEBUG("  [+] Position after propagation -   at " << positionOutput(
                      nextPar->position()) << ", timed at " << timeLim.time);
    }

    if (!nextPar) {
      ATH_MSG_DEBUG("  [!] Propagation failed, return 0");
      cache.m_parametersAtBoundary.boundaryInformation(cache.m_currentStatic, nextPar, nextPar);
      return returnParameters;
    }

    throwIntoGarbageBin(cache,nextPar);

    // material update has been done already by the propagator
    if (cache.m_path.x0Max > 0. &&
        ((cache.m_path.process < 100 && cache.m_path.x0Collected >= cache.m_path.x0Max) ||
         (cache.m_path.process > 100 && cache.m_path.l0Collected >= cache.m_path.x0Max))) {
      // trigger presampled interaction, provide material properties if needed
      // process interaction only if creation of secondaries allowed
      if (cache.m_currentStatic->geometrySignature() == Trk::ID || m_caloMsSecondary) {
        const Trk::Material *extMprop = cache.m_path.process > 100 ? cache.m_currentDense : nullptr;

        const Trk::TrackParameters *iPar = m_updators[0]->interact(timeLim.time, nextPar->position(),
                                                                   nextPar->momentum(), particle, cache.m_path.process,
                                                                   extMprop);

        if (!iPar) {
          return returnParameters;
        }

        throwIntoGarbageBin(cache,iPar);
        return extrapolateToVolumeWithPathLimit(cache,*iPar, timeLim, dir, particle, nextGeoID, destVol);
      }    // kill the particle without trace ( some validation info can be included here eventually )
        return returnParameters;
      
    }
    // decay ?
    if (timeLim.tMax > 0. && timeLim.time >= timeLim.tMax) {
      // process interaction only if creation of secondaries allowed
      if (cache.m_currentStatic->geometrySignature() == Trk::ID || m_caloMsSecondary) {
        // trigger presampled interaction
        const Trk::TrackParameters *iPar = m_updators[0]->interact(timeLim.time, nextPar->position(),
                                                                   nextPar->momentum(), particle, timeLim.process);
        if (!iPar) {
          return returnParameters;
        }
        throwIntoGarbageBin(cache,iPar);
        return extrapolateToVolumeWithPathLimit(cache,*iPar, timeLim, dir, particle, nextGeoID, destVol);
      }    // kill the particle without trace ( some validation info can be included here eventually )
        return returnParameters;
      
    }

    // check missing volume boundary
    if (nextPar && !(cache.m_currentDense->inside(nextPar->position(), m_tolerance)
                     || m_navigator->atVolumeBoundary(nextPar, cache.m_currentDense, dir, assocVol, m_tolerance))) {
      ATH_MSG_DEBUG("  [!] ERROR: missing volume boundary for volume" << cache.m_currentDense->volumeName());
    }


    ATH_MSG_DEBUG("  [+] Number of intersection solutions: " << solutions.size());

    unsigned int iSol = 0;
    while (iSol < solutions.size()) {
      if (solutions[iSol] < iDest) {
        return nextPar->clone();
      } if (solutions[iSol] < iDest + cache.m_staticBoundaries.size()) {
        // material attached ?
        const Trk::Layer *mb = cache.m_navigSurfs[solutions[iSol]].first->materialLayer();
        if (mb && m_includeMaterialEffects) {
          if (mb->layerMaterialProperties() && mb->layerMaterialProperties()->fullMaterial(nextPar->position())) {
            const ITimedMatEffUpdator *currentUpdator = subMaterialEffectsUpdator(*cache.m_currentStatic);
            nextPar = currentUpdator ?
                      currentUpdator->update(nextPar, *mb, timeLim, cache.m_path,
                                             cache.m_currentStatic->geometrySignature(), dir, particle) : nextPar;

            if (!nextPar) {
              ATH_MSG_VERBOSE("  [+] Update may have killed neutral track - return.");
              cache.m_parametersAtBoundary.resetBoundaryInformation();
              return returnParameters;
            }
            throwIntoGarbageBin(cache,nextPar);
          } else {    // material layer without material ?
            ATH_MSG_VERBOSE(" boundary layer without material:" << mb->layerIndex());
          }
        }

        // static volume boundary; return to the main loop
        unsigned int index = solutions[iSol] - iDest;

        // use global coordinates to retrieve attached volume (just for static!)
        nextVol = (cache.m_currentStatic->boundarySurfaces())[index].get()->attachedVolume(
          nextPar->position(), nextPar->momentum(), dir);
        // double check the next volume
        if (nextVol && !(nextVol->inside(nextPar->position() + 0.01 * dir * nextPar->momentum().normalized(), 0.))) {
          ATH_MSG_DEBUG(
            "  [!] WARNING: wrongly assigned static volume ?" << cache.m_currentStatic->volumeName() << "->" <<
            nextVol->volumeName());
          nextVol = m_navigator->trackingGeometry()->lowestStaticTrackingVolume(
            nextPar->position() + 0.01 * dir * nextPar->momentum().normalized());
          if (nextVol) {
            ATH_MSG_DEBUG("  new search yields: " << nextVol->volumeName());
          }
        }
        // end double check - to be removed after validation of the geometry gluing
        if (nextVol != cache.m_currentStatic) {
          cache.m_parametersAtBoundary.boundaryInformation(nextVol, nextPar, nextPar);
          ATH_MSG_DEBUG("  [+] StaticVol boundary reached of '" << cache.m_currentStatic->volumeName() << "'.");
          if (m_navigator->atVolumeBoundary(nextPar, cache.m_currentStatic, dir, assocVol,
                                            m_tolerance) && assocVol != nextVol) {
            cache.m_currentDense = cache.m_dense ? nextVol : cache.m_highestVolume;
          }
          // no next volume found --- end of the world
          if (!nextVol) {
            ATH_MSG_DEBUG("  [+] World boundary reached        - at " << positionOutput(
                            nextPar->position()) << ", timed at " << timeLim.time);
            nextGeoID = Trk::GeometrySignature(Trk::Unsigned);
            if (!destVol) {
              return nextPar->clone();
            }
          }
          // next volume found and parameters are at boundary
          if (nextVol /*&& nextPar nextPar is dereferenced anyway */) {
            ATH_MSG_DEBUG("  [+] Crossing to next volume '" << nextVol->volumeName() << "'");
            ATH_MSG_DEBUG("  [+] Crossing position is         - at " << positionOutput(nextPar->position()));
            if (!destVol && cache.m_currentStatic->geometrySignature() != nextVol->geometrySignature()) {
              nextGeoID = nextVol->geometrySignature();
              return nextPar->clone();
            }
          }
          return extrapolateToVolumeWithPathLimit(cache,*nextPar, timeLim, dir, particle, nextGeoID, destVol);
        }
      } else if (solutions[iSol] < iDest + cache.m_staticBoundaries.size() + cache.m_layers.size()) {
        // next layer; don't return passive material layers unless required
        unsigned int index = solutions[iSol] - iDest - cache.m_staticBoundaries.size();
        const Trk::Layer *nextLayer = cache.m_navigLays[index].second;
        // material update ?
        // bool matUp = nextLayer->layerMaterialProperties() && m_includeMaterialEffects &&
        // nextLayer->isOnLayer(nextPar->position());
        bool matUp = nextLayer->fullUpdateMaterialProperties(*nextPar) && m_includeMaterialEffects &&
                     nextLayer->isOnLayer(nextPar->position());
        // identical to last material layer ?
        // if (matUp && nextLayer == cache.m_lastMaterialLayer &&
        //     nextLayer->surfaceRepresentation().type() != Trk::Surface::Cylinder) {
        //   matUp = false;
        // }

        // material update
        const ITimedMatEffUpdator *currentUpdator = subMaterialEffectsUpdator(*cache.m_currentStatic);
        if (matUp) {
          double pIn = nextPar->momentum().mag();
          nextPar = currentUpdator ? currentUpdator->update(nextPar, *nextLayer, timeLim, cache.m_path,
                                                            cache.m_currentStatic->geometrySignature(), dir,
                                                            particle) : nextPar;
          if (!nextPar) {
            ATH_MSG_VERBOSE("  [+] Update may have killed track - return.");
            cache.m_parametersAtBoundary.resetBoundaryInformation();
            return returnParameters;
          } 
            ATH_MSG_VERBOSE(
              " Layer energy loss:" << nextPar->momentum().mag() - pIn << "at position:" << nextPar->position() << ", current momentum:" <<
              nextPar->momentum());
            throwIntoGarbageBin(cache,nextPar);
          
        }
        // active surface intersections ( Fatras hits ...)
        if (cache.m_hitVector && particle != Trk::neutron) {
          if (nextLayer->surfaceArray()) {
            // perform the overlap Search on this layer
            ATH_MSG_VERBOSE("  [o] Calling overlapSearch() on  layer.");
            overlapSearch(cache,*m_subPropagators[0], *currPar, *nextPar, *nextLayer, timeLim.time, dir, true, particle);
          } else if (nextLayer->layerType() > 0 && nextLayer->isOnLayer(nextPar->position())) {
            ATH_MSG_VERBOSE("  [o] Collecting intersection with active layer.");
            cache.m_hitVector->push_back(Trk::HitInfo(nextPar->clone(), timeLim.time, nextLayer->layerType(), 0.));
          }
        } // ------------------------------------------------- Fatras mode off -----------------------------------

        // TODO : debug the retrieval of next layer
        if (!m_robustSampling && cache.m_currentStatic->geometrySignature() != Trk::MS) {
          if (cache.m_navigLays[index].first && cache.m_navigLays[index].first->confinedLayers()) {
            const Trk::Layer *newLayer = nextLayer->nextLayer(nextPar->position(),
                                                              dir * nextPar->momentum().normalized());
            if (newLayer && newLayer != nextLayer) {
              bool found = false;
              int replace = -1;
              for (unsigned int i = 0; i < cache.m_navigLays.size(); i++) {
                if (cache.m_navigLays[i].second == newLayer) {
                  found = true;
                  break;
                }
                if (cache.m_navigLays[i].second != nextLayer) {
                  replace = i;
                }
              }
              if (!found) {
                if (replace > -1) {
                  cache.m_navigLays[replace].second = newLayer;
                  cache.m_navigSurfs[solutions[iSol] + replace - index].first = &(newLayer->surfaceRepresentation());
                } else {
                  // can't insert a surface in middle
                  return extrapolateToVolumeWithPathLimit(cache,*nextPar, timeLim, dir, particle, nextGeoID, destVol);
                }
              }
            }
          }
        }
        currPar = nextPar;
      } else if (solutions[iSol] < iDest + cache.m_staticBoundaries.size() + cache.m_layers.size() + cache.m_denseBoundaries.size()) {
        // dense volume boundary
        unsigned int index = solutions[iSol] - iDest - cache.m_staticBoundaries.size() - cache.m_layers.size();
        std::vector< std::pair<const Trk::TrackingVolume *, unsigned int> >::iterator dIter = cache.m_denseVols.begin();
        while (index >= (*dIter).second && dIter != cache.m_denseVols.end()) {
          index -= (*dIter).second;
          ++dIter;
        }
        if (dIter != cache.m_denseVols.end()) {
          currVol = (*dIter).first;
          nextVol = (currVol->boundarySurfaces())[index].get()->attachedVolume(*nextPar, dir);
          // the boundary orientation is not reliable
          Amg::Vector3D tp = nextPar->position() + 2 * m_tolerance * dir * nextPar->momentum().normalized();
          if (!nextVol || !nextVol->inside(tp, m_tolerance)) {   // search for dense volumes
            cache.m_currentDense = cache.m_highestVolume;
            if (cache.m_dense && cache.m_denseVols.empty()) {
              cache.m_currentDense = cache.m_currentStatic;
            } else {
              for (unsigned int i = 0; i < cache.m_denseVols.size(); i++) {
                const Trk::TrackingVolume *dVol = cache.m_denseVols[i].first;
                if (dVol->inside(tp, m_tolerance) && dVol->zOverAtimesRho() != 0.) {
                  cache.m_currentDense = dVol;
                  ATH_MSG_DEBUG("  [+] Next dense volume found: '" << cache.m_currentDense->volumeName() << "'.");
                  break;
                }
              } // loop over dense volumes
            }
          } else {
            cache.m_currentDense = nextVol;
            ATH_MSG_DEBUG("  [+] Next dense volume: '" << cache.m_currentDense->volumeName() << "'.");
          }
        }
      } else if (solutions[iSol] < iDest + cache.m_staticBoundaries.size() + cache.m_layers.size() + cache.m_denseBoundaries.size()
                 + cache.m_navigBoundaries.size()) {
        // navig volume boundary
        unsigned int index = solutions[iSol] - iDest - cache.m_staticBoundaries.size() - cache.m_layers.size() -
                             cache.m_denseBoundaries.size();
        std::vector< std::pair<const Trk::TrackingVolume *, unsigned int> >::iterator nIter = navigVols.begin();
        while (nIter != navigVols.end() && index >= (*nIter).second) {
          index -= (*nIter).second;
          ++nIter;
        }
        if (nIter != navigVols.end()) {
          currVol = (*nIter).first;
          nextVol = ((*nIter).first->boundarySurfaces())[index].get()->attachedVolume(*nextPar, dir);
          if (!nextVol) {
            ATH_MSG_DEBUG("  [+] Navigation volume boundary, leaving volume '"
                          << currVol->volumeName() << "'.");
          } else {
            ATH_MSG_DEBUG("  [+] Navigation volume boundary, entering volume '" << nextVol->volumeName() << "'.");
          }
          currPar = nextPar;
          // return only if detached volume boundaries not collected
          // if ( nextVol || !detachedBoundariesIncluded )
          if (nextVol) {
            return extrapolateToVolumeWithPathLimit(cache,*currPar, timeLim, dir, particle, nextGeoID, destVol);
          }
        }
      } else if (solutions[iSol] < iDest + cache.m_staticBoundaries.size() + cache.m_layers.size() + cache.m_denseBoundaries.size()
                 + cache.m_navigBoundaries.size() + cache.m_detachedBoundaries.size()) {
        // detached volume boundary
        unsigned int index = solutions[iSol] - iDest - cache.m_staticBoundaries.size() - cache.m_layers.size()
                             - cache.m_denseBoundaries.size() - cache.m_navigBoundaries.size();
        std::vector< std::pair<const Trk::DetachedTrackingVolume *,
                               unsigned int> >::iterator dIter = cache.m_detachedVols.begin();
        while (index >= (*dIter).second && dIter != cache.m_detachedVols.end()) {
          index -= (*dIter).second;
          ++dIter;
        }
        if (dIter != cache.m_detachedVols.end()) {
          currVol = (*dIter).first->trackingVolume();
          nextVol =
            ((*dIter).first->trackingVolume()->boundarySurfaces())[index].get()->attachedVolume(*nextPar, dir);
          if (!nextVol) {
            ATH_MSG_DEBUG("  [+] Detached volume boundary, leaving volume '" << currVol->volumeName() << "'.");
          } else {
            ATH_MSG_DEBUG("  [+] Detached volume boundary, entering volume '" << nextVol->volumeName() << "'.");
          }
          currPar = nextPar;
          // if ( nextVol || !detachedBoundariesIncluded)
          if (nextVol) {
             return extrapolateToVolumeWithPathLimit(cache, *currPar, timeLim, dir, particle, nextGeoID, destVol);
          }
        }
      }
      iSol++;
    }
    currPar = nextPar;
  }

  return returnParameters;
}

void
Trk::TimedExtrapolator::overlapSearch(Trk::TimedExtrapolator::Cache &cache,
                                      const IPropagator &prop,
                                      const TrackParameters &parm,
                                      const TrackParameters &parsOnLayer,
                                      const Layer &lay,
                                      // const TrackingVolume& tvol,
                                      float time,
                                      PropDirection dir,
                                      const BoundaryCheck& bcheck, // bcheck
                                      ParticleHypothesis particle,
                                      bool startingLayer) const {
  // indicate destination layer
  bool isDestinationLayer = false;
  // start and end surface for on-layer navigation
  //  -> take the start surface if ther parameter surface is owned by detector element
  const Trk::Surface *startSurface = ((parm.associatedSurface()).associatedDetectorElement() && startingLayer) ?
                                     &parm.associatedSurface() : nullptr;
  const Trk::Surface *endSurface = nullptr;
  // - the best detSurface to start from is the one associated to the detector element
  const Trk::Surface *detSurface = (parsOnLayer.associatedSurface()).associatedDetectorElement() ?
                                   &parsOnLayer.associatedSurface() : nullptr;

  ATH_MSG_VERBOSE("  [o] OverlapSearch called " << (startSurface ? "with " : "w/o ") << "start, "
                                                << (endSurface ? "with " : "w/o ") << "end surface.");

  if (!detSurface) {
    // of parsOnLayer are different from parm, then local position is safe, because the extrapolation
    //   to the detector surface has been done !
    detSurface = isDestinationLayer ? lay.subSurface(parsOnLayer.localPosition()) : lay.subSurface(
      parsOnLayer.position());
    if (detSurface) {
      ATH_MSG_VERBOSE("  [o] Detector surface found through subSurface() call");
    } else {
      ATH_MSG_VERBOSE("  [o] No Detector surface found on this layer.");
    }
  } else {
    ATH_MSG_VERBOSE("  [o] Detector surface found through parameter on layer association");
  }

  // indicate the start layer
  bool isStartLayer = (detSurface && detSurface == startSurface);

  const Trk::TrackParameters *detParameters = nullptr;
  // the temporary vector (might have to be ordered)
  std::vector<const Trk::TrackParameters *> detParametersOnLayer;
  bool reorderDetParametersOnLayer = false;
  // the first test for the detector surface to be hit (false test)
  // - only do this if the parameters aren't on the surface
  // (i.e. search on the start layer or end layer)
  if (isDestinationLayer) {
    detParameters = (&parsOnLayer);
  } else if (isStartLayer) {
    detParameters = (&parm);
  } else if (detSurface) {
    // detParameters = prop.propagate(parm, *detSurface, dir, false, tvol, particle);
    detParameters = prop.propagate(parm, *detSurface, dir, false, m_fieldProperties, particle);
  }

  // set the surface hit to true, it is anyway overruled
  bool surfaceHit = true;
  if (detParameters &&
      !isStartLayer &&
      !isDestinationLayer) {
    ATH_MSG_VERBOSE("  [o] First intersection with Detector surface: " << *detParameters);
    // for the later use in the overlapSearch
    surfaceHit = detParameters && detSurface ? detSurface->isOnSurface(detParameters->position()) : 0; // ,bcheck) -
                                                                                                       // creates
                                                                                                       // problems on
                                                                                                       // start layer;
    // check also for start/endSurface on this level

    surfaceHit = (surfaceHit && startSurface) ?
                 ((detParameters->position() - parm.position()).dot(dir * parm.momentum().normalized()) >
                  0) : surfaceHit;
    surfaceHit = (surfaceHit && endSurface) ?
                 ((detParameters->position() - parsOnLayer.position()).dot(dir * parsOnLayer.momentum().normalized()) <
                  0) : surfaceHit;

    // surface is hit within bounds (or at least with given boundary check directive) -> it counts
    // surface hit also survived start/endsurface search
    //
    // Convention for Fatras: always apply the full update on the last parameters
    //                        of the gathered vector (no pre/post schema)
    // don't record a hit on the destination surface
    if (surfaceHit &&
        detSurface != startSurface) {
      ATH_MSG_VERBOSE("  [H] Hit with detector surface recorded ! ");
      // push into the temporary vector
      detParametersOnLayer.push_back(detParameters);
    } else if (detParameters) {
      // no hit -> fill into the garbage bin
      ATH_MSG_VERBOSE("  [-] Detector surface hit cancelled through bounds check or start/end surface check.");
      throwIntoGarbageBin(cache,detParameters);
    }
  }

  // search for the overlap ------------------------------------------------------------------------
  if (detParameters) {
    // retrive compatible subsurfaces
    std::vector<Trk::SurfaceIntersection> cSurfaces;
    size_t ncSurfaces = lay.compatibleSurfaces(cSurfaces, *detParameters, Trk::anyDirection, bcheck, false);

    // import from StaticEngine.icc
    if (ncSurfaces) {
      ATH_MSG_VERBOSE("found " << ncSurfaces << " candidate sensitive surfaces to test.");
      // now loop over the surfaces:
      // the surfaces will be sorted @TODO integrate pathLength propagation into this
      for (auto &csf : cSurfaces) {
        // propagate to the compatible surface, return types are (pathLimit failure is excluded by Trk::anyDirection for
        // the moment):
        const Trk::TrackParameters *overlapParameters = prop.propagate(parm,
                                                                       *(csf.object),
                                                                       Trk::anyDirection,
                                                                       true,
                                                                       m_fieldProperties,
                                                                       particle);

        if (overlapParameters) {
          ATH_MSG_VERBOSE("  [+] Overlap surface was hit, checking start/end surface condition.");
          // check on start / end surface for on-layer navigaiton action
          surfaceHit = (startSurface) ?
                       ((overlapParameters->position() - parm.position()).dot(dir * parm.momentum().normalized()) >
                        0) : true;
          surfaceHit = (surfaceHit && endSurface) ?
                       ((overlapParameters->position() - parsOnLayer.position()).dot(dir *
                                                                                     parsOnLayer.momentum().normalized())
                        < 0) : surfaceHit;
          if (surfaceHit && csf.object!=detSurface) { //skipping the initial surface on which a hit has already been created
            ATH_MSG_VERBOSE("  [H] Hit with detector surface recorded !");
            // distinguish whether sorting is needed or not
            reorderDetParametersOnLayer = true;
            // push back into the temporary vector
            detParametersOnLayer.push_back(overlapParameters);
          } else { // the parameters have been cancelled by start/end surface
            // no hit -> fill into the garbage bin
            ATH_MSG_VERBOSE("  [-] Detector surface hit cancelled through start/end surface check.");
            throwIntoGarbageBin(cache,overlapParameters);
          }
        }
      } // loop over test surfaces done
    } // there are compatible surfaces
  }  // ---------------------------------------------------------------------------------------------

  // push them into the parameters vector
  std::vector<const Trk::TrackParameters *>::const_iterator parsOnLayerIter = detParametersOnLayer.begin();
  std::vector<const Trk::TrackParameters *>::const_iterator parsOnLayerIterEnd = detParametersOnLayer.end();

  // reorder the track parameters if neccessary, the overlap descriptor did not provide the ordered surfaces
  if (reorderDetParametersOnLayer) {
    // sort to reference of incoming parameters
    Trk::TrkParametersComparisonFunction parameterSorter(parm.position());
    sort(detParametersOnLayer.begin(), detParametersOnLayer.end(), parameterSorter);
  }

  // after sorting : reset the iterators
  parsOnLayerIter = detParametersOnLayer.begin();
  parsOnLayerIterEnd = detParametersOnLayer.end();
  // now fill them into the parameter vector -------> hit creation done <----------------------
  for (; parsOnLayerIter != parsOnLayerIterEnd; ++parsOnLayerIter) {
    if (cache.m_hitVector) {
      cache.m_hitVector->push_back(Trk::HitInfo(*parsOnLayerIter, time, 0, 0.));
    }
  }
}

std::string
Trk::TimedExtrapolator::positionOutput(const Amg::Vector3D &pos) const {
  std::stringstream outStream;

  if (m_printRzOutput) {
    outStream << "[r,phi,z] = [ " << pos.perp() << ", " << pos.phi() << ", " << pos.z() << " ]";
  } else {
    outStream << "[xyz] = [ " << pos.x() << ", " << pos.y() << ", " << pos.z() << " ]";
  }
  return outStream.str();
}

std::string
Trk::TimedExtrapolator::momentumOutput(const Amg::Vector3D &mom) const {
  std::stringstream outStream;

  outStream << "[eta,phi] = [ " << mom.eta() << ", " << mom.phi() << " ]";
  return outStream.str();
}

void
Trk::TimedExtrapolator::emptyGarbageBin(Trk::TimedExtrapolator::Cache &cache,
                                        const Trk::TrackParameters *trPar) const {
  // empty the garbage
  std::map<const Trk::TrackParameters *, bool>::iterator garbageIter = cache.m_garbageBin.begin();
  std::map<const Trk::TrackParameters *, bool>::iterator garbageEnd = cache.m_garbageBin.end();

  bool throwCurrent = false;

  for (; garbageIter != garbageEnd; ++garbageIter) {
    if (garbageIter->first && garbageIter->first != trPar) {
      delete (garbageIter->first);
    }
    if (garbageIter->first && garbageIter->first == trPar) {
      throwCurrent = true;
    }
  }

  cache.m_garbageBin.clear();
  if (throwCurrent) {
     throwIntoGarbageBin(cache,trPar);
  }
}

// the validation action -> propagated to the SubTools
void
Trk::TimedExtrapolator::validationAction() const {
  // record the updator validation information
  for (unsigned int imueot = 0; imueot < m_subUpdators.size(); ++imueot) {
    m_subUpdators[imueot]->validationAction();
  }
  // record the navigator validation information
  m_navigator->validationAction();
}

const Trk::TrackParameters *
Trk::TimedExtrapolator::transportNeutralsWithPathLimit(const Trk::TrackParameters &parm,
                                                       Trk::PathLimit &pathLim, Trk::TimeLimit &timeLim,
                                                       Trk::PropDirection dir,
                                                       Trk::ParticleHypothesis particle,
                                                       std::vector<Trk::HitInfo> * &hitInfo,
                                                       Trk::GeometrySignature &nextGeoID,
                                                       const Trk::TrackingVolume *boundaryVol) const {
   Trk::TimedExtrapolator::Cache cache(m_maxNavigSurf);
// extrapolation method intended for simulation of particle decay; collects the material up to pre-defined limit and
// triggers
// material interaction
// possible outcomes:1/ returns curvilinear parameters after reaching the maximal path (if to be destroyed)
//                   2/ returns parameters at destination volume boundary
//                   3/ returns 0 ( particle stopped ) but keeps material and timing info

  ATH_MSG_DEBUG(
    "M-[" << ++cache.m_methodSequence << "] transportNeutralsWithPathLimit(...) " << pathLim.x0Max << ", from " <<
    parm.position());

  // reset the path ( in x0 !!)
  cache.m_path = PathLimit(pathLim.x0Max - pathLim.x0Collected, pathLim.process);     // collect material locally

  // initialize time info
  cache.m_time = timeLim.time;

  // initialize hit vector
  cache.m_hitVector = hitInfo;

  cache.m_parametersAtBoundary.resetBoundaryInformation();

  // if no input volume, define as highest volume
  // const Trk::TrackingVolume* destVolume = boundaryVol ? boundaryVol : m_navigator->highestVolume();
  cache.m_currentStatic = nullptr;
  if (boundaryVol && !boundaryVol->inside(parm.position(), m_tolerance)) {
    return nullptr;
  }

  cache.m_particleMass = s_particleMasses.mass[particle];

  // extrapolate to destination volume boundary with path limit
  const Trk::TrackParameters *returnParms = transportToVolumeWithPathLimit(cache,parm, timeLim, dir, particle, nextGeoID,
                                                                           boundaryVol);

  // save actual path on output
  if (cache.m_path.x0Collected > 0.) {
    pathLim.updateMat(cache.m_path.x0Collected, cache.m_path.weightedZ / cache.m_path.x0Collected, cache.m_path.l0Collected);
  }

  // return timing
  timeLim.time = cache.m_time;

  std::map<const Trk::TrackParameters *, bool>::iterator garbageIter = cache.m_garbageBin.begin();
  std::map<const Trk::TrackParameters *, bool>::iterator garbageEnd = cache.m_garbageBin.end();
  for (; garbageIter != garbageEnd; ++garbageIter) if (garbageIter->first) {
    if(garbageIter->first == returnParms) {
      auto ret=returnParms->clone();
      ATH_MSG_DEBUG("  [+] garbage - at " << positionOutput(garbageIter->first->position())<<" parm="<<garbageIter->first<<" is the return param. Cloning to"<<ret);
      returnParms=ret;
    }
  }

  return returnParms;
}

const Trk::TrackParameters *
Trk::TimedExtrapolator::transportToVolumeWithPathLimit(Trk::TimedExtrapolator::Cache &cache,
                                                       const Trk::TrackParameters &parm,
                                                       Trk::TimeLimit &timeLim,
                                                       Trk::PropDirection dir,
                                                       Trk::ParticleHypothesis particle,
                                                       Trk::GeometrySignature &nextGeoID,
                                                       const Trk::TrackingVolume *destVol) const {
  // returns:
  //    A)  curvilinear track parameters if path or time limit reached
  //    B)  boundary parameters (at destination volume boundary)

  // initialize the return parameters vector
  const Trk::TrackParameters *returnParameters = nullptr;
  const Trk::TrackParameters *currPar = &parm;
  const Trk::TrackingVolume *currVol = nullptr;
  const Trk::TrackingVolume *nextVol = nullptr;
  const Trk::TrackingVolume *assocVol = nullptr;
  // int                        nEntryLays = 0;
  unsigned int iDest = 0;

  // destination volume boundary ?
  if (destVol && m_navigator->atVolumeBoundary(currPar, destVol, dir, nextVol, m_tolerance) && nextVol != destVol) {
    return &parm;
  }

  // bool resolveActive = m_resolveActive;
  if (!cache.m_highestVolume) {
    cache.m_highestVolume = m_navigator->highestVolume();
  }

  emptyGarbageBin(cache,&parm);
  // transport surfaces:  collect only those with valid intersection (easy to calculate for neutrals)
  if (cache.m_trSurfs.capacity() > m_maxNavigSurf) {
    cache.m_trSurfs.reserve(m_maxNavigSurf);
  }
  cache.m_trSurfs.clear();

  // target volume may not be part of tracking geometry
  if (destVol) {
    const Trk::TrackingVolume *tgVol = m_navigator->trackingGeometry()->trackingVolume(destVol->volumeName());
    if (!tgVol || tgVol != destVol) {
      const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > >& bounds = destVol->boundarySurfaces();
      for (unsigned int ib = 0; ib < bounds.size(); ib++) {
        const Trk::Surface &surf = (bounds[ib].get())->surfaceRepresentation();
        Trk::DistanceSolution distSol = surf.straightLineDistanceEstimate(currPar->position(),
                                                                          dir * currPar->momentum().normalized());
        if (distSol.numberOfSolutions() > 0 && distSol.first() > 0.) {
          // boundary check
          Amg::Vector3D gp = currPar->position() + distSol.first() * dir * currPar->momentum().normalized();
          if (surf.isOnSurface(gp, true, 0.001, 0.001)) {
            iDest++;
            cache.m_trSurfs.emplace_back(&surf, distSol.first());
          }   // valid intersection
        }  // along path
        if (distSol.numberOfSolutions() > 1 && distSol.second() > 0.) {
          // boundary check
          Amg::Vector3D gp = currPar->position() + distSol.second() * dir * currPar->momentum().normalized();
          if (surf.isOnSurface(gp, true, 0.001, 0.001)) {
            iDest++;
            cache.m_trSurfs.emplace_back(&surf, distSol.second());
          }   // valid intersection
        }
      } // end loop over boundaries
    } // end process external volume
  }

  // resolve current position
  if (cache.m_parametersAtBoundary.nextParameters == currPar) {
    cache.m_currentStatic = cache.m_parametersAtBoundary.nextVolume;
  } else {
    const Amg::Vector3D& gp = parm.position();
    if (!cache.m_currentStatic || !cache.m_currentStatic->inside(gp, m_tolerance)) {
      cache.m_currentStatic = m_navigator->trackingGeometry()->lowestStaticTrackingVolume(gp);

      if (!cache.m_currentStatic ||
          !cache.m_currentStatic->inside(currPar->position() + 0.01 * dir * currPar->momentum().normalized(), 0.)) {
        cache.m_currentStatic = m_navigator->trackingGeometry()->lowestStaticTrackingVolume(currPar->position()
                                                                                      + 0.01 * dir *
                                                                                      currPar->momentum().normalized());
      }
    }

    if (!cache.m_currentStatic) {
      // no next volume found --- end of the world
      ATH_MSG_DEBUG("  [+] Word boundary reached        - at " << positionOutput(currPar->position()));
      nextGeoID = Trk::GeometrySignature(Trk::Unsigned);
      return currPar;
    }
  }

  // current frame volume known-retrieve geoID
  nextGeoID = cache.m_currentStatic->geometrySignature();

  // resolve active Calo volumes if hit info required
  if (cache.m_hitVector && nextGeoID == Trk::Calo) {
    const Trk::AlignableTrackingVolume *alignTV = dynamic_cast<const Trk::AlignableTrackingVolume *> (cache.m_currentStatic);
    if (alignTV) {
      const Trk::TrackParameters *aPar = transportInAlignableTV(cache,parm, timeLim, dir, particle, nextGeoID, alignTV).trPar;
      if (!aPar) {
        return returnParameters;
      }
      throwIntoGarbageBin(cache,aPar);
      return transportToVolumeWithPathLimit(cache,*aPar, timeLim, dir, particle, nextGeoID, destVol);
    }
  }

  // distance to static volume boundaries recalculated
  // retrieve boundaries along path
  cache.m_trStaticBounds.clear();
  const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > > &bounds =
    cache.m_currentStatic->boundarySurfaces();
  for (unsigned int ib = 0; ib < bounds.size(); ib++) {
    const Trk::Surface &surf = (bounds[ib].get())->surfaceRepresentation();
    Trk::DistanceSolution distSol = surf.straightLineDistanceEstimate(currPar->position(),
                                                                      dir * currPar->momentum().normalized());
    if (distSol.numberOfSolutions() > 0 &&
        (distSol.currentDistance(false) > m_tolerance || distSol.numberOfSolutions() > 1) &&
        distSol.first() > m_tolerance) {
      double dist = distSol.first();
      // resolve multiple intersection solutions
      if (distSol.numberOfSolutions() > 1 && dist < m_tolerance && distSol.second() > dist) {
        dist = distSol.second();
      }
      // boundary check
      Amg::Vector3D gp = currPar->position() + dist * dir * currPar->momentum().normalized();
      if (surf.isOnSurface(gp, true, m_tolerance, m_tolerance)) {
        cache.m_trStaticBounds.insert(cache.m_trStaticBounds.begin(), Trk::DestBound(&surf, dist, ib));
      }
    }  // along path
    if (distSol.numberOfSolutions() > 1 && distSol.second() > m_tolerance) {
      double dist = distSol.second();
      // boundary check
      Amg::Vector3D gp = currPar->position() + dist * dir * currPar->momentum().unit();
      if (surf.isOnSurface(gp, true, m_tolerance, m_tolerance)) {
        if (dist > m_tolerance) {  // valid intersection
          cache.m_trStaticBounds.insert(cache.m_trStaticBounds.begin(), Trk::DestBound(&surf, dist, ib));
        }
      }
    }  // along path
  } // end loop over boundaries

  if (cache.m_trStaticBounds.empty()) {
    ATH_MSG_WARNING(
      "  transportToVolumeWithPathLimit() - at " << currPar->position() << ", missing static volume boundary "
                                                 << cache.m_currentStatic->volumeName() <<
      ": transport interrupted");

    ATH_MSG_DEBUG(
      "---> particle R,phi,z, momentum:" << currPar->position().perp() << "," << currPar->position().phi() << "," << currPar->position().z() << "," <<
      currPar->momentum());
    ATH_MSG_DEBUG("---> static volume position:" << cache.m_currentStatic->center());
    const Trk::CylinderVolumeBounds *cyl =
      dynamic_cast<const Trk::CylinderVolumeBounds *> (&(cache.m_currentStatic->volumeBounds()));
    if (cyl) {
      ATH_MSG_DEBUG(
        "---> cylinder volume dimensions:" << cyl->innerRadius() << "," << cyl->outerRadius() << "," <<
        cyl->halflengthZ());
    }


    for (unsigned int ib = 0; ib < bounds.size(); ib++) {
      const Trk::Surface &surf = (bounds[ib].get())->surfaceRepresentation();
      Trk::DistanceSolution distSol = surf.straightLineDistanceEstimate(currPar->position(),
                                                                        dir * currPar->momentum().unit());
      ATH_MSG_DEBUG(
        "---> decomposed boundary surface position, normal, estimated distance:" << ib << "," << surf.center() << "," <<
        surf.normal());
      ATH_MSG_DEBUG(
        "---> estimated distance to (first solution):boundary check:" << distSol.numberOfSolutions() << "," << distSol.first() << ":" <<
                    surf.isOnSurface(currPar->position() + distSol.first() * dir * currPar->momentum().unit(), true,
                                     m_tolerance, m_tolerance));
      if (distSol.numberOfSolutions() > 1) {
        ATH_MSG_DEBUG("---> estimated distance to (second solution):boundary check:" << distSol.second() << "," <<
                      surf.isOnSurface(currPar->position() + distSol.second() * dir * currPar->momentum().unit(), true,
                                       m_tolerance, m_tolerance));
      }
    }

    return returnParameters;
  } if (cache.m_trStaticBounds[0].distance < m_tolerance) {
    // TODO find out why this case (=exit from volume) haven't been handled by Navigator
    // ATH_MSG_WARNING( " recovering from glitch at the static volume boundary:"<<cache.m_trStaticBounds[0].distance );

    Amg::Vector3D gp = currPar->position() + m_tolerance * dir * currPar->momentum().unit();
    cache.m_currentStatic = m_navigator->trackingGeometry()->lowestStaticTrackingVolume(gp);

    if (cache.m_currentStatic) {
       return transportToVolumeWithPathLimit(cache,parm, timeLim, dir, particle, nextGeoID, destVol);
    } 
      ATH_MSG_DEBUG("  [+] World boundary reached        - at " << positionOutput(
                      currPar->position()) << ", timed at " << cache.m_time);
      nextGeoID = Trk::GeometrySignature(Trk::Unsigned);
      // if (!destVol) { return currPar;}
      return currPar;
    
  }

  cache.m_detachedVols.clear();
  cache.m_denseVols.clear();
  cache.m_trDenseBounds.clear();
  cache.m_trLays.clear();
  cache.m_navigLays.clear();

  // detached volume boundaries
  const std::vector<const Trk::DetachedTrackingVolume *> *detVols = cache.m_currentStatic->confinedDetachedVolumes();
  if (detVols) {
    std::vector<const Trk::DetachedTrackingVolume *>::const_iterator iTer = detVols->begin();
    for (; iTer != detVols->end(); ++iTer) {
      // active station ?
      const Trk::Layer *layR = (*iTer)->layerRepresentation();
      bool active = layR && layR->layerType();

      if (active) {
        if (!m_resolveMultilayers || !(*iTer)->multilayerRepresentation()) {
          const Trk::Surface &surf = layR->surfaceRepresentation();
          Trk::DistanceSolution distSol = surf.straightLineDistanceEstimate(currPar->position(),
                                                                            dir * currPar->momentum().normalized());
          if (distSol.numberOfSolutions() > 0 && distSol.first() > 0.) {
            // boundary check
            Amg::Vector3D gp = currPar->position() + distSol.first() * dir * currPar->momentum().normalized();
            if (surf.isOnSurface(gp, true, 0.001, 0.001)) {
              cache.m_trLays.emplace_back(&surf, distSol.first());
              cache.m_navigLays.emplace_back((*iTer)->trackingVolume(), layR);
            }
          }
        } else {
          const std::vector<const Trk::Layer *> *multi = (*iTer)->multilayerRepresentation();
          for (unsigned int i = 0; i < multi->size(); i++) {
            const Trk::Surface &surf = (*multi)[i]->surfaceRepresentation();
            Trk::DistanceSolution distSol = surf.straightLineDistanceEstimate(currPar->position(),
                                                                              dir * currPar->momentum().normalized());
            if (distSol.numberOfSolutions() > 0 && distSol.first() > 0.) {
              // boundary check
              Amg::Vector3D gp = currPar->position() + distSol.first() * dir * currPar->momentum().normalized();
              if (surf.isOnSurface(gp, true, 0.001, 0.001)) {
                cache.m_trLays.emplace_back(&surf, distSol.first());
                cache.m_navigLays.emplace_back((*iTer)->trackingVolume(), (*multi)[i]);
              }
            }
          }   // end loop over multilayers
        } // end unresolved active
      } // active done
      else if (cache.m_currentStatic->geometrySignature() != Trk::MS || !m_useMuonMatApprox ||
               (*iTer)->name().substr((*iTer)->name().size() - 4, 4) == "PERM") {  // retrieve inert detached objects
                                                                                   // only if needed
        // dense volume boundaries
        if ((*iTer)->trackingVolume()->zOverAtimesRho() != 0. &&
            (!(*iTer)->trackingVolume()->confinedDenseVolumes() ||
             (*iTer)->trackingVolume()->confinedDenseVolumes()->empty())
            && (!(*iTer)->trackingVolume()->confinedArbitraryLayers() ||
                (*iTer)->trackingVolume()->confinedArbitraryLayers()->empty())) {
          const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > >  &detBounds =
            (*iTer)->trackingVolume()->boundarySurfaces();
          int newB = 0;
          for (unsigned int ibb = 0; ibb < detBounds.size(); ibb++) {
            const Trk::Surface &surf = (detBounds[ibb].get())->surfaceRepresentation();
            Trk::DistanceSolution distSol = surf.straightLineDistanceEstimate(currPar->position(),
                                                                              dir * currPar->momentum().normalized());
            if (distSol.numberOfSolutions() > 0 && distSol.first() > 0.) {
              // boundary check
              Amg::Vector3D gp = currPar->position() + distSol.first() * dir * currPar->momentum().normalized();
              if (surf.isOnSurface(gp, true, 0.001, 0.001)) {
                cache.m_trDenseBounds.emplace_back(&surf, distSol.first());
                newB++;
              }   // valid intersection
            }  // along path
          } // end loop over boundaries
          if (newB > 0) {
            cache.m_denseVols.emplace_back((*iTer)->trackingVolume(), newB);
          }
        }
        // subvolumes ?
        // if ((*iTer)->trackingVolume()->confinedDenseVolumes() &&
        //  (*iTer)->trackingVolume()->confinedDenseVolumes()->size())
        //  ATH_MSG_WARNING( "  transportToVolumeWithPathLimit() - at " << currPar->position() <<", unresolved
        // subvolumes for  "
        //		   << (*iTer)->trackingVolume()->volumeName() );

        const std::vector<const Trk::TrackingVolume *> *confinedDense =
          (*iTer)->trackingVolume()->confinedDenseVolumes();
        if (confinedDense && !confinedDense->empty()) {
          std::vector<const Trk::TrackingVolume *>::const_iterator vIter = confinedDense->begin();
          for (; vIter != confinedDense->end(); ++vIter) {
            const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > >  &bounds =
              (*vIter)->boundarySurfaces();
            int newB = 0;
            for (unsigned int ibb = 0; ibb < bounds.size(); ibb++) {
              const Trk::Surface &surf = (bounds[ibb].get())->surfaceRepresentation();
              Trk::DistanceSolution distSol = surf.straightLineDistanceEstimate(currPar->position(),
                                                                                dir * currPar->momentum().normalized());
              if (distSol.numberOfSolutions() > 0 && distSol.first() > 0.) {
                // boundary check
                Amg::Vector3D gp = currPar->position() + distSol.first() * dir * currPar->momentum().normalized();
                if (surf.isOnSurface(gp, true, 0.001, 0.001)) {
                  cache.m_trDenseBounds.emplace_back(&surf, distSol.first());
                  newB++;
                }   // valid intersection
              }  // along path
            } // end loop over boundaries
            if (newB > 0) {
              cache.m_denseVols.emplace_back((*vIter), newB);
            }
            if ((*vIter)->confinedDenseVolumes() || (*vIter)->confinedArbitraryLayers()) {
              ATH_MSG_DEBUG(
                "  transportToVolumeWithPathLimit() - at " << currPar->position() << ", unresolved sublayers/subvolumes for  "
                                                           << (*vIter)->volumeName());
            }
          }
        }

        // confined layers
        const std::vector<const Trk::Layer *> *confLays = (*iTer)->trackingVolume()->confinedArbitraryLayers();
        if (confLays) {
          std::vector<const Trk::Layer *>::const_iterator lIt = confLays->begin();
          for (; lIt != confLays->end(); ++lIt) {
            const Trk::Surface &surf = (*lIt)->surfaceRepresentation();
            Trk::DistanceSolution distSol = surf.straightLineDistanceEstimate(currPar->position(),
                                                                              dir * currPar->momentum().normalized());
            if (distSol.numberOfSolutions() > 0 && distSol.first() > 0.) {
              // boundary check
              Amg::Vector3D gp = currPar->position() + distSol.first() * dir * currPar->momentum().normalized();
              if (surf.isOnSurface(gp, true, 0.001, 0.001)) {
                cache.m_trLays.emplace_back(&surf, distSol.first());
                cache.m_navigLays.emplace_back((*iTer)->trackingVolume(), *lIt);
              }   // valid intersection
            }  // along path
          }
        }  // end confined layers
      } // end inert material
    }
  } // end detached volumes
  cache.m_denseResolved = std::pair<unsigned int, unsigned int> (cache.m_denseVols.size(), cache.m_trDenseBounds.size());
  cache.m_layerResolved = cache.m_trLays.size();

  std::vector< Trk::DestBound >::iterator bIter = cache.m_trStaticBounds.begin();
  while (bIter != cache.m_trStaticBounds.end()) {
    cache.m_trSurfs.emplace_back((*bIter).surface, (*bIter).distance);
    ++bIter;
  }

  // std::cout <<"navigation in current static:"<< cache.m_trSurfs.size()<<","<<cache.m_trStaticBounds.size()<< std::endl;
  // for (unsigned int ib=0; ib<cache.m_trSurfs.size(); ib++) std::cout <<"distance to static:"<<
  // ib<<","<<cache.m_trSurfs[ib].second<<std::endl;

  // resolve the use of dense volumes
  cache.m_dense = (cache.m_currentStatic->geometrySignature() == Trk::MS && m_useMuonMatApprox) ||
            (cache.m_currentStatic->geometrySignature() != Trk::MS && m_useDenseVolumeDescription);

  // reset remaining counters
  cache.m_currentDense = cache.m_dense ?  cache.m_currentStatic : cache.m_highestVolume;
  cache.m_navigBoundaries.clear();
  if (cache.m_denseVols.size() > cache.m_denseResolved.first) {
    cache.m_denseVols.resize(cache.m_denseResolved.first);
    cache.m_trDenseBounds.resize(cache.m_denseResolved.second);
  }
  if (cache.m_layers.size() > cache.m_layerResolved) {
    cache.m_trLays.resize(cache.m_layerResolved);
    cache.m_navigLays.resize(cache.m_layerResolved);
  }

  // if (cache.m_currentStatic->entryLayerProvider()) nEntryLays = cache.m_currentStatic->entryLayerProvider()->layers().size();

  // confined layers
  if (cache.m_currentStatic->confinedLayers()) {
    std::vector<const Trk::Layer *> cLays = cache.m_currentStatic->confinedLayers()->arrayObjects();
    for (unsigned int i = 0; i < cLays.size(); i++) {
      if (cLays[i]->layerMaterialProperties()) {
        const Trk::Surface &surf = cLays[i]->surfaceRepresentation();
        Trk::DistanceSolution distSol = surf.straightLineDistanceEstimate(currPar->position(),
                                                                          dir * currPar->momentum().normalized());
        if (distSol.numberOfSolutions() > 0 && distSol.first() > 0.) {
          // boundary check
          Amg::Vector3D gp = currPar->position() + distSol.first() * dir * currPar->momentum().normalized();
          if (surf.isOnSurface(gp, true, 0.001, 0.001)) {
            cache.m_trLays.emplace_back(&surf, distSol.first());
            cache.m_navigLays.emplace_back(cache.m_currentStatic,
                                                                                              cLays[i]);
          }   // valid intersection
        }  // along path
      }
    }
  }

  // cache.m_trSurfs contains destination surface (if it exists), static volume boundaries
  // complete with TG cache.m_layers/dynamic layers, cache.m_denseBoundaries, cache.m_navigBoundaries, m_detachedBoundaries

  if (!cache.m_trLays.empty()) {
    cache.m_trSurfs.insert(cache.m_trSurfs.end(), cache.m_trLays.begin(), cache.m_trLays.end());
  }
  if (!cache.m_trDenseBounds.empty()) {
    cache.m_trSurfs.insert(cache.m_trSurfs.end(), cache.m_trDenseBounds.begin(), cache.m_trDenseBounds.end());
  }

  // current dense
  cache.m_currentDense = cache.m_highestVolume;

  for (unsigned int i = 0; i < cache.m_denseVols.size(); i++) {
    const Trk::TrackingVolume *dVol = cache.m_denseVols[i].first;
    if (dVol->inside(currPar->position(), m_tolerance) && dVol->zOverAtimesRho() != 0.) {
      if (!m_navigator->atVolumeBoundary(currPar, dVol, dir, nextVol, m_tolerance) ||
          dVol->inside(currPar->position() + 2 * m_tolerance * currPar->momentum().unit(), m_tolerance)) {
        cache.m_currentDense = dVol;
      }
    }
  }

  if (cache.m_dense && cache.m_currentDense == cache.m_highestVolume) {
    cache.m_currentDense = cache.m_currentStatic;
  }

  // ready to process
  // 1/ order valid intersections ( already in trSurfs )

  std::vector<unsigned int> sols;
  for (unsigned int i = 0; i < cache.m_trSurfs.size(); i++) {
    sols.push_back(i);
  }

  if (sols.size() > 1) {
    unsigned int itest = 1;
    while (itest < sols.size()) {
      if (cache.m_trSurfs[sols[itest]].second < cache.m_trSurfs[sols[itest - 1]].second) {
        unsigned int iex = sols[itest - 1];
        sols[itest - 1] = sols[itest];
        sols[itest] = iex;
        itest = 1;
      } else {
        itest++;
      }
    }
    // check ordering
    for (unsigned int is = 1; is < sols.size(); is++) {
      if (cache.m_trSurfs[sols[is]].second < cache.m_trSurfs[sols[is - 1]].second) {
        std::cout << "wrong intersection ordering" << std::endl;
      }
    }
  }


  // 2/ check time/material/boundary limit

  // update of cache.m_navigSurfs required if I/ entry into new navig volume, II/ exit from currentActive without overlaps

  nextVol = nullptr;
  const Trk::TrackParameters *nextPar = nullptr;

  double dist = 0.;
  double mom = currPar->momentum().mag();
  double beta = mom / sqrt(mom * mom + cache.m_particleMass * cache.m_particleMass) * Gaudi::Units::c_light;

  ATH_MSG_DEBUG("  [0] starting transport of neutral particle in (dense) volume " << cache.m_currentDense->volumeName());

  for (unsigned int is = 0; is < sols.size(); is++) {
    if (cache.m_trSurfs[sols[is]].second == 0.) {
      continue;
    }

    double step = cache.m_trSurfs[sols[is]].second - dist;

    Amg::Vector3D nextPos = currPar->position() + dir * currPar->momentum().normalized() * cache.m_trSurfs[sols[is]].second;
    // Amg::Vector3D halfStep = nextPos - 0.5*step*dir*currPar->momentum().normalized();

    // check missing volume boundary
    if (!(cache.m_currentDense->inside(nextPos, m_tolerance))) {
      ATH_MSG_DEBUG("  [!] WARNING: missing volume boundary for volume" << cache.m_currentDense->volumeName());
      // new search
      cache.m_currentDense = cache.m_highestVolume;
      for (unsigned int i = 0; i < cache.m_denseVols.size(); i++) {
        const Trk::TrackingVolume *dVol = cache.m_denseVols[i].first;
        if (dVol->inside(nextPos, m_tolerance) && dVol->zOverAtimesRho() != 0.) {
          cache.m_currentDense = dVol;
        }
      }
      if (cache.m_dense && cache.m_currentDense == cache.m_highestVolume) {
        cache.m_currentDense = cache.m_currentStatic;
      }

      ATH_MSG_DEBUG("  [!] new search for dense volume : " << cache.m_currentDense->volumeName());
    }

    double tDelta = step / beta;

    double mDelta = (cache.m_currentDense->zOverAtimesRho() != 0.) ? step / cache.m_currentDense->x0() : 0.;

    // in case of hadronic interaction retrieve nuclear interaction properties, too

    double frT = 1.;
    if (step > 0 && timeLim.tMax > cache.m_time && cache.m_time + tDelta >= timeLim.tMax) {
      frT = (timeLim.tMax - cache.m_time) * beta / step;
    }

    // TODO : compare x0 or l0 according to the process type
    double frM = 1.;
    if (mDelta > 0 && cache.m_path.x0Max > 0.) {
      if (cache.m_path.process < 100 && cache.m_path.x0Collected + mDelta > cache.m_path.x0Max) {
        frM = (cache.m_path.x0Max - cache.m_path.x0Collected) / mDelta;
      } else {          // waiting for hadronic interaction,  retrieve nuclear interaction properties
        double mDeltaL = cache.m_currentDense->L0 >
                         0. ? step / cache.m_currentDense->L0 : mDelta / 0.37 / cache.m_currentDense->averageZ();
        if (cache.m_path.l0Collected + mDeltaL > cache.m_path.x0Max) {
          frM = (cache.m_path.x0Max - cache.m_path.l0Collected) / mDeltaL;
        }
      }
    }

    double fr = fmin(frT, frM);

    // std::cout << "looping over intersections:"<<is<<","<< cache.m_trSurfs[sols[is]].second<<","<<step << ","<<
    // tDelta<<","<<mDelta << std::endl;

    if (fr < 1.) { // decay or material interaction during the step
      int process = frT < frM ? timeLim.process : cache.m_path.process;
      cache.m_time += fr * step / beta;
      if (mDelta > 0 && cache.m_currentDense->averageZ() > 0) {
        cache.m_path.updateMat(fr * mDelta, cache.m_currentDense->averageZ(), 0.);
      }

      nextPos = currPar->position() + dir * currPar->momentum().normalized() * (dist + fr * step);

      // process interaction only if creation of secondaries allowed
      if (cache.m_currentStatic->geometrySignature() == Trk::ID || m_caloMsSecondary) {
        const Trk::TrackParameters *nextPar = m_updators[0]->interact(cache.m_time, nextPos,
                                                                      currPar->momentum(), particle, process,
                                                                      cache.m_currentDense);

        if (nextPar) {
          ATH_MSG_DEBUG(" [!] WARNING: particle survives the interaction " << process);
        }

        if (nextPar && process == 121) {
          ATH_MSG_DEBUG(" [!] WARNING: failed hadronic interaction, killing the input particle anyway");
          return returnParameters;
        }

        if (!nextPar) {
          return returnParameters;
        }

        throwIntoGarbageBin(cache,nextPar);
        // return transportToVolumeWithPathLimit(cache,*nextPar, timeLim, dir, particle, nextGeoID, destVol);
      } else {  // kill particle without trace
        return returnParameters;
      }
    }  // end decay or material interaction durign the step

    // update
    dist = cache.m_trSurfs[sols[is]].second;
    if (mDelta > 0 && cache.m_currentDense->averageZ() > 0) {
      cache.m_path.updateMat(mDelta, cache.m_currentDense->averageZ(), 0.);
    }
    cache.m_time += tDelta;

    nextPar = new Trk::CurvilinearParameters(nextPos, currPar->momentum(), 1.);  // fake charge
    throwIntoGarbageBin(cache,nextPar);

    if (sols[is] < iDest) {      // destination volume (most often, subdetector boundary)
      return nextPar->clone();
    } if (sols[is] < iDest + cache.m_trStaticBounds.size()) {     // tracking geometry frame
      // material attached ?
      const Trk::Layer *mb = cache.m_trStaticBounds[sols[is] - iDest].surface->materialLayer();
      if (mb && m_includeMaterialEffects) {
        if (mb->layerMaterialProperties() && mb->layerMaterialProperties()->fullMaterial(nextPos)) {
          const ITimedMatEffUpdator *currentUpdator = subMaterialEffectsUpdator(*cache.m_currentStatic);
          nextPar = currentUpdator ?
                    currentUpdator->update(nextPar, *mb, timeLim, cache.m_path,
                                           cache.m_currentStatic->geometrySignature(), dir, particle) : nextPar;

          if (!nextPar) {
            ATH_MSG_VERBOSE("  [+] Update may have killed neutral track - return.");
            cache.m_parametersAtBoundary.resetBoundaryInformation();
            return returnParameters;
          } 
            throwIntoGarbageBin(cache,nextPar);
          
        } else {    // material layer without material ?
          ATH_MSG_VERBOSE(" boundary layer without material:" << mb->layerIndex());
        }
      }

      // static volume boundary; return to the main loop
      unsigned int index = cache.m_trStaticBounds[sols[is] - iDest].bIndex;
      // use global coordinates to retrieve attached volume (just for static!)
      nextVol = (cache.m_currentStatic->boundarySurfaces())[index].get()->attachedVolume(
        nextPar->position(), nextPar->momentum(), dir);
      // double check the next volume
      if (nextVol && !(nextVol->inside(nextPar->position() + 0.01 * dir * nextPar->momentum().normalized(), 0.))) {
        ATH_MSG_DEBUG(
          "  [!] WARNING: wrongly assigned static volume ?" << cache.m_currentStatic->volumeName() << "->" <<
          nextVol->volumeName());
        nextVol = m_navigator->trackingGeometry()->lowestStaticTrackingVolume(
          nextPar->position() + 0.01 * dir * nextPar->momentum().normalized());
        if (nextVol) {
          ATH_MSG_DEBUG("  new search yields: " << nextVol->volumeName());
        }
      }
      // end double check - to be removed after validation of the geometry gluing
      if (nextVol != cache.m_currentStatic) {
        ATH_MSG_DEBUG("  [+] StaticVol boundary reached of '" << cache.m_currentStatic->volumeName() << "'.");
        if (m_navigator->atVolumeBoundary(nextPar, cache.m_currentStatic, dir, assocVol,
                                          m_tolerance) && assocVol != cache.m_currentStatic) {
          cache.m_currentDense = cache.m_dense ? nextVol : cache.m_highestVolume;
        }
        // no next volume found --- end of the world
        if (!nextVol) {
          ATH_MSG_DEBUG("  [+] World boundary reached        - at " << positionOutput(
                          nextPar->position()) << ", timed at " << cache.m_time);
          nextGeoID = Trk::GeometrySignature(Trk::Unsigned);
          // if (!destVol) { return nextPar->clone();}
          return nextPar->clone();
        }
        // next volume found and parameters are at boundary
        if (nextVol /*&& nextPar nextPar is dereferenced anyway*/) {
          ATH_MSG_DEBUG("  [+] Crossing to next volume '" << nextVol->volumeName() << "'");
          ATH_MSG_DEBUG("  [+] Crossing position is         - at " << positionOutput(nextPar->position()));
          if (!destVol && cache.m_currentStatic->geometrySignature() != nextVol->geometrySignature()) {
            nextGeoID = nextVol->geometrySignature();
            return nextPar->clone();
          }
        }
        cache.m_parametersAtBoundary.boundaryInformation(nextVol, nextPar, nextPar);
        return transportToVolumeWithPathLimit(cache,*nextPar, timeLim, dir, particle, nextGeoID, destVol);
      }
      if (dist > 0.) {
        return transportToVolumeWithPathLimit(cache,*nextPar, timeLim, dir, particle, nextGeoID, destVol);
      }
    } else if (sols[is] < iDest + cache.m_trStaticBounds.size() + cache.m_trLays.size()) {     // layer
      // material thickness - simple approach
      unsigned int index = sols[is] - iDest - cache.m_trStaticBounds.size();
      const Trk::Layer *nextLayer = cache.m_navigLays[index].second;

      bool matUp = nextLayer->layerMaterialProperties()->fullMaterial(nextPos) && m_includeMaterialEffects;

      // if (!matUp && !nextLayer->layerMaterialProperties()->fullMaterial(nextPos) )
      //  ATH_MSG_WARNING("layer without material:"<< nextLayer->layerIndex());

      // identical to the last material layer ?
      
      // if (matUp && nextLayer == cache.m_lastMaterialLayer &&
      //     nextLayer->surfaceRepresentation().type() != Trk::Surface::Cylinder) {
      //   matUp = false;
      // }

      // material update
      if (matUp && m_includeMaterialEffects) {
        const ITimedMatEffUpdator *currentUpdator = subMaterialEffectsUpdator(*cache.m_currentStatic);

        nextPar = currentUpdator ?
                  currentUpdator->update(nextPar, *nextLayer, timeLim, cache.m_path,
                                         cache.m_currentStatic->geometrySignature(), dir, particle) : nextPar;

        if (!nextPar) {
          ATH_MSG_VERBOSE("  [+] Update may have killed neutral track - return.");
          cache.m_parametersAtBoundary.resetBoundaryInformation();
          return returnParameters;
        } 
          throwIntoGarbageBin(cache,nextPar);
        
      }
    } else if (sols[is] < iDest + cache.m_trStaticBounds.size() + cache.m_trLays.size() + cache.m_trDenseBounds.size()) {
      // dense volume boundary : no material update here, navigation only ( set cache.m_currentDense for next step )

      unsigned int index = sols[is] - iDest - cache.m_trStaticBounds.size() - cache.m_trLays.size();
      std::vector< std::pair<const Trk::TrackingVolume *, unsigned int> >::iterator dIter = cache.m_denseVols.begin();
      while (index >= (*dIter).second && dIter != cache.m_denseVols.end()) {
        index -= (*dIter).second;
        ++dIter;
      }
      if (dIter != cache.m_denseVols.end()) {
        currVol = (*dIter).first;

        if (m_navigator->trackingGeometry()->atVolumeBoundary(nextPos, nextPar->momentum(), currVol, assocVol, dir,
                                                              m_tolerance)) {
          if (assocVol && assocVol->zOverAtimesRho() != 0.) {
            cache.m_currentDense = assocVol;
          } else if (currVol->inside(nextPos + 0.002 * dir * nextPar->momentum().normalized())) {
            cache.m_currentDense = currVol;
          } else {
            // new search
            cache.m_currentDense = cache.m_highestVolume;
            if (m_useMuonMatApprox && cache.m_denseVols.empty()) {
              cache.m_currentDense = cache.m_currentStatic;
            } else {
              for (unsigned int i = 0; i < cache.m_denseVols.size(); i++) {
                const Trk::TrackingVolume *dVol = cache.m_denseVols[i].first;
                if (dVol->inside(nextPos + 0.002 * dir * nextPar->momentum().normalized(),
                                 m_tolerance) && dVol->zOverAtimesRho() != 0.) {
                  cache.m_currentDense = dVol;
                }
              }
            }
          }
        }
      }
    } else {   // detached volume bounds - not relevant ?
    }

    throwIntoGarbageBin(cache,nextPar);
  }

  ATH_MSG_DEBUG(
    "  transportToVolumeWithPathLimit() - return from volume " << cache.m_currentStatic->volumeName() << " at position:" <<
    nextPar->position());

  if (nextPar) {
    return nextPar->clone();
  } 
    return nullptr;
  
}

Trk::BoundaryTrackParameters
Trk::TimedExtrapolator::transportInAlignableTV(Trk::TimedExtrapolator::Cache &cache,
                                               const Trk::TrackParameters &parm,
                                               Trk::TimeLimit &timeLim,
                                               Trk::PropDirection dir,
                                               Trk::ParticleHypothesis particle,
                                               Trk::GeometrySignature &nextGeoID,
                                               const Trk::AlignableTrackingVolume *aliTV) const {
  ATH_MSG_DEBUG("  [0] starting transport of neutral particle in alignable volume " << aliTV->volumeName());

  // material loop in sensitive Calo volumes
  // returns: boundary parameters (static volume boundary)
  // material collection / intersection with active layers  ( binned material used )

  // initialize the return parameters vector
  const Trk::TrackParameters *currPar = &parm;
  const Trk::TrackingVolume *nextVol = nullptr;
  std::vector<Trk::IdentifiedIntersection> iis;

  emptyGarbageBin(cache,&parm);

  if (!aliTV) {
    return Trk::BoundaryTrackParameters(nullptr, nullptr, nullptr);
  }

  // TODO if volume entry go to entry of misaligned volume

  // save volume entry if collection present

  const Trk::BinnedMaterial *binMat = aliTV->binnedMaterial();

  const Trk::IdentifiedMaterial *binIDMat = nullptr;

  const Trk::Material *currMat = aliTV;     // material to be used

  // if (binMat && cache.m_hitVector) {
  //  binIDMat = binMat->material(currPar->position());
  //  if (binIDMat->second>0) cache.m_hitVector->push_back(Trk::HitInfo(currPar->clone(),timeLim.time,binIDMat->second,0.));
  // }

  // loop through binned material : save identifier, material, distance

  // binned material
  if (binMat) {
    Amg::Vector3D pos = currPar->position();
    Amg::Vector3D pot = currPar->position();
    Amg::Vector3D umo = currPar->momentum().normalized();

    binIDMat = binMat->material(pos);

    if (cache.m_hitVector && binIDMat) {
      // std::cout <<"id info at the alignable volume entry:"<<binIDMat->second<<std::endl;
      if (binIDMat->second > 0) {
        cache.m_hitVector->push_back(Trk::HitInfo(currPar->clone(), timeLim.time, binIDMat->second, 0.));
      }
    }

    const Trk::BinUtility *lbu = binMat->layerBinUtility(pos);
    if (lbu) {
      unsigned int cbin = lbu->bin(pos);
      // std::cout <<"layerBinUtility retrieved:"<<lbu->bins()<< std::endl;
      std::pair<size_t, float> d2n = lbu->distanceToNext(pos, dir * umo);
      // std::cout<<"estimated distance to the next bin:"<<d2n.first<<","<<d2n.second<< std::endl;
      float dTot = 0.;
      float distTot = 0.;
      // std::cout <<"input bin:"<<cbin<<", next: "<<d2n.first<<", at distance:"<<d2n.second<< std::endl;
      while (true) {
        if (d2n.first == cbin) {
          break;
        }
        dTot += d2n.second;
        distTot = dTot;
        pos = pos + d2n.second * dir * umo;
        if (!aliTV->inside(pos)) {
          break;   // step outside volume
        }
        cbin = d2n.first;
        d2n = lbu->distanceToNext(pos, dir * umo);
        if (d2n.first == cbin && fabs(d2n.second) < 0.002) {   // move ahead
          pos = pos + 0.002 * dir * umo;
          dTot += 0.002;
          d2n = lbu->distanceToNext(pos, dir * umo);
        }
        // std::cout <<"finding next bin?:"<<d2n.first<<","<<dTot<<"+"<<d2n.second<< std::endl;
        if (d2n.second > 0.001) {    // retrieve material and save bin entry
          pot = pos + 0.5 * d2n.second * dir * umo;
          binIDMat = binMat->material(pot);
          iis.emplace_back(distTot, binIDMat->second, binIDMat->first);
          // std::cout <<"saving next bin entry:"<< distTot<<","<<binIDMat->second<<std::endl;
        }
      }
    }
  }

  // resolve exit from the volume

  cache.m_trStaticBounds.clear();
  const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > > &bounds = aliTV->boundarySurfaces();
  for (unsigned int ib = 0; ib < bounds.size(); ib++) {
    const Trk::Surface &surf = (bounds[ib].get())->surfaceRepresentation();
    Trk::DistanceSolution distSol = surf.straightLineDistanceEstimate(currPar->position(),
                                                                      dir * currPar->momentum().normalized());
    double dist = distSol.first();
    // resolve multiple intersection solutions
    if (distSol.numberOfSolutions() > 1 && dist < m_tolerance && distSol.second() > dist) {
      dist = distSol.second();
    }
    // boundary check
    Amg::Vector3D gp = currPar->position() + dist * dir * currPar->momentum().normalized();
    // std::cout<<"alignable volume boundary:"<< ib<<","<<dist<<","<<
    // surf.isOnSurface(gp,true,m_tolerance,m_tolerance)<<std::endl;
    if (dist > m_tolerance && surf.isOnSurface(gp, true, m_tolerance, m_tolerance)) {
      const Trk::TrackingVolume *attachedVol = (bounds[ib].get())->attachedVolume(gp, currPar->momentum(), dir);

      if (attachedVol && !(attachedVol->inside(gp + 0.01 * dir * currPar->momentum().normalized(), m_tolerance))) {
        ATH_MSG_DEBUG(
          "  [!] WARNING: wrongly assigned exit volume ?" << cache.m_currentStatic->volumeName() << "->" <<
          attachedVol->volumeName());
        attachedVol = m_navigator->trackingGeometry()->lowestStaticTrackingVolume(
          gp + 0.01 * dir * currPar->momentum().normalized());
        if (attachedVol) {
          ATH_MSG_DEBUG("  new search yields: " << attachedVol->volumeName());
        }
      }

      if (attachedVol != cache.m_currentStatic) {   // exit
        nextVol = attachedVol;
        cache.m_trStaticBounds.insert(cache.m_trStaticBounds.begin(), Trk::DestBound(&surf, dist, ib));
      } else if (dist > 0.001) {
        const Trk::TrackingVolume *testVol = (bounds[ib].get())->attachedVolume(gp,
                                                                                   currPar->momentum(),
                                                                                   Trk::oppositeMomentum);
        ATH_MSG_WARNING(
          "gluing problem at the exit from alignable volume: " << gp.perp() << "," << gp.z() << ":" <<
          cache.m_currentStatic->volumeName());
        if (testVol) {
          ATH_MSG_DEBUG("inverted direction:" << testVol->volumeName());
        }
        if (testVol &&
            testVol->inside(gp + 0.01 * dir * currPar->momentum().normalized(),
                            m_tolerance) && testVol != cache.m_currentStatic) {
          ATH_MSG_DEBUG(
            "next volume resolved to:" << testVol->volumeName() << " at the position(R,Z):" << gp.perp() << "," <<
            gp.z());
          nextVol = testVol;
          cache.m_trStaticBounds.insert(cache.m_trStaticBounds.begin(), Trk::DestBound(&surf, dist, ib));
        }
      }
    }
  } // end loop over boundaries

  // if (nextVol) std::cout <<"nextVol, number of exit solutions:"<<
  // nextVol->volumeName()<<","<<cache.m_trStaticBounds.size()<< std::endl;

  if (cache.m_trStaticBounds.empty()) {
    ATH_MSG_WARNING("exit from alignable volume " << aliTV->volumeName() << " not resolved, aborting");
    return Trk::BoundaryTrackParameters(nullptr, nullptr, nullptr);
  } if (cache.m_trStaticBounds.size() > 1) {  // hit edge ?
    Amg::Vector3D gp = currPar->position() + (cache.m_trStaticBounds[0].distance + 1.) * dir *
                       currPar->momentum().normalized();
    nextVol = m_navigator->trackingGeometry()->lowestStaticTrackingVolume(gp);
    ATH_MSG_DEBUG("exit volume reassigned:" << nextVol->volumeName());
  }

  // exit from the volume may coincide with the last bin boundary - leave 10 microns marge
  if (!iis.empty() && cache.m_trStaticBounds[0].distance - iis.back().distance < 0.01) {
    iis.pop_back();
  }

  // add volume exit
  iis.emplace_back(cache.m_trStaticBounds[0].distance, 0, nullptr);

  // loop over intersection taking into account the material effects

  double dist = 0.;
  double mom = currPar->momentum().mag();
  double beta = mom / sqrt(mom * mom + cache.m_particleMass * cache.m_particleMass) * Gaudi::Units::c_light;
  Amg::Vector3D nextPos = currPar->position();

  int currLay = 0;

  for (unsigned int is = 0; is < iis.size(); is++) {
    if (iis[is].distance == 0.) {
      continue;
    }

    double step = iis[is].distance - dist;

    nextPos = currPar->position() + dir * currPar->momentum().normalized() * iis[is].distance;

    double tDelta = step / beta;

    double mDelta = (currMat->zOverAtimesRho() != 0.) ? step / currMat->x0() : 0.;

    // in case of hadronic interaction retrieve nuclear interaction properties, too

    double frT = 1.;
    if (step > 0 && timeLim.tMax > cache.m_time && cache.m_time + tDelta >= timeLim.tMax) {
      frT = (timeLim.tMax - cache.m_time) * beta / step;
    }

    // TODO : compare x0 or l0 according to the process type
    double frM = 1.;
    if (mDelta > 0 && cache.m_path.x0Max > 0.) {
      if (cache.m_path.process < 100 && cache.m_path.x0Collected + mDelta > cache.m_path.x0Max) {
        frM = (cache.m_path.x0Max - cache.m_path.x0Collected) / mDelta;
      } else {         // waiting for hadronic interaction,  retrieve nuclear interaction properties
        double mDeltaL = currMat->L0 > 0. ? step / currMat->L0 : mDelta / 0.37 / currMat->averageZ();
        if (cache.m_path.l0Collected + mDeltaL > cache.m_path.x0Max) {
          frM = (cache.m_path.x0Max - cache.m_path.l0Collected) / mDeltaL;
        }
      }
    }

    double fr = fmin(frT, frM);

    // std::cout << "looping over intersections:"<<is<<","<< cache.m_trSurfs[sols[is]].second<<","<<step << ","<<
    // tDelta<<","<<mDelta << std::endl;

    if (fr < 1.) { // decay or material interaction during the step
      int process = frT < frM ? timeLim.process : cache.m_path.process;
      cache.m_time += fr * step / beta;
      if (mDelta > 0 && currMat->averageZ() > 0) {
        cache.m_path.updateMat(fr * mDelta, currMat->averageZ(), 0.);
      }

      nextPos = currPar->position() + dir * currPar->momentum().normalized() * (dist + fr * step);

      // process interaction only if creation of secondaries allowed
      if (m_caloMsSecondary) {
        const Trk::TrackParameters *nextPar = m_updators[0]->interact(cache.m_time, nextPos,
                                                                      currPar->momentum(), particle, process, currMat);
        throwIntoGarbageBin(cache,nextPar);

        if (nextPar) {
          ATH_MSG_DEBUG(" [!] WARNING: particle survives the interaction " << process);
        }

        if (nextPar && process == 121) {
          ATH_MSG_DEBUG(" [!] WARNING: failed hadronic interaction, killing the input particle anyway");
          return Trk::BoundaryTrackParameters(nullptr, nullptr, nullptr);
        }

        if (!nextPar) {
          return Trk::BoundaryTrackParameters(nullptr, nullptr, nullptr);
        }

        // return transportToVolumeWithPathLimit(*nextPar, timeLim, dir, particle, nextGeoID, destVol);
      } else {  // kill particle without trace ?
        return Trk::BoundaryTrackParameters(nullptr, nullptr, nullptr);
      }
    }  // end decay or material interaction during the step

    // update
    dist = iis[is].distance;
    if (mDelta > 0 && currMat->averageZ() > 0) {
      cache.m_path.updateMat(mDelta, currMat->averageZ(), 0.);
    }
    cache.m_time += tDelta;

    if (is < iis.size() - 1) {  // update bin material info
      // binIDMat = binMat->material(nextPos);
      // currMat = binIDMat->first;
      currMat = iis[is].material;
      currLay = iis[is].identifier;

      if (cache.m_hitVector && iis[is].identifier > 0) {      // save entry to the next layer
        ATH_MSG_VERBOSE("active layer entry:" << currLay << " at R,z:" << nextPos.perp() << "," << nextPos.z());
        Trk::CurvilinearParameters *nextPar = new Trk::CurvilinearParameters(nextPos, currPar->momentum(), 0.);
        cache.m_hitVector->push_back(Trk::HitInfo(nextPar, timeLim.time, iis[is].identifier, 0.));
      }
    }
  }   // end loop over intersections

  Trk::CurvilinearParameters *nextPar = new Trk::CurvilinearParameters(nextPos, currPar->momentum(), 0.);

  if (cache.m_hitVector) {      // save volume exit /active layer only ?
    ATH_MSG_VERBOSE("active layer/volume exit:" << currLay << " at R,z:" << nextPos.perp() << "," << nextPos.z());
    if (binIDMat and(binIDMat->second > 0)) {
      cache.m_hitVector->push_back(Trk::HitInfo(nextPar->clone(), timeLim.time, currLay, 0.));
    }
  }

  throwIntoGarbageBin(cache,nextPar);

  // static volume boundary; return to the main loop : TODO move from misaligned to static
  // unsigned int index = cache.m_trStaticBounds[0].bIndex;
  // use global coordinates to retrieve attached volume (just for static!)
  // nextVol =
  // (cache.m_currentStatic->boundarySurfaces())[index].get()->attachedVolume(nextPar->position(),nextPar->momentum(),dir);
  // double check the next volume
  // if ( nextVol && !(nextVol->inside(nextPar->position()+0.01*nextPar->momentum().normalized(),m_tolerance) ) ) {
  //  ATH_MSG_DEBUG( "  [!] WARNING: wrongly assigned static volume ?"<< cache.m_currentStatic->volumeName()<<"->" <<
  // nextVol->volumeName() );
  //  nextVol =
  // m_navigator->trackingGeometry()->lowestStaticTrackingVolume(nextPar->position()+0.01*nextPar->momentum().normalized());
  //  if (nextVol) ATH_MSG_DEBUG( "  new search yields: "<< nextVol->volumeName() );
  // }

  ATH_MSG_DEBUG("  [+] StaticVol boundary reached of '" << cache.m_currentStatic->volumeName() << "'.");

  // no next volume found --- end of the world
  if (!nextVol) {
    ATH_MSG_DEBUG("  [+] World boundary reached        - at " << positionOutput(
                    nextPar->position()) << ", timed at " << cache.m_time);
    nextGeoID = Trk::GeometrySignature(Trk::Unsigned);
  } else {
    ATH_MSG_DEBUG("  [+] Crossing to next volume '" << nextVol->volumeName() << "'");
    ATH_MSG_DEBUG("  [+] Crossing position is         - at " << positionOutput(nextPar->position()));
  }

  cache.m_parametersAtBoundary.boundaryInformation(nextVol, nextPar, nextPar);

  return Trk::BoundaryTrackParameters(nextPar, nextVol, cache.m_currentStatic);
}

Trk::BoundaryTrackParameters
Trk::TimedExtrapolator::extrapolateInAlignableTV(Trk::TimedExtrapolator::Cache &cache,
                                                 const Trk::TrackParameters &parm,
                                                 Trk::TimeLimit &timeLim,
                                                 Trk::PropDirection dir,
                                                 Trk::ParticleHypothesis particle,
                                                 Trk::GeometrySignature &nextGeoID,
                                                 const Trk::AlignableTrackingVolume *vol) const {
  ATH_MSG_DEBUG("M-[" << ++cache.m_methodSequence << "] extrapolateInAlignableTV(...) " << vol->volumeName());

  // material loop in sensitive Calo volumes
  // extrapolation without target surface returns:
  //    A)    boundary parameters (static volume boundary)
  // if target surface:
  //    B)    trPar at target surface
  // material collection done by the propagator ( binned material used )

  // initialize the return parameters vector
  const Trk::TrackParameters *currPar = &parm;
  const Trk::AlignableTrackingVolume *staticVol = nullptr;
  const Trk::TrackingVolume *currVol = nullptr;
  const Trk::TrackingVolume *nextVol = nullptr;
  std::vector<unsigned int> solutions;
  // double tol = 0.001;
  // double path = 0.;
  if (!cache.m_highestVolume) {
    cache.m_highestVolume = m_navigator->highestVolume();
  }

  emptyGarbageBin(cache,&parm);

  // verify current position
  Amg::Vector3D gp = parm.position();
  if (vol && vol->inside(gp, m_tolerance)) {
    staticVol = vol;
  } else {
    currVol = m_navigator->trackingGeometry()->lowestStaticTrackingVolume(gp);
    const Trk::TrackingVolume *nextStatVol = nullptr;
    if (m_navigator->atVolumeBoundary(currPar, currVol, dir, nextStatVol, m_tolerance) && nextStatVol != currVol) {
      currVol = nextStatVol;
    }
    if (currVol && currVol != vol) {
      const Trk::AlignableTrackingVolume *aliTG = dynamic_cast<const Trk::AlignableTrackingVolume *> (currVol);
      if (aliTG) {
        staticVol = aliTG;
      }
    }
  }

  if (!staticVol) {
    ATH_MSG_DEBUG("  [!] failing in retrieval of AlignableTV, return 0");
    return Trk::BoundaryTrackParameters(nullptr, nullptr, nullptr);
  }

  // TODO if volume entry go to entry of misaligned volume

  // save volume entry if collection present

  if (cache.m_hitVector) {
    const Trk::BinnedMaterial *binMat = staticVol->binnedMaterial();
    if (binMat) {
      const Trk::IdentifiedMaterial *binIDMat = binMat->material(currPar->position());
      if (binIDMat->second > 0) {
        cache.m_hitVector->push_back(Trk::HitInfo(currPar->clone(), timeLim.time, binIDMat->second, 0.));
      }
    }
  }

  // navigation surfaces
  if (cache.m_navigSurfs.capacity() > m_maxNavigSurf) {
    cache.m_navigSurfs.reserve(m_maxNavigSurf);
  }
  cache.m_navigSurfs.clear();

  // assume new static volume, retrieve boundaries
  cache.m_currentStatic = staticVol;
  cache.m_staticBoundaries.clear();
  const std::vector< SharedObject<const BoundarySurface<TrackingVolume> > > &bounds = staticVol->boundarySurfaces();
  for (unsigned int ib = 0; ib < bounds.size(); ib++) {
    const Trk::Surface &surf = (bounds[ib].get())->surfaceRepresentation();
    cache.m_staticBoundaries.emplace_back(&surf, true);
  }

  cache.m_navigSurfs.insert(cache.m_navigSurfs.end(), cache.m_staticBoundaries.begin(), cache.m_staticBoundaries.end());

  // current dense
  cache.m_currentDense = staticVol;

  // ready to propagate
  // till: A/ static volume boundary(bcheck=true) , B/ destination surface(bcheck=false)

  nextVol = nullptr;
  while (currPar) {
    std::vector<unsigned int> solutions;
    // propagate now
    ATH_MSG_DEBUG("  [+] Starting propagation at position  " << positionOutput(currPar->position())
                                                             << " (current momentum: " << currPar->momentum().mag() <<
      ")");
    ATH_MSG_DEBUG("  [+] " << cache.m_navigSurfs.size() << " target surfaces in '" << cache.m_currentDense->volumeName() << "'.");
    //  arguments : inputParameters, vector of navigation surfaces, propagation direction, b field service, particle
    // type, result,
    //              material collection, intersection collection, path limit, switch for use of path limit, switch for
    // curvilinear on return, current TG volume
    const Trk::TrackParameters *nextPar = m_stepPropagator->propagateT(*currPar, cache.m_navigSurfs, dir, m_fieldProperties,
                                                                       particle, solutions, cache.m_path, timeLim, true,
                                                                       cache.m_currentDense, cache.m_hitVector);
    ATH_MSG_VERBOSE("  [+] Propagation done. ");
    if (nextPar) {
      ATH_MSG_DEBUG("  [+] Position after propagation -   at " << positionOutput(nextPar->position()));
    }

    if (nextPar) {
      ATH_MSG_DEBUG("  [+] Number of intersection solutions: " << solutions.size());
    }
    if (nextPar) {
      throwIntoGarbageBin(cache,nextPar);
    }

    // material update has been done already by the propagator
    if (cache.m_path.x0Max > 0. &&
        ((cache.m_path.process < 100 && cache.m_path.x0Collected >= cache.m_path.x0Max) ||
         (cache.m_path.process > 100 && cache.m_path.l0Collected >= cache.m_path.x0Max))) {
      // trigger presampled interaction, provide material properties if needed
      // process interaction only if creation of secondaries allowed
      if (cache.m_currentStatic->geometrySignature() == Trk::ID || m_caloMsSecondary) {
        const Trk::Material *extMprop = cache.m_path.process > 100 ? cache.m_currentDense : nullptr;

        const Trk::TrackParameters *iPar = nullptr;
        if (nextPar) {
          iPar = m_updators[0]->interact(timeLim.time, nextPar->position(),
                                         nextPar->momentum(), particle, cache.m_path.process, extMprop);
        }

        if (!iPar) {
          return Trk::BoundaryTrackParameters(nullptr, nullptr, nullptr);
        }

        throwIntoGarbageBin(cache,iPar);

        if (iPar && cache.m_path.process == 121) {
          ATH_MSG_DEBUG(" [!] WARNING: failed hadronic interaction, killing the input particle anyway");
          return Trk::BoundaryTrackParameters(nullptr, nullptr, nullptr);
        }

        // return transportToVolumeWithPathLimit(*nextPar, timeLim, dir, particle, nextGeoID, destVol);
      } else {  // kill particle without trace ?
        return Trk::BoundaryTrackParameters(nullptr, nullptr, nullptr);
      }
    }

    // decay ?
    if (timeLim.tMax > 0. && timeLim.time >= timeLim.tMax) {
      // process interaction only if creation of secondaries allowed
      if (cache.m_currentStatic->geometrySignature() == Trk::ID || m_caloMsSecondary) {
        // trigger presampled interaction
        const Trk::TrackParameters *iPar = m_updators[0]->interact(timeLim.time, nextPar->position(),
                                                                   nextPar->momentum(), particle, timeLim.process);
        if (!iPar) {
          return Trk::BoundaryTrackParameters(nullptr, nullptr, nullptr);
        }

        throwIntoGarbageBin(cache,iPar);
        ATH_MSG_WARNING("particle decay survival?" << particle << "," << timeLim.process);
        return Trk::BoundaryTrackParameters(nullptr, nullptr, nullptr);
      }    // kill the particle without trace ( some validation info can be included here eventually )
        return Trk::BoundaryTrackParameters(nullptr, nullptr, nullptr);
      
    }

    if (nextPar) {
      unsigned int iSol = 0;
      while (iSol < solutions.size()) {
        if (solutions[iSol] < cache.m_staticBoundaries.size()) {
          // TODO if massive boundary coded, add the material effects here
          // static volume boundary; return to the main loop : TODO move from misaligned to static
          unsigned int index = solutions[iSol];
          // use global coordinates to retrieve attached volume (just for static!)
          nextVol = (cache.m_currentStatic->boundarySurfaces())[index].get()->attachedVolume(
            nextPar->position(), nextPar->momentum(), dir);
          // double check the next volume
          if (nextVol && !(nextVol->inside(nextPar->position() + 0.01 * dir * nextPar->momentum().normalized(), 0.))) {
            ATH_MSG_DEBUG(
              "  [!] WARNING: wrongly assigned static volume ?" << cache.m_currentStatic->volumeName() << "->" <<
              nextVol->volumeName());
            nextVol = m_navigator->trackingGeometry()->lowestStaticTrackingVolume(
              nextPar->position() + 0.01 * dir * nextPar->momentum().normalized());
            if (nextVol) {
              ATH_MSG_DEBUG("  new search yields: " << nextVol->volumeName());
            }
          }
          // end double check - to be removed after validation of the geometry gluing
          // lateral exit from calo sample can be handled here
          if (cache.m_hitVector) {
            const Trk::BinnedMaterial *binMat = staticVol->binnedMaterial();
            if (binMat) {
              const Trk::IdentifiedMaterial *binIDMat = binMat->material(nextPar->position());
              // save only if entry to the sample present, the exit missing and non-zero step in the sample
              if (binIDMat && binIDMat->second > 0 && !cache.m_hitVector->empty() &&
                  cache.m_hitVector->back().detID == binIDMat->second) {
                // double s = (nextPar->position()-m_identifiedParameters->back().first->position()).mag();
                // if (s>0.001) m_identifiedParameters->push_back(std::pair<const Trk::TrackParameters*,int>
                // (nextPar->clone(), -binIDMat->second));
                cache.m_hitVector->push_back(Trk::HitInfo(nextPar->clone(), timeLim.time, -binIDMat->second, 0.));
              }
            }
          }
          // end lateral exit handling

          ATH_MSG_DEBUG("  [+] StaticVol boundary reached of '" << cache.m_currentStatic->volumeName() << "'.");
          // no next volume found --- end of the world
          if (!nextVol) {
            ATH_MSG_DEBUG("  [+] World boundary reached        - at " << positionOutput(
                            nextPar->position()) << ", timed at " << cache.m_time);
            nextGeoID = Trk::GeometrySignature(Trk::Unsigned);
          } else {
            ATH_MSG_DEBUG("  [+] Crossing to next volume '" << nextVol->volumeName() << "'");
            ATH_MSG_DEBUG("  [+] Crossing position is         - at " << positionOutput(nextPar->position()));
          }

          return Trk::BoundaryTrackParameters(nextPar, nextVol, cache.m_currentStatic);
        }
      }
    }

    currPar = nextPar;
  }

  return Trk::BoundaryTrackParameters(nullptr, nullptr, nullptr);
}
