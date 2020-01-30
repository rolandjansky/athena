/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include <vector>

#include "FTKTrackMakerMT.h"

#include "GaudiKernel/MsgStream.h"

#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "TrkSurfaces/Surface.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/TriggerInfo.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"

#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"
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
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"

#include "GeneratorObjects/McEventCollection.h"
#include "HepMC/GenParticle.h"

#include "TrkTruthData/PRD_MultiTruthCollection.h"

#include "AtlasDetDescr/AtlasDetectorID.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "PixelConditionsData/PixelOfflineCalibData.h"

#include "FTK_RecToolInterfaces/IFTK_DuplicateTrackRemovalTool.h"
#include "FTK_DataProviderInterfaces/IFTK_UncertaintyTool.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "InDetPrepRawData/SiClusterContainer.h"

#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"


using Gaudi::Units::micrometer;
using namespace InDet;

using Amg::Vector3D;


namespace {
  inline double square(const double x){
    return x*x;
  }
}


/////////////////////////////////////////////////////////////////////////////
FTKTrackMakerMT::FTKTrackMakerMT(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{ }

FTKTrackMakerMT::~FTKTrackMakerMT()
{ }


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


StatusCode FTKTrackMakerMT::initialize(){
  
  
  ATH_MSG_INFO("FTK_RDO_ReaderAlgo::initialize()" );
  
  
  ATH_CHECK( m_rdoContainerKey.initialize() );
  ATH_CHECK( m_pixelClusterContainerKey.initialize() );
  ATH_CHECK( m_sctClusterContainerKey.initialize() );
  ATH_CHECK( m_trackCollectionKey.initialize() );  
  ATH_CHECK(m_SCTDetEleCollKey.initialize());
  ATH_CHECK(m_clusterErrorKey.initialize());

  
  if (m_correctPixelClusters || m_correctSCTClusters) ATH_CHECK(m_uncertaintyTool.retrieve());
  ATH_CHECK(m_ROTcreator.retrieve());
  if (m_remove_duplicates) ATH_CHECK(m_DuplicateTrackRemovalTool.retrieve());
  ATH_CHECK(m_pixelLorentzAngleTool.retrieve());
  ATH_CHECK(m_sctLorentzAngleTool.retrieve());

  if ( m_trainingBeamspotX != 0. || m_trainingBeamspotY != 0. || m_trainingBeamspotZ != 0. || m_trainingBeamspotTiltX != 0. || m_trainingBeamspotTiltY != 0. ) {
    ATH_MSG_INFO( "Correcting for FTK training beamspot at x " <<  m_trainingBeamspotX <<" y " << 	m_trainingBeamspotY
		  << " z " <<  m_trainingBeamspotZ << " TiltX " << m_trainingBeamspotTiltX << "TiltY " << m_trainingBeamspotTiltY );
  }
  
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

StatusCode FTKTrackMakerMT::execute() {

  ATH_MSG_DEBUG("FTK_RDO_ReaderAlgo::execute()");
  
  const PixelID* pixidHelper;
  const SCT_ID* sctidHelper;

  ATH_CHECK(detStore()->retrieve(pixidHelper,"PixelID"));
  ATH_CHECK(detStore()->retrieve(sctidHelper,"SCT_ID"));

  SG::WriteHandle<PixelClusterContainer> pixelClusterContainer(m_pixelClusterContainerKey);
  ATH_CHECK( pixelClusterContainer.record (std::make_unique<PixelClusterContainer>(pixidHelper->wafer_hash_max())) );
  ATH_CHECK(pixelClusterContainer.isValid());
  ATH_MSG_DEBUG( "Container '" << pixelClusterContainer->name() << "' initialised" );
  ATH_CHECK( pixelClusterContainer.symLink (m_pixClusterContainerLinkKey) );
  ATH_MSG_DEBUG( "Pixel clusters '" << pixelClusterContainer.name() << "' symlinked in StoreGate");
  
  
  SG::WriteHandle<SCT_ClusterContainer> sctClusterContainer(m_sctClusterContainerKey);
  ATH_CHECK( sctClusterContainer.record (std::make_unique<SCT_ClusterContainer>(sctidHelper->wafer_hash_max())) );
  ATH_CHECK(sctClusterContainer.isValid());
  ATH_MSG_DEBUG( "Container '" << sctClusterContainer->name() << "' initialised" );
  
  ATH_CHECK( sctClusterContainer.symLink (m_sctClusterContainerLinkKey) );
  ATH_MSG_DEBUG( "SCT clusters '" << sctClusterContainer.name() << "' symlinked in StoreGate");
  
  SG::WriteHandle<TrackCollection> trackCollection(m_trackCollectionKey);
  ATH_CHECK( trackCollection.record (std::make_unique<TrackCollection>() ));
  ATH_CHECK(trackCollection.isValid());
  ATH_MSG_DEBUG( "Container '" << trackCollection.name() << "' initialised" );

  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> pixelDetEle(m_pixelDetEleCollKey);
  const InDetDD::SiDetectorElementCollection* pixelElements(pixelDetEle.retrieve());
  if (pixelElements==nullptr) {
    ATH_MSG_FATAL(m_pixelDetEleCollKey.fullKey() << " could not be retrieved");
    return StatusCode::FAILURE;
  }
  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> sctDetEle(m_SCTDetEleCollKey);
  const InDetDD::SiDetectorElementCollection* sctElements(sctDetEle.retrieve());
  if (sctElements==nullptr) {
    ATH_MSG_ERROR(m_SCTDetEleCollKey.fullKey() << " could not be retrieved");
    return StatusCode::FAILURE;
  }

  SG::ReadHandle<FTK_RawTrackContainer> rawTracks(m_rdoContainerKey);
  
  if (!rawTracks.isValid()) {
    ATH_MSG_WARNING( " failed to get RDO container ");
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG( "Data object " << rawTracks.name() << " found" );

  ATH_MSG_VERBOSE( "Beginning Track conversion");

  for (const FTK_RawTrack* track: *rawTracks) {

    double trkTheta = std::atan2(1.0,track->getCotTh());
    double trkEta = -std::log(std::tan(trkTheta/2));

    ATH_MSG_VERBOSE( "track theta: " << trkTheta << " eta: " << trkEta);

    DataVector<const Trk::TrackStateOnSurface>* trkTSoSVec = new DataVector<const Trk::TrackStateOnSurface>;

    // Create Pixel Clusters
    bool hasIBL=false;
    
    // Find if the track includes IBL - needed for the error calculaton 
    for( unsigned int cluster_number = 0; cluster_number < track->getPixelClusters().size(); ++cluster_number){
      if ( !track->isMissingPixelLayer(cluster_number)) {
	Identifier wafer_id = pixidHelper->wafer_id(Identifier(track->getPixelClusters()[cluster_number].getModuleID()));
	if (pixidHelper->barrel_ec(wafer_id)==0 && pixidHelper->layer_disk(wafer_id)==0) {
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
    (*trkTrackCovm)(0,0)= m_uncertaintyTool->getParamCovMtx(*track,  hasIBL,  FTKTrackParam::d0,     FTKTrackParam::d0);
    (*trkTrackCovm)(1,1)= m_uncertaintyTool->getParamCovMtx(*track,  hasIBL,    FTKTrackParam::z0,     FTKTrackParam::z0);
    (*trkTrackCovm)(2,2)= m_uncertaintyTool->getParamCovMtx(*track,  hasIBL,    FTKTrackParam::phi,    FTKTrackParam::phi);
    (*trkTrackCovm)(3,3)= m_uncertaintyTool->getParamCovMtx(*track,  hasIBL,    FTKTrackParam::theta,  FTKTrackParam::theta);
    (*trkTrackCovm)(4,4)= m_uncertaintyTool->getParamCovMtx(*track,  hasIBL,    FTKTrackParam::qOp,    FTKTrackParam::qOp);
    
    
    ATH_MSG_VERBOSE( " trkTrackCovm: (d0,z0,phi,theta,q/p)= " <<  (*trkTrackCovm)(0,0) << ", "<< (*trkTrackCovm)(1,1)<< ", " <<
		     (*trkTrackCovm)(2,2)<< ", "<<(*trkTrackCovm)(3,3)<< ", "<<  (*trkTrackCovm)(4,4));

    //
    // Construct the measured Perigee Parameters
    //
    
    double trkQOverP = sin(trkTheta)*track->getInvPt();
    Trk::PerigeeSurface periSurf;
    
    double dx = m_trainingBeamspotX + m_trainingBeamspotTiltX*track->getZ0();//correction for tilt
    double dy = m_trainingBeamspotY + m_trainingBeamspotTiltY*track->getZ0();//correction for tilt
    
    double d0 = track->getD0()-dx*sin( track->getPhi())+dy*cos( track->getPhi());
    double z0 = track->getZ0() - ((cos( track->getPhi()) *dx + sin( track->getPhi())*dy)/tan(trkTheta));
    
    const Trk::TrackParameters* trkPerigee = new Trk::Perigee( d0,
							       z0,
							       track->getPhi(),
							       trkTheta,
							       trkQOverP,
      periSurf,
							       trkTrackCovm);
    
    ATH_MSG_VERBOSE( "   Track perigee created  d0 " <<  d0 << " z0 " <<
		     z0<< " phi0 " << track->getPhi() << " theta " << trkTheta << " q/P " << trkQOverP);
    
    //
    // Build the TrackInfo
    //
    Trk::TrackInfo trkTrackInfo = Trk::TrackInfo(Trk::TrackInfo::Unknown, Trk::pion);
    
    //
    // Build the FitQuality
    //
    int ndof =  track->getPixelClusters().size()*2. + track->getSCTClusters().size()  - 5;
    
    const Trk::FitQuality* trkFitQuality = new Trk::FitQuality(track->getChi2(), ndof);
    
    
    std::vector<const Trk::RIO_OnTrack*> clusters;
    clusters.reserve(12);


    ATH_MSG_VERBOSE( "ConvertTrack: PixelClusterLoop: Pixel Clusters size = " << track->getPixelClusters().size());
    for( unsigned int cluster_number = 0; cluster_number < track->getPixelClusters().size(); ++cluster_number){
      
      
      FTK_RawPixelCluster raw_pixel_cluster = track->getPixelCluster(cluster_number);
      if ( track->isMissingPixelLayer(cluster_number)) {
	
	ATH_MSG_VERBOSE( "No hit for layer " << cluster_number);
	continue;
      }
      if (raw_pixel_cluster.getModuleID()==0){
	ATH_MSG_DEBUG( "hashId is " << raw_pixel_cluster.getModuleID() << " Layer " << cluster_number << " getWordA() "
		       << raw_pixel_cluster.getWordA() << " getWordB() " << raw_pixel_cluster.getWordB() );
      }
      const Trk::RIO_OnTrack* pixel_cluster_on_track = createPixelCluster(raw_pixel_cluster,*trkPerigee, pixelClusterContainer.ptr(), pixelElements, pixidHelper);
      if (pixel_cluster_on_track==nullptr){
	ATH_MSG_WARNING("PixelClusterOnTrack failed to create cluster " << cluster_number);
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
    
    const AtlasDetectorID* id_helper;
    ATH_CHECK(detStore()->retrieve(id_helper, "AtlasID"));

    std::vector<const Trk::RIO_OnTrack*> SCT_Clusters;
    
    ATH_MSG_VERBOSE( "   ConvertTrack: SCTClusterLoop: SCT Clusters size = " << track->getSCTClusters().size());
    for( unsigned int cluster_number = 0; cluster_number < track->getSCTClusters().size(); ++cluster_number){
      
      FTK_RawSCT_Cluster raw_cluster = track->getSCTCluster(cluster_number);
      if ( track->isMissingSCTLayer(cluster_number)) {
	ATH_MSG_VERBOSE( "  No SCT Hit for layer "  << cluster_number);
	continue;
      }
      const Trk::RIO_OnTrack* sct_cluster_on_track = createSCT_Cluster(raw_cluster, *trkPerigee, sctClusterContainer.ptr(), sctElements, sctidHelper);
      
      
      if (sct_cluster_on_track==nullptr){
	ATH_MSG_WARNING(" SCT_ClusterOnTrack failed to create cluster " <<  cluster_number);
      } else {
	
	ATH_MSG_VERBOSE( cluster_number << ": r= " << std::sqrt(std::pow(sct_cluster_on_track->globalPosition().x(),2)+std::pow(sct_cluster_on_track->globalPosition().y(),2))
			 << " z= " << sct_cluster_on_track->globalPosition().z());
	
	clusters.push_back(sct_cluster_on_track);
      }
    }

    
    std::sort(clusters.begin(), clusters.end(), 
	      [] (const Trk::RIO_OnTrack* cl1, const Trk::RIO_OnTrack* cl2) 
	      {
		return (cl1->globalPosition().mag()<cl2->globalPosition().mag());
	      }
	      );
    
    
    ATH_MSG_VERBOSE(" Creating TrackStateOnSurface:");
    int itr=0;
    for (auto pClus=clusters.begin(); pClus!=clusters.end(); pClus++, itr++) {
      ATH_MSG_VERBOSE( itr << ": " << " r= " << std::sqrt(std::pow((*pClus)->globalPosition().x(),2)+pow((*pClus)->globalPosition().y(),2))<<
		       "  z= " << (*pClus)->globalPosition().z() << "  mag= " << (*pClus)->globalPosition().mag() << " phi= " << (*pClus)->globalPosition().phi()<< " eta = " << (*pClus)->globalPosition().eta()<<
		       "  " << id_helper->print_to_string((*pClus)->identify()) );
      
      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
      typePattern.set(Trk::TrackStateOnSurface::Measurement);
      
      Trk::TrackStateOnSurface* trackStateOnSurface = new Trk::TrackStateOnSurface(*pClus, 0, 0, 0, typePattern);
      trkTSoSVec->push_back(trackStateOnSurface);
      
    }
    
    //
    // Build the TSOS
    //
    trkTSoSVec->push_back( new Trk::TrackStateOnSurface(nullptr, trkPerigee));
    
    
    //
    // And finally the Trk::Track
    //
    Trk::Track* new_track = new Trk::Track( trkTrackInfo, trkTSoSVec, trkFitQuality);
    
    trackCollection->push_back(new_track);
    
  }
  ATH_MSG_DEBUG("REGTEST FTKTrackmaker made " << trackCollection->size()<< "Tracks");
  return StatusCode::SUCCESS;
}


const Trk::RIO_OnTrack* FTKTrackMakerMT::createSCT_Cluster(const FTK_RawSCT_Cluster& raw_cluster, 
							   const Trk::TrackParameters& trkPerigee, 
							   InDet::SCT_ClusterContainer* sctClusterContainer,
							   const InDetDD::SiDetectorElementCollection* sctElements, const SCT_ID* sctidHelper) const {

  const IdentifierHash hash=raw_cluster.getModuleID();
  const int rawStripCoord= raw_cluster.getHitCoord();
  int clusterWidth=raw_cluster.getHitWidth();

  if (clusterWidth == 0) { // fix for case that width is not set
    if (rawStripCoord%2==1) { 
      clusterWidth=2; // cluster position is odd => between strips => width 2
    } else {
      clusterWidth=1; //  // cluster position is even => on strip => width 1
    } 
  }
  

  double stripCoord = static_cast<double> (rawStripCoord)/2.; // rawStribCoord is in units of half a strip

  int strip = static_cast<int>(stripCoord);

  const InDetDD::SiDetectorElement* pDE = sctElements ->getDetectorElement(hash);


  ATH_MSG_VERBOSE( " SCT FTKHit HitCoord rawStripCoord" << rawStripCoord << " hashID 0x" << std::hex << hash << std::dec << " " <<  sctidHelper->print_to_string(pDE->identify()));

  Identifier wafer_id = sctidHelper->wafer_id(hash);
  Identifier strip_id = sctidHelper->strip_id(wafer_id, strip);


  const InDetDD::SCT_ModuleSideDesign* design;
  if (sctidHelper->is_barrel(wafer_id)){
    design = (static_cast<const InDetDD::SCT_BarrelModuleSideDesign*>(&pDE->design()));
  } else{
    design = (static_cast<const InDetDD::SCT_ForwardModuleSideDesign*>(&pDE->design()));
  }

  const std::vector<Identifier> rdoList{strip_id};

  int firstStrip = static_cast<int>(stripCoord-0.5*(clusterWidth-1));
  int lastStrip  = static_cast<int>(stripCoord+0.5*(clusterWidth-1));

  ATH_MSG_VERBOSE( "FTK_DataProviderSvc::createSCT_Cluster: raw coord= " << rawStripCoord << " stripCoord " << stripCoord << 
		   " width " << clusterWidth << " strip, firstStrip, lastStrip= "  << strip << "," << firstStrip << "," << lastStrip);

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

  const double shift = m_sctLorentzAngleTool->getLorentzShift(hash);
  Amg::Vector2D localPos(centre.xPhi()+shift,  centre.xEta());

  ATH_MSG_VERBOSE(" centre.xPhi() " << centre.xPhi()  << " centre.xEta() " << centre.xEta());

  const std::pair<InDetDD::SiLocalPosition, InDetDD::SiLocalPosition> ends(design->endsOfStrip(centre));
  double stripLength(fabs(ends.first.xEta()-ends.second.xEta()));

  ATH_MSG_VERBOSE(" creating SiWidth with nstrips   = " << clusterWidth << " width " << width << " stripLength " << stripLength);
  InDet::SiWidth siWidth(Amg::Vector2D(clusterWidth,1), Amg::Vector2D(width,stripLength) );

  Amg::Vector2D position(localPos[Trk::locX], localPos[Trk::locY]);

  Amg::MatrixX* cov = new Amg::MatrixX(2,2);
  cov->setZero();

  (*cov)(0,0) = siWidth.phiR()*siWidth.phiR()/12.;
  (*cov)(1,1) = siWidth.z()*siWidth.z()/12.;
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
  const Trk::RIO_OnTrack* sct_cluster_on_track=nullptr;
  const IdentifierHash idHash = sctidHelper->wafer_hash(sctidHelper->wafer_id(pCL->identify()));

  InDet::SCT_ClusterCollection* pColl = getSCT_ClusterCollection(idHash, sctClusterContainer,sctidHelper);
  if(pColl!=nullptr) {
    pCL->setHashAndIndex(pColl->identifyHash(), pColl->size());
    pColl->push_back(pCL);

    if (m_correctSCTClusters) {
      sct_cluster_on_track= m_ROTcreator->correct(*pCL,trkPerigee);
    }

     if (!m_correctSCTClusters || sct_cluster_on_track == nullptr) {
      const double shift = m_sctLorentzAngleTool->getLorentzShift(idHash);

      Amg::Vector2D locPos(pCL->localPosition()[Trk::locX]+shift,pCL->localPosition()[Trk::locY]);
      ATH_MSG_VERBOSE("locX "<< pCL->localPosition()[Trk::locX] << " locY " << pCL->localPosition()[Trk::locY] << " lorentz shift " << shift);

      sct_cluster_on_track = new InDet::SCT_ClusterOnTrack (pCL,
							    locPos,
							    pCL->localCovariance(),
							    idHash,
							    pCL->globalPosition(),m_broadSCT_Errors); // last parameter: isbroad=true
    }

  } else {
    ATH_MSG_VERBOSE("Failed to get SCT cluster collection, cluster not created");
    
  }

  return sct_cluster_on_track;
}



const Trk::RIO_OnTrack*  FTKTrackMakerMT::createPixelCluster(const FTK_RawPixelCluster& raw_pixel_cluster,  const Trk::TrackParameters& trkPerigee, InDet::PixelClusterContainer* pixelClusterContainer, const InDetDD::SiDetectorElementCollection* pixelElements, const PixelID* pixidHelper) const {
  IdentifierHash hash = raw_pixel_cluster.getModuleID();
  Identifier wafer_id = pixidHelper->wafer_id(hash); 
  const InDetDD::SiDetectorElement* pDE = pixelElements->getDetectorElement(hash);

  ATH_MSG_VERBOSE( " Pixel FTKHit hashID 0x" << std::hex << hash << std::dec << " " << pixidHelper->print_to_string(pDE->identify()));

  const InDetDD::PixelModuleDesign* design
    (dynamic_cast<const InDetDD::PixelModuleDesign*>(&pDE->design()));

  ATH_MSG_VERBOSE( "FTK_DataProviderSvc::createPixelCluster: raw FTK cluster position: " <<
      " Row(phi): " <<  raw_pixel_cluster.getRowCoord() << " Col(eta): " << raw_pixel_cluster.getColCoord() <<
      " RowWidth: " << raw_pixel_cluster.getRowWidth() << " ColWidth: " << raw_pixel_cluster.getColWidth());

  unsigned int layer = pixidHelper->layer_disk(wafer_id);
  bool isBarrel = (pixidHelper->barrel_ec(wafer_id)==0);


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
    rawLocalPhiCoord = (design->rows()-1)*8 -  raw_pixel_cluster.getRowCoord(); // reverse direction for IBL matching encoding in TrigFTKSim/atlClustering 
                                                                                //Max value is number of pixels in phi = 335 * 8 bits resolution per pixel 
  } else {
    rawLocalPhiCoord = raw_pixel_cluster.getRowCoord();
  }

  int rawLocalEtaCoord= raw_pixel_cluster.getColCoord();

  const InDetDD::SiCellId cornerCell(0, 0);
  const InDetDD::SiLocalPosition localPositionOfCornerCell = design->localPositionOfCell(cornerCell);
  const double phi0 = localPositionOfCornerCell.xPhi();
  const double eta0 = localPositionOfCornerCell.xEta();

  ATH_MSG_VERBOSE( " local position of pixel at (0,0) is "<<  phi0 << ",  " << eta0);



  // ftk row coordinates: units of 6.25 microns, origin is centre of zeroth pixel
  // ftk column coordinates: units of 25 microns, origin is edge of the zeroth coordinate

  double phiPos = static_cast<double> (rawLocalPhiCoord) * 6.25e-3 + phi0; // rawLocalPhiCoord=0 is the centre of the zeroth pixel

  if (!isBarrel) phiPos-=0.025; // correcttion for the fact that rawLocalPhiCoord=0 seems to be the edge of the endcap pixel, not the centre!
  
  double etaPos=0.;
  if (isBarrel && layer==0) {
    etaPos = static_cast<double> (rawLocalEtaCoord) * 25.0e-3 + eta0 - 0.25; // rawLocalEtaCoord=0 is the edge (-0.25mm) of the zeroth IBL pixel.
  } else {
    etaPos = static_cast<double>( rawLocalEtaCoord) * 25.0e-3 + eta0 - 0.3; // rawLocalEtaCoord=0 is the edge (-0.3mm) of the zeroth pixel.
  }

  if (isBarrel)  {
    phiPos += m_pixelBarrelPhiOffsets[layer];
    etaPos += m_pixelBarrelEtaOffsets[layer];
  } else {
    phiPos += m_pixelEndCapPhiOffsets[layer];
    etaPos += m_pixelEndCapEtaOffsets[layer];
  }



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
  ATH_MSG_VERBOSE( "Cell created with phiIndex " << phi_index << " eta_index " <<   eta_index << " expected: phiIndex " << static_cast<int> ((phiPos-phi0)/design->phiPitch()) << " eta index " << static_cast<int>((etaPos-eta0)/design->etaPitch()));


  Identifier pixel_id = pixidHelper->pixel_id(wafer_id, phi_index, eta_index);


  int phiWidth    = std::max(raw_pixel_cluster.getRowWidth(),1u);
  int etaWidth    = std::max(raw_pixel_cluster.getColWidth(),1u);

  int colMin = static_cast<int>(eta_index-0.5*etaWidth);
  int colMax = colMin+etaWidth;

  int rowMin = static_cast<int>(phi_index-0.5*phiWidth);
  int rowMax = rowMin+phiWidth;


  double etaW = design->widthFromColumnRange(colMin, colMax-1);
  double phiW = design->widthFromRowRange(rowMin, rowMax-1);


  ATH_MSG_VERBOSE("phiW = design->widthFromRowRange("<<rowMin<<","<<rowMax-1<<") = " << phiW);
  ATH_MSG_VERBOSE("etaW = design->widthFromRowRange("<<colMin<<","<<colMax-1<<") = " << etaW);

  InDet::SiWidth siWidth(Amg::Vector2D(phiWidth,etaWidth),Amg::Vector2D(phiW,etaW));

  Amg::Vector2D position(phiPos,etaPos);

  ATH_MSG_VERBOSE("FTK_DataProviderSvc::createPixelCluster: local coordinates phiPos, etaPos"<<  phiPos << ", " << etaPos);
  ATH_MSG_VERBOSE(" FTK cluster phiwidth " << phiWidth << " etawidth " <<  etaWidth << " siWidth.phiR() " << siWidth.phiR() << " siWidth.z() " << siWidth.z());

  // bool blayer = pDE->isBlayer();

  const std::vector<Identifier> rdoList{pixel_id};

  Amg::MatrixX* cov = new Amg::MatrixX(2,2);
  cov->setZero();

  (*cov)(0,0) = siWidth.phiR()*siWidth.phiR()/12.;
  (*cov)(1,1) = siWidth.z()*siWidth.z()/12.;

  ATH_MSG_VERBOSE("Setting defaulterrors (*cov)(0,0) " <<  (*cov)(0,0) << " (*cov)(1,1)" << (*cov)(1,1) );

  const Amg::Vector2D& colRow = siWidth.colRow();
  double averageZPitch = siWidth.z()/colRow.y();
  ATH_MSG_VERBOSE(" siWidth.colRow() " << siWidth.colRow());
  ATH_MSG_VERBOSE(" siWidth.z() " << siWidth.z() <<   " colRow.y() " << colRow.y() << "  averageZpitch " << averageZPitch  );


  // use parameterization only if the cluster does not
  // contain long pixels or ganged pixels
  // Also require calibration service is available....
  
  double eta = -std::log(std::tan(trkPerigee.parameters()[Trk::theta]/2.));
  

  if(averageZPitch > 399*micrometer && averageZPitch < 401*micrometer){
    SG::ReadCondHandle<PixelCalib::PixelOfflineCalibData> offlineCalibData(m_clusterErrorKey);
    if(pDE->isBarrel()){
      // Barrel corrections //
      int ibin = offlineCalibData->getPixelClusterErrorData()->getBarrelBin(eta,int(colRow.y()),int(colRow.x()));
      (*cov)(0,0) = square(offlineCalibData->getPixelClusterErrorData()->getPixelBarrelPhiError(ibin));
      (*cov)(1,1) = square(offlineCalibData->getPixelClusterErrorData()->getPixelBarrelEtaError(ibin));
      ATH_MSG_VERBOSE("Barrel Corrections " << (*cov)(0,0)<< " " <<  (*cov)(1,1) );
    } else{
      // End-cap corrections //
      int ibin = offlineCalibData->getPixelClusterErrorData()->getEndcapBin(int(colRow.y()),int(colRow.x()));
      (*cov)(0,0) = square(offlineCalibData->getPixelClusterErrorData()->getPixelEndcapPhiError(ibin));
      (*cov)(1,1) = square(offlineCalibData->getPixelClusterErrorData()->getPixelEndcapRError(ibin));
      ATH_MSG_VERBOSE("Endcap Corrections " << (*cov)(0,0)<< " " <<  (*cov)(1,1) );
    }      
  } else {
    ATH_MSG_VERBOSE(" Corrections for pixel with length!=400um " << averageZPitch);
    (*cov)(0,0) = square(siWidth.phiR()/colRow.x())/12.  ;
    (*cov)(1,1) = square(averageZPitch)/12.  ;
  }

  InDet::PixelCluster* pixel_cluster = new InDet::PixelCluster(pixel_id, position, rdoList, siWidth,
      pDE, cov);
  ATH_MSG_VERBOSE("covariance " << (*cov)(0,0) << ", " << (*cov)(0,1));
  ATH_MSG_VERBOSE("           " << (*cov)(1,0) << ", " <<   (*cov)(1,1)) ;



  const IdentifierHash idHash = pixidHelper->wafer_hash(pixidHelper->wafer_id(pixel_cluster->identify()));
  ATH_MSG_VERBOSE(" hash " << hash << " wafer hash " << idHash);

  const Trk::RIO_OnTrack* pixel_cluster_on_track=nullptr;

  InDet::PixelClusterCollection* pColl = getPixelClusterCollection(idHash, pixelClusterContainer, pixidHelper);
  if(pColl!=nullptr) {
    pixel_cluster->setHashAndIndex(pColl->identifyHash(), pColl->size());
    pColl->push_back(pixel_cluster);
    
    if (m_correctPixelClusters) {
      pixel_cluster_on_track =  m_ROTcreator->correct(*pixel_cluster,trkPerigee);
    } 
    if (!m_correctPixelClusters || pixel_cluster_on_track==nullptr) {
      const double shift = m_pixelLorentzAngleTool->getLorentzShift(hash);
      Amg::Vector2D locPos(pixel_cluster->localPosition()[Trk::locX]+shift,pixel_cluster->localPosition()[Trk::locY]);
      ATH_MSG_VERBOSE("locX "<< pixel_cluster->localPosition()[Trk::locX] << " locY " << pixel_cluster->localPosition()[Trk::locY] << " lorentz shift " << shift);
      pixel_cluster_on_track=new InDet::PixelClusterOnTrack (pixel_cluster,
							     locPos,
							     pixel_cluster->localCovariance(),
							     idHash,
							     pixel_cluster->globalPosition(),
							     pixel_cluster->gangedPixel(),m_broadPixelErrors);  // last parameter: isbroad=true
    }

  }

  return pixel_cluster_on_track;
 }





InDet::PixelClusterCollection* FTKTrackMakerMT::getPixelClusterCollection(IdentifierHash hashId, InDet::PixelClusterContainer* pixelClusterContainer, const PixelID* pixidHelper) const {
  
  InDet::PixelClusterCollection* pcoll = nullptr;
  //use naughtyRetrieve to make clear this is a potentially thread hostile retrieve.
  //naughtyRetrieve will protect external caches if it has an external cache by failing
  if( pixelClusterContainer->naughtyRetrieve(hashId, pcoll).isFailure()){
    ATH_MSG_ERROR("Using a thread hostile naughtyRetrieve on an external cache");
    return nullptr;
  }
  if(pcoll != nullptr) {
    return pcoll;
  }
  ATH_MSG_VERBOSE(" Creating  PixelClusterCollection with hashid 0x" << std::hex << hashId << std::dec);
  Identifier id = pixidHelper->wafer_id(hashId);
  pcoll = new InDet::PixelClusterCollection(hashId);
  pcoll->setIdentifier(id);
  StatusCode sc = pixelClusterContainer->addCollection(pcoll,hashId);
  if (sc.isFailure()){
    ATH_MSG_VERBOSE(" PixelClusterContainer->addCollection failed PixelClusterCollection with hashid 0x" << std::hex << hashId << std::dec);
    delete pcoll;
    return nullptr;
  }
  return pcoll;
}

InDet::SCT_ClusterCollection*  FTKTrackMakerMT::getSCT_ClusterCollection(IdentifierHash hashId,InDet::SCT_ClusterContainer* sctClusterContainer, const SCT_ID* sctidHelper) const {

  InDet::SCT_ClusterCollection* pcoll = nullptr;
  //use naughtyRetrieve to make clear this is a potentially thread hostile retrieve.
  //naughtyRetrieve will protect external caches if it has an external cache by failing
  if( sctClusterContainer->naughtyRetrieve(hashId, pcoll).isFailure()){
    ATH_MSG_ERROR("Using a thread hostile naughtyRetrieve on an external cache");
    return nullptr;
  }
  if(pcoll != nullptr) {
    return pcoll;
  }
  ATH_MSG_VERBOSE(" Creating  SCT_ClusterCollection with hashid 0x" << std::hex << hashId << std::dec);
  Identifier id = sctidHelper->wafer_id(hashId);
  pcoll = new InDet::SCT_ClusterCollection(hashId);
  pcoll->setIdentifier(id);
  StatusCode sc = sctClusterContainer->addCollection(pcoll,hashId);
  if (sc.isFailure()){
    ATH_MSG_VERBOSE(" SCT_ClusterContainer->addCollection failed PixelClusterCollection with hashid 0x" << std::hex << hashId << std::dec);
    delete pcoll;
    return nullptr;
  }
  return pcoll;
}


