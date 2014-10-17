/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////
// SiSmearedDigitizationTool.cxx
//   Implementation file for class SiSmearedDigitizationTool
////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software

// Pixel digitization includes
#include "FastSiDigitization/SiSmearedDigitizationTool.h"

// Det Descr
#include "Identifier/Identifier.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "ISF_FatrasDetDescrModel/PlanarDetElement.h"

#include "InDetReadoutGeometry/SiReadoutCellId.h"
#include "InDetReadoutGeometry/SiDetectorDesign.h"
#include "InDetReadoutGeometry/SiCellId.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetSimData/InDetSimDataCollection.h"

// Random numbers
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Random/RandLandau.h"
#include "CLHEP/Vector/ThreeVector.h"

// Gaudi includes
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/PropertyMgr.h"

// DataHandle
#include "StoreGate/DataHandle.h"

// Pile-up
#include "PileUpTools/PileUpMergeSvc.h"

#include "InDetReadoutGeometry/SiDetectorDesign.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetReadoutGeometry/SCT_ModuleSideDesign.h"
#include "InDetReadoutGeometry/SCT_BarrelModuleSideDesign.h"
#include "InDetReadoutGeometry/SCT_ForwardModuleSideDesign.h"

// Fatras
#include "InDetPrepRawData/PixelCluster.h"
#include "SiClusterizationTool/ClusterMakerTool.h"
#include "TrkExUtils/LineIntersection2D.h"
#include "InDetPrepRawData/SiWidth.h"
#include "InDetSimEvent/SiHitCollection.h"
#include "SiClusterizationTool/IPixelClusteringTool.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetPrepRawData/SiCluster.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

// Trk includes
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"

// Root
#include "TTree.h"
#include "TFile.h"

#include <cmath>

using namespace InDetDD;

// Constructor with parameters:
SiSmearedDigitizationTool::SiSmearedDigitizationTool(const std::string &type, const std::string &name, 
						     const IInterface* parent):

  PileUpToolBase(type, name, parent),
  m_thpcsi(NULL),
  m_rndmSvc("AtRndmGenSvc",name),
  m_manager_pix(NULL),
  m_manager_sct(NULL),
  m_pixel_ID(0),
  m_sct_ID(0),
  m_randomEngine(0),
  m_randomEngineName("SiSmearedDigitization"),
  m_pitch_X(0),
  m_pitch_Y(0),
  m_merge(false),
  m_nSigma(0.),
  m_pixelClusterContainer(0),
  m_sctClusterContainer(0),
  m_planarClusterContainer(0),
  m_mergeSvc("PileUpMergeSvc",name),
  m_prdTruthNamePixel("PRD_MultiTruthPixel"),
  m_prdTruthNameSCT("PRD_MultiTruthSCT"),
  m_prdTruthNamePlanar("PRD_MultiTruthPlanar"),
  m_SmearPixel(true), //true: smear pixel --- false: smear SCT
  m_emulateAtlas(false), // error rotation for endcap SCT
  m_detElementMapName("Pixel_IdHashDetElementMap"),
  m_checkSmear(false),
  m_thistSvc(NULL),
  m_outputFile(NULL),
  m_currentTree(NULL),
  m_x_pixel(0),
  m_y_pixel(0),
  m_x_exit_pixel(0),
  m_y_exit_pixel(0),
  m_z_exit_pixel(0),
  m_x_entry_pixel(0),
  m_y_entry_pixel(0),
  m_z_entry_pixel(0),
  m_x_pixel_global(0),
  m_y_pixel_global(0),
  m_z_pixel_global(0),
  m_x_SCT(0), 
  m_x_exit_SCT(0),
  m_y_exit_SCT(0),
  m_z_exit_SCT(0),
  m_x_entry_SCT(0),
  m_y_entry_SCT(0),
  m_z_entry_SCT(0),
  m_x_pixel_smeared(0),
  m_y_pixel_smeared(0),
  m_x_SCT_smeared(0),
  m_Err_x_pixel(0),
  m_Err_y_pixel(0),
  m_Err_x_SCT(0),
  m_Err_y_SCT(0),
  m_trackingGeometrySvc("TrackingGeometrySvc","AtlasTrackingGeometrySvc"),
  m_trackingGeometry(0),
  m_trackingGeometryName("AtlasTrackingGeometry"),
  m_useCustomGeometry(false)
{
  declareInterface<ISiSmearedDigitizationTool>(this);
  declareProperty("RndmSvc",                   m_rndmSvc, "Random Number Service used in SCT & Pixel digitization" );
  declareProperty("RndmEngine",                m_randomEngineName, "Random engine name");
  declareProperty("InputObjectName",           m_inputObjectName="PixelHits", "Input Object name" );
  declareProperty("pitch_X",                     m_pitch_X);
  declareProperty("pitch_Y",                     m_pitch_Y);
  declareProperty("MergeClusters",               m_merge);
  declareProperty("Nsigma",                      m_nSigma);
  declareProperty("SmearPixel",                m_SmearPixel, "Enable Pixel or SCT Smearing");
  declareProperty("PixelClusterContainerName", m_pixel_SiClustersName="PixelClusters");
  declareProperty("SCT_ClusterContainerName",  m_Sct_SiClustersName="SCT_Clusters");
  declareProperty("PlanarClusterContainerName", m_planar_SiClustersName="PlanarClusters");
  declareProperty("CheckSmear", m_checkSmear);
  
  // get the service handle for the TrackingGeometry
  declareProperty("TrackingGeometrySvc"          , m_trackingGeometrySvc);
  declareProperty("UseCustomGeometry", m_useCustomGeometry);
  
}


// Initialize method:
StatusCode SiSmearedDigitizationTool::initialize()
{

  ATH_MSG_DEBUG ( "SiSmearedDigitizationTool::initialize()" );

  //locate the AtRndmGenSvc and initialize our local ptr
  if (!m_rndmSvc.retrieve().isSuccess()) 
    {
      ATH_MSG_ERROR ( "Could not find given RndmSvc" );
      return StatusCode::FAILURE;
    }

  if(m_SmearPixel){ // Smear Pixel
    // Get the Pixel Detector Manager
    if (StatusCode::SUCCESS != detStore()->retrieve(m_manager_pix,"Pixel") ) {
      ATH_MSG_ERROR ( "Can't get Pixel_DetectorManager " );
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG ( "Retrieved Pixel_DetectorManager with version "  << m_manager_pix->getVersion().majorNum() );
    }

    if (detStore()->retrieve(m_pixel_ID, "PixelID").isFailure()) {
      ATH_MSG_ERROR ( "Could not get Pixel ID helper" );
      return StatusCode::FAILURE;
    }

  }else{ // Smear SCT
    // Get the SCT Detector Manager
    if (StatusCode::SUCCESS != detStore()->retrieve(m_manager_sct,"SCT") ) {
      ATH_MSG_ERROR ( "Can't get SCT_DetectorManager " );
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG ( "Retrieved SCT_DetectorManager with version "  << m_manager_sct->getVersion().majorNum() );
    }
    
    if (detStore()->retrieve(m_sct_ID, "SCT_ID").isFailure()) {
      ATH_MSG_ERROR ( "Could not get SCT ID helper" );
      return StatusCode::FAILURE;
    }
    
    if (detStore()->retrieve(m_pixel_ID, "PixelID").isFailure()) {
      ATH_MSG_ERROR ( "Could not get Pixel ID helper" );
      return StatusCode::FAILURE;
    }
    
    m_inputObjectName="SCT_Hits"; // Set the input object name
      
  }
  
  //Get own engine with own seeds:
  m_randomEngine = m_rndmSvc->GetEngine(m_randomEngineName);
  if (!m_randomEngine) {
    ATH_MSG_ERROR( "Could not get random engine '" << m_randomEngineName << "'" );
    return StatusCode::FAILURE; 
  }

  if (m_inputObjectName=="") 
    {
      ATH_MSG_FATAL ( "Property InputObjectName not set !" );
      return StatusCode::FAILURE;
    } 
  else 
    {
      ATH_MSG_DEBUG ( "Input objects: '" << m_inputObjectName << "'" );
    }

  //locate the PileUpMergeSvc and initialize our local ptr
  if (!m_mergeSvc.retrieve().isSuccess()) {
    ATH_MSG_ERROR ( "Could not find PileUpMergeSvc" );
    return StatusCode::FAILURE;
  }

  if (m_SmearPixel){

    if (m_trackingGeometrySvc.retrieve().isFailure()) {
      ATH_MSG_FATAL( "Cannot retrieve TrackingGeometrySvc. Abort job. " );
      return StatusCode::FAILURE;
    }
    
    m_trackingGeometryName = m_trackingGeometrySvc->trackingGeometryName();

  }
  
  if (m_checkSmear){
    
    // get THistSvc
    if (service("THistSvc",m_thistSvc).isFailure()) {
      ATH_MSG_ERROR("Cannot find THistSvc ");
      return StatusCode::FAILURE;
    }

    if (m_SmearPixel){
      m_outputFile = new TFile("CheckSmearing_Pixel.root","RECREATE");
      m_currentTree = new TTree("PixelTree","Check smearing Pixel");
      m_currentTree->Branch("pixel_X"       , &m_x_pixel, "m_x_pixel/D");
      m_currentTree->Branch("pixel_Y"       , &m_y_pixel, "m_y_pixel/D");
      m_currentTree->Branch("pixel_X_exit"       , &m_x_exit_pixel, "m_x_exit_pixel/D");
      m_currentTree->Branch("pixel_Y_exit"       , &m_y_exit_pixel, "m_y_exit_pixel/D");
      m_currentTree->Branch("pixel_Z_exit"       , &m_z_exit_pixel, "m_z_exit_pixel/D");
      m_currentTree->Branch("pixel_X_entry"       , &m_x_entry_pixel, "m_x_entry_pixel/D");
      m_currentTree->Branch("pixel_Y_entry"       , &m_y_entry_pixel, "m_y_entry_pixel/D");
      m_currentTree->Branch("pixel_Z_entry"       , &m_z_entry_pixel, "m_z_entry_pixel/D");
      m_currentTree->Branch("pixel_X_global", &m_x_pixel_global, "m_x_pixel_global/D");
      m_currentTree->Branch("pixel_Y_global", &m_y_pixel_global, "m_y_pixel_global/D");
      m_currentTree->Branch("pixel_Z_global", &m_z_pixel_global, "m_z_pixel_global/D");
      m_currentTree->Branch("pixel_X_smear" , &m_x_pixel_smeared, "m_x_pixel_smeared/D");
      m_currentTree->Branch("pixel_Y_smear" , &m_y_pixel_smeared, "m_y_pixel_smeared/D");
      m_currentTree->Branch("pixel_Err_X"   , &m_Err_x_pixel, "m_Err_x_pixel/D");
      m_currentTree->Branch("pixel_Err_Y"   , &m_Err_y_pixel, "m_Err_y_pixel/D");

      if( m_thistSvc->regTree("PixelTree",m_currentTree).isFailure()) {
	ATH_MSG_ERROR("Cannot register Ttree");
	return StatusCode::FAILURE;
      }else{
	ATH_MSG_DEBUG ( "Ttree registered" );      
      }
    }
    else{
      m_outputFile = new TFile("CheckSmearing_SCT.root","RECREATE");
      m_currentTree = new TTree("SCT_Tree","Check smearing SCT");
      m_currentTree->Branch("SCT_X"        , &m_x_SCT, "m_x_SCT/D");
      m_currentTree->Branch("SCT_exit_X"        , &m_x_exit_SCT, "m_x_exit_SCT/D");
      m_currentTree->Branch("SCT_exit_Y"        , &m_y_exit_SCT, "m_y_exit_SCT/D");
      m_currentTree->Branch("SCT_exit_Z"        , &m_z_exit_SCT, "m_z_exit_SCT/D");
      m_currentTree->Branch("SCT_entry_X"        , &m_x_entry_SCT, "m_x_entry_SCT/D");
      m_currentTree->Branch("SCT_entry_Y"        , &m_y_entry_SCT, "m_y_entry_SCT/D");
      m_currentTree->Branch("SCT_entry_Z"        , &m_z_entry_SCT, "m_z_entry_SCT/D");
      m_currentTree->Branch("SCT_X_smear"  , &m_x_SCT_smeared, "m_x_SCT_smeared/D");
      m_currentTree->Branch("SCT_Err_X"    , &m_Err_x_SCT, "m_Err_x_SCT/D");

      if( m_thistSvc->regTree("SCT_Tree",m_currentTree).isFailure()) {
	ATH_MSG_ERROR("Cannot register Ttree");
	return StatusCode::FAILURE;
      }else{
	ATH_MSG_DEBUG ( "Ttree registered" );      
      }
    }
    
    
  }
  
  return StatusCode::SUCCESS;
}

// Finalize method:
StatusCode SiSmearedDigitizationTool::finalize()
{

  if (m_checkSmear){
    m_outputFile->cd();
    m_currentTree->Write();
    m_outputFile->Close();
    ATH_MSG_DEBUG ( "SiSmearedDigitizationTool : Writing Tree" );
    
  }

  ATH_MSG_DEBUG ( "SiSmearedDigitizationTool : finalize()" );


  return StatusCode::SUCCESS;

}

StatusCode SiSmearedDigitizationTool::prepareEvent(unsigned int) 
{

  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: in pixel prepareEvent() ---" );
  
  m_siHitCollList.clear();
  m_thpcsi = new TimedHitCollection<SiHit>();

  return StatusCode::SUCCESS;
}


StatusCode SiSmearedDigitizationTool::processBunchXing(int bunchXing,
						       PileUpEventInfo::SubEvent::const_iterator bSubEvents,
						       PileUpEventInfo::SubEvent::const_iterator eSubEvents) 
{

  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: in pixel processBunchXing() ---" );

  m_seen.push_back(std::make_pair(std::distance(bSubEvents,eSubEvents), bunchXing));
  PileUpEventInfo::SubEvent::const_iterator iEvt(bSubEvents);
  while (iEvt != eSubEvents) {
    StoreGateSvc& seStore(*iEvt->pSubEvtSG);
    PileUpTimeEventIndex thisEventIndex(PileUpTimeEventIndex(static_cast<int>(iEvt->time()),iEvt->index()));
    const SiHitCollection* seHitColl(NULL);
    if (!seStore.retrieve(seHitColl,m_inputObjectName).isSuccess()) {
      ATH_MSG_ERROR ( "SubEvent SiHitCollection not found in StoreGate " << seStore.name() );
      return StatusCode::FAILURE;
    }

    //    if (seHitColl->size() == 0) {break;}

    ATH_MSG_INFO ( "SiHitCollection found with " << seHitColl->size() << " hits" );
    //Copy Hit Collection
    SiHitCollection* siHitColl(new SiHitCollection("PixelHits"));
    SiHitCollection::const_iterator i(seHitColl->begin());
    SiHitCollection::const_iterator e(seHitColl->end());
    // Read hits from this collection
    for (; i!=e; ++i) {
      const SiHit sihit(*i);
      siHitColl->Insert(sihit);
    }
    m_thpcsi->insert(thisEventIndex, siHitColl);
    //store these for deletion at the end of mergeEvent
    m_siHitCollList.push_back(siHitColl);
    ++iEvt;


  }
  return StatusCode::SUCCESS;
}


StatusCode SiSmearedDigitizationTool::processAllSubEvents() {

  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: in pixel processAllSubEvents() ---" );

  if (m_useCustomGeometry){
    iFatras::PlanarClusterContainer * symSiContainer=0;
    m_planarClusterContainer = new iFatras::PlanarClusterContainer(m_pixel_ID->wafer_hash_max());
    
    if(!m_planarClusterContainer) {
      ATH_MSG_FATAL( "[ --- ] Could not create PlanarClusterContainer");
      return StatusCode::FAILURE;
    }
    
    // --------------------------------------
    // PlanarCluster container registration 
    
    m_planarClusterContainer->cleanup();
    if ((evtStore()->record(m_planarClusterContainer, m_planar_SiClustersName)).isFailure())   {
      if ((evtStore()->retrieve(m_planarClusterContainer, m_planar_SiClustersName)).isFailure())   {
	ATH_MSG_FATAL("[ hitproc ] Error while registering PlanarCluster container");
	return StatusCode::FAILURE;
      }
    }
    
    if ((evtStore()->symLink(m_planarClusterContainer,symSiContainer)).isFailure()) {
      ATH_MSG_FATAL( "[ --- ] PlanarClusterContainer could not be symlinked to PlanarClusterContainter in StoreGate !" );
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO( "[ hitproc ] PlanarClusterContainer symlinked to PlanarClusterContainer in StoreGate" );
    }
    
  } else {

    InDet::SiClusterContainer* symSiContainer=0;
  
    if(m_SmearPixel){ // Smear Pixel
      m_pixelClusterContainer = new InDet::PixelClusterContainer(m_pixel_ID->wafer_hash_max());
      
      if(!m_pixelClusterContainer) {
	ATH_MSG_FATAL( "[ --- ] Could not create PixelClusterContainer");
	return StatusCode::FAILURE;
      }

      // --------------------------------------
      // PixelCluster container registration 
      
      m_pixelClusterContainer->cleanup();
      if ((evtStore()->record(m_pixelClusterContainer, m_pixel_SiClustersName)).isFailure())   {
	if ((evtStore()->retrieve(m_pixelClusterContainer, m_pixel_SiClustersName)).isFailure())   {
	  ATH_MSG_FATAL("[ hitproc ] Error while registering PixelCluster container");
	  return StatusCode::FAILURE;
	}
      }
      
      // symlink the Pixel Container 
      // Pixel
      
      if ((evtStore()->symLink(m_pixelClusterContainer,symSiContainer)).isFailure()) {
	ATH_MSG_FATAL( "[ --- ] PixelClusterContainer could not be symlinked to SiClusterContainter in StoreGate !" );
	return StatusCode::FAILURE;
      } else {
	ATH_MSG_INFO( "[ hitproc ] PixelClusterContainer symlinked to SiClusterContainer in StoreGate" );
      }
      
    }else{ // Smear SCT
      m_sctClusterContainer = new InDet::SCT_ClusterContainer(m_sct_ID->wafer_hash_max());
      
      if(!m_sctClusterContainer) {
	ATH_MSG_FATAL( "[ --- ] Could not create SCT_ClusterContainer");
	return StatusCode::FAILURE;
      }

      // --------------------------------------
      // SCT_Cluster container registration
      m_sctClusterContainer->cleanup();
      if ((evtStore()->record(m_sctClusterContainer, m_Sct_SiClustersName)).isFailure())   {
	ATH_MSG_FATAL("[ hitproc ] Error while registering SCT_Cluster container");
	return StatusCode::FAILURE;
      }
      // symlink the SCT Container 
      // SCT
      
      if ((evtStore()->symLink(m_sctClusterContainer,symSiContainer)).isFailure()) {
	ATH_MSG_FATAL( "[ --- ] SCT_ClusterContainer could not be symlinked to SiClusterContainter in StoreGate !" );
	return StatusCode::FAILURE;
      } else {
	ATH_MSG_DEBUG( "[ hitproc ] SCT_ClusterContainer symlinked to SiClusterContainer in StoreGate" );
      }
      
    }
    
  }

  if (retrieveTruth().isFailure()) {
    ATH_MSG_FATAL ( "retrieveTruth() failed!" );
    return StatusCode::FAILURE;
  }

  //  get the container(s)
  typedef PileUpMergeSvc::TimedList<SiHitCollection>::type TimedHitCollList;
  
  m_simHitColl = new SiHitCollection;

  //this is a list<pair<time_t, DataLink<SCTUncompressedHitCollection> > >
  TimedHitCollList hitCollList;
  unsigned int numberOfSimHits(0);
  if ( !(m_mergeSvc->retrieveSubEvtsData(m_inputObjectName, hitCollList, numberOfSimHits).isSuccess()) && hitCollList.size()==0 ) {
    ATH_MSG_ERROR ( "Could not fill TimedHitCollList" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ( hitCollList.size() << " SiHitCollections with key " << m_inputObjectName << " found" );
  }
  
  // Define Hit Collection
  TimedHitCollection<SiHit> thpcsi(numberOfSimHits);
  
  //now merge all collections into one
  TimedHitCollList::iterator   iColl(hitCollList.begin());
  TimedHitCollList::iterator endColl(hitCollList.end()  );
  
  // loop on the hit collections
  while ( iColl != endColl ) {
    const SiHitCollection* p_collection(iColl->second);
    thpcsi.insert(iColl->first, p_collection);
    ATH_MSG_DEBUG ( "SiHitCollection found with " << p_collection->size() << " hits" );
    ++iColl;
  }
  m_thpcsi = &thpcsi;

  // Process the Hits straw by straw: get the iterator pairs for given straw
  if(this->digitize().isFailure()) {
    ATH_MSG_FATAL ( "digitize method failed!" );
    return StatusCode::FAILURE;
  }

  if(this->mergeEvent().isFailure()) {
    ATH_MSG_FATAL ( "merge method failed!" );
    return StatusCode::FAILURE;
  }
  
  if (createAndStoreRIOs().isFailure()) {
    ATH_MSG_FATAL ( "createAndStoreRIOs() failed!" );
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_DEBUG ( "createAndStoreRIOs() succeeded" );
  }

  return StatusCode::SUCCESS;
  
}

StatusCode SiSmearedDigitizationTool::retrieveTruth(){
  
  if (m_useCustomGeometry) {
    m_planarPrdTruth = new PRD_MultiTruthCollection;
    
    if ((evtStore()->contains<PRD_MultiTruthCollection>(m_prdTruthNamePlanar))){
      if((evtStore()->retrieve(m_planarPrdTruth, m_prdTruthNamePlanar)).isFailure()){
	ATH_MSG_FATAL("Could not retrieve collection " << m_prdTruthNamePlanar);
	return StatusCode::FAILURE;
      }
    }else{
      if((evtStore()->record(m_planarPrdTruth, m_prdTruthNamePlanar)).isFailure()){
	ATH_MSG_FATAL("Could not record collection " << m_prdTruthNamePlanar);
	return StatusCode::FAILURE;
      }
    } 
  
  } else {
    
    if(m_SmearPixel){ // Smear Pixel
      
      m_pixelPrdTruth = new PRD_MultiTruthCollection;
      
      if ((evtStore()->contains<PRD_MultiTruthCollection>(m_prdTruthNamePixel))){
	if((evtStore()->retrieve(m_pixelPrdTruth, m_prdTruthNamePixel)).isFailure()){
	  ATH_MSG_FATAL("Could not retrieve collection " << m_prdTruthNamePixel);
	  return StatusCode::FAILURE;
	}
      }else{
	if((evtStore()->record(m_pixelPrdTruth, m_prdTruthNamePixel)).isFailure()){
	  ATH_MSG_FATAL("Could not record collection " << m_prdTruthNamePixel);
	  return StatusCode::FAILURE;
	}
      } 
    }else{ // Smear SCT
      m_SCTPrdTruth = new PRD_MultiTruthCollection;
      
      if ((evtStore()->contains<PRD_MultiTruthCollection>(m_prdTruthNameSCT))){
	if((evtStore()->retrieve(m_SCTPrdTruth, m_prdTruthNameSCT)).isFailure()){
	  ATH_MSG_FATAL("Could not retrieve collection " << m_prdTruthNameSCT);
	  return StatusCode::FAILURE;
	}
      }else{
	if((evtStore()->record(m_SCTPrdTruth, m_prdTruthNameSCT)).isFailure()){
	  ATH_MSG_FATAL("Could not record collection " << m_prdTruthNameSCT);
	  return StatusCode::FAILURE;
	} 
      }
    }
  }
  
  return StatusCode::SUCCESS;
  
}

template<typename CLUSTER> 
StatusCode SiSmearedDigitizationTool::FillTruthMap(PRD_MultiTruthCollection * map, CLUSTER * cluster, TimedHitPtr<SiHit> hit){

  ATH_MSG_DEBUG("Truth map filling with cluster " << *cluster << " and link = " << hit->particleLink());
  if (hit->particleLink().isValid()){
    map->insert(std::make_pair(cluster->identify(), hit->particleLink()));
    ATH_MSG_DEBUG("Truth map filled with cluster " << *cluster << " and link = " << hit->particleLink());
  }else{
    ATH_MSG_DEBUG("Particle link NOT valid!! Truth map NOT filled with cluster" << cluster << " and link = " << hit->particleLink());
  }

  return StatusCode::SUCCESS;
}

StatusCode SiSmearedDigitizationTool::mergeEvent(){
  
  if (m_useCustomGeometry)
    return mergeEvent(m_planarClusterMap);
  else if (m_SmearPixel)
    return mergeEvent(m_pixelClusterMap);
  else
    return mergeEvent(m_sctClusterMap);
}

template<typename CLUSTER>
double SiSmearedDigitizationTool::calculateDistance(CLUSTER * clusterA, CLUSTER * clusterB){

  // take needed information on the first clusters
  Amg::Vector2D intersection_a = clusterA->localPosition();
  
  // take needed information on the second clusters
  Amg::Vector2D intersection_b = clusterB->localPosition();
  
  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: intersection_a = " << intersection_a);
  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: intersection_b = " << intersection_b);
	
  double distX = intersection_a.x() - intersection_b.x();
  double distY = intersection_a.y() - intersection_b.y();
	
  return sqrt(distX*distX + distY*distY);
}

template<typename CLUSTER>
double SiSmearedDigitizationTool::calculateSigma(CLUSTER * clusterA, CLUSTER * clusterB){
  // take needed information on the first cluster
  Amg::MatrixX clusterErr_a = clusterA->localCovariance();
  
  // take needed information on the second clusters
  Amg::MatrixX clusterErr_b = clusterB->localCovariance();
  
  double sigmaX = sqrt(Amg::error(clusterErr_a,Trk::locX) * Amg::error(clusterErr_a,Trk::locX) +
		       Amg::error(clusterErr_b,Trk::locX) * Amg::error(clusterErr_b,Trk::locX));
  
  double sigmaY = sqrt(Amg::error(clusterErr_a,Trk::locY) * Amg::error(clusterErr_a,Trk::locY) +
		       Amg::error(clusterErr_b,Trk::locY) * Amg::error(clusterErr_b,Trk::locY));
  
  return sqrt(sigmaX*sigmaX + sigmaY*sigmaY);
}

template<typename CLUSTER>
ClusterInfo * SiSmearedDigitizationTool::calculateNewCluster(CLUSTER * clusterA, CLUSTER * clusterB) {
  // take needed information on the first clusters
  Amg::Vector2D intersection_a = clusterA->localPosition();
  Amg::MatrixX clusterErr_a = clusterA->localCovariance();
  
  // take needed information on the second clusters
  Amg::Vector2D intersection_b = clusterB->localPosition();
  Amg::MatrixX clusterErr_b = clusterB->localCovariance();
  
  double sigmaX = sqrt(Amg::error(clusterErr_a,Trk::locX) * Amg::error(clusterErr_a,Trk::locX) +
		       Amg::error(clusterErr_b,Trk::locX) * Amg::error(clusterErr_b,Trk::locX));
  
  double sigmaY = sqrt(Amg::error(clusterErr_a,Trk::locY) * Amg::error(clusterErr_a,Trk::locY) +
		       Amg::error(clusterErr_b,Trk::locY) * Amg::error(clusterErr_b,Trk::locY));

  double interX = 0.5*(intersection_a.x()+intersection_b.x());
  double interY = 0.5*(intersection_a.y()+intersection_b.y());
  
  Amg::Vector2D* intersection = new Amg::Vector2D(interX, interY);
  
  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: intersection = " << *intersection);

  InDet::SiWidth siWidth_a = clusterA->width();
  InDet::SiWidth siWidth_b = clusterB->width();

  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: siWidth_a = " << siWidth_a);
  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: siWidth_b = " << siWidth_b);
  
  Amg::Vector2D colRow = siWidth_a.colRow() + siWidth_b.colRow();
  Amg::Vector2D  phiRz = siWidth_a.widthPhiRZ() + siWidth_b.widthPhiRZ();
  
  InDet::SiWidth * siWidth = new InDet::SiWidth(colRow, phiRz);  
  
  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: siWidth = " << *siWidth);

  AmgSymMatrix(2) covariance;
  covariance.setIdentity();
  covariance(Trk::locX,Trk::locX) = sigmaX*sigmaX;
  covariance(Trk::locY,Trk::locY) = sigmaY*sigmaY;
  Amg::MatrixX* clusterErr = new Amg::MatrixX(covariance);

  return new ClusterInfo(intersection, siWidth, clusterErr);

}

StatusCode SiSmearedDigitizationTool::mergeEvent(Planar_detElement_RIO_map * cluster_map)
{
  // The idea is first to check how many cluster we have to merge and then merge them.
  // The loop is done until there aren't any clusters to merge
  
  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: in mergeEvent() ---" );
  
  Planar_detElement_RIO_map::iterator i = cluster_map->begin();
  Planar_detElement_RIO_map::iterator e = cluster_map->end();

  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: cluster_map size --- " << cluster_map->size() );
  
  for (; i != e; i = cluster_map->upper_bound(i->first)){
    IdentifierHash current_id = i->first;
    // Check if clusters with current_id have been already considered

    ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: " << cluster_map->count(current_id) << "elements with key " << current_id );
    
    bool NewMerge = true;
    
    while (NewMerge) {
      NewMerge = false;
      std::pair <Planar_detElement_RIO_map::iterator, Planar_detElement_RIO_map::iterator> range = cluster_map->equal_range(current_id);
      for ( Planar_detElement_RIO_map::iterator iter = range.first; iter != range.second;  ++iter){
	
	ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: outer loop!");
	
	for ( Planar_detElement_RIO_map::iterator inner_iter = std::next(iter); inner_iter != range.second; ++inner_iter){
	  
	  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: inner loop!");
	  
	  double dist = calculateDistance((*iter).second,(*inner_iter).second);
	  
	  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: dist = " << dist);
	  
	  double sigma = calculateSigma((*iter).second,(*inner_iter).second);
	  
	  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: sigma = " << sigma);
	  
	  if( dist <= m_nSigma * sigma) {
	    
	    std::vector<Identifier> rdoList;
	    
	    ClusterInfo * clusterInfo = calculateNewCluster((*iter).second,(*inner_iter).second);
	    
	    Amg::Vector2D* intersection = std::get<0>(*clusterInfo);
	    InDet::SiWidth* siWidth = std::get<1>(*clusterInfo);
	    Amg::MatrixX* clusterErr = std::get<2>(*clusterInfo);
	    
	    const iFatras::PlanarDetElement* hitPlanarDetElement = (((*inner_iter).second)->detectorElement());
	    
	    std::pair<int, int> intersectionXY = hitPlanarDetElement->cellsOfPosition(*intersection);
	    Identifier intersectionId = m_pixel_ID->pixel_id(hitPlanarDetElement->identify(), intersectionXY.first, intersectionXY.second);
	    rdoList.push_back(intersectionId);
	    
	    ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: intersectionId = " << intersectionId);
	    
	    InDetDD::SiCellId currentCellId = InDetDD::SiCellId(m_pixel_ID->phi_index(intersectionId) , m_pixel_ID->eta_index(intersectionId));
	    
	    if (!currentCellId.isValid()) continue;
	    
	    iFatras::PlanarCluster* planarCluster = new iFatras::PlanarCluster(intersectionId,
									       *intersection,
									       rdoList,
									       *siWidth,   
									       hitPlanarDetElement,
									       clusterErr);
	    
	    ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: merged planar cluster = " << *planarCluster);
	    
	    ((*inner_iter).second) = planarCluster;
	    
	    cluster_map->erase(iter);
	    NewMerge = true;
	    goto REPEAT_LOOP;
	  }
	}
      }
    REPEAT_LOOP: ;
    }
  }
    
  i = cluster_map->begin();
  e = cluster_map->end();
  
  for (; i != e; i = cluster_map->upper_bound(i->first)){
    IdentifierHash current_id = i->first;
    // Check if clusters with current_id have been already considered
    
    ATH_MSG_DEBUG( "--- AFTER MERGING SiSmearedDigitizationTool: " << cluster_map->count(current_id) << "elements with key " << current_id );
    
    std::pair <Planar_detElement_RIO_map::iterator, Planar_detElement_RIO_map::iterator> range = cluster_map->equal_range(current_id);
    for ( Planar_detElement_RIO_map::iterator iter = range.first; iter != range.second; ++iter){
      ATH_MSG_DEBUG( "--- AFTER MERGING SiSmearedDigitizationTool: Planar Cluster = " << *((*iter).second) );
    }
  }
  
  return StatusCode::SUCCESS;
}

StatusCode SiSmearedDigitizationTool::mergeEvent(Pixel_detElement_RIO_map * cluster_map)
{
  // The idea is first to check how many cluster we have to merge and then merge them.
  // The loop is done until there aren't any clusters to merge
  
  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: in mergeEvent() ---" );
  
  Pixel_detElement_RIO_map::iterator i = cluster_map->begin();
  Pixel_detElement_RIO_map::iterator e = cluster_map->end();

  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: cluster_map size --- " << cluster_map->size() );
  
  for (; i != e; i = cluster_map->upper_bound(i->first)){
    IdentifierHash current_id = i->first;
    // Check if clusters with current_id have been already considered

    ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: " << cluster_map->count(current_id) << "elements with key " << current_id );
    bool NewMerge = true;
    
    while (NewMerge) {
      NewMerge = false;
      std::pair <Pixel_detElement_RIO_map::iterator, Pixel_detElement_RIO_map::iterator> range = cluster_map->equal_range(current_id);
      for ( Pixel_detElement_RIO_map::iterator iter = range.first; iter != range.second; ++iter){
	
	ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: outer loop!");
	
	for ( Pixel_detElement_RIO_map::iterator inner_iter = std::next(iter); inner_iter != range.second; ++inner_iter){
	  
	  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: inner loop!");
	  
	  double dist = calculateDistance((*iter).second,(*inner_iter).second);
	  
	  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: dist = " << dist);
	  
	  double sigma = calculateSigma((*iter).second,(*inner_iter).second);
	  
	  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: sigma = " << sigma);
	  
	  if( dist <= m_nSigma * sigma) {
	    
	    std::vector<Identifier> rdoList;
	    
	    ClusterInfo * clusterInfo = calculateNewCluster((*iter).second,(*inner_iter).second);
	    
	    Amg::Vector2D* intersection = std::get<0>(*clusterInfo);
	    InDet::SiWidth* siWidth = std::get<1>(*clusterInfo);
	    Amg::MatrixX* clusterErr = std::get<2>(*clusterInfo);
	    
	    const InDetDD::SiDetectorElement* hitSiDetElement = (((*inner_iter).second)->detectorElement());
	    
	    Identifier intersectionId = hitSiDetElement->identifierOfPosition(*intersection);
	    
	    rdoList.push_back(intersectionId);
	    
	    ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: intersectionId = " << intersectionId);
	    
	    InDetDD::SiCellId currentCellId = hitSiDetElement->cellIdFromIdentifier(intersectionId);
	    
	    if (!currentCellId.isValid()) continue;
	    
	    InDet::PixelCluster* pixelCluster = new InDet::PixelCluster(intersectionId,
									*intersection,
									rdoList,
									*siWidth,					   
									hitSiDetElement,
									clusterErr);
	    
	    ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: merged pixel cluster = " << *pixelCluster);
	    
	    ((*inner_iter).second) = pixelCluster;
	    
	    cluster_map->erase(iter);
	    NewMerge = true;
	    goto REPEAT_LOOP;
	  }
	}
      }
    REPEAT_LOOP: ;
    }
  }
  
  i = cluster_map->begin();
  e = cluster_map->end();
  
  for (; i != e; i = cluster_map->upper_bound(i->first)){
    IdentifierHash current_id = i->first;
    // Check if clusters with current_id have been already considered

    ATH_MSG_DEBUG( "--- AFTER MERGING SiSmearedDigitizationTool: " << cluster_map->count(current_id) << "elements with key " << current_id );
    
    std::pair <Pixel_detElement_RIO_map::iterator, Pixel_detElement_RIO_map::iterator> range = cluster_map->equal_range(current_id);
    for ( Pixel_detElement_RIO_map::iterator iter = range.first; iter != range.second; ++iter){
      ATH_MSG_DEBUG( "--- AFTER MERGING SiSmearedDigitizationTool: Pixel Cluster = " << *((*iter).second) );
    }
  }
  
  return StatusCode::SUCCESS;
}

StatusCode SiSmearedDigitizationTool::mergeEvent(SCT_detElement_RIO_map * cluster_map)
{
  // The idea is first to check how many cluster we have to merge and then merge them.
  // The loop is done until there aren't any clusters to merge
  
  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: in mergeEvent() ---" );
  
  SCT_detElement_RIO_map::iterator i = cluster_map->begin();
  SCT_detElement_RIO_map::iterator e = cluster_map->end();

  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: cluster_map size --- " << cluster_map->size() );
  
  for (; i != e; i = cluster_map->upper_bound(i->first)){
    IdentifierHash current_id = i->first;
    // Check if clusters with current_id have been already considered

    ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: " << cluster_map->count(current_id) << "elements with key " << current_id );
    bool NewMerge = true;
    
    while (NewMerge) {
      NewMerge = false;
      std::pair <SCT_detElement_RIO_map::iterator, SCT_detElement_RIO_map::iterator> range = cluster_map->equal_range(current_id);
      for (SCT_detElement_RIO_map::iterator iter = range.first; iter != range.second; ++iter){
	
	ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: outer loop!");
	
	for ( SCT_detElement_RIO_map::iterator inner_iter = std::next(iter); inner_iter != range.second; ++inner_iter){
	  
	  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: inner loop!");
	  
	  double dist = calculateDistance((*iter).second,(*inner_iter).second);
	  
	  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: dist = " << dist);
	  
	  double sigma = calculateSigma((*iter).second,(*inner_iter).second);
	  
	  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: sigma = " << sigma);
	  
	  if( dist <= m_nSigma * sigma) {
	    
	    std::vector<Identifier> rdoList;
	    
	    ClusterInfo * clusterInfo = calculateNewCluster((*iter).second,(*inner_iter).second);
	    
	    Amg::Vector2D* intersection = std::get<0>(*clusterInfo);
	    InDet::SiWidth* siWidth = std::get<1>(*clusterInfo);
	    Amg::MatrixX* clusterErr = std::get<2>(*clusterInfo);
	    
	    const InDetDD::SiDetectorElement* hitSiDetElement = (((*inner_iter).second)->detectorElement());
	    
	    Identifier intersectionId = hitSiDetElement->identifierOfPosition(*intersection);
	    
	    rdoList.push_back(intersectionId);
	    
	    ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: intersectionId = " << intersectionId);
	    
	    InDetDD::SiCellId currentCellId = hitSiDetElement->cellIdFromIdentifier(intersectionId);
	    
	    if (!currentCellId.isValid()) continue;
	    
	    InDet::SCT_Cluster* sctCluster = new InDet::SCT_Cluster(intersectionId,
								    *intersection,
								    rdoList,
								    *siWidth,					   
								    hitSiDetElement,
								    clusterErr);
	    
	    ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: merged sct cluster = " << *sctCluster);
	    
	    ((*inner_iter).second) = sctCluster;
	    
	    cluster_map->erase(iter);
	    NewMerge = true;
	    goto REPEAT_LOOP;
	  }
	}
      }
    REPEAT_LOOP: ;
    }
  }
  
  i = cluster_map->begin();
  e = cluster_map->end();
  
  for (; i != e; i = cluster_map->upper_bound(i->first)){
    IdentifierHash current_id = i->first;
    // Check if clusters with current_id have been already considered

    ATH_MSG_DEBUG( "--- AFTER MERGING SiSmearedDigitizationTool: " << cluster_map->count(current_id) << "elements with key " << current_id );
    
    std::pair <SCT_detElement_RIO_map::iterator, SCT_detElement_RIO_map::iterator> range = cluster_map->equal_range(current_id);
    for ( SCT_detElement_RIO_map::iterator iter = range.first; iter != range.second; ++iter){
      ATH_MSG_DEBUG( "--- AFTER MERGING SiSmearedDigitizationTool: SCT Cluster = " << *((*iter).second) );
    }
  }
  
  return StatusCode::SUCCESS;
}
  

StatusCode SiSmearedDigitizationTool::digitize() 
{
  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: in SiSmearedDigizationTool::digitize() ---" );

  if(m_SmearPixel){ // Smear Pixel
    if ((detStore()->retrieve(m_trackingGeometry, m_trackingGeometryName)).isFailure()) {
      ATH_MSG_FATAL( "Could not retrieve TrackingGeometry '" << m_trackingGeometryName << "' from DetectorStore." );
      return StatusCode::FAILURE;
    }
    else
      ATH_MSG_INFO( "TrackingGeometry '" << m_trackingGeometryName << "' successfully retrieved from DetectorStore." );
     
    m_detElementMap = new iFatras::IdHashDetElementCollection;
    //Retrieve and/or store the map with IdHash to DetElement 
    if ((detStore()->contains<iFatras::IdHashDetElementCollection>(m_detElementMapName))){
      if((detStore()->retrieve(m_detElementMap, m_detElementMapName)).isFailure()){
	ATH_MSG_FATAL("Could not retrieve collection " << m_detElementMapName);
	return StatusCode::FAILURE;
      }
      else
	ATH_MSG_DEBUG("Found and Retrieved collection " << m_detElementMapName);
    }
           
  }   
   
  TimedHitCollection<SiHit>::const_iterator i, e;
   
  if(m_useCustomGeometry) 
    m_planarClusterMap = new Planar_detElement_RIO_map;
  else {
    if(m_SmearPixel){ // Smear Pixel
      m_pixelClusterMap = new Pixel_detElement_RIO_map;
    }else{ // Smear SCT
      m_sctClusterMap = new SCT_detElement_RIO_map;
    }
  }

  while (m_thpcsi->nextDetectorElement(i, e)) {
    
    while (i != e) {
     
      TimedHitPtr<SiHit> hit(*i++);
      int barrelEC = hit->getBarrelEndcap();
      int layerDisk = hit->getLayerDisk();
      int phiModule = hit->getPhiModule();
      int etaModule = hit->getEtaModule();

      //int trkn = hit->trackNumber();
      //const HepMcParticleLink McLink = HepMcParticleLink(trkn,hit.eventId());
      //const HepMC::GenParticle* genPart= McLink.cptr();

      const InDetDD::SiDetectorElement* hitSiDetElement;
      const iFatras::PlanarDetElement* hitPlanarDetElement = 0;
     
      if(m_SmearPixel){ // Smear Pixel
	if (!m_useCustomGeometry) {
	  const InDetDD::SiDetectorElement* hitSiDetElement_temp = m_manager_pix->getDetectorElement(barrelEC,layerDisk,phiModule,etaModule);
	  ATH_MSG_DEBUG("Pixel SiDetectorElement --> barrel_ec " << barrelEC << ", layer_disk " << layerDisk << ", phi_module " << phiModule << ", eta_module " << etaModule ); 
	  hitSiDetElement = hitSiDetElement_temp;}
	else {
	  Identifier idwafer = m_pixel_ID->wafer_id(barrelEC,layerDisk,phiModule,etaModule);
	  IdentifierHash idhash = m_pixel_ID->wafer_hash(m_pixel_ID->wafer_id(idwafer));
	  iFatras::IdHashDetElementCollection::iterator it_map = m_detElementMap->find(idhash);
	  if (it_map == m_detElementMap->end()) 
	    ATH_MSG_WARNING("Id hash " << idhash << " not found in the map from id hash to planar detector element.");
	  else{
	    ATH_MSG_DEBUG("Id hash " << idhash << " found in the map.");
	    const iFatras::PlanarDetElement* hitPlanarDetElement_temp = it_map->second;
	    ATH_MSG_DEBUG("Pixel PlanarDetElement --> barrel_ec " << barrelEC << ", layer_disk " << layerDisk << ", phi_module " << phiModule << ", eta_module " << etaModule ); 
	    hitPlanarDetElement = hitPlanarDetElement_temp;
	    ATH_MSG_DEBUG("Surface " << hitPlanarDetElement->surface());
	   
	  }
	}      
      }
      else{ // Smear SCT
	int side = hit->getSide();
	const InDetDD::SiDetectorElement* hitSiDetElement_temp = m_manager_sct->getDetectorElement(barrelEC,layerDisk,phiModule,etaModule,side);
	ATH_MSG_DEBUG("SCT SiDetectorElement --> barrel_ec " << barrelEC << ", layer_disk " << layerDisk << ", phi_module " << phiModule << ", eta_module " << etaModule << ", side " << side); 
	hitSiDetElement = hitSiDetElement_temp;
      }

      if(m_SmearPixel){ // Smear Pixel
	if (!hitSiDetElement && !hitPlanarDetElement) {ATH_MSG_ERROR( " could not get detector element for both SiDetElement and PlanarDetElement"); continue;} 
	else if(!hitSiDetElement && !m_useCustomGeometry) {ATH_MSG_ERROR( " could not get detector element for SiDetElement"); continue;}
	else if(!hitPlanarDetElement && m_useCustomGeometry) {ATH_MSG_ERROR( " could not get detector element for PlanarDetElement"); continue;}
      }
      else
	if (!hitSiDetElement) {ATH_MSG_ERROR( " could not get detector element for SiDetElement"); continue;}
     
      if (m_SmearPixel && !m_useCustomGeometry && !(hitSiDetElement->isPixel())) continue;
     
      if (m_SmearPixel && m_useCustomGeometry && !(hitPlanarDetElement->isPixel())) continue;
     
      IdentifierHash waferID;
     
      if (!m_useCustomGeometry)
	if(m_SmearPixel){ // Smear Pixel
	  waferID = m_pixel_ID->wafer_hash(hitSiDetElement->identify());
	}else{ // Smear SCT
	  waferID = m_sct_ID->wafer_hash(hitSiDetElement->identify());
	}
      else
	if(m_SmearPixel){ // Smear Pixel
	  waferID = m_pixel_ID->wafer_hash(hitPlanarDetElement->identify());
	}else{ // Smear SCT
	  waferID = m_sct_ID->wafer_hash(hitPlanarDetElement->identify());
	}
     
      Amg::Vector3D localStartPosition = (Amg::Vector3D)hit->localStartPosition();
      Amg::Vector3D localEndPosition   = (Amg::Vector3D)hit->localEndPosition();
     
      double localEntryX = localStartPosition.y();
      double localEntryY = localStartPosition.z();
      double localEntryZ = localStartPosition.x();
      double localExitX = localEndPosition.y();
      double localExitY = localEndPosition.z();
      double localExitZ = localEndPosition.x();
     
      double thickness = 0.0;
     
      if (!m_useCustomGeometry)
	thickness = hitSiDetElement->thickness();
      else
	thickness = hitPlanarDetElement->thickness();
     
      // Transform to reconstruction local coordinates (different x,y,z ordering and sign conventions compared to simulation coords)
      if (!m_SmearPixel) { // Smear SCT
       
// 	double hitDepth  = hitSiDetElement->hitDepthDirection();
// 	double hitPhi    = hitSiDetElement->hitPhiDirection();
// 	double hitEta    = hitSiDetElement->hitEtaDirection();

	HepGeom::Point3D<double> sct_localStartPosition = hit->localStartPosition();
	HepGeom::Point3D<double> sct_localEndPosition = hit->localEndPosition();
// 	
	sct_localStartPosition = hitSiDetElement->hitLocalToLocal3D(sct_localStartPosition);
	sct_localEndPosition = hitSiDetElement->hitLocalToLocal3D(sct_localEndPosition);
	
	localEntryX = sct_localStartPosition.x();
	localEntryY = sct_localStartPosition.y();
	localEntryZ = sct_localStartPosition.z();
	localExitX  = sct_localEndPosition.x();
	localExitY  = sct_localEndPosition.y();
	localExitZ  = sct_localEndPosition.z();
	
	// localEntryX = localStartPosition.y();
	// localEntryY = localStartPosition.z();
	// localEntryZ = localStartPosition.x();
	// localExitX  = localEndPosition.y();
	// localExitY  = localEndPosition.z();
	// localExitZ  = localEndPosition.x();
       
	// localEntryX *= hitPhi;
	// localExitX *= hitPhi;
	// localEntryY *= hitEta;
	// localExitY *= hitEta;
	// localEntryZ *= hitDepth;
	// localExitZ *= hitDepth;
      }
     
      double distX = localExitX-localEntryX;
      double distY = localExitY-localEntryY;
     
      if (distX<0)
	distX=-distX;
      if (distY<0)
	distY=-distY;
     
      if(m_SmearPixel){ // Smear Pixel
	ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: pixel start position --- " << localEntryX << ",  " << localEntryY << ",  " << localEntryZ );
	ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: pixel exit position --- " << localExitX << ",  " << localExitY << ",  " << localExitZ );
	m_x_entry_pixel = localEntryX;
	m_y_entry_pixel = localEntryY;
	m_z_entry_pixel = localEntryZ;
	m_x_exit_pixel = localExitX;
	m_y_exit_pixel = localExitY;
	m_z_exit_pixel = localExitZ;
      }else{ // Smear SCT
	ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: SCT start position --- " << localEntryX << ",  " << localEntryY << ",  " << localEntryZ );
	ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: SCT exit position --- " << localExitX << ",  " << localExitY << ",  " << localExitZ );
	m_x_entry_SCT = localEntryX;
	m_y_entry_SCT = localEntryY;
	m_z_entry_SCT = localEntryZ;
	m_x_exit_SCT = localExitX;
	m_y_exit_SCT = localExitY;
	m_z_exit_SCT = localExitZ;
      }
     
      Amg::Vector2D localEntry(localEntryX,localEntryY);
      Amg::Vector2D localExit(localExitX,localExitY);
      Amg::Vector3D localExit3D(localExitX,localExitY,localExitZ);
     
      // the pixel positions and other needed stuff for the geometrical clustering
      std::vector<Identifier>           rdoList;
            
      Amg::Vector3D localDirection(localExitX-localEntryX, localExitY-localEntryY, localExitZ-localEntryZ);
     
      InDetDD::SiCellId entryCellId;
      InDetDD::SiCellId exitCellId;
     
      if(!m_useCustomGeometry) {
	// get the identifier of the entry and the exit
	Identifier entryId = hitSiDetElement->identifierOfPosition(localEntry);
	Identifier exitId  = hitSiDetElement->identifierOfPosition(localExit);

	// now get the cellIds and check whether they're valid
	entryCellId = hitSiDetElement->cellIdFromIdentifier(entryId);
	exitCellId  = hitSiDetElement->cellIdFromIdentifier(exitId);

	ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: entryId " << entryId << " --- exitId " << exitId );
	ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: entryCellId " << entryCellId << " --- exitCellId " << exitCellId );

	ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: surface " << hitSiDetElement->surface());
	
      } 
      else {
	// get the identifier of the entry and the exit
	std::pair<int, int> entryXY = hitPlanarDetElement->cellsOfPosition(localEntry);	
	Identifier entryId = m_pixel_ID->pixel_id(barrelEC, layerDisk, phiModule, etaModule, entryXY.first, entryXY.second);
	// now get the cellIds and check whether they're valid
	if (hitPlanarDetElement->isLastCellIdValid()) 
	  entryCellId = InDetDD::SiCellId(m_pixel_ID->phi_index(entryId), m_pixel_ID->eta_index(entryId));
	else
	  entryCellId = InDetDD::SiCellId();
	
	std::pair<int, int> exitXY = hitPlanarDetElement->cellsOfPosition(localExit);
	Identifier exitId  = m_pixel_ID->pixel_id(barrelEC, layerDisk, phiModule, etaModule, exitXY.first, exitXY.second);
	// now get the cellIds and check whether they're valid
	if (hitPlanarDetElement->isLastCellIdValid()) 
	  exitCellId  = InDetDD::SiCellId(m_pixel_ID->phi_index(exitId) , m_pixel_ID->eta_index(exitId));
	else
	  exitCellId = InDetDD::SiCellId();
	  
	ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: entryId " << entryId << " --- exitId " << exitId );
	ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: entryCellId " << entryCellId << " --- exitCellId " << exitCellId );
	
      }
     
      // entry / exit validity    
      bool entryValid = entryCellId.isValid();
      bool exitValid  = exitCellId.isValid();

      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: entryValid? " << entryValid << " --- exitValid? " << exitValid );
     
      if (!entryValid && !exitValid) continue;
     
      // the intersecetion id and cellId of it
      double interX = 0.5*(localEntryX+localExitX);
      double interY = 0.5*(localEntryY+localExitY);
     
      if(m_SmearPixel){ // Smear Pixel
	ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: pixel inter X --- " << interX );
	ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: pixel inter Y --- " << interY );
	ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: pixel dist X --- " << distX );
	ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: pixel dist Y --- " << distY );
      }else{ // Smear SCT
	ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: SCT inter X --- " << interX );
	ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: SCT inter Y --- " << interY );
      }
     
      //the particle crosses at least n pixels (in x direction you have timesX, in y direction you have timesY)
      double timesY = (m_pitch_Y) ? floor(distY/m_pitch_Y) : 0;
      double timesX = (m_pitch_X) ? floor(distX/m_pitch_X) : 0;
     
      double newdistY = distY - (timesY*m_pitch_Y);
      double newdistX = distX - (timesX*m_pitch_X);
     
      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: pixel times X --- " << timesX );
      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: pixel times Y --- " << timesY );
      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: pixel new dist X --- " << newdistX );
      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: pixel new dist Y --- " << newdistY );
     
      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: pixel thickness --- " << thickness );
    
      //Probability
     
      double ProbY = 2*newdistY/(m_pitch_Y+newdistY);
      double ProbX = 2*newdistX/(m_pitch_X+newdistX);
     
      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: pixel ProbX --- " << ProbX );
      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: pixel ProbY --- " << ProbY );
     
      // create the errors
      double sigmaX = m_pitch_X/sqrt(12.);
      double sigmaY = m_pitch_Y/sqrt(12.);
     
      if(m_SmearPixel){
      
	if (timesY>0){
	  if (CLHEP::RandFlat::shoot(m_randomEngine, 0.0, 1.0) < ProbY){
	    sigmaY = (double)(timesY+2)*m_pitch_Y/sqrt(12.);
	    timesY+=2;}
	  else{
	    sigmaY = (double)(timesY+1)*m_pitch_Y/sqrt(12.);
	    timesY+=1;}
	}
	if (timesX>0){
	  if (CLHEP::RandFlat::shoot(m_randomEngine, 0.0, 1.0) < ProbX){
	    sigmaX = (double)(timesX+2)*m_pitch_X/sqrt(12.);
	    timesX+=2;}
	  else{
	    sigmaX = (double)(timesX+1)*m_pitch_X/sqrt(12.);
	    timesX+=1;}
	}
      }
     
      if(m_SmearPixel){ // Smear Pixel
	ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: sigma X --- " << sigmaX);
	ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: sigma Y --- " << sigmaY);
      }else{ // Smear SCT
	ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: sigma X --- " << sigmaX);
      }
    
      double temp_X = interX;
      double temp_Y = interY;
     
      // Smear intersection 
      // create the smdar parameter
    
      if (sigmaX != 0.){
	double sParX = CLHEP::RandGauss::shoot(m_randomEngine, 0., sigmaX);
	ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: extracted gaussian value for X --- " << sParX);
	interX += sParX;  
      }
     
      if(m_SmearPixel){ // Smear Pixel also in y direction
       
	if (sigmaY != 0.)
	  {
	    double sParY = CLHEP::RandGauss::shoot(m_randomEngine, 0., sigmaY);
	    ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: extracted gaussian value for Y --- " << sParY);
	    interY += sParY;
	  }
      
      }else{ // Smear SCT --> Fix y coordinate to 0
	interY = 0;
      }
        
     
      // Define the current smeared center position
      Amg::Vector2D intersection(interX,interY);
     
      if(m_SmearPixel)
	ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: Intersection after smearing for Pixel: " << intersection);
      else
	ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: Intersection after smearing for SCT: " << intersection);
    
      Identifier intersectionId;
      if (!m_useCustomGeometry)
	intersectionId = hitSiDetElement->identifierOfPosition(intersection);
      else {
	std::pair<int, int> intersectionXY = hitPlanarDetElement->cellsOfPosition(intersection);
	// get the identifier of the entry and the exit
	intersectionId = m_pixel_ID->pixel_id(barrelEC, layerDisk, phiModule, etaModule, intersectionXY.first, intersectionXY.second);
      }
      rdoList.push_back(intersectionId);
    
      InDetDD::SiCellId currentCellId = (m_useCustomGeometry) ? InDetDD::SiCellId(m_pixel_ID->phi_index(intersectionId) , m_pixel_ID->eta_index(intersectionId)) : hitSiDetElement->cellIdFromIdentifier(intersectionId);
      if (!currentCellId.isValid()) continue;
    
      if(m_SmearPixel){ // Smear Pixel --> Create Pixel Cluster

	if(timesX == 0 && timesY == 0)
	  { timesX = 1; timesY = 1; }
	
	double lengthX = (m_pitch_X) ? timesX*m_pitch_X : 0.8;
	double lengthY = (m_pitch_Y) ? timesY*m_pitch_Y : 0.8;
	
	InDet::SiWidth* siWidth = new InDet::SiWidth(Amg::Vector2D(timesX,timesY), Amg::Vector2D(lengthX, lengthY));  
      
	InDet::PixelCluster* pixelCluster = 0;
	iFatras::PlanarCluster* planarCluster = 0;

	AmgSymMatrix(2) covariance;
	covariance.setIdentity();
	covariance(Trk::locX,Trk::locX) = sigmaX*sigmaX;
	covariance(Trk::locY,Trk::locY) = sigmaY*sigmaY;
	Amg::MatrixX* clusterErr = new Amg::MatrixX(covariance);
      
	// create the cluster
	if (!m_useCustomGeometry) {
	  pixelCluster = new InDet::PixelCluster(intersectionId,
						 intersection,
						 rdoList,
						 *siWidth,					   
						 hitSiDetElement,
						 clusterErr);
	  m_pixelClusterMap->insert(std::pair<IdentifierHash, const InDet::PixelCluster* >(waferID, pixelCluster));

	  if (FillTruthMap(m_pixelPrdTruth, pixelCluster, hit).isFailure()) {
	    ATH_MSG_FATAL ( "FillTruthMap() for pixel failed!" );
	    return StatusCode::FAILURE;
	  }
	  
	} else {
	  planarCluster = new iFatras::PlanarCluster(intersectionId,
						     intersection,
						     rdoList,
						     *siWidth,					   
						     hitPlanarDetElement,
						     clusterErr);
	  
	  Planar_detElement_RIO_map::iterator it_map = m_planarClusterMap->find(waferID);

	  m_planarClusterMap->insert(std::pair<IdentifierHash, const iFatras::PlanarCluster* >(waferID, planarCluster));
	  
	  if (FillTruthMap(m_planarPrdTruth, planarCluster, hit).isFailure()) {
	    ATH_MSG_FATAL ( "FillTruthMap() for planar element failed!" );
	    return StatusCode::FAILURE;
	  }

	} 
      
	if (m_checkSmear)
	  {

	    if (m_useCustomGeometry)
	      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: planarCluster --> " << *planarCluster);
	    else 
	      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: pixelCluster --> " << *pixelCluster);
	    //Take info to store in the tree
	    m_x_pixel = temp_X;
	    m_y_pixel = temp_Y;

	    m_x_pixel_smeared = m_useCustomGeometry ? (planarCluster->localPosition()).x() : (pixelCluster->localPosition()).x();
	    m_y_pixel_smeared = m_useCustomGeometry ? (planarCluster->localPosition()).y() : (pixelCluster->localPosition()).y();
	  
	    ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: BEFORE SMEARING LocalPosition --> X = " << m_x_pixel << "  Y = " << m_y_pixel);
	    ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: LocalPosition --> X = " << m_x_pixel_smeared << "  Y = " << m_y_pixel_smeared);
	    if (!m_useCustomGeometry) {
	      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: GlobalPosition --> X = " << (pixelCluster->globalPosition()).x() << "  Y = " << (pixelCluster->globalPosition()).y());
	      m_x_pixel_global = (pixelCluster->globalPosition()).x();
	      m_y_pixel_global = (pixelCluster->globalPosition()).y();
	      m_z_pixel_global = (pixelCluster->globalPosition()).z();
				
	    } else {
	      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: GlobalPosition --> X = " << (planarCluster->globalPosition()).x() << "  Y = " << (planarCluster->globalPosition()).y());
	      m_x_pixel_global = (planarCluster->globalPosition()).x();
	      m_y_pixel_global = (planarCluster->globalPosition()).y();
	      m_z_pixel_global = (planarCluster->globalPosition()).z();
	    }	   
	  
	    if (!m_useCustomGeometry) {
	      m_Err_x_pixel = Amg::error(pixelCluster->localCovariance(), Trk::locX);
	      m_Err_y_pixel = Amg::error(pixelCluster->localCovariance(), Trk::locY);
	    } else {
	      m_Err_x_pixel = Amg::error(planarCluster->localCovariance(), Trk::locX);
	      m_Err_y_pixel = Amg::error(planarCluster->localCovariance(), Trk::locY);
	    }

	    ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: Error --> X = " << m_Err_x_pixel << "  Y = " << m_Err_y_pixel);
	
	    m_currentTree -> Fill();
	  
	  }
      
	      
      }else{ // Smear SCT --> Create SCT Cluster

	// Pixel Design needed -------------------------------------------------------------
	InDetDD::SCT_ModuleSideDesign* design_sct;
	
	design_sct = (InDetDD::SCT_ModuleSideDesign*)&hitSiDetElement->design();
  
	if (!design_sct) {
	  ATH_MSG_INFO ( "Could not get design"<< design_sct) ;
	  continue;
	}
      
	// prepare & create the siWidth
	InDet::SCT_Cluster* sctCluster = 0;
	// Find length of strip at centre
	double clusterWidth = rdoList.size()*hitSiDetElement->phiPitch(intersection); 
	const std::pair<InDetDD::SiLocalPosition, InDetDD::SiLocalPosition> ends(design_sct->endsOfStrip(intersection));
	double stripLength = fabs(ends.first.xEta()-ends.second.xEta());
	InDet::SiWidth* siWidth = new InDet::SiWidth(Amg::Vector2D(int(rdoList.size()),1),
						     Amg::Vector2D(clusterWidth,stripLength) ); 

	const Amg::Vector2D& colRow = siWidth->colRow();
	AmgSymMatrix(2) mat;
	mat.setIdentity();
	mat(Trk::locX,Trk::locX) = sigmaX*sigmaX;
	mat(Trk::locY,Trk::locY) = hitSiDetElement->length()*hitSiDetElement->length()/12.;

	// single strip - resolution close to pitch/sqrt(12)
	// two-strip hits: better resolution, approx. 40% lower
      
	if(m_emulateAtlas && hitSiDetElement->design().shape() == InDetDD::Trapezoid){// rotation for endcap SCT
	
	  if(colRow.x() == 1){
	    mat(1,1) = pow(siWidth->phiR(),2)/12;
	  }
	  else if(colRow.x() == 2){
	    mat(1,1) = pow(0.27*siWidth->phiR(),2)/12;
	  }
	  else{
	    mat(1,1) = pow(siWidth->phiR(),2)/12;
	  }
	  mat(2,2) = pow(siWidth->z()/colRow.y(),2)/12;
	
	
	  double sn      = hitSiDetElement->sinStereoLocal(intersection); 
	  double sn2     = sn*sn;
	  double cs2     = 1.-sn2;
	  double w       = hitSiDetElement->phiPitch(intersection)/hitSiDetElement->phiPitch(); 
	  double v0      = mat(1,1)*w*w;
	  double v1      = mat(2,2);
	  mat(1,1) = (cs2*v0+sn2*v1);
	  mat(2,1) = (sn*sqrt(cs2)*(v0-v1));
	  mat(2,2) = (sn2*v0+cs2*v1);
	}
     
	Amg::MatrixX* clusterErr = new Amg::MatrixX(mat);
          
	sctCluster = new InDet::SCT_Cluster(intersectionId,
					    intersection,
					    rdoList,
					    *siWidth,					   
					    hitSiDetElement,
					    clusterErr);
	if (m_checkSmear)
	  {
	    //Take info to store in the tree
	    m_x_SCT = temp_X;
 	  	  
	    m_x_SCT_smeared = (sctCluster->localPosition()).x();
	  	  
	    ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: BEFORE SMEARING LocalPosition --> X = " << m_x_SCT );
	    ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: LocalPosition --> X = " << m_x_SCT_smeared );
	    ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: GlobalPosition --> X = " << (sctCluster->globalPosition()).x() << "  Y = " << (sctCluster->globalPosition()).y());
	  
	    m_Err_x_SCT = Amg::error(sctCluster->localCovariance(), Trk::locX);

	    ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: Error --> X = " << m_Err_x_SCT );	
	  
	    m_currentTree -> Fill();
	  
	  }

	m_sctClusterMap->insert(std::pair<IdentifierHash, const InDet::SCT_Cluster* >(waferID, sctCluster));

	if (FillTruthMap(m_SCTPrdTruth, sctCluster, hit).isFailure()) {
	  ATH_MSG_FATAL ( "FillTruthMap() for SCT failed!" );
	  return StatusCode::FAILURE;
	}

      }
    
    }
  }
  
  return StatusCode::SUCCESS;
}


StatusCode SiSmearedDigitizationTool::createAndStoreRIOs() 
{
  if(m_useCustomGeometry){ // store Planar RIOs
      
    ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: in planar createAndStoreRIOs() ---" );
    
    Planar_detElement_RIO_map::iterator i = m_planarClusterMap->begin();
    Planar_detElement_RIO_map::iterator e = m_planarClusterMap->end();
    
    iFatras::PlanarClusterCollection* clusterCollection = 0;
    
    for (; i != e; i = m_planarClusterMap->upper_bound(i->first)){
      
      std::pair <Planar_detElement_RIO_map::iterator, Planar_detElement_RIO_map::iterator> range;
      range = m_planarClusterMap->equal_range(i->first);
      
      Planar_detElement_RIO_map::iterator firstDetElem;
      firstDetElem = range.first;
      //const iFatras::PlanarCluster* planarCluster = (firstDetElem->second);
      
      IdentifierHash waferID;
      waferID = firstDetElem->first;
      
      m_detElementMap = new iFatras::IdHashDetElementCollection;
      
      //Retrieve and/or store the map with IdHash to DetElement 
      if ((detStore()->contains<iFatras::IdHashDetElementCollection>(m_detElementMapName))){
	if((detStore()->retrieve(m_detElementMap, m_detElementMapName)).isFailure()){
	  ATH_MSG_FATAL("Could not retrieve collection " << m_detElementMapName);
	  return StatusCode::FAILURE;
	}
	else
	  ATH_MSG_DEBUG("Found and Retrieved collection " << m_detElementMapName);
      }
      
      ATH_MSG_DEBUG("Number of elements " << m_detElementMap->size() );

      iFatras::IdHashDetElementCollection::iterator it_map = m_detElementMap->find(waferID);
      if (it_map == m_detElementMap->end()) {
	ATH_MSG_WARNING("Id hash " << waferID << " not found in the map from id hash to planar detector element.");
	return StatusCode::FAILURE;
      }
      else ATH_MSG_DEBUG("Id hash " << waferID << " found in the map.");
      
      const iFatras::PlanarDetElement* detElement = it_map->second;

      // std::cout << "Noemi --> waferID = " << waferID << " planarCluster = " << *planarCluster << " detElement = " << detElement << " Surface = " << detElement->surface() << std::endl;
           
      clusterCollection = new iFatras::PlanarClusterCollection(waferID);
      clusterCollection->setIdentifier(detElement->identify());
      
      for ( Planar_detElement_RIO_map::iterator iter = range.first; iter != range.second; ++iter){
	
	iFatras::PlanarCluster* planarCluster = const_cast<iFatras::PlanarCluster*>((*iter).second);
	planarCluster->setHashAndIndex(clusterCollection->identifyHash(),clusterCollection->size());
	clusterCollection->push_back(planarCluster);
      }
      
      if (clusterCollection) {
	if ((m_planarClusterContainer->addCollection(clusterCollection, waferID)).isFailure()){
	  ATH_MSG_WARNING( "Could not add collection to Identifyable container !" );
	}
      }
    } // end for
    
    m_planarClusterMap->clear();
      
  } else {
    
    if(m_SmearPixel){ // Store Pixel RIOs
      
      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: in pixel createAndStoreRIOs() ---" );
      
      Pixel_detElement_RIO_map::iterator i = m_pixelClusterMap->begin();
      Pixel_detElement_RIO_map::iterator e = m_pixelClusterMap->end();
      
      InDet::PixelClusterCollection* clusterCollection = 0;
      
      for (; i != e; i = m_pixelClusterMap->upper_bound(i->first)){
	
	std::pair <Pixel_detElement_RIO_map::iterator, Pixel_detElement_RIO_map::iterator> range;
	range = m_pixelClusterMap->equal_range(i->first);
	
	Pixel_detElement_RIO_map::iterator firstDetElem;
	firstDetElem = range.first;
	//const InDet::PixelCluster* pixelCluster = (firstDetElem->second);
	
	IdentifierHash waferID;
	waferID = firstDetElem->first;
	
	const InDetDD::SiDetectorElement* detElement = m_manager_pix->getDetectorElement(waferID);
	
	clusterCollection = new InDet::PixelClusterCollection(waferID);
	clusterCollection->setIdentifier(detElement->identify());
	
	for ( Pixel_detElement_RIO_map::iterator iter = range.first; iter != range.second; ++iter){
	  
	  InDet::PixelCluster* pixelCluster = const_cast<InDet::PixelCluster*>((*iter).second);
	  pixelCluster->setHashAndIndex(clusterCollection->identifyHash(),clusterCollection->size());
	  clusterCollection->push_back(pixelCluster);
	  
	}
	
	if (clusterCollection) {
	  if ((m_pixelClusterContainer->addCollection(clusterCollection, waferID)).isFailure()){
	    ATH_MSG_WARNING( "Could not add collection to Identifyable container !" );
	  }
	}
      } // end for
      
      m_pixelClusterMap->clear();
      
    }else{ // Store SCT RIOs
      
      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: in SCT createAndStoreRIOs() ---" );
      
      SCT_detElement_RIO_map::iterator i = m_sctClusterMap->begin();
      SCT_detElement_RIO_map::iterator e = m_sctClusterMap->end();
      
      InDet::SCT_ClusterCollection* clusterCollection = 0;
      
      for (; i != e; i = m_sctClusterMap->upper_bound(i->first)){
	std::pair <SCT_detElement_RIO_map::iterator, SCT_detElement_RIO_map::iterator> range;
	range = m_sctClusterMap->equal_range(i->first);
	
	SCT_detElement_RIO_map::iterator firstDetElem;
	firstDetElem = range.first;
	//const InDet::SCT_Cluster* sctCluster = (firstDetElem->second);
	IdentifierHash waferID;
	waferID = firstDetElem->first;
	const InDetDD::SiDetectorElement* detElement = m_manager_sct->getDetectorElement(waferID);
	
	clusterCollection = new InDet::SCT_ClusterCollection(waferID);
	clusterCollection->setIdentifier(detElement->identify());
	
	
	for ( SCT_detElement_RIO_map::iterator iter = range.first; iter != range.second; ++iter){
	  InDet::SCT_Cluster* sctCluster = const_cast<InDet::SCT_Cluster*>((*iter).second);
	  sctCluster->setHashAndIndex(clusterCollection->identifyHash(),clusterCollection->size());
	  clusterCollection->push_back(sctCluster);
	}
	
	if (clusterCollection) {
	  if ((m_sctClusterContainer->addCollection(clusterCollection, clusterCollection->identifyHash())).isFailure()){
	    ATH_MSG_WARNING( "Could not add collection to Identifyable container !" );
	  }
	}
      } // end for
      
      m_sctClusterMap->clear();
    }
  }
  
  return StatusCode::SUCCESS;
}