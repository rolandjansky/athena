/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CLHEP/Units/SystemOfUnits.h"
#include "FTK_DataProviderSvc/FTKFastDataProviderSvc.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkFitterUtils/FitterTypes.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "StoreGate/DataHandle.h"


#include "AtlasDetDescr/AtlasDetectorID.h"

#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "EventInfo/EventIncident.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "FTK_RecToolInterfaces/IFTK_VertexFinderTool.h"
#include "TrkVertexFitterInterfaces/IVertexCollectionSortingTool.h"

#include "TrkToolInterfaces/ITrackParticleCreatorTool.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "InDetRecToolInterfaces/IVertexFinder.h"

#include "IRegionSelector/IRoiDescriptor.h"
#include "IRegionSelector/RoiUtil.h"


using Gaudi::Units::micrometer;
using namespace InDet;


FTKFastDataProviderSvc::FTKFastDataProviderSvc(const std::string& name, ISvcLocator* svc):
  base_class(name, svc),
  m_storeGate("StoreGateSvc", name),
  m_particleCreatorTool("Trk::ParticleCreatorTool"),
  m_VertexFinderTool("InDet::InDetIterativePriVxFinderTool"),
  m_VertexCollectionSortingTool ("Trk::VertexCollectionSortingTool"),
  m_RawVertexFinderTool("FTK_VertexFinderTool"),
  m_conv_tracks(nullptr),
  m_refit_tracks(nullptr),
  m_conv_tp(nullptr),
  m_refit_tp(nullptr),
  m_conv_tpAuxCont(nullptr),
  m_refit_tpAuxCont(nullptr),
  m_conv_vertex(nullptr),
  m_refit_vertex(nullptr),
  m_fast_vertex_conv(nullptr),
  m_fast_vertex_refit(nullptr),
  m_got_conv_vertex(false),
  m_got_refit_vertex(false),
  m_got_fast_vertex_refit(false),
  m_got_fast_vertex_conv(false),
  m_newEvent(true),
  m_trackCacheName("FTK_TrackCollection"),
  m_trackParticleCacheName("FTK_TrackParticleContainer"),
  m_vertexCacheName("FTK_VertexContainer"),
  m_doVertexing(true),
  m_doVertexSorting(true),
  m_done_conv_tp(false),
  m_done_refit_tp(false)

{
  declareProperty("TrackCollectionName",m_trackCacheName);
  declareProperty("TrackParticleContainerName",m_trackParticleCacheName);
  declareProperty("VertexContainerName",m_vertexCacheName);
  declareProperty("TrackParticleCreatorTool", m_particleCreatorTool);
  declareProperty("VertexFinderTool",m_VertexFinderTool);
  declareProperty("RawVertexFinderTool",m_RawVertexFinderTool);
  declareProperty("VertexCollectionSortingTool",m_VertexCollectionSortingTool );
  declareProperty("doVertexSorting",m_doVertexSorting );
  declareProperty("DoVertexing",m_doVertexing, "Enable Vertexing methods");
}

std::vector<unsigned int> FTKFastDataProviderSvc::nMissingSCTClusters() {
  return m_nMissingSCTClusters;
}

std::vector<unsigned int> FTKFastDataProviderSvc::nMissingPixelClusters() {
  return m_nMissingPixelClusters;
}

std::vector<unsigned int> FTKFastDataProviderSvc::nFailedSCTClusters() {
  return m_nFailedSCTClusters;
}

std::vector<unsigned int> FTKFastDataProviderSvc::nFailedPixelClusters() {
  return m_nFailedPixelClusters;
}


std::string FTKFastDataProviderSvc::getTrackParticleCacheName(const bool withRefit) {
  return (withRefit? m_trackParticleCacheName+"Refit": m_trackParticleCacheName);
}
std::string FTKFastDataProviderSvc::getTrackCacheName(const bool withRefit){
  return (withRefit? m_trackCacheName+"Refit": m_trackCacheName);
}
std::string FTKFastDataProviderSvc::getVertexCacheName(const bool withRefit){
  return (withRefit? m_vertexCacheName+"Refit": m_vertexCacheName);
}
std::string FTKFastDataProviderSvc::getFastVertexCacheName(const bool withRefit) {
  return (withRefit? m_vertexCacheName+"FastRefit": m_vertexCacheName+"Fast");
}


StatusCode FTKFastDataProviderSvc::initialize() {

  ATH_MSG_INFO( "FTKFastDataProviderSvc: Provides FTKFastSim tracks from smeared HLT tracks in collections " << m_trackCacheName<< ", " << m_trackCacheName<<"Refit");
  ATH_MSG_INFO( "TrackParticleContainer names: " <<  m_trackParticleCacheName << ", " << m_trackParticleCacheName <<"Refit");
  ATH_MSG_INFO( "VertexContainer names: " << m_vertexCacheName<< ", " <<m_vertexCacheName<<"Refit");

  /// Setup StoreGateSvc ///
  ATH_CHECK( m_storeGate.retrieve() );
  ATH_MSG_INFO( " getting particleCreatorTool tool with name " << m_particleCreatorTool.name());
  ATH_CHECK(m_particleCreatorTool.retrieve());
  if (m_doVertexing) {
    ATH_MSG_INFO( " getting FTK_RawTrackVertexFinderTool tool with name " << m_RawVertexFinderTool.name());
    ATH_CHECK(m_RawVertexFinderTool.retrieve());
    ATH_MSG_INFO( " getting vertexFinderTool tool with name " << m_VertexFinderTool.name());
    ATH_CHECK(m_VertexFinderTool.retrieve());
    if (m_doVertexSorting) { 
      ATH_MSG_INFO( " getting vertexCollectionSortingTool tool with name " << m_VertexCollectionSortingTool.name());
      ATH_CHECK(m_VertexCollectionSortingTool.retrieve());
    }
  } else {
    ATH_MSG_INFO( " Vertex Finding is Disabled");
  }
  // Register incident handler
  ServiceHandle<IIncidentSvc> iincSvc( "IncidentSvc", name());
  ATH_CHECK(iincSvc.retrieve());
  iincSvc->addListener( this, IncidentType::EndEvent);


  // Not used but required by interface
  for (int ie=0; ie<4; ie++){
    m_nFailedPixelClusters.push_back(0);
    m_nMissingPixelClusters.push_back(0);
  }
  for (int ie=0; ie<8; ie++){
    m_nFailedSCTClusters.push_back(0);
    m_nMissingSCTClusters.push_back(0);
  }

  return StatusCode::SUCCESS;

}


Trk::Track* FTKFastDataProviderSvc::getCachedTrack(const unsigned int, const bool) {
  return nullptr;
}


unsigned int FTKFastDataProviderSvc::nTrackParticleErrors(const bool withRefit) {

  unsigned int nErrors=0;
  
  if (withRefit) {
    if (m_done_refit_tp) nErrors = m_refit_tp->size() -  m_refit_tracks->size();
  } else {
    if (m_done_conv_tp) nErrors = m_conv_tp->size() -  m_conv_tracks->size();
  }
  return nErrors;

}


unsigned int FTKFastDataProviderSvc::nRawTracks() {
  ATH_MSG_DEBUG("FTKFastDataProviderSvc::nRawTracks: Using FTKFastSim based on smeared tracks - No RAW tracks available"); 
  return (0);
}


unsigned int FTKFastDataProviderSvc::nTracks(const bool withRefit) {

  return (withRefit? m_refit_tracks->size(): m_conv_tracks->size());
}

unsigned int FTKFastDataProviderSvc::nTrackParticles(const bool withRefit) {

  return (withRefit? m_refit_tp->size(): m_conv_tp->size());
}

unsigned int FTKFastDataProviderSvc::nTrackErrors(const bool ) {

  unsigned int nErrors=0;
  return nErrors;
}


xAOD::TrackParticleContainer* FTKFastDataProviderSvc::getTrackParticlesInRoi(const IRoiDescriptor& roi, const bool withRefit, unsigned int& nErrors){
  m_nErrors=0;
  xAOD::TrackParticleContainer* tracks = this->getTrackParticlesInRoi(roi, withRefit);
  nErrors = m_nErrors;
  return tracks;
}

xAOD::TrackParticleContainer* FTKFastDataProviderSvc::getTrackParticlesInRoi(const IRoiDescriptor& roi, const bool withRefit){
  
  ATH_MSG_DEBUG("FTKFastDataProviderSvc::getTrackParticlesInRoi called with Refit " << withRefit);
  xAOD::TrackParticleContainer *tpcont = new xAOD::TrackParticleContainer(SG::VIEW_ELEMENTS);
  
  
  StatusCode sc = this->fillTrackParticleCache(withRefit);
  if (sc.isFailure()) {
    return tpcont;
  }

  xAOD::TrackParticleContainer* tpCache = (withRefit? m_refit_tp : m_conv_tp );

  ATH_MSG_DEBUG(" getTrackParticlesInRoi got: "  << tpCache->size() << " TrackParticles from cache"); 
  if (roi.isFullscan()) {
    for (auto tp : *tpCache) tpcont->push_back(tp);
  } else {
    for (auto tp : *tpCache) if (insideRoi(roi,tp)) tpcont->push_back(tp);
  }    
  ATH_MSG_DEBUG("getTrackParticlesinRoI: Returning refitted TrackParticleContainer Size = " << tpcont->size());
  return tpcont;
}



StatusCode FTKFastDataProviderSvc::fillTrackParticleCache(const bool withRefit){

  if ((withRefit && m_done_refit_tp)|| (!withRefit && m_done_conv_tp)) return StatusCode::SUCCESS;
  ATH_MSG_DEBUG("FTKFastDataProviderSvc::fillTrackParticleCache called with Refit " << withRefit);

  if (initTrackParticleCache(withRefit).isFailure()) {
    return StatusCode::FAILURE;
  }
  xAOD::TrackParticleContainer* tpCache = (withRefit? m_refit_tp : m_conv_tp );
  TrackCollection* tracks = (withRefit? m_refit_tracks : m_conv_tracks );

  for (unsigned int itrack=0; itrack<tracks->size(); itrack++) {
    // Create ElementiLink for Trk::Track from pointer to TrackCollection and index of track within the collection
    const ElementLink<TrackCollection> trackLink(*tracks, itrack);

    if (trackLink.isValid()) {
      xAOD::TrackParticle* tp = m_particleCreatorTool->createParticle( trackLink, tpCache);
      if (tp ==nullptr) {
	ATH_MSG_DEBUG ("Failed to create TrackParticle from track with index " << itrack);
      }
    } else {
      ATH_MSG_WARNING ("invalid ElementLink to track with index "<<itrack-1);
    }
     
  } 
  if (withRefit) { 
    m_done_refit_tp=true;
    ATH_MSG_DEBUG("fillTrackParticleCache: Returning TrackParticleContainer of refitted tracks with Size = " << m_refit_tp->size());
  }else {
    m_done_conv_tp = true;
    ATH_MSG_DEBUG("fillTrackParticleCache: Returning TrackParticleContainer of converted tracks with Size = " << m_conv_tp->size());
  }

  return  StatusCode::SUCCESS;
}


xAOD::TrackParticleContainer* FTKFastDataProviderSvc::getTrackParticles(const bool withRefit, unsigned int& nErrors){

  // Return TrackParticleCollection plus number of missing tracks due to errors 

  m_nErrors=0;
  xAOD::TrackParticleContainer* tracks=this->getTrackParticles(withRefit);
  nErrors=m_nErrors;
  return tracks;
}

xAOD::TrackParticleContainer* FTKFastDataProviderSvc::getTrackParticles(const bool withRefit){

  xAOD::TrackParticleContainer *tpcont = new xAOD::TrackParticleContainer(SG::VIEW_ELEMENTS);

  if (fillTrackParticleCache(withRefit).isFailure()) {
    return tpcont;
  }

  if (withRefit) {
    for (auto tp = m_refit_tp->begin(); tp != m_refit_tp->end(); tp++) {
      tpcont->push_back(*tp);
    }
    ATH_MSG_DEBUG("getTrackParticles: Returning TrackParticleContainer Size = " << tpcont->size());
  } else {
    for (auto tp = m_conv_tp->begin(); tp != m_conv_tp->end(); tp++) {
      tpcont->push_back(*tp);
    }
    ATH_MSG_DEBUG("getTrackParticles: Returning converted TrackParticleContainer Size = " << tpcont->size());
  }

  return tpcont;
}




xAOD::VertexContainer* FTKFastDataProviderSvc::getFastVertices(const ftk::FTK_TrackType trackType/* default  ftk::RawTrack*/) {

   xAOD::VertexContainer* userVertex = new xAOD::VertexContainer(SG::VIEW_ELEMENTS);

   if (trackType == ftk::RawTrack) {
     ATH_MSG_DEBUG( "FTKFastDataProviderSvc::getFastVertices: cannot make vertices from RAW tracks ");
     return userVertex;
   }
  
   bool userefit=false;
   if (trackType == ftk::RefittedTrack) userefit=true;
   if (initTrackCache(userefit).isFailure()) return userVertex;

   if (!userefit) {
      ATH_MSG_DEBUG( "getFastFTKVertices from Converted FTK tracks ");
      if (!m_got_fast_vertex_conv) {
	std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> vertices = m_RawVertexFinderTool->findVertex(m_conv_tracks);
	std::string cacheName=m_vertexCacheName+"Fast";
	StatusCode sc = m_storeGate->record(vertices.first, cacheName);
	if (sc.isFailure()) {
	  ATH_MSG_DEBUG( "Failed to record xAOD::VertexContainer " << cacheName );
	  delete(vertices.first);
	  delete(vertices.second);
	  return userVertex;
	}
	sc = m_storeGate->record(vertices.second, cacheName+"Aux.");
	if (sc.isFailure()) {
	  ATH_MSG_DEBUG( "getFastVertices: Failed to record VertexAuxCollection " << cacheName );
	  delete(vertices.second);
	  return  userVertex;
	}

	m_fast_vertex_conv = vertices.first;
	m_got_fast_vertex_conv = true;
      }
      for (auto pv = m_fast_vertex_conv->begin(); pv != m_fast_vertex_conv->end(); ++pv) {
         userVertex->push_back(*pv);
      }
      ATH_MSG_DEBUG( "getFastFTKVertices: returning " << m_fast_vertex_conv->size() <<  " vertices from Converted tracks ");
      return userVertex;
   }
   else {
      ATH_MSG_DEBUG( "getFastFTKVertices from Refit FTK tracks ");
      if (!m_got_fast_vertex_refit) {
         std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> vertices = m_RawVertexFinderTool->findVertex(m_refit_tracks);
         std::string cacheName=m_vertexCacheName+"FastRefit";
         StatusCode sc = m_storeGate->record( vertices.first, cacheName);
         if (sc.isFailure()) {
            ATH_MSG_DEBUG( "Failed to record xAOD::VertexContainer " << cacheName );
            delete(vertices.first);
            delete(vertices.second);
            return userVertex;
         }
	 sc = m_storeGate->record(vertices.second, cacheName+"Aux.");
	 if (sc.isFailure()) {
	   ATH_MSG_DEBUG( "getFastVertices: Failed to record VertexAuxCollection " << cacheName );
	   delete(vertices.second);
	   return userVertex;
	 }

         m_fast_vertex_refit = vertices.first;
         m_got_fast_vertex_refit = true;
      }
      for (auto pv = m_fast_vertex_refit->begin(); pv != m_fast_vertex_refit->end(); ++pv) {
         userVertex->push_back(*pv);
      }
      ATH_MSG_DEBUG( "getFastFTKVertices: returning " << m_fast_vertex_refit->size() <<  " vertices from refit tracks ");
      return userVertex;
   }
}

bool FTKFastDataProviderSvc::fillVertexContainerCache(bool withRefit, xAOD::TrackParticleContainer* tps) {

  bool gotVertices = false;

  xAOD::VertexContainer* myVertexContainer = nullptr;
  xAOD::VertexAuxContainer* myVertexAuxContainer = nullptr;
  std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> myVxContainers = std::make_pair( myVertexContainer, myVertexAuxContainer );
  
  if (tps->size() > 1) {
    ATH_MSG_DEBUG( "fillVertexContainerCache: finding vertices from " << tps->size() << " TrackParticles ");
    
    std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> theXAODContainers = m_VertexFinderTool->findVertex(tps);
    
    if (theXAODContainers.first != nullptr) {
      
      if (theXAODContainers.first->size() >1 && m_doVertexSorting) {
	ATH_MSG_DEBUG( "doing vertex sorting");
	myVxContainers = m_VertexCollectionSortingTool->sortVertexContainer(*theXAODContainers.first);
	delete theXAODContainers.first; 
	delete theXAODContainers.second; 
      } else {
	ATH_MSG_DEBUG( "NOT doing vertex sorting");

	myVxContainers.first = theXAODContainers.first;
	myVxContainers.second = theXAODContainers.second;
      }
      if (myVxContainers.first != nullptr && myVxContainers.first->hasStore()) gotVertices=true;
    }
  }
  if (!gotVertices) {
    ATH_MSG_DEBUG( "failed to make vertices, creating empty collection");
    myVxContainers.first = new  xAOD::VertexContainer();
    myVxContainers.second = new  xAOD::VertexAuxContainer();
    myVxContainers.first->setStore( myVxContainers.second);
    gotVertices=true;
  }
  
  std::string cacheName= m_vertexCacheName;
  if (withRefit) cacheName+="Refit";
  
  StatusCode sc = m_storeGate->record(myVxContainers.first, cacheName);
  if (sc.isFailure()) {
    ATH_MSG_DEBUG( "fillVertexContainerCache: Failed to record VertexCollection " << cacheName );
    delete(myVxContainers.first);
    delete(myVxContainers.second);
    gotVertices=false;
  } else {
    sc = m_storeGate->record(myVxContainers.second, cacheName+"Aux.");
    if (sc.isFailure()) {
      ATH_MSG_DEBUG( "fillVertexContainerCache: Failed to record VertexAuxCollection " << cacheName );
      delete(myVxContainers.second);
      gotVertices=false;
    }
  }
  
  if (gotVertices) {
    if (withRefit) {
      m_refit_vertex = myVxContainers.first;
    } else {
      m_conv_vertex = myVxContainers.first;
    }
    
    ATH_MSG_DEBUG( "fillVertexContainerCache: got "<< myVxContainers.first->size() << " vertices");
  }
    
  return gotVertices;
}

xAOD::VertexContainer* FTKFastDataProviderSvc::getVertexContainer(const bool withRefit){

   xAOD::VertexContainer* userVertex = new xAOD::VertexContainer(SG::VIEW_ELEMENTS);
   
   bool doVertexing = m_doVertexing;
   if (doVertexing) {
     if (fillTrackParticleCache(withRefit).isSuccess()) {
       if (withRefit && m_refit_tp->size()<2) doVertexing=false;
       if ((!withRefit) && m_conv_tp->size()<2) doVertexing=false;
     }
   }
   if (!doVertexing) {

     // must always create a VertexContainer in StoreGate
     ATH_MSG_DEBUG( "getVertexContainer: Not eneough tracks for vertexing, creating empty VertexContainer");
     std::string cacheName= m_vertexCacheName;
     if (withRefit) cacheName+="Refit";
     if (!m_storeGate->contains<xAOD::VertexContainer>(cacheName)) {
       xAOD::VertexContainer* vertex = new xAOD::VertexContainer();
       xAOD::VertexAuxContainer* vertexAux =new xAOD::VertexAuxContainer();
       vertex->setStore(vertexAux);
      StatusCode sc = m_storeGate->record(vertex, cacheName);
      if (sc.isFailure()) {
	ATH_MSG_DEBUG( "getVertexContainer: Failed to record VertexCollection " << cacheName );
	delete(vertex);
	delete(vertexAux);
      } else {
	sc = m_storeGate->record(vertexAux, cacheName+"Aux.");
	if (sc.isFailure()) {
	  ATH_MSG_DEBUG( "getVertexContainer: Failed to record VertexAuxCollection " << cacheName );
	  delete(vertexAux);
	} else {
	  ATH_MSG_DEBUG( "recorded empty VertexContainer in storegate");
	}
      }
     }	  
     return userVertex;
   }

   if (withRefit) { // get vertex from refitted tracks
     if (!m_got_refit_vertex) {
       ATH_MSG_DEBUG( "getVertexContainer: filling VertexContainer from refitted tracks ");
      m_got_refit_vertex = fillVertexContainerCache(withRefit, m_refit_tp);
     }
     if (m_got_refit_vertex) {
       ATH_MSG_DEBUG( "getVertexContainer: cache contains " << m_refit_vertex->size() <<  " vertices from refitted tracks");
       for (auto pv = m_refit_vertex->begin(); pv != m_refit_vertex->end(); ++pv) {
	 userVertex->push_back(*pv);
       }
     }
   } else {   // get vertex from converted tracks
     if (!m_got_conv_vertex) {
       ATH_MSG_DEBUG( "getVertexContainer: filling VertexContainer from converted tracks ");
       m_got_conv_vertex = fillVertexContainerCache(withRefit, m_conv_tp);
     }
     if (m_got_conv_vertex) {
       ATH_MSG_DEBUG( "getVertexContainer: cache contains " << m_conv_vertex->size() <<  " vertices from converted tracks");
       for (auto pv = m_conv_vertex->begin(); pv != m_conv_vertex->end(); ++pv) {
	 userVertex->push_back(*pv);
       }
     }
   }
   return userVertex;
}
  

StatusCode FTKFastDataProviderSvc::getVertexContainer(xAOD::VertexContainer* userVertex, const bool withRefit){

  if ((!m_doVertexing) || fillTrackParticleCache(withRefit).isFailure()) {
    // must always create a VertexContainer in StroreGate
    
    std::string cacheName= m_vertexCacheName;
    if (withRefit) cacheName+="Refit";
    if (!m_storeGate->contains<xAOD::VertexContainer>(cacheName)) {
      xAOD::VertexContainer* vertex = new xAOD::VertexContainer();
      xAOD::VertexAuxContainer* vertexAux =new xAOD::VertexAuxContainer();
      vertex->setStore(vertexAux);
      StatusCode sc = m_storeGate->record(vertex, cacheName);
      if (sc.isFailure()) {
	ATH_MSG_DEBUG( "fillVertexContainerCache: Failed to record VertexCollection " << cacheName );
	delete(vertex);
	delete(vertexAux);
      } else {
	sc = m_storeGate->record(vertexAux, cacheName+"Aux.");
	if (sc.isFailure()) {
	  ATH_MSG_DEBUG( "fillVertexContainerCache: Failed to record VertexAuxCollection " << cacheName );
	  delete(vertexAux);
	}
      }
    }
    
    return StatusCode::SUCCESS;
  }

  if (withRefit) { // get vertex from refitted tracks
    if (!m_got_refit_vertex) {
      ATH_MSG_DEBUG( "getVertexContainer: filling VertexContainer from refitted tracks ");
      m_got_refit_vertex = fillVertexContainerCache(withRefit, m_refit_tp);
    }
    if (m_got_refit_vertex) {
      ATH_MSG_DEBUG( "getVertexContainer: cache contains " << m_refit_vertex->size() <<  " vertices from refitted tracks");
      for (auto pv = m_refit_vertex->begin(); pv != m_refit_vertex->end(); ++pv) {
        userVertex->push_back(new xAOD::Vertex(*(*pv)));
      }
    }
  } else {   // get vertex from converted tracks
    if (!m_got_conv_vertex) {
      ATH_MSG_DEBUG( "getVertexContainer: filling VertexContainer from converted tracks ");
      m_got_conv_vertex = fillVertexContainerCache(withRefit, m_conv_tp);
    }
    if (m_got_conv_vertex) {
      ATH_MSG_DEBUG( "getVertexContainer: cache contains " << m_conv_vertex->size() <<  " vertices from converted tracks");
      for (auto pv = m_conv_vertex->begin(); pv != m_conv_vertex->end(); ++pv) {
	xAOD::Vertex* vert = new xAOD::Vertex(*(*pv));
        userVertex->push_back(vert);
      }
    }
  }
  return StatusCode::SUCCESS;

}



TrackCollection* FTKFastDataProviderSvc::getTracks(const bool withRefit, unsigned int& nErrors){
  m_nErrors=0;
  TrackCollection* tracks = this->getTracks(withRefit);
  nErrors=m_nErrors;
  return tracks;
}

const FTK_RawTrackContainer* FTKFastDataProviderSvc::getRawTracks() {
  ATH_MSG_DEBUG("FTKFastDataProviderSvc::getRawTracks: Using FTKFastSim based on smeared tracks - No RAW tracks available"); 
  return (nullptr);
}

TrackCollection* FTKFastDataProviderSvc::getTracks(const bool withRefit){

  TrackCollection *userTracks=new TrackCollection(SG::VIEW_ELEMENTS);

  if (initTrackCache(withRefit).isFailure()) {
    return userTracks;
  }

  TrackCollection* tracks = (withRefit?  m_refit_tracks : m_conv_tracks);

  for (Trk::Track *track: *tracks){
    userTracks->push_back(track);
  }
  if (withRefit) {
    ATH_MSG_DEBUG( "getTracks: Return refitted TrackContainer Size = " << userTracks->size());
  } else {
    ATH_MSG_DEBUG( "getTracks: Return TrackContainer Size = " << userTracks->size());
  }

  return userTracks;

}

TrackCollection* FTKFastDataProviderSvc::getTracksInRoi(const IRoiDescriptor& roi, const bool withRefit, unsigned int& nErrors){
  m_nErrors=0;
  TrackCollection* tracks = this->getTracksInRoi(roi,withRefit);
  nErrors=m_nErrors;
  return tracks;
}

TrackCollection* FTKFastDataProviderSvc::getTracksInRoi(const IRoiDescriptor& roi, const bool withRefit){

  TrackCollection *userTracks=new TrackCollection(SG::VIEW_ELEMENTS);

  // If we're at the start of a new event, get the tracks from StoreGate
  if (initTrackCache(withRefit).isFailure()) {
    return userTracks;
  }

  TrackCollection* tracks = (withRefit?  m_refit_tracks : m_conv_tracks);

  if (roi.isFullscan()) {
    for (Trk::Track *track: *tracks){
      userTracks->push_back(track);
    }
  } else {
    for (Trk::Track* track: *tracks){
      /// Check if the track is inside the ROI ///
      
      const Trk::TrackParameters* trackPars = track->perigeeParameters();
      if(trackPars==nullptr) {
	ATH_MSG_DEBUG( "FTKFastDataProviderSvc::getTracksInRoi track has no perigeeParameters");
	continue;
      }
      
      if (insideRoi(roi,track)) userTracks->push_back(track);
      
    }
  }
  if (withRefit) {
    ATH_MSG_DEBUG( "getTracksinRoi: Return refitted TrackContainer Size = " << userTracks->size());
  } else {
    ATH_MSG_DEBUG( "getTracksinRoi: Return TrackContainer Size = " << userTracks->size());
  }

  return userTracks;

}

bool FTKFastDataProviderSvc::insideRoi(const IRoiDescriptor& roi, const Trk::Track *track) {

  const Trk::TrackParameters* trackPars = track->perigeeParameters();
  if(trackPars==nullptr) {
    ATH_MSG_DEBUG( "FTKFastDataProviderSvc::insideRoi track has no perigeeParameters");
    return false;
  }
  
  return (RoiUtil::containsPhi(roi, trackPars->parameters()[Trk::phi0]) && RoiUtil::contains(roi,trackPars->parameters()[Trk::z0], 1./tan(trackPars->parameters()[Trk::theta])));
}

bool FTKFastDataProviderSvc::insideRoi(const IRoiDescriptor& roi, const xAOD::TrackParticle *track) {

  return (RoiUtil::containsPhi(roi, track->phi0()) && RoiUtil::contains(roi,track->z0(), 1./tan(track->theta())));
}


StatusCode FTKFastDataProviderSvc::initTrackCache(bool withRefit) {
  
  if (!withRefit){
    if (!m_storeGate->contains<TrackCollection>(m_trackCacheName)) {
      ATH_MSG_DEBUG( "initTrackCache: Failed to find smeared FTK tracks " <<  m_trackCacheName);
    return StatusCode::FAILURE;
    }
    ATH_CHECK(m_storeGate->retrieve(m_conv_tracks, m_trackCacheName));
    ATH_MSG_DEBUG( "initTrackCache: Retrieved " << m_conv_tracks->size() << "  smeared FTK tracks ");
  } else {
    if (!m_storeGate->contains<TrackCollection>(m_trackCacheName+"Refit")) {
      ATH_MSG_DEBUG( "initTrackCache: Failed to find smeared FTKRefit tracks " <<  m_trackCacheName+"Refit");
      return StatusCode::FAILURE;
    } else {
      StatusCode sc = m_storeGate->retrieve(m_refit_tracks, m_trackCacheName+"Refit");
      if (sc.isFailure()) {
	ATH_MSG_DEBUG( "initTrackCache: Failed to retrieve smeared FTKRefit tracks " <<  m_trackCacheName+"Refit");
	return StatusCode::FAILURE;
      }
    }
  }

  return  StatusCode::SUCCESS;
}

StatusCode FTKFastDataProviderSvc::initTrackParticleCache(bool withRefit) {

  ATH_MSG_VERBOSE("initTrackParticleCache");

  // Must always create a container, even if it is empty

  bool gotTracks=false;
  if (!initTrackCache(withRefit).isFailure()) gotTracks=true; 

  if (withRefit) {

    if (!m_storeGate->contains<xAOD::TrackParticleContainer>(m_trackParticleCacheName+"Refit")) {

      ATH_MSG_VERBOSE( "initTrackParticleCache: creating container for refitted TrackParticles");
      m_refit_tp=  new xAOD::TrackParticleContainer();

      m_refit_tpAuxCont =  new xAOD::TrackParticleAuxContainer();

      m_refit_tp->setStore( m_refit_tpAuxCont);

      StatusCode sc = m_storeGate->record(m_refit_tp, m_trackParticleCacheName+"Refit");
      if (sc.isFailure()) {
        ATH_MSG_DEBUG( "initTrackParticleCache: Failed to record TrackParticleContainer " <<  m_trackParticleCacheName << "Refit No FTK TrackspParticles returned");
        delete(m_refit_tp);
        delete(m_refit_tpAuxCont);
        return sc;
      }
      sc = m_storeGate->record(m_refit_tpAuxCont, m_trackParticleCacheName+"RefitAux.");
      if (sc.isFailure()) {
        ATH_MSG_DEBUG( "initTrackParticleCache: Failed to record TrackParticleAuxCollection " <<  m_trackParticleCacheName << "RefitAux. No FTK tracks returned");
        delete(m_refit_tpAuxCont);
        return sc;
      }
    } 
  } else {

    if (!m_storeGate->contains<xAOD::TrackParticleContainer>(m_trackParticleCacheName)) {

      ATH_MSG_VERBOSE( "initTrackParticleCache: creating container for converted TrackParticles");

      m_conv_tp=  new xAOD::TrackParticleContainer();
      m_conv_tpAuxCont =  new xAOD::TrackParticleAuxContainer();
      m_conv_tp->setStore( m_conv_tpAuxCont);

      StatusCode sc = m_storeGate->record(m_conv_tp, m_trackParticleCacheName);


      if (sc.isFailure()) {
        ATH_MSG_DEBUG( "initTrackParticleCache: Failed to record TrackParticleCollection " <<  m_trackParticleCacheName << " No FTK tracks returned");
        delete(m_conv_tp);
        delete(m_conv_tpAuxCont);
        return sc;
      }

      sc = m_storeGate->record(m_conv_tpAuxCont, m_trackParticleCacheName+"Aux.");
      if (sc.isFailure()) {
        ATH_MSG_DEBUG( "initTrackParticleCache: Failed to record TrackParticleAuxContainer " <<  m_trackParticleCacheName << "Aux - No FTK tracks returned");
        delete(m_conv_tpAuxCont);
        return sc;
      }
      
    } 
  }
  return (gotTracks?StatusCode::SUCCESS:StatusCode::FAILURE);
}


void FTKFastDataProviderSvc::handle(const Incident& incident) {
  if ( incident.type() == IncidentType::EndEvent) {
    ATH_MSG_DEBUG("   Handle: get EndEvent incident");

    m_done_conv_tp=false;
    m_done_refit_tp=false;
    m_conv_tracks=nullptr;
    m_refit_tracks=nullptr;
    m_got_conv_vertex=false;
    m_got_refit_vertex=false;
    m_got_fast_vertex_refit = false;
    m_got_fast_vertex_conv = false;

  }
}


