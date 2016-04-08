/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelClusterValidationNtupleWriter.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////
//aldo.beati

#include "InDetTrackValidation/PixelClusterValidationNtupleWriter.h"
// Gaudi
#include "GaudiKernel/ITHistSvc.h"
#include "TTree.h"

#include "xAODEventInfo/EventInfo.h"

#include "InDetPrepRawData/PixelCluster.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiLocalPosition.h"
#include "InDetReadoutGeometry/SiCellId.h"

#include "GeneratorObjects/HepMcParticleLink.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenEvent.h"
#include "InDetSimEvent/SiHit.h"

#include "PixelConditionsServices/IPixelOfflineCalibSvc.h"
#include <cmath>
#include <TMath.h>

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "PixelGeoModel/IBLParameterSvc.h"

#include <string>

//Beam Spot Condition
#include "InDetBeamSpotService/IBeamCondSvc.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"

using CLHEP::GeV;


//----------------------------- PixelClusterValidationNtupleWriter ----------------------------
InDet::PixelClusterValidationNtupleWriter::PixelClusterValidationNtupleWriter(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator),
  m_riocontainer(0),
  m_hitcontainer(0),
  m_rdocontainer(0),
  //       m_inputTrackCollection("Tracks")
  //m_fullNtupleName("/NTUPLES/FILE1/FitterValidation/TrackStatistics"),
  mjo_riocontainername("PixelClusters"),
  mjo_hitcontainername("PixelHits"),
  mjo_rdocontainername("PixelRDOs"),
  mjo_trackCollectionName("CombinedInDetTracks"),
  mjo_Hits(false),
  mjo_MC(false),
  mjo_NotAssociated(false),
  mjo_WriteDetailedPixelInformation(false),
  mjo_NN(false),
  mjo_ToT(false),
  mjo_onTrack(false),
  m_IBLAbsent(true),
  m_IBLParameterSvc("IBLParameterSvc",name),
  m_pixelCalib("PixelOfflineCalibSvc",name),
  m_iBeamCondSvc("BeamCondSvc",name),
  m_ntupleFileName("/NTUPLES/FILE1"),
  m_ntupleDirName("FitterValidation"),
  m_ntupleTreeName("PixelClu"),
  m_ntupleNNTreeName("NNinput"),
  m_pixelid(0),
  m_nt(0),
  m_nnt(0),
  m_geoModel(0),
  m_PixClusLocX(0),
  m_PixClusLocY(0),
  m_PixClusLocXcentroid(0),
  m_PixClusLocYcentroid(0),
  m_PixClusLocX_LorentzCorrection(0),
  m_PixClusGloX(0),
  m_PixClusGloY(0),
  m_PixClusGloZ(0),
  m_PixClusEta(0),
  m_PixClusPhi(0),
  m_PixECBarrel(0),
  m_PixClusLayer (0),
  m_PixEtaModule(0),
  m_PixPhiModule(0),
  m_PixClusGroupsize(0),
  m_PixClusRow(0),
  m_PixClusCol(0),
  m_PixDeltaRow(0),
  m_PixDeltaCol(0),
  m_PixOmegaPhi(0),
  m_PixOmegaEta(0),
  m_PixClusToT(0),
  m_PixClusCharge(0),
  m_PixClusLvl1(0),
  m_PixClusGanged(0),
  m_PixClusFake(0),
  m_PixClusAmbiguous(0),
  m_PixClusLocXana(0),
  m_PixClusLocYana(0),
  m_PixClusErrX(0),
  m_PixClusErrY(0),
  m_PixClusEtaIndex(0),
  m_PixClusPhiIndex(0),
  m_PixClusChargeList(0),
  m_PixClusToTList(0),
  m_PixClusLVL1AList(0),
  m_PixClusIsSplit(0),
  m_PixClusSplitProb1(0),
  m_PixClusSplitProb2(0),
  m_PixHitXstartpos(0),
  m_PixHitYstartpos(0),
  m_PixHitZstartpos(0),
  m_PixHitXendpos(0),
  m_PixHitYendpos(0),
  m_PixHitZendpos(0),
  m_PixHitBarrelEndcap(0),
  m_PixHitLayerDisk(0),
  m_PixHitEtaModule(0),
  m_PixHitPhiModule(0),
  m_PixHitEtaIndex(0),
  m_PixHitPhiIndex(0),
  m_PixHitAngle(0),
  m_PixHitAngle2(0),
  m_PixHitAngle_Lorcorr(0),
  m_PixHitEnergyLoss(0),
  m_PixHitTime(0),
  m_PixHitPDGParticle(0),
  m_PixHitFlag(0),
  //    m_PixHitEtaIndex_delta(0),
  //    m_PixHitPhiIndex_delta(0),
  m_MC_Xpos(0),
  m_MC_Ypos(0),
  m_MC_IncidentAngle(0),
  m_MC_IncidentAngle2(0),
  m_MC_IncidentAngle_Lorcorr(0),
  m_MC_EnergyLoss(0),
  m_MC_Time(0),
  m_MC_PDGParticle(0),
  m_MC_Flag(0),
  m_NotAssociated_Xpos(0),
  m_NotAssociated_Ypos(0),
  m_NotAssociated_BarrelEndcap(0),
  m_NotAssociated_LayerDisk(0),
  m_NotAssociated_EtaModule(0),
  m_NotAssociated_PhiModule(0),
  m_NotAssociated_IncidentAngle(0),
  m_NotAssociated_IncidentAngle2(0),
  m_NotAssociated_IncidentAngle_Lorcorr(0),
  m_NotAssociated_EnergyLoss(0),
  m_NotAssociated_Time(0),
  m_NotAssociated_PDGParticle(0),
  m_NotAssociated_Flag(0),
  m_MultipleAssociated_Index(0),
  m_NN_sizeX(0), 
  m_NN_sizeY(0),
  m_NN_matrixOfToT(0),
  m_NN_vectorOfPitchesY(0),
  m_NN_ClusterPixLayer(0),
  m_NN_ClusterPixBarrelEC(0),
  m_NN_phiBS(0),
  m_NN_thetaBS(0),
  m_NN_etaModule(0),
  m_NN_useTrackInfo(0),
  m_NN_columnWeightedPosition(0),
  m_NN_rowWeightedPosition(0),
  m_NN_localColumnWeightedPosition(0),
  m_NN_localRowWeightedPosition(0),
  m_NN_positionX(0),
  m_NN_positionY(0),
  m_NN_position_idX(0),
  m_NN_position_idY(0),
  m_NN_theta(0),
  m_NN_phi(0)
{
  declareProperty("PixelClusterContainer", mjo_riocontainername);
  declareProperty("SiHitCollection",    mjo_hitcontainername);
  declareProperty("PixelRDOCollection", mjo_rdocontainername);
  declareProperty("TrackCollection", mjo_trackCollectionName);
  declareProperty("DoHits", mjo_Hits);
  declareProperty("DoMC", mjo_MC);
  declareProperty("WriteNNTraining", mjo_NN);
  declareProperty("UseToT", mjo_ToT);
  declareProperty("OnlyClusterOnTrack", mjo_onTrack);
  declareProperty("NtupleNNTreeName", m_ntupleNNTreeName);
  declareProperty("WriteDetailedPixelInformation", mjo_WriteDetailedPixelInformation);
  declareProperty("FindNotAssociatedParticle", mjo_NotAssociated);
  declareProperty("NtupleFileName", m_ntupleFileName);
  declareProperty("NtupleDirectoryName", m_ntupleDirName);
  declareProperty("NtupleTreeName", m_ntupleTreeName);
  declareProperty("PixelOfflineCalibSvc", m_pixelCalib);   
}

//----------------------------- ~PixelClusterValidationNtupleWriter ---------------------------
InDet::PixelClusterValidationNtupleWriter::~PixelClusterValidationNtupleWriter() {}


//----------------------------- PixelClusterValidationNtupleWriter::initialize ----------------
StatusCode InDet::PixelClusterValidationNtupleWriter::initialize() {
  ATH_MSG_INFO( "PixelClusterValidationNtupleWriter initialize()" );

  // ----------------------------
  // retrive pointer to THistSvc
  // ----------------------------
  ITHistSvc *tHistSvc;
  StatusCode sc =  service("THistSvc", tHistSvc);
  if ( sc.isFailure() ) {
      ATH_MSG_ERROR( "Unable to retrieve pointer to THistSvc" );
      return sc;
  }
   
  if (m_pixelCalib.retrieve().isFailure() ) {
      ATH_MSG_ERROR( "Failed to retrieve service " << m_pixelCalib );
      return StatusCode::FAILURE;
  } else 
      ATH_MSG_DEBUG( "Retrieved service " << m_pixelCalib );
     
  // ----------------------------
  // create tree and register it to THistSvc
  // ----------------------------
  m_nt = new TTree(TString(m_ntupleTreeName), "Clusters");
  // NB: we must not delete the tree, this is done by THistSvc
  std::string fullNtupleName =  "/"+m_ntupleFileName+"/"+m_ntupleDirName+"/"+m_ntupleTreeName;
  sc = tHistSvc->regTree(fullNtupleName, m_nt);
  if (sc.isFailure()) {
      ATH_MSG_ERROR( "Unable to register TTree: " << fullNtupleName );
      return sc;
  }

  
  if(mjo_NN){    
    m_nnt = new TTree(TString(m_ntupleNNTreeName), "NNtraining");
    // NB: we must not delete the tree, this is done by THistSvc
    std::string fullNtupleName =  "/"+m_ntupleFileName+"/"+m_ntupleDirName+"/"+m_ntupleNNTreeName;
    ATH_MSG_INFO( "NNTraining TTree: " << fullNtupleName );

    sc = tHistSvc->regTree(fullNtupleName, m_nnt);
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Unable to register TTree: " << fullNtupleName );
      return sc;
    }
  }

  if (m_iBeamCondSvc.retrieve().isFailure())
  {
    msg(MSG::ERROR) << "Could not find BeamCondSvc." << endreq;
    return StatusCode::FAILURE;
  }
    
  if (m_IBLParameterSvc.retrieve().isFailure()) {
    ATH_MSG_WARNING( "Could not retrieve IBLParameterSvc");
  }
  else {
    m_IBLParameterSvc->setBoolParameters(m_IBLAbsent,"IBLAbsent");
  } 
    
    
  _slhc_layer_flip_1 = -1;
  _slhc_layer_flip_2 = -1;
  _slhc_layer_flip_3 = -1;  
    
    
  // ----------------------------
  // get the geometry
  // ----------------------------  

  if ( !m_IBLAbsent ) {
    ATH_MSG_INFO("Found IBL+Atlas"  );
    _geoId = IBL;
    _pitchY = ATLASPITCHY; //nb. only the inner layer has a different _pitchY, This case is tested later
  }
  //
  //// Cartigny Layout
  else if (m_geoModel->atlasVersion().find("ATLAS-SLHC-01-02")!=std::string::npos) {
    ATH_MSG_INFO("Found ATLAS SLHC Cartigny/LoI Layout: " << m_geoModel->atlasVersion() );
    _geoId = SLHC2;
    _pitchY = SLHCPITCHY; // Def to .25, if layer <0 pitch to .15 (see below L1916) 
    // In ATLAS_SLHC_01-02-01 layers 0, 2 & 4, Odd eta modules are fliped.
    _slhc_layer_flip_1 = 0;
    _slhc_layer_flip_2 = 2;
    _slhc_layer_flip_3 = 4;  
   
  } else if (m_geoModel->atlasVersion().find("SLHC")!=std::string::npos) {
    ATH_MSG_INFO("Found SLHC Atlas Version from geomodel: " << m_geoModel->atlasVersion() );
    _geoId = SLHC;
    _pitchY = SLHCPITCHY;
    // In layers 0,1 and 5, Odd eta modules are fliped.
    _slhc_layer_flip_1 = 0;
    _slhc_layer_flip_2 = 1;
    _slhc_layer_flip_3 = 5;  
  }else {
    ATH_MSG_INFO("No UPGRADE Geometry Version found from geomodel: " << m_geoModel->atlasVersion() << " Assuming Run1 Geometry" );
    _geoId = ATLAS;
    _pitchY = ATLASPITCHY;
  }





  // if you want to write the NN input you also need pixel detailed info 
  // if(mjo_NN){   mjo_Hits = true; mjo_MC = true;  mjo_NotAssociated= true; }
  
  
  if (detStore()->retrieve(m_pixelid, "PixelID").isFailure()) {
    ATH_MSG_FATAL ("Could not get Pixel ID helper");
    return StatusCode::FAILURE;
  }

  StatusCode scg = service("GeoModelSvc",m_geoModel);
  if (scg.isFailure()) {
    ATH_MSG_FATAL ("GeoModelSvc not found !");
    return StatusCode::FAILURE;
  }
    
  // ----------------------------
  // add items
  // ----------------------------
  // event info:
  // ----------------------------
  // sc = nt->addItem("TRT_RunNumber", m_runNumber) ;       // Run number
  m_nt->Branch("PixelEventNumber", &m_eventNumber, "event_number/I") ;   // Event number
  //m_nt->Branch("PixelnRIOs", &m_nRIOs,       "Number_of_RIOs/I") ;   // number of Pixel RIOs
  m_nt->Branch("PixelClusNum", &m_pixClusNum, "number_cluster_per_event/I") ; // number of clusters per event
  m_nt->Branch("LVL1TriggerType", &m_lvl1TriggerType, "type_of_trigger/I");

  // ----------------------------
  // RIO info:
  // ----------------------------
  m_PixClusLocX = new std::vector<float>();
  m_PixClusLocY = new std::vector<float>();
  m_PixClusLocXcentroid = new std::vector<float>();
  m_PixClusLocYcentroid = new std::vector<float>();
  m_PixClusLocX_LorentzCorrection = new std::vector<float>();
  m_PixClusGloX = new std::vector<float>();
  m_PixClusGloY = new std::vector<float>();
  m_PixClusGloZ = new std::vector<float>();
  m_PixClusEta = new std::vector<float>();
  m_PixClusPhi = new std::vector<float>();
  m_PixECBarrel = new std::vector<float>();
  m_PixClusLayer = new std::vector<float>();
  m_PixEtaModule = new std::vector<float>();
  m_PixPhiModule = new std::vector<float>();
  m_PixClusGroupsize = new std::vector<float>();
  m_PixClusRow = new std::vector<float>();
  m_PixClusCol = new std::vector<float>();
  m_PixDeltaRow = new std::vector<int>();
  m_PixDeltaCol = new std::vector<int>();
  m_PixOmegaPhi = new std::vector<float>();
  m_PixOmegaEta = new std::vector<float>();
  m_PixClusToT = new std::vector<int>();
  m_PixClusCharge = new std::vector<float>();
  m_PixClusLvl1 = new std::vector<int>();
  m_PixClusGanged = new std::vector<int>();
  m_PixClusFake = new std::vector<int>();
  m_PixClusAmbiguous = new std::vector<int>();
  m_PixClusLocXana = new std::vector<float>();
  m_PixClusLocYana = new std::vector<float>();
  m_PixClusErrX = new std::vector<float>();
  m_PixClusErrY = new std::vector<float>();
  m_PixClusEtaIndex = new std::vector<std::vector<int> >();
  m_PixClusPhiIndex = new std::vector<std::vector<int> >();
  m_PixClusChargeList = new std::vector<std::vector<float> >();
  m_PixClusToTList = new std::vector<std::vector<int> >();
  m_PixClusLVL1AList = new std::vector<std::vector<int> >();
  m_PixClusIsSplit = new std::vector<int>();
  m_PixClusSplitProb1 = new std::vector<float>();
  m_PixClusSplitProb2 = new std::vector<float>();


  m_nt->Branch("PixClusLocX", &m_PixClusLocX);
  m_nt->Branch("PixClusLocY", &m_PixClusLocY);
  m_nt->Branch("PixClusLocXcentroid", &m_PixClusLocXcentroid);
  m_nt->Branch("PixClusLocYcentroid", &m_PixClusLocYcentroid);
  m_nt->Branch("PixClusLocX_LorentzCorrection", &m_PixClusLocX_LorentzCorrection);
  m_nt->Branch("PixClusGloX", &m_PixClusGloX);
  m_nt->Branch("PixClusGloY", &m_PixClusGloY);
  m_nt->Branch("PixClusGloZ", &m_PixClusGloZ);
  m_nt->Branch("PixClusEta", &m_PixClusEta);
  m_nt->Branch("PixClusPhi", &m_PixClusPhi);
  m_nt->Branch("PixECBarrel", &m_PixECBarrel);
  m_nt->Branch("PixClusLayer", &m_PixClusLayer);
  m_nt->Branch("PixEtaModule", &m_PixEtaModule);
  m_nt->Branch("PixPhiModule", &m_PixPhiModule);
  m_nt->Branch("PixClusGroupsize", &m_PixClusGroupsize);
  m_nt->Branch("PixClusRow", &m_PixClusRow);
  m_nt->Branch("PixClusCol", &m_PixClusCol);
  m_nt->Branch("PixDeltaRow", &m_PixDeltaRow);
  m_nt->Branch("PixDeltaCol", &m_PixDeltaCol);
  m_nt->Branch("PixOmegaPhi", &m_PixOmegaPhi);
  m_nt->Branch("PixOmegaEta", &m_PixOmegaEta);
  m_nt->Branch("PixClusToT", &m_PixClusToT);
  m_nt->Branch("PixClusCharge", &m_PixClusCharge);
  m_nt->Branch("PixClusLvl1", &m_PixClusLvl1);
  m_nt->Branch("PixClusGanged", &m_PixClusGanged);
  m_nt->Branch("PixClusFake", &m_PixClusFake);
  m_nt->Branch("PixClusAmbiguous", &m_PixClusAmbiguous);
  m_nt->Branch("PixClusIsSplit", &m_PixClusIsSplit);
  m_nt->Branch("PixClusSplitProb1", &m_PixClusSplitProb1);
  m_nt->Branch("PixClusSplitProb2", &m_PixClusSplitProb2);
  m_nt->Branch("PixClusErrX", &m_PixClusErrX);
  m_nt->Branch("PixClusErrY", &m_PixClusErrY);


  if ( mjo_WriteDetailedPixelInformation ){
    ATH_MSG_INFO("Creating branches for detailed pixel information");
    m_nt->Branch("PixClusEtaIndex",&m_PixClusEtaIndex) ;
    m_nt->Branch("PixClusPhiIndex",&m_PixClusPhiIndex) ;
    m_nt->Branch("PixClusChargeList",&m_PixClusChargeList) ;
    m_nt->Branch("PixClusToTList",&m_PixClusToTList) ;
    m_nt->Branch("PixClusLVLAList",&m_PixClusLVL1AList) ;
  }

  // ----------------------------
  // Hit info:
  // ----------------------------
  m_PixHitXstartpos = new std::vector<float>();
  m_PixHitYstartpos = new std::vector<float>();
  m_PixHitZstartpos = new std::vector<float>();
  m_PixHitXendpos = new std::vector<float>();
  m_PixHitYendpos = new std::vector<float>();
  m_PixHitZendpos = new std::vector<float>();
  m_PixHitBarrelEndcap = new std::vector<int>();
  m_PixHitLayerDisk = new std::vector<int>();
  m_PixHitEtaModule = new std::vector<int>();
  m_PixHitPhiModule = new std::vector<int>();
  m_PixHitEtaIndex = new std::vector<int>();
  m_PixHitPhiIndex = new std::vector<int>();
  m_PixHitAngle = new std::vector<float>();
  m_PixHitAngle2 = new std::vector<float>();
  m_PixHitAngle_Lorcorr = new std::vector<float>();
  m_PixHitEnergyLoss = new std::vector<float>();
  m_PixHitTime = new std::vector<float>();
  m_PixHitPDGParticle = new std::vector<int>();
  m_PixHitFlag = new std::vector<int>();
  m_PixHitBarcode = new std::vector<int>();
  m_PixHitParticleP = new std::vector<float>();
//    m_PixHitEtaIndex_delta = new std::vector<int>();
//    m_PixHitPhiIndex_delta = new std::vector<int>();

  // ----------------------------
  // Particle info:
  // ----------------------------
  m_MC_Xpos = new std::vector<float>();
  m_MC_Ypos = new std::vector<float>();
  m_MC_IncidentAngle = new std::vector<float>();
  m_MC_IncidentAngle2 = new std::vector<float>();
  m_MC_IncidentAngle_Lorcorr = new std::vector<float>();
  m_MC_EnergyLoss = new std::vector<float>();
  m_MC_Time = new std::vector<float>();
  m_MC_PDGParticle = new std::vector<int>();
  m_MC_Flag = new std::vector<int>();
  m_MC_Barcode = new std::vector<int>();
  m_MC_Momentum = new std::vector<float>();

  m_MultipleAssociated_Index = new std::vector<std::vector<int> >();

  // ----------------------------
  // Not Associated Particle info:
  // ----------------------------
  m_NotAssociated_Xpos = new std::vector<float>();
  m_NotAssociated_Ypos = new std::vector<float>();
  m_NotAssociated_BarrelEndcap = new std::vector<int>();
  m_NotAssociated_LayerDisk = new std::vector<int>();
  m_NotAssociated_EtaModule = new std::vector<int>();
  m_NotAssociated_PhiModule = new std::vector<int>();
  m_NotAssociated_IncidentAngle = new std::vector<float>();
  m_NotAssociated_IncidentAngle2 = new std::vector<float>();
  m_NotAssociated_IncidentAngle_Lorcorr = new std::vector<float>();
  m_NotAssociated_EnergyLoss = new std::vector<float>();
  m_NotAssociated_Time = new std::vector<float>();
  m_NotAssociated_PDGParticle = new std::vector<int>();
  m_NotAssociated_Flag = new std::vector<int>();

/// ----------------------------------
/// NN training INFO
/// ----------------------------------

  m_NN_sizeX = new std::vector<int>(); 
  m_NN_sizeY = new std::vector<int>() ; 
  //    m_NN_matrixOfToT = new std::vector<std::vector<std::vector<float> > >();
  m_NN_matrixOfToT = new std::vector<std::vector<float> > ();
  m_NN_vectorOfPitchesY= new std::vector<std::vector<float> >();
  m_NN_ClusterPixLayer= new std::vector<int>();
  m_NN_ClusterPixBarrelEC= new std::vector<int>();
  m_NN_phiBS= new std::vector<float>() ;
  m_NN_thetaBS= new std::vector<float>();
  m_NN_etaModule= new std::vector<float>();
  m_NN_useTrackInfo= new std::vector<bool>() ; 
  m_NN_columnWeightedPosition= new std::vector<int>() ;
  m_NN_localRowWeightedPosition= new std::vector<double>();
  m_NN_localColumnWeightedPosition= new std::vector<double>() ;
  m_NN_rowWeightedPosition= new std::vector<int>();
  m_NN_positionX = new std::vector<std::vector<float> > ();
  m_NN_positionY = new std::vector<std::vector<float> > ();
  m_NN_position_idX = new std::vector<std::vector<float> > ();
  m_NN_position_idY = new std::vector<std::vector<float> > ();
  m_NN_theta = new std::vector<std::vector<float> > ();
  m_NN_phi = new std::vector<std::vector<float> > ();
  m_NN_pdgid = new std::vector<std::vector<int> > ();
  m_NN_barcode = new std::vector<std::vector<int> > ();
  m_NN_momentum = new std::vector<std::vector<float> > ();
  m_NN_distanceX = new std::vector<std::vector<float> > ();
  m_NN_distanceY = new std::vector<std::vector<float> > ();


  if (mjo_NN==true) {
    m_nnt->Branch("NN_sizeX", &m_NN_sizeX);
    m_nnt->Branch("NN_sizeY", &m_NN_sizeY);
    m_nnt->Branch("NN_matrixOfToT", &m_NN_matrixOfToT);
    m_nnt->Branch("NN_vectorOfPitchesY", &m_NN_vectorOfPitchesY);
    m_nnt->Branch("NN_ClusterPixLayer" ,&m_NN_ClusterPixLayer);
    m_nnt->Branch("NN_ClusterPixBarrelEC", &m_NN_ClusterPixBarrelEC);
    m_nnt->Branch("NN_phiBS", &m_NN_phiBS);
    m_nnt->Branch("NN_thetaBS",&m_NN_thetaBS);
    m_nnt->Branch("NN_etaModule",&m_NN_etaModule);
    m_nnt->Branch("NN_useTrackInfo", &m_NN_useTrackInfo);
    m_nnt->Branch("NN_columnWeightedPosition", &m_NN_columnWeightedPosition);
    m_nnt->Branch("NN_rowWeightedPosition", &m_NN_rowWeightedPosition);
    m_nnt->Branch("NN_localColumnWeightedPosition", &m_NN_localColumnWeightedPosition);
    m_nnt->Branch("NN_localRowWeightedPosition", &m_NN_localRowWeightedPosition);

    m_nnt->Branch("NN_positionX", &m_NN_positionX);
    m_nnt->Branch("NN_positionY", &m_NN_positionY);
    m_nnt->Branch("NN_position_idX", &m_NN_position_idX);
    m_nnt->Branch("NN_position_idY", &m_NN_position_idY);
    m_nnt->Branch("NN_theta", &m_NN_theta);
    m_nnt->Branch("NN_phi", &m_NN_phi);
    
    
    m_nnt->Branch("NN_pdgid", &m_NN_pdgid);
    m_nnt->Branch("NN_barcode", &m_NN_barcode);
    m_nnt->Branch("NN_momentum", &m_NN_momentum); 
    m_nnt->Branch("NN_distanceX", &m_NN_distanceX);
    m_nnt->Branch("NN_distanceY", &m_NN_distanceY);
    

  }
    
  if (mjo_Hits==true) {
    m_nt->Branch("PixelHitsNum", &m_pixHitsNum, "number_hits_per_event/I") ; // number of hits per event
    m_nt->Branch("PixHitXstartpos", &m_PixHitXstartpos);
    m_nt->Branch("PixHitYstartpos", &m_PixHitYstartpos);
    m_nt->Branch("PixHitZstartpos", &m_PixHitZstartpos);
    m_nt->Branch("PixHitXendpos", &m_PixHitXendpos);
    m_nt->Branch("PixHitYendpos", &m_PixHitYendpos);
    m_nt->Branch("PixHitZendpos", &m_PixHitZendpos);
    m_nt->Branch("PixHitBarrelEndcap", &m_PixHitBarrelEndcap);
    m_nt->Branch("PixHitLayerDisk", &m_PixHitLayerDisk);
    m_nt->Branch("PixHitEtaModule", &m_PixHitEtaModule);
    m_nt->Branch("PixHitPhiModule", &m_PixHitPhiModule);
    m_nt->Branch("PixHitEtaIndex", &m_PixHitEtaIndex);
    m_nt->Branch("PixHitPhiIndex", &m_PixHitPhiIndex);
    m_nt->Branch("PixHitAngle", &m_PixHitAngle);
    m_nt->Branch("PixHitAngle2", &m_PixHitAngle2);
    m_nt->Branch("PixHitAngle_Lorcorr", &m_PixHitAngle_Lorcorr);
    m_nt->Branch("PixHitEnergyLoss", &m_PixHitEnergyLoss);
    m_nt->Branch("PixHitTime", &m_PixHitTime);
    m_nt->Branch("PixHitPDGParticle", &m_PixHitPDGParticle);
    m_nt->Branch("PixHitFlag", &m_PixHitFlag);
    m_nt->Branch("PixHitBarcode", &m_PixHitBarcode);
    m_nt->Branch("PixHitParticleP", &m_PixHitParticleP);
  }

  if (mjo_MC==true) {
    m_nt->Branch("MC_Xpos", &m_MC_Xpos);
    m_nt->Branch("MC_Ypos", &m_MC_Ypos);
    m_nt->Branch("MC_IncidentAngle", &m_MC_IncidentAngle);
    m_nt->Branch("MC_IncidentAngle2", &m_MC_IncidentAngle2);
    m_nt->Branch("MC_IncidentAngle_Lorcorr", &m_MC_IncidentAngle_Lorcorr);
    m_nt->Branch("MC_EnergyLoss", &m_MC_EnergyLoss);
    m_nt->Branch("MC_Time", &m_MC_Time);
    m_nt->Branch("MC_PDGParticle", &m_MC_PDGParticle);
    m_nt->Branch("MC_Flag", &m_MC_Flag);
    m_nt->Branch("MC_Barcode", &m_MC_Barcode);
    m_nt->Branch("MC_Momentum", &m_MC_Momentum);

    m_nt->Branch("PixClusLocXana", &m_PixClusLocXana);
    m_nt->Branch("PixClusLocYana", &m_PixClusLocYana);
    //      m_nt->Branch("PixClusErrX", &m_PixClusErrX);
    //  m_nt->Branch("PixClusErrY", &m_PixClusErrY);

    m_nt->Branch("MultipleAssociatedIndex",&m_MultipleAssociated_Index);
  }

  if (mjo_NotAssociated==true) {
    m_nt->Branch("NotAssociated_Xpos", &m_NotAssociated_Xpos);
    m_nt->Branch("NotAssociated_Ypos", &m_NotAssociated_Ypos);
    m_nt->Branch("NotAssociated_BarrelEndcap", &m_NotAssociated_BarrelEndcap);
    m_nt->Branch("NotAssociated_LayerDisk", &m_NotAssociated_LayerDisk);
    m_nt->Branch("NotAssociated_EtaModule", &m_NotAssociated_EtaModule);
    m_nt->Branch("NotAssociated_PhiModule", &m_NotAssociated_PhiModule);
    m_nt->Branch("NotAssociated_IncidentAngle", &m_NotAssociated_IncidentAngle);
    m_nt->Branch("NotAssociated_IncidentAngle2", &m_NotAssociated_IncidentAngle2);
    m_nt->Branch("NotAssociated_IncidentAngle_Lorcorr", &m_NotAssociated_IncidentAngle_Lorcorr);
    m_nt->Branch("NotAssociated_EnergyLoss", &m_NotAssociated_EnergyLoss);
    m_nt->Branch("NotAssociated_Time", &m_NotAssociated_Time);
    m_nt->Branch("NotAssociated_PDGParticle", &m_NotAssociated_PDGParticle);
    m_nt->Branch("NotAssociated_Flag", &m_NotAssociated_Flag);
  }
  return sc;
}

//----------------------------- PixelClusterValidationNtupleWriter::execute -------------------
StatusCode InDet::PixelClusterValidationNtupleWriter::execute() {
   
  // ----------------------------
  // get the container with Pixel RIOs and HITs
  // ----------------------------
  m_riocontainer = 0;
  StatusCode sc = evtStore()->retrieve(m_riocontainer, mjo_riocontainername);
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Could not get PrepRawDataContainer" );
    return StatusCode::FAILURE;
  }

  m_hitcontainer = 0;
  if ( mjo_Hits || mjo_MC || mjo_NotAssociated || mjo_NN) {
    sc = evtStore()->retrieve(m_hitcontainer, mjo_hitcontainername);
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Could not get SiHitCollection" );
      return StatusCode::FAILURE;
    }
  }

  if ( mjo_WriteDetailedPixelInformation ) {
    sc = evtStore()->retrieve(m_rdocontainer, mjo_rdocontainername);
    if ( sc.isFailure() ) {
      ATH_MSG_WARNING( "Could not find container " << mjo_rdocontainername <<" : disabling detailed information output" );
      m_rdocontainer = 0;
      mjo_WriteDetailedPixelInformation = false;
    }
  }
  m_trackCollection=0;
  if( mjo_onTrack){
    sc = evtStore()->retrieve(m_trackCollection,mjo_trackCollectionName);
    if ( sc.isFailure() ){
      ATH_MSG_ERROR("Could not retrieve TrackCollection '" << mjo_trackCollectionName << "'.");
      return StatusCode::FAILURE;
    }
  }

    // ----------------------------
  // get the event number
  // ----------------------------
  const xAOD::EventInfo* eventInfo;
  sc = evtStore()->retrieve(eventInfo);
  if (sc.isFailure()) {
      ATH_MSG_ERROR( "Could not retrieve event info" );
  }
  m_eventNumber = eventInfo->eventNumber();
  m_lvl1TriggerType = eventInfo->level1TriggerType();

  //    int iterator_eventnumber=-1;
  //    int RIOindex = 0;
  //    const int maxRIO_size = 1000000;
  int HITindex = 0;
  m_nRIOs = 0;
  m_PixClusLocX->clear();
  m_PixClusLocY->clear();
  m_PixClusLocXcentroid->clear();
  m_PixClusLocYcentroid->clear();
  m_PixClusLocX_LorentzCorrection->clear();
  m_PixClusGloX->clear();
  m_PixClusGloY->clear();
  m_PixClusGloZ->clear();
  m_PixClusEta->clear();
  m_PixClusPhi->clear();
  m_PixECBarrel->clear();
  m_PixClusLayer->clear() ;
  m_PixEtaModule->clear();
  m_PixPhiModule->clear();
  m_PixClusGroupsize->clear();
  m_PixClusRow->clear();
  m_PixClusCol->clear();
  m_PixDeltaRow->clear();
  m_PixDeltaCol->clear();
  m_PixOmegaPhi->clear();
  m_PixOmegaEta->clear();
  m_PixClusToT->clear();
  m_PixClusCharge->clear();
  m_PixClusLvl1->clear();
  m_PixClusGanged->clear();
  m_PixClusFake->clear();
  m_PixClusAmbiguous->clear();
  m_PixClusLocXana->clear();
  m_PixClusLocYana->clear();
  m_PixClusErrX->clear();
  m_PixClusErrY->clear();
  m_PixClusEtaIndex->clear();
  m_PixClusPhiIndex->clear();
  m_PixClusChargeList->clear();
  m_PixClusToTList->clear();
  m_PixClusLVL1AList->clear();
  m_PixClusIsSplit->clear();
  m_PixClusSplitProb1->clear();
  m_PixClusSplitProb2->clear();
  m_PixHitXstartpos->clear();
  m_PixHitYstartpos->clear();
  m_PixHitZstartpos->clear();
  m_PixHitXendpos->clear();
  m_PixHitYendpos->clear();
  m_PixHitZendpos->clear();
  m_PixHitBarrelEndcap->clear();
  m_PixHitLayerDisk->clear();
  m_PixHitEtaModule->clear();
  m_PixHitPhiModule->clear();
  m_PixHitEtaIndex->clear();
  m_PixHitPhiIndex->clear();
  m_PixHitAngle->clear();
  m_PixHitAngle2->clear();
  m_PixHitAngle_Lorcorr->clear();
  m_PixHitEnergyLoss->clear();
  m_PixHitTime->clear();
  m_PixHitPDGParticle->clear();
  m_PixHitFlag->clear();
  m_PixHitBarcode->clear();
  m_PixHitParticleP->clear();
  m_MultipleAssociated_Index->clear();
//    m_PixHitEtaIndex_delta->clear();
//    m_PixHitPhiIndex_delta->clear();
  
  m_MC_Xpos->clear();
  m_MC_Ypos->clear();
  m_MC_IncidentAngle->clear();
  m_MC_IncidentAngle2->clear();
  m_MC_IncidentAngle_Lorcorr->clear();
  m_MC_EnergyLoss->clear();
  m_MC_Time->clear();
  m_MC_PDGParticle->clear();
  m_MC_Flag->clear();
  m_MC_Barcode->clear();
  m_MC_Momentum->clear();

  m_NotAssociated_Xpos->clear();
  m_NotAssociated_Ypos->clear();
  m_NotAssociated_BarrelEndcap->clear();
  m_NotAssociated_LayerDisk->clear();
  m_NotAssociated_EtaModule->clear();
  m_NotAssociated_PhiModule->clear();
  m_NotAssociated_IncidentAngle->clear();
  m_NotAssociated_IncidentAngle2->clear();
  m_NotAssociated_IncidentAngle_Lorcorr->clear();
  m_NotAssociated_EnergyLoss->clear();
  m_NotAssociated_Time->clear();
  m_NotAssociated_PDGParticle->clear();
  m_NotAssociated_Flag->clear();


  m_NN_sizeX->clear();
  m_NN_sizeY->clear();
  m_NN_matrixOfToT->clear();
  m_NN_vectorOfPitchesY->clear();
  m_NN_ClusterPixLayer->clear();
  m_NN_ClusterPixBarrelEC->clear();
  m_NN_phiBS->clear();
  m_NN_thetaBS->clear();
  m_NN_etaModule->clear();
  m_NN_useTrackInfo->clear();
  m_NN_columnWeightedPosition->clear();
  m_NN_rowWeightedPosition->clear();
  m_NN_localColumnWeightedPosition->clear();
  m_NN_localRowWeightedPosition->clear();

  m_NN_positionX->clear();
  m_NN_positionY->clear();
  m_NN_position_idX->clear();
  m_NN_position_idY->clear();

  m_NN_theta->clear();
  m_NN_phi->clear();
  
  m_NN_pdgid->clear();
  m_NN_barcode->clear();
  m_NN_momentum->clear();
  m_NN_distanceX->clear();
  m_NN_distanceY->clear();

  ATH_MSG_DEBUG( "Variables Cleared" );

  //  A vector of NNinput (struct)  one element for each cluster
  //    std::vector<NNinput*>* input =  new std::vector<NNinput*>();
  //    NNinput* input;


  // ----------------------------
  // get all the RIO_Collections in the container
  // ----------------------------


  if(!mjo_onTrack){
      
    InDet::PixelClusterContainer::const_iterator containerIterator = m_riocontainer->begin();
    for ( ; containerIterator != m_riocontainer->end(); containerIterator++) { // loop1
      ATH_MSG_DEBUG( "There are " << (*containerIterator)->size() << " entries in the PrepRawDataCollection" );
      const PixelClusterCollection *colNext=(*containerIterator);
      if (!colNext) {
        continue;
      }
      Identifier colID = colNext->identify();
      IdentifierHash element_hash = m_pixelid->wafer_hash(colID);
      DataVector<PixelCluster>::const_iterator p_clus;

      for (p_clus=colNext->begin(); p_clus!=colNext->end(); ++p_clus) {
        const PixelCluster& cluster = **p_clus;
        sc = fillClusterVector(cluster, element_hash);
        if(sc.isFailure()){
          ATH_MSG_ERROR( "Could not fill cluster vectors" );
        }
      }
    }    
  }else{
    TrackCollection::const_iterator tcIter    = m_trackCollection->begin();
    TrackCollection::const_iterator tcIterEnd = m_trackCollection->end();
    for ( ; tcIter != tcIterEnd; ++tcIter ){
 
 
      const DataVector<const Trk::MeasurementBase>* measurements = (*tcIter)->measurementsOnTrack();
 
      DataVector<const Trk::MeasurementBase>::const_iterator mIter    =  measurements->begin();
      DataVector<const Trk::MeasurementBase>::const_iterator mIterEnd =  measurements->end();
 
 
      int countMeasurements=0;
 
      for ( ; mIter != mIterEnd; ++mIter,++countMeasurements){
   
   
        // cast to Pixel cluster
        const InDet::PixelClusterOnTrack* pcot = dynamic_cast<const InDet::PixelClusterOnTrack*>(*mIter);
   
        if(!pcot)continue;
   
        //      for (p_clus=colNext->begin(); p_clus!=colNext->end(); ++p_clus) {
   
        const InDet::PixelCluster* pCluster  = pcot->prepRawData();
        const InDet::PixelCluster cluster = *pCluster;
 
        const InDetDD::SiDetectorElement* element = pCluster->detectorElement(); 
        if(!element) continue;   // return 0;
        IdentifierHash      element_hash  = element->identifyHash(); 

        sc = fillClusterVector(cluster, element_hash);
        if(sc.isFailure()){
          ATH_MSG_ERROR( "Could not fill cluster vectors" );
        }
      }
    }   
  }
  m_pixClusNum = m_nRIOs;

  // ----------------------------
  // get all the SiHITs_Collections in the container
  // ----------------------------
  if ( mjo_Hits || mjo_MC ||  mjo_NotAssociated || mjo_NN ) {
    SiHitCollection::const_iterator collectionIterator = m_hitcontainer->begin();
    ATH_MSG_DEBUG( "Found " << m_hitcontainer->size() << " SiHit " );
    int k=0;
    for ( ; collectionIterator != m_hitcontainer->end(); collectionIterator++) {
      k++;
      ATH_MSG_DEBUG( "There are entries in the SiHitCollection" );

      const SiHit *curHit = &(*collectionIterator);

      const HepMcParticleLink& pLink = (*curHit).particleLink();
      const HepMC::GenParticle* genP = pLink.cptr();
        
      if( genP ) {
       //if (genP->pdg_id() != 22) std::cout << "Low energy truth particle: "   << genP->barcode() << " "<< genP->pdg_id() << "  with Energy: " << sqrt( pow( genP->momentum().px(), 2 ) + pow( genP->momentum().py(), 2 ) +pow( genP->momentum().pz(), 2 ) ) / GeV << std::endl;
        //(*curHit).print();
        //if( genP->momentum().e() < 1 * MeV ) print = true;
        //std::cout << "Position diff: " << (*curHit).localEndPosition()-(*curHit).localStartPosition() << std::endl;
        m_PixHitPDGParticle->push_back(genP->pdg_id());
        m_PixHitBarcode->push_back( genP->barcode()  );
        m_PixHitParticleP->push_back(  sqrt( pow( genP->momentum().px(), 2 ) + pow( genP->momentum().py(), 2 ) +pow( genP->momentum().pz(), 2 ) ) / GeV );
      } else {
        m_PixHitPDGParticle->push_back(0);
        m_PixHitBarcode->push_back( -1000 );
        m_PixHitParticleP->push_back(  -1000 );
      }
      
          
           

      m_PixHitXstartpos->push_back(float((*curHit).localStartPosition().x()));
      m_PixHitYstartpos->push_back(float((*curHit).localStartPosition().y()));
      m_PixHitZstartpos->push_back(float((*curHit).localStartPosition().z()));

      double Energy;
      Energy = (*curHit).energyLoss()/GeV;
      int flag = 0;

      SiHitCollection::const_iterator compColl = collectionIterator+1; //m_hitcontainer->begin();

      while (fabs((*curHit).localEndPosition().x())<0.124995) {
        if (FindNextHit(curHit,compColl)!=0) {
          const SiHit *nextHit = FindNextHit(curHit,compColl);

          double ang=((*curHit).localEndPosition()-(*curHit).localStartPosition()).angle((*nextHit).localEndPosition()-(*nextHit).localStartPosition());

          if (ang < 0.03) {
            curHit = FindNextHit(curHit,compColl);
            Energy += (*curHit).energyLoss()/GeV;
            flag = 1;
            compColl++;
          } else {
            curHit = FindNextHit(curHit,compColl);
            Energy += (*curHit).energyLoss()/GeV;
            flag = 10;
            compColl++;
          }
        } else {
          flag = 2;
          break;
        }
        collectionIterator++;
      }

      m_PixHitXendpos->push_back(float((*curHit).localEndPosition().x()));
      m_PixHitYendpos->push_back(float((*curHit).localEndPosition().y()));
      m_PixHitZendpos->push_back(float((*curHit).localEndPosition().z()));

      if ((*curHit).getBarrelEndcap()==1) m_PixHitBarrelEndcap->push_back(int((*curHit).getBarrelEndcap())*(-2));
      else m_PixHitBarrelEndcap->push_back(int((*curHit).getBarrelEndcap()));
      m_PixHitLayerDisk->push_back(int((*curHit).getLayerDisk()));
      m_PixHitEtaModule->push_back(int((*curHit).getEtaModule()));
      m_PixHitPhiModule->push_back(int((*curHit).getPhiModule()));

      m_PixHitEnergyLoss->push_back(float(Energy));

      const PixelID* PID = dynamic_cast<const PixelID*>(m_pixelid);
      Identifier id = PID->wafer_id((*curHit).getBarrelEndcap(), (*curHit).getLayerDisk(),(*curHit).getPhiModule(), (*curHit).getEtaModule());
      //      Identifier id = PID->wafer_id((*m_PixHitBarrelEndcap)[HITindex], (*m_PixHitLayerDisk)[HITindex],
      //                    (*m_PixHitPhiModule)[HITindex], (*m_PixHitEtaModule)[HITindex]);
      StatusCode sc = StatusCode::SUCCESS;
      const InDetDD::PixelDetectorManager* pixeldetmngr;
      sc = detStore()->retrieve(pixeldetmngr, "Pixel");

      const InDetDD::SiDetectorElement* sielement=pixeldetmngr->getDetectorElement(id);

      float tanlorentz = sielement->getTanLorentzAnglePhi();
      int readoutside = sielement->design().readoutSide();

      Amg::Vector2D pos=sielement->hitLocalToLocal(((*m_PixHitZstartpos)[HITindex]+(*m_PixHitZendpos)[HITindex])/2,
                                                   ((*m_PixHitYstartpos)[HITindex]+(*m_PixHitYendpos)[HITindex])/2);
      //Amg::Vector2D pos_start=sielement->hitLocalToLocal((*m_PixHitZstartpos)[HITindex],(*m_PixHitYstartpos)[HITindex]);
      //Amg::Vector2D pos_end=sielement->hitLocalToLocal((*m_PixHitZendpos)[HITindex],(*m_PixHitYendpos)[HITindex]);
      InDetDD::SiCellId diode=sielement->cellIdOfPosition(pos);
      //InDetDD::SiCellId diode_start=sielement->cellIdOfPosition(pos_start);
      //InDetDD::SiCellId diode_end=sielement->cellIdOfPosition(pos_end);
      m_PixHitEtaIndex->push_back(int(diode.etaIndex()));
      m_PixHitPhiIndex->push_back(int(diode.phiIndex()));
      //      m_PixHitEtaIndex_delta->push_back(int(fabs(diode_start.etaIndex()-diode_end.etaIndex())));
      //      m_PixHitPhiIndex_delta->push_back(int(fabs(diode_start.phiIndex()-diode_end.phiIndex())));

      ////
      //            if ((*m_PixHitBarrelEndcap)[HITindex]!=0 && ((*m_PixHitPhiModule)[HITindex]%2)==0) {
      //                (*m_PixHitYstartpos)[HITindex]*=(-1.);
      //                (*m_PixHitYendpos)[HITindex]*=(-1.);
      //            }
      //            if ((*m_PixHitBarrelEndcap)[HITindex]==2 && ((*m_PixHitPhiModule)[HITindex]%2)!=0) {
      //                (*m_PixHitXstartpos)[HITindex]*=(-1.);
      //                (*m_PixHitXendpos)[HITindex]*=(-1.);
      //            }
      //            if ((*m_PixHitBarrelEndcap)[HITindex]==-2 && ((*m_PixHitPhiModule)[HITindex]%2)==0) {
      //                (*m_PixHitXstartpos)[HITindex]*=(-1.);
      //                (*m_PixHitXendpos)[HITindex]*=(-1.);
      //            }
      //
      //            m_PixHitAngle->push_back(float(atan(((*m_PixHitYendpos)[HITindex]-(*m_PixHitYstartpos)[HITindex])/((*m_PixHitXendpos)[HITindex]-(*m_PixHitXstartpos)[HITindex]))));
      //            m_PixHitAngle2->push_back(float(atan2(((*m_PixHitZendpos)[HITindex]-(*m_PixHitZstartpos)[HITindex]),((*m_PixHitXendpos)[HITindex]-(*m_PixHitXstartpos)[HITindex]))));
      ////

      //
      // Geometry transformations 
      //
      // SLHC
      if(_geoId==SLHC || _geoId==SLHC2){
        ATH_MSG_DEBUG(" SLHC geo transform.");
        // Odd eta modules in end caps are flipped.
        int layer = (*m_PixHitLayerDisk)[HITindex];
        int etaMod  = (*m_PixHitEtaModule)[HITindex];
        int ecBarrel = (*m_PixHitBarrelEndcap)[HITindex];
        if (ecBarrel!=0){
          // if(layer==0 || layer==1 || layer==5 ){ // SLHC-01-00-00 - UTOPIA
          // if(layer==0 || layer==2 || layer==4 ){ // SLHC-01-20-01 - Cartigny
          if(layer==_slhc_layer_flip_1  || layer==_slhc_layer_flip_2 || layer==_slhc_layer_flip_3 ){    
            if(etaMod%2!=0){
              (*m_PixHitYstartpos)[HITindex]*=(-1.);
              (*m_PixHitYendpos)[HITindex]*=(-1.);
              (*m_PixHitXstartpos)[HITindex]*=(-1.);
              (*m_PixHitXendpos)[HITindex]*=(-1.);  
            }
            // In layers 2,3 and 4, Even eta modules are fliped.
          } else {
            if(etaMod%2==0){
              (*m_PixHitYstartpos)[HITindex]*=(-1.);
              (*m_PixHitYendpos)[HITindex]*=(-1.);
              (*m_PixHitXstartpos)[HITindex]*=(-1.);
              (*m_PixHitXendpos)[HITindex]*=(-1.);
            }
          }

          if(ecBarrel==2){
              (*m_PixHitYstartpos)[HITindex]*=(-1.);
              (*m_PixHitYendpos)[HITindex]*=(-1.);
          }
        }
      }else {
        ATH_MSG_DEBUG(" ATLAS/IBL geo transform.");
        //
        // ATLAS+IBL 
        if ((*m_PixHitBarrelEndcap)[HITindex]!=0 && ((*m_PixHitPhiModule)[HITindex]%2)==0) {
          (*m_PixHitYstartpos)[HITindex]*=(-1.);
          (*m_PixHitYendpos)[HITindex]*=(-1.);
        }
        if ((*m_PixHitBarrelEndcap)[HITindex]==2 && ((*m_PixHitPhiModule)[HITindex]%2)!=0) {
          (*m_PixHitXstartpos)[HITindex]*=(-1.);
          (*m_PixHitXendpos)[HITindex]*=(-1.);
        }
        if ((*m_PixHitBarrelEndcap)[HITindex]==-2 && ((*m_PixHitPhiModule)[HITindex]%2)==0) {
          (*m_PixHitXstartpos)[HITindex]*=(-1.);
          (*m_PixHitXendpos)[HITindex]*=(-1.);
        }    
      }
      // Theta in module coordinates.
      float angle2=float(atan2(((*m_PixHitZendpos)[HITindex]-(*m_PixHitZstartpos)[HITindex]),
                               ((*m_PixHitXendpos)[HITindex]-(*m_PixHitXstartpos)[HITindex])));
      m_PixHitAngle2->push_back(angle2);
      // Phi in module coordinates.
      m_PixHitAngle->push_back(float(atan(((*m_PixHitYendpos)[HITindex]-(*m_PixHitYstartpos)[HITindex])/((*m_PixHitXendpos)[HITindex]-(*m_PixHitXstartpos)[HITindex]))));

      double bowphi = atan2(((*m_PixHitYendpos)[HITindex]-(*m_PixHitYstartpos)[HITindex]),((*m_PixHitXendpos)[HITindex]-(*m_PixHitXstartpos)[HITindex]));

      m_PixHitAngle_Lorcorr->push_back(atan(tan(bowphi)-readoutside*tanlorentz));     

      if (flag==10) {
        if ((*m_PixHitXstartpos)[HITindex]>0.124) flag=11;
      }
      if (fabs(((*m_PixHitXstartpos)[HITindex]+(*m_PixHitXendpos)[HITindex])/2)>0.124) flag=14;

      m_PixHitFlag->push_back(flag);
      m_PixHitTime->push_back(float((*curHit).meanTime()));
     
      ATH_MSG_DEBUG( "G4 hit has flag " << (*m_PixHitFlag)[HITindex] << 
              " and PDG code " << (*m_PixHitPDGParticle)[HITindex] << 
              " on module is " << (*m_PixHitBarrelEndcap)[HITindex] << " " << (*m_PixHitLayerDisk)[HITindex] << 
              " " << (*m_PixHitEtaModule)[HITindex] << " " << (*m_PixHitPhiModule)[HITindex] <<
              " Lorentz angle is " << tanlorentz << 
              " and readoutside is " << readoutside << 
              " and bowphi is " << bowphi << 
              " and angle is " << atan(tan(bowphi)-readoutside*tanlorentz)  );

      HITindex++;
    }

    m_pixHitsNum = HITindex;

    std::vector<float> Xpos(m_pixHitsNum), Ypos(m_pixHitsNum), Zpos(m_pixHitsNum);

    ATH_MSG_DEBUG( "Found " << m_pixHitsNum << " G4Hits." );
    for (int i=0; i<m_pixHitsNum; i++) {
        Xpos[i]=((*m_PixHitXstartpos)[i]+(*m_PixHitXendpos)[i])/2;
        Ypos[i]=((*m_PixHitYstartpos)[i]+(*m_PixHitYendpos)[i])/2;
        Zpos[i]=((*m_PixHitZstartpos)[i]+(*m_PixHitZendpos)[i])/2;
    }

    ATH_MSG_DEBUG( "Found " << m_pixClusNum << " Clusters." );

    for (int i=0; i<m_pixClusNum; i++) {
   
      int group = int((*m_PixClusGroupsize)[i]);
   
//            int part = FindAssociatedParticle(i, m_pixHitsNum, group);

// find the particles associated to a cluster . Since it can happen that there is more than one particle crossing a cluster
// the function FindMultipleAssociatedParticle() returns a vector of index  
// This is backward compatible with the old code (which associate only 1 particle
// since the m_MC_* variables are also filled in the same way i.e. taking the first
// element of this vector 
   
      std::vector<int> MultiplePart;
      MultiplePart = FindMultipleAssociatedParticle(i, m_pixHitsNum, group);
  
      ATH_MSG_VERBOSE("Particles found: " << MultiplePart.size() );

      if(mjo_NN){

        // a vector of positions because there is more than one particle, 

        std::vector<float> positionsX;
        std::vector<float> positionsY;

        // expressed respect with the center of the cluster
        std::vector<float> positions_idX;
        std::vector<float> positions_idY;

        std::vector<float> theta;
        std::vector<float> phi;

        std::vector<int> pdgId;
        std::vector<int> barcode;
        std::vector<float> momentum;
        std::vector<float> distanceX;
        std::vector<float> distanceY;

        // get pixel identifier
        const PixelID* PID = dynamic_cast<const PixelID*>(m_pixelid);
        Identifier id = PID->wafer_id( (*m_PixECBarrel)[i]  , 
                                      (*m_PixClusLayer)[i] ,
                                      (*m_PixPhiModule)[i] , 
                                      (*m_PixEtaModule)[i] );
     

        const InDetDD::PixelDetectorManager* pixeldetmngr;
        sc = detStore()->retrieve(pixeldetmngr, "Pixel");
 
        const InDetDD::SiDetectorElement* element=pixeldetmngr->getDetectorElement(id);

        const InDetDD::PixelModuleDesign* design(dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design()));

        // lorentz shift correction    
        double shift = element->getLorentzCorrection();

        for(unsigned int mp=0; mp<MultiplePart.size(); mp++){

          int partIndex = MultiplePart[mp];

          pdgId.push_back( (*m_PixHitPDGParticle)[partIndex] );
          barcode.push_back( (*m_PixHitBarcode)[partIndex] );
          momentum.push_back( (*m_PixHitParticleP)[partIndex] );
          distanceX.push_back( (*m_PixHitYendpos)[partIndex] - (*m_PixHitYstartpos)[partIndex] );
          distanceY.push_back( (*m_PixHitZendpos)[partIndex] - (*m_PixHitZstartpos)[partIndex] );

          // position lorentz shift corrected
          float YposC = Ypos[partIndex]-shift;

          if (fabs(YposC)>design->width()/2 && 
              fabs(Ypos[partIndex])<design->width()/2)
          {
//                std::cout << " FOUND ADJUSTMENT" << std::endl;
            if (YposC>design->width()/2)
            {
              YposC=design->width()/2-1e-6;
            } else if (YposC<-design->width()/2)
            {
              YposC=-design->width()/2+1e-6;
            }
          }
              
          positionsX.push_back(YposC);
          positionsY.push_back(Zpos[partIndex]);


          InDetDD::SiLocalPosition siLocalTruthPosition(Zpos[partIndex],YposC ) ;
          InDetDD::SiCellId cellIdOfTruthPosition =design->cellIdOfPosition(siLocalTruthPosition);

          int truthEtaIndex =  cellIdOfTruthPosition.etaIndex();
          int truthPhiIndex =  cellIdOfTruthPosition.phiIndex();

          InDetDD::SiDiodesParameters diodeParameters = design->parameters(cellIdOfTruthPosition);
          double pitchY = diodeParameters.width().xEta();
          double pitchX = diodeParameters.width().xPhi();

          // pixel center
          InDetDD::SiLocalPosition siLocalPositionCenter(design->positionFromColumnRow(truthEtaIndex,truthPhiIndex)); 
          double pixelCenterY = siLocalPositionCenter.xEta();
          double pixelCenterX = siLocalPositionCenter.xPhi();


          // truth index
          double truthIndexY =  truthEtaIndex + (Zpos[partIndex] - pixelCenterY)/pitchY;
          double truthIndexX =  truthPhiIndex + (YposC           - pixelCenterX)/pitchX;


          positions_idX.push_back( truthIndexX - (*m_NN_rowWeightedPosition)[i] );
          positions_idY.push_back( truthIndexY - (*m_NN_columnWeightedPosition)[i]);

       // int ECBarrel = (*m_PixECBarrel)[i];
       // if(ECBarrel!=0){
       //  std::cout <<std::dec<< ECBarrel<<" truthPhiIndex " << truthPhiIndex 
       //     << "\t YposC "<<YposC
       //     << "\t PixelCenterX "<<pixelCenterX
       //     << "\t pitchX "<<pitchX  
       //     << "\t truthIndexX   " << truthIndexX 
       //     << "\t rowWeightedPosition  " <<(*m_NN_rowWeightedPosition)[i]
       //     << "\t Position id X "<<truthIndexX - (*m_NN_rowWeightedPosition)[i] 
       //     <<"   "<<ECBarrel
       //    // << " Zpos "<< Zpos[partIndex] 
       //    // << " YposC: " << YposC << " pixelCenterY " << pixelCenterY << " pixelCenterX : " << pixelCenterX 
       //    // << " pitchY " << pitchY << " pitchX " << pitchX << " length: " << design->length() 
       //    // << " width: " << design->width() << " with max: " << design->maxWidth() 
       //     << std::endl;
       // }

          theta.push_back((*m_PixHitAngle2)[partIndex]);
          phi.push_back((*m_PixHitAngle_Lorcorr)[partIndex]);
        }
     
     
        m_NN_positionX->push_back(positionsX);
        m_NN_positionY->push_back(positionsY);


        m_NN_position_idX->push_back(positions_idX);
        m_NN_position_idY->push_back(positions_idY);


        m_NN_theta->push_back(theta);
        m_NN_phi->push_back(phi);

        m_NN_pdgid->push_back(pdgId);
        m_NN_barcode->push_back(barcode);
        m_NN_momentum->push_back(momentum);
        m_NN_distanceX->push_back(distanceX);
        m_NN_distanceY->push_back(distanceY);    
      }
   
       
      m_MultipleAssociated_Index->push_back(MultiplePart);
   
      int Npart = MultiplePart.size();

      if (Npart==0) {
        m_MC_Xpos->push_back(-100);
        m_MC_Ypos->push_back(-100);
        m_MC_IncidentAngle->push_back(-100);
        m_MC_IncidentAngle2->push_back(-100);
        m_MC_IncidentAngle_Lorcorr->push_back(-100);
        m_MC_Flag->push_back(-100);
        m_MC_EnergyLoss->push_back(-100);
        m_MC_PDGParticle->push_back(-100);
        m_MC_Time->push_back(-100);
        m_MC_Barcode->push_back(-100);
        m_MC_Momentum->push_back(-100);
        m_PixClusLocXana->push_back(-100);
        m_PixClusLocYana->push_back(-100);
        //  m_PixClusErrX->push_back(-100);
        //              m_PixClusErrY->push_back(-100);
      } else {
        int part = MultiplePart[0];

        m_MC_Xpos->push_back(Ypos[part]);
        m_MC_Ypos->push_back(Zpos[part]);
        m_MC_IncidentAngle->push_back((*m_PixHitAngle)[part]);
        m_MC_IncidentAngle2->push_back((*m_PixHitAngle2)[part]);
        m_MC_IncidentAngle_Lorcorr->push_back((*m_PixHitAngle_Lorcorr)[part]);
        m_MC_Flag->push_back((*m_PixHitFlag)[part]);
        m_MC_EnergyLoss->push_back(((*m_PixHitEnergyLoss)[part]));
        m_MC_PDGParticle->push_back(((*m_PixHitPDGParticle)[part]));
        m_MC_Time->push_back(((*m_PixHitTime)[part]));
        m_MC_Barcode->push_back(((*m_PixHitBarcode)[part]));
        m_MC_Momentum->push_back(((*m_PixHitParticleP)[part]));

        double corrX;
        double corrY;
        // double errphi;
        // double erreta;

        if((*m_PixECBarrel)[i]==0){
        double angle = (*m_PixHitAngle_Lorcorr)[part]*180/M_PI;
        //if(angle<0) angle*=-1;

        double theta = (*m_PixHitAngle2)[part];
        if(theta > M_PI / 2 ) theta -= M_PI;
        else if(theta < - M_PI / 2 ) theta += M_PI;
        theta = ( M_PI / 2 ) - theta;
        double trketa = -log(tan(theta/2));

        corrX = m_pixelCalib->getBarrelDeltaX((*m_PixDeltaRow)[i],angle);
        corrY = m_pixelCalib->getBarrelDeltaY((*m_PixDeltaCol)[i],trketa);

        // new error code
        //errphi = m_pixelCalib->getBarrelNewErrorPhi(angle,(*m_PixDeltaRow)[i]);
        //erreta = m_pixelCalib->getBarrelErrorEta(trketa,(*m_PixDeltaCol)[i],(*m_PixDeltaRow)[i]);
        // end error code
      }else{
        corrX = m_pixelCalib->getEndcapDeltaX();
        corrY = m_pixelCalib->getEndcapDeltaY();
        // new error code
        //errphi = m_pixelCalib->getEndCapErrorPhi((*m_PixDeltaCol)[i],(*m_PixDeltaRow)[i]);
        //erreta = m_pixelCalib->getEndCapErrorEta((*m_PixDeltaCol)[i],(*m_PixDeltaRow)[i]);
        // end error code
      }
       
       
      ATH_MSG_DEBUG( "Cluster " << i << " on module " <<  (*m_PixECBarrel)[i] << " " << (*m_PixClusLayer)[i] 
                    << " " << (*m_PixEtaModule)[i] << " " << (*m_PixPhiModule)[i] 
                    << " ClusSize " << (*m_PixDeltaRow)[i] << " " << (*m_PixDeltaCol)[i] 
                    << " Omega " << (*m_PixOmegaPhi)[i] << " " << (*m_PixOmegaEta)[i] 
                    << " Angle " << (*m_PixHitAngle_Lorcorr)[part]*180/M_PI << " Eta " << (*m_PixHitAngle2)[part]);

      ATH_MSG_DEBUG( "Correction " << corrX << " " << corrY );

      corrX *=((*m_PixOmegaPhi)[i]-0.5);
      corrY *=((*m_PixOmegaEta)[i]-0.5);

      ATH_MSG_DEBUG( "Correction " << corrX << " " << corrY );

      m_PixClusLocXana->push_back((*m_PixClusLocXcentroid)[i]+corrX);
      m_PixClusLocYana->push_back((*m_PixClusLocYcentroid)[i]+corrY);  
      //       m_PixClusErrX->push_back(errphi);
      //       m_PixClusErrY->push_back(erreta);  

      ATH_MSG_DEBUG( "So we get in X " << (*m_PixClusLocXcentroid)[i] << " -> " <<  (*m_PixClusLocXcentroid)[i]+corrX );
      ATH_MSG_DEBUG( "So we get in Y " << (*m_PixClusLocYcentroid)[i] << " -> " <<  (*m_PixClusLocYcentroid)[i]+corrY );

    }
  }
  
    int PartNum = (*m_MC_Xpos).size();
    for (int i=0; i<m_pixHitsNum; i++) {
      int m=0;
      for (m=0; m<PartNum; m++) {
        if (Ypos[i]==(*m_MC_Xpos)[m] && Zpos[i]==(*m_MC_Ypos)[m]) break;
      }
      if (m==PartNum) {
        m_NotAssociated_Xpos->push_back(Ypos[i]);
        m_NotAssociated_Ypos->push_back(Zpos[i]);
        m_NotAssociated_IncidentAngle->push_back((*m_PixHitAngle)[i]);
        m_NotAssociated_IncidentAngle2->push_back((*m_PixHitAngle2)[i]);
        m_NotAssociated_Flag->push_back((*m_PixHitFlag)[i]);
        m_NotAssociated_EnergyLoss->push_back(((*m_PixHitEnergyLoss)[i]));
        m_NotAssociated_BarrelEndcap->push_back((*m_PixHitBarrelEndcap)[i]);
        m_NotAssociated_LayerDisk->push_back((*m_PixHitLayerDisk)[i]);
        m_NotAssociated_EtaModule->push_back((*m_PixHitEtaModule)[i]);
        m_NotAssociated_PhiModule->push_back((*m_PixHitPhiModule)[i]);
        m_NotAssociated_PDGParticle->push_back((*m_PixHitPDGParticle)[i]);
        m_NotAssociated_Time->push_back((*m_PixHitTime)[i]);
        m_NotAssociated_IncidentAngle_Lorcorr->push_back((*m_PixHitAngle_Lorcorr)[i]);
      }
    }
  }
    
    
  m_nt->Fill();
   
  if(mjo_NN) { m_nnt->Fill(); }


  return StatusCode::SUCCESS;
}

//----------------------------- PixelClusterValidationNtupleWriter::GetDigitalPosition --------
Amg::Vector2D InDet::PixelClusterValidationNtupleWriter::GetDigitalPosition(
  const InDet::PixelCluster*& pix,
  const InDetDD::SiDetectorElement*& EL) const {
 
  const InDetDD::PixelModuleDesign* design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&EL->design());
  std::vector<Identifier> rdos = pix->rdoList();
  std::vector<Identifier>::const_iterator oneRDO = rdos.begin();
  int rowmin=9999;
  int rowmax=-9999;
  int colmin=9999;
  int colmax=-9999;
  for(; oneRDO != rdos.end(); oneRDO++) {
    int row = m_pixelid->phi_index(*oneRDO);
    int col = m_pixelid->eta_index(*oneRDO);
    if(rowmin > row) rowmin = row;
    if(rowmax < row) rowmax = row;
    if(colmin > col) colmin = col;
    if(colmax < col) colmax = col;
  }
  //m_PixFirstRow->push_back(rowmin);
  //m_PixFirstCol->push_back(colmin);
  InDetDD::SiLocalPosition pos1 = design->positionFromColumnRow(colmin,rowmin);
  InDetDD::SiLocalPosition pos2 = design->positionFromColumnRow(colmax,rowmin);
  InDetDD::SiLocalPosition pos3 = design->positionFromColumnRow(colmin,rowmax);
  InDetDD::SiLocalPosition pos4 = design->positionFromColumnRow(colmax,rowmax);
  InDetDD::SiLocalPosition centroid = 0.25*(pos1+pos2+pos3+pos4);
  double shift = EL->getLorentzCorrection();
  m_PixClusLocX_LorentzCorrection->push_back(shift);
  Amg::Vector2D position(centroid.xPhi() + shift, centroid.xEta());
  return position;
}

//----------------------------- PixelClusterValidationNtupleWriter::FindNextHit ---------------
const SiHit * InDet::PixelClusterValidationNtupleWriter::FindNextHit(const SiHit *startHit, SiHitCollection::const_iterator Coll) {
  for (; Coll != m_hitcontainer->end(); Coll++) {
    const SiHit *compHit = &(*Coll);
    if (fabs(((*startHit).localEndPosition().x()-(*compHit).localStartPosition().x()))<0.00005 &&
      fabs(((*startHit).localEndPosition().y()-(*compHit).localStartPosition().y()))<0.00005 &&
      fabs(((*startHit).localEndPosition().z()-(*compHit).localStartPosition().z()))<0.00005)
    {
      return compHit;
      break;
    }
  }
  if (Coll == m_hitcontainer->end()) return 0;
  return 0;
}

//----------------------------- PixelClusterValidationNtupleWriter::FindAssociatedParticle ----
int InDet::PixelClusterValidationNtupleWriter::FindAssociatedParticle(int cl, int m_pixHitsNum, int hit) {
  int ht=-1;
  for (int i=0; i<m_pixHitsNum; i++) {
    if ((*m_PixHitBarrelEndcap)[i]==(*m_PixECBarrel)[cl]&& (*m_PixHitLayerDisk)[i]==(*m_PixClusLayer)[cl] &&
          (*m_PixHitEtaModule)[i]==(*m_PixEtaModule)[cl] && (*m_PixHitPhiModule)[i]==(*m_PixPhiModule)[cl]) {
      for (int j=0; j< hit; j++) {
        if (fabs((*m_PixClusEtaIndex)[cl][j]-(*m_PixHitEtaIndex)[i])<=1 &&
                fabs((*m_PixClusPhiIndex)[cl][j]-(*m_PixHitPhiIndex)[i])<=1 ) {
//          if(fabs((*m_PixClusEtaIndex)[cl][j]-(*m_PixHitEtaIndex)[i])<=((((*m_PixHitEtaIndex_delta)[i]/2)+2)) &&
//         fabs((*m_PixClusPhiIndex)[cl][j]-(*m_PixHitPhiIndex)[i])<=((((*m_PixHitPhiIndex_delta)[i]/2)+2))){
          ht = i;
          break;
        }
      }
    }
    if (ht==i)break;
    //else continue;
  }
  return ht;
}


//----------------------------- PixelClusterValidationNtupleWriter::FindMultipleAssociatedParticle ----
std::vector<int> InDet::PixelClusterValidationNtupleWriter::FindMultipleAssociatedParticle(int cl, int m_pixHitsNum, int hit) {
  std::vector<int> ht;
  
  for (int i=0; i<m_pixHitsNum; i++) {
    if ((*m_PixHitBarrelEndcap)[i]==(*m_PixECBarrel)[cl]&& (*m_PixHitLayerDisk)[i]==(*m_PixClusLayer)[cl] &&
        (*m_PixHitEtaModule)[i]==(*m_PixEtaModule)[cl] && (*m_PixHitPhiModule)[i]==(*m_PixPhiModule)[cl]) {
      for (int j=0; j< hit; j++) {
        if (fabs((*m_PixClusEtaIndex)[cl][j]-(*m_PixHitEtaIndex)[i])<=1 &&
            fabs((*m_PixClusPhiIndex)[cl][j]-(*m_PixHitPhiIndex)[i])<=1 ) {


          //          if(fabs((*m_PixClusEtaIndex)[cl][j]-(*m_PixHitEtaIndex)[i])<=((((*m_PixHitEtaIndex_delta)[i]/2)+2)) &&
          //         fabs((*m_PixClusPhiIndex)[cl][j]-(*m_PixHitPhiIndex)[i])<=((((*m_PixHitPhiIndex_delta)[i]/2)+2))){
          if ( (*m_PixHitPDGParticle)[i]== 0 ) continue;

          ht.push_back(i);
          break;
        }
      }
    }
    //  if (ht==i)break;
    //else continue;
  }
  return ht;
}





 StatusCode InDet::PixelClusterValidationNtupleWriter::createInput(const InDet::PixelCluster& pCluster,
     //    HepPoint3D & beamSpotPosition,
           Amg::Vector3D& beamSpotPosition,     
         double & tanl,
        double & pitch_y,
        int sizeX,
        int sizeY)
{

  //% if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << " Starting creating input from cluster " << endreq;
  ATH_MSG_VERBOSE( " Starting creating input from cluster "   );

  const InDetDD::SiDetectorElement* element=pCluster.detectorElement();
  if (element==0) {
    ATH_MSG_ERROR("Could not get detector element");
    return  StatusCode::FAILURE;
  }
  const AtlasDetectorID* aid = element->getIdHelper();
  if (aid==0)
    {
      ATH_MSG_ERROR("Could not get ATLASDetectorID");
      return  StatusCode::FAILURE;
   }
  const PixelID* pixelIDp=dynamic_cast<const PixelID*>(aid);
  if (!pixelIDp)
    {
      ATH_MSG_ERROR("Could not get PixelID pointer");
      return StatusCode::FAILURE;
    } 
  const PixelID& pixelID = *pixelIDp;
  
//  NNinput* input=new NNinput();
  std::unique_ptr<NNinput> input( new NNinput ); 
  
  //Identifier elementID = element->identify();
  const InDetDD::PixelModuleDesign* design
    (dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design()));
  
  
  //  const InDet::PixelCluster* pCluster  = pcot->prepRawData();
  const std::vector<Identifier>& rdos  = pCluster.rdoList();  
  
  //  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << " Number of RDOs: " << rdos.size()  << endreq;
  ATH_MSG_VERBOSE( "Number of RDOs: " << rdos.size() );
  //const std::vector<float>& chList     = pCluster.chargeList();
  //const std::vector<int>&  totList     = pCluster.totList(); 
  int chargeSize=pCluster.chargeList().size();
  int totSize=pCluster.totList().size(); 
  int rdoSize=rdos.size(); 
  
  if(rdoSize!=totSize){
    ATH_MSG_WARNING(" Cluster size problem"
      <<"\n Number of charges: " << chargeSize
      <<"\n Number of ToT    : " << totSize 
      <<"\n Number of rdos   : " << rdoSize);
  }
  std::vector<Identifier>::const_iterator rdosBegin = rdos.begin();
  std::vector<Identifier>::const_iterator rdosEnd = rdos.end();

  // Set chargeOrToT vecor based on mjo_ToT flag.
  // Exit with error if the used vector is empty.
  std::vector<float> chargeOrToTList;// = pCluster.chargeList();
  if(mjo_ToT){
    const std::vector<int>&  totList = pCluster.totList(); 
    if(totList.empty()){
      ATH_MSG_ERROR(" List of cluster ToT is empty. Try using charge -set \"UseToT=false\"-" );
      return  StatusCode::FAILURE;
    }
    // Copy int ToT values to vector.
    std::vector<int>::const_iterator it;
    for( it= totList.begin(); it != totList.end(); ++it){
      float tmp=*it;
      chargeOrToTList.push_back(tmp);
    }
  } else {
    const std::vector<float>& chList = pCluster.chargeList();
    if(chList.empty()) {
      ATH_MSG_ERROR(" List of cluster charge is empty. Try using ToT -set \"UseToT=true\"-" );
      return  StatusCode::FAILURE;
    }
    // Copy charge values to vector.
    std::vector<float>::const_iterator it;
    for( it = chList.begin(); it != chList.end(); ++it ){
      chargeOrToTList.push_back(*it);
    }
  }
  std::vector<float>::const_iterator chargeOrToT= chargeOrToTList.begin();
  
  InDetDD::SiLocalPosition sumOfWeightedPositions(0,0,0);
  double sumOfTot=0;

  int rowMin = 999;
  int rowMax = -1;
  int colMin = 999;
  int colMax = -1;  
  
  for (; rdosBegin!= rdosEnd; ++rdosBegin, ++chargeOrToT)
  {
    
    Identifier rId =  *rdosBegin;
    int row = pixelID.phi_index(rId);
    int col = pixelID.eta_index(rId);
    
    if(msgLvl(MSG::VERBOSE)) { 
      msg(MSG::VERBOSE) << " Adding pixel row: " << row << " col: " << col 
       << " Charge/ToT: " << *chargeOrToT <<" ("<<mjo_ToT<<")"<< endreq;
    }
    InDetDD::SiLocalPosition siLocalPosition
      (design->positionFromColumnRow(col,row)); 
    
    sumOfWeightedPositions += (*chargeOrToT)*siLocalPosition;
    sumOfTot += (*chargeOrToT);
    
    if (row < rowMin){ 
      rowMin = row; 
    } 
    
    if (row > rowMax){
      rowMax = row;
    }
    
    if (col < colMin){
      colMin = col;
    }
    
    if (col > colMax){
      colMax = col;
    }
  }
  sumOfWeightedPositions /= sumOfTot;
  
  // if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Weighted position: (" << sumOfWeightedPositions << ")" << endreq;
  // what you want to know is simple:
  // just the row and column of this average position!
  
  InDetDD::SiCellId cellIdWeightedPosition=design->cellIdOfPosition(sumOfWeightedPositions);
  
  if (!cellIdWeightedPosition.isValid())
  {
    msg(MSG::ERROR) << "Weighted position is on invalid CellID." << endreq;
  }
  
  int columnWeightedPosition=cellIdWeightedPosition.etaIndex();
  int rowWeightedPosition=cellIdWeightedPosition.phiIndex();

  ATH_MSG_DEBUG(" weighted pos row: " << rowWeightedPosition << " col: " << columnWeightedPosition ); 

  InDetDD::SiLocalPosition w = design->positionFromColumnRow(columnWeightedPosition,rowWeightedPosition);

  double localColumnWeightedPosition = w.xEta();
  double localRowWeightedPosition = w.xPhi();
  
  int centralIndexX=(sizeX-1)/2;
  int centralIndexY=(sizeY-1)/2;

  if (abs(rowWeightedPosition-rowMin)>centralIndexX ||
      abs(rowWeightedPosition-rowMax)>centralIndexX)
  {
    ATH_MSG_DEBUG(" Cluster too large rowMin " << rowMin << " rowMax " << rowMax << " centralX " << centralIndexX);
    return  StatusCode::FAILURE;
  }
  
  if (abs(columnWeightedPosition-colMin)>centralIndexY ||
      abs(columnWeightedPosition-colMax)>centralIndexY)
  {
    ATH_MSG_DEBUG(" Cluster too large colMin" << colMin << " colMax " << colMax << " centralY " << centralIndexY);
    return  StatusCode::FAILURE;
  }
  
  for (int a=0;a<sizeX;a++) {
    std::vector<float> Yvector;
    for (int b=0;b<sizeY;b++) {
      Yvector.push_back(0);
    }
    input->matrixOfToT.push_back(Yvector);
  }

  // initialize the vector of Y pitches to default values.
  for (int b=0; b<sizeY;b++) {
    input->vectorOfPitchesY.push_back(pitch_y);
  }

  rdosBegin = rdos.begin();
  chargeOrToT = chargeOrToTList.begin();
  
  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << " Putting together the n. " << rdos.size() 
          << " rdos into a matrix." << endreq;
  
  //  int totIdx=0;
  for (; rdosBegin!= rdosEnd; ++rdosBegin, ++chargeOrToT)
  {
    Identifier rId =  *rdosBegin;
    int absrow = pixelID.phi_index(rId)-rowWeightedPosition+centralIndexX;
    int abscol = pixelID.eta_index(rId)-columnWeightedPosition+centralIndexY;
    
    if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << " phi Index: " << pixelID.phi_index(rId) 
     << " absrow: " << absrow << " eta Idx: " << pixelID.eta_index(rId) 
     << " abscol: " << abscol << " tot " << *chargeOrToT << endreq;

    if (absrow <0 || absrow > sizeX)
    {
      ATH_MSG_DEBUG(" problem with index: " << absrow << " min: " << 0 << " max: " << sizeX);
      //delete input;
      return  StatusCode::FAILURE;
    }
    if (abscol <0 || abscol > sizeY)
    {
      ATH_MSG_DEBUG(" problem with index: " << abscol << " min: " << 0 << " max: " << sizeY);
      //delete input;
      return  StatusCode::FAILURE;
    }
    input->matrixOfToT[absrow][abscol]=*chargeOrToT;

    InDetDD::SiCellId cellId = element->cellIdFromIdentifier(*rdosBegin);
    InDetDD::SiDiodesParameters diodeParameters = design->parameters(cellId);
    double pitchY = diodeParameters.width().xEta();
    // Most pixels have standard size, if different pix found (i.e. ganged) update pitch value.
    if(input->vectorOfPitchesY[abscol] != pitchY){
       ATH_MSG_VERBOSE(" Update y pitch size Was  "<<input->vectorOfPitchesY[abscol]<<"  set to  "<<pitchY
         <<"\n col "<<abscol<<" row "<<absrow<<" ToT "<<*chargeOrToT);
       input->vectorOfPitchesY[abscol]=pitchY;
    }
  }//end iteration on rdos
  Identifier pixidentif=pCluster.identify();
  input->etaModule=(int)pixelID.eta_module(pixidentif);

  input->ClusterPixLayer=(int)pixelID.layer_disk(pixidentif);
  input->ClusterPixBarrelEC=(int)pixelID.barrel_ec(pixidentif);
  
  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << " eta module: " << input->etaModule << endreq;
  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << " Layer number: " << input->ClusterPixLayer 
          << " Barrel / endcap: " << input->ClusterPixBarrelEC << endreq;

  ATH_MSG_VERBOSE( " End RDO LOOP " );
          
  input->useTrackInfo=false;
  
  const Amg::Vector2D& prdLocPos    = pCluster.localPosition();
  InDetDD::SiLocalPosition centroid(prdLocPos);

  Amg::Vector3D globalPos = element->globalPosition(centroid);
  Amg::Vector3D my_track = globalPos-beamSpotPosition; 


  Amg::Vector3D my_normal = element->normal();
  Amg::Vector3D my_phiax = element->phiAxis();
  Amg::Vector3D my_etaax = element->etaAxis();


  float trkphicomp = my_track.dot(my_phiax);
  float trketacomp = my_track.dot(my_etaax);
  float trknormcomp = my_track.dot(my_normal);
  double bowphi = atan2(trkphicomp,trknormcomp);
  double boweta = atan2(trketacomp,trknormcomp);
  tanl = element->getTanLorentzAnglePhi();
  if(bowphi > M_PI/2) bowphi -= M_PI;
  if(bowphi < -M_PI/2) bowphi += M_PI;
  int readoutside = design->readoutSide();
  double angle = atan(tan(bowphi)-readoutside*tanl);

  input->phi=angle;

  //  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << " Angle theta bef corr: " << boweta << endreq;
  ATH_MSG_VERBOSE( " Angle theta bef corr: " << boweta );
  if (boweta>TMath::Pi()/2.) boweta-=TMath::Pi();
  if (boweta<-TMath::Pi()/2.) boweta+=TMath::Pi();

  input->theta=boweta;
  
  ATH_MSG_VERBOSE(" Angle phi: " << angle << " theta: " << boweta );
  input->rowWeightedPosition=rowWeightedPosition;
  input->columnWeightedPosition=columnWeightedPosition;

  ATH_MSG_VERBOSE(" RowWeightedPosition: " << rowWeightedPosition << " ColWeightedPosition: " << columnWeightedPosition );

  ATH_MSG_DEBUG( "matrixOfToT converted in a std::vector<float>  " );

  // store the matrixOfToT in a vector  
  std::vector<float> inputData;
  for (int u=0;u<sizeX;u++)
  {
    for (int s=0;s<sizeY;s++)
    {
      inputData.push_back( input->matrixOfToT[u][s]);
    }
  }
  ATH_MSG_VERBOSE( "matrixOfToT converted in a std::vector<float>  " );

  m_NN_matrixOfToT->push_back(inputData);
   
  ATH_MSG_VERBOSE( "... and saved  " );

  m_NN_sizeX->push_back(sizeX); 
  
  m_NN_sizeY->push_back(sizeY); 
    
  m_NN_vectorOfPitchesY->push_back(input->vectorOfPitchesY);
  
  m_NN_ClusterPixLayer->push_back(input->ClusterPixLayer);
  
  m_NN_ClusterPixBarrelEC->push_back(input->ClusterPixBarrelEC);
  
  m_NN_phiBS->push_back(input->phi);
  
  m_NN_thetaBS->push_back(input->theta);
  
  m_NN_etaModule->push_back(input->etaModule);
  
  m_NN_useTrackInfo->push_back(input->useTrackInfo);
  
  m_NN_columnWeightedPosition->push_back(columnWeightedPosition);
  
  m_NN_rowWeightedPosition->push_back(rowWeightedPosition);

  m_NN_localColumnWeightedPosition->push_back(localColumnWeightedPosition);
  
  m_NN_localRowWeightedPosition->push_back(localRowWeightedPosition);


  
  ATH_MSG_VERBOSE( "NN training Written" );
  

  return StatusCode::SUCCESS;


}//end create NNinput function

//----------------------------- PixelClusterValidationNtupleWriter::fillClusterVector ------------------
StatusCode InDet::PixelClusterValidationNtupleWriter::fillClusterVector( const InDet::PixelCluster& cluster, IdentifierHash element_hash  ) {



  int RIOindex = 0;
  const int maxRIO_size = 1000000;
  Identifier clId = cluster.identify();

  unsigned int GroupSize = cluster.rdoList().size();
  
  const std::vector<Identifier>& rdoID = cluster.rdoList();
  std::vector<int>* eta = new std::vector<int>(GroupSize);
  std::vector<int>* phi = new std::vector<int>(GroupSize);
  std::vector<int>* LVL1AList= new std::vector<int>(GroupSize);
  std::vector<int>* ToTList=   new std::vector<int>(GroupSize);
  std::vector<float>* ChargeList=new std::vector<float>(GroupSize);
  for (unsigned int w=0; w<GroupSize; w++) {
    (*eta)[w] = m_pixelid->eta_index((rdoID)[w]);
    (*phi)[w] = m_pixelid->phi_index((rdoID)[w]);
  }
  
  
  if (  m_nRIOs   >= maxRIO_size ) {
    ATH_MSG_WARNING( "reached max. number of RIO entries in the ntuple" );
    m_nt->Fill();
    delete eta;
    delete phi;
    delete LVL1AList;
    delete ToTList;
    delete ChargeList;
    return StatusCode::SUCCESS;
  }
  else {
    m_PixClusGloX     ->push_back(float(cluster.globalPosition().x()));
    m_PixClusGloY     ->push_back(float(cluster.globalPosition().y()));
    m_PixClusGloZ     ->push_back(float(cluster.globalPosition().z()));
    m_PixClusEta      ->push_back(float(cluster.globalPosition().eta()));
    m_PixClusPhi      ->push_back(float(cluster.globalPosition().phi()));
    m_PixOmegaPhi     ->push_back(float(cluster.omegax()));
    m_PixOmegaEta     ->push_back(float(cluster.omegay()));
    
    const Amg::Vector2D  locPos = cluster.localPosition();
    m_PixClusLocX     ->push_back(float(locPos[Trk::locX]));
    m_PixClusLocY     ->push_back(float(locPos[Trk::locY]));
    
    const Amg::MatrixX  locErrMat = cluster.localCovariance();

    m_PixClusErrX->push_back( Amg::error(locErrMat,Trk::locX));
    m_PixClusErrY->push_back( Amg::error(locErrMat,Trk::locY));


    

  
    
    
    const InDet::PixelCluster* sc = &cluster;    //= dynamic_cast<const InDet::PixelCluster*>(*p_clus);                
    const InDetDD::SiDetectorElement* element = sc->detectorElement();   
    Amg::Vector2D centroid = GetDigitalPosition(sc,element);
    m_PixClusLocXcentroid ->push_back(float(centroid[Trk::locX]));
    m_PixClusLocYcentroid ->push_back(float(centroid[Trk::locY]));
    
    m_PixClusGroupsize->push_back(float(GroupSize));                   //cluster groupsize
    m_PixClusLayer    ->push_back(float(m_pixelid->layer_disk(clId))); //cluster layer
    m_PixEtaModule    ->push_back(float(m_pixelid->eta_module(clId))); //cluster ...
    m_PixPhiModule    ->push_back(float(m_pixelid->phi_module(clId))); //cluster ...
    m_PixClusRow      ->push_back(float(m_pixelid->phi_index(clId))) ; //cluster row
    m_PixClusCol      ->push_back(float(m_pixelid->eta_index(clId))) ; //cluster col
    m_PixECBarrel     ->push_back(float(m_pixelid->barrel_ec(clId))) ; //endcap or barrel
    InDet::SiWidth  width = cluster.width();
    m_PixDeltaRow     ->push_back(int(width.colRow().x()));
    m_PixDeltaCol     ->push_back(int(width.colRow().y()));
    m_PixClusToT      ->push_back(int(cluster.totalToT()));
    m_PixClusCharge   ->push_back(float(cluster.totalCharge()));
    m_PixClusLvl1     ->push_back(int(cluster.LVL1A()));
    m_PixClusGanged   ->push_back(cluster.gangedPixel()?1:0);
    m_PixClusFake     ->push_back(cluster.isFake()?1:0);
    m_PixClusAmbiguous->push_back(cluster.isAmbiguous()?1:0);
    
    m_PixClusIsSplit  ->push_back(cluster.isSplit()?1:0);
    m_PixClusSplitProb1->push_back(cluster.splitProbability1() );
    m_PixClusSplitProb2->push_back(cluster.splitProbability2() );
    
    m_PixClusEtaIndex->push_back(*eta);
    m_PixClusPhiIndex->push_back(*phi);
    
    if ( mjo_WriteDetailedPixelInformation ){

      PixelRDO_Container::const_iterator rdo_collection; 
      if ( m_rdocontainer ) {
        rdo_collection = m_rdocontainer->indexFind(element_hash);
      }

      if ( m_rdocontainer && rdo_collection!=m_rdocontainer->end() ) {
        for (unsigned int w=0; w<GroupSize; w++) {
          int LVL1L = -1, ToTL=-1;
          PixelRDO_Collection::const_iterator pRDO    = (*rdo_collection)->begin();
          PixelRDO_Collection::const_iterator pRDOend = (*rdo_collection)->end();
          // take into account ganged pixels are not in the RDO collection
          Identifier rdoID1 = rdoID[w], rdoID2 = rdoID[w]; 
          if ( cluster.gangedPixel() ) {
            InDetDD::SiCellId cellID = element->cellIdFromIdentifier (rdoID[w]);
            if ( element->numberOfConnectedCells (cellID) > 1) {
              InDetDD::SiCellId gangedCellID = element->connectedCell (cellID,0);
              if ( gangedCellID==cellID ) gangedCellID = element->connectedCell (cellID,1);
              rdoID2 = element->identifierFromCellId (gangedCellID);
              // ATH_MSG_VERBOSE ( "Ganged ID= " << rdoID1 << "," << rdoID2 );
            } 
          }
  
          while (pRDO!=pRDOend) {
            Identifier myID = (*pRDO)->identify(); 
            if ( myID == rdoID1 || myID == rdoID2 ) {
              LVL1L = (*pRDO)->getLVL1A();
              ToTL  = (*pRDO)->getToT();
              break;
            }
            pRDO++;
          }
          if ( LVL1L==-1 || ToTL==-1 ) 
            ATH_MSG_DEBUG("Cannot find RDO " << m_pixelid->show_to_string(rdoID1) << " col/row=" << m_pixelid->eta_index(rdoID1) << "," << m_pixelid->phi_index(rdoID1));
          (*LVL1AList)[w]=LVL1L;
          (*ToTList)[w]=ToTL;
        }
        if ( cluster.chargeList().size()==GroupSize ) {
          for (unsigned int w=0; w<GroupSize; w++) (*ChargeList)[w]=(cluster.chargeList())[w];
        } else {
          for (unsigned int w=0; w<GroupSize; w++) (*ChargeList)[w]=-1.;        
        }
        m_PixClusLVL1AList->push_back(*LVL1AList);
        m_PixClusToTList->push_back(*ToTList);
        m_PixClusChargeList->push_back(*ChargeList);
      } 
    }else{  if ( mjo_WriteDetailedPixelInformation ) ATH_MSG_DEBUG("Cannot find RDO: Detailed Pixel Information  cannot be written" ); }
  }
  
  
  // Writing output for NN training
  
  if(mjo_NN){
    
    ATH_MSG_DEBUG( "Writing output for NN training" );
    
    Trk::RecVertex beamposition(m_iBeamCondSvc->beamVtx()); 
    
    Amg::Vector3D BS = beamposition.position();
         
    InDet::SiWidth  width = cluster.width();
    
    double tanl=0.0;
    int l_sizeX =7;//width.colRow().x();
    int l_sizeY =7;//width.colRow().y();
    //       input = new NNinput();
    
    //      ATH_MSG_VERBOSE( "Clustersize is: " << l_sizeX << " and " << l_sizeY   );
    
    double pitch_y = _pitchY;
    
    if (_geoId==IBL) {
      if (m_pixelid->barrel_ec(clId) == 0) {
        if (m_pixelid->layer_disk(clId) == 0) {
          pitch_y = IBLPITCHY;
          ATH_MSG_VERBOSE( "USING THE IBL ZERO LAYER." );
        }
      }
    } else if(_geoId==SLHC2) {
      if (m_pixelid->barrel_ec(clId) == 0) {
        if (m_pixelid->layer_disk(clId) < 2) {
          pitch_y = SLHC2_PITCHY_A;
          ATH_MSG_VERBOSE( "USING .15um pitch, Cartigny layout." );
        }
      }
    }

    StatusCode   sc = createInput(cluster,BS,tanl,pitch_y, l_sizeX, l_sizeY);
    
    
    if ( sc.isFailure() ) {
      ATH_MSG_DEBUG( "Unable to Write  infos for NN training" ); 
      
      std::vector<float> inputData; 
      m_NN_matrixOfToT->push_back(inputData);
      m_NN_vectorOfPitchesY->push_back(inputData);
      m_NN_sizeX->push_back(-100); 
      m_NN_sizeY->push_back(-100); 
      m_NN_ClusterPixLayer->push_back(-100);
      m_NN_ClusterPixBarrelEC->push_back(-100);
      m_NN_phiBS->push_back(-100);
      m_NN_thetaBS->push_back(-100);
      m_NN_etaModule->push_back(-100);
      m_NN_columnWeightedPosition->push_back(-100); 
      m_NN_rowWeightedPosition->push_back(-100); 
      m_NN_localColumnWeightedPosition->push_back(-100); 
      m_NN_localRowWeightedPosition->push_back(-100); 
      
    }
    
  }
  
  
  RIOindex++;
  delete eta;
  delete phi;
  delete LVL1AList;
  delete ToTList;
  delete ChargeList;
  //  delete input;
  
  m_nRIOs++;
  // m_nRIOs += RIOindex;  //number of clusters per event
  //   RIOindex;
  return StatusCode::SUCCESS;
}





//----------------------------- PixelClusterValidationNtupleWriter::finalize ------------------
StatusCode InDet::PixelClusterValidationNtupleWriter::finalize() {
    ATH_MSG_INFO( "PixelClusterValidationNtupleWriter finalize()" );
    delete m_PixClusLocX;
    delete m_PixClusLocY;
    delete m_PixClusLocXcentroid;
    delete m_PixClusLocYcentroid;
    delete m_PixClusLocX_LorentzCorrection;
    delete m_PixClusGloX;
    delete m_PixClusGloY;
    delete m_PixClusGloZ;
    delete m_PixClusEta;
    delete m_PixClusPhi;
    delete m_PixECBarrel;
    delete m_PixClusLayer ;
    delete m_PixEtaModule;
    delete m_PixPhiModule;
    delete m_PixClusGroupsize;
    delete m_PixClusRow;
    delete m_PixClusCol;
    delete m_PixDeltaRow;
    delete m_PixDeltaCol;
    delete m_PixOmegaPhi;
    delete m_PixOmegaEta;
    delete m_PixClusToT;
    delete m_PixClusCharge;
    delete m_PixClusLvl1;
    delete m_PixClusGanged;
    delete m_PixClusFake;
    delete m_PixClusAmbiguous;
    delete m_PixClusLocXana;
    delete m_PixClusLocYana;
    delete m_PixClusErrX;
    delete m_PixClusErrY;
    delete m_PixClusEtaIndex;
    delete m_PixClusPhiIndex;
    delete m_PixClusChargeList;
    delete m_PixClusToTList;
    delete m_PixClusLVL1AList;
    delete m_PixClusIsSplit;
    delete m_PixClusSplitProb1;
    delete m_PixClusSplitProb2;
    delete m_PixHitXstartpos;
    delete m_PixHitYstartpos;
    delete m_PixHitZstartpos;
    delete m_PixHitXendpos;
    delete m_PixHitYendpos;
    delete m_PixHitZendpos;
    delete m_PixHitBarrelEndcap;
    delete m_PixHitLayerDisk;
    delete m_PixHitEtaModule;
    delete m_PixHitPhiModule;
    delete m_PixHitEtaIndex;
    delete m_PixHitPhiIndex;
    delete m_PixHitAngle;
    delete m_PixHitAngle2;
    delete m_PixHitAngle_Lorcorr;
    delete m_PixHitEnergyLoss;
    delete m_PixHitTime;
    delete m_PixHitPDGParticle;
    delete m_PixHitFlag;
//    delete m_PixHitEtaIndex_delta;
//    delete m_PixHitPhiIndex_delta;
    delete m_MC_Xpos;
    delete m_MC_Ypos;
    delete m_MC_IncidentAngle;
    delete m_MC_IncidentAngle2;
    delete m_MC_IncidentAngle_Lorcorr;
    delete m_MC_EnergyLoss;
    delete m_MC_Time;
    delete m_MC_PDGParticle;
    delete m_MC_Flag;
    delete m_NotAssociated_Xpos;
    delete m_NotAssociated_Ypos;
    delete m_NotAssociated_BarrelEndcap;
    delete m_NotAssociated_LayerDisk;
    delete m_NotAssociated_EtaModule;
    delete m_NotAssociated_PhiModule;
    delete m_NotAssociated_IncidentAngle;
    delete m_NotAssociated_IncidentAngle2;
    delete m_NotAssociated_IncidentAngle_Lorcorr;
    delete m_NotAssociated_EnergyLoss;
    delete m_NotAssociated_Time;
    delete m_NotAssociated_PDGParticle;
    delete m_NotAssociated_Flag;

    delete m_NN_sizeX;
    delete m_NN_sizeY;
    delete m_NN_matrixOfToT;
    delete m_NN_vectorOfPitchesY;
    delete m_NN_ClusterPixLayer;
    delete m_NN_ClusterPixBarrelEC;
    delete m_NN_phiBS;
    delete m_NN_thetaBS;
    delete m_NN_etaModule;
    delete m_NN_useTrackInfo;
    delete m_NN_columnWeightedPosition;
    delete m_NN_rowWeightedPosition;
    delete m_NN_positionX;
    delete m_NN_positionY;
    delete m_NN_position_idX;
    delete m_NN_position_idY;
    delete m_NN_pdgid;
    delete m_NN_barcode;
    delete m_NN_momentum;
    delete m_NN_distanceX;
    delete m_NN_distanceY;

    delete m_NN_theta;
    delete m_NN_phi;

    m_PixClusLocX=0;
    m_PixClusLocY=0;
    m_PixClusLocXcentroid=0;
    m_PixClusLocYcentroid=0;
    m_PixClusLocX_LorentzCorrection=0;
    m_PixClusGloX=0;
    m_PixClusGloY=0;
    m_PixClusGloZ=0;
    m_PixClusEta=0;
    m_PixClusPhi=0;
    m_PixECBarrel=0;
    m_PixClusLayer=0;
    m_PixEtaModule=0;
    m_PixPhiModule=0;
    m_PixClusGroupsize=0;
    m_PixClusRow=0;
    m_PixClusCol=0;
    m_PixDeltaRow=0;
    m_PixDeltaCol=0;
    m_PixOmegaPhi=0;
    m_PixOmegaEta=0;
    m_PixClusToT=0;
    m_PixClusCharge=0;
    m_PixClusLvl1=0;
    m_PixClusGanged=0;
    m_PixClusFake=0;
    m_PixClusAmbiguous=0;
    m_PixClusLocXana=0;
    m_PixClusLocYana=0;
    m_PixClusErrX=0;
    m_PixClusErrY=0;
    m_PixClusEtaIndex=0;
    m_PixClusPhiIndex=0;
    m_PixClusChargeList=0;
    m_PixClusToTList=0;
    m_PixClusLVL1AList=0;
    m_PixClusIsSplit=0;
    m_PixClusSplitProb1=0;
    m_PixClusSplitProb2=0;
    m_PixHitXstartpos=0;
    m_PixHitYstartpos=0;
    m_PixHitZstartpos=0;
    m_PixHitXendpos=0;
    m_PixHitYendpos=0;
    m_PixHitZendpos=0;
    m_PixHitBarrelEndcap=0;
    m_PixHitLayerDisk=0;
    m_PixHitEtaModule=0;
    m_PixHitPhiModule=0;
    m_PixHitEtaIndex=0;
    m_PixHitPhiIndex=0;
    m_PixHitAngle=0;
    m_PixHitAngle2=0;
    m_PixHitAngle_Lorcorr=0;
    m_PixHitEnergyLoss=0;
    m_PixHitTime=0;
    m_PixHitPDGParticle=0;
    m_PixHitFlag=0;
//    m_PixHitEtaIndex_delta=0;
//    m_PixHitPhiIndex_delta=0;
    m_MC_Xpos=0;
    m_MC_Ypos=0;
    m_MC_IncidentAngle=0;
    m_MC_IncidentAngle2=0;
    m_MC_IncidentAngle_Lorcorr=0;
    m_MC_EnergyLoss=0;
    m_MC_Time=0;
    m_MC_PDGParticle=0;
    m_MC_Flag=0;
    m_NotAssociated_Xpos=0;
    m_NotAssociated_Ypos=0;
    m_NotAssociated_BarrelEndcap=0;
    m_NotAssociated_LayerDisk=0;
    m_NotAssociated_EtaModule=0;
    m_NotAssociated_PhiModule=0;
    m_NotAssociated_IncidentAngle=0;
    m_NotAssociated_IncidentAngle2=0;
    m_NotAssociated_IncidentAngle_Lorcorr=0;
    m_NotAssociated_EnergyLoss=0;
    m_NotAssociated_Time=0;
    m_NotAssociated_PDGParticle=0;
    m_NotAssociated_Flag=0;
    
    m_NN_sizeX=0;
    m_NN_sizeY=0;
    m_NN_matrixOfToT=0;
    m_NN_vectorOfPitchesY=0;
    m_NN_ClusterPixLayer=0;
    m_NN_ClusterPixBarrelEC=0;
    m_NN_phiBS=0;
    m_NN_thetaBS=0;
    m_NN_etaModule=0;
    m_NN_useTrackInfo=0;
    m_NN_columnWeightedPosition=0;
    m_NN_rowWeightedPosition=0;
    m_NN_positionX=0;
    m_NN_positionY=0;
    m_NN_position_idX=0;
    m_NN_position_idY=0;
    m_NN_theta=0;
    m_NN_phi=0;

    return StatusCode::SUCCESS;
}

