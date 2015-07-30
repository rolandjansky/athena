/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CLHEP/Units/SystemOfUnits.h"
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
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "InDetPrepRawData/SiClusterContainer.h"

#include "GeneratorObjects/McEventCollection.h"
#include "HepMC/GenParticle.h"

#include "TrkTruthData/PRD_MultiTruthCollection.h"
//#include "TrkEventUtils/PrepRawDataComparisonFunction.h"


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

using Gaudi::Units::micrometer;
using namespace InDet;

namespace {
  inline double square(const double x){
    return x*x;
  }
  const double ONE_TWELFTH = 1./12.;


}



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
  m_trackFitter("Trk::ITrackFitter/InDetTrigTrackFitter"),
  m_trackSumTool("Trk::TrackSummaryTool"),
  m_particleCreatorTool("Trk::ParticleCreatorTool"),
  m_VertexFinderTool("InDet::InDetIterativePriVxFinderTool"),
  m_trainingBeamspotX(0.),
  m_trainingBeamspotY(0.),
  m_trainingBeamspotZ(0.),
  m_trainingBeamspotTiltX(0.),
  m_trainingBeamspotTiltY(0.),
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
  m_trackCacheName("FTK_TrackCollection"),
  m_trackParticleCacheName("FTK_TrackParticleContainer"),
  m_VxContainerCacheName("FTK_VxContainer"),
  m_VertexContainerCacheName("FTK_VertexContainer"),
  m_doTruth(false),
  m_ftkPixelTruthName("FTK_PRD_MultiTruthPixel"),
  m_ftkSctTruthName("FTK_PRD_MultiTruthSCT"),
  m_mcTruthName("TruthEvent"), m_mcEventCollection(NULL),
  m_collectionsReady(false),
  m_PixelClusterContainerName("FTK_PixelClusterContainer"),
  m_SCT_ClusterContainerName("FTK_SCT_ClusterContainer"),
  m_pTscaleFactor(1.),
  m_fixBadTracks(false),
  m_rejectBadTracks(false),
  m_dPhiCut(0.4),
  m_dEtaCut(0.4)
{
  declareProperty("TrackCollectionName",m_trackCacheName);
  declareProperty("TrackParticleContainerName",m_trackParticleCacheName);
  declareProperty("VxContainerName",m_VxContainerCacheName);
  declareProperty("VertexContainerName",m_VertexContainerCacheName);	  
  declareProperty("TrackFitter", m_trackFitter);
  declareProperty("UncertaintyTool",m_uncertaintyTool);
  declareProperty("TrackSummaryTool", m_trackSumTool);
  declareProperty("TrackParticleCreatorTool", m_particleCreatorTool);
  declareProperty("VertexFinderTool",m_VertexFinderTool);
  declareProperty("doTruth",m_doTruth);
  declareProperty("PixelTruthName",m_ftkPixelTruthName);
  declareProperty("SctTruthName",m_ftkSctTruthName);
  declareProperty("mcTruthName", m_mcTruthName);
  declareProperty("PixelClusterContainerName", m_PixelClusterContainerName);
  declareProperty("SCT_ClusterContainerName", m_SCT_ClusterContainerName);
  declareProperty("TrainingBeamspotX", m_trainingBeamspotX);
  declareProperty("TrainingBeamspotY", m_trainingBeamspotY);
  declareProperty("TrainingBeamspotZ", m_trainingBeamspotZ);
  declareProperty("TrainingBeamspotTiltX", m_trainingBeamspotTiltX);
  declareProperty("TrainingBeamspotTiltY", m_trainingBeamspotTiltY);
  declareProperty("pTscaleFactor", m_pTscaleFactor);
  declareProperty("fixBadTracks", m_fixBadTracks);
  declareProperty("rejectBadTracks", m_rejectBadTracks);
  declareProperty("BadTrackdPhiCut", m_dPhiCut);
  declareProperty("BadTrackdEtaCut", m_dEtaCut);
}

FTK_DataProviderSvc::~FTK_DataProviderSvc(){
}

StatusCode FTK_DataProviderSvc::initialize() {

  /// Setup StoreGateSvc ///

  ATH_CHECK(service( "StoreGateSvc", m_storeGate ));
  StoreGateSvc* detStore;
  ATH_CHECK(service("DetectorStore", detStore));
  ATH_CHECK(m_offlineCalibSvc.retrieve());
  ATH_CHECK(detStore->retrieve(m_pixelId, "PixelID"));  
  ATH_CHECK(detStore->retrieve(m_sctId, "SCT_ID"));  
  ATH_CHECK(detStore->retrieve(m_pixelManager));
  ATH_CHECK(detStore->retrieve(m_SCT_Manager));
  ATH_CHECK(detStore->retrieve(m_id_helper, "AtlasID"));
  ATH_MSG_INFO( " getting UncertaintyTool with name " << m_uncertaintyTool.name());
  ATH_CHECK(m_uncertaintyTool.retrieve());
  ATH_MSG_INFO( " getting TrackFitter tool with name " << m_trackFitter.name());
  ATH_CHECK(m_trackFitter.retrieve());
  ATH_MSG_INFO( " getting TrackSummary tool with name " << m_trackSumTool.name());
  ATH_CHECK(m_trackSumTool.retrieve());
  ATH_MSG_INFO( " getting particleCreatorTool tool with name " << m_particleCreatorTool.name());
  ATH_CHECK(m_particleCreatorTool.retrieve());
  ATH_MSG_INFO( " getting vertexFinderTool tool with name " << m_VertexFinderTool.name());
  ATH_CHECK(m_VertexFinderTool.retrieve());

  // Register incident handler
  ServiceHandle<IIncidentSvc> iincSvc( "IncidentSvc", name());
  ATH_CHECK(iincSvc.retrieve());
  iincSvc->addListener( this, IncidentType::EndEvent);

  ATH_MSG_INFO( "TrackCollection names: " << m_trackCacheName<< ", " << m_trackCacheName<<"Refit");
  ATH_MSG_INFO( "TrackParticleCollection names: " <<  m_trackParticleCacheName << ", " << m_trackParticleCacheName <<"Refit");
  ATH_MSG_INFO( "VxContainer names: " <<  m_VxContainerCacheName<< ", " << m_VxContainerCacheName<< "Refit");
  ATH_MSG_INFO( "VertexContainer names: " << m_VertexContainerCacheName<< ", " <<m_VertexContainerCacheName<<"Refit");
  ATH_MSG_INFO( "PixelClusterContainer name : " << m_PixelClusterContainerName);
  ATH_MSG_INFO( "SCT_ClusterContainer name : " << m_SCT_ClusterContainerName);
  ATH_MSG_INFO( "PRD Truth SCT name: " << m_ftkSctTruthName);
  ATH_MSG_INFO( "PRD Truth Pixel name : " << m_ftkPixelTruthName);


  ATH_MSG_INFO( "Correcting for FTK training beamspot at x " <<  m_trainingBeamspotX <<" y " << 	m_trainingBeamspotY 
		<< " z " <<  m_trainingBeamspotZ << " TiltX " << m_trainingBeamspotTiltX << "TiltY " << m_trainingBeamspotTiltY );
  

  return StatusCode::SUCCESS; 

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
      if (roi.isFullscan() || (roi.containsPhi(ftk_track->getPhi()) && roi.contains(ftk_track->getZ0(), ftk_track->getCotTh()))) {

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
          } else {
	    ATH_MSG_DEBUG("Setting m_refit_tp_map["<<  ftk_track_index <<"]=-2");
	    m_refit_tp_map[ftk_track_index]=-2;
	  }
        }
      }
    }
    ATH_MSG_DEBUG("getTrackParticlesIn Roi: Returning TrackParticleContainer Size = " << tpcont->size());
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
          } else {
	    ATH_MSG_DEBUG("Setting m_conv_tp_map["<<  ftk_track_index <<"]=-2");
	    m_conv_tp_map[ftk_track_index] = -2;
	  }
        }
      }
    }
    ATH_MSG_DEBUG("getTrackParticlesinRoI: Returning refitted TrackParticleContainer Size = " << tpcont->size());
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
      if( m_refit_tp_map[ftk_track_index]==-1) { 
        ATH_MSG_DEBUG("fillTrackParticleCache: get cached track " << ftk_track_index);
	Trk::Track *track = getCachedTrack(ftk_track_index, withRefit); 
        if (track != nullptr) {
          ATH_MSG_DEBUG("creating ElementLink");
          const ElementLink<TrackCollection> trackLink(&(*track), *m_refit_tracks);

          xAOD::TrackParticle* tp = m_particleCreatorTool->createParticle( trackLink, m_refit_tp);
          if (tp !=nullptr) {
            m_refit_tp_map[ftk_track_index] = (int) m_refit_tp->size()-1;
            ATH_MSG_DEBUG("TrackParticle from refitted track added to cache at index " << m_refit_tp_map[ftk_track_index] 
                << "  created from FTK track at index " << ftk_track_index);
          }
        } else {
	  ATH_MSG_DEBUG("Setting m_refit_tp_map["<<  ftk_track_index <<"]=-2");
	  m_refit_tp_map[ftk_track_index] = -2;
	}
      }
    }
    ATH_MSG_DEBUG("fillTrackParticleCache: Returning TrackParticleContainer Size = " << m_refit_tp->size());

  } else {// create TrackParticle from converted track

    for (unsigned int ftk_track_index = 0; ftk_track_index != m_ftk_tracks->size(); ++ftk_track_index){

      if( m_conv_tp_map[ftk_track_index]==-1) { 
        Trk::Track *track = getCachedTrack(ftk_track_index, withRefit); 
        if (track !=nullptr) {
          ATH_MSG_DEBUG("creating ElementLink");
          const ElementLink<TrackCollection> trackLink(&(*track), *m_conv_tracks);

          ATH_MSG_DEBUG("creating TrackParticle");
          xAOD::TrackParticle* tp = m_particleCreatorTool->createParticle( trackLink, m_conv_tp);
          if (tp !=nullptr) {
            m_conv_tp_map[ftk_track_index] = (int) m_conv_tp->size()-1;
            ATH_MSG_DEBUG("TrackParticle from converted track added to cache at index " << m_conv_tp_map[ftk_track_index] 
                << "  created from FTK track at index " << ftk_track_index);
          }
        } else {
	  ATH_MSG_DEBUG("Setting m_conv_tp_map["<<  ftk_track_index <<"]=-2");
	  m_conv_tp_map[ftk_track_index] = -2;
	} 
      }
    }

    ATH_MSG_DEBUG("fillTrackParticleCache: Returning TrackParticleContainer Size = " << m_conv_tp->size());
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
    ATH_MSG_DEBUG("getTrackParticles: Returning TrackParticleContainer Size = " << tpcont->size());
  } else {
    for (auto tp = m_conv_tp->begin(); tp != m_conv_tp->end(); tp++) {
      tpcont->push_back(*tp);
    } 
    ATH_MSG_DEBUG("getTrackParticles: Returning converted TrackParticleContainer Size = " << tpcont->size());
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
      ATH_MSG_VERBOSE( "fillTrackCache: FTK Track " << ftk_track_index << " not yet converted, beginning conversion");
      Trk::Track *new_track = ConvertTrack(ftk_track_index);
      if (new_track!=nullptr) {
	m_conv_track_map[ftk_track_index] = (int) m_conv_tracks->size();
	ATH_MSG_VERBOSE( "fillTrackCache:  adding coverted track to cache at index " <<  m_conv_tracks->size() << " corresponding to FTK track " << ftk_track_index);
	m_conv_tracks->push_back(new_track);
	track = m_conv_tracks->at((unsigned int) m_conv_track_map[ftk_track_index]);
	ATH_MSG_VERBOSE( "fillTrackCache:  add track summary information to converted track ");
	m_trackSumTool->updateTrack(*track);
      } else {
	ATH_MSG_VERBOSE("Setting m_conv_track_map["<<  ftk_track_index <<"]=-2");
	m_conv_track_map[ftk_track_index]=-2;
      }
    } 
    if (m_conv_track_map[ftk_track_index]>-1 && withRefit && m_refit_track_map[ftk_track_index]==-1) { // need to do refit
      ATH_MSG_VERBOSE( "fillTrackCache:Beginning Track Refit for FTK track " << ftk_track_index);
      Trk::Track *newRefitTrack = m_trackFitter->fit(*track, false, Trk::pion);
      if(newRefitTrack == nullptr) {
        ATH_MSG_DEBUG("fillTrackCache: refit failed - refitted track coresponding to FTK track with index " << ftk_track_index << " has NOT added to  output collection");
        m_refit_track_map[ftk_track_index] = -2; // flag so we don't try to refit again
	ATH_MSG_VERBOSE("Setting m_refit_track_map["<<  ftk_track_index <<"]=-2");
      } else { 
        ATH_MSG_VERBOSE( "fillTrackCache: Track Refit SUCEEDED "); 
        ATH_MSG_VERBOSE( "fillTrackCache:  adding refitted track to cache at index " <<  m_refit_tracks->size() << " corresponding to FTK track " << ftk_track_index);
        m_refit_track_map[ftk_track_index] = (int) m_refit_tracks->size();
        m_refit_tracks->push_back(newRefitTrack);
        ATH_MSG_VERBOSE( "fillTrackCache:  adding track summary information to refitted track ");
        Trk::Track *refitTrack = m_refit_tracks->at((unsigned int) m_refit_track_map[ftk_track_index]);
        m_trackSumTool->updateTrack(*refitTrack);
      }
    }
  }

  if (withRefit) {
    ATH_MSG_DEBUG("fillTrackCache: " << m_conv_tracks->size() << " converted tracks and " << m_refit_tracks->size() << " refitted tracks");
  } else { 
    ATH_MSG_DEBUG("fillTrackCache: " << m_conv_tracks->size() << " converted tracks");
  }


  return sc;
}

bool FTK_DataProviderSvc::fillVxContainer(bool withRefit, TrackCollection* tracks) {

  bool gotVertices=false;

  if (tracks->size() > 0) {
    ATH_MSG_DEBUG("fillVxContainer: finding vertices from " << tracks->size() << " tracks ");
    VxContainer* vertices = m_VertexFinderTool->findVertex(tracks);
    if (vertices == nullptr) return gotVertices;
    std::string cacheName= m_VxContainerCacheName;
    if (withRefit) cacheName+="Refit";
    StatusCode sc = m_storeGate->record(vertices, cacheName);
    if (sc.isFailure()) {
      ATH_MSG_DEBUG( "fillVxContainer: Failed to record VxCollection " << cacheName );
      delete(vertices);
      return gotVertices;
    }
    gotVertices = true;
    if (withRefit) {
      m_refit_vx= vertices;
    } else {
      m_conv_vx= vertices;
    }
    ATH_MSG_DEBUG( "fillVxContainer: returning " << vertices->size() <<  " vertices ");
  }
  return gotVertices;
}

VxContainer* FTK_DataProviderSvc::getVxContainer(const bool withRefit) {

  VxContainer *userVertex = new VxContainer(SG::VIEW_ELEMENTS);

  if (fillTrackCache(withRefit).isFailure()) return userVertex;

  if (withRefit) {
    if (!m_got_refit_vx) {
      ATH_MSG_DEBUG( "getVxContainer: filling VxContainer from refitted tracks ");
      m_got_refit_vx = fillVxContainer(withRefit, m_refit_tracks);
      ATH_MSG_DEBUG( "getVxContainer: found " << m_refit_vx->size() << " vertices from refitted tracks");
    }
    if (m_got_refit_vx) {
      ATH_MSG_DEBUG( "getVxContainer: cache contains " << m_refit_vx->size() <<  " vertices from refitted tracks");
      for (auto pv = m_refit_vx->begin(); pv != m_refit_vx->end(); ++pv) {
        userVertex->push_back(*pv);
      }
    }
  } else {
    if (!m_got_conv_vx) {
      ATH_MSG_DEBUG( "getVxContainer: filling VxContainer from converted tracks ");
      m_got_conv_vx = fillVxContainer(withRefit, m_conv_tracks);
      ATH_MSG_DEBUG( "getVxContainer: found " << m_conv_vx->size() << " vertices from converted tracks");
    }
    if (m_got_conv_vx) {
      ATH_MSG_DEBUG( "getVxContainer: cache contains " << m_conv_vx->size() <<  " vertices from converted tracks");
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
    ATH_MSG_DEBUG( "fillVertexContainerCache: finding vertices from " << tps->size() << " TrackParticles ");
    std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> vertices = m_VertexFinderTool->findVertex(tps);
    if (vertices.first == nullptr) return gotVertices;

    std::string cacheName= m_VertexContainerCacheName;
    if (withRefit) cacheName+="Refit";

    StatusCode sc = m_storeGate->record(vertices.first, cacheName);
    if (sc.isFailure()) {
      ATH_MSG_DEBUG( "fillVertexContainerCache: Failed to record VertexCollection " << cacheName );
      delete(vertices.first);
      delete(vertices.second);
      return gotVertices;
    }
    sc = m_storeGate->record(vertices.second, cacheName+"Aux");
    if (sc.isFailure()) {
      ATH_MSG_DEBUG( "fillVertexContainerCache: Failed to record VertexAuxCollection " << cacheName );
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
  ATH_MSG_DEBUG( "getTracks: Raw FTK Container Size = " << m_ftk_tracks->size());
  ATH_MSG_VERBOSE( "getTracks: Converting Tracks");
  for (unsigned int ftk_track_index = 0; ftk_track_index != m_ftk_tracks->size(); ++ftk_track_index){

    Trk::Track* track = this->getCachedTrack(ftk_track_index, withRefit);
    if (track != nullptr) userTracks->push_back(track);
  }
  if (withRefit) {
    ATH_MSG_DEBUG( "getTracks: Return refitted TrackContainer Size = " << userTracks->size());
  } else { 
    ATH_MSG_DEBUG( "getTracks: Return TrackContainer Size = " << userTracks->size());
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
  ATH_MSG_DEBUG( "getTracksInRoi: Raw FTK Container Size = " << m_ftk_tracks->size());
  ATH_MSG_VERBOSE( "getTracksInRoI: Converting Tracks");
  for (unsigned int ftk_track_index = 0; ftk_track_index != m_ftk_tracks->size(); ++ftk_track_index){

    /// Check if the track is inside the ROI ///

    const FTK_RawTrack* ftk_track= m_ftk_tracks->at(ftk_track_index);
    if (roi.isFullscan() || (roi.containsPhi(ftk_track->getPhi()) && roi.contains(ftk_track->getZ0(), ftk_track->getCotTh()))) {

      Trk::Track* track = this->getCachedTrack(ftk_track_index, withRefit);
      if (track != nullptr) userTracks->push_back(track);
    }
  }
  if (withRefit) {
    ATH_MSG_DEBUG( "getTracksinRoi: Return refitted TrackContainer Size = " << userTracks->size());
  } else { 
    ATH_MSG_DEBUG( "getTracksinRoi: Return TrackContainer Size = " << userTracks->size());
  }

  return userTracks;

}

Trk::Track* FTK_DataProviderSvc::getCachedTrack(const unsigned int ftk_track_index, const bool withRefit) {

  Trk::Track *track= nullptr;
  Trk::Track* refitTrack= nullptr;

  if( m_conv_track_map[ftk_track_index]>-1) { /// Track already converted ///
    track = m_conv_tracks->at((unsigned int) m_conv_track_map[ftk_track_index]);
    if (track == nullptr) {
      ATH_MSG_VERBOSE( "getCachedTrack:  failed to retrieve converted track with index " << m_conv_track_map[ftk_track_index] << " corresponding to FTK track " << ftk_track_index);
      return nullptr;
    } else {
      ATH_MSG_VERBOSE( "getCachedTrack:  retrieved converted track with index " << m_conv_track_map[ftk_track_index] << " corresponding to FTK track " << ftk_track_index);
    }
    
  } else { /// This track hasn't been converted yet, do it now ///                                                                                                                           
    
    ATH_MSG_VERBOSE( "getCachedTrack: ConversionLoop: FTK Track " << ftk_track_index << " not yet converted, beginning conversion");
    Trk::Track *new_track = ConvertTrack(ftk_track_index);
    if (new_track != nullptr) {
      m_conv_track_map[ftk_track_index] = (int) m_conv_tracks->size();
      ATH_MSG_VERBOSE( "getCachedTrack:  adding coverted track to cache at index " <<  m_conv_tracks->size() << " corresponding to FTK track " << ftk_track_index);
      m_conv_tracks->push_back(new_track);
      track = m_conv_tracks->at((unsigned int) m_conv_track_map[ftk_track_index]);
      ATH_MSG_VERBOSE( "getCachedTrack: add track summary information to converted track");
      m_trackSumTool->updateTrack(*track);
    } else {
      m_conv_track_map[ftk_track_index] = -2;
      ATH_MSG_VERBOSE( "getCachedTrack: conversion failed, marking ftk track with index " << ftk_track_index << " as bad");
    }
    
  } 
  
  if (!withRefit) return track;
  
  if  (m_refit_track_map[ftk_track_index] > -1) {
    // track has already been refitted //
    refitTrack = m_refit_tracks->at((unsigned int) m_refit_track_map[ftk_track_index]);
    if (refitTrack == nullptr) {
      ATH_MSG_VERBOSE( "getCachedTrack:  failed to retrieve refitted track with index " << m_refit_track_map[ftk_track_index] << " corresponding to FTK track " << ftk_track_index);
      m_refit_track_map[ftk_track_index] = -2;
    }
  } else if (m_conv_track_map[ftk_track_index] != -2 && m_refit_track_map[ftk_track_index]!=-2){ 
    // Need to do refit//
    ATH_MSG_VERBOSE( "getCachedTrack: Beginning Track Refit for FTK track " << ftk_track_index);
    Trk::Track *newRefitTrack = m_trackFitter->fit(*track, false, Trk::pion);
    if(newRefitTrack == nullptr || newRefitTrack == 0 ) {
      ATH_MSG_DEBUG("getCachedTrack: refit failed - refitted track coresponding to FTK track with index " << ftk_track_index << " has NOT added to  output collection");
      m_refit_track_map[ftk_track_index] = -2;
      return nullptr;
    } else { 
      ATH_MSG_VERBOSE( "getCachedTrack: Track Refit SUCEEDED");
      ATH_MSG_VERBOSE( "getCachedTrack:  adding refitted track to cache at index " <<  m_refit_tracks->size() << " corrsponding to FTK track " << ftk_track_index);
      m_refit_track_map[ftk_track_index] = (int) m_refit_tracks->size();
      m_refit_tracks->push_back(newRefitTrack);
      refitTrack = m_refit_tracks->at((unsigned int) m_refit_track_map[ftk_track_index]);
      ATH_MSG_VERBOSE( "getCachedTrack:  adding track summary information to refitted track");
      m_trackSumTool->updateTrack(*refitTrack);
    }
  }
  ATH_MSG_VERBOSE( "getCachedTrack: returning refitted track ");
  return refitTrack;
}


StatusCode FTK_DataProviderSvc::getFTK_RawTracksFromSG(){
  /// get the FTK Track pointers from StoreGate ///

  if (!m_newEvent) {
    return StatusCode::SUCCESS;
  }
  m_newEvent=false;


  if(m_doTruth) {//get MC-truth collections
    m_collectionsReady=true;
    StatusCode sc = getTruthCollections();
    if(sc.isFailure()) {
      ATH_MSG_DEBUG("PRD FTK Truth making failed ...");
      m_collectionsReady=false;
    }
  }

  // new event - get the tracks from StoreGate

  if (!m_storeGate->contains<FTK_RawTrackContainer>(m_RDO_key)) {
    ATH_MSG_DEBUG( "getFTK_RawTracksFromSG: FTK tracks  "<< m_RDO_key <<" not found in StoreGate !");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_VERBOSE( "getFTK_RawTracksFromSG:  Doing storegate retreive");
    StatusCode sc = m_storeGate->retrieve(m_ftk_tracks, m_RDO_key);
    if (sc.isFailure()) {
      ATH_MSG_VERBOSE( "getFTK_RawTracksFromSG: Failed to get FTK Tracks Container");
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG( "getFTK_RawTracksFromSG:  Got " << m_ftk_tracks->size() << " raw FTK tracks (RDO) from  StoreGate ");
    m_gotRawTracks = true;

  }
  // Creating collection for pixel clusters

  m_PixelClusterContainer = new InDet::PixelClusterContainer(m_pixelId->wafer_hash_max());
  m_PixelClusterContainer->addRef();
  StatusCode sc = m_storeGate->record(m_PixelClusterContainer,m_PixelClusterContainerName);
  if (sc.isFailure()) {
    ATH_MSG_DEBUG("Error registering the FTK pixel container in the SG");
    return sc;
  }
  // Generic format link for the pixel clusters
  const InDet::SiClusterContainer *symSiContainerPxl(0x0);
  sc = m_storeGate->symLink(m_PixelClusterContainer,symSiContainerPxl);
  if (sc.isFailure()) {
    ATH_MSG_DEBUG("Error creating the sym-link to the Pixel clusters");
    return sc;
  }
  // Creating collection for the SCT clusters
  m_SCT_ClusterContainer = new InDet::SCT_ClusterContainer(m_sctId->wafer_hash_max());
  m_SCT_ClusterContainer->addRef();
  sc = m_storeGate->record(m_SCT_ClusterContainer,m_SCT_ClusterContainerName);
  if (sc.isFailure()) {
    ATH_MSG_DEBUG("Error registering the FTK SCT container in the SG");
    return sc;
  }
  // Generic format link for the pixel clusters
  const InDet::SiClusterContainer *symSiContainerSCT(0x0);
  sc = m_storeGate->symLink(m_SCT_ClusterContainer,symSiContainerSCT);
  if (sc.isFailure()) {
    ATH_MSG_DEBUG("Error creating the sym-link to the SCT clusters");
    return sc;
  }
  

  return  StatusCode::SUCCESS;
}

StatusCode FTK_DataProviderSvc::initTrackCache(bool withRefit) {

  StatusCode sc=StatusCode::SUCCESS;

  if (!m_storeGate->contains<TrackCollection>(m_trackCacheName)) {
    
    m_conv_tracks = new TrackCollection;
    sc = m_storeGate->record(m_conv_tracks, m_trackCacheName);
    if (sc.isFailure()) {
      ATH_MSG_DEBUG( "initTrackCache: Failed to record TrackCollection " <<  m_trackCacheName << " No FTK tracks returned");
      delete(m_conv_tracks);
      return sc;
    }
    for (unsigned int i = 0; i!=m_ftk_tracks->size(); i++) m_conv_track_map.push_back(-1);
    for (unsigned int i = 0; i!=m_ftk_tracks->size(); i++) m_refit_track_map.push_back(-1);
    ATH_MSG_VERBOSE( "initTrackCache: converted track cache initialised ");
  }
  if (withRefit) {
    if (!m_storeGate->contains<TrackCollection>(m_trackCacheName+"Refit")) {

      m_refit_tracks = new TrackCollection;
      sc = m_storeGate->record(m_refit_tracks, m_trackCacheName+"Refit");
      if (sc.isFailure()) {
        ATH_MSG_DEBUG( "initTrackCache: Failed to record TrackCollection " <<  m_trackCacheName << "Refit - No FTK tracks returned");
        delete(m_refit_tracks);
        return sc;
      }
      ATH_MSG_VERBOSE( "initTrackCache: refitted track cache initialised ");
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
        ATH_MSG_DEBUG( "initTrackParticleCache: Failed to record TrackParticleContainer " <<  m_trackParticleCacheName << "Refit No FTK TrackspParticles returned");
        delete(m_refit_tp);
        return sc;
      }
    }

    if (!m_storeGate->contains<xAOD::TrackParticleAuxContainer>(m_trackParticleCacheName+"AuxRefit")) {
      ATH_MSG_VERBOSE( "initTrackParticleCache: initialising auxstore for refitted TrackParticles"); 
      m_refit_tpAuxCont =  new xAOD::TrackParticleAuxContainer();
      sc = m_storeGate->record(m_refit_tpAuxCont, m_trackParticleCacheName+"AuxRefit");
      if (sc.isFailure()) {
        ATH_MSG_DEBUG( "initTrackParticleCache: Failed to record TrackParticleAuxCollection " <<  m_trackParticleCacheName << "AuxRefit No FTK tracks returned");
        delete(m_refit_tpAuxCont);
        return sc;
      }

      m_refit_tp->setStore( m_refit_tpAuxCont);
      for (unsigned int i = 0; i!=m_ftk_tracks->size(); i++) m_refit_tp_map.push_back(-1);
      ATH_MSG_VERBOSE( "initTrackParticleCache: cache initialized for refitted TracksParticles"); 

    }
  } else {

    if (!m_storeGate->contains<xAOD::TrackParticleContainer>(m_trackParticleCacheName)) {
      m_conv_tp=  new xAOD::TrackParticleContainer();
      sc = m_storeGate->record(m_conv_tp, m_trackParticleCacheName);
      if (sc.isFailure()) {
        ATH_MSG_DEBUG( "initTrackParticleCache: Failed to record TrackParticleCollection " <<  m_trackParticleCacheName << " No FTK tracks returned");
        delete(m_conv_tp);
        return sc;
      }
    }

    if (!m_storeGate->contains<xAOD::TrackParticleAuxContainer>(m_trackParticleCacheName+"Aux")) {
      m_conv_tpAuxCont =  new xAOD::TrackParticleAuxContainer();
      sc = m_storeGate->record(m_conv_tpAuxCont, m_trackParticleCacheName+"Aux");
      if (sc.isFailure()) {
        ATH_MSG_DEBUG( "initTrackParticleCache: Failed to record TrackParticleAuxContainer " <<  m_trackParticleCacheName << "Aux - No FTK tracks returned");
        delete(m_conv_tpAuxCont);
        return sc;
      }

      m_conv_tp->setStore( m_conv_tpAuxCont);
      for (unsigned int i = 0; i!=m_ftk_tracks->size(); i++) m_conv_tp_map.push_back(-1);
      ATH_MSG_VERBOSE( "initTrackParticleCache: cache initialized for converted TracksParticles"); 
    }

  }
  return sc;
}



Trk::Track* FTK_DataProviderSvc::ConvertTrack(const unsigned int iTrack){
  //std::unique_ptr< Trk::Track > FTK_DataProviderSvc::ConvertTrack(int iTrack){

  const FTK_RawTrack &track = *m_ftk_tracks->at(iTrack);

  ATH_MSG_VERBOSE( "ConvertTrack: Beginning Track conversion");

  double trkTheta = std::atan2(1.0,track.getCotTh());
  double trkEta = -std::log(std::tan(trkTheta/2));

  ATH_MSG_VERBOSE( "ConvertTrack: track theta: " << trkTheta << " eta: " << trkEta);

  DataVector<const Trk::TrackStateOnSurface>* trkTSoSVec = new DataVector<const Trk::TrackStateOnSurface>;

  std::vector<InDet::SiClusterOnTrack*> clusters;

  // Create Pixel Clusters
  bool hasIBL=false;

  float pixelPhi=0;
  float pixelEta=0;

  ATH_MSG_VERBOSE( "   ConvertTrack: PixelClusterLoop: Pixel Clusters size = " << track.getPixelClusters().size());
  for( unsigned int cluster_number = 0; cluster_number < track.getPixelClusters().size(); ++cluster_number){
    if(track.getPixelClusters().size() > 4){
      ATH_MSG_DEBUG( "   ConvertTrack: PixelClusterLoop: Too Many Pixel Clusters! " << track.getPixelClusters().size());
      break;
    }
    //    ATH_MSG_VERBOSE( "   ConvertTrack: PixelClusterLoop: Creating Pixel Cluster Number " << cluster_number);
    FTK_RawPixelCluster raw_pixel_cluster = track.getPixelCluster(cluster_number);
    InDet::PixelCluster* pixel_cluster = createPixelCluster(raw_pixel_cluster,trkEta);
    if(pixel_cluster != nullptr) {
      Identifier wafer_id = m_pixelId->wafer_id(pixel_cluster->identify());
      if (m_pixelId->barrel_ec(wafer_id)==0 && m_pixelId->layer_disk(wafer_id)==0) hasIBL=true;
      const IdentifierHash idHash = m_pixelId->wafer_hash(m_pixelId->wafer_id(pixel_cluster->identify()));

      //ATH_MSG_VERBOSE( "   ConvertTrack: PixelClusterLoop: InDet::PixelCluster Created ");
      InDet::PixelClusterCollection* pColl = getPixelClusterCollection(idHash);
      if(pColl!=NULL) {
	pixel_cluster->setHashAndIndex(pColl->identifyHash(), pColl->size()); 
	pColl->push_back(pixel_cluster); 
      

	InDet::PixelClusterOnTrack* pixel_cluster_on_track = new InDet::PixelClusterOnTrack(pixel_cluster,
											    pixel_cluster->localPosition(),
											    pixel_cluster->localCovariance(),
											    idHash,
											    pixel_cluster->globalPosition(),
											    pixel_cluster->gangedPixel());
	
	ATH_MSG_VERBOSE( " Pixel Cluster "  << cluster_number << ": r= " << std::sqrt(std::pow(pixel_cluster_on_track->globalPosition().x(),2)+pow(pixel_cluster_on_track->globalPosition().y(),2))<<
			 "  z= " << pixel_cluster_on_track->globalPosition().z() << " phi " << pixel_cluster_on_track->globalPosition().phi() << " eta " << pixel_cluster_on_track->globalPosition().eta());

	clusters.push_back(pixel_cluster_on_track);
	pixelPhi =  pixel_cluster_on_track->globalPosition().phi();
	pixelEta =  pixel_cluster_on_track->globalPosition().eta();
      }

    } else{
      ATH_MSG_DEBUG( "   ConvertTrack: PixelClusterLoop: Failed to create pixel hit "<< cluster_number << " skipped ");
    }
  }


  //
  // Create the SCT Clusters
  //


  
  std::vector<InDet::SCT_ClusterOnTrack*> SCT_Clusters;

  ATH_MSG_VERBOSE( "   ConvertTrack: SCTClusterLoop: SCT Clusters size = " << track.getSCTClusters().size());
  for( unsigned int cluster_number = 0; cluster_number < track.getSCTClusters().size(); ++cluster_number){
    if(track.getSCTClusters().size() > 8){
      ATH_MSG_DEBUG( "   ConvertTrack: SCTClusterLoop: Too Many SCT Clusters! " << track.getPixelClusters().size());
      break;
    }
    //    ATH_MSG_VERBOSE( "   ConvertTrack: SCTClusterLoop: Creating SCT Cluster Number " << cluster_number);
    FTK_RawSCT_Cluster raw_cluster = track.getSCTCluster(cluster_number);
    InDet::SCT_Cluster* sct_cluster = createSCT_Cluster(raw_cluster.getModuleID(), raw_cluster.getHitCoord(), raw_cluster.getHitWidth());
    if (m_fixBadTracks) {
      IdentifierHash id2 =  raw_cluster.getModuleID() + 0x1000;
      if ( id2 <  m_sctId->wafer_hash_max()) {

	InDet::SCT_Cluster* sct_cluster2 = createSCT_Cluster(raw_cluster.getModuleID()+0x1000, raw_cluster.getHitCoord(), raw_cluster.getHitWidth());
	if(sct_cluster != nullptr && sct_cluster2 != nullptr) {
	  float dphi1 = fabs(sct_cluster->globalPosition().phi() - pixelPhi);
	  while (dphi1 > CLHEP::twopi) dphi1-=CLHEP::twopi;
	  float dphi2 = fabs(sct_cluster2->globalPosition().phi() - pixelPhi);
	  while (dphi2 > CLHEP::twopi) dphi2-=CLHEP::twopi;
	  float deta1 = sct_cluster->globalPosition().eta() - pixelEta;
	  float deta2 = sct_cluster2->globalPosition().eta() - pixelEta;
	  float dr1=square(dphi1) + square(deta1);
	  float dr2=square(dphi2) + square(deta2);
	  if (dr1 <= dr2) {
	  
	    ATH_MSG_VERBOSE( " choosing cluster1 r= " << std::sqrt(std::pow(sct_cluster->globalPosition().x(),2)+pow(sct_cluster->globalPosition().y(),2))<<
			     "  z= " << sct_cluster->globalPosition().z() << "  mag= " << sct_cluster->globalPosition().mag() << 
			     " phi= " << sct_cluster->globalPosition().phi()<< " eta = " << sct_cluster->globalPosition().eta()<< 
			     "  " << m_id_helper->print_to_string(sct_cluster->identify() ));
	    ATH_MSG_VERBOSE( " dphi1 " << dphi1 << "deta1 " << deta1 << " dr1 " << dr1); 
	    ATH_MSG_VERBOSE( " instead of cluster2 r= " << std::sqrt(std::pow(sct_cluster2->globalPosition().x(),2)+pow(sct_cluster2->globalPosition().y(),2))<<
			     "  z= " << sct_cluster2->globalPosition().z() << "  mag= " << sct_cluster2->globalPosition().mag() << " phi= " << sct_cluster2->globalPosition().phi()<< " eta = " << sct_cluster2->globalPosition().eta()<< 
			     "  " << m_id_helper->print_to_string(sct_cluster2->identify() ));
	    ATH_MSG_VERBOSE( " dphi2 " << dphi2 << "  deta2 " << deta2 << "  dr2 " << dr2); 
	  } else {
	    ATH_MSG_VERBOSE( " choosing cluster2 r= " << std::sqrt(std::pow(sct_cluster2->globalPosition().x(),2)+pow(sct_cluster2->globalPosition().y(),2))<<
			     "  z= " << sct_cluster2->globalPosition().z() << "  mag= " << sct_cluster2->globalPosition().mag() << 
			     " phi= " << sct_cluster2->globalPosition().phi()<< " eta = " << sct_cluster2->globalPosition().eta()<< 
			     "  " << m_id_helper->print_to_string(sct_cluster2->identify() ));
	    ATH_MSG_VERBOSE( " dphi2 " << dphi2 << "  deta2 " << deta2 << "  dr2 " << dr2); 
	    ATH_MSG_VERBOSE( " instead of cluster1 itr r= " << std::sqrt(std::pow(sct_cluster->globalPosition().x(),2)+pow(sct_cluster->globalPosition().y(),2))<<
			     "  z= " << sct_cluster->globalPosition().z() << "  mag= " << sct_cluster->globalPosition().mag() << " phi= " << sct_cluster->globalPosition().phi()<< " eta = " << sct_cluster->globalPosition().eta()<< 
			     "  " << m_id_helper->print_to_string(sct_cluster->identify() ));
	    ATH_MSG_VERBOSE( " dphi1 " << dphi1 << "  deta1 " << deta1 << "  dr1 " << dr1); 
	    
	    delete (sct_cluster);
	    sct_cluster = sct_cluster2;
	  }
	}
      }
    }
    if(sct_cluster != nullptr) {

      //      ATH_MSG_VERBOSE( "   ConvertTrack: SCTClusterLoop: Created SCT_Cluster SUCCESS ");
      const IdentifierHash idHash = m_sctId->wafer_hash(m_sctId->wafer_id(sct_cluster->identify()));
      
      InDet::SCT_ClusterCollection* pColl = getSCT_ClusterCollection(idHash);
      if(pColl!=NULL) {
	sct_cluster->setHashAndIndex(pColl->identifyHash(), pColl->size()); 
	pColl->push_back(sct_cluster); 
	
      
      
	InDet::SCT_ClusterOnTrack* sct_cluster_on_track = new InDet::SCT_ClusterOnTrack(sct_cluster,
										      sct_cluster->localPosition(),
										      sct_cluster->localCovariance(),
										      idHash,
										      sct_cluster->globalPosition());
	ATH_MSG_VERBOSE( cluster_number << ": r= " << std::sqrt(std::pow(sct_cluster_on_track->globalPosition().x(),2)+pow(sct_cluster_on_track->globalPosition().y(),2))<<
		       "  z= " << sct_cluster_on_track->globalPosition().z());

	if(m_doTruth && m_collectionsReady) {
	  ATH_MSG_VERBOSE("SCT cluster MC barcode = "<<raw_cluster.getBarcode() );
	  createSCT_Truth(sct_cluster->identify(), raw_cluster.getBarcode() );
	}



	clusters.push_back(sct_cluster_on_track);
      }
    } else {
      ATH_MSG_DEBUG( "   ConvertTrack: SCTClusterLoop: Created SCT_Cluster FAILED ");
    }
  }

  std::sort(clusters.begin(), clusters.end(), compareFTK_Clusters);

  if (m_rejectBadTracks) {
    int itr=0;
    float meanEta = 0;
    float meanPhi=0;
    for (auto pClus=clusters.begin(); pClus!=clusters.end(); pClus++, itr++) {
      
      ATH_MSG_VERBOSE( itr << ": " << " r= " << std::sqrt(std::pow((*pClus)->globalPosition().x(),2)+pow((*pClus)->globalPosition().y(),2))<<
		       "  z= " << (*pClus)->globalPosition().z() << "  mag= " << (*pClus)->globalPosition().mag() << " phi= " << (*pClus)->globalPosition().phi()<< " eta = " << (*pClus)->globalPosition().eta()<< 
		       "  " << m_id_helper->print_to_string((*pClus)->identify()) );
      meanEta+=(*pClus)->globalPosition().eta();
      meanPhi+=(*pClus)->globalPosition().phi();
    }
    
    meanEta = meanEta/(float) itr;
    meanPhi = meanPhi/(float) itr;
    itr = 0;

  
    bool problem = false;
    for (auto pClus=clusters.begin(); pClus!=clusters.end(); pClus++, itr++) {
      float dphi = fabs(((*pClus)->globalPosition().phi())-meanPhi);
      while (dphi > CLHEP::twopi) dphi-=CLHEP::twopi;
      if (fabs(((*pClus)->globalPosition().eta())-meanEta)>m_dEtaCut || fabs(dphi)>m_dPhiCut) {
	problem = true;
	break;
      }
    }
    if (problem) {
      ATH_MSG_DEBUG( " SEVERE problem with FTKHit information on FTKTrack " << iTrack << " No refit possible for this track:");
      m_refit_track_map[iTrack] = -2; // marke track as not good for refit.
      itr=0;
      for (auto pClus=clusters.begin(); pClus!=clusters.end(); pClus++, itr++) {
	float dphi = fabs(((*pClus)->globalPosition().phi())-meanPhi);
	while (dphi > CLHEP::twopi) dphi-=CLHEP::twopi;
	if (fabs(((*pClus)->globalPosition().eta())-meanEta)>m_dEtaCut || fabs(dphi)>m_dPhiCut) {
	  ATH_MSG_DEBUG ("*"<< itr << ": " << "  z= " << (*pClus)->globalPosition().z() << "  r= " << std::sqrt(std::pow((*pClus)->globalPosition().x(),2)+pow((*pClus)->globalPosition().y(),2))<<
			   "  mag= " << (*pClus)->globalPosition().mag() << "  phi= " << (*pClus)->globalPosition().phi()<< 
			   " dPhi " << (*pClus)->globalPosition().phi()-meanPhi << "   eta = " << (*pClus)->globalPosition().eta() << "  deta " <<  
			 (*pClus)->globalPosition().eta()-meanEta << "  " << m_id_helper->print_to_string((*pClus)->identify()));
	} else {
	  ATH_MSG_DEBUG( " "<< itr << ": " << "  z= " << (*pClus)->globalPosition().z() << "  r= " << std::sqrt(std::pow((*pClus)->globalPosition().x(),2)+pow((*pClus)->globalPosition().y(),2))<<
			   "  mag= " << (*pClus)->globalPosition().mag() << "  phi= " << (*pClus)->globalPosition().phi()<< 
			   " dPhi " << (*pClus)->globalPosition().phi()-meanPhi << "   eta = " << (*pClus)->globalPosition().eta() << "  deta " <<  
			   (*pClus)->globalPosition().eta()-meanEta << "  " << m_id_helper->print_to_string((*pClus)->identify()));
	  
	}
	
	delete (*pClus);
      }
      return nullptr;
      
    }
  }


  ATH_MSG_VERBOSE(" Creating TrackStateOnSurface:");
  int itr=0;
  for (auto pClus=clusters.begin(); pClus!=clusters.end(); pClus++, itr++) {
    
    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
    typePattern.set(Trk::TrackStateOnSurface::Measurement);
    
    Trk::TrackStateOnSurface* trackStateOnSurface = new Trk::TrackStateOnSurface(*pClus, 0, 0, 0, typePattern);
    trkTSoSVec->push_back(trackStateOnSurface);
    
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


  ATH_MSG_VERBOSE( " trkTrackCovm: (d0,z0,phi,theta,q/p)= " <<  (*trkTrackCovm)(0,0) << ", "<< (*trkTrackCovm)(1,1)<< ", " << 
      (*trkTrackCovm)(2,2)<< ", "<<(*trkTrackCovm)(3,3)<< ", "<<  (*trkTrackCovm)(4,4));

  //
  // Construct the measured Perigee Parameters
  //

  double trkQOverP = sin(trkTheta)*track.getCurv()/m_pTscaleFactor; // note track.getCurv() currently returns q/pT not curv!
  Trk::PerigeeSurface periSurf;

  double dx = m_trainingBeamspotX + m_trainingBeamspotTiltX*track.getZ0();//correction for tilt
  double dy = m_trainingBeamspotY + m_trainingBeamspotTiltY*track.getZ0();//correction for tilt

  double d0 = track.getD0();
  d0 = d0-dx*sin( track.getPhi())+dy*cos( track.getPhi());


  const Trk::Perigee* trkPerigee = new Trk::Perigee( d0,
      track.getZ0() + m_trainingBeamspotZ,
      track.getPhi(),
      trkTheta,
      trkQOverP,
      periSurf,
      trkTrackCovm);

  ATH_MSG_VERBOSE( "   ConvertTrack: Track perigee created  d0 " <<  d0 << " z0 " <<
      track.getZ0() + m_trainingBeamspotZ << " phi0 " << track.getPhi() << " theta " << trkTheta << " q/P " << trkQOverP);
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
    ATH_MSG_DEBUG("   ConvertTrack: Warning ndof= " << ndof << " Track has only " 
        << track.getPixelClusters().size() << track.getSCTClusters().size() << " sct hits ");
  }
  const Trk::FitQuality* trkFitQuality = new Trk::FitQuality(track.getChi2(), chi2OverNdof);

  //
  // And finally the Trk::Track
  //
  Trk::Track* new_track = new Trk::Track( trkTrackInfo, trkTSoSVec, trkFitQuality);

  //******************************


  return new_track;
}


InDet::SCT_Cluster* FTK_DataProviderSvc::createSCT_Cluster(const IdentifierHash hash, const int strip, const int clusterWidth) {


  const InDetDD::SiDetectorElement* pDE = m_SCT_Manager->getDetectorElement(hash);
  float locPos = strip+0.1; // adding 0.1 to prevent rounding errors                                                                                      

  int w = clusterWidth;
  if (w==0) {
    ATH_MSG_DEBUG( " SCT FTKHit HitCoord " << locPos << " has zero width. Setting to 1 " <<  m_id_helper->print_to_string(pDE->identify()));
    w = 1;
  }
  Identifier wafer_id = m_sctId->wafer_id(hash);
  Identifier strip_id = m_sctId->strip_id(wafer_id, strip);

  const InDetDD::SCT_ModuleSideDesign* design;
  if (m_sctId->is_barrel(wafer_id)){
    design = (static_cast<const InDetDD::SCT_BarrelModuleSideDesign*>(&pDE->design()));
  } else{
    design = (static_cast<const InDetDD::SCT_ForwardModuleSideDesign*>(&pDE->design()));
  }

  std::vector<Identifier> rdoList;
  rdoList.push_back(strip_id);

  int firstStrip = (int)(locPos+0.5-0.5*(w-1));
  int lastStrip  = (int)(locPos+0.5+0.5*(w-1));

  ATH_MSG_VERBOSE( "FTK_DataProviderSvc::createSCT_Cluster: raw coord= " << strip << " width " << w << " locpos= " <<
      locPos << " strip, firstStrip, lastStrip= "  << strip << "," << firstStrip << "," << lastStrip);

  if (firstStrip < 0) {
    firstStrip = 0;
    ATH_MSG_VERBOSE( " firstStrip was " << firstStrip << " setting to 0");
  }
  if (lastStrip >= design->cells()) {
    lastStrip = design->cells()-1;
    ATH_MSG_VERBOSE( " lastStrip was " << lastStrip << " setting to " << design->cells()-1);
  }
  const int nStrips(lastStrip - firstStrip + 1);

  const InDetDD::SiCellId cell1(firstStrip - 1);
  const InDetDD::SiCellId cell2(lastStrip + 1);

  const InDetDD::SiLocalPosition firstStripPos(pDE->rawLocalPositionOfCell(cell1));
  const InDetDD::SiLocalPosition lastStripPos(pDE->rawLocalPositionOfCell(cell2));

  const double width((double(nStrips)/double(nStrips+1))*( lastStripPos.xPhi()-firstStripPos.xPhi()));
  const InDetDD::SiLocalPosition centre((firstStripPos+lastStripPos)/2.0);

  Amg::Vector2D localPos(centre.xPhi(),  centre.xEta());

  ATH_MSG_VERBOSE(" centre.xPhi() " << centre.xPhi()  << " centre.xEta() " << centre.xEta());


  const std::pair<InDetDD::SiLocalPosition, InDetDD::SiLocalPosition> ends(design->endsOfStrip(centre));
  double stripLength(fabs(ends.first.xEta()-ends.second.xEta()));

  ATH_MSG_VERBOSE(" creating SiWidth with nstrips   = " << w << " width " << width << " stripLength " << stripLength); 
  InDet::SiWidth siWidth(Amg::Vector2D(w,1), Amg::Vector2D(width,stripLength) );

  double shift = pDE->getLorentzCorrection();

  double derivedPos = localPos[Trk::locX]+shift;
  double rawPos = (strip-0.5*design->cells())*pDE->phiPitch();

  if(fabs(derivedPos-rawPos)>0.5*pDE->phiPitch()) {
    derivedPos = rawPos+shift;
  }

  Amg::Vector2D position(derivedPos, localPos[Trk::locY]);

  Amg::MatrixX* cov = new Amg::MatrixX(2,2);
  cov->setZero();

  (*cov)(0,0) = siWidth.phiR()*siWidth.phiR()* ONE_TWELFTH;
  (*cov)(1,1) = siWidth.z()*siWidth.z()*ONE_TWELFTH;
  ATH_MSG_VERBOSE(" siWidth.phiR() " << siWidth.phiR() << " siWidth.z() " << siWidth.z() );

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
    ATH_MSG_VERBOSE(" pDE->sinStereoLocal(localPos) " << pDE->sinStereoLocal(localPos) << " pDE->phiPitch(localPos) " << pDE->phiPitch(localPos) << " pDE->phiPitch() " << pDE->phiPitch() << " siWidth.phiR() " << siWidth.phiR() << " siWidth.z() " << siWidth.z() );
  }

  InDet::SCT_Cluster* pCL = new InDet::SCT_Cluster(strip_id, position, rdoList, siWidth, pDE, cov);
  ATH_MSG_VERBOSE("covariance " << (*cov)(0,0) << ", " << (*cov)(0,1)); 
  ATH_MSG_VERBOSE("           " << (*cov)(1,0) << ", " << (*cov)(1,0)) ;

  return pCL;
}





InDet::PixelCluster* FTK_DataProviderSvc::createPixelCluster(const FTK_RawPixelCluster& raw_pixel_cluster, float eta) {
  IdentifierHash hash = raw_pixel_cluster.getModuleID(); 
  Identifier wafer_id = m_pixelId->wafer_id(hash); // Need to set up this tool

  ATH_MSG_VERBOSE("createPixelCluster: New FTK pixel cluster with hash Ox" << std::hex<< hash << " wafer id " << wafer_id << std::dec);

  const InDetDD::SiDetectorElement* pixelDetectorElement = m_pixelManager->getDetectorElement(hash);
  const InDetDD::PixelModuleDesign* design
    (dynamic_cast<const InDetDD::PixelModuleDesign*>(&pixelDetectorElement->design()));

  ATH_MSG_VERBOSE( "FTK_DataProviderSvc::createPixelCluster: raw FTK cluster position: " <<
      " Row(phi): " <<  raw_pixel_cluster.getRowCoord() << " Col(eta): " << raw_pixel_cluster.getColCoord() << 
      " RowWidth: " << raw_pixel_cluster.getRowWidth() << " ColWidth: " << raw_pixel_cluster.getColWidth());

  if (m_pixelId->barrel_ec(wafer_id)==0) {
    if (m_pixelId->layer_disk(wafer_id)==0) {
      ATH_MSG_VERBOSE( " IBL ");
    } else {
      ATH_MSG_VERBOSE( " Pixel Barrel layer  " << m_pixelId->layer_disk(wafer_id));
    }
  } else {
    ATH_MSG_VERBOSE( " Pixel Endcap layer  " << m_pixelId->layer_disk(wafer_id));
  }
  ATH_MSG_VERBOSE( " Module rows= " << design->rows() << " phiPitch= " << design->phiPitch() << " width= " << design->width() ); 
  ATH_MSG_VERBOSE( " columns = " << design->columns() << " etaPitch= " << design->etaPitch() <<  " length " << design->length());

  int phiIndex = raw_pixel_cluster.getRowCoord();
  int etaIndex = raw_pixel_cluster.getColCoord();

  const InDetDD::SiCellId cell(phiIndex, etaIndex);



  if(m_pixelId->barrel_ec(wafer_id)!=0 && m_pixelId->layer_disk(wafer_id)==0 && phiIndex==0 && etaIndex==0) { // ***temp hack - remove bad hits
    ATH_MSG_DEBUG( "FTK_DataProviderSvc::createPixelCluster: Invalid Cell barrel_ec=0 layer_disk=0, phiIndex= "
		     << phiIndex << "etaIndex= "<< etaIndex);
    return nullptr;
  }

  if(!cell.isValid()) {
    ATH_MSG_DEBUG( "FTK_DataProviderSvc::createPixelCluster: Invalid Cell at phiIndex, etaIndex "<<  phiIndex << ", " << etaIndex);
    return nullptr;
  }

  const InDetDD::SiLocalPosition cellLocalPosition(pixelDetectorElement->rawLocalPositionOfCell(cell));

  double phiOffset = (raw_pixel_cluster.getRowCoord() - (float) phiIndex) *  design->phiPitch();
  double etaOffset = (raw_pixel_cluster.getColCoord() - (float) etaIndex) *  design->etaPitch();

  double phiPos = cellLocalPosition.xPhi() + phiOffset;
  double etaPos = cellLocalPosition.xEta() + etaOffset;
  ATH_MSG_VERBOSE( "FTK_DataProviderSvc::createPixelCluster: local coordinates phiPos, etaPos "<<  phiPos << ", " << etaPos);

  //InDetDD::SiLocalPosition silPos(phiPos, etaPos);// use the converted positions in mm !!!
  //InDetDD::SiCellId cell =  design->cellIdOfPosition(silPos);
  int phi_index = cell.phiIndex();
  int eta_index = cell.etaIndex();

  ATH_MSG_VERBOSE( " created cell with phiIndex, etaIndex " << cell.phiIndex() << ", " << cell.etaIndex());

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

  ATH_MSG_VERBOSE("FTK_DataProviderSvc::createPixelCluster: local coordinates phiPos, etaPos"<<  phiPos+shift << ", " << etaPos << " includes Lorentz shift " << shift);
  ATH_MSG_VERBOSE(" phiwidth " << phiWidth << " etawidth " <<  etaWidth << " siWidth.phiR() " << siWidth.phiR() << " siWidth.z() " << siWidth.z());

  // bool blayer = pixelDetectorElement->isBlayer();

  std::vector<Identifier> rdoList;
  rdoList.push_back(pixel_id);

  Amg::MatrixX* cov = new Amg::MatrixX(2,2);
  cov->setZero();

  (*cov)(0,0) = siWidth.phiR()*siWidth.phiR()*ONE_TWELFTH;
  (*cov)(1,1) = siWidth.z()*siWidth.z()* ONE_TWELFTH;

  ATH_MSG_VERBOSE("Setting defaulterrors (*cov)(0,0) " <<  (*cov)(0,0) << " (*cov)(1,1)" << (*cov)(1,1) );

  if(m_offlineCalibSvc != 0) {
    ATH_MSG_VERBOSE("Using offlineCalibSvc" );
    const Amg::Vector2D& colRow = siWidth.colRow();
    double averageZPitch = siWidth.z()/colRow.y();
    ATH_MSG_VERBOSE(" siWidth.colRow() " << siWidth.colRow() << " siWidth.z() " << " siWidth.z() " <<   " colRow.y() " << colRow.y() << "  averageZpitch " << averageZPitch  );


    // use parameterization only if the cluster does not 
    // contain long pixels or ganged pixels
    // Also require calibration service is available....
   
    if(averageZPitch > 399*micrometer && averageZPitch < 401*micrometer){
      if(pixelDetectorElement->isBarrel()){
        // Barrel corrections //
        (*cov)(0,0) =  square(m_offlineCalibSvc->getBarrelErrorPhi(eta,int(colRow.y()),int(colRow.x())));
	(*cov)(1,1) = square(m_offlineCalibSvc->getBarrelErrorEta(eta,int(colRow.y()),int(colRow.x())));
	ATH_MSG_VERBOSE("Barrel Corrections " << (*cov)(0,0)<< " " <<  (*cov)(1,1) );
      } else{
        // End-cap corrections //
        (*cov)(0,0) = square(m_offlineCalibSvc->getEndCapErrorPhi(int(colRow.y()),int(colRow.x())));
        (*cov)(1,1) = square(m_offlineCalibSvc->getEndCapErrorEta(int(colRow.y()),int(colRow.x())));
	ATH_MSG_VERBOSE("Endcap Corrections " << (*cov)(0,0)<< " " <<  (*cov)(1,1) );
      }
    } else {
      ATH_MSG_VERBOSE(" Corrections for pixel with length!=400um " << averageZPitch);
      (*cov)(0,0) = square(siWidth.phiR()/colRow.x()) * ONE_TWELFTH  ;
      (*cov)(1,1) = square(averageZPitch)* ONE_TWELFTH  ;
    }
  } 
  InDet::PixelCluster* pixel_cluster = new InDet::PixelCluster(pixel_id, position, rdoList, siWidth,
      pixelDetectorElement, cov);
  ATH_MSG_VERBOSE("covariance " << (*cov)(0,0) << ", " << (*cov)(0,1)); 
  ATH_MSG_VERBOSE("           " << (*cov)(1,0) << ", " <<   (*cov)(1,0)) ;
  return pixel_cluster;
}




StatusCode FTK_DataProviderSvc::getTruthCollections() {
  

  StatusCode sc = m_storeGate->retrieve(m_mcEventCollection, m_mcTruthName ); 

  if ( sc.isFailure() ){ 
    ATH_MSG_DEBUG("Cannot retrieve McEventCollection "<<m_mcTruthName); 
    return StatusCode::FAILURE; 
  } 

  if(!m_storeGate->contains<PRD_MultiTruthCollection>(m_ftkPixelTruthName)) { 

     m_ftkPixelTruth = new PRD_MultiTruthCollection;
            
     sc=m_storeGate->record(m_ftkPixelTruth,m_ftkPixelTruthName,false); 
     if(sc.isFailure()) { 
       ATH_MSG_DEBUG("Pixel FTK Truth Container " << m_ftkPixelTruthName  
           <<" cannot be recorded in StoreGate !"); 
       return sc; 
     } else { 
       ATH_MSG_DEBUG("Pixel FTK Truth Container " << m_ftkPixelTruthName   
         << " is recorded in StoreGate"); 
     } 
  } else {     
    ATH_MSG_DEBUG("Pixel FTK Truth Container " << m_ftkPixelTruthName     
        << " is found in StoreGate"); 
  }

  if(!m_storeGate->contains<PRD_MultiTruthCollection>(m_ftkSctTruthName)) { 

     m_ftkSctTruth = new PRD_MultiTruthCollection;
            
     sc=m_storeGate->record(m_ftkSctTruth,m_ftkSctTruthName,false); 
     if(sc.isFailure()) { 
       ATH_MSG_DEBUG("SCT FTK Truth Container " << m_ftkSctTruthName  
		 <<" cannot be recorded in StoreGate !"); 
       return sc; 
     } else { 
	 ATH_MSG_DEBUG("SCT FTK Truth Container " << m_ftkSctTruthName   
		   << " is recorded in StoreGate"); 
     } 
  } else {     
      ATH_MSG_DEBUG("SCT FTK Truth Container " << m_ftkSctTruthName     
		<< " is found in StoreGate"); 
  }
  return sc;
}

void FTK_DataProviderSvc::createSCT_Truth(Identifier id, int barCode) {

  bool found=false;
  unsigned eventIdx=0;
  McEventCollection::const_iterator event = m_mcEventCollection->begin();    
  for ( ; event != m_mcEventCollection->end(); ++event, ++eventIdx ){ 
    const HepMC::GenParticle* p = (*event)->barcode_to_particle(barCode);
    if(p!=NULL) {
      ATH_MSG_VERBOSE(" Found GenParticle for SCT hit barcode " << barCode << " pdg_id " << p->pdg_id());
      m_ftkSctTruth->insert(std::make_pair(id,HepMcParticleLink(p,eventIdx)));
      found=true;
      break;
    }
  }
  if (not found) ATH_MSG_VERBOSE(" GenParticle not found for SCT hit barcode " << barCode);
}

void FTK_DataProviderSvc::createPixelTruth(Identifier id, int barCode) {
  bool found=false;
  unsigned eventIdx=0;
  McEventCollection::const_iterator event = m_mcEventCollection->begin();    
  for ( ; event != m_mcEventCollection->end(); ++event, ++eventIdx ){ 
    const HepMC::GenParticle* p = (*event)->barcode_to_particle(barCode);
    if(p!=NULL) {
      ATH_MSG_VERBOSE(" Found GenParticle for Pixel hit barcode " << barCode << " pdg_id " << p->pdg_id());
      m_ftkPixelTruth->insert(std::make_pair(id,HepMcParticleLink(p,eventIdx)));
      found = true;
      break;
    }
  }
  if (not found) ATH_MSG_VERBOSE(" GenParticle not found for Pixel hit barcode " << barCode);
}


InDet::PixelClusterCollection* FTK_DataProviderSvc::getPixelClusterCollection(IdentifierHash hashId) {

  InDet::PixelClusterContainer::const_iterator it = m_PixelClusterContainer->indexFind(hashId);
  if(it!=m_PixelClusterContainer->end()) {
    InDet::PixelClusterCollection* pcoll = const_cast<InDet::PixelClusterCollection*>(&(**it));
    return pcoll;
  }
  Identifier id = m_pixelId->wafer_id(hashId);
  InDet::PixelClusterCollection* pcoll = new InDet::PixelClusterCollection(hashId);
  pcoll->setIdentifier(id);
  StatusCode sc = m_PixelClusterContainer->addCollection(pcoll,hashId);
  if (sc.isFailure()){
    delete pcoll;
    return NULL;
  }
  return pcoll;
}

InDet::SCT_ClusterCollection*  FTK_DataProviderSvc::getSCT_ClusterCollection(IdentifierHash hashId) {

  InDet::SCT_ClusterContainer::const_iterator it = m_SCT_ClusterContainer->indexFind(hashId);
  if(it!=m_SCT_ClusterContainer->end()) {
    InDet::SCT_ClusterCollection* pcoll = const_cast<InDet::SCT_ClusterCollection*>(&(**it));
    return pcoll;
  }
  Identifier id = m_sctId->wafer_id(hashId);
  InDet::SCT_ClusterCollection* pcoll = new InDet::SCT_ClusterCollection(hashId);
  pcoll->setIdentifier(id);
  StatusCode sc = m_SCT_ClusterContainer->addCollection(pcoll,hashId);
  if (sc.isFailure()){
    delete pcoll;
    return NULL;
  }
  return pcoll;
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
    ATH_MSG_DEBUG("   Handle: get EndEvent incident");

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
