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
  m_useDiscSurface(false),
  m_pixelClusterContainer(0),
  m_sctClusterContainer(0),
  m_planarClusterContainer(0),
  m_mergeSvc("PileUpMergeSvc",name),
  m_prdTruthNamePixel("PRD_MultiTruthPixel"),
  m_prdTruthNameSCT("PRD_MultiTruthSCT"),
  m_prdTruthNamePlanar("PRD_MultiTruthPlanar"),
  m_SmearPixel(true), //true: smear pixel --- false: smear SCT
  m_emulateAtlas(true), // error rotation for endcap SCT
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
  m_x_SCT_global(0),
  m_y_SCT_global(0),
  m_z_SCT_global(0),
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
  declareProperty("RndmSvc",                      m_rndmSvc, "Random Number Service used in SCT & Pixel digitization" );
  declareProperty("RndmEngine",                   m_randomEngineName, "Random engine name");
  declareProperty("InputObjectName",              m_inputObjectName="PixelHits", "Input Object name" );
  declareProperty("pitch_X",                      m_pitch_X);
  declareProperty("pitch_Y",                      m_pitch_Y);
  declareProperty("MergeClusters",                m_merge);
  declareProperty("Nsigma",                       m_nSigma);
  declareProperty("SmearPixel",                   m_SmearPixel, "Enable Pixel or SCT Smearing");
  declareProperty("PixelClusterContainerName",    m_pixel_SiClustersName="PixelClusters");
  declareProperty("SCT_ClusterContainerName",     m_Sct_SiClustersName="SCT_Clusters");
  declareProperty("PlanarClusterContainerName",   m_planar_SiClustersName="PlanarClusters");
  declareProperty("PRD_TruthPlanarContainerName", m_prdTruthNamePlanar="PRD_MultiTruthPlanar");
  declareProperty("DetectorElementMapName",       m_detElementMapName="Pixel_IdHashDetElementMap");
  declareProperty("CheckSmear",                   m_checkSmear);

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

  if (m_SmearPixel && m_useCustomGeometry){

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
      m_currentTree->Branch("pixel_X"            , &m_x_pixel         , "m_x_pixel/D");
      m_currentTree->Branch("pixel_Y"            , &m_y_pixel         , "m_y_pixel/D");
      m_currentTree->Branch("pixel_X_exit"       , &m_x_exit_pixel    , "m_x_exit_pixel/D");
      m_currentTree->Branch("pixel_Y_exit"       , &m_y_exit_pixel    , "m_y_exit_pixel/D");
      m_currentTree->Branch("pixel_Z_exit"       , &m_z_exit_pixel    , "m_z_exit_pixel/D");
      m_currentTree->Branch("pixel_X_entry"      , &m_x_entry_pixel   , "m_x_entry_pixel/D");
      m_currentTree->Branch("pixel_Y_entry"      , &m_y_entry_pixel   , "m_y_entry_pixel/D");
      m_currentTree->Branch("pixel_Z_entry"      , &m_z_entry_pixel   , "m_z_entry_pixel/D");
      m_currentTree->Branch("pixel_X_global"     , &m_x_pixel_global  , "m_x_pixel_global/D");
      m_currentTree->Branch("pixel_Y_global"     , &m_y_pixel_global  , "m_y_pixel_global/D");
      m_currentTree->Branch("pixel_Z_global"     , &m_z_pixel_global  , "m_z_pixel_global/D");
      m_currentTree->Branch("pixel_X_smear"      , &m_x_pixel_smeared , "m_x_pixel_smeared/D");
      m_currentTree->Branch("pixel_Y_smear"      , &m_y_pixel_smeared , "m_y_pixel_smeared/D");
      m_currentTree->Branch("pixel_Err_X"        , &m_Err_x_pixel     , "m_Err_x_pixel/D");
      m_currentTree->Branch("pixel_Err_Y"        , &m_Err_y_pixel     , "m_Err_y_pixel/D");

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
      m_currentTree->Branch("SCT_X"             , &m_x_SCT         , "m_x_SCT/D");
      m_currentTree->Branch("SCT_exit_X"        , &m_x_exit_SCT    , "m_x_exit_SCT/D");
      m_currentTree->Branch("SCT_exit_Y"        , &m_y_exit_SCT    , "m_y_exit_SCT/D");
      m_currentTree->Branch("SCT_exit_Z"        , &m_z_exit_SCT    , "m_z_exit_SCT/D");
      m_currentTree->Branch("SCT_entry_X"       , &m_x_entry_SCT   , "m_x_entry_SCT/D");
      m_currentTree->Branch("SCT_entry_Y"       , &m_y_entry_SCT   , "m_y_entry_SCT/D");
      m_currentTree->Branch("SCT_entry_Z"       , &m_z_entry_SCT   , "m_z_entry_SCT/D");
      m_currentTree->Branch("SCT_X_global"      , &m_x_SCT_global  , "m_x_SCT_global/D");
      m_currentTree->Branch("SCT_Y_global"      , &m_y_SCT_global  , "m_y_SCT_global/D");
      m_currentTree->Branch("SCT_Z_global"      , &m_z_SCT_global  , "m_z_SCT_global/D");
      m_currentTree->Branch("SCT_X_smear"       , &m_x_SCT_smeared , "m_x_SCT_smeared/D");
      m_currentTree->Branch("SCT_Err_X"         , &m_Err_x_SCT     , "m_Err_x_SCT/D");

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
                                                       SubEventIterator bSubEvents,
                                                       SubEventIterator eSubEvents)
{

  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: in pixel processBunchXing() ---" );

  m_seen.push_back(std::make_pair(std::distance(bSubEvents,eSubEvents), bunchXing));
  SubEventIterator iEvt(bSubEvents);
  while (iEvt != eSubEvents) {
    StoreGateSvc& seStore(*iEvt->ptr()->evtStore());
    PileUpTimeEventIndex thisEventIndex(PileUpTimeEventIndex(static_cast<int>(iEvt->time()),iEvt->index()));
    const SiHitCollection* seHitColl(NULL);
    if (!seStore.retrieve(seHitColl,m_inputObjectName).isSuccess()) {
      ATH_MSG_ERROR ( "SubEvent SiHitCollection not found in StoreGate " << seStore.name() );
      return StatusCode::FAILURE;
    }

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
    if(m_SmearPixel) // Smear Pixel
      m_planarClusterContainer = new iFatras::PlanarClusterContainer(m_pixel_ID->wafer_hash_max());
    else
      m_planarClusterContainer = new iFatras::PlanarClusterContainer(m_sct_ID->wafer_hash_max());

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

  if (m_merge)
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
    return mergeClusters(m_planarClusterMap);
  else if (m_SmearPixel)
    return mergeClusters(m_pixelClusterMap);
  else
    return mergeClusters(m_sctClusterMap);
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
ClusterInfo SiSmearedDigitizationTool::calculateNewCluster(CLUSTER * clusterA, CLUSTER * clusterB) {
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

  Amg::Vector2D intersection(interX, interY);

  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: intersection = " << intersection);

  InDet::SiWidth siWidth_a = clusterA->width();
  InDet::SiWidth siWidth_b = clusterB->width();

  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: siWidth_a = " << siWidth_a);
  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: siWidth_b = " << siWidth_b);

  Amg::Vector2D colRow = siWidth_a.colRow() + siWidth_b.colRow();
  Amg::Vector2D  phiRz = siWidth_a.widthPhiRZ() + siWidth_b.widthPhiRZ();

  InDet::SiWidth siWidth(colRow, phiRz);

  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: siWidth = " << siWidth);

  AmgSymMatrix(2) covariance;
  covariance.setIdentity();
  covariance(Trk::locX,Trk::locX) = sigmaX*sigmaX;
  covariance(Trk::locY,Trk::locY) = sigmaY*sigmaY;
  Amg::MatrixX* clusterErr = new Amg::MatrixX(covariance);

  return ClusterInfo( intersection, siWidth, clusterErr );

}

StatusCode SiSmearedDigitizationTool::mergeClusters(Pixel_detElement_RIO_map * cluster_map)
{
  // The idea is first to check how many cluster we have to merge and then merge them.
  // The loop is done until there aren't any clusters to merge

  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: in mergeClusters() using PixelClusters --- ");

  Pixel_detElement_RIO_map::iterator i = cluster_map->begin();
  Pixel_detElement_RIO_map::iterator e = cluster_map->end();

  for (; i != e; i = cluster_map->upper_bound(i->first)){
    IdentifierHash current_id = i->first;
    // Check if clusters with current_id have been already considered

    bool NewMerge = true;

    while (NewMerge) {
      NewMerge = false;
      std::pair <Pixel_detElement_RIO_map::iterator, Pixel_detElement_RIO_map::iterator> range = cluster_map->equal_range(current_id);

      for ( Pixel_detElement_RIO_map::iterator iter = range.first; iter != range.second;  ++iter){
        for (Pixel_detElement_RIO_map::iterator inner_iter = std::next(iter); inner_iter != range.second; ++inner_iter){

          double dist = calculateDistance((*iter).second,(*inner_iter).second);
          double sigma = calculateSigma((*iter).second,(*inner_iter).second);

          if( dist <= m_nSigma * sigma) {

            std::vector<Identifier> rdoList;

            Amg::Vector2D intersection;
            InDet::SiWidth siWidth;
            Amg::MatrixX *clusterErr;
            std::tie( intersection, siWidth, clusterErr ) = calculateNewCluster( iter->second, inner_iter->second );

            const InDetDD::SiDetectorElement* hitSiDetElement = (((*inner_iter).second)->detectorElement());
            Identifier intersectionId = hitSiDetElement->identifierOfPosition(intersection);

            rdoList.push_back(intersectionId);

            InDetDD::SiCellId currentCellId = hitSiDetElement->cellIdFromIdentifier(intersectionId);

            if ( !currentCellId.isValid() ) {
              delete clusterErr;
              continue;
            }

            InDet::PixelCluster* pixelCluster = new InDet::PixelCluster(intersectionId,
                                                                        intersection,
                                                                        rdoList,
                                                                        siWidth,
                                                                        hitSiDetElement,
                                                                        clusterErr);
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

  // i = cluster_map->begin();
  // e = cluster_map->end();

  // for (; i != e; i = cluster_map->upper_bound(i->first)){
  //   IdentifierHash current_id = i->first;
  //   // Check if clusters with current_id have been already considered

  //   ATH_MSG_DEBUG( "--- AFTER MERGING SiSmearedDigitizationTool: " << cluster_map->count(current_id) << "elements with key " << current_id );

  //   std::pair <Pixel_detElement_RIO_map::iterator, Pixel_detElement_RIO_map::iterator> range = cluster_map->equal_range(current_id);
  //   for ( Pixel_detElement_RIO_map::iterator iter = range.first; iter != range.second; ++iter){
  //     ATH_MSG_DEBUG( "--- AFTER MERGING SiSmearedDigitizationTool: Planar Cluster = " << *((*iter).second) );
  //   }
  // }

  return StatusCode::SUCCESS;
}

StatusCode SiSmearedDigitizationTool::mergeClusters(SCT_detElement_RIO_map * cluster_map)
{
  // The idea is first to check how many cluster we have to merge and then merge them.
  // The loop is done until there aren't any clusters to merge

  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: in mergeClusters() using SCT_Clusters --- ");

  SCT_detElement_RIO_map::iterator i = cluster_map->begin();
  SCT_detElement_RIO_map::iterator e = cluster_map->end();

  for (; i != e; i = cluster_map->upper_bound(i->first)){
    IdentifierHash current_id = i->first;
    // Check if clusters with current_id have been already considered
    bool NewMerge = true;

    while (NewMerge) {
      NewMerge = false;

      std::pair <SCT_detElement_RIO_map::iterator, SCT_detElement_RIO_map::iterator> range = cluster_map->equal_range(current_id);
      for ( SCT_detElement_RIO_map::iterator iter = range.first; iter != range.second;  ++iter){
        for ( SCT_detElement_RIO_map::iterator inner_iter = std::next(iter); inner_iter != range.second; ++inner_iter){

          double dist = calculateDistance((*iter).second,(*inner_iter).second);

          double sigma = calculateSigma((*iter).second,(*inner_iter).second);

          if( dist <= m_nSigma * sigma) {

            std::vector<Identifier> rdoList;

            Amg::Vector2D intersection;
            InDet::SiWidth siWidth;
            Amg::MatrixX *clusterErr;
            std::tie( intersection, siWidth, clusterErr ) = calculateNewCluster( iter->second, inner_iter->second );

            const InDetDD::SiDetectorElement* hitSiDetElement = (((*inner_iter).second)->detectorElement());
            Identifier intersectionId = hitSiDetElement->identifierOfPosition(intersection);

            rdoList.push_back(intersectionId);

            InDetDD::SiCellId currentCellId = hitSiDetElement->cellIdFromIdentifier(intersectionId);

            if ( !currentCellId.isValid() ) {
              delete clusterErr;
              continue;
            }

            InDet::SCT_Cluster* sctCluster = new InDet::SCT_Cluster(intersectionId,
                                                                    intersection,
                                                                    rdoList,
                                                                    siWidth,
                                                                    hitSiDetElement,
                                                                    clusterErr);
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

  // i = cluster_map->begin();
  // e = cluster_map->end();

  // for (; i != e; i = cluster_map->upper_bound(i->first)){
  //   IdentifierHash current_id = i->first;
  //   // Check if clusters with current_id have been already considered

  //   ATH_MSG_DEBUG( "--- AFTER MERGING SiSmearedDigitizationTool: " << cluster_map->count(current_id) << "elements with key " << current_id );

  //   std::pair <SCT_detElement_RIO_map::iterator, SCT_detElement_RIO_map::iterator> range = cluster_map->equal_range(current_id);
  //   for ( SCT_detElement_RIO_map::iterator iter = range.first; iter != range.second; ++iter){
  //     ATH_MSG_DEBUG( "--- AFTER MERGING SiSmearedDigitizationTool: Pixel Cluster = " << *((*iter).second) );
  //   }
  // }

  return StatusCode::SUCCESS;
}

StatusCode SiSmearedDigitizationTool::mergeClusters(Planar_detElement_RIO_map * cluster_map)
{
  // The idea is first to check how many cluster we have to merge and then merge them.
  // The loop is done until there aren't any clusters to merge

  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: in mergeClusters() using PlanarClusters ---" );

  Planar_detElement_RIO_map::iterator i = cluster_map->begin();
  Planar_detElement_RIO_map::iterator e = cluster_map->end();

  for (; i != e; i = cluster_map->upper_bound(i->first)){
    IdentifierHash current_id = i->first;
    // Check if clusters with current_id have been already considered
    bool NewMerge = true;

    while (NewMerge) {
      NewMerge = false;

      std::pair <Planar_detElement_RIO_map::iterator, Planar_detElement_RIO_map::iterator> range = cluster_map->equal_range(current_id);
      for ( Planar_detElement_RIO_map::iterator iter = range.first; iter != range.second;  ++iter){
        for (Planar_detElement_RIO_map::iterator inner_iter = std::next(iter); inner_iter != range.second; ++inner_iter){

          double dist = calculateDistance((*iter).second,(*inner_iter).second);

          double sigma = calculateSigma((*iter).second,(*inner_iter).second);

          if( dist <= m_nSigma * sigma) {

            std::vector<Identifier> rdoList;

            Amg::Vector2D intersection;
            InDet::SiWidth siWidth;
            Amg::MatrixX *clusterErr;
            std::tie( intersection, siWidth, clusterErr ) = calculateNewCluster( iter->second, inner_iter->second );

            const iFatras::PlanarDetElement* hitPlanarDetElement = (((*inner_iter).second)->detectorElement());
            Identifier intersectionId;

            std::pair<int, int> intersectionXY;
            if (hitPlanarDetElement->cellOfPosition(intersection, intersectionXY)) {
              intersectionId = m_SmearPixel ? m_pixel_ID->pixel_id(hitPlanarDetElement->identify(), intersectionXY.first, intersectionXY.second) :
                m_sct_ID->strip_id(hitPlanarDetElement->identify(), intersectionXY.first);
            }

            rdoList.push_back(intersectionId);

            InDetDD::SiCellId currentCellId = m_SmearPixel ? InDetDD::SiCellId(m_pixel_ID->phi_index(intersectionId) , m_pixel_ID->eta_index(intersectionId)) : InDetDD::SiCellId(m_sct_ID->strip(intersectionId));

            if ( !currentCellId.isValid() ) {
              delete clusterErr;
              continue;
            }

            iFatras::PlanarCluster* planarCluster = new iFatras::PlanarCluster(intersectionId,
                                                                               intersection,
                                                                               rdoList,
                                                                               siWidth,
                                                                               hitPlanarDetElement,
                                                                               clusterErr);
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

  // i = cluster_map->begin();
  // e = cluster_map->end();

  // for (; i != e; i = cluster_map->upper_bound(i->first)){
  //   IdentifierHash current_id = i->first;
  //   // Check if clusters with current_id have been already considered

  //   ATH_MSG_DEBUG( "--- AFTER MERGING SiSmearedDigitizationTool: " << cluster_map->count(current_id) << "elements with key " << current_id );
  //   std::pair <Planar_detElement_RIO_map::iterator, Planar_detElement_RIO_map::iterator> range = cluster_map->equal_range(current_id);
  //   for ( Planar_detElement_RIO_map::iterator iter = range.first; iter != range.second; ++iter){
  //     ATH_MSG_DEBUG( "--- AFTER MERGING SiSmearedDigitizationTool: Planar Cluster = " << *((*iter).second) );
  //   }
  // }

  return StatusCode::SUCCESS;
}

StatusCode SiSmearedDigitizationTool::digitize()
{
  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: in SiSmearedDigizationTool::digitize() ---" );

  if(m_useCustomGeometry) {
    if ((detStore()->retrieve(m_trackingGeometry, m_trackingGeometryName)).isFailure()) {
      ATH_MSG_FATAL( "Could not retrieve TrackingGeometry '" << m_trackingGeometryName << "' from DetectorStore." );
      return StatusCode::FAILURE;
    }
    else
      ATH_MSG_INFO( "TrackingGeometry '" << m_trackingGeometryName << "' successfully retrieved from DetectorStore." );
  }

  m_detElementMap = new iFatras::IdHashDetElementCollection;
  //Retrieve and/or store the map with IdHash to DetElement
  if ((detStore()->contains<iFatras::IdHashDetElementCollection>(m_detElementMapName))){
    if((detStore()->retrieve(m_detElementMap, m_detElementMapName)).isFailure()){
      ATH_MSG_FATAL("Could not retrieve collection " << m_detElementMapName);
      return StatusCode::FAILURE;
    } else ATH_MSG_DEBUG("Found and Retrieved collection " << m_detElementMapName);
  } else ATH_MSG_DEBUG("Collection " << m_detElementMapName  << " not found!");


  TimedHitCollection<SiHit>::const_iterator i, e;

  if (m_useCustomGeometry)
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
      m_useDiscSurface = false;

      TimedHitPtr<SiHit> hit(*i++);
      int barrelEC  = hit->getBarrelEndcap();
      int layerDisk = hit->getLayerDisk();
      int phiModule = hit->getPhiModule();
      int etaModule = hit->getEtaModule();
      int side      = 0;

      const InDetDD::SiDetectorElement* hitSiDetElement = 0;
      const iFatras::PlanarDetElement* hitPlanarDetElement = 0;

      if(m_SmearPixel){ // Smear Pixel
        if (!m_useCustomGeometry) { // Not custom Pixel
          const InDetDD::SiDetectorElement* hitSiDetElement_temp = m_manager_pix->getDetectorElement(barrelEC,layerDisk,phiModule,etaModule);
          ATH_MSG_DEBUG("Pixel SiDetectorElement --> barrel_ec " << barrelEC << ", layer_disk " << layerDisk << ", phi_module " << phiModule << ", eta_module " << etaModule );
          hitSiDetElement = hitSiDetElement_temp;
        } else { // Custom Pixel
          Identifier idwafer = m_pixel_ID->wafer_id(barrelEC,layerDisk,phiModule,etaModule);
          IdentifierHash idhash = m_pixel_ID->wafer_hash(m_pixel_ID->wafer_id(idwafer));
          iFatras::IdHashDetElementCollection::iterator it_map = m_detElementMap->find(idhash);
          if (it_map == m_detElementMap->end())
            ATH_MSG_WARNING("Id hash " << idhash << " not found in the map from id hash to planar detector element.");
          else {
            ATH_MSG_DEBUG("Id hash " << idhash << " found in the map.");
            const iFatras::PlanarDetElement* hitPlanarDetElement_temp = it_map->second;
            ATH_MSG_DEBUG("Pixel PlanarDetElement --> barrel_ec " << barrelEC << ", layer_disk " << layerDisk << ", phi_module " << phiModule << ", eta_module " << etaModule );
            hitPlanarDetElement = hitPlanarDetElement_temp;
            ATH_MSG_DEBUG("Surface " << hitPlanarDetElement->surface());
          }
        }
      } else { // Smear SCT
        side = hit->getSide();
        if (!m_useCustomGeometry) {// Not custom SCT
          const InDetDD::SiDetectorElement* hitSiDetElement_temp = m_manager_sct->getDetectorElement(barrelEC,layerDisk,phiModule,etaModule,side);
          ATH_MSG_DEBUG("SCT SiDetectorElement --> barrel_ec " << barrelEC << ", layer_disk " << layerDisk << ", phi_module " << phiModule << ", eta_module " << etaModule << ", side " << side);
          hitSiDetElement = hitSiDetElement_temp;
        } else { // Custom SCT
          Identifier idwafer = m_sct_ID->wafer_id(barrelEC,layerDisk,phiModule,etaModule,side);
          IdentifierHash idhash = m_sct_ID->wafer_hash(m_sct_ID->wafer_id(idwafer));
          iFatras::IdHashDetElementCollection::iterator it_map = m_detElementMap->find(idhash);
          if (it_map == m_detElementMap->end())
            ATH_MSG_WARNING("Id hash " << idhash << " not found in the map from id hash to planar detector element.");
          else{
            ATH_MSG_DEBUG("Id hash " << idhash << " found in the map.");
            const iFatras::PlanarDetElement* hitPlanarDetElement_temp = it_map->second;
            ATH_MSG_DEBUG("SCT PlanarDetElement --> barrel_ec " << barrelEC << ", layer_disk " << layerDisk << ", phi_module " << phiModule << ", eta_module " << etaModule << ", side " << side);
            hitPlanarDetElement = hitPlanarDetElement_temp;
            ATH_MSG_DEBUG("Surface " << hitPlanarDetElement->surface());
          }
        }
      }

      if ( !hitSiDetElement && !hitPlanarDetElement ) {
        ATH_MSG_ERROR( " could not get detector element for both SiDetElement and PlanarDetElement");
        continue;
      }
      else if ( !hitSiDetElement && !m_useCustomGeometry ) {
        ATH_MSG_ERROR( " could not get detector element for SiDetElement");
        continue;
      }
      else if( !hitPlanarDetElement && m_useCustomGeometry ) {
        ATH_MSG_ERROR( " could not get detector element for PlanarDetElement");
        continue;
      }

      if (m_SmearPixel && !m_useCustomGeometry && !(hitSiDetElement->isPixel())) continue;
      if (m_SmearPixel && m_useCustomGeometry && !(hitPlanarDetElement->isPixel())) continue;

      if (!m_SmearPixel && !m_useCustomGeometry && !(hitSiDetElement->isSCT())) continue;
      if (!m_SmearPixel && m_useCustomGeometry && !(hitPlanarDetElement->isSCT())) continue;

      IdentifierHash waferID;

      if (!m_useCustomGeometry) // Not custom
        if(m_SmearPixel){ // Smear Pixel
          waferID = m_pixel_ID->wafer_hash(hitSiDetElement->identify());
        }else{ // Smear SCT
          waferID = m_sct_ID->wafer_hash(hitSiDetElement->identify());
        }
      else // Custom
        if(m_SmearPixel){ // Smear Pixel
          waferID = m_pixel_ID->wafer_hash(hitPlanarDetElement->identify());
        }else{ // Smear SCT
          waferID = m_sct_ID->wafer_hash(hitPlanarDetElement->identify());
        }

      if (m_useCustomGeometry) {
        m_pitch_X = hitPlanarDetElement->pitchX();
        m_pitch_Y = hitPlanarDetElement->pitchY();
      }

      HepGeom::Point3D<double> pix_localStartPosition = hit->localStartPosition();
      HepGeom::Point3D<double> pix_localEndPosition   = hit->localEndPosition();

      if (!m_useCustomGeometry) {
        pix_localStartPosition = hitSiDetElement->hitLocalToLocal3D(pix_localStartPosition);
        pix_localEndPosition = hitSiDetElement->hitLocalToLocal3D(pix_localEndPosition);
      } else {
        const Trk::Surface* hitSurface = &hitPlanarDetElement->surface();
        const Amg::Transform3D& sTransform = hitSurface->transform().inverse();
        const Amg::Transform3D& hitTransform = Amg::CLHEPTransformToEigen(hitPlanarDetElement->transformHit());
        Amg::Vector3D localStartPosition(pix_localStartPosition.x(), pix_localStartPosition.y(), pix_localStartPosition.z());
        Amg::Vector3D localEndPosition(pix_localEndPosition.x(), pix_localEndPosition.y(), pix_localEndPosition.z());
        Amg::Vector3D localEntry(sTransform*(hitTransform*localStartPosition));
        Amg::Vector3D localExit(sTransform*(hitTransform*localEndPosition));
        pix_localStartPosition = HepGeom::Point3D<double>( localEntry.x(), localEntry.y(), localEntry.z() );
        pix_localEndPosition = HepGeom::Point3D<double>( localExit.x(), localExit.y(), localExit.z() );
      }

      double localEntryX = pix_localStartPosition.x();
      double localEntryY = pix_localStartPosition.y();
      double localEntryZ = pix_localStartPosition.z();
      double localExitX = pix_localEndPosition.x();
      double localExitY = pix_localEndPosition.y();
      double localExitZ = pix_localEndPosition.z();

      double thickness = 0.0;

      if (!m_useCustomGeometry)
        thickness = hitSiDetElement->thickness();
      else
        thickness = hitPlanarDetElement->thickness();

      // Transform to reconstruction local coordinates (different x,y,z ordering and sign conventions compared to simulation coords)
      if (!m_SmearPixel) { // Smear SCT
        HepGeom::Point3D<double> sct_localStartPosition = hit->localStartPosition();
        HepGeom::Point3D<double> sct_localEndPosition = hit->localEndPosition();

        if (!m_useCustomGeometry) {
          sct_localStartPosition = hitSiDetElement->hitLocalToLocal3D(sct_localStartPosition);
          sct_localEndPosition = hitSiDetElement->hitLocalToLocal3D(sct_localEndPosition);
        } else {
          const Trk::Surface* hitSurface = &hitPlanarDetElement->surface();
          const Amg::Transform3D& sTransform = hitSurface->transform().inverse();
          const Amg::Transform3D& hitTransform = Amg::CLHEPTransformToEigen( hitPlanarDetElement->transformHit());
          Amg::Vector3D localStartPosition(sct_localStartPosition.x(), sct_localStartPosition.y(), sct_localStartPosition.z());
          ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: SCT local start position --- " << localStartPosition.x() << ",  " << localStartPosition.y() << ",  " << localStartPosition.z());
          Amg::Vector3D localEndPosition(sct_localEndPosition.x(), sct_localEndPosition.y(), sct_localEndPosition.z());
          ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: SCT local end position --- " << localEndPosition.x() << ",  " << localEndPosition.y() << ",  " << localEndPosition.z());
          Amg::Vector3D localEntry(sTransform*(hitTransform*localStartPosition));
          Amg::Vector3D localExit(sTransform*(hitTransform*localEndPosition));
          sct_localStartPosition = HepGeom::Point3D<double>( localEntry.x(), localEntry.y(), localEntry.z() );
          sct_localEndPosition = HepGeom::Point3D<double>( localExit.x(), localExit.y(), localExit.z() );

          // Transform to the (R,Phi) local frame of Disc Surface if needed.
          // This happens if the Surface type is DiscSurface and if the bounds are DiscTrapezoidalBounds (this is not checked since is the only case now)
          if (hitSurface->type() == Trk::Surface::Disc) {
            m_useDiscSurface = true;
            const Trk::DiscSurface* disc = dynamic_cast<const Trk::DiscSurface*>(hitSurface);
            Amg::Vector2D polLocalEntry = *(disc->localCartesianToPolar(Amg::Vector2D(localEntry.x(), localEntry.y())));
            Amg::Vector2D polLocalExit = *(disc->localCartesianToPolar(Amg::Vector2D(localExit.x(), localExit.y())));
            sct_localStartPosition = HepGeom::Point3D<double>( polLocalEntry.x(), polLocalEntry.y(), localEntry.z());
            sct_localEndPosition = HepGeom::Point3D<double>( polLocalExit.x(), polLocalExit.y(), localExit.z());
          }
        }

        localEntryX = sct_localStartPosition.x();
        localEntryY = sct_localStartPosition.y();
        localEntryZ = sct_localStartPosition.z();
        localExitX  = sct_localEndPosition.x();
        localExitY  = sct_localEndPosition.y();
        localExitZ  = sct_localEndPosition.z();
      }

      double distX = fabs(fabs(localExitX)-fabs(localEntryX));
      double distY = fabs(fabs(localExitY)-fabs(localEntryY));

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
        std::pair<int, int> entryXY;
        // now get the cellId and check whether it is valid
        if (hitPlanarDetElement->cellOfPosition(localEntry, entryXY)) {
          Identifier entryId = m_SmearPixel ? m_pixel_ID->pixel_id(barrelEC, layerDisk, phiModule, etaModule, entryXY.first, entryXY.second) :
            m_sct_ID->strip_id(barrelEC, layerDisk, phiModule, etaModule, side, entryXY.first);
          entryCellId = m_SmearPixel ? InDetDD::SiCellId(m_pixel_ID->phi_index(entryId), m_pixel_ID->eta_index(entryId)) :
            InDetDD::SiCellId(m_sct_ID->strip(entryId));
          ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool " << (m_SmearPixel ? "pixel:" : "SCT:") << " entryId " << entryId << " --- entryCellId " << entryCellId );
        } else {
          entryCellId = InDetDD::SiCellId();
          ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool " << (m_SmearPixel ? "pixel:" : "SCT:") << " Not Valid CellId" );
        }

        std::pair<int, int> exitXY;
        // now get the cellId and check whether it is valid
        if (hitPlanarDetElement->cellOfPosition(localExit, exitXY)) {
          Identifier exitId  = m_SmearPixel ? m_pixel_ID->pixel_id(barrelEC, layerDisk, phiModule, etaModule, exitXY.first, exitXY.second) :
            m_sct_ID->strip_id(barrelEC, layerDisk, phiModule, etaModule, side, exitXY.first);
          exitCellId  = m_SmearPixel ? InDetDD::SiCellId(m_pixel_ID->phi_index(exitId) , m_pixel_ID->eta_index(exitId)) :
            InDetDD::SiCellId(m_sct_ID->strip(exitId));
          ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool " << (m_SmearPixel ? "pixel:" : "SCT:") << " exitId " << exitId << " --- exitCellId " << exitCellId );
        } else {
          exitCellId = InDetDD::SiCellId();
          ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool " << (m_SmearPixel ? "pixel:" : "SCT:") << " Not Valid CellId" );
        }

      }

      // entry / exit validity
      bool entryValid = entryCellId.isValid();
      bool exitValid  = exitCellId.isValid();

      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: entryValid? " << entryValid << " --- exitValid? " << exitValid );

      if (!entryValid && !exitValid) continue;

      // the intersecetion id and cellId of it
      double interX = 0.5*(localEntryX+localExitX);
      double interY = 0.5*(localEntryY+localExitY);

      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: " << (m_SmearPixel ? "pixel" : "SCT") << " inter X --- " << interX );
      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: " << (m_SmearPixel ? "pixel" : "SCT") << " inter Y --- " << interY );
      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: " << (m_SmearPixel ? "pixel" : "SCT") << " dist X --- "  << distX );
      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: " << (m_SmearPixel ? "pixel" : "SCT") << " dist Y --- "  << distY );

      //the particle crosses at least n pixels (in x direction you have timesX, in y direction you have timesY)
      double timesX = (m_pitch_X) ? floor(distX/m_pitch_X) : 0;
      double timesY = (m_pitch_Y) ? floor(distY/m_pitch_Y) : 0;

      double newdistX = distX - (timesX*m_pitch_X);
      double newdistY = distY - (timesY*m_pitch_Y);

      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: times X --- " << timesX );
      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: times Y --- " << timesY );
      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: new dist X --- " << newdistX );
      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: new dist Y --- " << newdistY );
      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: thickness --- " << thickness );

      //Probability

      double ProbY = 2*newdistY/(m_pitch_Y+newdistY);
      double ProbX = 2*newdistX/(m_pitch_X+newdistX);

      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: ProbX --- " << ProbX );
      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: ProbY --- " << ProbY );

      // create the errors
      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: pitch X --- " << m_pitch_X );
      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: pitch Y --- " << m_pitch_Y );

      double sigmaX = m_pitch_X/sqrt(12.);
      double sigmaY = m_pitch_Y/sqrt(12.);

      int elementX = timesX+1;
      int elementY = timesY+1;

      if(m_SmearPixel){
        if (CLHEP::RandFlat::shoot(m_randomEngine, 0.0, 1.0) < ProbY){ // number of crossed pixel is (timesY+1)+1
          sigmaY = (double)(timesY+2)*m_pitch_Y/sqrt(12.);
          elementY++;
        } else // number of crossed pixel is (timesY+1)
          sigmaY = (double)(timesY+1)*m_pitch_Y/sqrt(12.);

        if (CLHEP::RandFlat::shoot(m_randomEngine, 0.0, 1.0) < ProbX){ // number of crossed pixel is (timesY+1)+1
          sigmaX = (double)(timesX+2)*m_pitch_X/sqrt(12.);
          elementX++;
        } else // number of crossed pixel is (timesY+1)
          sigmaX = (double)(timesX+1)*m_pitch_X/sqrt(12.);
      }

      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool " << (m_SmearPixel ? "pixel" : "SCT") << " sigma X --- " << sigmaX);
      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool " << (m_SmearPixel ? "pixel" : "SCT") << " sigma Y --- " << sigmaY);


      double temp_X = interX;
      double temp_Y = interY;

      // Smear intersection
      // create the smear parameter
      if(m_SmearPixel){ // Smear Pixel also in y direction
        if (sigmaX != 0.) {
          double sParX = 0.;
          do {
            sParX = CLHEP::RandGauss::shoot(m_randomEngine, 0., sigmaX);
            ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: extracted gaussian value for X --- " << sParX);
          } while (fabs(interX+sParX)>(hitPlanarDetElement->lengthXmin()/2.));
          interX += sParX;
        }
        if (sigmaY != 0.) {
          double sParY = 0.;
          do {
            sParY = CLHEP::RandGauss::shoot(m_randomEngine, 0., sigmaY);
            ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: extracted gaussian value for Y --- " << sParY);
          }  while (fabs(interY+sParY)>(hitPlanarDetElement->lengthY()/2.));
          interY += sParY;
        }
      }

      // Define the current smeared center position
      if(!m_SmearPixel && !m_useDiscSurface) {
        // correct position x first if you have a trapezoid
        if (hitPlanarDetElement->shape() == InDetDD::Trapezoid) {
          double lengthY    = hitPlanarDetElement->lengthY();
          double lengthXmin = hitPlanarDetElement->lengthXmin();
          double lengthXmax = hitPlanarDetElement->lengthXmax();
          double height     = lengthY/2.*(lengthXmax+lengthXmin)/(lengthXmax-lengthXmin)+interY;
          double tan        = interX/height;
          interX -= interY*tan;
        }
        interY = 0.;
      }


      Amg::Vector2D intersection(interX, interY);

      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: Intersection after smearing: " << intersection);

      Identifier intersectionId;
      if (!m_useCustomGeometry)
        intersectionId = hitSiDetElement->identifierOfPosition(intersection);
      else {
        std::pair<int, int> intersectionXY;
        if (hitPlanarDetElement->cellOfPosition(intersection, intersectionXY))
          // get the identifier of the entry and the exit
          intersectionId = m_SmearPixel ? m_pixel_ID->pixel_id(barrelEC, layerDisk, phiModule, etaModule, intersectionXY.first, intersectionXY.second) : m_sct_ID->strip_id(barrelEC, layerDisk, phiModule, etaModule, side, intersectionXY.first);
      }

      rdoList.push_back(intersectionId);
      InDetDD::SiCellId currentCellId = m_useCustomGeometry ? (m_SmearPixel ? InDetDD::SiCellId(m_pixel_ID->phi_index(intersectionId) , m_pixel_ID->eta_index(intersectionId)) : InDetDD::SiCellId(m_sct_ID->strip(intersectionId))) : hitSiDetElement->cellIdFromIdentifier(intersectionId);

      if (!currentCellId.isValid()) continue;

      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: Intersection Id = " << intersectionId << " --- currentCellId = " << currentCellId );

      if(m_SmearPixel){ // Smear Pixel --> Create Pixel Cluster

        double lengthX = (m_pitch_X) ? elementX*m_pitch_X : 1.;
        double lengthY = (m_pitch_Y) ? elementY*m_pitch_Y : 1.;

        if (m_pitch_X == 0. || m_pitch_Y == 0.)
          ATH_MSG_WARNING( "--- SiSmearedDigitizationTool: pitchX and/or pitchY are 0. Cluster length is forced to be 1. mm");

        InDet::SiWidth* siWidth = new InDet::SiWidth(Amg::Vector2D(elementX,elementY), Amg::Vector2D(lengthX, lengthY));

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

        } else { // create planar clusters for pixel
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

      } else { // Smear SCT --> Create SCT Cluster

        // prepare the clusters
        InDet::SCT_Cluster * sctCluster = 0;
        iFatras::PlanarCluster * planarCluster = 0;

        if (!m_useCustomGeometry) {
          // Pixel Design needed -------------------------------------------------------------
          InDetDD::SCT_ModuleSideDesign* design_sct;

          design_sct = (InDetDD::SCT_ModuleSideDesign*)&hitSiDetElement->design();

          if (!design_sct) {
            ATH_MSG_INFO ( "Could not get design"<< design_sct) ;
            continue;
          }

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

          InDetDD::DetectorShape elShape = hitSiDetElement->design().shape();
          if(m_emulateAtlas && elShape == InDetDD::Trapezoid){// rotation for endcap SCT

            if(colRow.x() == 1){
              mat(Trk::locX,Trk::locX) = pow(siWidth->phiR(),2)/12;
            }
            else if(colRow.x() == 2){
              mat(Trk::locX,Trk::locX) = pow(0.27*siWidth->phiR(),2)/12;
            }
            else{
              mat(Trk::locX,Trk::locX) = pow(siWidth->phiR(),2)/12;
            }

            mat(Trk::locY,Trk::locY) = pow(siWidth->z()/colRow.y(),2)/12;
            double sn      = hitSiDetElement->sinStereoLocal(intersection);
            double sn2     = sn*sn;
            double cs2     = 1.-sn2;
            double w       = hitSiDetElement->phiPitch(intersection)/hitSiDetElement->phiPitch();
            double v0      = mat(Trk::locX, Trk::locX)*w*w;
            double v1      = mat(Trk::locY, Trk::locY);
            mat(Trk::locX, Trk::locX) = (cs2*v0+sn2*v1);
            mat(Trk::locY, Trk::locX) = (sn*sqrt(cs2)*(v0-v1));
            mat(Trk::locY, Trk::locY) = (sn2*v0+cs2*v1);
          }

          Amg::MatrixX* clusterErr = new Amg::MatrixX(mat);

          sctCluster = new InDet::SCT_Cluster(intersectionId,
                                              intersection,
                                              rdoList,
                                              *siWidth,
                                              hitSiDetElement,
                                              clusterErr);

          m_sctClusterMap->insert(std::pair<IdentifierHash, const InDet::SCT_Cluster* >(waferID, sctCluster));

          if (FillTruthMap(m_SCTPrdTruth, sctCluster, hit).isFailure()) {
            ATH_MSG_FATAL ( "FillTruthMap() for SCT failed!" );
            return StatusCode::FAILURE;
          }

        } else {
          // Find length of strip at centre
          double clusterWidth = rdoList.size()*hitPlanarDetElement->phiPitch(intersection);
          double stripLength  = hitPlanarDetElement->stripLength(intersection);

          InDet::SiWidth* siWidth = new InDet::SiWidth(Amg::Vector2D(int(rdoList.size()),1),
                                                       Amg::Vector2D(clusterWidth,stripLength) );

          AmgSymMatrix(2) mat;
          mat.setIdentity();
          if(m_useDiscSurface) {
            mat(Trk::locR,Trk::locR) = stripLength*stripLength/12.;
            mat(Trk::locPhi,Trk::locPhi) = sigmaY*sigmaY;
          } else {
            mat(Trk::locX,Trk::locX) = sigmaX*sigmaX;
            mat(Trk::locY,Trk::locY) = stripLength*stripLength/12.;

            InDetDD::DetectorShape elShape = hitPlanarDetElement->shape();
            if(m_emulateAtlas && elShape == InDetDD::Trapezoid){// rotation for endcap SCT

              const Amg::Vector2D& colRow = siWidth->colRow();

              if(colRow.x() == 1){
                mat(Trk::locX,Trk::locX) = pow(siWidth->phiR(),2)/12;
              }
              else if(colRow.x() == 2){
                mat(Trk::locX,Trk::locX) = pow(0.27*siWidth->phiR(),2)/12;
              }
              else{
                mat(Trk::locX,Trk::locX) = pow(siWidth->phiR(),2)/12;
              }

              mat(Trk::locY,Trk::locY) = pow(siWidth->z()/colRow.y(),2)/12;
              double sn      = hitPlanarDetElement->sinStereoLocal(intersection);
              double sn2     = sn*sn;
              double cs2     = 1.-sn2;
              double w       = hitPlanarDetElement->phiPitch(intersection)/hitPlanarDetElement->phiPitch();
              double v0      = mat(Trk::locX, Trk::locX)*w*w;
              double v1      = mat(Trk::locY, Trk::locY);
              mat(Trk::locX, Trk::locX) = (cs2*v0+sn2*v1);
              mat(Trk::locY, Trk::locX) = (sn*sqrt(cs2)*(v0-v1));
              mat(Trk::locY, Trk::locY) = (sn2*v0+cs2*v1);
            }

          }

          Amg::MatrixX* clusterErr = new Amg::MatrixX(mat);

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
              ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: SCT_Cluster --> " << *sctCluster);

            //Take info to store in the tree
            m_x_SCT = m_useDiscSurface ? temp_Y : temp_X;
            m_x_SCT_smeared = m_useCustomGeometry ? (m_useDiscSurface ? planarCluster->localPosition()[Trk::locPhi] : planarCluster->localPosition()[Trk::locX]) : (sctCluster->localPosition()).x();

            ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: BEFORE SMEARING LocalPosition --> X = " << m_x_SCT );
            ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: LocalPosition --> X = " << m_x_SCT_smeared );

            if (!m_useCustomGeometry) {
              ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: GlobalPosition --> X = " << (sctCluster->globalPosition()).x() << "  Y = " << (sctCluster->globalPosition()).y());
              m_x_SCT_global = (sctCluster->globalPosition()).x();
              m_y_SCT_global = (sctCluster->globalPosition()).y();
              m_z_SCT_global = (sctCluster->globalPosition()).z();

            } else {
              ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: GlobalPosition --> X = " << (planarCluster->globalPosition()).x() << "  Y = " << (planarCluster->globalPosition()).y());
              m_x_SCT_global = (planarCluster->globalPosition()).x();
              m_y_SCT_global = (planarCluster->globalPosition()).y();
              m_z_SCT_global = (planarCluster->globalPosition()).z();
            }

            m_Err_x_SCT = m_useCustomGeometry ? (m_useDiscSurface ? Amg::error(planarCluster->localCovariance(), Trk::locPhi) : Amg::error(planarCluster->localCovariance(), Trk::locX)) : Amg::error(sctCluster->localCovariance(), Trk::locX);
            ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: Error --> X = " << m_Err_x_SCT );

            m_currentTree -> Fill();

          }
      }
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode SiSmearedDigitizationTool::createAndStoreRIOs()
{
  if ( m_useCustomGeometry ) { // store Planar RIOs

    ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: in planar createAndStoreRIOs() ---" );

    Planar_detElement_RIO_map::iterator i = m_planarClusterMap->begin();
    Planar_detElement_RIO_map::iterator e = m_planarClusterMap->end();

    //Retrieve and/or store the map with IdHash to DetElement
    m_detElementMap = new iFatras::IdHashDetElementCollection;
    if ((detStore()->contains<iFatras::IdHashDetElementCollection>(m_detElementMapName))){
      if((detStore()->retrieve(m_detElementMap, m_detElementMapName)).isFailure()){
        ATH_MSG_FATAL("Could not retrieve collection " << m_detElementMapName);
        return StatusCode::FAILURE;
      }
      else
        ATH_MSG_DEBUG("Found and Retrieved collection " << m_detElementMapName);
    }
    ATH_MSG_DEBUG("Number of elements " << m_detElementMap->size() );

    for (; i != e; i = m_planarClusterMap->upper_bound(i->first)){

      std::pair <Planar_detElement_RIO_map::iterator, Planar_detElement_RIO_map::iterator> range;
      range = m_planarClusterMap->equal_range(i->first);

      Planar_detElement_RIO_map::iterator firstDetElem;
      firstDetElem = range.first;

      IdentifierHash waferID;
      waferID = firstDetElem->first;

      iFatras::IdHashDetElementCollection::iterator it_map = m_detElementMap->find(waferID);
      if ( it_map == m_detElementMap->end() ) {
        ATH_MSG_WARNING("Id hash " << waferID << " not found in the map from id hash to planar detector element.");
        return StatusCode::FAILURE;
      } else {
        ATH_MSG_DEBUG("Id hash " << waferID << " found in the map.");
      }

      const iFatras::PlanarDetElement* detElement = it_map->second;

      iFatras::PlanarClusterCollection *clusterCollection = new iFatras::PlanarClusterCollection(waferID);
      clusterCollection->setIdentifier(detElement->identify());

      for ( Planar_detElement_RIO_map::iterator iter = range.first; iter != range.second; ++iter ) {

        iFatras::PlanarCluster* planarCluster = const_cast<iFatras::PlanarCluster*>((*iter).second);
        planarCluster->setHashAndIndex(clusterCollection->identifyHash(),clusterCollection->size());
        clusterCollection->push_back(planarCluster);
      }

      if ( m_planarClusterContainer->addCollection( clusterCollection, waferID ).isFailure() ) {
        ATH_MSG_WARNING( "Could not add collection to Identifyable container !" );
      }

    } // end for

    m_planarClusterMap->clear();

  } else if ( m_SmearPixel ) { // Store Pixel RIOs

    ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: in pixel createAndStoreRIOs() ---" );

    Pixel_detElement_RIO_map::iterator i = m_pixelClusterMap->begin();
    Pixel_detElement_RIO_map::iterator e = m_pixelClusterMap->end();

    for ( ; i != e; i = m_pixelClusterMap->upper_bound(i->first) ) {

      std::pair <Pixel_detElement_RIO_map::iterator, Pixel_detElement_RIO_map::iterator> range;
      range = m_pixelClusterMap->equal_range(i->first);

      Pixel_detElement_RIO_map::iterator firstDetElem;
      firstDetElem = range.first;

      IdentifierHash waferID;
      waferID = firstDetElem->first;

      const InDetDD::SiDetectorElement* detElement = m_manager_pix->getDetectorElement(waferID);

      InDet::PixelClusterCollection *clusterCollection = new InDet::PixelClusterCollection(waferID);
      clusterCollection->setIdentifier(detElement->identify());

      for ( Pixel_detElement_RIO_map::iterator iter = range.first; iter != range.second; ++iter ) {

        InDet::PixelCluster* pixelCluster = const_cast<InDet::PixelCluster*>((*iter).second);
        pixelCluster->setHashAndIndex(clusterCollection->identifyHash(),clusterCollection->size());
        clusterCollection->push_back(pixelCluster);
      }

      if ( m_pixelClusterContainer->addCollection( clusterCollection, waferID ).isFailure() ) {
        ATH_MSG_WARNING( "Could not add collection to Identifyable container !" );
      }
    } // end for

    m_pixelClusterMap->clear();

  }
  else { // Store SCT RIOs

    ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: in SCT createAndStoreRIOs() ---" );

    SCT_detElement_RIO_map::iterator i = m_sctClusterMap->begin();
    SCT_detElement_RIO_map::iterator e = m_sctClusterMap->end();

    for ( ; i != e; i = m_sctClusterMap->upper_bound(i->first) ) {
      std::pair <SCT_detElement_RIO_map::iterator, SCT_detElement_RIO_map::iterator> range;
      range = m_sctClusterMap->equal_range(i->first);

      SCT_detElement_RIO_map::iterator firstDetElem;
      firstDetElem = range.first;

      IdentifierHash waferID;
      waferID = firstDetElem->first;
      const InDetDD::SiDetectorElement* detElement = m_manager_sct->getDetectorElement(waferID);

      InDet::SCT_ClusterCollection *clusterCollection = new InDet::SCT_ClusterCollection(waferID);
      clusterCollection->setIdentifier(detElement->identify());


      for ( SCT_detElement_RIO_map::iterator iter = range.first; iter != range.second; ++iter ) {
        InDet::SCT_Cluster* sctCluster = const_cast<InDet::SCT_Cluster*>((*iter).second);
        sctCluster->setHashAndIndex(clusterCollection->identifyHash(),clusterCollection->size());
        clusterCollection->push_back(sctCluster);
      }

      if ( m_sctClusterContainer->addCollection( clusterCollection, clusterCollection->identifyHash() ).isFailure() ) {
        ATH_MSG_WARNING( "Could not add collection to Identifyable container !" );
      }

    } // end for

    m_sctClusterMap->clear();
  }

  return StatusCode::SUCCESS;
}
