/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InDetTrackHoleSearchTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "InDetTrackHoleSearch/InDetTrackHoleSearchTool.h"

//Trk
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkEventUtils/TrackStateOnSurfaceComparisonFunction.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkGeometry/TrackingVolume.h"
#include "Identifier/Identifier.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetRecToolInterfaces/IInDetTestPixelLayerTool.h"
#include "TrkVolumes/Volume.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include <set>

//================ Constructor =================================================
InDet::InDetTrackHoleSearchTool::InDetTrackHoleSearchTool(const std::string& t,
                                                          const std::string& n,
                                                          const IInterface*  p) :
  AthAlgTool(t,n,p),
  m_atlasId(nullptr),
  m_extrapolator("Trk::Extrapolator"),
  m_pixelLayerTool("InDet::InDetTestPixelLayerTool"),
  m_geoModelSvc("GeoModelSvc", n),
  m_extendedListOfHoles(false),
  m_cosmic(false),
  m_usepix(true),
  m_usesct(true),
  m_checkBadSCTChip(true),
  m_warning(0) {
  declareInterface<ITrackHoleSearchTool>(this);
  declareProperty("Extrapolator"         , m_extrapolator);
  declareProperty("PixelLayerTool"       , m_pixelLayerTool);
  declareProperty("GeoModelService"      , m_geoModelSvc);
  declareProperty("ExtendedListOfHoles"  , m_extendedListOfHoles = false);
  declareProperty("Cosmics"              , m_cosmic);
  declareProperty("usePixel"             , m_usepix);
  declareProperty("useSCT"               , m_usesct);
  declareProperty("checkBadSCTChip"      , m_checkBadSCTChip);
  declareProperty("minSiHits"            , m_minSiHits = 3);  
  declareProperty("CountDeadModulesAfterLastHit", m_countDeadModulesAfterLastHit = true);  
  declareProperty("phitol"               , m_phitol = 3.);
  declareProperty("etatol"               , m_etatol = 3.);
}

//================ Destructor =================================================
InDet::InDetTrackHoleSearchTool::~InDetTrackHoleSearchTool() {
}

//================ Initialisation =================================================
StatusCode InDet::InDetTrackHoleSearchTool::initialize() {
  
  StatusCode sc = AlgTool::initialize();
  if (sc.isFailure()) return sc;

  ATH_CHECK(detStore()->retrieve(m_atlasId, "AtlasID"));

  // Get TrkExtrapolator from ToolService
  ATH_CHECK(m_extrapolator.retrieve());
  ATH_MSG_INFO("Retrieved tool " << m_extrapolator);

  if (m_usepix) {
    // Get PixelConditionsSummaryTool
    ATH_CHECK(m_pixelCondSummaryTool.retrieve());
    ATH_MSG_INFO("Retrieved tool " << m_pixelCondSummaryTool);
    // Get InDetPixelLayerTool from ToolService
    ATH_CHECK(m_pixelLayerTool.retrieve());
    ATH_MSG_INFO("Retrieved tool " << m_pixelLayerTool);
  }

  if (m_usesct) {
    // Get SctConditionsSummaryTool
    ATH_CHECK(m_sctCondSummaryTool.retrieve());
    ATH_MSG_INFO("Retrieved tool " << m_sctCondSummaryTool);
  } else {
    m_sctCondSummaryTool.disable();
  }

  if (m_checkBadSCTChip) {
    // Check if ITk Strip is used because isBadSCTChipStrip method is valid only for SCT.
    ATH_CHECK(m_geoModelSvc.retrieve());
    if (m_geoModelSvc->geoConfig()==GeoModel::GEO_RUN4 or
        m_geoModelSvc->geoConfig()==GeoModel::GEO_ITk) {
      ATH_MSG_WARNING("Since ITk Strip is used, m_checkBadSCTChip is turned off.");
      m_checkBadSCTChip = false;
    }
  }

  if (m_extendedListOfHoles) ATH_MSG_INFO("Search for extended list of holes ");

  ATH_MSG_INFO("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================
StatusCode InDet::InDetTrackHoleSearchTool::finalize() {
  StatusCode sc = AlgTool::finalize();
  return sc;
}

//============================================================================================
void InDet::InDetTrackHoleSearchTool::countHoles(const Trk::Track& track, 
                                                 std::vector<int>& information,
                                                 const Trk::ParticleHypothesis partHyp) const {
  std::vector<const Trk::TrackStateOnSurface*>* listOfHoles = nullptr;
  searchForHoles(track,&information,listOfHoles,partHyp);
  if (listOfHoles) {
    ATH_MSG_ERROR("listOfHoles is leaking in countHoles !!!");
    for (std::vector<const Trk::TrackStateOnSurface*>::const_iterator it = listOfHoles->begin();
         it != listOfHoles->end(); ++it) {
      delete (*it);
    }
    delete listOfHoles; 
    listOfHoles = nullptr;
  }
  return;
}

//============================================================================================
const DataVector<const Trk::TrackStateOnSurface>* InDet::InDetTrackHoleSearchTool::getHolesOnTrack(const Trk::Track& track, 
                                                                                                   const Trk::ParticleHypothesis partHyp) const {
  std::vector<const Trk::TrackStateOnSurface*>* listOfHoles = new std::vector<const Trk::TrackStateOnSurface*>;
  searchForHoles(track, 0, listOfHoles,partHyp);

  DataVector<const Trk::TrackStateOnSurface>* output = new DataVector<const Trk::TrackStateOnSurface>;
  for (std::vector<const Trk::TrackStateOnSurface*>::const_iterator it = listOfHoles->begin();
       it != listOfHoles->end();
       ++it)
    output->push_back(*it);

  delete listOfHoles;
  listOfHoles = nullptr;
  return output;
}

//============================================================================================
const Trk::Track*  InDet::InDetTrackHoleSearchTool::getTrackWithHoles(const Trk::Track& track, 
                                                                      const Trk::ParticleHypothesis partHyp) const {
  std::vector<const Trk::TrackStateOnSurface*>* listOfHoles = new std::vector<const Trk::TrackStateOnSurface*>;
  searchForHoles(track, 0, listOfHoles,partHyp);
  const  Trk::Track* newTrack = addHolesToTrack(track,listOfHoles);
  delete listOfHoles;
  listOfHoles = nullptr;
  return newTrack;
}

//============================================================================================
const Trk::Track* InDet::InDetTrackHoleSearchTool::getTrackWithHolesAndOutliers(const Trk::Track& track, 
                                                                                const Trk::ParticleHypothesis partHyp) const {
  return getTrackWithHoles(track,partHyp);
}


//============================================================================================

void InDet::InDetTrackHoleSearchTool::searchForHoles(const Trk::Track& track, 
                                                     std::vector<int>* information,
                                                     std::vector<const Trk::TrackStateOnSurface*>* listOfHoles,
                                                     const Trk::ParticleHypothesis partHyp) const {
  ATH_MSG_DEBUG("starting searchForHoles()");

  if (information) {
    (*information)[Trk::numberOfPixelHoles]        = -1;
    (*information)[Trk::numberOfSCTHoles]          = -1;
    (*information)[Trk::numberOfSCTDoubleHoles]    = -1;
    (*information)[Trk::numberOfPixelDeadSensors]  = -1;
    (*information)[Trk::numberOfSCTDeadSensors]    = -1;
    (*information)[Trk::numberOfTRTHoles]          = -1;
    (*information)[Trk::numberOfTRTDeadStraws]     = -1;

  }
  
  std::map<const Identifier, const Trk::TrackStateOnSurface*> mapOfHits;

  // JEF: fix of [bug #44382] Poor Tracking Software Performance without SCT
  // the mapOfPrediction needs the knowledge weather a holesearch should be carried out
  // on this identifier or just the search for dead modules
  // therefore: if the boolean is set to true, a holesearch will be caarried out, otherwise just 
  // the search for dead modules
  // for identifiers BEFORE the last measurement (and after the first): holesearch will be carried out always
  // for identifiers AFTER the last measurement (or before the first), we have to distiguish two different scenarios:
  // 1) extendedListOfHoles==True: a hole search will be carried out for all identifiers
  // 2) extendedListOfHoles==False (default for collisions): dead modules will be counted after the last
  //    measurement, but no holes
  std::map<const Identifier, std::pair<const Trk::TrackParameters*,const bool> >     mapOfPredictions;

  bool listOk = getMapOfHits(track,partHyp,mapOfHits,mapOfPredictions);

  if (listOk) {
    ATH_MSG_DEBUG("Perform stepwise hole search");
    performHoleSearchStepWise(mapOfHits, mapOfPredictions, information, listOfHoles);
  } else {
    ATH_MSG_DEBUG("List of hits not properly obtained, abort hole search.");
  }

  for (std::map<const Identifier, std::pair<const Trk::TrackParameters*, const bool> >::iterator it = mapOfPredictions.begin();
       it != mapOfPredictions.end(); ++it) {
    delete (it->second).first;
    (it->second).first = nullptr;
  }

  return;  
}

// ====================================================================================================================
bool InDet::InDetTrackHoleSearchTool::getMapOfHits(const Trk::Track& track, 
                                                   const Trk::ParticleHypothesis partHyp,
                                                   std::map<const Identifier, const Trk::TrackStateOnSurface*>& mapOfHits,
                                                   std::map<const Identifier, std::pair<const Trk::TrackParameters*,const bool> >& mapOfPredictions) const {

  /* find all Si TSOS
     - all of the above have to have identifiers
     - keep outliers and holes
     - mark tracks with TRT
     - sort into map by identifier
  */
  //std::cout << "track: " << track << std::endl;
  int imeas = 0;
  const Trk::TrackParameters* firstsipar = nullptr;
  
  for (DataVector<const Trk::TrackStateOnSurface>::const_iterator iterTSOS = track.trackStateOnSurfaces()->begin();
       iterTSOS!=track.trackStateOnSurfaces()->end();++iterTSOS) { 
    // type of state is measurement, hole or outlier ?
    if ((*iterTSOS)->type(Trk::TrackStateOnSurface::Measurement) ||
        (*iterTSOS)->type(Trk::TrackStateOnSurface::Outlier)) {
      Identifier id ;
      bool hasID = false;
      if ((*iterTSOS)->measurementOnTrack() != nullptr
          && (*iterTSOS)->measurementOnTrack()->associatedSurface().associatedDetectorElement() != nullptr
          && (*iterTSOS)->measurementOnTrack()->associatedSurface().associatedDetectorElement()->identify() != 0) { 
        id    = (*iterTSOS)->measurementOnTrack()->associatedSurface().associatedDetectorElement()->identify();
        hasID = true;
      } else if ((*iterTSOS)->trackParameters() != nullptr
                 && (*iterTSOS)->trackParameters()->associatedSurface().associatedDetectorElement() != nullptr
                 && (*iterTSOS)->trackParameters()->associatedSurface().associatedDetectorElement()->identify() != 0) { 
        id    = (*iterTSOS)->trackParameters()->associatedSurface().associatedDetectorElement()->identify();
        hasID = true;
      }
      // copy all Si track states, including the holes and outliers
      if (hasID && (m_atlasId->is_pixel(id) || m_atlasId->is_sct(id))) {
        // sort the state according to the id 
        mapOfHits.insert(std::pair<const Identifier, const Trk::TrackStateOnSurface*>(id,*iterTSOS));
        if (!(*iterTSOS)->type(Trk::TrackStateOnSurface::Outlier)) {
          ++imeas;
          if (!(*iterTSOS)->trackParameters() && m_warning<10) {
            m_warning++;
            ATH_MSG_WARNING("No track parameters available for state of type measurement");
            ATH_MSG_WARNING("Don't run this tool on slimmed tracks!");
            if (m_warning==10) ATH_MSG_WARNING("(last message!)");
          }
        }
        // for cosmics: remember parameters of first SI TSOS
        if (m_cosmic && !firstsipar && (*iterTSOS)->trackParameters()) firstsipar=(*iterTSOS)->trackParameters(); 
        if ((*iterTSOS)->trackParameters()) {
          ATH_MSG_VERBOSE("TSOS pos: " << (*iterTSOS)->trackParameters()->position() 
                          << "   r: " << sqrt(pow((*iterTSOS)->trackParameters()->position().x(),2)
                                              +pow((*iterTSOS)->trackParameters()->position().y(),2))
                          << "     Si measurement");
        }
      } else {
        if ((*iterTSOS)->trackParameters()) {
          ATH_MSG_VERBOSE("TSOS pos: " << (*iterTSOS)->trackParameters()->position() 
                          << "   r: " << sqrt(pow((*iterTSOS)->trackParameters()->position().x(),2)
                                              +pow((*iterTSOS)->trackParameters()->position().y(),2)) 
                          << "     TRT measurement");
        }
      }
    }
  }

  ATH_MSG_DEBUG("Number of Si hits + outliers on original track: " << mapOfHits.size() << " , hits only: " << imeas);
  if ((imeas<m_minSiHits && ! m_extendedListOfHoles) || imeas==0 || (!firstsipar && m_cosmic)) {
    ATH_MSG_DEBUG("Less than "<< m_minSiHits << " Si measurements " << "on track, abort ID hole search");
    mapOfHits.clear();
    return false;
  }

  // find starting point for 2nd iteration to find predictions
  const Trk::TrackParameters* startParameters = nullptr;

  // ------- special logic for cosmics to obtain start point for hole search

  if (m_cosmic) {
    // retrieve surface, from which hole search should start
    // retrieve tracking geometry 
    const Trk::TrackingGeometry* trackingGeometry =  m_extrapolator->trackingGeometry();
    // get sct volume
    const Trk::TrackingVolume* sctVolume = trackingGeometry->trackingVolume("InDet::Detectors::SCT::Barrel"); 
    //get BoundarySurface for cylinder between sct and trt
    const Trk::CylinderSurface* sctCylinder = dynamic_cast<const Trk::CylinderSurface*>
      (sctVolume->boundarySurfaces()[Trk::tubeOuterCover].getPtr());
    if (!sctCylinder) {
      ATH_MSG_ERROR ("cast to CylinderSurface failed, should never happen !");
      return false;
    }

    // extrapolate track to cylinder; take this as starting point for hole search

    if (firstsipar) {
      //std::cout << "firstsipar: " << *firstsipar << " pos: " << firstsipar->position() << std::endl;
      startParameters = m_extrapolator->extrapolate(*firstsipar,
                                                    *sctCylinder, 
                                                    Trk::oppositeMomentum, 
                                                    true, 
                                                    partHyp);
    }
    
    // if track can't be extrapolated to this cylinder (EC track!), extrapolate to disc outside TRT/SCT EC
    if (!startParameters) {  
      ATH_MSG_DEBUG("no start parameters on SCT cylinder, try TRT ec disc");
      // get BoundarySurface for disk which encloses TRT ECs
      // depending on track origin use neg or pos EC
      const Trk::DiscSurface* trtDisc;
      // inverse logic: tracks with theta < M_PI/2 have origin in negative EC
      if (firstsipar->parameters()[Trk::theta] < M_PI/2.) {
        const Trk::TrackingVolume* trtVolume = trackingGeometry->trackingVolume("InDet::Detectors::TRT::NegativeEndcap");
        trtDisc = dynamic_cast<const Trk::DiscSurface*> (trtVolume->boundarySurfaces()[Trk::negativeFaceXY].getPtr());
      } else {
        const Trk::TrackingVolume* trtVolume = trackingGeometry->trackingVolume("InDet::Detectors::TRT::PositiveEndcap");  
        trtDisc = dynamic_cast<const Trk::DiscSurface*> (trtVolume->boundarySurfaces()[Trk::positiveFaceXY].getPtr());
      }
      // extrapolate track to disk
      startParameters = m_extrapolator->extrapolate(*firstsipar,
                                                    *trtDisc,
                                                    Trk::oppositeMomentum, 
                                                    true, 
                                                    partHyp);
    }
  } else {  // no cosmics
    
    if (track.perigeeParameters()) {
      startParameters = track.perigeeParameters()->clone();
    } else if (track.trackParameters()->front()) {
      ATH_MSG_DEBUG("No perigee, extrapolate to 0,0,0");
      // go back to perigee
      startParameters = m_extrapolator->extrapolate(*(track.trackParameters()->front()),
                                                    Trk::PerigeeSurface(),
                                                    Trk::anyDirection, 
                                                    false, partHyp);
    }
  }

  if (!startParameters) {
    ATH_MSG_DEBUG("No start point obtained, hole search not performed.");
    mapOfHits.clear();
    return false;
  }

  bool foundFirst = false;
  // ME - if we look for an extended list of holes, we take the first module, otherwise we searchfor the first measurment
  // if we have a cosmics track, we want to get the extended list as well!
  if (m_extendedListOfHoles ||  m_cosmic) {
    ATH_MSG_DEBUG("We are looking for an extended list of holes, so add eventual holes before first hits");
    foundFirst = true;
  }
  
  Identifier          id(0);
  const Trk::Surface* surf  = nullptr;
  bool                hasID = false;
  
  // 2nd iteration to find predictions
  DataVector<const Trk::TrackStateOnSurface>::const_iterator iterTSOS = track.trackStateOnSurfaces()->begin();

  // for cosmics: go to the first si mearsurement on track
  // for cosmics: add perigee as intermediate state, as the extrapolator does nothing if the starting layer and destination layer are the same
  if (m_cosmic) {
    while (iterTSOS!=track.trackStateOnSurfaces()->end()
           && (!(*iterTSOS)->type(Trk::TrackStateOnSurface::Measurement)
               || !dynamic_cast<const Trk::PlaneSurface*>(&(*iterTSOS)->measurementOnTrack()->associatedSurface()))) {
      iterTSOS++;
    }
  }

  ATH_MSG_VERBOSE("start position: " << startParameters->position() 
                  << "   r: " << sqrt(pow(startParameters->position().x(),2)
                                      +pow(startParameters->position().y(),2)));
 
  int measno=0;
  int nmeas=(int)track.measurementsOnTrack()->size();
  for (; iterTSOS!=track.trackStateOnSurfaces()->end();++iterTSOS) {
    const Trk::Perigee *per=nullptr;
    // type of state is measurement ?
    if ((*iterTSOS)->type(Trk::TrackStateOnSurface::Measurement) ||
        (*iterTSOS)->type(Trk::TrackStateOnSurface::Outlier) ||
        ((*iterTSOS)->type(Trk::TrackStateOnSurface::Perigee) && m_cosmic)) {
      hasID=false;
      per=nullptr;
      if (m_cosmic && (*iterTSOS)->trackParameters()) per=dynamic_cast<const Trk::Perigee*>((*iterTSOS)->trackParameters());
      if ((*iterTSOS)->measurementOnTrack() != nullptr
          && (*iterTSOS)->measurementOnTrack()->associatedSurface().associatedDetectorElement() != nullptr
          && (*iterTSOS)->measurementOnTrack()->associatedSurface().associatedDetectorElement()->identify() != 0) { 
        id    = (*iterTSOS)->measurementOnTrack()->associatedSurface().associatedDetectorElement()->identify();
        surf  = &(*iterTSOS)->measurementOnTrack()->associatedSurface();
        hasID = true;
      } else if ((*iterTSOS)->trackParameters() != nullptr
                 && (*iterTSOS)->trackParameters()->associatedSurface().associatedDetectorElement() != nullptr
                 && (*iterTSOS)->trackParameters()->associatedSurface().associatedDetectorElement()->identify() != 0) { 
        id    = (*iterTSOS)->trackParameters()->associatedSurface().associatedDetectorElement()->identify();
        surf  = &((*iterTSOS)->trackParameters()->associatedSurface());
        hasID = true;
      } else if (m_cosmic && per) {
        surf=&((*iterTSOS)->trackParameters()->associatedSurface());
      }

      // see if this is an Si state !
      if ((m_cosmic && per) || (hasID && (m_atlasId->is_pixel(id) || m_atlasId->is_sct(id) || m_atlasId->is_trt(id)))) {
        
        if (m_atlasId->is_trt(id)) ATH_MSG_VERBOSE("Target is TRT, see if we can add something");
        
        // extrapolate stepwise to this parameter (be careful, sorting might be wrong)
        const std::vector<const Trk::TrackParameters*>* paramList =
          m_extrapolator->extrapolateStepwise(*startParameters,
                                              *surf,
                                              Trk::alongMomentum,
                                              false, partHyp);
        
        if (!paramList) {
          ATH_MSG_VERBOSE("--> Did not manage to extrapolate to surface!!!");
          continue;
        }

        ATH_MSG_VERBOSE("Number of parameters in this step: " << paramList->size());
        
        // loop over the predictons and analyze them
        for (std::vector<const Trk::TrackParameters*>::const_iterator it = paramList->begin();
             it != paramList->end(); ++it) {
          // copy pointer
          const Trk::TrackParameters* thisParameters = *it;
          ATH_MSG_VERBOSE("extrapolated pos: " << thisParameters->position() << "   r: " << 
                          sqrt(pow(thisParameters->position().x(),2)+pow(thisParameters->position().y(),2)));
      
          // check if surface has identifer !
          Identifier id2;
          if ((thisParameters->associatedSurface()).associatedDetectorElement() != nullptr &&
              (thisParameters->associatedSurface()).associatedDetectorElement()->identify() != 0) {
            id2 = (thisParameters->associatedSurface()).associatedDetectorElement()->identify();
          } else {
            ATH_MSG_VERBOSE("Surface has no detector element ID, skip it");
            if (dynamic_cast<const Trk::Perigee*>(thisParameters)) {
              delete startParameters;
              startParameters = thisParameters->clone();
            }
            continue;
          }

          // check if it is Si or Pixel
          if (!(m_atlasId->is_pixel(id2) || m_atlasId->is_sct(id2))) {
            ATH_MSG_VERBOSE("Surface is not Pixel or SCT, stop loop over parameters in this step");
          
            // for collisions, we want to stop at the first trt measurement; whereas for cosmics not
            // here we will have trt measurements on the track before the first si measurement!
            if (dynamic_cast<const Trk::Perigee*>(thisParameters)) {
              delete startParameters;
              startParameters = thisParameters->clone();
            }
            if (m_cosmic) continue;
            else break;
          }
      
          // see if this surface is in the list
          std::map<const Identifier, const Trk::TrackStateOnSurface*>::iterator iTSOS = mapOfHits.find(id2);
      
          if (iTSOS == mapOfHits.end() && !foundFirst) {
            ATH_MSG_VERBOSE("Si surface before first Si measurement, skip it and continue");
            continue;
          }
      
          // this is a measurement on the track ?
          if (iTSOS != mapOfHits.end()) {
            if (!foundFirst && !(*iterTSOS)->type(Trk::TrackStateOnSurface::Outlier)) {
              ATH_MSG_VERBOSE("Found first Si measurement !");
              foundFirst = true;
            }
          
            // is this a surface which might have a better prediction ?
            if (iTSOS->second->trackParameters()) {
              ATH_MSG_VERBOSE("Found track parameter on Si surface, take it");
              delete startParameters;
              startParameters = iTSOS->second->trackParameters()->clone();
            } else {
              ATH_MSG_VERBOSE("No parameter, take extrapolation");
              delete startParameters;
              startParameters = thisParameters->clone();
            }
          }
      
          // add surface, test insert !
          const Trk::TrackParameters *clonepar=thisParameters->clone();
          std::pair<const Trk::TrackParameters*,const bool> trackparampair (clonepar,true);
          if (mapOfPredictions.insert(std::pair<const Identifier, std::pair<const Trk::TrackParameters*,const bool> >(id2,trackparampair)).second) {
            ATH_MSG_VERBOSE("Added Si surface to mapOfPredictions");
          } else {
            ATH_MSG_VERBOSE("Had this, it is a double, skipped");
            delete clonepar;
            clonepar = nullptr;
          }
        }
        
        // clean up the inputparameters
        if (paramList) {
          for (std::vector<const Trk::TrackParameters*>::const_iterator it = paramList->begin();
               it != paramList->end(); ++it) {
            delete *it;
          }
          delete paramList;
          paramList = nullptr;
        }
        
        if (!(m_atlasId->is_pixel(id) || m_atlasId->is_sct(id))) {
          ATH_MSG_VERBOSE("Target was no longer an Si element, break loop");
          break;
        }
        
      }
    }
    if ((*iterTSOS)->type(Trk::TrackStateOnSurface::Measurement) && !(*iterTSOS)->type(Trk::TrackStateOnSurface::Outlier)) measno++;
    if (measno==nmeas) break; // don't count holes after last measurement, this is done below...
  }


  // if last extrapolation is not TRT:
  // - if countDeadModulesAfterLastHit (robustness): search for dead modules
  // - if cosmics or extendedListOfHoles: search for more holes after the last measurement (and also dead modules)
  if (!m_atlasId->is_trt(id) && (m_countDeadModulesAfterLastHit || m_extendedListOfHoles || m_cosmic)) {
    ATH_MSG_DEBUG("Search for dead modules after the last Si measurement");
    if (m_extendedListOfHoles || m_cosmic) ATH_MSG_DEBUG("Search for extended list of holes");
      
    Trk::CylinderVolumeBounds* cylinderBounds = new Trk::CylinderVolumeBounds(560, 2750); 
    // don't delete the cylinderBounds -> it's taken care of by Trk::VOlume (see Trk::SharedObject)
    Trk::Volume* boundaryVol = new Trk::Volume(0, dynamic_cast<Trk::VolumeBounds*>(cylinderBounds)); 
    // extrapolate this parameter blindly to search for more Si hits (not very fast, I know)
    const std::vector<const Trk::TrackParameters*>* paramList = m_extrapolator->extrapolateBlindly(*startParameters,
                                                                                                   Trk::alongMomentum,
                                                                                                   false, partHyp,
                                                                                                   boundaryVol);
    if (!paramList) {
      ATH_MSG_VERBOSE("--> Did not manage to extrapolate to another surface, break loop");
    } else {    
      ATH_MSG_VERBOSE("Number of parameters in this step: " << paramList->size());
  
      // loop over the predictions and analyze them
      for (std::vector<const Trk::TrackParameters*>::const_iterator it = paramList->begin();
           it != paramList->end(); ++it) {
        // copy pointer
        const Trk::TrackParameters* thisParameter = *it;      
      
        // check if surface has identifer !
        Identifier id2;
        if (thisParameter->associatedSurface().associatedDetectorElement() != nullptr &&
            thisParameter->associatedSurface().associatedDetectorElement()->identify() != 0) {
          id2 = thisParameter->associatedSurface().associatedDetectorElement()->identify();
    
          // check if it is Si or Pixel
          if (!(m_atlasId->is_pixel(id2) || m_atlasId->is_sct(id2))) {
            ATH_MSG_VERBOSE("Surface is not Pixel or SCT, stop loop over parameters in this step");
            break;
          }
    
          // add surface, test insert !
          const Trk::TrackParameters *clonepar=thisParameter->clone();
          // JEF: bool parameter in trackparampair: flag weather this hit should be considered as hole; if not, just cound dead modules
          std::pair<const Trk::TrackParameters*,const bool> trackparampair (clonepar,m_extendedListOfHoles || m_cosmic);
          if (mapOfPredictions.insert(std::pair<const Identifier, std::pair<const Trk::TrackParameters*,const bool> >(id2,trackparampair)).second) {
            ATH_MSG_VERBOSE("Added Si surface");
          } else {
            ATH_MSG_VERBOSE("Had this, it is a double, skipped");
            delete clonepar;
            clonepar = nullptr;
          }
        } else {
          ATH_MSG_VERBOSE("Surface has no detector element ID, skip it");
        }
      
        // keep going... 
        delete startParameters;
        startParameters = thisParameter->clone();
      }
  
      // clean up the inputparameters
      if (paramList) {
        for (std::vector<const Trk::TrackParameters*>::const_iterator it = paramList->begin();
             it != paramList->end(); ++it) delete *it;
        delete paramList;
        paramList = nullptr;
      }
    }

    // gotta clean up ...
    delete boundaryVol;
    boundaryVol = nullptr;
    //delete cylinderBounds; cylinderBounds = 0; // Till : don't delete this guy, it's deleted already when the boundaryVol gets deleted !
  }
    
  ATH_MSG_DEBUG("Number of Predictions found: " << mapOfPredictions.size());
  
  delete startParameters;
  return true;
  
}

//=================================================================================================================
void InDet::InDetTrackHoleSearchTool::performHoleSearchStepWise(std::map<const Identifier, const Trk::TrackStateOnSurface*>& mapOfHits,
                                                                std::map<const Identifier, std::pair<const Trk::TrackParameters*, const bool> >& mapOfPredictions,
                                                                std::vector<int>* information,
                                                                std::vector<const Trk::TrackStateOnSurface*>* listOfHoles) const {
  /** This function looks for holes in a given set of TrackStateOnSurface (TSOS) within the Si-detectors.
      In order to do so, an extrapolation is performed from detector element to the next and compared to the ones in the TSOS.
      If surfaces other than the ones in the track are crossed, these are possible holes or dead modules. Checks for sensitivity of 
      struck material are performed.
      
      The function requires the TSOS to have either TrackParameters (should have) or a MeasurementBase (must have). The startPoint
      of an extrapolation is required to be a TP. In case only a MB is present, the extrapolation Startpoint is set by the
      result TP of the last extrapolation, if possible.
  */
  
  // counters to steer first/last Si hit logic
  unsigned int foundTSOS = 0;
  int  PixelHoles = 0, SctHoles = 0, SctDoubleHoles = 0, PixelDead=0, SctDead=0;
  std::set<Identifier> SctHoleIds;
  
  ATH_MSG_DEBUG("Start iteration");
  ATH_MSG_DEBUG("Number of hits+outliers: " << mapOfHits.size() << " and predicted parameters:" << mapOfPredictions.size());
  
  for (std::map<const Identifier,std::pair<const Trk::TrackParameters*,const bool> >::const_iterator it = mapOfPredictions.begin();
       it != mapOfPredictions.end(); ++it) {
      
    const Trk::TrackParameters* nextParameters = (it->second).first;
      
    Identifier id = nextParameters->associatedSurface().associatedDetectorElement()->identify();
      
    // search for this ID in the list
    std::map<const Identifier, const Trk::TrackStateOnSurface*>::iterator iTSOS = mapOfHits.find(id);
      
    if (iTSOS == mapOfHits.end()) { 
      bool isgood = true;
      if (!isSensitive(nextParameters, isgood)) {
        if (isgood) ATH_MSG_VERBOSE("Extrapolation not in sensitive area, ignore and continue");

        if (!isgood) {
          if (m_atlasId->is_pixel(id)) {
        
            ATH_MSG_VERBOSE("Found element is a dead pixel module, add it to the list and continue");
            ++PixelDead;
          } else if (m_atlasId->is_sct(id)) {
        
            ATH_MSG_VERBOSE("Found element is a dead SCT module, add it to the list and continue");
            ++SctDead;
          }
        }
        continue;
      }
    
      // increment tmp counters only if this detElement should be considered for a proper holesearch
      // this info is the boolean in the (mapOfPredictions->second).second
      if (((it->second).second)) {
        if (m_atlasId->is_pixel(id)) {
          ATH_MSG_VERBOSE("Found element is a Pixel hole, add it to the list and continue");
          ++PixelHoles;
        } else if (m_atlasId->is_sct(id)) {
          ATH_MSG_VERBOSE("Found element is a SCT hole, add it to the list and continue");
          ++SctHoles;
                  
          // check double sct 
          // obtain backside of SCT module
          const InDetDD::SiDetectorElement* thisElement = 
            dynamic_cast<const InDetDD::SiDetectorElement *> (nextParameters->associatedSurface().associatedDetectorElement());
          if (!thisElement) {
            ATH_MSG_ERROR ("cast to SiDetectorElement failed, should never happen !");
            continue;
          }
                  
          const Identifier otherId = thisElement->otherSide()->identify();
          // loop over holes and look for the other one
          if (SctHoleIds.find(otherId) != SctHoleIds.end()) {
            ATH_MSG_VERBOSE("Found an SCT double hole !!!");
            ++SctDoubleHoles;
          }
          // keep this id for double side check
          SctHoleIds.insert(id);
      
        }
        // add to tmp list of holes
        if (listOfHoles) listOfHoles->push_back(createHoleTSOS(nextParameters));
        continue;
      } else {
        continue;
      }
    } // end (iTSOS == mapOfHits.end())
      
    if (iTSOS->second->type(Trk::TrackStateOnSurface::Outlier)) {
      ++foundTSOS;
      ATH_MSG_VERBOSE("Found TSOS is an outlier, not a hole, skip it and continue");
      // remove this one from the map
      mapOfHits.erase(iTSOS);
      continue;
    }
      
    if (iTSOS->second->type(Trk::TrackStateOnSurface::Measurement)) {
      ++foundTSOS;
      ATH_MSG_VERBOSE("Found TSOS is a measurement, continue");
      // remove this one from the map
      mapOfHits.erase(iTSOS);
      continue;
    }
  } // end of loop
  
  ATH_MSG_DEBUG("==> Total number of holes found: " 
                << PixelHoles << " Pixel holes, "
                << SctHoles << " Sct holes, "
                << SctDoubleHoles << " Double holes");
  
  if (listOfHoles) ATH_MSG_DEBUG("==> Size of listOfHoles: " << listOfHoles->size());
  
  if (mapOfHits.size() != 0) {
    int ioutliers = 0, imeasurements = 0;
    for (std::map<const Identifier, const Trk::TrackStateOnSurface*>::const_iterator iter = mapOfHits.begin(); 
         iter != mapOfHits.end(); ++iter) {
      if (iter->second->type(Trk::TrackStateOnSurface::Outlier)) ++ioutliers;
      else if (iter->second->type(Trk::TrackStateOnSurface::Measurement)) ++imeasurements;
      else ATH_MSG_ERROR("Found wrong TSOS in map !!!");
    }

    if (imeasurements > 0) {
      if (PixelHoles+SctHoles+SctDoubleHoles > 0) {
        ATH_MSG_DEBUG("Not all measurements found, but holes. Left measurements: "
                      << imeasurements << " outliers: " << ioutliers << " found: " << foundTSOS
                      << " Pixel holes: " << PixelHoles << " Sct holes: " << SctHoles 
                      << " Double holes: " << SctDoubleHoles);
      } else {
        ATH_MSG_DEBUG("Problem ? Not all measurements found. Left measurements: "
                      << imeasurements << " outliers: " << ioutliers << " found: " << foundTSOS);
      }
    }
  }
  
  // update information and return
  if (information) {
    (*information)[Trk::numberOfPixelHoles]       = PixelHoles;
    (*information)[Trk::numberOfSCTHoles]         = SctHoles;
    (*information)[Trk::numberOfSCTDoubleHoles]   = SctDoubleHoles;
    (*information)[Trk::numberOfPixelDeadSensors] = PixelDead;
    (*information)[Trk::numberOfSCTDeadSensors]   = SctDead;
  }
  
  return;
}

// ====================================================================================================================
bool InDet::InDetTrackHoleSearchTool::isSensitive(const Trk::TrackParameters* parameters, bool &isgood) const {
  // do strict boundary check for SCT and Pixels to make sure we are in active area
  
  if (!parameters ||
      !parameters->associatedSurface().associatedDetectorElement()) return false;
  
  const InDetDD::SiDetectorElement* siElement =
    dynamic_cast<const InDetDD::SiDetectorElement*> (parameters->associatedSurface().associatedDetectorElement());
  if (siElement == nullptr) {
    // -------  in dubio pro reo --> return false (is assumed insensitive)
    ATH_MSG_DEBUG("TrackParameters do not belong to a Si Element");
    return false;
  }
  
  double phitol;
  double etatol;
  
  if (parameters->covariance()) {
    phitol = m_phitol * sqrt((*parameters->covariance())(Trk::locX,Trk::locX));
    etatol = m_etatol * sqrt((*parameters->covariance())(Trk::locY,Trk::locY));
  } else {
    phitol = 2.5;
    etatol = 5.0;
  }
  // not on bond gap within tolerance
  if (siElement->nearBondGap(parameters->localPosition(), etatol)) {
    ATH_MSG_VERBOSE("---> extrapolation on bond gap within " << etatol << ", return");
    return false;
  }

  bool isActiveElement = true;
  // inside detector within tolerance
  InDetDD::SiIntersect siIn = siElement->inDetector(parameters->localPosition(), phitol, etatol);
  if (!siIn.in()) {
    ATH_MSG_VERBOSE("---> extrapolation not inside (active?) det"
                    << "ector within "<<phitol<<" "<<etatol<<", but check for dead module anyway");
    isActiveElement=false;
  }
  
  // errors might be too big (especially for track seeds or short tracks)
  // check for dead modules if extrapolation with smaller errors are in active detector (2.5 5.)
  if (!isActiveElement) {
    if (phitol>2.5 || etatol>5) {
      siIn = siElement->inDetector(parameters->localPosition(), 2.5, 5.);
      if (!siIn.in()) {
        ATH_MSG_VERBOSE("extrapolation too close to inactive detector; abort search for dead module");
        return false;
      }
    } else {
      ATH_MSG_VERBOSE("extrapolation precise enough and too close to inactive detector; abort search for dead module");
      return false;
    }
  }

  Identifier     id     = siElement->identify();
  IdentifierHash idHash = siElement->identifyHash();
  // check if it is a dead module using conditions services ! 
  if (m_atlasId->is_pixel(id)) { 
    if (m_usepix) {
      ATH_MSG_VERBOSE("Found element is a Pixel module without a hit, see if it might be dead");
      isgood=m_pixelCondSummaryTool->isGood(idHash);
      isgood=m_pixelLayerTool->expectHit(parameters);
      if (isgood) {
        // this detElement is only cosidered as hole if the extrapolation of
        // the track plus its error hits the active material
        if (isActiveElement) {
          ATH_MSG_VERBOSE("Pixel module is good, this is a hole canditate !");
          return true;
        }
      } else {
        ATH_MSG_VERBOSE("Track is hitting a bad Pixel module, this is not a hole candidate!");
        return false;
      }
    }
  } else if (m_atlasId->is_sct(id)) {
    if (m_usesct) {
      ATH_MSG_VERBOSE("Found element is a SCT module without a hit, see if it might be dead");
      isgood=m_sctCondSummaryTool->isGood(idHash);
      if (isgood) {
        // this detElement is only cosidered as hole if the extrapolation of
        // the track plus its error hits the active material
        if (isActiveElement) {

          if (m_checkBadSCTChip and isBadSCTChipStrip(id, *parameters, *siElement)) {
            ATH_MSG_VERBOSE("Track is hiting a bad SCT chip, this is not a hole candidate!");
            isgood = false;
            return false;
          }

          ATH_MSG_VERBOSE("SCT module is good, this is a hole candidate !");
          return true;
        }
      } else {
        ATH_MSG_VERBOSE("Track is hiting a bad SCT module, this is not a hole candidate!");
        return false;
      }
    }
  } else {
    ATH_MSG_WARNING("unknown identifier type, this should not happen !"); 
    return false;
  }
  // the extrapolation of the track plus its error might not 
  // be inside the active detector for a good module
  return false; 
}

// ====================================================================================================================
const Trk::TrackStateOnSurface* InDet::InDetTrackHoleSearchTool::createHoleTSOS(const Trk::TrackParameters* trackPar) const {
  std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
  typePattern.set(Trk::TrackStateOnSurface::Hole);
  const Trk::TrackStateOnSurface* tsos = new Trk::TrackStateOnSurface(0,trackPar->clone(),0,0,typePattern);
  return tsos;
}

// ====================================================================================================================
const Trk::Track*  InDet::InDetTrackHoleSearchTool::addHolesToTrack(const Trk::Track& oldTrack, 
                                                                    std::vector<const Trk::TrackStateOnSurface*>* listOfHoles) const {
  DataVector<const Trk::TrackStateOnSurface>* trackTSOS = new DataVector<const Trk::TrackStateOnSurface>;

  // get states from track 
  for (DataVector<const Trk::TrackStateOnSurface>::const_iterator it = oldTrack.trackStateOnSurfaces()->begin();
       it != oldTrack.trackStateOnSurfaces()->end(); ++it) {
    // veto old holes
    if (!(*it)->type(Trk::TrackStateOnSurface::Hole)) trackTSOS->push_back(new Trk::TrackStateOnSurface(**it));
  }

  // if we have no holes on the old track and no holes found by search, then we just copy the track
  if (oldTrack.trackStateOnSurfaces()->size() == trackTSOS->size() && listOfHoles->size() == 0) {
    ATH_MSG_DEBUG("No holes on track, copy input track to new track");
    // create copy of track
    const Trk::Track* newTrack = new Trk::Track(oldTrack.info(),trackTSOS,oldTrack.fitQuality() ? oldTrack.fitQuality()->clone() : nullptr);
    return newTrack;
  }

  // add new holes
  for (std::vector<const Trk::TrackStateOnSurface*>::const_iterator it = listOfHoles->begin();
       it != listOfHoles->end(); ++it) {
    trackTSOS->push_back(*it);
  }

  // sort
  const Trk::TrackParameters* perigee = oldTrack.perigeeParameters();
  if (!perigee) perigee = (*(oldTrack.trackStateOnSurfaces()->begin()))->trackParameters();

  if (perigee) {

    Trk::TrackStateOnSurfaceComparisonFunction* CompFunc = 
      new Trk::TrackStateOnSurfaceComparisonFunction(perigee->momentum());
  
    // we always have to sort holes in 
    // if (!is_sorted(trackTSOS->begin(),trackTSOS->end(), *CompFunc)) {
      
    if (fabs(perigee->parameters()[Trk::qOverP]) > 0.002) {
      /* invest n*(logN)**2 sorting time for lowPt, coping with a possibly
         not 100% transitive comparison functor.
         DataVector doesn't have stable sort, so we need to tamper with
         its vector content in order to avoid sort to get caught in DV full
         object ownership */
      ATH_MSG_DEBUG("sorting vector with stable_sort ");
      std::vector<const Trk::TrackStateOnSurface*>* PtrVector
        = const_cast<std::vector<const Trk::TrackStateOnSurface*>* > (&trackTSOS->stdcont());
      stable_sort(PtrVector->begin(), PtrVector->end(), *CompFunc);
    } else {
      trackTSOS->sort(*CompFunc); // respects DV object ownership
    }

    delete CompFunc;
  }

  // create copy of track
  const Trk::Track* newTrack = new Trk::Track(oldTrack.info(),trackTSOS,oldTrack.fitQuality() ? oldTrack.fitQuality()->clone() : nullptr);

  return newTrack;
}

// ====================================================================================================================
bool InDet::InDetTrackHoleSearchTool::isBadSCTChipStrip(const Identifier& waferId, 
                                                        const Trk::TrackParameters& parameters,
                                                        const InDetDD::SiDetectorElement& siElement) const {
  // Check if the track passes through a bad SCT ABCD chip or a bad SCT strip.
  // A chip and a strip are determined by the extrapolated position.
  // Algorithm is based on InnerDetector/InDetMonitoring/SCT_Monitoring/src/SCTHitEffMonTool.cxx

  // Check the input.
  if (not m_atlasId->is_sct(waferId)) {
    ATH_MSG_WARNING(waferId << " is not an SCT Identifier");
    return true;
  }

  // Get strip id from local position.
  // Due to the limited position resolution, we may pick up a neighboring strip...
  const Amg::Vector2D localPos(parameters.localPosition());
  const Identifier stripIdentifier(siElement.identifierOfPosition(localPos));
  if (not m_atlasId->is_sct(stripIdentifier)) {
    ATH_MSG_WARNING(stripIdentifier << " is not an SCT Identifier");
    return true;
  }

  // The extrapolated position is on a bad chip.
  if (not m_sctCondSummaryTool->isGood(stripIdentifier, InDetConditions::SCT_CHIP)) return true;
  // The extrapolated position is on a bad strip. (We may need to check neighboring strips.)
  if (not m_sctCondSummaryTool->isGood(stripIdentifier, InDetConditions::SCT_STRIP)) return true;
  return false;
}
