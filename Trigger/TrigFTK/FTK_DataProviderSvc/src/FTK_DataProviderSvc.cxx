/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FTK_DataProviderSvc/FTK_DataProviderSvc.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetReadoutGeometry/SCT_ModuleSideDesign.h"
#include "InDetReadoutGeometry/SCT_BarrelModuleSideDesign.h"
#include "InDetReadoutGeometry/SCT_ForwardModuleSideDesign.h"

#include "IRegionSelector/IRoiDescriptor.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkFitterUtils/FitterTypes.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkParameters/TrackParameters.h"

#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "StoreGate/DataHandle.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"

#include "AtlasDetDescr/AtlasDetectorID.h"

#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "EventInfo/EventIncident.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "PixelConditionsServices/IPixelOfflineCalibSvc.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkTrackSummary/TrackSummary.h"

#include "FTK_DataProviderInterfaces/IFTK_UncertaintyTool.h"

#include "TrkToolInterfaces/ITrackParticleCreatorTool.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "InDetRecToolInterfaces/IVertexFinder.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/VxCandidate.h"



#include <map>

#define DEBUGMSG MSG::DEBUG
#define VERBOSEMSG MSG::VERBOSE

using Gaudi::Units::micrometer;
using namespace InDet;

FTK_DataProviderSvc::FTK_DataProviderSvc(const std::string& name, ISvcLocator* svc):
  AthService(name, svc),
  m_RDO_key("FTK_RDO_Tracks"),
  m_storeGate(0),
  m_offlineCalibSvc("PixelOfflineCalibSvc", name),
  m_pixelId(0),
  m_sctId(0),
  m_pixelManager(0),
  m_SCT_Manager(0),
  m_id_helper(0),
  m_uncertaintyTool("FTK_UncertaintyTool"),
  m_trackFitter("Trk::GlobalChi2Fitter/TrigFTKTrackFitter"),
  m_trackSumTool("Trk::TrackSummaryTool/InDetTrigTrackSummaryToolSharedHits"),
  m_particleCreatorTool("Trk::ParticleCreatorTool/InDetTrigParticleCreatorTool"),
  m_VertexFinderTool("InDet::InDetIterativePriVxFinderTool/InDetTrigIterativePriVxFinderTool"),
  m_ftk_tracks(nullptr),
  m_conv_tracks(nullptr),
  m_refit_tracks(nullptr),
  m_conv_tp(nullptr),
  m_refit_tp(nullptr),
  m_conv_tpAuxCont(nullptr),
  m_refit_tpAuxCont(nullptr),
  m_conv_vx(nullptr),
  m_refit_vx(nullptr),
  m_got_conv_vx(false),
  m_got_refit_vx(false),
  m_conv_vertex(nullptr),
  m_refit_vertex(nullptr),
  m_got_conv_vertex(false),
  m_got_refit_vertex(false),
  m_conv_track_map(0),
  m_refit_track_map(0),
  m_conv_tp_map(0),
  m_refit_tp_map(0),
  m_newEvent(true),
  m_gotRawTracks(false),
  m_trackCacheName("FTKcache_Track"),
  m_trackParticleCacheName("FTKcache_TrackParticle"),
  m_VxContainerCacheName("FTKcache_VxContainer"),
  m_VertexContainerCacheName("FTKcache_VertexContainer"),
  athenaLog(0)
{
  athenaLog = new MsgStream(msgSvc(), name);
  declareProperty( "TrackFitter", m_trackFitter);
  declareProperty("UncertaintyTool",m_uncertaintyTool);
  declareProperty("TrackSummaryTool", m_trackSumTool);
  declareProperty("TrackParticleCreatorTool", m_particleCreatorTool);
  declareProperty("VertexFinderTool",m_VertexFinderTool);
}

FTK_DataProviderSvc::~FTK_DataProviderSvc(){
  delete athenaLog;
}

StatusCode FTK_DataProviderSvc::initialize() {
  
  /// Setup StoreGateSvc ///
  
  StatusCode sc = service( "StoreGateSvc", m_storeGate );
  if (sc.isFailure()) {
    (*athenaLog) << MSG::FATAL << "Unable to retrieve StoreGate service" << endreq;
    return sc;
  }  
  
  StoreGateSvc* detStore;
  sc = service("DetectorStore", detStore);
  if (sc.isFailure()) {
    (*athenaLog) << MSG::FATAL << "Unable to retrieve Detector Store service" << endreq;
    return sc;
  }  

  /// Setup Pixel Services ///
  sc = m_offlineCalibSvc.retrieve();
  if (sc.isFailure() || !m_offlineCalibSvc ) {
    if ( !m_offlineCalibSvc.empty() ) {
      (*athenaLog) << MSG::FATAL << "Offline Calibration Svc not found! " << endreq;
      return sc;
    }
  }
  
  if (detStore->retrieve(m_pixelId, "PixelID").isFailure()) {
    (*athenaLog) << MSG::FATAL << "Pixel ID not found! " << endreq;
    return StatusCode::FAILURE;
  }
  
  if (detStore->retrieve(m_sctId, "SCT_ID").isFailure()) {
    (*athenaLog) << MSG::FATAL << "SCT ID not found! " << endreq;
    return StatusCode::FAILURE;
  }
  
  sc = detStore->retrieve(m_pixelManager);
  if( sc.isFailure() ) {
    (*athenaLog) << MSG::FATAL << "Pixel Manager not found! " << endreq;
    return sc;
  }
  
  sc = detStore->retrieve(m_SCT_Manager);
  if( sc.isFailure() ) {
    (*athenaLog) << MSG::FATAL << "SCT Manager not found! " << endreq;
    return sc;
  }
  
  if (detStore->retrieve(m_id_helper, "AtlasID").isFailure()) {
    (*athenaLog) << MSG::FATAL << "ATLASID  not found! " << endreq;
    return StatusCode::FAILURE;
  }

  // Get the Uncertainty Tool
  if (m_uncertaintyTool.retrieve().isFailure() ) {
    (*athenaLog) << MSG::ERROR << "Failed to retrieve tool " << m_uncertaintyTool << endreq;
    return StatusCode::FAILURE;
  }

  
  sc = m_trackFitter.retrieve();
  if(sc.isFailure()) {
    (*athenaLog) << MSG::FATAL << "Track Fitter not found! " << endreq;
    return sc;
  } else {
    (*athenaLog) << MSG::INFO << " Retrieved " << m_trackFitter << endreq;
  }
  
  sc = m_trackSumTool.retrieve();
  if(sc.isFailure()) {
    (*athenaLog) << MSG::FATAL << "TrackSummaryTool not found! " << endreq;
    return sc;
  } else {
    (*athenaLog) << MSG::INFO << " Retrieved " << m_trackSumTool << endreq;
  }
  
  sc = m_particleCreatorTool.retrieve();
  if(sc.isFailure()) {
    (*athenaLog) << MSG::FATAL << "ParticleCreatorTool not found! " << endreq;
    return sc;
  } else {
    (*athenaLog) << MSG::INFO << " Retrieved " << m_particleCreatorTool << endreq;
  }

  sc = m_VertexFinderTool.retrieve();
  if(sc.isFailure()) {
    (*athenaLog) << MSG::FATAL << "VertexFinderTool not found! " << endreq;
    return sc;
  } else {
    (*athenaLog) << MSG::INFO << " Retrieved " << m_VertexFinderTool << endreq;
  }



  // Register incident handler
  ServiceHandle<IIncidentSvc> iincSvc( "IncidentSvc", name());
  sc = iincSvc.retrieve().isSuccess();
  if (sc.isFailure()){
    (*athenaLog) << MSG::FATAL << "Unble to locate IncidentSvc" << endreq;
    return sc;
  } else {
    iincSvc->addListener( this, IncidentType::EndEvent);
  }

  (*athenaLog) << MSG::INFO << "Service Initialized" << endreq;
  return sc; 

}

StatusCode FTK_DataProviderSvc::finalize() {
  return StatusCode::SUCCESS;
}


xAOD::TrackParticleContainer* FTK_DataProviderSvc::getTrackParticlesInRoi(const IRoiDescriptor& roi, const bool withRefit){

  StatusCode sc=StatusCode::SUCCESS;

  xAOD::TrackParticleContainer *tpcont = new xAOD::TrackParticleContainer(SG::VIEW_ELEMENTS);
  if (initTrackParticleCache(withRefit).isFailure()) {
    return tpcont;
  }
  
  if (withRefit) {
    for (unsigned int ftk_track_index = 0; ftk_track_index != m_ftk_tracks->size(); ++ftk_track_index){

      const FTK_RawTrack* ftk_track= m_ftk_tracks->at(ftk_track_index);
      if (roi.containsPhi(ftk_track->getPhi()) && roi.contains(ftk_track->getZ0(), ftk_track->getCotTh())) {

	if( m_refit_tp_map[ftk_track_index]>-1) { 
	  tpcont->push_back(m_refit_tp->at((unsigned int) m_refit_tp_map[ftk_track_index]));
	} else {
	  Trk::Track *track = getCachedTrack(ftk_track_index, withRefit); 
	  if (track !=nullptr) {
	    const ElementLink<TrackCollection> trackLink(*m_refit_tracks, (unsigned int) m_refit_tp_map[ftk_track_index]);
	    
	    xAOD::TrackParticle* tp = m_particleCreatorTool->createParticle( trackLink, m_refit_tp);
	    if (tp !=nullptr) {
	      m_refit_tp_map[ftk_track_index] = (int) m_refit_tp->size()-1;
	      tpcont->push_back(tp);
	    }
	  }
	}
      }
    }
    (*athenaLog) << DEBUGMSG << "getTrackParticlesIn Roi: Returning TrackParticleContainer Size = " << tpcont->size() << endreq;
  } else { 
    for (unsigned int ftk_track_index = 0; ftk_track_index != m_ftk_tracks->size(); ++ftk_track_index){

      const FTK_RawTrack* ftk_track= m_ftk_tracks->at(ftk_track_index);
      if (roi.containsPhi(ftk_track->getPhi()) && roi.contains(ftk_track->getZ0(), ftk_track->getCotTh())) {

	if( m_conv_tp_map[ftk_track_index]>-1) { 
	  tpcont->push_back(m_conv_tp->at((unsigned int) m_conv_tp_map[ftk_track_index]));
	} else {
	  Trk::Track *track = getCachedTrack(ftk_track_index, withRefit); 
	  if (track != nullptr) {
	    const ElementLink<TrackCollection> trackLink(*m_conv_tracks, (unsigned int) m_conv_tp_map[ftk_track_index]);
	    
	    xAOD::TrackParticle* tp = m_particleCreatorTool->createParticle( trackLink, m_conv_tp);
	    if (tp !=nullptr) {
	      m_conv_tp_map[ftk_track_index] = (int) m_conv_tp->size()-1;
	      tpcont->push_back(tp);
	    }
	  }
	}
      }
    }
    (*athenaLog) << DEBUGMSG << "getTrackParticlesinRoI: Returning refitted TrackParticleContainer Size = " << tpcont->size() << endreq;
  }    
  
  return tpcont;
}



StatusCode FTK_DataProviderSvc::fillTrackParticleCache(const bool withRefit){

  StatusCode sc = StatusCode::SUCCESS;
  
  if (initTrackParticleCache(withRefit).isFailure()) {
    return StatusCode::FAILURE;
  }
  
  if (withRefit) { // create TrackParticle from Refitted track
    for (unsigned int ftk_track_index = 0; ftk_track_index != m_ftk_tracks->size(); ++ftk_track_index){
      if( m_refit_tp_map[ftk_track_index]<0) { 
	Trk::Track *track = getCachedTrack(ftk_track_index, withRefit); 
	if (track != nullptr) {
	  (*athenaLog) << DEBUGMSG << "creating ElementLink" << endreq;
	  const ElementLink<TrackCollection> trackLink(&(*track), *m_refit_tracks);
	  
	  xAOD::TrackParticle* tp = m_particleCreatorTool->createParticle( trackLink, m_refit_tp);
	  if (tp !=nullptr) {
	    m_refit_tp_map[ftk_track_index] = (int) m_refit_tp->size()-1;
	    (*athenaLog) << DEBUGMSG << "TrackParticle from refitted track added to cache at index " << m_refit_tp_map[ftk_track_index] 
			 << "  created from FTK track at index " << ftk_track_index << endreq;
	  }
	}
      }
    }
    (*athenaLog) << DEBUGMSG << "fillTrackParticleCache: Returning TrackParticleContainer Size = " << m_refit_tp->size() << endreq;

  } else {// create TrackParticle from converted track

    for (unsigned int ftk_track_index = 0; ftk_track_index != m_ftk_tracks->size(); ++ftk_track_index){

      if( m_conv_tp_map[ftk_track_index]<0) { 
	Trk::Track *track = getCachedTrack(ftk_track_index, withRefit); 
	if (track !=nullptr) {
	  (*athenaLog) << DEBUGMSG << "creating ElementLink" << endreq;
	  const ElementLink<TrackCollection> trackLink(&(*track), *m_conv_tracks);
	  
	  (*athenaLog) << DEBUGMSG << "creating TrackParticle" << endreq;
	  xAOD::TrackParticle* tp = m_particleCreatorTool->createParticle( trackLink, m_conv_tp);
	  if (tp !=nullptr) {
	    m_conv_tp_map[ftk_track_index] = (int) m_conv_tp->size()-1;
	    (*athenaLog) << DEBUGMSG << "TrackParticle from converted track added to cache at index " << m_conv_tp_map[ftk_track_index] 
			 << "  created from FTK track at index " << ftk_track_index << endreq;
	  }
	}
      }
    }

    (*athenaLog) << DEBUGMSG << "fillTrackParticleCache: Returning TrackParticleContainer Size = " << m_conv_tp->size() << endreq;
  }
  
  return sc;
}

xAOD::TrackParticleContainer* FTK_DataProviderSvc::getTrackParticles(const bool withRefit){
  
  xAOD::TrackParticleContainer *tpcont = new xAOD::TrackParticleContainer(SG::VIEW_ELEMENTS);

  if (fillTrackParticleCache(withRefit).isFailure()) {
    return tpcont;
  }
  
  if (withRefit) {
    for (auto tp = m_refit_tp->begin(); tp != m_refit_tp->end(); tp++) {
      tpcont->push_back(*tp);
    } 
    (*athenaLog) << DEBUGMSG << "getTrackParticles: Returning TrackParticleContainer Size = " << tpcont->size() << endreq;
  } else {
    for (auto tp = m_conv_tp->begin(); tp != m_conv_tp->end(); tp++) {
      tpcont->push_back(*tp);
    } 
    (*athenaLog) << DEBUGMSG << "getTrackParticles: Returning converted TrackParticleContainer Size = " << tpcont->size() << endreq;
  }
  
  return tpcont;
}



StatusCode FTK_DataProviderSvc::fillTrackCache(const bool withRefit) {

  // Takes empty or partially filled track cache and fills it up 
  // by converting raw FTK tracks and optionally performing a refit when withRefit==true


  StatusCode sc = StatusCode::SUCCESS;
  
  /// If we're at the start of a new event, get the tracks from StoreGate 
  if (getFTK_RawTracksFromSG().isFailure()) {
    return StatusCode::FAILURE; 
  }    
  if (initTrackCache(withRefit).isFailure()) {
    return StatusCode::FAILURE; 
  }

  for (unsigned int ftk_track_index = 0; ftk_track_index != m_ftk_tracks->size(); ++ftk_track_index){
    
    Trk::Track *track=nullptr;
    
    if( m_conv_track_map[ftk_track_index]==-1) { /// Track needs to be converted ///
      (*athenaLog) << VERBOSEMSG << "fillTrackCache: FTK Track " << ftk_track_index << " not yet converted, beginning conversion" << endreq;
      Trk::Track *new_track = ConvertTrack(ftk_track_index);
      m_conv_track_map[ftk_track_index] = (int) m_conv_tracks->size();
      (*athenaLog) << VERBOSEMSG << "fillTrackCache:  adding coverted track to cache at index " <<  m_conv_tracks->size() << " corresponding to FTK track " << ftk_track_index << endreq;
      m_conv_tracks->push_back(new_track);
      track = m_conv_tracks->at((unsigned int) m_conv_track_map[ftk_track_index]);
      (*athenaLog) << VERBOSEMSG << "fillTrackCache:  add track summary information to converted track " << endreq;
      m_trackSumTool->updateTrack(*track);
    } 
    if (m_conv_track_map[ftk_track_index]>-1 && withRefit && m_refit_track_map[ftk_track_index]==-1) { // need to do refit
      (*athenaLog) << VERBOSEMSG << "fillTrackCache:Beginning Track Refit for FTK track " << ftk_track_index <<  endreq;
      Trk::Track *newRefitTrack = m_trackFitter->fit(*track, false, Trk::pion);
      if(newRefitTrack == nullptr || newRefitTrack == 0) {
	(*athenaLog) << DEBUGMSG << "fillTrackCache: refit failed - refitted track coresponding to FTK track with index " << ftk_track_index << " has NOT added to  output collection" << endreq;
	m_refit_track_map[ftk_track_index] = -2; // flag so we don't try to refit again
      } else { 
	(*athenaLog) << VERBOSEMSG << "fillTrackCache: Track Refit SUCEEDED " << endreq; 
	(*athenaLog) << VERBOSEMSG << "fillTrackCache:  adding refitted track to cache at index " <<  m_refit_tracks->size() << " corrsponding to FTK track " << ftk_track_index << endreq;
	m_refit_track_map[ftk_track_index] = (int) m_refit_tracks->size();
	m_refit_tracks->push_back(newRefitTrack);
	(*athenaLog) << VERBOSEMSG << "fillTrackCache:  adding track summary information to refitted track " << endreq;
	Trk::Track *refitTrack = m_refit_tracks->at((unsigned int) m_refit_track_map[ftk_track_index]);
	m_trackSumTool->updateTrack(*refitTrack);
      }
    }
  }
  
  if (withRefit) {
    (*athenaLog) << DEBUGMSG << "fillTrackCache: " << m_conv_tracks->size() << " converted tracks and " << m_refit_tracks->size() << " refitted tracks" <<  endreq;
  } else { 
    (*athenaLog) << DEBUGMSG << "fillTrackCache: " << m_conv_tracks->size() << " converted tracks" << endreq;
  }
  

  return sc;
}

bool FTK_DataProviderSvc::fillVxContainer(bool withRefit, TrackCollection* tracks) {
  
  bool gotVertices=false;
  
  if (tracks->size() > 0) {
    (*athenaLog) << DEBUGMSG << "fillVxContainer: finding vertices from " << tracks->size() << " tracks " << endreq;
    VxContainer* vertices = m_VertexFinderTool->findVertex(tracks);
    if (vertices == nullptr) return gotVertices;
    std::string cacheName= m_VxContainerCacheName;
    if (withRefit) cacheName+="Refit";
    StatusCode sc = m_storeGate->record(vertices, cacheName);
    if (sc.isFailure()) {
      (*athenaLog) << MSG::ERROR << "fillVxContainer: Failed to record VxCollection " << cacheName  << endreq;
      delete(vertices);
      return gotVertices;
    }
    gotVertices = true;
    if (withRefit) {
      m_refit_vx= vertices;
    } else {
      m_conv_vx= vertices;
    }
    (*athenaLog) << DEBUGMSG << "fillVxContainer: returning " << vertices->size() <<  " vertices " << endreq;
  }
  return gotVertices;
}

VxContainer* FTK_DataProviderSvc::getVxContainer(const bool withRefit) {

  VxContainer *userVertex = new VxContainer(SG::VIEW_ELEMENTS);

  if (fillTrackCache(withRefit).isFailure()) return userVertex;

  if (withRefit) {
    if (!m_got_refit_vx) {
      (*athenaLog) << DEBUGMSG << "getVxContainer: filling VxContainer from refitted tracks " << endreq;
      m_got_refit_vx = fillVxContainer(withRefit, m_refit_tracks);
      (*athenaLog) << DEBUGMSG << "getVxContainer: found " << m_refit_vx->size() << " vertices from refitted tracks" << endreq;
    }
    if (m_got_refit_vx) {
      (*athenaLog) << DEBUGMSG << "getVxContainer: cache contains " << m_refit_vx->size() <<  " vertices from refitted tracks" << endreq;
      for (auto pv = m_refit_vx->begin(); pv != m_refit_vx->end(); ++pv) {
	userVertex->push_back(*pv);
      }
    }
  } else {
    if (!m_got_conv_vx) {
      (*athenaLog) << DEBUGMSG << "getVxContainer: filling VxContainer from converted tracks " << endreq;
      m_got_conv_vx = fillVxContainer(withRefit, m_conv_tracks);
      (*athenaLog) << DEBUGMSG << "getVxContainer: found " << m_conv_vx->size() << " vertices from converted tracks" << endreq;
    }
    if (m_got_conv_vx) {
      (*athenaLog) << DEBUGMSG << "getVxContainer: cache contains " << m_conv_vx->size() <<  " vertices from converted tracks" << endreq;
      for (auto pv = m_conv_vx->begin(); pv != m_conv_vx->end(); ++pv) {
	userVertex->push_back(*pv);
      }
    }
  }
  return userVertex;
}

bool FTK_DataProviderSvc::fillVertexContainerCache(bool withRefit, xAOD::TrackParticleContainer* tps) {
  
  bool gotVertices = false;
  if (tps->size() > 0) {
    (*athenaLog) << DEBUGMSG << "fillVertexContainerCache: finding vertices from " << tps->size() << " TrackParticles " << endreq;
    std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> vertices = m_VertexFinderTool->findVertex(tps);
    if (vertices.first == nullptr) return gotVertices;
    
    std::string cacheName= m_VertexContainerCacheName;
    if (withRefit) cacheName+="Refit";

    StatusCode sc = m_storeGate->record(vertices.first, cacheName);
    if (sc.isFailure()) {
      (*athenaLog) << MSG::ERROR << "fillVertexContainerCache: Failed to record VertexCollection " << cacheName  << endreq;
      delete(vertices.first);
      delete(vertices.second);
      return gotVertices;
    }
    sc = m_storeGate->record(vertices.second, cacheName+"Aux");
    if (sc.isFailure()) {
      (*athenaLog) << MSG::ERROR << "fillVertexContainerCache: Failed to record VertexAuxCollection " << cacheName  << endreq;
      delete(vertices.second);
      return gotVertices;
    }
    if (withRefit) {
      m_refit_vertex = vertices.first;
    } else {
      m_conv_vertex = vertices.first;
    }      
    gotVertices=true;
  }
  
  return gotVertices;
}

xAOD::VertexContainer* FTK_DataProviderSvc::getVertexContainer(const bool withRefit){

  xAOD::VertexContainer* userVertex = new xAOD::VertexContainer(SG::VIEW_ELEMENTS);

  if (fillTrackParticleCache(withRefit).isFailure()) return userVertex;

  if (withRefit) { // get vertex from refitted tracks
    if (!m_got_refit_vertex) {
      (*athenaLog) << DEBUGMSG << "getVertexContainer: filling VertexContainer from refitted tracks " << endreq;
      m_got_refit_vertex = fillVertexContainerCache(withRefit, m_refit_tp);
    }
    if (m_got_refit_vertex) {
      (*athenaLog) << DEBUGMSG << "getVertexContainer: cache contains " << m_refit_vertex->size() <<  " vertices from refitted tracks" << endreq;
      for (auto pv = m_refit_vertex->begin(); pv != m_refit_vertex->end(); ++pv) {
	userVertex->push_back(*pv);
      }
    }
  } else {   // get vertex from converted tracks
    if (!m_got_conv_vertex) {
      (*athenaLog) << DEBUGMSG << "getVertexContainer: filling VertexContainer from converted tracks " << endreq;
      m_got_conv_vertex = fillVertexContainerCache(withRefit, m_conv_tp);
    }
    if (m_got_conv_vertex) {
      (*athenaLog) << DEBUGMSG << "getVertexContainer: cache contains " << m_conv_vertex->size() <<  " vertices from converted tracks" << endreq;
      for (auto pv = m_conv_vertex->begin(); pv != m_conv_vertex->end(); ++pv) {
	userVertex->push_back(*pv);
      }
    }
  }
  return userVertex;    
    
}


TrackCollection* FTK_DataProviderSvc::getTracks(const bool withRefit){

  TrackCollection *userTracks = new TrackCollection(SG::VIEW_ELEMENTS);

  /// If we're at the start of a new event, get the tracks from StoreGate 
  if (getFTK_RawTracksFromSG().isFailure()) {
      return userTracks; 
  }    
  if (initTrackCache(withRefit).isFailure()) {
    return userTracks; 
  }

  /// Do conversion or return already converted tracks   
  (*athenaLog) << DEBUGMSG << "getTracks: Raw FTK Container Size = " << m_ftk_tracks->size() << endreq;
  (*athenaLog) << VERBOSEMSG << "getTracks: Converting Tracks" << endreq;
  for (unsigned int ftk_track_index = 0; ftk_track_index != m_ftk_tracks->size(); ++ftk_track_index){

    Trk::Track* track = this->getCachedTrack(ftk_track_index, withRefit);
    if (track != nullptr) userTracks->push_back(track);
  }
  if (withRefit) {
    (*athenaLog) << DEBUGMSG << "getTracks: Return refitted TrackContainer Size = " << userTracks->size() << endreq;
  } else { 
    (*athenaLog) << DEBUGMSG << "getTracks: Return TrackContainer Size = " << userTracks->size() << endreq;
  }
  
  return userTracks;

}

TrackCollection* FTK_DataProviderSvc::getTracksInRoi(const IRoiDescriptor& roi, const bool withRefit){

  TrackCollection *userTracks = new TrackCollection(SG::VIEW_ELEMENTS);

  /// If we're at the start of a new event, get the tracks from StoreGate 
  if (getFTK_RawTracksFromSG().isFailure()) {
      return userTracks; 
  }    
  if (initTrackCache(withRefit).isFailure()) {
    return userTracks; 
  }

  /// Do conversion or return already converted tracks   
  (*athenaLog) << DEBUGMSG << "getTracksInRoi: Raw FTK Container Size = " << m_ftk_tracks->size() << endreq;
  (*athenaLog) << VERBOSEMSG << "getTracksInRoI: Converting Tracks" << endreq;
  for (unsigned int ftk_track_index = 0; ftk_track_index != m_ftk_tracks->size(); ++ftk_track_index){

    /// Check if the track is inside the ROI ///

    const FTK_RawTrack* ftk_track= m_ftk_tracks->at(ftk_track_index);
    if (roi.containsPhi(ftk_track->getPhi()) && roi.contains(ftk_track->getZ0(), ftk_track->getCotTh())) {
      
      Trk::Track* track = this->getCachedTrack(ftk_track_index, withRefit);
      if (track != nullptr) userTracks->push_back(track);
    }
  }
  if (withRefit) {
    (*athenaLog) << DEBUGMSG << "getTracksinRoi: Return refitted TrackContainer Size = " << userTracks->size() << endreq;
  } else { 
    (*athenaLog) << DEBUGMSG << "getTracksinRoi: Return TrackContainer Size = " << userTracks->size() << endreq;
  }
  
  return userTracks;

}

Trk::Track* FTK_DataProviderSvc::getCachedTrack(const unsigned int ftk_track_index, const bool withRefit) {
  
  Trk::Track *track= nullptr;
  Trk::Track* refitTrack= nullptr;
  
  if( m_conv_track_map[ftk_track_index]>-1) { /// Track already converted ///
    track = m_conv_tracks->at((unsigned int) m_conv_track_map[ftk_track_index]);
    if (track == nullptr) {
      (*athenaLog) << VERBOSEMSG << "getCachedTrack:  failed to retrieve converted track with index " << m_conv_track_map[ftk_track_index] << " corresponding to FTK track " << ftk_track_index << endreq;
      return nullptr;
    } else {
      (*athenaLog) << VERBOSEMSG << "getCachedTrack:  retrieved converted track with index " << m_conv_track_map[ftk_track_index] << " corresponding to FTK track " << ftk_track_index << endreq;
    }
  
  } else { /// This track hasn't been converted yet, do it now ///                                                                                                                           

    (*athenaLog) << VERBOSEMSG << "getCachedTrack: ConversionLoop: FTK Track " << ftk_track_index << " not yet converted, beginning conversion" << endreq;
    Trk::Track *new_track = ConvertTrack(ftk_track_index);
    m_conv_track_map[ftk_track_index] = (int) m_conv_tracks->size();
    (*athenaLog) << VERBOSEMSG << "getCachedTrack:  adding coverted track to cache at index " <<  m_conv_tracks->size() << " corresponding to FTK track " << ftk_track_index << endreq;
    m_conv_tracks->push_back(new_track);
    track = m_conv_tracks->at((unsigned int) m_conv_track_map[ftk_track_index]);
    (*athenaLog) << VERBOSEMSG << "getCachedTrack: add track summary information to converted track" << endreq;
    m_trackSumTool->updateTrack(*track);

  } 
  
  if (!withRefit) return track;

  if  (m_refit_track_map[ftk_track_index]>-1){ 
    // track has already been refitted //
    refitTrack = m_refit_tracks->at((unsigned int) m_refit_track_map[ftk_track_index]);
    if (refitTrack == nullptr) {
      (*athenaLog) << VERBOSEMSG << "getCachedTrack:  failed to retrieve refitted track with index " << m_refit_track_map[ftk_track_index] << " corresponding to FTK track " << ftk_track_index << endreq;
      m_refit_track_map[ftk_track_index] = -2;
    }
  } else if (m_refit_track_map[ftk_track_index]!=-2){ 
    // Need to do refit//
    (*athenaLog) << VERBOSEMSG << "getCachedTrack: Beginning Track Refit for FTK track " << ftk_track_index <<  endreq;
    Trk::Track *newRefitTrack = m_trackFitter->fit(*track, false, Trk::pion);
    if(newRefitTrack == nullptr || newRefitTrack == 0 ) {
      (*athenaLog) << DEBUGMSG << "getCachedTrack: refit failed - refitted track coresponding to FTK track with index " << ftk_track_index << " has NOT added to  output collection" << endreq;
      m_refit_track_map[ftk_track_index] = -2;
      return nullptr;
    } else { 
      (*athenaLog) << VERBOSEMSG << "getCachedTrack: Track Refit SUCEEDED" << endreq;
      (*athenaLog) << VERBOSEMSG << "getCachedTrack:  adding refitted track to cache at index " <<  m_refit_tracks->size() << " corrsponding to FTK track " << ftk_track_index << endreq;
      m_refit_track_map[ftk_track_index] = (int) m_refit_tracks->size();
      m_refit_tracks->push_back(newRefitTrack);
      refitTrack = m_refit_tracks->at((unsigned int) m_refit_track_map[ftk_track_index]);
      (*athenaLog) << VERBOSEMSG << "getCachedTrack:  adding track summary information to refitted track" << endreq;
      m_trackSumTool->updateTrack(*refitTrack);
    }
  }
  (*athenaLog) << VERBOSEMSG << "getCachedTrack: returning refitted track " << endreq;
  return refitTrack;
}


StatusCode FTK_DataProviderSvc::getFTK_RawTracksFromSG(){
/// get the FTK Track pointers from StoreGate ///

  if (!m_newEvent) {
    return StatusCode::SUCCESS;
  }
  m_newEvent=false;
  // new event - get the tracks from StoreGate

  if (!m_storeGate->contains<FTK_RawTrackContainer>(m_RDO_key)) {
    (*athenaLog) << MSG::WARNING << "getFTK_RawTracksFromSG: FTK tracks  "<< m_RDO_key <<" not found in StoreGate !"<< endreq;
    return StatusCode::FAILURE;
  } else {
    (*athenaLog) << VERBOSEMSG << "getFTK_RawTracksFromSG:  Doing storegate retreive" << endreq;
    StatusCode sc = m_storeGate->retrieve(m_ftk_tracks, m_RDO_key);
    if (sc.isFailure()) {
      (*athenaLog) << VERBOSEMSG << "getFTK_RawTracksFromSG: Failed to get FTK Tracks Container" << endreq;
      return StatusCode::FAILURE;
    }
    (*athenaLog) << DEBUGMSG << "getFTK_RawTracksFromSG:  Got " << m_ftk_tracks->size() << " raw FTK tracks (RDO) from  StoreGate " << endreq;
    m_gotRawTracks = true;

  }
  return  StatusCode::SUCCESS;
}

StatusCode FTK_DataProviderSvc::initTrackCache(bool withRefit) {
  
  StatusCode sc=StatusCode::SUCCESS;
  
  if (!withRefit){
    if (!m_storeGate->contains<TrackCollection>(m_trackCacheName)) {
      
      m_conv_tracks = new TrackCollection;
      sc = m_storeGate->record(m_conv_tracks, m_trackCacheName);
      if (sc.isFailure()) {
	(*athenaLog) << MSG::ERROR << "initTrackCache: Failed to record TrackCollection " <<  m_trackCacheName << " No FTK tracks returned" << endreq;
	delete(m_conv_tracks);
	return sc;
      }
      for (unsigned int i = 0; i!=m_ftk_tracks->size(); i++) m_conv_track_map.push_back(-1);
      (*athenaLog) << VERBOSEMSG << "initTrackCache: converted track cache initialised " << endreq;
    }
  } else {
    if (!m_storeGate->contains<TrackCollection>(m_trackCacheName+"Refit")) {
      
      m_refit_tracks = new TrackCollection;
      sc = m_storeGate->record(m_refit_tracks, m_trackCacheName+"Refit");
      if (sc.isFailure()) {
	(*athenaLog) << MSG::ERROR << "initTrackCache: Failed to record TrackCollection " <<  m_trackCacheName << "Refit - No FTK tracks returned" << endreq;
	delete(m_refit_tracks);
	return sc;
      }
      for (unsigned int i = 0; i!=m_ftk_tracks->size(); i++) m_refit_track_map.push_back(-1);
      (*athenaLog) << VERBOSEMSG << "initTrackCache: refitted track cache initialised " << endreq;
    }
  }
  return sc;
}

StatusCode FTK_DataProviderSvc::initTrackParticleCache(bool withRefit) {
  StatusCode sc;

  if (withRefit) {

    if (!m_storeGate->contains<xAOD::TrackParticleContainer>(m_trackParticleCacheName+"Refit")) {

      m_refit_tp=  new xAOD::TrackParticleContainer();
      sc = m_storeGate->record(m_refit_tp, m_trackParticleCacheName+"Refit");
      if (sc.isFailure()) {
	(*athenaLog) << MSG::ERROR << "initTrackParticleCache: Failed to record TrackParticleContainer " <<  m_trackParticleCacheName << "Refit No FTK TrackspParticles returned" << endreq;
	delete(m_refit_tp);
	return sc;
      }
    }

    if (!m_storeGate->contains<xAOD::TrackParticleAuxContainer>(m_trackParticleCacheName+"AuxRefit")) {
      (*athenaLog) << VERBOSEMSG << "initTrackParticleCache: initialising auxstore for refitted TrackParticles" << endreq; 
      m_refit_tpAuxCont =  new xAOD::TrackParticleAuxContainer();
      sc = m_storeGate->record(m_refit_tpAuxCont, m_trackParticleCacheName+"AuxRefit");
      if (sc.isFailure()) {
	(*athenaLog) << MSG::ERROR << "initTrackParticleCache: Failed to record TrackParticleAuxCollection " <<  m_trackParticleCacheName << "AuxRefit No FTK tracks returned" << endreq;
	delete(m_refit_tpAuxCont);
	return sc;
      }
      
      m_refit_tp->setStore( m_refit_tpAuxCont);
      for (unsigned int i = 0; i!=m_ftk_tracks->size(); i++) m_refit_tp_map.push_back(-1);
      (*athenaLog) << VERBOSEMSG << "initTrackParticleCache: cache initialized for refitted TracksParticles" << endreq; 

    }
  } else {

    if (!m_storeGate->contains<xAOD::TrackParticleContainer>(m_trackParticleCacheName)) {
      m_conv_tp=  new xAOD::TrackParticleContainer();
      sc = m_storeGate->record(m_conv_tp, m_trackParticleCacheName);
      if (sc.isFailure()) {
	(*athenaLog) << MSG::ERROR << "initTrackParticleCache: Failed to record TrackParticleCollection " <<  m_trackParticleCacheName << " No FTK tracks returned" << endreq;
	delete(m_conv_tp);
	return sc;
      }
    }
    
    if (!m_storeGate->contains<xAOD::TrackParticleAuxContainer>(m_trackParticleCacheName+"Aux")) {
      m_conv_tpAuxCont =  new xAOD::TrackParticleAuxContainer();
      sc = m_storeGate->record(m_conv_tpAuxCont, m_trackParticleCacheName+"Aux");
      if (sc.isFailure()) {
	(*athenaLog) << MSG::ERROR << "initTrackParticleCache: Failed to record TrackParticleAuxContainer " <<  m_trackParticleCacheName << "Aux - No FTK tracks returned" << endreq;
	delete(m_conv_tpAuxCont);
	return sc;
      }
    
      m_conv_tp->setStore( m_conv_tpAuxCont);
      for (unsigned int i = 0; i!=m_ftk_tracks->size(); i++) m_conv_tp_map.push_back(-1);
      (*athenaLog) << VERBOSEMSG << "initTrackParticleCache: cache initialized for converted TracksParticles" << endreq; 
    }

  }
  return sc;
}

Trk::Track* FTK_DataProviderSvc::ConvertTrack(const unsigned int iTrack){
  //std::unique_ptr< Trk::Track > FTK_DataProviderSvc::ConvertTrack(int iTrack){
  
  const FTK_RawTrack &track = *m_ftk_tracks->at(iTrack);

  (*athenaLog) << VERBOSEMSG << "ConvertTrack: Beginning Track conversion" << endreq;
  
  double trkTheta = std::atan2(1.0,track.getCotTh());
  double trkEta = -std::log(std::tan(trkTheta/2));
  
  (*athenaLog) << VERBOSEMSG << "ConvertTrack: track theta: " << trkTheta << " eta: " << trkEta << endreq;
  
  DataVector<const Trk::TrackStateOnSurface>* trkTSoSVec = new DataVector<const Trk::TrackStateOnSurface>;
  
  
  // Create Pixel Clusters
  bool hasIBL=false;
  
  (*athenaLog) << VERBOSEMSG << "   ConvertTrack: PixelClusterLoop: Pixel Clusters size = " << track.getPixelClusters().size() << endreq;
  for( unsigned int cluster_number = 0; cluster_number < track.getPixelClusters().size(); ++cluster_number){
    if(track.getPixelClusters().size() > 4){
      (*athenaLog) << MSG::WARNING << "   ConvertTrack: PixelClusterLoop: Too Many Pixel Clusters! " << track.getPixelClusters().size() << endreq;
      break;
    }
    (*athenaLog) << VERBOSEMSG << "   ConvertTrack: PixelClusterLoop: Creating Pixel Cluster Number " << cluster_number << endreq;
    FTK_RawPixelCluster raw_pixel_cluster = track.getPixelCluster(cluster_number);
    InDet::PixelCluster* pixel_cluster = createPixelCluster(raw_pixel_cluster,trkEta);
    if(pixel_cluster != nullptr) {
      (*athenaLog) << VERBOSEMSG << "   ConvertTrack: PixelClusterLoop: InDet::PixelCluster Created "<< endreq;
      Identifier wafer_id = m_pixelId->wafer_id(pixel_cluster->identify());
      if (m_pixelId->barrel_ec(wafer_id)==0 && m_pixelId->layer_disk(wafer_id)==0) hasIBL=true;
      const IdentifierHash idHash = m_pixelId->wafer_hash(m_pixelId->wafer_id(pixel_cluster->identify()));
      
      InDet::PixelClusterOnTrack* pixel_cluster_on_track = new InDet::PixelClusterOnTrack(pixel_cluster,
      											  pixel_cluster->localPosition(),
											  pixel_cluster->localCovariance(),
											  idHash,
											  pixel_cluster->globalPosition(),
											  pixel_cluster->gangedPixel());
      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
      typePattern.set(Trk::TrackStateOnSurface::Measurement);
      Trk::TrackStateOnSurface* trackStateOnSurface = new Trk::TrackStateOnSurface(pixel_cluster_on_track, 0, 0, 0, typePattern);
      trkTSoSVec->push_back(trackStateOnSurface);
    } else{
      (*athenaLog) << MSG::WARNING << "   ConvertTrack: PixelClusterLoop: Failed to create pixel hit "<< cluster_number << " skipped " << endreq;
    }
  }
  
  //
  // Create the SCT Clusters
  //
  
  (*athenaLog) << VERBOSEMSG << "   ConvertTrack: SCTClusterLoop: SCT Clusters size = " << track.getSCTClusters().size() << endreq;
  for( unsigned int cluster_number = 0; cluster_number < track.getSCTClusters().size(); ++cluster_number){
    if(track.getSCTClusters().size() > 8){
      (*athenaLog) << MSG::WARNING << "   ConvertTrack: SCTClusterLoop: Too Many SCT Clusters! " << track.getPixelClusters().size() << endreq;
      break;
    }
    (*athenaLog) << VERBOSEMSG << "   ConvertTrack: SCTClusterLoop: Creating SCT Cluster Number " << cluster_number << endreq;
    FTK_RawSCT_Cluster raw_cluster = track.getSCTCluster(cluster_number);
    InDet::SCT_Cluster* sct_cluster = createSCT_Cluster(raw_cluster);
    if(sct_cluster != nullptr) {
      (*athenaLog) << VERBOSEMSG << "   ConvertTrack: SCTClusterLoop: Created SCT_Cluster SUCCESS " << endreq;
      const IdentifierHash idHash = m_sctId->wafer_hash(m_sctId->wafer_id(sct_cluster->identify()));
      InDet::SCT_ClusterOnTrack* sct_cluster_on_track = new InDet::SCT_ClusterOnTrack(sct_cluster,
										      sct_cluster->localPosition(),
										      sct_cluster->localCovariance(),
										      idHash,
										      sct_cluster->globalPosition());
      
      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
      typePattern.set(Trk::TrackStateOnSurface::Measurement);
      
      Trk::TrackStateOnSurface* trackStateOnSurface = new Trk::TrackStateOnSurface(sct_cluster_on_track, 0, 0, 0, typePattern);
      trkTSoSVec->push_back(trackStateOnSurface);
    } else {
      (*athenaLog) << MSG::WARNING << "   ConvertTrack: SCTClusterLoop: Created SCT_Cluster FAILED " <<  endreq;
    }
  }
  
  //
  // Create track perigee parameters
  // ******************************************
  
  AmgSymMatrix(5) *trkTrackCovm = new AmgSymMatrix(5);
  trkTrackCovm->setZero();
  (*trkTrackCovm)(0,0)= m_uncertaintyTool->getParamCovMtx(track,  hasIBL,  FTKTrackParam::d0,     FTKTrackParam::d0);
  (*trkTrackCovm)(1,1)= m_uncertaintyTool->getParamCovMtx(track,  hasIBL,    FTKTrackParam::z0,     FTKTrackParam::z0);
  (*trkTrackCovm)(2,2)= m_uncertaintyTool->getParamCovMtx(track,  hasIBL,    FTKTrackParam::phi,    FTKTrackParam::phi);
  (*trkTrackCovm)(3,3)= m_uncertaintyTool->getParamCovMtx(track,  hasIBL,    FTKTrackParam::theta,  FTKTrackParam::theta);
  (*trkTrackCovm)(4,4)= m_uncertaintyTool->getParamCovMtx(track,  hasIBL,    FTKTrackParam::qOp,    FTKTrackParam::qOp);


  (*athenaLog) << VERBOSEMSG << " trkTrackCovm: (d0,z0,phi,theta,q/p)= " <<  (*trkTrackCovm)(0,0) << ", "<< (*trkTrackCovm)(1,1)<< ", " << 
    (*trkTrackCovm)(2,2)<< ", "<<(*trkTrackCovm)(3,3)<< ", "<<  (*trkTrackCovm)(4,4) << endreq;

 //
  // Construct the measured Perigee Parameters
  //
  
  double trkQOverP = sin(trkTheta)*track.getCurv()/2.; // note track.getCurv() currently returns q/pT not curv!
  Trk::PerigeeSurface periSurf;


  
  const Trk::Perigee* trkPerigee = new Trk::Perigee( track.getD0(),
						     track.getZ0(),
						     track.getPhi(),
						     trkTheta,
						     trkQOverP,
						     periSurf,
						     trkTrackCovm);
  
  (*athenaLog) << VERBOSEMSG << "   ConvertTrack: Track perigee created  d0 " <<  track.getD0()<< " z0 " <<
    track.getZ0() << " phi0 " << track.getPhi() << " theta " << trkTheta << " q/P " << trkQOverP << endreq;
  //
  // Build the TSOS
  //
  trkTSoSVec->push_back( new Trk::TrackStateOnSurface(NULL, trkPerigee));
  
  //
  // Build the TrackInfo
  //
  Trk::TrackInfo trkTrackInfo = Trk::TrackInfo(Trk::TrackInfo::Unknown, Trk::pion);
  
  //
  // Build the FitQuality
  //
  int ndof =  track.getPixelClusters().size()*2. + track.getSCTClusters().size()  - 5;
  double chi2OverNdof = 1000.;
  if (ndof > 0) {
    chi2OverNdof = track.getChi2()/ (double) ndof;
  } else {
    (*athenaLog) << DEBUGMSG << "   ConvertTrack: Warning ndof= " << ndof << " Track has only " 
		 << track.getPixelClusters().size() << track.getSCTClusters().size() << " sct hits " << endreq;
  }
  const Trk::FitQuality* trkFitQuality = new Trk::FitQuality(track.getChi2(), chi2OverNdof);
  
  //
  // And finally the Trk::Track
  //
  Trk::Track* new_track = new Trk::Track( trkTrackInfo, trkTSoSVec, trkFitQuality);
  
  //******************************
      
      
  return new_track;
}


InDet::SCT_Cluster* FTK_DataProviderSvc::createSCT_Cluster(const FTK_RawSCT_Cluster& raw_cluster) {

  IdentifierHash hash = raw_cluster.getModuleID();
  const InDetDD::SiDetectorElement* pDE = m_SCT_Manager->getDetectorElement(hash);
  float locPos = raw_cluster.getHitCoord()+0.1; // adding 0.1 to prevent rounding errors                                                                                      
  int w = raw_cluster.getHitWidth();
  int strip = (int)(locPos);

  if(w==0) w=1; // good coding there whoever you are

  Identifier wafer_id = m_sctId->wafer_id(hash);
  Identifier strip_id = m_sctId->strip_id(wafer_id, strip);

  std::vector<Identifier> rdoList;
  rdoList.push_back(strip_id);

  int firstStrip = (int)(locPos+0.5-0.5*w);
  int lastStrip  = (int)(locPos+0.5+0.5*w) -1 ;

  (*athenaLog) << VERBOSEMSG<< "FTK_DataProviderSvc::createSCT_Cluster: raw coord= " << raw_cluster.getHitCoord() << " locpos= " <<
    locPos << " strip, firstStrip, lastStrip= "  << strip << "," << firstStrip << "," << lastStrip << endreq;


  const int nStrips(lastStrip - firstStrip + 1);

  const InDetDD::SiCellId cell1(firstStrip - 1);
  const InDetDD::SiCellId cell2(lastStrip + 1);

  const InDetDD::SiLocalPosition firstStripPos(pDE->rawLocalPositionOfCell(cell1));
  const InDetDD::SiLocalPosition lastStripPos(pDE->rawLocalPositionOfCell(cell2));

  const double width((double(nStrips)/double(nStrips+1))*( lastStripPos.xPhi()-firstStripPos.xPhi()));
  const InDetDD::SiLocalPosition centre((firstStripPos+lastStripPos)/2.0);

  Amg::Vector2D localPos(centre.xPhi(),  centre.xEta());

  const InDetDD::SCT_ModuleSideDesign* design;
  if (m_sctId->is_barrel(wafer_id)){
    design = (static_cast<const InDetDD::SCT_BarrelModuleSideDesign*>(&pDE->design()));
  } else{
    design = (static_cast<const InDetDD::SCT_ForwardModuleSideDesign*>(&pDE->design()));
  }

  const std::pair<InDetDD::SiLocalPosition, InDetDD::SiLocalPosition> ends(design->endsOfStrip(centre));
  double stripLength(fabs(ends.first.xEta()-ends.second.xEta()));


  InDet::SiWidth siWidth(Amg::Vector2D(w,1), Amg::Vector2D(width,stripLength) );
  
  double shift = pDE->getLorentzCorrection();

  double derivedPos = localPos[Trk::locX]+shift;
  double rawPos = (raw_cluster.getHitCoord()-0.5*767)*pDE->phiPitch();

  if(fabs(derivedPos-rawPos)>0.5*pDE->phiPitch()) {
    derivedPos = rawPos+shift;
  }

  Amg::Vector2D position(derivedPos, localPos[Trk::locY]);

  Amg::MatrixX* cov = new Amg::MatrixX(2,2);
  cov->setZero();

  (*cov)(0,0) = siWidth.phiR()*siWidth.phiR()/12;
  (*cov)(1,1) = siWidth.z()*siWidth.z()/12;

  // rotation for endcap SCT      


  if(pDE->design().shape() == InDetDD::Trapezoid) {
    double sn      = pDE->sinStereoLocal(localPos);
    double sn2     = sn*sn;
    double cs2     = 1.-sn2;
    double w       = pDE->phiPitch(localPos)/pDE->phiPitch();
    double v0      = (*cov)(0,0)*w*w;
    double v1      = (*cov)(1,1);
    (*cov)(0,0) = (cs2*v0+sn2*v1);
    (*cov)(1,0) = (sn*sqrt(cs2)*(v0-v1));
    (*cov)(0,1) = (sn*sqrt(cs2)*(v0-v1));
    (*cov)(1,1) = (sn2*v0+cs2*v1);
  }

  InDet::SCT_Cluster* pCL = new InDet::SCT_Cluster(strip_id, position, rdoList, siWidth, pDE, cov);
  
  return pCL;
}





InDet::PixelCluster* FTK_DataProviderSvc::createPixelCluster(const FTK_RawPixelCluster& raw_pixel_cluster, float eta) {
  IdentifierHash hash = raw_pixel_cluster.getModuleID(); 
  Identifier wafer_id = m_pixelId->wafer_id(hash); // Need to set up this tool

  (*athenaLog) << "createPixelCluster: New FTK pixel cluster with hash " << hash << " wafer id " << wafer_id << endreq;

  const InDetDD::SiDetectorElement* pixelDetectorElement = m_pixelManager->getDetectorElement(hash);
  const InDetDD::PixelModuleDesign* design
    (dynamic_cast<const InDetDD::PixelModuleDesign*>(&pixelDetectorElement->design()));

  (*athenaLog) << VERBOSEMSG<< "FTK_DataProviderSvc::createPixelCluster: raw FTK cluster position: " <<
    " Row(phi): " <<  raw_pixel_cluster.getRowCoord() << " Col(eta): " << raw_pixel_cluster.getColCoord() << 
    " RowWidth: " << raw_pixel_cluster.getRowWidth() << " ColWidth: " << raw_pixel_cluster.getColWidth() << endreq;

  if (m_pixelId->barrel_ec(wafer_id)==0) {
    if (m_pixelId->layer_disk(wafer_id)==0) {
      (*athenaLog) << VERBOSEMSG << " IBL " << endreq;
    } else {
      (*athenaLog) << VERBOSEMSG << " Pixel Barrel layer  " << m_pixelId->layer_disk(wafer_id) << endreq;
    }
  } else {
    (*athenaLog) << VERBOSEMSG << " Pixel Endcap layer  " << m_pixelId->layer_disk(wafer_id) << endreq;
  }
  (*athenaLog) << VERBOSEMSG<< " Module rows= " << design->rows() << " phiPitch= " << design->phiPitch() << " width= " << design->width()  << endreq; 
  (*athenaLog) << VERBOSEMSG<< " columns = " << design->columns() << " etaPitch= " << design->etaPitch() <<  " length " << design->length() << endreq;

  int phiIndex = raw_pixel_cluster.getRowCoord();
  int etaIndex = raw_pixel_cluster.getColCoord();

  const InDetDD::SiCellId cell(phiIndex, etaIndex);



  if(m_pixelId->barrel_ec(wafer_id)!=0 && m_pixelId->layer_disk(wafer_id)==0 && phiIndex==0 && etaIndex==0) { // ***temp hack - remove bad hits
    (*athenaLog) << MSG::WARNING << "FTK_DataProviderSvc::createPixelCluster: Invalid Cell at phiIndex, etaIndex "<<  phiIndex << ", " << etaIndex << endreq;
    return nullptr;
  }

  if(!cell.isValid()) {
    (*athenaLog) << MSG::WARNING << "FTK_DataProviderSvc::createPixelCluster: Invalid Cell at phiIndex, etaIndex "<<  phiIndex << ", " << etaIndex << endreq;
    return nullptr;
  }

  const InDetDD::SiLocalPosition cellLocalPosition(pixelDetectorElement->rawLocalPositionOfCell(cell));

  double phiOffset = (raw_pixel_cluster.getRowCoord() - (float) phiIndex) *  design->phiPitch();
  double etaOffset = (raw_pixel_cluster.getColCoord() - (float) etaIndex) *  design->etaPitch();

  double phiPos = cellLocalPosition.xPhi() + phiOffset;
  double etaPos = cellLocalPosition.xEta() + etaOffset;
  (*athenaLog) << VERBOSEMSG<< "FTK_DataProviderSvc::createPixelCluster: NEWEST local coordinates phiPos, etaPos "<<  phiPos << ", " << etaPos <<  endreq;

  //InDetDD::SiLocalPosition silPos(phiPos, etaPos);// use the converted positions in mm !!!
  //InDetDD::SiCellId cell =  design->cellIdOfPosition(silPos);
  int phi_index = cell.phiIndex();
  int eta_index = cell.etaIndex();

  (*athenaLog) << VERBOSEMSG<< " created cell with phiIndex, etaIndex " << cell.phiIndex() << ", " << cell.etaIndex() <<  endreq;

  Identifier pixel_id = m_pixelId->pixel_id(wafer_id, phi_index, eta_index);

  int phiWidth    = raw_pixel_cluster.getRowWidth(); 
  int etaWidth    = raw_pixel_cluster.getColWidth(); 

  int colMin = (int)(eta_index-0.5*etaWidth);
  int colMax = colMin+etaWidth;

  int rowMin = (int)(phi_index-0.5*phiWidth);
  int rowMax = rowMin+phiWidth;

  double etaW = design->widthFromColumnRange(colMin, colMax-1);
  double phiW = design->widthFromRowRange(rowMin, rowMax-1);

  InDet::SiWidth siWidth(Amg::Vector2D(phiWidth,etaWidth),Amg::Vector2D(phiW,etaW));
  
  double shift = pixelDetectorElement->getLorentzCorrection();
  Amg::Vector2D position(phiPos+shift,etaPos);

  (*athenaLog) << VERBOSEMSG<< "FTK_DataProviderSvc::createPixelCluster: local coordinates phiPos, etaPos"<<  phiPos+shift << ", " << etaPos << " includes Lorentz shift " << shift <<  endreq;



  std::vector<Identifier> rdoList;
  rdoList.push_back(pixel_id);

  Amg::MatrixX* cov = new Amg::MatrixX(2,2);
  cov->setZero();

  (*cov)(0,0) = siWidth.phiR()*siWidth.phiR()/12;
  (*cov)(1,1) = siWidth.z()*siWidth.z()/12;

  if(m_offlineCalibSvc != 0) {
    const Amg::Vector2D& colRow = siWidth.colRow();
    double averageZPitch = siWidth.z()/colRow.y();
    if(averageZPitch > 399*micrometer && averageZPitch < 401*micrometer){
      if(pixelDetectorElement->isBarrel()){
	// Barrel corrections //
        (*cov)(0,0) =  pow(m_offlineCalibSvc->getBarrelErrorPhi(eta,int(colRow.y()),int(colRow.x())),2);
        (*cov)(1,1) = pow(m_offlineCalibSvc->getBarrelErrorEta(eta,int(colRow.y()),int(colRow.x())),2);
      }
      else{
	// End-cap corrections //
        (*cov)(0,0) = pow(m_offlineCalibSvc->getEndCapErrorPhi(int(colRow.y()),int(colRow.x())),2);
        (*cov)(1,1) =  pow(m_offlineCalibSvc->getEndCapErrorEta(int(colRow.y()),int(colRow.x())),2);
      }
    } else {
      (*cov)(0,0) = pow(siWidth.phiR()/colRow.x(),2)/12;
      (*cov)(1,1) = pow(averageZPitch,2)/12;
    }
  } 
  InDet::PixelCluster* pixel_cluster = new InDet::PixelCluster(pixel_id, position, rdoList, siWidth,
  						     pixelDetectorElement, cov);
  return pixel_cluster;
}

const InterfaceID& FTK_DataProviderSvc::type() const {
  return IFTK_DataProviderSvc::interfaceID();
}

StatusCode FTK_DataProviderSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if (IFTK_DataProviderSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface=(IFTK_DataProviderSvc*)this;
  } else {
    return AthService::queryInterface(riid,ppvInterface);
  }
  return StatusCode::SUCCESS;
}

void FTK_DataProviderSvc::handle(const Incident& incident) {
  if ( incident.type() == IncidentType::EndEvent) {
    (*athenaLog) << DEBUGMSG << "   Handle: get EndEvent incident" << endreq;
    
    if (m_gotRawTracks) {
      m_conv_track_map.clear();
      m_refit_track_map.clear();
      m_conv_tp_map.clear();
      m_refit_tp_map.clear();
    }
    m_newEvent = true;
    m_gotRawTracks = false;
    m_got_conv_vx=false;
    m_got_refit_vx=false;
    m_got_conv_vertex=false;
    m_got_refit_vertex=false;

  }
}
