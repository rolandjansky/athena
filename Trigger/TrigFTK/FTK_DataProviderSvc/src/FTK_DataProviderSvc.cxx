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
#include "RoiDescriptor/RoiDescriptor.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkFitterUtils/FitterTypes.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkParameters/TrackParameters.h"
//#include "TrigFTKSim/ftkdefs.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "StoreGate/DataHandle.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"

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
#include "FTK_RecToolInterfaces/IFTK_DuplicateTrackRemovalTool.h"
#include "FTK_RecToolInterfaces/IFTK_VertexFinderTool.h"
#include "FTK_DataProviderInterfaces/IFTK_UncertaintyTool.h"

#include "TrkToolInterfaces/ITrackParticleCreatorTool.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "InDetRecToolInterfaces/IVertexFinder.h"

#undef FTK_useViewVector
#ifdef  FTK_useViewVector
#include "xAODViews/TrackParticleViewVector.h"
#include "xAODViews/VertexViewVector.h"
#include "TrkViews/TrackViewVector.h"
#endif

#include "IRegionSelector/IRoiDescriptor.h"
#include "IRegionSelector/RoiUtil.h"

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
  m_RawVertexFinderTool("FTK_VertexFinderTool"),
  m_ROTcreator("Trk::IRIO_OnTrackCreator/FTK_ROTcreatorTool"),
  m_DuplicateTrackRemovalTool("FTK_DuplicateTrackRemovalTool"),
  m_trainingBeamspotX(0.),
  m_trainingBeamspotY(0.),
  m_trainingBeamspotZ(0.),
  m_trainingBeamspotTiltX(0.),
  m_trainingBeamspotTiltY(0.),
  m_remove_duplicates(false),
  m_ftk_tracks(nullptr),
  m_conv_tracks(nullptr),
  m_refit_tracks(nullptr),
  m_conv_tp(nullptr),
  m_refit_tp(nullptr),
  m_conv_tpAuxCont(nullptr),
  m_refit_tpAuxCont(nullptr),
  m_conv_vertex(nullptr),
  m_refit_vertex(nullptr),
  m_fast_vertex_raw(nullptr),
  m_fast_vertex_conv(nullptr),
  m_fast_vertex_refit(nullptr),
  m_got_conv_vertex(false),
  m_got_refit_vertex(false),
  m_got_fast_vertex_refit(false),
  m_got_fast_vertex_conv(false),
  m_got_fast_vertex_raw(false),
  m_correctPixelClusters(true),
  m_correctSCTClusters(true),
  m_broadPixelErrors(true),
  m_broadSCT_Errors(true),
  m_conv_track_map(0),
  m_refit_track_map(0),
  m_conv_tp_map(0),
  m_refit_tp_map(0),
  m_newEvent(true),
  m_gotRawTracks(false),
  m_trackCacheName("FTK_TrackCollection"),
  m_trackParticleCacheName("FTK_TrackParticleContainer"),
  m_vertexCacheName("FTK_VertexContainer"),
  m_doTruth(false),
  m_ftkPixelTruthName("FTK_PRD_MultiTruthPixel"),
  m_ftkSctTruthName("FTK_PRD_MultiTruthSCT"),
  m_mcTruthName("TruthEvent"), m_mcEventCollection(NULL),
  m_collectionsReady(false),
  m_PixelClusterContainerName("FTK_PixelClusterContainer"),
  m_SCT_ClusterContainerName("FTK_SCT_ClusterContainer"),
  m_pTscaleFactor(1.),
  m_rejectBadTracks(false),
  m_dPhiCut(0.4),
  m_dEtaCut(0.6),
  m_nErrors(0),
  m_reverseIBLlocx(false)
{
  m_pixelBarrelPhiOffsets.reserve(4);
  m_pixelBarrelEtaOffsets.reserve(4);
  m_pixelEndCapPhiOffsets.reserve(3);
  m_pixelEndCapEtaOffsets.reserve(3);
  for (int ib=0; ib<4; ib++) {
    m_pixelBarrelPhiOffsets.push_back(0.); 
    m_pixelBarrelEtaOffsets.push_back(0.);
  }
  for (int ie=0; ie<3; ie++) {
    m_pixelEndCapPhiOffsets.push_back(0.); 
    m_pixelEndCapEtaOffsets.push_back(0.);
  }
  m_nFailedSCTClusters.reserve(8);
  m_nMissingSCTClusters.reserve(8);
  m_nFailedPixelClusters.reserve(4);
  m_nMissingPixelClusters.reserve(4);
  for (int ie=0; ie<4; ie++){
    m_nFailedPixelClusters.push_back(0);
    m_nMissingPixelClusters.push_back(0);
  }
  for (int ie=0; ie<8; ie++){
    m_nFailedSCTClusters.push_back(0);
    m_nMissingSCTClusters.push_back(0);
  }

  declareProperty("TrackCollectionName",m_trackCacheName);
  declareProperty("TrackParticleContainerName",m_trackParticleCacheName);
  declareProperty("VertexContainerName",m_vertexCacheName);
  declareProperty("TrackFitter", m_trackFitter);
  declareProperty("UncertaintyTool",m_uncertaintyTool);
  declareProperty("TrackSummaryTool", m_trackSumTool);
  declareProperty("DuplicateTrackRemovalTool",m_DuplicateTrackRemovalTool);
  declareProperty("TrackParticleCreatorTool", m_particleCreatorTool);
  declareProperty("VertexFinderTool",m_VertexFinderTool);
  declareProperty("ROTcreatorTool",m_ROTcreator);
  declareProperty("RawVertexFinderTool",m_RawVertexFinderTool);
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
  declareProperty("rejectBadTracks", m_rejectBadTracks);
  declareProperty("BadTrackdPhiCut", m_dPhiCut);
  declareProperty("BadTrackdEtaCut", m_dEtaCut);
  declareProperty("PixelBarrelPhiOffsets", m_pixelBarrelPhiOffsets," Pixel Barrel Phi Offsets in mm" );
  declareProperty("PixelBarrelEtaOffsets", m_pixelBarrelEtaOffsets," Pixel Barrel Eta Offsets in mm" );
  declareProperty("PixelEndCapPhiOffsets", m_pixelEndCapPhiOffsets," Pixel EndCap Phi Offsets in mm" );
  declareProperty("PixelEndCapEtaOffsets", m_pixelEndCapEtaOffsets," Pixel EndCap Eta Offsets in mm" );
  declareProperty("CorrectPixelClusters",m_correctPixelClusters);
  declareProperty("CorrectSCTClusters",m_correctSCTClusters);
  declareProperty("setBroadPixelClusterOnTrackErrors",m_broadPixelErrors);
  declareProperty("setBroadSCT_ClusterOnTrackErrors",m_broadSCT_Errors);
  declareProperty("RemoveDuplicates",m_remove_duplicates);
  declareProperty("ReverseIBLlocX",m_reverseIBLlocx, "reverse the direction of IBL locX from FTK");


}

FTK_DataProviderSvc::~FTK_DataProviderSvc(){
}


std::vector<unsigned int> FTK_DataProviderSvc::nMissingSCTClusters() {
  return m_nMissingSCTClusters;
}

std::vector<unsigned int> FTK_DataProviderSvc::nMissingPixelClusters() {
  return m_nMissingPixelClusters;
}

std::vector<unsigned int> FTK_DataProviderSvc::nFailedSCTClusters() {
  return m_nFailedSCTClusters;
}

std::vector<unsigned int> FTK_DataProviderSvc::nFailedPixelClusters() {
  return m_nFailedPixelClusters;
}


std::string FTK_DataProviderSvc::getTrackParticleCacheName(const bool withRefit) {
  return (withRefit? m_trackParticleCacheName+"Refit": m_trackParticleCacheName);
}
std::string FTK_DataProviderSvc::getTrackCacheName(const bool withRefit){
  return (withRefit? m_trackCacheName+"Refit": m_trackCacheName);
}
std::string FTK_DataProviderSvc::getVertexCacheName(const bool withRefit){
  return (withRefit? m_vertexCacheName+"Refit": m_vertexCacheName);
}
std::string FTK_DataProviderSvc::getFastVertexCacheName(const bool withRefit) {
  return (withRefit? m_vertexCacheName+"FastRefit": m_vertexCacheName+"Fast");
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
  ATH_MSG_INFO( " getting DuplicateTrackRemovalTool tool with name " << m_DuplicateTrackRemovalTool.name());
  ATH_CHECK(m_DuplicateTrackRemovalTool.retrieve());
  ATH_MSG_INFO( " getting FTK_RawTrackVertexFinderTool tool with name " << m_RawVertexFinderTool.name());
  ATH_CHECK(m_RawVertexFinderTool.retrieve());
  ATH_MSG_INFO( " getting ROTcreator tool with name " << m_ROTcreator.name());
  ATH_CHECK(m_ROTcreator.retrieve());

  // Register incident handler
  ServiceHandle<IIncidentSvc> iincSvc( "IncidentSvc", name());
  ATH_CHECK(iincSvc.retrieve());
  iincSvc->addListener( this, IncidentType::EndEvent);

  ATH_MSG_INFO( "TrackCollection names: " << m_trackCacheName<< ", " << m_trackCacheName<<"Refit");
  ATH_MSG_INFO( "TrackParticleContainer names: " <<  m_trackParticleCacheName << ", " << m_trackParticleCacheName <<"Refit");
  ATH_MSG_INFO( "VertexContainer names: " << m_vertexCacheName<< ", " <<m_vertexCacheName<<"Refit");
  ATH_MSG_INFO( "PixelClusterContainer name : " << m_PixelClusterContainerName);
  ATH_MSG_INFO( "SCT_ClusterContainer name : " << m_SCT_ClusterContainerName);
  ATH_MSG_INFO( "PRD Truth SCT name: " << m_ftkSctTruthName);
  ATH_MSG_INFO( "PRD Truth Pixel name : " << m_ftkPixelTruthName);

  ATH_MSG_INFO( "Correcting for FTK training beamspot at x " <<  m_trainingBeamspotX <<" y " << 	m_trainingBeamspotY
      << " z " <<  m_trainingBeamspotZ << " TiltX " << m_trainingBeamspotTiltX << "TiltY " << m_trainingBeamspotTiltY );
  ATH_MSG_INFO( " Pixel Barrel Phi Offsets (pixels): " << m_pixelBarrelPhiOffsets);
  ATH_MSG_INFO( " Pixel Barrel Eta Offsets (pixels): " << m_pixelBarrelEtaOffsets);
  ATH_MSG_INFO( " Pixel EndCap Phi Offsets (pixels): " << m_pixelEndCapPhiOffsets);
  ATH_MSG_INFO( " Pixel EndCap Eta Offsets (pixels): " << m_pixelEndCapEtaOffsets);
  if  (m_reverseIBLlocx) ATH_MSG_INFO( "Reversing the direction of IBL LocX");

  if (m_correctPixelClusters) {
    ATH_MSG_INFO( " applying all corrections (lorentz, angle,  sag) to Pixel Clusters on converted tracks using RotCreatorTool");
  } else {
    ATH_MSG_INFO( " only applying Lorentz all correction to Pixel Clusters on converted tracks");
    if (m_broadPixelErrors) ATH_MSG_INFO( "Creating PixelClusterOnTrack with broad errors");
  }
  if (m_correctSCTClusters) {
    ATH_MSG_INFO( " applying all corrections to SCT Clusters on converted tracks using RotCreatorTool");
  } else { 
    ATH_MSG_INFO( " only applying Lorentz correction to SCT Clusters on converted tracks");
    if (m_broadSCT_Errors) ATH_MSG_INFO( "Creating SCT_ClusterOnTrack with broad errors");
  } 


  return StatusCode::SUCCESS;

}

StatusCode FTK_DataProviderSvc::finalize() {
  return StatusCode::SUCCESS;
}


unsigned int FTK_DataProviderSvc::nTrackParticleErrors(const bool withRefit) {

  unsigned int nErrors=0;

  if (withRefit) {
    for (auto& it : m_refit_tp_map) 
      if (it==-2) nErrors++;
  } else {
    for (auto& it : m_conv_tp_map) 
      if (it==-2) nErrors++;
  }    
  return nErrors;

}


unsigned int FTK_DataProviderSvc::nRawTracks() {
  if (!m_gotRawTracks) getFTK_RawTracksFromSG();
  return m_ftk_tracks->size();
}


unsigned int FTK_DataProviderSvc::nTracks(const bool withRefit) {

  unsigned int nTrk=0;

  if (withRefit) {
    for (auto& it : m_refit_track_map) 
      if (it>=0) nTrk++;
  } else {
    for (auto& it : m_conv_track_map) 
      if (it>=0) nTrk++;
  }    
  return nTrk;

}

unsigned int FTK_DataProviderSvc::nTrackParticles(const bool withRefit) {

  unsigned int nTrk=0;

  if (withRefit) {
    for (auto& it : m_refit_tp_map) 
      if (it>=0) nTrk++;
  } else {
    for (auto& it : m_conv_tp_map) 
      if (it>=0) nTrk++;
  }    
  return nTrk;

}

unsigned int FTK_DataProviderSvc::nTrackErrors(const bool withRefit) {

  unsigned int nErrors=0;

  if (withRefit) {
    for (auto& it : m_refit_track_map) 
      if (it==-2) nErrors++;
  } else {
    for (auto& it : m_conv_track_map) 
      if (it==-2) nErrors++;
  }    
  return nErrors;

}


xAOD::TrackParticleContainer* FTK_DataProviderSvc::getTrackParticlesInRoi(const IRoiDescriptor& roi, const bool withRefit, unsigned int& nErrors){
  m_nErrors=0;
  xAOD::TrackParticleContainer* tracks = this->getTrackParticlesInRoi(roi, withRefit);
  nErrors = m_nErrors;
  return tracks;
}

xAOD::TrackParticleContainer* FTK_DataProviderSvc::getTrackParticlesInRoi(const IRoiDescriptor& roi, const bool withRefit){

  ATH_MSG_DEBUG("FTK_DataProviderSvc::getTrackParticlesInRoi called with Refit " << withRefit);

#ifdef  FTK_useViewVector
  xAOD::TrackParticleContainer *tpcont = new xAOD::TrackParticleViewVector();
#else
  xAOD::TrackParticleContainer *tpcont = new xAOD::TrackParticleContainer(SG::VIEW_ELEMENTS);
#endif

  if (initTrackParticleCache(withRefit).isFailure()) {
    return tpcont;
  }

  if (withRefit) {
    for (unsigned int ftk_track_index = 0; ftk_track_index != m_ftk_tracks->size(); ++ftk_track_index){

      // Get FTK track
      const FTK_RawTrack* ftk_track= m_ftk_tracks->at(ftk_track_index);
      // check if track is inside RoI
      if (roi.isFullscan() || (RoiUtil::containsPhi(roi,ftk_track->getPhi()) && RoiUtil::contains(roi, ftk_track->getZ0(), ftk_track->getCotTh()))) {
      //if (roi.isFullscan() || (roi.containsPhi(ftk_track->getPhi()) && roi.contains(ftk_track->getZ0(), ftk_track->getCotTh()))) {//Old interface

        if( m_refit_tp_map[ftk_track_index]>-1) {
          // tp is in cache
          tpcont->push_back(m_refit_tp->at((unsigned int) m_refit_tp_map[ftk_track_index]));
        } else {
          // tp not in cache, need to create it
          // first get the Trk::Track
          Trk::Track *track = getCachedTrack(ftk_track_index, withRefit);
          if (track !=nullptr) {

            // Create ElementLink for Trk::Track from pointer to TrackCollection and index of track within the collection
            ATH_MSG_VERBOSE("creating ElementLink to m_refit_track_map["<<ftk_track_index<<"]= "<<m_refit_track_map[ftk_track_index]);
            const ElementLink<TrackCollection> trackLink(*m_refit_tracks, (unsigned int) m_refit_track_map[ftk_track_index]);

            if (trackLink.isValid()) {
              // create the tp
              ATH_MSG_VERBOSE("creating TrackParticle");
              xAOD::TrackParticle* tp = m_particleCreatorTool->createParticle( trackLink, m_refit_tp);
              if (tp !=nullptr) {
                m_refit_tp_map[ftk_track_index] = (int) m_refit_tp->size()-1;
                tpcont->push_back(tp);
              }
            } else { //trackLink not valid
              ATH_MSG_ERROR ("invalid ElementLink to m_refit_track_map["<<ftk_track_index<<"] = " << m_refit_track_map[ftk_track_index]);
              m_refit_tp_map[ftk_track_index]=-2;
	      m_nErrors++;
            }
          } else { // track==nullptr
            ATH_MSG_VERBOSE("Setting m_refit_tp_map["<<  ftk_track_index <<"]=-2");
            m_refit_tp_map[ftk_track_index]=-2;
	    m_nErrors++;
          }
        }
      }
    }
    ATH_MSG_DEBUG("getTrackParticlesIn Roi: Returning TrackParticleContainer Size = " << tpcont->size());
  } else {
    for (unsigned int ftk_track_index = 0; ftk_track_index != m_ftk_tracks->size(); ++ftk_track_index){

      // Get FTK track
      const FTK_RawTrack* ftk_track= m_ftk_tracks->at(ftk_track_index);

      if (RoiUtil::containsPhi(roi,ftk_track->getPhi()) && RoiUtil::contains(roi,ftk_track->getZ0(), ftk_track->getCotTh())) { 
      //if (roi.containsPhi(ftk_track->getPhi()) && roi.contains(ftk_track->getZ0(), ftk_track->getCotTh())) {
        // check track is inside RoI

        if( m_conv_tp_map[ftk_track_index]>-1) {
          // tp is already in the cache
          tpcont->push_back(m_conv_tp->at((unsigned int) m_conv_tp_map[ftk_track_index]));
        } else {
          // tp not in the cache so create it

          // first get the Trk::Track
          Trk::Track *track = getCachedTrack(ftk_track_index, withRefit);
          if (track != nullptr) {
            // Create ElementLink from pointer to container and index of track within the container
            ATH_MSG_VERBOSE("creating ElementLink to m_conv_track_map["<<ftk_track_index<<"] = " << m_conv_track_map[ftk_track_index]);
            const ElementLink<TrackCollection> trackLink(*m_conv_tracks, (unsigned int) m_conv_track_map[ftk_track_index]);
            if (trackLink.isValid()) {
              ATH_MSG_VERBOSE("creating TrackParticle");
              xAOD::TrackParticle* tp = m_particleCreatorTool->createParticle( trackLink, m_conv_tp);
              if (tp !=nullptr) {
                m_conv_tp_map[ftk_track_index] = (int) m_conv_tp->size()-1;
                tpcont->push_back(tp);
              } else {
                ATH_MSG_ERROR ("invalid ElementLink to m_refit_conv_map["<<ftk_track_index<<"] = " <<m_conv_track_map[ftk_track_index]);
                m_refit_tp_map[ftk_track_index]=-2;
		m_nErrors++;
              }
            }
          } else { // track==nullptr
            ATH_MSG_VERBOSE("Setting m_conv_tp_map["<<  ftk_track_index <<"]=-2");
            m_conv_tp_map[ftk_track_index] = -2;
	    m_nErrors++;
          }
        }
      }
    }
    ATH_MSG_DEBUG("getTrackParticlesinRoI: Returning refitted TrackParticleContainer Size = " << tpcont->size());
  }

  return tpcont;
}



StatusCode FTK_DataProviderSvc::fillTrackParticleCache(const bool withRefit){

  ATH_MSG_DEBUG("FTK_DataProviderSvc::fillTrackParticleCache called with Refit " << withRefit);

  if (initTrackParticleCache(withRefit).isFailure()) {
    return StatusCode::FAILURE;
  }

  if (withRefit) { // create TrackParticle from Refitted track
    for (unsigned int ftk_track_index = 0; ftk_track_index != m_ftk_tracks->size(); ++ftk_track_index){

      if( m_refit_tp_map[ftk_track_index]==-1) {
        // tp not in cache, need to create it

        // first get the track
        ATH_MSG_VERBOSE("fillTrackParticleCache: get cached track " << ftk_track_index);
        Trk::Track *track = getCachedTrack(ftk_track_index, withRefit);
        if (track != nullptr) {

          // Create ElementiLink for Trk::Track from pointer to TrackCollection and index of track within the collection
          ATH_MSG_VERBOSE("creating ElementLink to m_refit_track_map["<<ftk_track_index<<"] = "<< m_refit_track_map[ftk_track_index]);
          const ElementLink<TrackCollection> trackLink(*m_refit_tracks, (unsigned int) m_refit_track_map[ftk_track_index]);

          if (trackLink.isValid()) {
            xAOD::TrackParticle* tp = m_particleCreatorTool->createParticle( trackLink, m_refit_tp);
            if (tp !=nullptr) {
              m_refit_tp_map[ftk_track_index] = (int) m_refit_tp->size()-1;
              ATH_MSG_VERBOSE("TrackParticle from refitted track added to cache at index " << m_refit_tp_map[ftk_track_index]
                  << "  created from FTK track at index " << ftk_track_index);
            } else {
              ATH_MSG_ERROR ("invalid ElementLink to m_refit_refit_map["<<ftk_track_index<<"] = "<< m_refit_track_map[ftk_track_index]);
              m_refit_tp_map[ftk_track_index]=-2;
            }

          }
        } else { // track==nullptr
          ATH_MSG_VERBOSE("Setting m_refit_tp_map["<<  ftk_track_index <<"]=-2");
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
          ATH_MSG_VERBOSE("creating ElementLink to m_conv_track_map["<<ftk_track_index<<"] = "<< m_conv_track_map[ftk_track_index]);

          // Create ElementLink for Trk::Track from pointer to TrackCollection and index of track within the collection
          const ElementLink<TrackCollection> trackLink(*m_conv_tracks, (unsigned int) m_conv_track_map[ftk_track_index]);

          if (trackLink.isValid()) {
            ATH_MSG_VERBOSE("creating TrackParticle");
            xAOD::TrackParticle* tp = m_particleCreatorTool->createParticle( trackLink, m_conv_tp);
            if (tp !=nullptr) {
              m_conv_tp_map[ftk_track_index] = (int) m_conv_tp->size()-1;
              ATH_MSG_VERBOSE("TrackParticle from converted track added to cache at index " << m_conv_tp_map[ftk_track_index]
                  << "  created from FTK track at index " << ftk_track_index);
            }
          } else {
            ATH_MSG_ERROR ("invalid ElementLink to m_conv_track_map["<<ftk_track_index<<"] = "<< m_conv_track_map[ftk_track_index]);
            m_conv_tp_map[ftk_track_index] = -2;
          }
        } else { // track== nullptr
          ATH_MSG_VERBOSE("Setting m_conv_tp_map["<<  ftk_track_index <<"]=-2");
          m_conv_tp_map[ftk_track_index] = -2;
        }
      }

    }

    ATH_MSG_DEBUG("fillTrackParticleCache: Returning TrackParticleContainer Size = " << m_conv_tp->size());
  }

  return  StatusCode::SUCCESS;
}


xAOD::TrackParticleContainer* FTK_DataProviderSvc::getTrackParticles(const bool withRefit, unsigned int& nErrors){

  // Return TrackParticleCollection plus number of missing tracks du to errors 

  m_nErrors=0;
  xAOD::TrackParticleContainer* tracks=this->getTrackParticles(withRefit);
  nErrors=m_nErrors;
  return tracks;
}

xAOD::TrackParticleContainer* FTK_DataProviderSvc::getTrackParticles(const bool withRefit){

#ifdef  FTK_useViewVector
  xAOD::TrackParticleContainer *tpcont = new xAOD::TrackParticleViewVector();
#else
  xAOD::TrackParticleContainer *tpcont = new xAOD::TrackParticleContainer(SG::VIEW_ELEMENTS);
#endif

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


  /// If we're at the start of a new event, get the tracks from StoreGate
  getFTK_RawTracksFromSG();

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
        ATH_MSG_VERBOSE("fillTrackCache: refit failed - refitted track coresponding to FTK track with index " << ftk_track_index << " has NOT added to  output collection");
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


  return StatusCode::SUCCESS;


}

xAOD::VertexContainer* FTK_DataProviderSvc::getFastVertices(const ftk::FTK_TrackType trackType/* default  ftk::RawTrack*/) {

#ifdef  FTK_useViewVector
   xAOD::VertexContainer* userVertex = new xAOD::VertexViewVector();
#else
   xAOD::VertexContainer* userVertex = new xAOD::VertexContainer(SG::VIEW_ELEMENTS);
#endif
   if (trackType == ftk::RawTrack) {
     ATH_MSG_DEBUG( "getFastVertices: filling xAOD::VertexContainer from RAW tracks ");
     // get vertices from Raw FTK tracks	
     if (!m_got_fast_vertex_raw) {
       getFTK_RawTracksFromSG();
       if (!m_gotRawTracks) return userVertex;
       std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> vertices = m_RawVertexFinderTool->findVertex(m_ftk_tracks);
       std::string cacheName=m_vertexCacheName+"Raw";
       StatusCode sc = m_storeGate->record(vertices.first, cacheName);
       if (sc.isFailure()) {
	 ATH_MSG_DEBUG( "getFastVertices: Failed to record xAOD::VertexCollection " << cacheName );
	 delete(vertices.first);
	 delete(vertices.second);
	 return userVertex;
       }
       sc = m_storeGate->record(vertices.second, cacheName+"Aux.");
       if (sc.isFailure()) {
	 ATH_MSG_DEBUG( "fillVertexContainerCache: Failed to record VertexAuxCollection " << cacheName );
	 delete(vertices.second);
	 return userVertex;
       }

       m_fast_vertex_raw = vertices.first; 
       m_got_fast_vertex_raw = true;
     }
	
     for (auto pv = m_fast_vertex_raw->begin(); pv != m_fast_vertex_raw->end(); ++pv) {
       userVertex->push_back(*pv);
     }
     ATH_MSG_DEBUG( "getFastVertices: returning " << m_fast_vertex_raw->size() <<  " vertices from RAW tracks ");
     return userVertex;
     
   } // if (trackType == ftk::RawTrack) 
  
   bool userefit=false;
   if (trackType == ftk::RefittedTrack) userefit=true;
   if (fillTrackCache(userefit).isFailure()) return userVertex;

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
	  ATH_MSG_DEBUG( "fillVertexContainerCache: Failed to record VertexAuxCollection " << cacheName );
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
	   ATH_MSG_DEBUG( "fillVertexContainerCache: Failed to record VertexAuxCollection " << cacheName );
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

bool FTK_DataProviderSvc::fillVertexContainerCache(bool withRefit, xAOD::TrackParticleContainer* tps) {

  bool gotVertices = false;
  if (tps->size() > 0) {
    ATH_MSG_DEBUG( "fillVertexContainerCache: finding vertices from " << tps->size() << " TrackParticles ");
    std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> vertices = m_VertexFinderTool->findVertex(tps);
    if (vertices.first == nullptr) return gotVertices;

    std::string cacheName= m_vertexCacheName;
    if (withRefit) cacheName+="Refit";

    StatusCode sc = m_storeGate->record(vertices.first, cacheName);
    if (sc.isFailure()) {
      ATH_MSG_DEBUG( "fillVertexContainerCache: Failed to record VertexCollection " << cacheName );
      delete(vertices.first);
      delete(vertices.second);
      return gotVertices;
    }
    sc = m_storeGate->record(vertices.second, cacheName+"Aux.");
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

#ifdef  FTK_useViewVector
   xAOD::VertexContainer* userVertex = new xAOD::VertexViewVector();
#else
   xAOD::VertexContainer* userVertex = new xAOD::VertexContainer(SG::VIEW_ELEMENTS);
#endif

  if (fillTrackParticleCache(withRefit).isFailure()) {

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

StatusCode FTK_DataProviderSvc::getVertexContainer(xAOD::VertexContainer* userVertex, const bool withRefit){

  if (fillTrackParticleCache(withRefit).isFailure()) {
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



TrackCollection* FTK_DataProviderSvc::getTracks(const bool withRefit, unsigned int& nErrors){
  m_nErrors=0;
  TrackCollection* tracks = this->getTracks(withRefit);
  nErrors=m_nErrors;
  return tracks;
}

TrackCollection* FTK_DataProviderSvc::getTracks(const bool withRefit){

#ifdef  FTK_useViewVector
  TrackCollection *userTracks=new TrackViewVector();
#else
  TrackCollection *userTracks=new TrackCollection(SG::VIEW_ELEMENTS);
#endif


  /// If we're at the start of a new event, get the tracks from StoreGate
  getFTK_RawTracksFromSG();
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

TrackCollection* FTK_DataProviderSvc::getTracksInRoi(const IRoiDescriptor& roi, const bool withRefit, unsigned int& nErrors){
  m_nErrors=0;
  TrackCollection* tracks = this->getTracksInRoi(roi,withRefit);
  nErrors=m_nErrors;
  return tracks;
}

TrackCollection* FTK_DataProviderSvc::getTracksInRoi(const IRoiDescriptor& roi, const bool withRefit){

#ifdef  FTK_useViewVector
  TrackCollection *userTracks=new TrackViewVector();
#else
  TrackCollection *userTracks=new TrackCollection(SG::VIEW_ELEMENTS);
#endif


  /// If we're at the start of a new event, get the tracks from StoreGate
  getFTK_RawTracksFromSG();
  if (initTrackCache(withRefit).isFailure()) {
    return userTracks;
  }

  /// Do conversion or return already converted tracks
  ATH_MSG_DEBUG( "getTracksInRoi: Raw FTK Container Size = " << m_ftk_tracks->size());
  ATH_MSG_VERBOSE( "getTracksInRoI: Converting Tracks");
  for (unsigned int ftk_track_index = 0; ftk_track_index != m_ftk_tracks->size(); ++ftk_track_index){

    /// Check if the track is inside the ROI ///

    const FTK_RawTrack* ftk_track= m_ftk_tracks->at(ftk_track_index);
    if (roi.isFullscan() || (RoiUtil::containsPhi(roi,ftk_track->getPhi()) && RoiUtil::contains(roi,ftk_track->getZ0(), ftk_track->getCotTh()))) {
    //if (roi.isFullscan() || (roi.containsPhi(ftk_track->getPhi()) && roi.contains(ftk_track->getZ0(), ftk_track->getCotTh()))) {
      Trk::Track* track = this->getCachedTrack(ftk_track_index, withRefit);
      if (track != nullptr) {
	userTracks->push_back(track);
      } else {
	m_nErrors++;
      } 

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


  ATH_MSG_VERBOSE( "FTK_DataProviderSvc::getCachedTrack called for FTK track " << ftk_track_index <<" Refit " << withRefit);
  // First check if the converted track is in teh cache and create it if it isn't

  if( m_conv_track_map[ftk_track_index]>-1) { /// Track already converted ///
    ATH_MSG_VERBOSE( "m_conv_track_map[" <<ftk_track_index<<"]= "<<m_conv_track_map[ftk_track_index]);
    track = m_conv_tracks->at((unsigned int) m_conv_track_map[ftk_track_index]);
    if (track == nullptr) {
      ATH_MSG_VERBOSE( "getCachedTrack:  failed to retrieve converted track with index " << m_conv_track_map[ftk_track_index] << " corresponding to FTK track " << ftk_track_index);
      return nullptr;
    } else {
      ATH_MSG_VERBOSE( "getCachedTrack:  retrieved converted track with index " << m_conv_track_map[ftk_track_index] << " corresponding to FTK track " << ftk_track_index);
    }

  } else { /// This track hasn't been converted yet, do it now ///
    if( m_conv_track_map[ftk_track_index]!=-2) {
      ATH_MSG_VERBOSE( "getCachedTrack: FTK Track " << ftk_track_index << " not yet converted, beginning conversion");
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

    } else {
      ATH_MSG_VERBOSE( "getCachedTrack: failed to create converted track because m_conv_track_map["<<ftk_track_index
          << "] = " << m_conv_track_map[ftk_track_index]);
    }
  }

  if (track== nullptr || !withRefit) {
    if (track!=nullptr) {
      ATH_MSG_VERBOSE( "getCachedTrack: returning converted track");
    } else {
      ATH_MSG_VERBOSE( "getCachedTrack: returning nullptr");
    }
    return track;
  }

  // Refit the track

  if  (m_refit_track_map[ftk_track_index] > -1) {
    // track has already been refitted //
    ATH_MSG_VERBOSE( "getCachedTrack: track has already been refitted and is in refitted TrackCollection with index m_refit_track_map["<<ftk_track_index<<"] = " << m_refit_track_map[ftk_track_index]);
    refitTrack = m_refit_tracks->at((unsigned int) m_refit_track_map[ftk_track_index]);
    if (refitTrack == nullptr) {
      ATH_MSG_VERBOSE( "getCachedTrack:  failed to retrieve refitted track with index " << m_refit_track_map[ftk_track_index] << " corresponding to FTK track " << ftk_track_index);
      m_refit_track_map[ftk_track_index] = -2;
    }

  } else if (m_conv_track_map[ftk_track_index] != -2 && m_refit_track_map[ftk_track_index]!=-2){
    // Need to do refit//
    ATH_MSG_VERBOSE( "getCachedTrack: Beginning Track Refit for FTK track " << ftk_track_index);
    Trk::Track *newRefitTrack = m_trackFitter->fit(*track, false, Trk::pion);
    if(newRefitTrack == nullptr ) {
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
  } else {
    ATH_MSG_VERBOSE( "getCachedTrack: can't refit track because m_refit_track_map["<<ftk_track_index<<"] = " << m_refit_track_map[ftk_track_index]);
  }
  if (refitTrack!=nullptr) {
    ATH_MSG_VERBOSE( "getCachedTrack: returning refitted track");
  } else {
    ATH_MSG_VERBOSE( "getCachedTrack: returning nullptr");
  }
  return refitTrack;
}


void FTK_DataProviderSvc::getFTK_RawTracksFromSG(){
  /// get the FTK Track pointers from StoreGate ///

  if (!m_newEvent) return;
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
  } else {    

    StatusCode sc = StatusCode::SUCCESS;
    if (m_remove_duplicates){//get all tracks, and then call duplicate removal tool
      const FTK_RawTrackContainer* temporaryTracks=nullptr;
      sc = m_storeGate->retrieve(temporaryTracks, m_RDO_key);
      if (!sc.isFailure()) {
	ATH_MSG_DEBUG( "getFTK_RawTracksFromSG:  Got " << temporaryTracks->size() << " raw FTK tracks (RDO) from  StoreGate before removeDuplicates");
	m_ftk_tracks = m_DuplicateTrackRemovalTool->removeDuplicates(temporaryTracks);
      }
    }
    else{//the original way
      sc = m_storeGate->retrieve(m_ftk_tracks, m_RDO_key);
    }

    if (sc.isFailure()) {
      ATH_MSG_VERBOSE( "getFTK_RawTracksFromSG: Failed to get FTK Tracks Container");
    } else {
      ATH_MSG_DEBUG( "getFTK_RawTracksFromSG:  Got " << m_ftk_tracks->size() << " raw FTK tracks (RDO) from  StoreGate ");
      if (m_ftk_tracks->size()==0){
	ATH_MSG_VERBOSE( "no FTK Tracks in the event");
      } else {
	m_gotRawTracks = true;
      }
    }      
  }

  // Creating collection for pixel clusters
  m_PixelClusterContainer = new InDet::PixelClusterContainer(m_pixelId->wafer_hash_max());
  m_PixelClusterContainer->addRef();
  StatusCode sc = m_storeGate->record(m_PixelClusterContainer,m_PixelClusterContainerName);
  if (sc.isFailure()) {
    ATH_MSG_DEBUG("Error registering the FTK pixel container in the SG");
    return;
  }
  // Generic format link for the pixel clusters
  const InDet::SiClusterContainer *symSiContainerPxl(0x0);
  sc = m_storeGate->symLink(m_PixelClusterContainer,symSiContainerPxl);
  if (sc.isFailure()) {
    ATH_MSG_DEBUG("Error creating the sym-link to the Pixel clusters");
    return;
  }
  // Creating collection for the SCT clusters
  m_SCT_ClusterContainer = new InDet::SCT_ClusterContainer(m_sctId->wafer_hash_max());
  m_SCT_ClusterContainer->addRef();
  sc = m_storeGate->record(m_SCT_ClusterContainer,m_SCT_ClusterContainerName);
  if (sc.isFailure()) {
    ATH_MSG_DEBUG("Error registering the FTK SCT container in the SG");
    return;
  }
  // Generic format link for the pixel clusters
  const InDet::SiClusterContainer *symSiContainerSCT(0x0);
  sc = m_storeGate->symLink(m_SCT_ClusterContainer,symSiContainerSCT);
  if (sc.isFailure()) {
    ATH_MSG_DEBUG("Error creating the sym-link to the SCT clusters");
    return;
  }


  return;
}

StatusCode FTK_DataProviderSvc::initTrackCache(bool withRefit) {
  
  
  if (!m_storeGate->contains<TrackCollection>(m_trackCacheName)) {
    
    m_conv_tracks = new TrackCollection;
    StatusCode sc = m_storeGate->record(m_conv_tracks, m_trackCacheName);
    if (sc.isFailure()) {
      ATH_MSG_DEBUG( "initTrackCache: Failed to record TrackCollection " <<  m_trackCacheName << " No FTK tracks returned");
      delete(m_conv_tracks);
      return sc;
    }
    
    if (m_gotRawTracks) {
      if (m_conv_track_map.size()==0) {
	m_conv_track_map.reserve(m_ftk_tracks->size());
	for (unsigned int i = 0; i!=m_ftk_tracks->size(); i++) m_conv_track_map.push_back(-1);
	
	ATH_MSG_VERBOSE( "initTrackCache: converted track map initialised with size " << m_ftk_tracks->size());
      }
      if (m_refit_track_map.size()==0) {
	m_refit_track_map.reserve(m_ftk_tracks->size());
	for (unsigned int i = 0; i!=m_ftk_tracks->size(); i++) m_refit_track_map.push_back(-1);
	ATH_MSG_VERBOSE( "initTrackCache: refitted track map initialised with size " << m_ftk_tracks->size());
      }
    }
  }
  if (withRefit) {
    if (!m_storeGate->contains<TrackCollection>(m_trackCacheName+"Refit")) {
      m_refit_tracks = new TrackCollection;
      StatusCode sc = m_storeGate->record(m_refit_tracks, m_trackCacheName+"Refit");
      if (sc.isFailure()) {
	ATH_MSG_DEBUG( "initTrackCache: Failed to record TrackCollection " <<  m_trackCacheName << "Refit - No FTK tracks returned");
	delete(m_refit_tracks);
	return sc;
      }
      ATH_MSG_VERBOSE( "initTrackCache: refitted track cache initialised ");
    }
  }

return  (m_gotRawTracks? StatusCode::SUCCESS : StatusCode::FAILURE) ;
}

StatusCode FTK_DataProviderSvc::initTrackParticleCache(bool withRefit) {


  // Must always create a container, even if it is empty

  bool gotTracks=false;
  getFTK_RawTracksFromSG();
  if (m_gotRawTracks) {
    if (!initTrackCache(withRefit).isFailure()) {  
      gotTracks=true; 
    } 
  }

  if (withRefit) {

    if (!m_storeGate->contains<xAOD::TrackParticleContainer>(m_trackParticleCacheName+"Refit")) {

      m_refit_tp=  new xAOD::TrackParticleContainer();

      ATH_MSG_VERBOSE( "initTrackParticleCache: initialising auxstore for refitted TrackParticles");
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
      if (gotTracks) {
	m_refit_tp_map.reserve(m_ftk_tracks->size());
	for (unsigned int i = 0; i!=m_ftk_tracks->size(); i++) m_refit_tp_map.push_back(-1);
	ATH_MSG_VERBOSE( "initTrackParticleCache:  map initialized for refitted TracksParticles with size " << m_ftk_tracks->size());
      }
    }
  } else {

    if (!m_storeGate->contains<xAOD::TrackParticleContainer>(m_trackParticleCacheName)) {
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
      if (gotTracks) {
	m_conv_tp_map.reserve(m_ftk_tracks->size());
	for (unsigned int i = 0; i!=m_ftk_tracks->size(); i++) m_conv_tp_map.push_back(-1);
	ATH_MSG_VERBOSE( "initTrackParticleCache: map initialized for converted TracksParticles, map size "<<m_ftk_tracks->size());
      }
    }
  }
  return (gotTracks?StatusCode::SUCCESS:StatusCode::FAILURE);
}



Trk::Track* FTK_DataProviderSvc::ConvertTrack(const unsigned int iTrack){
  //std::unique_ptr< Trk::Track > FTK_DataProviderSvc::ConvertTrack(int iTrack){

  const FTK_RawTrack &track = *m_ftk_tracks->at(iTrack);

  ATH_MSG_VERBOSE( "ConvertTrack: Beginning Track conversion");

  double trkTheta = std::atan2(1.0,track.getCotTh());
  double trkEta = -std::log(std::tan(trkTheta/2));

  ATH_MSG_VERBOSE( "ConvertTrack: track theta: " << trkTheta << " eta: " << trkEta);

  DataVector<const Trk::TrackStateOnSurface>* trkTSoSVec = new DataVector<const Trk::TrackStateOnSurface>;





  // Create Pixel Clusters
  bool hasIBL=false;

  //float pixelPhi=0;
  //float pixelEta=0;

  // Find if the track includes IBL - needed for the error calculaton 
  for( unsigned int cluster_number = 0; cluster_number < track.getPixelClusters().size(); ++cluster_number){
    if ( !track.isMissingPixelLayer(cluster_number)) {
      Identifier wafer_id = m_pixelId->wafer_id(Identifier(track.getPixelClusters()[cluster_number].getModuleID()));
      if (m_pixelId->barrel_ec(wafer_id)==0 && m_pixelId->layer_disk(wafer_id)==0) {
	hasIBL=true;
	break;
      }
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


  ATH_MSG_VERBOSE( " trkTrackCovm: (d0,z0,phi,theta,q/p)= " <<  (*trkTrackCovm)(0,0) << ", "<< (*trkTrackCovm)(1,1)<< ", " <<
      (*trkTrackCovm)(2,2)<< ", "<<(*trkTrackCovm)(3,3)<< ", "<<  (*trkTrackCovm)(4,4));

  //
  // Construct the measured Perigee Parameters
  //

  double trkQOverP = sin(trkTheta)*track.getInvPt()/m_pTscaleFactor;
  Trk::PerigeeSurface periSurf;

  double dx = m_trainingBeamspotX + m_trainingBeamspotTiltX*track.getZ0();//correction for tilt
  double dy = m_trainingBeamspotY + m_trainingBeamspotTiltY*track.getZ0();//correction for tilt

  double d0 = track.getD0()-dx*sin( track.getPhi())+dy*cos( track.getPhi());
  double z0 = track.getZ0() - ((cos( track.getPhi()) *dx + sin( track.getPhi())*dy)/tan(trkTheta));

  const Trk::TrackParameters* trkPerigee = new Trk::Perigee( d0,
      z0,
      track.getPhi(),
      trkTheta,
      trkQOverP,
      periSurf,
      trkTrackCovm);

  ATH_MSG_VERBOSE( "   ConvertTrack: Track perigee created  d0 " <<  d0 << " z0 " <<
      z0<< " phi0 " << track.getPhi() << " theta " << trkTheta << " q/P " << trkQOverP);

  //
  // Build the TrackInfo
  //
  Trk::TrackInfo trkTrackInfo = Trk::TrackInfo(Trk::TrackInfo::Unknown, Trk::pion);

  //
  // Build the FitQuality
  //
  int ndof =  track.getPixelClusters().size()*2. + track.getSCTClusters().size()  - 5;

  const Trk::FitQuality* trkFitQuality = new Trk::FitQuality(track.getChi2(), ndof);


  std::vector<const Trk::RIO_OnTrack*> clusters;

  ATH_MSG_VERBOSE( "   ConvertTrack: PixelClusterLoop: Pixel Clusters size = " << track.getPixelClusters().size());
  for( unsigned int cluster_number = 0; cluster_number < track.getPixelClusters().size(); ++cluster_number){


    FTK_RawPixelCluster raw_pixel_cluster = track.getPixelCluster(cluster_number);
    if ( track.isMissingPixelLayer(cluster_number)) {

      ATH_MSG_VERBOSE( " No hit for layer " << cluster_number);
      m_nMissingPixelClusters[cluster_number]++;
      continue;
    }
    if (raw_pixel_cluster.getModuleID()==0){
      ATH_MSG_DEBUG( "hashId is " << raw_pixel_cluster.getModuleID() << " Layer " << cluster_number << " getWordA() "
          << raw_pixel_cluster.getWordA() << " getWordB() " << raw_pixel_cluster.getWordB() );
    }
    const Trk::RIO_OnTrack* pixel_cluster_on_track = createPixelCluster(raw_pixel_cluster,*trkPerigee);
    if (pixel_cluster_on_track==nullptr){
      ATH_MSG_WARNING(" PixelClusterOnTrack failed to create cluster " << cluster_number);
      m_nFailedPixelClusters[cluster_number]++;
    } else {

      clusters.push_back(pixel_cluster_on_track);

      ATH_MSG_VERBOSE(cluster_number << ": r= " << std::sqrt(std::pow(pixel_cluster_on_track->globalPosition().x(),2)+pow(pixel_cluster_on_track->globalPosition().y(),2))
		      << " z= " << pixel_cluster_on_track->globalPosition().z() << " phi " << pixel_cluster_on_track->globalPosition().phi() 
		      << " eta= " << pixel_cluster_on_track->globalPosition().eta());
    }
      
  }	


  //
  // Create the SCT Clusters
  //



  std::vector<const Trk::RIO_OnTrack*> SCT_Clusters;

  ATH_MSG_VERBOSE( "   ConvertTrack: SCTClusterLoop: SCT Clusters size = " << track.getSCTClusters().size());
  for( unsigned int cluster_number = 0; cluster_number < track.getSCTClusters().size(); ++cluster_number){

    FTK_RawSCT_Cluster raw_cluster = track.getSCTCluster(cluster_number);
    if ( track.isMissingSCTLayer(cluster_number)) {
      ATH_MSG_VERBOSE( "  No SCT Hit for layer "  << cluster_number);
      m_nMissingSCTClusters[cluster_number]++;
      continue;
    }
    const Trk::RIO_OnTrack* sct_cluster_on_track = createSCT_Cluster(raw_cluster, *trkPerigee);

	
    if (sct_cluster_on_track==nullptr){
      ATH_MSG_WARNING(" SCT_ClusterOnTrack failed to create cluster " <<  cluster_number);
      m_nFailedSCTClusters[cluster_number]++;
    } else {
      
      ATH_MSG_VERBOSE( cluster_number << ": r= " << std::sqrt(std::pow(sct_cluster_on_track->globalPosition().x(),2)+std::pow(sct_cluster_on_track->globalPosition().y(),2))
		       << " z= " << sct_cluster_on_track->globalPosition().z());
	  
      clusters.push_back(sct_cluster_on_track);
    }
  }
  
  std::sort(clusters.begin(), clusters.end(), compareFTK_Clusters);

  if (m_rejectBadTracks) {
    int itr=0;
    float meanEta = 0;
    float meanPhi=0;
    float phi0=0;
    for (auto pClus=clusters.begin(); pClus!=clusters.end(); pClus++, itr++) {

      ATH_MSG_VERBOSE( itr << ": " << " r= " << std::sqrt(std::pow((*pClus)->globalPosition().x(),2)+pow((*pClus)->globalPosition().y(),2))<<
          "  z= " << (*pClus)->globalPosition().z() << "  mag= " << (*pClus)->globalPosition().mag() << " phi= " << (*pClus)->globalPosition().phi()<< " eta = " << (*pClus)->globalPosition().eta()<<
          "  " << m_id_helper->print_to_string((*pClus)->identify()) );
      meanEta+=(*pClus)->globalPosition().eta();

      if (itr == 0){
        phi0=(*pClus)->globalPosition().phi();
      } else {
        meanPhi+=this->dphi(phi0,(*pClus)->globalPosition().phi());
      }
    }

    if (itr > 0) {
      meanEta = meanEta/(float) itr;
      meanPhi = meanPhi/(float) itr + phi0;
    }

    itr = 0;



    bool problem = false;
    for (auto pClus=clusters.begin(); pClus!=clusters.end(); pClus++, itr++) {
      float dphi = this->dphi((*pClus)->globalPosition().phi(),meanPhi);
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
      ATH_MSG_VERBOSE( itr << ": " << " r= " << std::sqrt(std::pow((*pClus)->globalPosition().x(),2)+pow((*pClus)->globalPosition().y(),2))<<
          "  z= " << (*pClus)->globalPosition().z() << "  mag= " << (*pClus)->globalPosition().mag() << " phi= " << (*pClus)->globalPosition().phi()<< " eta = " << (*pClus)->globalPosition().eta()<<
          "  " << m_id_helper->print_to_string((*pClus)->identify()) );

    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
    typePattern.set(Trk::TrackStateOnSurface::Measurement);

    Trk::TrackStateOnSurface* trackStateOnSurface = new Trk::TrackStateOnSurface(*pClus, 0, 0, 0, typePattern);
    trkTSoSVec->push_back(trackStateOnSurface);

  }

  //
  // Build the TSOS
  //
  trkTSoSVec->push_back( new Trk::TrackStateOnSurface(NULL, trkPerigee));


  //
  // And finally the Trk::Track
  //
  Trk::Track* new_track = new Trk::Track( trkTrackInfo, trkTSoSVec, trkFitQuality);

  //******************************


  return new_track;
}


const Trk::RIO_OnTrack* FTK_DataProviderSvc::createSCT_Cluster(const FTK_RawSCT_Cluster& raw_cluster, const Trk::TrackParameters& trkPerigee) {


  const IdentifierHash hash=raw_cluster.getModuleID();
  const int rawStripCoord= raw_cluster.getHitCoord();
  const int clusterWidth=raw_cluster.getHitWidth();

  // clusterWidth is 0 for a 1-strip cluster, 1 for 2-strips, etc.

  double stripCoord = ((double) rawStripCoord)/2.; // rawStribCoord is in units of half a strip

  int strip = (int) stripCoord;

  const InDetDD::SiDetectorElement* pDE = m_SCT_Manager->getDetectorElement(hash);


  ATH_MSG_VERBOSE( " SCT FTKHit HitCoord rawStripCoord" << rawStripCoord << " hashID 0x" << std::hex << hash << std::dec << " " << m_id_helper->print_to_string(pDE->identify()));

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

  int firstStrip = (int)(stripCoord-0.5*clusterWidth);
  int lastStrip  = (int)(stripCoord+0.5*clusterWidth);

  ATH_MSG_VERBOSE( "FTK_DataProviderSvc::createSCT_Cluster: raw coord= " << rawStripCoord << " stripCoord " << stripCoord << 
		   " width " << clusterWidth+1 << " strip, firstStrip, lastStrip= "  << strip << "," << firstStrip << "," << lastStrip);

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

  ATH_MSG_VERBOSE(" creating SiWidth with nstrips   = " << clusterWidth+1 << " width " << width << " stripLength " << stripLength);
  InDet::SiWidth siWidth(Amg::Vector2D(clusterWidth+1,1), Amg::Vector2D(width,stripLength) );

  //  double shift = pDE->getLorentzCorrection();

  //  double derivedPos = localPos[Trk::locX]+shift;
  //double derivedPos = localPos[Trk::locX];
  //double rawPos = (strip-0.5*design->cells())*pDE->phiPitch();

  //  if(fabs(derivedPos-rawPos)>0.5*pDE->phiPitch()) {
  //  derivedPos = rawPos+shift;
  // }
  //  Amg::Vector2D position(derivedPos, localPos[Trk::locY]);

  Amg::Vector2D position(localPos[Trk::locX], localPos[Trk::locY]);

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
  ATH_MSG_VERBOSE("           " << (*cov)(1,0) << ", " << (*cov)(1,1)) ;


  //      ATH_MSG_VERBOSE( "   ConvertTrack: SCTClusterLoop: Created SCT_Cluster SUCCESS ");
  const IdentifierHash idHash = m_sctId->wafer_hash(m_sctId->wafer_id(pCL->identify()));

  const Trk::RIO_OnTrack* sct_cluster_on_track=nullptr;
  InDet::SCT_ClusterCollection* pColl = getSCT_ClusterCollection(idHash);
  if(pColl!=NULL) {
    pCL->setHashAndIndex(pColl->identifyHash(), pColl->size());
    pColl->push_back(pCL);

    if (m_correctSCTClusters) {
      //	  const Trk::PrepRawData* cluster = sct_cluster;
      sct_cluster_on_track= m_ROTcreator->correct(*pCL,trkPerigee);
    }

     if (!m_correctSCTClusters || sct_cluster_on_track == nullptr) {
      double shift = pDE->getLorentzCorrection();

      Amg::Vector2D locPos(pCL->localPosition()[Trk::locX]+shift,pCL->localPosition()[Trk::locY]);
      ATH_MSG_VERBOSE("locX "<< pCL->localPosition()[Trk::locX] << " locY " << pCL->localPosition()[Trk::locY] << " lorentz shift " << shift);

      sct_cluster_on_track = new InDet::SCT_ClusterOnTrack (pCL,
							    locPos,
							    pCL->localCovariance(),
							    idHash,
							    pCL->globalPosition(),m_broadSCT_Errors); // last parameter: isbroad=true
    }

    if(sct_cluster_on_track!=nullptr && m_doTruth && m_collectionsReady) {
      ATH_MSG_VERBOSE("SCT cluster MC barcode = "<< (int) raw_cluster.getBarcode() );
      createSCT_Truth(pCL->identify(), raw_cluster.getBarcode() );
    }
  } else {
    ATH_MSG_VERBOSE("Failed to get SCT cluster collection, cluster not created");
    
  }

  return sct_cluster_on_track;
}


float FTK_DataProviderSvc::dphi(const float p1, const float p2) const {
  float phi1=p1;
  float phi2=p2;
  while (phi1<0) phi1+=CLHEP::twopi;
  while (phi1> CLHEP::twopi) phi1-=CLHEP::twopi;
  while (phi2<0) phi2+=CLHEP::twopi;
  while (phi2> CLHEP::twopi) phi2-=CLHEP::twopi;
  float diff1 = phi2 - phi1;
  if (fabs(diff1) > CLHEP::pi) {
    if (phi2 > phi1) {
      diff1 = diff1 - CLHEP::twopi;
    } else {
      diff1 = diff1 + CLHEP::twopi;
    }
  }
  //  ATH_MSG_VERBOSE(" phi1 " << p1 << " phi2 " << p2 << " dphi=phi2-phi1= " << diff1);
  return diff1;
}




const Trk::RIO_OnTrack*  FTK_DataProviderSvc::createPixelCluster(const FTK_RawPixelCluster& raw_pixel_cluster,  const Trk::TrackParameters& trkPerigee) {
  IdentifierHash hash = raw_pixel_cluster.getModuleID();
  Identifier wafer_id = m_pixelId->wafer_id(hash); // Need to set up this tool
  const InDetDD::SiDetectorElement* pDE = m_pixelManager->getDetectorElement(hash);

  ATH_MSG_VERBOSE( " Pixel FTKHit hashID 0x" << std::hex << hash << std::dec << " " << m_id_helper->print_to_string(pDE->identify()));

  const InDetDD::SiDetectorElement* pixelDetectorElement = m_pixelManager->getDetectorElement(hash);
  const InDetDD::PixelModuleDesign* design
    (dynamic_cast<const InDetDD::PixelModuleDesign*>(&pixelDetectorElement->design()));

  ATH_MSG_VERBOSE( "FTK_DataProviderSvc::createPixelCluster: raw FTK cluster position: " <<
      " Row(phi): " <<  raw_pixel_cluster.getRowCoord() << " Col(eta): " << raw_pixel_cluster.getColCoord() <<
      " RowWidth: " << raw_pixel_cluster.getRowWidth() << " ColWidth: " << raw_pixel_cluster.getColWidth());

  unsigned int layer = m_pixelId->layer_disk(wafer_id);
  bool isBarrel = (m_pixelId->barrel_ec(wafer_id)==0);


  if (isBarrel) {
    if (layer==0) {
      ATH_MSG_VERBOSE( " IBL ");
    } else {
      ATH_MSG_VERBOSE( " Pixel Barrel layer  " << layer);
    }
  } else {
    ATH_MSG_VERBOSE( " Pixel Endcap layer  " << layer);
  }
  ATH_MSG_VERBOSE( " Module rows= " << design->rows() << " phiPitch= " << design->phiPitch() << " width= " << design->width() );
  ATH_MSG_VERBOSE( " columns = " << design->columns() << " etaPitch= " << design->etaPitch() <<  " length " << design->length());

  int rawLocalPhiCoord;
  
  if (m_reverseIBLlocx && isBarrel && layer==0) {
    rawLocalPhiCoord = 2680 -  raw_pixel_cluster.getRowCoord(); //335*8=2680
  } else {
    rawLocalPhiCoord = raw_pixel_cluster.getRowCoord();
  }

  int rawLocalEtaCoord= raw_pixel_cluster.getColCoord();

  const InDetDD::SiCellId cornerCell(0, 0);
  const InDetDD::SiLocalPosition localPositionOfCornerCell = design->localPositionOfCell(cornerCell);
  const double phi0 = localPositionOfCornerCell.xPhi();
  const double eta0 = localPositionOfCornerCell.xEta();

  ATH_MSG_VERBOSE( " local position of pixel at (0,0) is "<<  phi0 << ",  " << eta0);



  // zero is center of the row coordinates, so to find the cell we can use it, units of 6.25 microns
  // zero is edge of the column coordinates, so to find the cell we add 0.5, units of 25 microns
  double phiPos = ((double) rawLocalPhiCoord) * 6.25e-3 + phi0; // rawLocalPhiCoord=0 is the centre of the zeroth pixel
  double etaPos = ((double) rawLocalEtaCoord) * 25.0e-3 + eta0 - 0.3; // rawLocalEtaCoord=0 is the edge (-0.3mm) of the zeroth pixel.

  if (isBarrel)  {
    phiPos += m_pixelBarrelPhiOffsets[layer];
    etaPos += m_pixelBarrelEtaOffsets[layer];
  } else {
    phiPos += m_pixelEndCapPhiOffsets[layer];
    etaPos += m_pixelEndCapEtaOffsets[layer];
  }


  //  double shift = pixelDetectorElement->getLorentzCorrection();

  ATH_MSG_VERBOSE( "Cluster position phiPos, etaPos "<<  phiPos << ", " << etaPos);

  if ( etaPos >  design->length()/2.) {
    ATH_MSG_DEBUG( "Illegal position: Setting Cluster eta position to module length/2.");
    etaPos = design->length()/2.;
  } else if (  etaPos <  -design->length()/2. ) {
    etaPos = -design->length()/2.;
    ATH_MSG_DEBUG( "llegal position: Setting Cluster eta position to -(module length/2.)");
  }
  if ( phiPos >  design->width()/2.) {
    phiPos = design->width()/2.;
    ATH_MSG_DEBUG( "llegal position: Setting Cluster phi position to module width/2.");
  } else if (  phiPos <  -design->width()/2. ) {
    phiPos = -design->width()/2.;
    ATH_MSG_DEBUG( "llegal position: Setting Cluster phi position to -(module width/2.)");
  }
  const InDetDD::SiLocalPosition cellLocalPosition(etaPos,phiPos);

  const InDetDD::SiCellId  cell = design->cellIdOfPosition(cellLocalPosition);

  int phi_index = cell.phiIndex();
  int eta_index = cell.etaIndex();
  ATH_MSG_VERBOSE( "Cell created with phiIndex " << phi_index << " eta_index " <<   eta_index << " expected: phiIndex " << (int) ((phiPos-phi0)/design->phiPitch()) << " eta index " << (int) ((etaPos-eta0)/design->etaPitch()));


  Identifier pixel_id = m_pixelId->pixel_id(wafer_id, phi_index, eta_index);


  int phiWidth    = std::max(raw_pixel_cluster.getRowWidth(),1u);
  int etaWidth    = std::max(raw_pixel_cluster.getColWidth(),1u);

  int colMin = (int)(eta_index-0.5*etaWidth);
  int colMax = colMin+etaWidth;

  int rowMin = (int)(phi_index-0.5*phiWidth);
  int rowMax = rowMin+phiWidth;


  double etaW = design->widthFromColumnRange(colMin, colMax-1);
  double phiW = design->widthFromRowRange(rowMin, rowMax-1);


  ATH_MSG_VERBOSE("phiW = design->widthFromRowRange("<<rowMin<<","<<rowMax-1<<") = " << phiW);
  ATH_MSG_VERBOSE("etaW = design->widthFromRowRange("<<colMin<<","<<colMax-1<<") = " << etaW);

  InDet::SiWidth siWidth(Amg::Vector2D(phiWidth,etaWidth),Amg::Vector2D(phiW,etaW));

  //  Amg::Vector2D position(phiPos+shift,etaPos);
  Amg::Vector2D position(phiPos,etaPos);

  //  ATH_MSG_VERBOSE("FTK_DataProviderSvc::createPixelCluster: local coordinates phiPos, etaPos"<<  phiPos+shift << ", " << etaPos << " includes Lorentz shift " << shift);
  ATH_MSG_VERBOSE("FTK_DataProviderSvc::createPixelCluster: local coordinates phiPos, etaPos"<<  phiPos << ", " << etaPos);
  ATH_MSG_VERBOSE(" FTK cluster phiwidth " << phiWidth << " etawidth " <<  etaWidth << " siWidth.phiR() " << siWidth.phiR() << " siWidth.z() " << siWidth.z());

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
    ATH_MSG_VERBOSE(" siWidth.colRow() " << siWidth.colRow());
    ATH_MSG_VERBOSE(" siWidth.z() " << siWidth.z() <<   " colRow.y() " << colRow.y() << "  averageZpitch " << averageZPitch  );


    // use parameterization only if the cluster does not
    // contain long pixels or ganged pixels
    // Also require calibration service is available....

    double eta = -std::log(std::tan(trkPerigee.parameters()[Trk::theta]/2.));


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
  ATH_MSG_VERBOSE("           " << (*cov)(1,0) << ", " <<   (*cov)(1,1)) ;



  const IdentifierHash idHash = m_pixelId->wafer_hash(m_pixelId->wafer_id(pixel_cluster->identify()));
  ATH_MSG_VERBOSE(" hash " << hash << " wafer hash " << idHash);

  const Trk::RIO_OnTrack* pixel_cluster_on_track = nullptr;

  InDet::PixelClusterCollection* pColl = getPixelClusterCollection(idHash);
  if(pColl!=NULL) {
    pixel_cluster->setHashAndIndex(pColl->identifyHash(), pColl->size());
    pColl->push_back(pixel_cluster);
    
    if (m_correctPixelClusters) {
      pixel_cluster_on_track = m_ROTcreator->correct(*pixel_cluster,trkPerigee);
    } 
    if (!m_correctPixelClusters || pixel_cluster_on_track==nullptr) {
      double shift = pixelDetectorElement->getLorentzCorrection();
       Amg::Vector2D locPos(pixel_cluster->localPosition()[Trk::locX]+shift,pixel_cluster->localPosition()[Trk::locY]);
      ATH_MSG_VERBOSE("locX "<< pixel_cluster->localPosition()[Trk::locX] << " locY " << pixel_cluster->localPosition()[Trk::locY] << " lorentz shift " << shift);
      pixel_cluster_on_track=new InDet::PixelClusterOnTrack (pixel_cluster,
							     locPos,
							     pixel_cluster->localCovariance(),
							     idHash,
							     pixel_cluster->globalPosition(),
							     pixel_cluster->gangedPixel(),m_broadPixelErrors);  // last parameter: isbroad=true
    }

    if(pixel_cluster_on_track!=nullptr && m_doTruth && m_collectionsReady) {
      ATH_MSG_VERBOSE("Pixel cluster MC barcode = "<< (int) raw_pixel_cluster.getBarcode() );
      createPixelTruth(pixel_cluster->identify(),raw_pixel_cluster.getBarcode() );
    }
  }

  return pixel_cluster_on_track;
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
      ATH_MSG_VERBOSE(" Found GenParticle for SCT hit barcode " << barCode << " pdg_id " << p->pdg_id() << " inserting into SCT_Truth at " << m_ftkSctTruth->size());
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
      ATH_MSG_VERBOSE(" Found GenParticle for Pixel hit barcode " << barCode << " pdg_id " << p->pdg_id() << " inserting into PixelTruth at " << m_ftkPixelTruth->size());
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
  ATH_MSG_VERBOSE(" Creating  PixelClusterCollection with hashid 0x" << std::hex << hashId << std::dec);
  Identifier id = m_pixelId->wafer_id(hashId);
  InDet::PixelClusterCollection* pcoll = new InDet::PixelClusterCollection(hashId);
  pcoll->setIdentifier(id);
  StatusCode sc = m_PixelClusterContainer->addCollection(pcoll,hashId);
  if (sc.isFailure()){
    ATH_MSG_VERBOSE(" PixelClusterContainer->addCollection failed PixelClusterCollection with hashid 0x" << std::hex << hashId << std::dec);
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
  ATH_MSG_VERBOSE(" Creating  SCT_ClusterCollection with hashid 0x" << std::hex << hashId << std::dec);
  Identifier id = m_sctId->wafer_id(hashId);
  InDet::SCT_ClusterCollection* pcoll = new InDet::SCT_ClusterCollection(hashId);
  pcoll->setIdentifier(id);
  StatusCode sc = m_SCT_ClusterContainer->addCollection(pcoll,hashId);
  if (sc.isFailure()){
    ATH_MSG_VERBOSE(" SCT_ClusterContainer->addCollection failed PixelClusterCollection with hashid 0x" << std::hex << hashId << std::dec);
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
    m_got_conv_vertex=false;
    m_got_refit_vertex=false;
    m_got_fast_vertex_refit = false;
    m_got_fast_vertex_conv = false;
    m_got_fast_vertex_raw = false;
    m_nErrors=0;
    for (auto& it :  m_nFailedSCTClusters) it=0;
    for (auto& it :  m_nFailedPixelClusters) it=0;
    for (auto& it :  m_nMissingSCTClusters) it=0;
    for (auto& it :  m_nMissingPixelClusters) it=0;

  }
}
