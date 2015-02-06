/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetReadoutGeometry/SCT_ModuleSideDesign.h"
#include "InDetReadoutGeometry/SCT_BarrelModuleSideDesign.h"
#include "InDetReadoutGeometry/SCT_ForwardModuleSideDesign.h"

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
#include "FTK_DataProviderSvc/FTK_DataProviderSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include <map>

using Gaudi::Units::micrometer;
using namespace InDet;

FTK_DataProviderSvc::FTK_DataProviderSvc(const std::string& name, ISvcLocator* svc):
  AthService(name, svc),
  m_RDO_key("FTK_RDO_Tracks"),
  m_offlineCalibSvc("PixelOfflineCalibSvc", name),
  m_trackFitter("Trk::DistributedKalmanFilter/InDetTrackFitter"),
  m_ftk_tracks(0x0),
  athenaLog(0)
{
  athenaLog = new MsgStream(msgSvc(), name);
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
  
  /*sc = m_trackFitter.retrieve();
  if(sc.isFailure()) {
    (*athenaLog) << MSG::FATAL << "Track Fitter not found! " << endreq;
    return sc;
    }*/
  
  (*athenaLog) << MSG::DEBUG << "Service Initialized" << endreq;
  return sc; 
  
}

StatusCode FTK_DataProviderSvc::beginEvent() {

  StatusCode sc(StatusCode::SUCCESS);

  /// First get the FTK tracks from storegate if you don't already have them ///                                                                                                 
  (*athenaLog) << MSG::DEBUG << "beginEvent: Retrieveing Tracks From StoreGate" << endreq;
  sc = GetTracksFromStoreGate();
  (*athenaLog) << MSG::DEBUG << "beginEvent: Tracks Size = " << m_ftk_tracks->size() << endreq;

  /// Now create the cache if it hasn't been created already, note that this is different for xAOD ///                                                                           
  (*athenaLog) << MSG::DEBUG << "beginEvent: Initializing Track Cache" << endreq;
  (*athenaLog) << MSG::DEBUG << "beginEvent: Cache Size before initialization" << m_cached_tracks.size() << endreq;
  (*athenaLog) << MSG::DEBUG << "beginEvent: Objects Size before initialization" << m_cached_track_objects.size() << endreq;
  if (m_cached_tracks.empty()) sc = InitializeTrackCache();
  (*athenaLog) << MSG::DEBUG << "beginEvent: Cache Size   = " << m_cached_tracks.size() << endreq;
  (*athenaLog) << MSG::DEBUG << "beginEvent: Objects Size = " << m_cached_track_objects.size() << endreq;

  return sc;
}

StatusCode FTK_DataProviderSvc::endEvent() {
  (*athenaLog) << MSG::DEBUG << "endEvent: Cache Size = " << m_cached_tracks.size() << endreq;
  (*athenaLog) << MSG::DEBUG << "endEvent: Objects Size = " << m_cached_track_objects.size() << endreq;
  m_cached_tracks.clear();
  m_cached_track_objects.clear();
  (*athenaLog) << MSG::DEBUG << "endEvent: Cache cleared " << endreq;
  (*athenaLog) << MSG::DEBUG << "endEvent: Cache Size after clearing = " << m_cached_tracks.size() << endreq;
  (*athenaLog) << MSG::DEBUG << "endEvent: Objects Size after clearing = " << m_cached_track_objects.size() << endreq;
  return StatusCode::SUCCESS;
}

StatusCode FTK_DataProviderSvc::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode FTK_DataProviderSvc::GetTracksFullScan(TrackCollection &track_collection, bool do_refit){

  /// Do conversion or return already converted tracks   
  (*athenaLog) << MSG::DEBUG << "GetTracksFullScan: Converting Tracks" << endreq;
  (*athenaLog) << MSG::DEBUG << "GetTracksFullScan: FTK Track Size = " << m_ftk_tracks->size() << endreq;
  for (unsigned int track_counter = 0; track_counter != m_ftk_tracks->size(); ++track_counter){

    /// Check if the track has already been converted ///
    (*athenaLog) << MSG::DEBUG << "GetTracksFullScan: ConversionLoop: Checking if track already converted" << endreq;
    (*athenaLog) << MSG::DEBUG << "GetTracksFullScan: ConversionLoop:  m_cached_tracks[track_counter] = " <<  m_cached_tracks[track_counter] << endreq;
    if( m_cached_tracks[track_counter] == -1) {
      (*athenaLog) << MSG::DEBUG << "GetTracksFullScan: ConversionLoop: Track not yet converted, beginning conversion" << endreq;
      /// This track hasn't been converted yet, do it now ///                                                                                                                      
      Trk::Track* track = ConvertTrack(*m_ftk_tracks->at(track_counter), do_refit);
      //float theta  = track->perigeeParameters()->parameters()[Trk::theta];
      //float eta    = -log(tan(0.5*theta));
      //float qOverP = track->perigeeParameters()->parameters()[Trk::qOverP];
      //float pT     = sin(theta)/qOverP;
      (*athenaLog) << MSG::DEBUG << "GetTracksFullScan: ConversionLoop: Track converted" << endreq;
            
      const int key     = track_counter;
      m_cached_track_objects.push_back(track);
      const int value   = m_cached_track_objects.size() - 1;
      (*athenaLog) << MSG::DEBUG << "GetTracksFullScan: ConversionLoop: Adding to Cache with Key " << key << " and location " << value << endreq;
      m_cached_tracks.insert(std::make_pair( key, value ));
      track_collection.push_back(track);
    }
    else {
      /// Converted Trk::Track already exists, add it to the output collection /// 
      (*athenaLog) << MSG::DEBUG << "GetTracksFullScan: ConversionLoop: Track already converted, retrieveing from cache" << endreq;
      track_collection.push_back(m_cached_track_objects[m_cached_tracks[track_counter]]);
    }
  }
  (*athenaLog) << MSG::DEBUG << "GetTracksFullScan: Return Track Container Size = " << track_collection.size() << endreq;
  
  return StatusCode::SUCCESS;

}

//StatusCode FTK_DataProviderSvc::GetTracksInRoi(TrackCollection &track_collection, const RoiDescriptor* roi, bool do_refit){
StatusCode FTK_DataProviderSvc::GetTracksInRoi(TrackCollection &track_collection, const IRoiDescriptor& roi, bool do_refit){
  
  /// Do conversion or return already converted tracks   
  (*athenaLog) << MSG::DEBUG << "GetTracksInRoi: Converting Tracks" << endreq;
  (*athenaLog) << MSG::DEBUG << "GetTracksInRoi: FTK Track Size = " << m_ftk_tracks->size() << endreq;
  for (unsigned int track_counter = 0; track_counter != m_ftk_tracks->size(); ++track_counter){

    /// Check if the track is inside the ROI ///
    if (!(roi.containsPhi(m_ftk_tracks->at(track_counter)->getPhi() && roi.containsZed( m_ftk_tracks->at(track_counter)->getZ0(),
                                                                                        m_ftk_tracks->at(track_counter)->getCurv())))) continue;

    /// Check if the track has already been converted ///
    (*athenaLog) << MSG::DEBUG << "GetTracksInRoi: ConversionLoop: Checking if track already converted" << endreq;
    (*athenaLog) << MSG::DEBUG << "GetTracksInRoi: ConversionLoop:  m_cached_tracks[track_counter] = " <<  m_cached_tracks[track_counter] << endreq;
    if( m_cached_tracks[track_counter] == -1) {
      (*athenaLog) << MSG::DEBUG << "GetTracksInRoi: ConversionLoop: Track not yet converted, beginning conversion" << endreq;
      /// This track hasn't been converted yet, do it now ///                                                                                                                      
      Trk::Track* track = ConvertTrack(*m_ftk_tracks->at(track_counter), do_refit);
      //float theta  = track->perigeeParameters()->parameters()[Trk::theta];
      //float eta    = -log(tan(0.5*theta));
      //float qOverP = track->perigeeParameters()->parameters()[Trk::qOverP];
      //float pT     = sin(theta)/qOverP;
      (*athenaLog) << MSG::DEBUG << "GetTracksInRoi: ConversionLoop: Track converted" << endreq;
            
      const int key     = track_counter;
      m_cached_track_objects.push_back(track);
      const int value   = m_cached_track_objects.size() - 1;
      (*athenaLog) << MSG::DEBUG << "GetTracksInRoi: ConversionLoop: Adding to Cache with Key " << key << " and location " << value << endreq;
      m_cached_tracks.insert(std::make_pair( key, value ));
      track_collection.push_back(track);
    }
    else {
      /// Converted Trk::Track already exists, add it to the output collection /// 
      (*athenaLog) << MSG::DEBUG << "GetTracksInRoi: ConversionLoop: Track already converted, retrieveing from cache" << endreq;
      track_collection.push_back(m_cached_track_objects[m_cached_tracks[track_counter]]);
    }
  }
  (*athenaLog) << MSG::DEBUG << "GetTracksinRoi: Return Track Container Size = " << track_collection.size() << endreq;
  
  return StatusCode::SUCCESS;

}

StatusCode FTK_DataProviderSvc::GetTracksFromStoreGate(){

  /// Get the FTK Track pointers from StoreGate ///
  StatusCode sc(StatusCode::SUCCESS);

  if (!m_storeGate->contains<FTK_RawTrackContainer>(m_RDO_key)) {
    (*athenaLog) << MSG::WARNING << " FTK tracks  "<< m_RDO_key <<" not found in StoreGate !"<< endreq;
    return sc;
  } else {
    sc = m_storeGate->retrieve(m_ftk_tracks, m_RDO_key);
    if (sc.isFailure()) {
      (*athenaLog) << MSG::ERROR << "Failed to get FTK Tracks Container" << endreq;
      return sc;
    }
  }

  return sc;
}

StatusCode FTK_DataProviderSvc::InitializeTrackCache() {

  if( m_ftk_tracks == 0 ){
    (*athenaLog) << MSG::ERROR << "    InitializeTrackCache: FTKTrackCollection is empty, cannot initialize cahce "<< endreq;
    return StatusCode::FAILURE;
  }

  if (m_cached_tracks.empty()){
    (*athenaLog) << MSG::DEBUG << "    InitializeTrackCache: Creating a new cache for converted FTK_RawTracks -> Trk::Tracks "<< endreq;
    for(unsigned int i = 0; i < m_ftk_tracks->size(); ++i){
      const int key = i; 
      m_cached_tracks.insert(std::make_pair(key, -1.));
    }
  } else {
    (*athenaLog) << MSG::DEBUG << "    InitializeTrackCache: Cache already exists, with size = " << m_cached_tracks.size()  << endreq;
  }

  (*athenaLog) << MSG::DEBUG << "    InitializeTrackCache: Cache created " << endreq;
  
  /// Now quick sanity check: cache and ftk_track container should be the same size ///
  if (m_cached_tracks.size() != m_ftk_tracks->size()){
    (*athenaLog) << MSG::ERROR << "    InitializeTrackCache: FTK Track container has size " << m_ftk_tracks->size() << " but cache has " << m_cached_tracks.size() << endreq;
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}



Trk::Track* FTK_DataProviderSvc::ConvertTrack(const FTK_RawTrack &track, bool do_refit){

  ///////////////////////////////////////
  /// Create track perigee parameters ///
  ///////////////////////////////////////

  (*athenaLog) << MSG::DEBUG << "   ConvertTrack: Beginning Track conversion" << endreq;
  DataVector<const Trk::TrackStateOnSurface>* track_param_vector = new DataVector<const Trk::TrackStateOnSurface>;

  Trk::PerigeeSurface perigeeSurface;
  Trk::Perigee* trkPerigee = new Trk::Perigee( track.getD0(),
                                               track.getZ0(),
                                               track.getPhi(),
                                               atan2(1.0, track.getCotTh()),
                                               track.getCurv(),
                                               perigeeSurface);
  (*athenaLog) << MSG::DEBUG << "   ConvertTrack: Track perigee created" << endreq;

  std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
  typePattern.set(Trk::TrackStateOnSurface::Perigee);
  const Trk::TrackStateOnSurface*trkTSoS= new Trk::TrackStateOnSurface(0,trkPerigee,0,0,typePattern);
  track_param_vector->push_back(trkTSoS);

  /////////////////////////////////
  /// Create the Pixel Clusters ///
  /////////////////////////////////

  (*athenaLog) << MSG::DEBUG << "   ConvertTrack: PixelClusterLoop: Pixel Clusters size = " << track.getPixelClusters().size() << endreq;
  for( unsigned int cluster_number = 0; cluster_number < track.getPixelClusters().size(); ++cluster_number){
    if(track.getPixelClusters().size() > 4){
      (*athenaLog) << MSG::WARNING << "   ConvertTrack: PixelClusterLoop: Too Many Pixel Clusters! " << track.getPixelClusters().size() << endreq;
     break;
    }
    (*athenaLog) << MSG::DEBUG << "   ConvertTrack: PixelClusterLoop: Creating Pixel Cluster Number " << cluster_number << endreq;
    FTK_RawPixelCluster raw_pixel_cluster = track.getPixelCluster(cluster_number);
    InDet::PixelCluster* pixel_cluster = createPixelCluster(raw_pixel_cluster,trkPerigee->eta());
    if(pixel_cluster != NULL) {
      (*athenaLog) << MSG::DEBUG << "   ConvertTrack: PixelClusterLoop: InDet::PixelCluster Created "<< endreq;
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
      track_param_vector->push_back(trackStateOnSurface);
    } else{
      (*athenaLog) << MSG::WARNING << "   ConvertTrack: PixelClusterLoop: InDet::PixelCluster Failed "<< endreq;
    }
  }
  
  ///////////////////////////////
  /// Create the SCT Clusters ///
  ///////////////////////////////

  (*athenaLog) << MSG::DEBUG << "   ConvertTrack: SCTClusterLoop: SCT Clusters size = " << track.getSCTClusters().size() << endreq;
  for( unsigned int cluster_number = 0; cluster_number < track.getSCTClusters().size(); ++cluster_number){
    if(track.getSCTClusters().size() > 8){
      (*athenaLog) << MSG::WARNING << "   ConvertTrack: SCTClusterLoop: Too Many SCT Clusters! " << track.getPixelClusters().size() << endreq;
      break;
    }
    (*athenaLog) << MSG::DEBUG << "   ConvertTrack: SCTClusterLoop: Creating SCT Cluster Number " << cluster_number << endreq;
    FTK_RawSCT_Cluster raw_cluster = track.getSCTCluster(cluster_number);
    InDet::SCT_Cluster* sct_cluster = createSCT_Cluster(raw_cluster);
    if(sct_cluster != NULL) {
      (*athenaLog) << MSG::DEBUG << "   ConvertTrack: SCTClusterLoop: Created SCT_Cluster SUCCESS " << endreq;
      const IdentifierHash idHash = m_sctId->wafer_hash(m_sctId->wafer_id(sct_cluster->identify()));
      InDet::SCT_ClusterOnTrack* sct_cluster_on_track = new InDet::SCT_ClusterOnTrack(sct_cluster,
										      sct_cluster->localPosition(),
										      sct_cluster->localCovariance(),
										      idHash,
										      sct_cluster->globalPosition());
      
      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
      typePattern.set(Trk::TrackStateOnSurface::Measurement);
       
      Trk::TrackStateOnSurface* trackStateOnSurface = new Trk::TrackStateOnSurface(sct_cluster_on_track, 0, 0, 0, typePattern);
      track_param_vector->push_back(trackStateOnSurface);
    } else {
      (*athenaLog) << MSG::WARNING << "   ConvertTrack: SCTClusterLoop: Created SCT_Cluster FAILED " <<  endreq;
    }
  }
    
  ////////////////////////////////////////////////////////////////////
  /// Create track quality information and perform refit if needed ///
  ////////////////////////////////////////////////////////////////////
  (*athenaLog) << MSG::DEBUG << "   ConvertTrack: Creating Track Quality Information " <<  endreq;
  Trk::TrackInfo trkTrackInfo(Trk::TrackInfo::Unknown, Trk::pion);
  //const Trk::FitQuality* fit_quality = new Trk::FitQuality(track->getChi2(), track->getChi2()/track->getChi2ndof());
  const Trk::FitQuality* fit_quality = new Trk::FitQuality(); // we dont have this info in the raw tracks...
  
  (*athenaLog) << MSG::DEBUG << "   ConvertTrack: Creating new Trk::Track object " <<  endreq;
  Trk::Track* new_track = new Trk::Track(trkTrackInfo, track_param_vector, fit_quality);
  
  if(!do_refit) {
    (*athenaLog) << MSG::DEBUG << "   ConvertTrack: No Refit required, returning track " <<  endreq;
    return new_track;
  }
  else {
    (*athenaLog) << MSG::DEBUG << "   ConvertTrack: Beginning Track Refit " <<  endreq;
    Trk::Track* new_fitted_track = m_trackFitter->fit(*new_track, false, Trk::pion);
    if(new_fitted_track != NULL) {
      (*athenaLog) << MSG::DEBUG << "   ConvertTrack: Track Refit SUCEEDED" << new_fitted_track->perigeeParameters() << endreq;
      delete new_track;
      return new_fitted_track;
    }
    else {
      (*athenaLog) << MSG::DEBUG << "   ConvertTrack: Track Refit FAILED Keeping Original" << new_fitted_track->perigeeParameters() << endreq;
      return new_track;
    }
  }
  (*athenaLog) << MSG::FATAL << "   ConvertTrack: End of function. You should never see this" << endreq;
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
  IdentifierHash hash = raw_pixel_cluster.getModuleID(); // Barcode?
  Identifier wafer_id = m_pixelId->wafer_id(hash); // Need to set up this tool

  const InDetDD::SiDetectorElement* pixelDetectorElement = m_pixelManager->getDetectorElement(hash);
  const InDetDD::PixelModuleDesign* design
    (dynamic_cast<const InDetDD::PixelModuleDesign*>(&pixelDetectorElement->design()));

  float locPosPhi = raw_pixel_cluster.getRowCoord(); 
  float locPosEta = raw_pixel_cluster.getColCoord(); 
  int phiWidth    = raw_pixel_cluster.getRowWidth(); 
  int etaWidth    = raw_pixel_cluster.getColWidth(); 

  bool isIBL = (m_pixelId->barrel_ec(wafer_id)==0) && (m_pixelId->layer_disk(wafer_id)==0);
  double phiPos = 0.0;
  double etaPos = 0.0;

  if (isIBL) {
    phiPos = (raw_pixel_cluster.getRowCoord()-0.5*336 + 0.5 - 0.14)*0.05; // + half a pixel  - 7um
    etaPos = (raw_pixel_cluster.getColCoord()-0.5*160)*0.25 * (162.+0.400/0.250) / 160;
  } else {
    if (m_pixelId->barrel_ec(wafer_id)==0) {
      phiPos = (raw_pixel_cluster.getRowCoord()-0.5*328+0.5)*0.05; // + half a pixel for barrel layers
    } else {
      phiPos = (raw_pixel_cluster.getRowCoord()-0.5*328)*0.05;
    }
    etaPos = (raw_pixel_cluster.getColCoord()-0.5*144)*0.4 * 152./144.;
  }

  locPosPhi = phiPos; // this part seems stupid, why is it even set before the is IBL line?
  locPosEta = etaPos;

  InDetDD::SiLocalPosition silPos(locPosEta, locPosPhi);// use the converted positions in mm !!!
  InDetDD::SiCellId cell =  design->cellIdOfPosition(silPos);
  if(!cell.isValid()) {
    return NULL;
  }
  int phi_index = cell.phiIndex();
  int eta_index = cell.etaIndex();

  Identifier pixel_id = m_pixelId->pixel_id(wafer_id, phi_index, eta_index);

  int colMin = (int)(eta_index-0.5*etaWidth);
  int colMax = colMin+etaWidth;

  int rowMin = (int)(phi_index-0.5*phiWidth);
  int rowMax = rowMin+phiWidth;

  double etaW = design->widthFromColumnRange(colMin, colMax-1);
  double phiW = design->widthFromRowRange(rowMin, rowMax-1);

  InDet::SiWidth siWidth(Amg::Vector2D(phiWidth,etaWidth),Amg::Vector2D(phiW,etaW));
  
  double shift = pixelDetectorElement->getLorentzCorrection();
  Amg::Vector2D position(phiPos+shift,etaPos);

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
