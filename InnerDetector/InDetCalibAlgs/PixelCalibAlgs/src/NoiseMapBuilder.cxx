/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PixelCalibAlgs
#include "PixelCalibAlgs/NoiseMapBuilder.h"
#include "PixelCalibAlgs/PixelConvert.h"

// PixelConditions
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
#include "PixelConditionsServices/IPixelByteStreamErrorsSvc.h"
#include "PixelConditionsServices/ISpecialPixelMapSvc.h" // kazuki
#include "PixelConditionsData/SpecialPixelMap.h"

// Gaudi
#include "GaudiKernel/ITHistSvc.h"

// EDM
#include "InDetRawData/PixelRDO_Container.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h" // kazuki
#include "InDetReadoutGeometry/SiDetectorElement.h" // kazuki
#include "InDetReadoutGeometry/PixelModuleDesign.h" // kazuki
#include "InDetReadoutGeometry/SiDetectorElementCollection.h" // kazuki
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

// geometry
#include "InDetIdentifier/PixelID.h"

// ROOT
#include "TH2.h"
#include "TString.h" // kazuki

// standard library
#include <string>
#include <sstream>
#include <algorithm>
#include <map> // kazuki
#include <fstream>
#include <cstdlib> // getenv


NoiseMapBuilder::NoiseMapBuilder(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_tHistSvc("THistSvc", name),
  m_pixelConditionsSummarySvc("PixelConditionsSummarySvc", name),
  m_BSErrorsSvc("PixelByteStreamErrorsSvc",name),
  m_specialPixelMapSvc("SpecialPixelMapSvc", name), // kazuki
  m_pixelRDOKey("PixelRDOs"),
  m_isIBL(true), // kazuki
  m_nEvents(0.),
  m_nEventsHist(nullptr),
  m_nEventsLBHist(nullptr),
  m_disabledModules(nullptr),
  m_overlayedPixelNoiseMap(nullptr),
  m_overlayedIBLDCNoiseMap(nullptr),
  m_overlayedIBLSCNoiseMap(nullptr),
  m_pixelID(0),
  m_pixman(0), // kazuki
  m_disk1ACut(1.e-3),
  m_disk2ACut(1.e-3),
  m_disk3ACut(1.e-3),
  m_disk1CCut(1.e-3),
  m_disk2CCut(1.e-3),
  m_disk3CCut(1.e-3),
  m_iblCut(1.e-3), // kazuki
  m_bLayerCut(1.e-3),
  m_layer1Cut(1.e-3),
  m_layer2Cut(1.e-3),
  m_dbmCut(1.e-3), // kazuki
  //m_longPixelMultiplier(1.),
  m_longPixelMultiplier(1.5), // kazuki
  //m_gangedPixelMultiplier(1.),
  m_gangedPixelMultiplier(2.), // kazuki
  m_occupancyPerBC(true),
  m_nBCReadout(2),
  m_lbMin(0),
  m_lbMax(-1),
  m_calculateNoiseMaps(false)
{
  declareProperty("PixelRDOKey", m_pixelRDOKey, "StoreGate key of pixel RDOs");
  declareProperty("Disk1ACut", m_disk1ACut, "Occupancy cut for Disk1A pixels");
  declareProperty("Disk2ACut", m_disk2ACut, "Occupancy cut for Disk2A pixels");
  declareProperty("Disk3ACut", m_disk3ACut, "Occupancy cut for Disk3A pixels");
  declareProperty("Disk1CCut", m_disk1CCut, "Occupancy cut for Disk1C pixels");
  declareProperty("Disk2CCut", m_disk2CCut, "Occupancy cut for Disk2C pixels");
  declareProperty("Disk3CCut", m_disk3CCut, "Occupancy cut for Disk3C pixels");
  declareProperty("BLayerCut", m_bLayerCut, "Occupancy cut for BLayer pixels");
  declareProperty("Layer1Cut", m_layer1Cut, "Occupancy cut for Layer1 pixels");
  declareProperty("Layer2Cut", m_layer2Cut, "Occupancy cut for Layer2 pixels");
  declareProperty("IBLCut", m_dbmCut, "Occupancy cut for DBM pixels"); // kazuki
  declareProperty("NBCReadout", m_nBCReadout, "Number of bunch crossings read out");
  declareProperty("LBMin", m_lbMin, "First lumi block to consider");
  declareProperty("LBMax", m_lbMax, "Last lumi block to consider");
  declareProperty("LongPixelMultiplier", m_longPixelMultiplier, "Multiplier for long pixels");
  declareProperty("GangedPixelMultiplier", m_gangedPixelMultiplier, "Multiplier for ganged pixels");
  declareProperty("OccupancyPerBC", m_occupancyPerBC, "Calculate occupancy per BC or per event");
  declareProperty("CalculateNoiseMaps", m_calculateNoiseMaps, "If false only build hit maps");
  declareProperty("THistSvc", m_tHistSvc, "THistSvc");
  declareProperty("PixelConditionsSummarySvc", m_pixelConditionsSummarySvc, "PixelConditionsSummarySvc");
  declareProperty("PixelByteStreamSummarySvc", m_BSErrorsSvc, "PixelBSErrorsSvc");
  declareProperty("isIBL", m_isIBL, "If false IBL not considered"); // kazuki
}


NoiseMapBuilder::~NoiseMapBuilder(){}



StatusCode NoiseMapBuilder::initialize(){
  ATH_MSG_INFO( "Initializing NoiseMapBuilder" );

  StatusCode sc = detStore()->retrieve( m_pixelID, "PixelID" );
  if( !sc.isSuccess() ){
    ATH_MSG_FATAL( "Unable to retrieve pixel ID helper" );
    return StatusCode::FAILURE;
  }

  // kazuki ==>
  sc = m_specialPixelMapSvc.retrieve();
  if( !sc.isSuccess() ){
    ATH_MSG_FATAL( "Unable to retrieve SpecialPixelMapSvc" );
    return StatusCode::FAILURE;
  }

  sc = detStore()->retrieve( m_pixman, "Pixel" );
  if( !sc.isSuccess() ){
    ATH_MSG_FATAL( "Unable to retrieve pixel ID manager" );
    return StatusCode::FAILURE;
  }
  // <== //

  sc = m_tHistSvc.retrieve();
  if( !sc.isSuccess() ){
    ATH_MSG_FATAL( "Unable to retrieve THistSvc" );
    return StatusCode::FAILURE;
  }

  sc = m_pixelConditionsSummarySvc.retrieve();
  if( !sc.isSuccess() ){
    ATH_MSG_FATAL( "Unable to retrieve PixelConditionsSummarySvc" );
    return StatusCode::FAILURE;
  }

  sc = m_BSErrorsSvc.retrieve();
  if( !sc.isSuccess() ){
    ATH_MSG_FATAL( "Unable to retrieve bytestream errors service" );
    return StatusCode::FAILURE;
  }

  m_hitMaps.resize(m_pixelID->wafer_hash_max());
  m_LBdependence.resize(m_pixelID->wafer_hash_max());
  m_BCIDdependence.resize(m_pixelID->wafer_hash_max());
  m_TOTdistributions.resize(m_pixelID->wafer_hash_max());

  if( m_calculateNoiseMaps ){
    m_noiseMaps.resize(m_pixelID->wafer_hash_max());
  }

  m_nEventsHist = new TH1D("NEvents", "NEvents", 1, 0, 1);
  m_tHistSvc->regHist("/histfile/NEvents", m_nEventsHist).setChecked();

  const int nLBmax = 3001;
  m_nEventsLBHist = new TH1D("NEventsLB", "NEventsLB", nLBmax, -0.5, nLBmax + 0.5);
  m_tHistSvc->regHist("/histfile/NEventsLB", m_nEventsLBHist).setChecked();

  //std::string testarea = std::getenv("TestArea");
  //ifstream ifs(testarea + "/InstallArea/share/PixelMapping_Run2.dat");
  char* tmppath = std::getenv("DATAPATH");
  if(tmppath == NULL){
      ATH_MSG_FATAL( "Unable to retrieve environmental DATAPATH" );
      return StatusCode::FAILURE;
  }
  std::string cmtpath(tmppath);
  std::vector<std::string> paths = splitter(cmtpath, ':');
  std::ifstream ifs;
  for (const auto& x : paths){
    if(is_file_exist((x + "/PixelMapping_Run2.dat").c_str())){
      if(m_isIBL){
        ifs.open(x + "/PixelMapping_Run2.dat");
      } else {
        ifs.open(x + "/PixelMapping_May08.dat");
      }
      int tmp_barrel_ec; int tmp_layer; int tmp_module_phi; int tmp_module_eta; std::string tmp_module_name;
      std::vector<int> tmp_position;
      tmp_position.resize(4);
      //int counter = 0; // debug
      while(ifs >> tmp_barrel_ec >> tmp_layer >> tmp_module_phi >> tmp_module_eta >> tmp_module_name) {
        tmp_position[0] = tmp_barrel_ec;
        tmp_position[1] = tmp_layer;
        tmp_position[2] = tmp_module_phi;
        tmp_position[3] = tmp_module_eta;
        m_pixelMapping.push_back(std::make_pair(tmp_module_name, tmp_position));
      }
      break;
    }
  }

#if 0
  // conversion map from position to DCS ID
  // ECA
  std::map<int, int> phi2M_ECA;
  phi2M_ECA[0] = 1;
  phi2M_ECA[1] = 6;
  phi2M_ECA[2] = 2;
  phi2M_ECA[3] = 5;
  phi2M_ECA[4] = 3;
  phi2M_ECA[5] = 4;
  std::map<int, TString> phi2moduleID_ECA;
  for(int phi = 0; phi < 48; phi++){
    phi = phi + 6;
    if (phi >= 48) phi = phi % 6;
    phi2moduleID_ECA[phi] = Form("B%02d_S%d_M%d",(phi / 12) + 1, (phi % 12 < 6) ? 1 : 2, phi2M_ECA[phi % 6]);
  }
  // ECC
  std::map<int, int> phi2M_ECC;
  phi2M_ECC[0] = 4;
  phi2M_ECC[1] = 3;
  phi2M_ECC[2] = 5;
  phi2M_ECC[3] = 2;
  phi2M_ECC[4] = 6;
  phi2M_ECC[5] = 1;
  std::map<int, TString> phi2moduleID_ECC;
  for(int phi = 0; phi < 48; phi++){
    phi = phi + 6;
    if (phi >= 48) phi = phi % 6;
    phi2moduleID_ECC[phi] = Form("B%02d_S%d_M%d",(phi / 12) + 1, (phi % 12 < 6) ? 1 : 2, phi2M_ECC[phi % 6]);
  }
  // Barrel
  int nlayer = 3;
  if (m_isIBL) nlayer = 4;
  //
  std::vector<int> module_phi_max;
  if (m_isIBL) module_phi_max.push_back(14);
  module_phi_max.push_back(22);
  module_phi_max.push_back(38);
  module_phi_max.push_back(52);
  // IBL
  std::map<int, TString> eta2moduleID_IBL;
  eta2moduleID_IBL[9] = TString("A_M4_A8_2");
  eta2moduleID_IBL[8] = TString("A_M4_A8_1");
  eta2moduleID_IBL[7] = TString("A_M4_A7_2");
  eta2moduleID_IBL[6] = TString("A_M4_A7_1");
  eta2moduleID_IBL[5] = TString("A_M3_A6");
  eta2moduleID_IBL[4] = TString("A_M3_A5");
  eta2moduleID_IBL[3] = TString("A_M2_A4");
  eta2moduleID_IBL[2] = TString("A_M2_A3");
  eta2moduleID_IBL[1] = TString("A_M1_A2");
  eta2moduleID_IBL[0] = TString("A_M1_A1");
  eta2moduleID_IBL[-1] = TString("C_M1_C1");
  eta2moduleID_IBL[-2] = TString("C_M1_C2");
  eta2moduleID_IBL[-3] = TString("C_M2_C3");
  eta2moduleID_IBL[-4] = TString("C_M2_C4");
  eta2moduleID_IBL[-5] = TString("C_M3_C5");
  eta2moduleID_IBL[-6] = TString("C_M3_C6");
  eta2moduleID_IBL[-7] = TString("C_M4_C7_1");
  eta2moduleID_IBL[-8] = TString("C_M4_C7_2");
  eta2moduleID_IBL[-9] = TString("C_M4_C8_1");
  eta2moduleID_IBL[-10] = TString("C_M4_C8_2");
  // Pixel
  std::map<int, TString> eta2moduleID_PixelBarrel;
  eta2moduleID_PixelBarrel[6] = TString("_M6A");
  eta2moduleID_PixelBarrel[5] = TString("_M5A");
  eta2moduleID_PixelBarrel[4] = TString("_M4A");
  eta2moduleID_PixelBarrel[3] = TString("_M3A");
  eta2moduleID_PixelBarrel[2] = TString("_M2A");
  eta2moduleID_PixelBarrel[1] = TString("_M1A");
  eta2moduleID_PixelBarrel[0] = TString("_M0");
  eta2moduleID_PixelBarrel[-1] = TString("_M1C");
  eta2moduleID_PixelBarrel[-2] = TString("_M2C");
  eta2moduleID_PixelBarrel[-3] = TString("_M3C");
  eta2moduleID_PixelBarrel[-4] = TString("_M4C");
  eta2moduleID_PixelBarrel[-5] = TString("_M5C");
  eta2moduleID_PixelBarrel[-6] = TString("_M6C");
  //
  std::map<int, TString> phi2moduleID_BLayer;
  for(int phi = 0; phi < 22; phi++){
    if(phi == 0) {
      phi2moduleID_BLayer[phi] = TString("B11_S2");
    } else {
      phi2moduleID_BLayer[phi] = Form("B%02d_S%d",(phi + 1) / 2, (phi % 2) ? 1 : 2);
    }
  }
  //
  std::map<int, TString> phi2moduleID_Layer1;
  std::map<int, TString> phi2moduleID_Layer2;
  for(int phi = 0; phi < 52; phi++){
    phi2moduleID_Layer1[phi] = Form("B%02d_S%d", (phi / 2) + 1, (phi % 2) ? 2 : 1);
    if (phi == 51) phi2moduleID_Layer2[phi] = TString("B01_S1");
    else phi2moduleID_Layer2[phi] = Form("B%02d_S%d", (phi + 1) / 2 + 1, (phi % 2) ? 1 : 2);
  }
  // DBM
  std::map<int, TString> phi2moduleID_DBM;
  phi2moduleID_DBM[2] = "12_M1";
  phi2moduleID_DBM[3] = "12_M2";
  phi2moduleID_DBM[0] = "34_M3";
  phi2moduleID_DBM[1] = "34_M4";
  std::map<int, int> phi2moduleNum_DBM;
  phi2moduleNum_DBM[2] = 1;
  phi2moduleNum_DBM[3] = 4;
  phi2moduleNum_DBM[0] = 7;
  phi2moduleNum_DBM[1] = 10;
#endif

  // kazuki
  InDetDD::SiDetectorElementCollection::const_iterator iter, itermin, itermax;
  itermin = m_pixman->getDetectorElementBegin();
  itermax = m_pixman->getDetectorElementEnd();
  if(m_pixelID->wafer_hash_max() > 1744) m_isIBL = true; // #modules only Pixel is 1744
  //std::cout << "DEBUG: wafer_hash_max = " << m_pixelID->wafer_hash_max() << std::endl;
  std::map<int, std::string> barrel_ec2string;
  barrel_ec2string[0] = "barrel";
  barrel_ec2string[2] = "endcapA";
  barrel_ec2string[-2] = "endcapC";
  barrel_ec2string[4] = "DBMA";
  barrel_ec2string[-4] = "DBMC";
  std::map<int, std::string> layer2flavour;
  if (m_isIBL) {
    layer2flavour[0] = "IBL";
    layer2flavour[1] = "B-layer";
    layer2flavour[2] = "Layer1";
    layer2flavour[3] = "Layer2";
  } else {
    layer2flavour[0] = "B-layer";
    layer2flavour[1] = "Layer1";
    layer2flavour[2] = "Layer2";
  }
  // initialize histograms
  for( iter = itermin; iter != itermax; ++iter) {
    const InDetDD::SiDetectorElement* element = *iter;
    if(element == 0) continue;
    Identifier ident = element->identify();
    if(!m_pixelID->is_pixel(ident)) continue;  // OK this Element is included
    const InDetDD::PixelModuleDesign* design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design());
    if(!design) continue;
    //unsigned int mchips = design->numberOfCircuits();
    int barrel     = m_pixelID->barrel_ec (ident);
    int layer      = m_pixelID->layer_disk(ident); // kazuki
    int module_phi = m_pixelID->phi_module(ident);
    int module_eta = m_pixelID->eta_module(ident); // kazuki
    int moduleHash = m_pixelID->wafer_hash(ident);

    // debug
    // std::cout << moduleHash << " " << NoiseMapBuilder::getDCSIDFromPosition(barrel,layer,module_phi,module_eta) << std::endl;

    std::ostringstream names;
    TString tmp_onlineID;
    std::string onlineID;
    if (m_isIBL) { // --- IBL --- //
      if ( barrel == 0 ) { // barrel
        if (layer == 0) { // IBL
          m_moduleHashList.push_back(moduleHash);

          //tmp_onlineID = Form("LI_S%02d_",module_phi + 1) + eta2moduleID_IBL[module_eta];
          //onlineID = std::string(tmp_onlineID);
          onlineID = NoiseMapBuilder::getDCSIDFromPosition(barrel,layer,module_phi,module_eta);
          // hit map
          m_hitMaps[moduleHash] = new TH2D(onlineID.c_str(), onlineID.c_str(), 160, -0., 160., 336, 0., 336.);
          names << "/histfile/hitMaps_barrel/IBL/" << onlineID;
          m_tHistSvc->regHist(names.str().c_str(), m_hitMaps[moduleHash]).setChecked();
          names.str(""); names.clear();
          // LB dependence
          m_LBdependence[moduleHash] = new TH1D(onlineID.c_str(), onlineID.c_str(), nLBmax, -0.5, nLBmax + 0.5);
          names << "/histfile/LBdep_barrel/IBL/" << onlineID;
          m_tHistSvc->regHist(names.str().c_str(), m_LBdependence[moduleHash]).setChecked();
          names.str(""); names.clear();
          // BCID dependence
          m_BCIDdependence[moduleHash] = new TH1D(onlineID.c_str(), onlineID.c_str(), 301, -0.5, 300.5);
          names << "/histfile/BCIDdep_barrel/IBL/" << onlineID;
          m_tHistSvc->regHist(names.str().c_str(), m_BCIDdependence[moduleHash]).setChecked();
          names.str(""); names.clear();
          // TOT
          m_TOTdistributions[moduleHash] = new TH1D(onlineID.c_str(), onlineID.c_str(), 19, -0.5, 18.5);
          names << "/histfile/TOT_barrel/IBL/" << onlineID;
          m_tHistSvc->regHist(names.str().c_str(), m_TOTdistributions[moduleHash]).setChecked();
          names.str(""); names.clear();
          // noise map
          if( m_calculateNoiseMaps ){
            m_noiseMaps[moduleHash] = new TH2C(onlineID.c_str(), onlineID.c_str(), 160, -0., 160., 336, 0., 336.);
            names << "/histfile/noiseMaps_barrel/IBL/" << onlineID;
            m_tHistSvc->regHist(names.str().c_str(), m_noiseMaps[moduleHash]).setChecked();
            names.str(""); names.clear();
          }
        } else if (layer == 1) { // BLayer
          m_moduleHashList.push_back(moduleHash);

          //tmp_onlineID = Form("L%d_",layer - 1) + phi2moduleID_BLayer[module_phi] + eta2moduleID_PixelBarrel[module_eta];
          //onlineID = std::string(tmp_onlineID);
          onlineID = NoiseMapBuilder::getDCSIDFromPosition(barrel,layer,module_phi,module_eta);
          // hit map
          m_hitMaps[moduleHash] = new TH2D(onlineID.c_str(), onlineID.c_str(), 144, -0., 144., 328, 0., 328.);
          names << "/histfile/hitMaps_barrel/B-layer/" << onlineID;
          m_tHistSvc->regHist(names.str().c_str(), m_hitMaps[moduleHash]).setChecked();
          names.str(""); names.clear();
          // LB dependence
          m_LBdependence[moduleHash] = new TH1D(onlineID.c_str(), onlineID.c_str(), nLBmax, -0.5, nLBmax + 0.5);
          names << "/histfile/LBdep_barrel/B-layer/" << onlineID;
          m_tHistSvc->regHist(names.str().c_str(), m_LBdependence[moduleHash]).setChecked();
          names.str(""); names.clear();
          // BCID dependence
          m_BCIDdependence[moduleHash] = new TH1D(onlineID.c_str(), onlineID.c_str(), 301, -0.5, 300.5);
          names << "/histfile/BCIDdep_barrel/B-layer/" << onlineID;
          m_tHistSvc->regHist(names.str().c_str(), m_BCIDdependence[moduleHash]).setChecked();
          names.str(""); names.clear();
          // TOT
          m_TOTdistributions[moduleHash] = new TH1D(onlineID.c_str(), onlineID.c_str(), 256, -0.5, 255.5);
          names << "/histfile/TOT_barrel/B-layer/" << onlineID;
          m_tHistSvc->regHist(names.str().c_str(), m_TOTdistributions[moduleHash]).setChecked();
          names.str(""); names.clear();
          // noise map
          if( m_calculateNoiseMaps ){
            m_noiseMaps[moduleHash] = new TH2C(onlineID.c_str(), onlineID.c_str(), 144, -0., 144., 328, 0., 328.);
            names << "/histfile/noiseMaps_barrel/B-layer/" << onlineID;
            m_tHistSvc->regHist(names.str().c_str(), m_noiseMaps[moduleHash]).setChecked();
            names.str(""); names.clear();
          }
        } else { // Layer-1,2
          m_moduleHashList.push_back(moduleHash);

          //if(layer == 2) tmp_onlineID = Form("L%d_",layer - 1) + phi2moduleID_Layer1[module_phi] + eta2moduleID_PixelBarrel[module_eta];
          //else if(layer == 3) tmp_onlineID = Form("L%d_",layer - 1) + phi2moduleID_Layer2[module_phi] + eta2moduleID_PixelBarrel[module_eta];

          //onlineID = std::string(tmp_onlineID);
          onlineID = NoiseMapBuilder::getDCSIDFromPosition(barrel,layer,module_phi,module_eta);
          // hit map
          m_hitMaps[moduleHash] = new TH2D(onlineID.c_str(), onlineID.c_str(), 144, -0., 144., 328, 0., 328.);
          names << "/histfile/hitMaps_barrel/Layer" << layer - 1 << "/" << onlineID;
          m_tHistSvc->regHist(names.str().c_str(), m_hitMaps[moduleHash]).setChecked();
          names.str(""); names.clear();
          // LB dependence
          m_LBdependence[moduleHash] = new TH1D(onlineID.c_str(), onlineID.c_str(), nLBmax, -0.5, nLBmax + 0.5);
          names << "/histfile/LBdep_barrel/Layer" << layer - 1 << "/" << onlineID;
          m_tHistSvc->regHist(names.str().c_str(), m_LBdependence[moduleHash]).setChecked();
          names.str(""); names.clear();
          // BCID dependence
          m_BCIDdependence[moduleHash] = new TH1D(onlineID.c_str(), onlineID.c_str(), 301, -0.5, 300.5);
          names << "/histfile/BCIDdep_barrel/Layer" << layer - 1 << "/" <<onlineID;
          m_tHistSvc->regHist(names.str().c_str(), m_BCIDdependence[moduleHash]).setChecked();
          names.str(""); names.clear();
          // TOT
          m_TOTdistributions[moduleHash] = new TH1D(onlineID.c_str(), onlineID.c_str(), 256, -0.5, 255.5);
          names << "/histfile/TOT_barrel/Layer" << layer - 1 << "/" << onlineID;
          m_tHistSvc->regHist(names.str().c_str(), m_TOTdistributions[moduleHash]).setChecked();
          names.str(""); names.clear();
          // noise map
          if( m_calculateNoiseMaps ){
            m_noiseMaps[moduleHash] = new TH2C(onlineID.c_str(), onlineID.c_str(), 144, -0., 144., 328, 0., 328.);
            names << "/histfile/noiseMaps_barrel/Layer" << layer - 1 << "/" << onlineID;
            m_tHistSvc->regHist(names.str().c_str(), m_noiseMaps[moduleHash]).setChecked();
            names.str(""); names.clear();
          }
        }
      } else if ( barrel == 2 ) { // ECA
        m_moduleHashList.push_back(moduleHash);

        //tmp_onlineID = Form("D%dA_",layer + 1) + phi2moduleID_ECA[module_phi];
        //onlineID = std::string(tmp_onlineID);
        onlineID = NoiseMapBuilder::getDCSIDFromPosition(barrel,layer,module_phi,module_eta);
        // hitmap
        m_hitMaps[moduleHash] = new TH2D(onlineID.c_str(), onlineID.c_str(), 144, -0., 144., 328, 0., 328.);
        names << "/histfile/hitMaps_endcapA/Disk" << (layer + 1) << "/" << onlineID;
        m_tHistSvc->regHist(names.str().c_str(), m_hitMaps[moduleHash]).setChecked();
        names.str(""); names.clear();
        // Lumi Block dependence
        m_LBdependence[moduleHash] = new TH1D(onlineID.c_str(), onlineID.c_str(), nLBmax, -0.5, nLBmax + 0.5);
        names << "/histfile/LBdep_endcapA/Disk" << (layer + 1) << "/" << onlineID;
        m_tHistSvc->regHist(names.str().c_str(), m_LBdependence[moduleHash]).setChecked();
        names.str(""); names.clear();
        // BCID dependence
        m_BCIDdependence[moduleHash] = new TH1D(onlineID.c_str(), onlineID.c_str(), 301, -0.5, 300.5);
        names << "/histfile/BCIDdep_endcapA/Layer" << (layer + 1) << "/" <<onlineID;
        m_tHistSvc->regHist(names.str().c_str(), m_BCIDdependence[moduleHash]).setChecked();
        names.str(""); names.clear();
        // TOT
        m_TOTdistributions[moduleHash] = new TH1D(onlineID.c_str(), onlineID.c_str(), 256, -0.5, 255.5);
        names << "/histfile/TOT_endcapA/Layer" << (layer + 1) << "/" << onlineID;
        m_tHistSvc->regHist(names.str().c_str(), m_TOTdistributions[moduleHash]).setChecked();
        names.str(""); names.clear();
        // noise map
        if( m_calculateNoiseMaps ){
          m_noiseMaps[moduleHash] = new TH2C(onlineID.c_str(), onlineID.c_str(), 144, -0., 144., 328, 0., 328.);
          names << "/histfile/noiseMaps_endcapA/Disk" << (layer + 1) << "/" << onlineID;
          m_tHistSvc->regHist(names.str().c_str(), m_noiseMaps[moduleHash]).setChecked();
          names.str(""); names.clear();
        }
      } else if ( barrel == -2) { // ECC
        m_moduleHashList.push_back(moduleHash);

        //tmp_onlineID = Form("D%dC_",layer + 1) + phi2moduleID_ECC[module_phi];
        //onlineID = std::string(tmp_onlineID);
        onlineID = NoiseMapBuilder::getDCSIDFromPosition(barrel,layer,module_phi,module_eta);
        std::ostringstream names;
        // hitmap
        m_hitMaps[moduleHash] = new TH2D(onlineID.c_str(), onlineID.c_str(), 144, -0., 144., 328, 0., 328.);
        names << "/histfile/hitMaps_endcapC/Disk" << (layer + 1) << "/" << onlineID;
        m_tHistSvc->regHist(names.str().c_str(), m_hitMaps[moduleHash]).setChecked();
        names.str(""); names.clear();
        // LB dependence
        m_LBdependence[moduleHash] = new TH1D(onlineID.c_str(), onlineID.c_str(), nLBmax, -0.5, nLBmax + 0.5);
        names << "/histfile/LBdep_endcapC/Disk" << (layer + 1) << "/" << onlineID;
        m_tHistSvc->regHist(names.str().c_str(), m_LBdependence[moduleHash]).setChecked();
        names.str(""); names.clear();
        // BCID dependence
        m_BCIDdependence[moduleHash] = new TH1D(onlineID.c_str(), onlineID.c_str(), 301, -0.5, 300.5);
        names << "/histfile/BCIDdep_endcapC/Layer" << (layer + 1) << "/" <<onlineID;
        m_tHistSvc->regHist(names.str().c_str(), m_BCIDdependence[moduleHash]).setChecked();
        names.str(""); names.clear();
        // TOT
        m_TOTdistributions[moduleHash] = new TH1D(onlineID.c_str(), onlineID.c_str(), 256, -0.5, 255.5);
        names << "/histfile/TOT_endcapC/Layer" << (layer + 1) << "/" << onlineID;
        m_tHistSvc->regHist(names.str().c_str(), m_TOTdistributions[moduleHash]).setChecked();
        names.str(""); names.clear();
        // noise map
        if( m_calculateNoiseMaps ){
          m_noiseMaps[moduleHash] = new TH2C(onlineID.c_str(), onlineID.c_str(), 144, -0., 144., 328, 0., 328.);
          names << "/histfile/noiseMaps_endcapC/Disk" << (layer + 1) << "/" << onlineID;
          m_tHistSvc->regHist(names.str().c_str(), m_noiseMaps[moduleHash]).setChecked();
          names.str(""); names.clear();
        }
      } else if ( barrel == 4 ) { // DBM A
        m_moduleHashList.push_back(moduleHash);
        //tmp_onlineID = Form("LI_S15_A_" + phi2moduleID_DBM[module_phi] + "_A%d", phi2moduleNum_DBM[module_phi] + layer);
        //onlineID = std::string(tmp_onlineID);
        onlineID = NoiseMapBuilder::getDCSIDFromPosition(barrel,layer,module_phi,module_eta);
        std::ostringstream names;
        // hitmap
        m_hitMaps[moduleHash] = new TH2D(onlineID.c_str(), onlineID.c_str(), 80, -0., 80., 336, 0., 336.);
        names << "/histfile/hitMaps_DBMA/Layer" << (layer + 1) << "/" << onlineID;
        m_tHistSvc->regHist(names.str().c_str(), m_hitMaps[moduleHash]).setChecked();
        names.str(""); names.clear();
        // LB dependence
        m_LBdependence[moduleHash] = new TH1D(onlineID.c_str(), onlineID.c_str(), nLBmax, -0.5, nLBmax + 0.5);
        names << "/histfile/LBdep_DBMA/Layer" << (layer + 1) << "/" << onlineID;
        m_tHistSvc->regHist(names.str().c_str(), m_LBdependence[moduleHash]).setChecked();
        names.str(""); names.clear();
        // BCID dependence
        m_BCIDdependence[moduleHash] = new TH1D(onlineID.c_str(), onlineID.c_str(), 301, -0.5, 300.5);
        names << "/histfile/BCIDdep_DBMA/Layer" << (layer + 1) << "/" <<onlineID;
        m_tHistSvc->regHist(names.str().c_str(), m_BCIDdependence[moduleHash]).setChecked();
        names.str(""); names.clear();
        // TOT
        m_TOTdistributions[moduleHash] = new TH1D(onlineID.c_str(), onlineID.c_str(), 256, -0.5, 255.5);
        names << "/histfile/TOT_DBMA/Layer" << (layer + 1) << "/" << onlineID;
        m_tHistSvc->regHist(names.str().c_str(), m_TOTdistributions[moduleHash]).setChecked();
        names.str(""); names.clear();
        // noise map
        if( m_calculateNoiseMaps ){
          m_noiseMaps[moduleHash] = new TH2C(onlineID.c_str(), onlineID.c_str(), 80, -0., 80., 336, 0., 336.);
          names << "/histfile/noiseMaps_DBMA/Layer" << (layer + 1) << "/" << onlineID;
          m_tHistSvc->regHist(names.str().c_str(), m_noiseMaps[moduleHash]).setChecked();
          names.str(""); names.clear();
        }
      } else if ( barrel == -4) { // DBM C
        m_moduleHashList.push_back(moduleHash);
        //tmp_onlineID = Form("LI_S15_C_" + phi2moduleID_DBM[module_phi] + "_C%d", phi2moduleNum_DBM[module_phi] + layer);
        //onlineID = std::string(tmp_onlineID);
        onlineID = NoiseMapBuilder::getDCSIDFromPosition(barrel,layer,module_phi,module_eta);
        std::ostringstream names;
        // hitmap
        m_hitMaps[moduleHash] = new TH2D(onlineID.c_str(), onlineID.c_str(), 80, -0., 80., 336, 0., 336.);
        names << "/histfile/hitMaps_DBMC/Layer" << (layer + 1) << "/" << onlineID;
        m_tHistSvc->regHist(names.str().c_str(), m_hitMaps[moduleHash]).setChecked();
        names.str(""); names.clear();
        // LB dependence
	m_LBdependence[moduleHash] = new TH1D(onlineID.c_str(), onlineID.c_str(), nLBmax, -0.5, nLBmax + 0.5);
        names << "/histfile/LBdep_DBMC/Layer" << (layer + 1) << "/" << onlineID;
        m_tHistSvc->regHist(names.str().c_str(), m_LBdependence[moduleHash]).setChecked();
        names.str(""); names.clear();
        // BCID dependence
        m_BCIDdependence[moduleHash] = new TH1D(onlineID.c_str(), onlineID.c_str(), 301, -0.5, 300.5);
        names << "/histfile/BCIDdep_DBMC/Layer" << (layer + 1) << "/" <<onlineID;
        m_tHistSvc->regHist(names.str().c_str(), m_BCIDdependence[moduleHash]).setChecked();
        names.str(""); names.clear();
        // TOT
        m_TOTdistributions[moduleHash] = new TH1D(onlineID.c_str(), onlineID.c_str(), 256, -0.5, 255.5);
        names << "/histfile/TOT_DBMC/Layer" << (layer + 1) << "/" << onlineID;
        m_tHistSvc->regHist(names.str().c_str(), m_TOTdistributions[moduleHash]).setChecked();
        names.str(""); names.clear();
        // noise map
        if( m_calculateNoiseMaps ){
          m_noiseMaps[moduleHash] = new TH2C(onlineID.c_str(), onlineID.c_str(), 80, -0., 80., 336, 0., 336.);
          names << "/histfile/noiseMaps_DBMC/Layer" << (layer + 1) << "/" << onlineID;
          m_tHistSvc->regHist(names.str().c_str(), m_noiseMaps[moduleHash]).setChecked();
          names.str(""); names.clear();
        }
      }
    } // end if m_isIBL

    //for (const auto& moduleHashInList : m_moduleHashList) {
    //  std::cout << "[DEBUG] moduleHash " << moduleHashInList << std::endl;
    //}
    //  // endcap A
    //  for(int layer = 0; layer < 3; layer++){
    //    for(int module_phi = 0; module_phi < 48; module_phi++){
    //
    //      Identifier moduleID = m_pixelID->wafer_id(2, layer, module_phi, 0);
    //      IdentifierHash moduleHash = m_pixelID->wafer_hash(moduleID);
    //      std::cout << "DEBUG: " << "moduleHash ECA = " << moduleHash << std::endl; // kazuki
    //      m_moduleHashList.push_back(int(moduleHash)); // kazuki
    //
    //      std::string onlineID;
    //      TString tmp_onlineID;
    //      //
    //      if (m_isIBL) { // ----- IBL ----- //
    //        tmp_onlineID = Form("D%dA_",layer + 1) + phi2moduleID_ECA[module_phi];
    //        onlineID = std::string(tmp_onlineID); // TString 2 std::string
    //      } else { // default
    //        unsigned int hashID = ( ((m_pixelID->barrel_ec(moduleID) + 2) / 2) << 25 ) +
    //          ( m_pixelID->layer_disk(moduleID) << 23) +
    //          ( m_pixelID->phi_module(moduleID) << 17) +
    //          ( (m_pixelID->eta_module(moduleID) + 6) << 13);
    //
    //        //std::string onlineID = PixelConvert::DCSID(PixelConvert::OnlineID(hashID));
    //        onlineID = PixelConvert::DCSID(PixelConvert::OnlineID(hashID));
    //      }
    //
    //      std::ostringstream names;
    //
    //      m_hitMaps[moduleHash] = new TH2D(onlineID.c_str(), onlineID.c_str(), 144, -0., 144., 328, 0., 328.);
    //
    //      names << "/histfile/hitMaps_endcapA/Disk" << (layer + 1) << "/" << onlineID;
    //      m_tHistSvc->regHist(names.str().c_str(), m_hitMaps[moduleHash]).setChecked();
    //      names.str(""); names.clear();
    //
    //      m_LBdependence[moduleHash] = new TH1D(onlineID.c_str(), onlineID.c_str(), 2001, -0.5, 2000.5);
    //
    //      names << "/histfile/LBdep_endcapA/Disk" << (layer + 1) << "/" << onlineID;
    //      m_tHistSvc->regHist(names.str().c_str(), m_LBdependence[moduleHash]).setChecked();
    //      names.str(""); names.clear();
    //
    //      if( m_calculateNoiseMaps ){
    //        m_noiseMaps[moduleHash] = new TH2C(onlineID.c_str(), onlineID.c_str(), 144, -0., 144., 328, 0., 328.);
    //
    //        names << "/histfile/noiseMaps_endcapA/Disk" << (layer + 1) << "/" << onlineID;
    //        m_tHistSvc->regHist(names.str().c_str(), m_noiseMaps[moduleHash]).setChecked();
    //        names.str(""); names.clear();
    //      }
    //    }
    //  }

    // endcap C

    /*
       for(int layer = 0; layer < 3; layer++){
       for(int module_phi = 0; module_phi < 48; module_phi++){

       Identifier moduleID = m_pixelID->wafer_id(-2, layer, module_phi, 0);
       IdentifierHash moduleHash = m_pixelID->wafer_hash(moduleID);
       std::cout << "DEBUG: " << "moduleHash ECC = " << moduleHash << std::endl; // kazuki
       m_moduleHashList.push_back(int(moduleHash));

       std::string onlineID;
       TString tmp_onlineID;
//
if (m_isIBL) { // ----- IBL ----- //
tmp_onlineID = Form("D%dC_",layer + 1) + phi2moduleID_ECC[module_phi];
onlineID = std::string(tmp_onlineID); // TString 2 std::string
} else { // default
unsigned int hashID = ( ((m_pixelID->barrel_ec(moduleID) + 2) / 2) << 25 ) +
( m_pixelID->layer_disk(moduleID) << 23) +
( m_pixelID->phi_module(moduleID) << 17) +
( (m_pixelID->eta_module(moduleID) + 6) << 13);

    //std::string onlineID = PixelConvert::DCSID(PixelConvert::OnlineID(hashID));
    onlineID = PixelConvert::DCSID(PixelConvert::OnlineID(hashID));
    }

    std::ostringstream names;

    m_hitMaps[moduleHash] = new TH2D(onlineID.c_str(), onlineID.c_str(), 144, -0., 144., 328, 0., 328.);

    names << "/histfile/hitMaps_endcapC/Disk" << (layer + 1) << "/" << onlineID;
    m_tHistSvc->regHist(names.str().c_str(), m_hitMaps[moduleHash]).setChecked();
    names.str(""); names.clear();

    m_LBdependence[moduleHash] = new TH1D(onlineID.c_str(), onlineID.c_str(), 2001, -0.5, 2000.5);

    names << "/histfile/LBdep_endcapC/Disk" << (layer + 1) << "/" << onlineID;
    m_tHistSvc->regHist(names.str().c_str(), m_LBdependence[moduleHash]).setChecked();
    names.str(""); names.clear();

    if( m_calculateNoiseMaps ){
    m_noiseMaps[moduleHash] = new TH2C(onlineID.c_str(), onlineID.c_str(), 144, -0., 144., 328, 0., 328.);

    names << "/histfile/noiseMaps_endcapC/Disk" << (layer + 1) << "/" << onlineID;
    m_tHistSvc->regHist(names.str().c_str(), m_noiseMaps[moduleHash]).setChecked();
    names.str(""); names.clear();
    }
    }
    }
    */


// barrel


// DBM
/*
   std::map<int, int, TString> layereta2moduleID_DBM;
   layereta2moduleID_DBM[0][2] = TString("LI_S15_A_12_M1_A1_2");
   layereta2moduleID_DBM[1][2] = TString("LI_S15_A_12_M1_A2_2");
   layereta2moduleID_DBM[2][2] = TString("LI_S15_A_12_M1_A3_2");
   layereta2moduleID_DBM[0][3] = TString("LI_S15_A_34_M2_A4_2");
   layereta2moduleID_DBM[1][3] = TString("LI_S15_A_34_M2_A5_2");
   layereta2moduleID_DBM[2][3] = TString("LI_S15_A_34_M2_A6_2");
   layereta2moduleID_DBM[0][0] = TString("LI_S15_A_12_M3_A1_1");
   layereta2moduleID_DBM[1][0] = TString("LI_S15_A_12_M3_A2_1");
   layereta2moduleID_DBM[2][0] = TString("LI_S15_A_12_M3_A3_1");
   layereta2moduleID_DBM[0][1] = TString("LI_S15_A_34_M4_A4_1");
   layereta2moduleID_DBM[1][1] = TString("LI_S15_A_34_M4_A5_1");
   layereta2moduleID_DBM[2][1] = TString("LI_S15_A_34_M4_A6_1");
   */
//for(int layer = 0; layer < 3; layer++)
//      for(int layer = 0; layer < nlayer; layer++) // kazuki
//      {
//
//        /*
//           int module_phi_max = 0;
//           if(layer == 0) module_phi_max = 22;
//           if(layer == 1) module_phi_max = 38;
//           if(layer == 2) module_phi_max = 52;
//           */
//
//        int module_eta_min(0);
//        int module_eta_max(0);
//        if(m_isIBL && layer == 0) { // IBL
//          module_eta_min = -10;
//          module_eta_max = 9;
//        } else { // Pixel Barrel
//          module_eta_min = -6;
//          module_eta_max = 6;
//        }
//
//        // kazuki
//        /*
//           std::cout << "DEBUG: " << "\n"
//           << "m_pixelID->phi_module_max(0) = " << m_pixelID->phi_module_max(m_pixelID->wafer_id(-2, 0, 0, 0)) << "\n"
//           << "m_pixelID->eta_module_max(0) = " << m_pixelID->eta_module_max(m_pixelID->wafer_id(-2, 0, 0, 0)) << "\n"
//           << "m_pixelID->eta_module_min(0) = " << m_pixelID->eta_module_min(m_pixelID->wafer_id(-2, 0, 0, 0)) << "\n"
//           << "m_pixelID->phi_index_max(0) = " << m_pixelID->phi_index_max(m_pixelID->wafer_id(-2, 0, 0, 0)) << "\n"
//           << "m_pixelID->eta_index_max(0) = " << m_pixelID->eta_index_max(m_pixelID->wafer_id(-2, 0, 0, 0)) << "\n"
//           << "m_pixelID->phi_module_max(144) = " << m_pixelID->phi_module_max(m_pixelID->wafer_id(0, 0, 0, 0)) << "\n"
//           << "m_pixelID->eta_module_max(144) = " << m_pixelID->eta_module_max(m_pixelID->wafer_id(0, 0, 0, 0)) << "\n"
//           << "m_pixelID->eta_module_min(144) = " << m_pixelID->eta_module_min(m_pixelID->wafer_id(0, 0, 0, 0)) << "\n"
//           << "m_pixelID->phi_index_max(144) = " << m_pixelID->phi_index_max(m_pixelID->wafer_id(0, 0, 0, 0)) << "\n"
//           << "m_pixelID->eta_index_max(144) = " << m_pixelID->eta_index_max(m_pixelID->wafer_id(0, 0, 0, 0)) << "\n"
//           << "m_pixelID->phi_module_max(0, 1, 0, 0) = " << m_pixelID->phi_module_max(m_pixelID->wafer_id(0, 1, 0, 0)) << "\n"
//           << "m_pixelID->eta_module_max(0, 1, 0, 0) = " << m_pixelID->eta_module_max(m_pixelID->wafer_id(0, 1, 0, 0)) << "\n"
//           << "m_pixelID->eta_module_min(0, 1, 0, 0) = " << m_pixelID->eta_module_min(m_pixelID->wafer_id(0, 1, 0, 0)) << "\n"
//           << "m_pixelID->phi_index_max(0, 1, 0, 0) = " << m_pixelID->phi_index_max(m_pixelID->wafer_id(0, 1, 0, 0)) << "\n"
//           << "m_pixelID->eta_index_max(0, 1, 0, 0) = " << m_pixelID->eta_index_max(m_pixelID->wafer_id(0, 1, 0, 0)) << "\n"
//           << "m_pixelID->phi_module_max(0, 2, 0, 0) = " << m_pixelID->phi_module_max(m_pixelID->wafer_id(0, 2, 0, 0)) << "\n"
//           << "m_pixelID->eta_module_max(0, 2, 0, 0) = " << m_pixelID->eta_module_max(m_pixelID->wafer_id(0, 2, 0, 0)) << "\n"
//           << "m_pixelID->eta_module_min(0, 2, 0, 0) = " << m_pixelID->eta_module_min(m_pixelID->wafer_id(0, 2, 0, 0)) << "\n"
//           << "m_pixelID->phi_index_max(0, 2, 0, 0) = " << m_pixelID->phi_index_max(m_pixelID->wafer_id(0, 2, 0, 0)) << "\n"
//           << "m_pixelID->eta_index_max(0, 2, 0, 0) = " << m_pixelID->eta_index_max(m_pixelID->wafer_id(0, 2, 0, 0)) << "\n"
//           << "m_pixelID->phi_module_max(0, 3, 0, 0) = " << m_pixelID->phi_module_max(m_pixelID->wafer_id(0, 3, 0, 0)) << "\n"
//           << "m_pixelID->eta_module_max(0, 3, 0, 0) = " << m_pixelID->eta_module_max(m_pixelID->wafer_id(0, 3, 0, 0)) << "\n"
//           << "m_pixelID->eta_module_min(0, 3, 0, 0) = " << m_pixelID->eta_module_min(m_pixelID->wafer_id(0, 3, 0, 0)) << "\n"
//           << "m_pixelID->phi_index_max(0, 3, 0, 0) = " << m_pixelID->phi_index_max(m_pixelID->wafer_id(0, 3, 0, 0)) << "\n"
//           << "m_pixelID->eta_index_max(0, 3, 0, 0) = " << m_pixelID->eta_index_max(m_pixelID->wafer_id(0, 3, 0, 0)) << "\n"
//           << "m_pixelID->phi_module_max(1740) = " << m_pixelID->phi_module_max(m_pixelID->wafer_id(2, 0, 0, 0)) << "\n"
//           << "m_pixelID->eta_module_max(1740) = " << m_pixelID->eta_module_max(m_pixelID->wafer_id(2, 0, 0, 0)) << "\n"
//           << "m_pixelID->eta_module_min(1740) = " << m_pixelID->eta_module_min(m_pixelID->wafer_id(2, 0, 0, 0)) << "\n"
//           << "m_pixelID->phi_index_max(1740) = " << m_pixelID->phi_index_max(m_pixelID->wafer_id(2, 0, 0, 0)) << "\n"
//           << "m_pixelID->eta_index_max(1740) = " << m_pixelID->eta_index_max(m_pixelID->wafer_id(2, 0, 0, 0))
//           << std::endl;
//           std::cout << "DEBUG: " << "\n"
//           << "m_pixelID->wafer_hash_max(): " << m_pixelID->wafer_hash_max() << "\n"
//           << "m_pixelID->pixel_hash_max(): " << m_pixelID->pixel_hash_max()
//           << std::endl;
//           */
//        //for(int module_phi = 0; module_phi < module_phi_max; module_phi++)
//        for(int module_phi = 0; module_phi < module_phi_max[layer]; module_phi++) // kazuki
//        {
//          //for(int module_eta = -6; module_eta < 7; module_eta++)
//          for(int module_eta = module_eta_min; module_eta <= module_eta_max; module_eta++)
//          {
//
//            Identifier moduleID = m_pixelID->wafer_id(0, layer, module_phi, module_eta);
//            IdentifierHash moduleHash = m_pixelID->wafer_hash(moduleID);
//            std::cout << "DEBUG: " << "moduleHash Barrel = " << moduleHash << std::endl; // kazuki
//            m_moduleHashList.push_back(int(moduleHash));
//
//            std::string onlineID;
//            TString tmp_onlineID;
//            //
//            if (m_isIBL) { // ----- IBL ----- //
//              if (layer == 0) {
//                tmp_onlineID = Form("LI_S%02d_",module_phi + 1) + eta2moduleID_IBL[module_eta];
//              } else if (layer == 1) {
//                tmp_onlineID = Form("L%d_",layer - 1) + phi2moduleID_BLayer[module_phi] + eta2moduleID_PixelBarrel[module_eta];
//              } else {
//                tmp_onlineID = Form("L%d_",layer - 1) + phi2moduleID_Layer12[module_phi] + eta2moduleID_PixelBarrel[module_eta];
//              }
//              onlineID = std::string(tmp_onlineID); // TString 2 std::string
//            } else { // default
//              unsigned int hashID = ( ((m_pixelID->barrel_ec(moduleID) + 2) / 2) << 25 ) +
//                ( m_pixelID->layer_disk(moduleID) << 23) +
//                ( m_pixelID->phi_module(moduleID) << 17) +
//                ( (m_pixelID->eta_module(moduleID) + 6) << 13);
//
//              onlineID = PixelConvert::DCSID(PixelConvert::OnlineID(hashID));
//            }
//
//            std::ostringstream names;
//
//            if(m_isIBL && layer == 0) { // ----- IBL ----- //
//              m_hitMaps[moduleHash] = new TH2D(onlineID.c_str(), onlineID.c_str(), 160, -0., 160., 336, 0., 336.);
//            } else {
//              m_hitMaps[moduleHash] = new TH2D(onlineID.c_str(), onlineID.c_str(), 144, -0., 144., 328, 0., 328.);
//            }
//
//            if(m_isIBL) { // ----- IBL ----- //
//              if(layer == 0) names << "/histfile/hitMaps_barrel/IBL/" << onlineID;
//              else if(layer == 1) names << "/histfile/hitMaps_barrel/B-Layer/" << onlineID;
//              else names << "/histfile/hitMaps_barrel/Layer" << layer - 1 << "/" << onlineID;
//            } else { // only Pixel
//              if(layer == 0) names << "/histfile/hitMaps_barrel/B-layer/" << onlineID;
//              else names << "/histfile/hitMaps_barrel/Layer" << layer << "/" << onlineID;
//            }
//            m_tHistSvc->regHist(names.str().c_str(), m_hitMaps[moduleHash]).setChecked();
//            names.str(""); names.clear();
//
//            m_LBdependence[moduleHash] = new TH1D(onlineID.c_str(), onlineID.c_str(), 2001, -0.5, 2000.5);
//
//            if(m_isIBL) { // ----- IBL ----- //
//              if(layer == 0) names << "/histfile/LBdep_barrel/IBL/" << onlineID;
//              else if(layer == 1) names << "/histfile/LBdep_barrel/B-Layer/" << onlineID;
//              else names << "/histfile/LBdep_barrel/Layer" << layer - 1 << "/" << onlineID;
//            } else { // only Pixel
//              if(layer == 0) names << "/histfile/LBdep_barrel/B-layer/" << onlineID;
//              else names << "/histfile/LBdep_barrel/Layer" << layer << "/" << onlineID;
//            }
//            m_tHistSvc->regHist(names.str().c_str(), m_LBdependence[moduleHash]).setChecked();
//            names.str(""); names.clear();
//
//            if( m_calculateNoiseMaps ){
//              if(m_isIBL && layer == 0) { // ----- IBL ----- //
//                m_noiseMaps[moduleHash] = new TH2C(onlineID.c_str(), onlineID.c_str(), 244, -0., 244., 377, 0., 377.);
//              } else {
//                m_noiseMaps[moduleHash] = new TH2C(onlineID.c_str(), onlineID.c_str(), 144, -0., 144., 328, 0., 328.);
//              }
//
//              if(m_isIBL) {
//                if(layer == 0) names << "/histfile/noiseMaps_barrel/IBL/" << onlineID;
//                else if(layer == 1) names << "/histfile/noiseMaps_barrel/B-layer/" << onlineID;
//                else names << "/histfile/noiseMaps_barrel/Layer" << layer - 1 << "/" << onlineID;
//              } else { // only Pixel
//                if(layer == 0) names << "/histfile/noiseMaps_barrel/B-layer/" << onlineID;
//                else names << "/histfile/noiseMaps_barrel/Layer" << layer << "/" << onlineID;
//              }
//              m_tHistSvc->regHist(names.str().c_str(), m_noiseMaps[moduleHash]).setChecked();
//              names.str(""); names.clear();
//            }
//          }
//        }
//      }

//m_disabledModules = new TH1D("DisabledModules", "Number of events disabled vs. IdentifierHash", 1744, 0, 1744);
  }
  m_disabledModules = new TH1D("DisabledModules", "Number of events disabled vs. IdentifierHash", 2048, 0, 2048);
  m_tHistSvc->regHist("/histfile/DisabledModules", m_disabledModules).setChecked();

  if (m_calculateNoiseMaps) {
    m_overlayedPixelNoiseMap = new TH2D("overlayedPixelNoiseMap", "Noisy pixel map overlayed all Pixel modules", 144, -0., 144., 328, 0., 328.);
    m_tHistSvc->regHist("/histfile/overlayedPixelNoiseMap", m_overlayedPixelNoiseMap).setChecked();

    m_overlayedIBLDCNoiseMap = new TH2D("overlayedIBLDCNoiseMap", "Noisy pixel map overlayed all IBL Planar modules", 160, -0., 160., 336, 0., 336.);
    m_overlayedIBLSCNoiseMap = new TH2D("overlayedIBLSCNoiseMap", "Noisy pixel map overlayed all IBL 3D modules", 80, -0., 80., 336, 0., 336.);
    if (m_isIBL) {
      m_tHistSvc->regHist("/histfile/overlayedIBLDCNoiseMap", m_overlayedIBLDCNoiseMap).setChecked();
      m_tHistSvc->regHist("/histfile/overlayedIBLSCNoiseMap", m_overlayedIBLSCNoiseMap).setChecked();
    }
  }

  return StatusCode::SUCCESS;
}



StatusCode NoiseMapBuilder::execute(){

  ATH_MSG_DEBUG( "Executing NoiseMapBuilder" );


  const EventInfo* eventInfo;

  StatusCode sc = sgSvc()->retrieve(eventInfo);
  if( !sc.isSuccess() ){
    ATH_MSG_FATAL( "Unable to retrieve event info" );
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "Event info retrieved" );
  int LB =  static_cast<int>(eventInfo->event_ID()->lumi_block());
  if( LB < m_lbMin ||
      ( m_lbMax >= m_lbMin && LB > m_lbMax ) ){

    ATH_MSG_VERBOSE( "Event in lumiblock " << eventInfo->event_ID()->lumi_block() <<
        " not in selected range [" << m_lbMin << "," << m_lbMax << "], skipped");

    return StatusCode::SUCCESS;
  }


  const DataHandle< PixelRDO_Container > pixelRDOs;

  sc = sgSvc()->retrieve(pixelRDOs, m_pixelRDOKey);
  if( !sc.isSuccess() ){
    ATH_MSG_FATAL( "Unable to retrieve pixel RDO container at " << m_pixelRDOKey );
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "Pixel RDO container retrieved" );


  for(PixelRDO_Container::const_iterator coll = pixelRDOs->begin(); coll != pixelRDOs->end(); coll++){

    const InDetRawDataCollection<PixelRDORawData>* PixelRDOCollection(*coll);

    if(PixelRDOCollection != 0){
      Identifier moduleID = PixelRDOCollection->identify();
      IdentifierHash moduleHash = m_pixelID->wafer_hash(moduleID);
      //std::cout << "DEBUG: " << "moduleID, moduleHash = " << moduleID << ", " << moduleHash << std::endl;
      if  ( !(m_pixelConditionsSummarySvc->isGood(moduleHash)) ) {
        //takubo std::cout << "[DEBUG] !(m_pixelConditionsSummarySvc->isGood(moduleHash)) : moduleID = " << moduleID << std::endl;
        continue;// exclude bad modules
      }
      int errors = m_BSErrorsSvc->getModuleErrors(moduleHash);
      if ( ( errors & 0x0001C000 ) ) {
        //takubo std::cout << "[DEBUG] errors & 0x0001C000 : moduleID = " << moduleID << std::endl;
        continue;                            // exclude FE synch errors
      }
      for(DataVector<PixelRDORawData>::const_iterator rdo = PixelRDOCollection->begin();
          rdo!=PixelRDOCollection->end(); ++rdo){

        Identifier rdoID = (*rdo)->identify();
        unsigned int pixel_eta = m_pixelID->eta_index(rdoID);
        unsigned int pixel_phi = m_pixelID->phi_index(rdoID);

        // for debug
        int TOT = (*rdo)->getToT(); // it returns a 8 bits "word"
        int BCID = (*rdo)->getBCID();
        //int LVL1ID = (*rdo)->getLVL1ID();
        //int LVL1A = (*rdo)->getLVL1A();

        //if( std::find(m_moduleHashList.begin(), m_moduleHashList.end(), moduleHash ) == m_moduleHashList.end() ) continue;
        //if ( std::binary_search(m_moduleHashList.begin(), m_moduleHashList.end(), moduleHash) == false ) continue;
        m_hitMaps[moduleHash]->Fill(pixel_eta, pixel_phi);
        m_LBdependence[moduleHash]->Fill(LB);
        // debug
        //takubo if(moduleHash == 200) std::cout << "[DEBUG] BCID = " << BCID << std::endl;
        m_BCIDdependence[moduleHash]->Fill(BCID);
        m_TOTdistributions[moduleHash]->Fill(TOT);
      }
    }
  }


  for(unsigned int moduleHash = 0; moduleHash < m_pixelID->wafer_hash_max(); moduleHash++) {
    if( !m_pixelConditionsSummarySvc->isActive( moduleHash ) ){
      m_disabledModules->Fill( moduleHash );
    }
  }

  m_nEvents++;
  m_nEventsHist->Fill(.5);

  m_nEventsLBHist->Fill(LB);

  return StatusCode::SUCCESS;
}


StatusCode NoiseMapBuilder::finalize() {

  ATH_MSG_INFO( "Finalizing NoiseMapBuilder" );

  if( m_occupancyPerBC ){
    m_nEvents *= m_nBCReadout;
  }

  int minLogOccupancy = 8;
  double minOccupancy = pow(10.,-minLogOccupancy);

  TH1D* globalOccupancy= new TH1D("occupancy","Pixel occupancy",minLogOccupancy*10,-minLogOccupancy,0.);
  m_tHistSvc->regHist("/histfile/occupancy",globalOccupancy).setChecked();

  std::vector<std::string> components;
  components.push_back("Disk1A");
  components.push_back("Disk2A");
  components.push_back("Disk3A");
  components.push_back("Disk1C");
  components.push_back("Disk2C");
  components.push_back("Disk3C");
  if (m_isIBL) components.push_back("IBL"); // ----- IBL ----- //
  components.push_back("B-layer");
  components.push_back("Layer1");
  components.push_back("Layer2");
  if (m_isIBL) components.push_back("DBMA");
  if (m_isIBL) components.push_back("DBMC");

  std::vector<std::string> types;
  types.push_back("Normal");
  types.push_back("Ganged");
  types.push_back("InterGanged");
  types.push_back("Long");
  types.push_back("Long-Ganged");
  types.push_back("Long-InterGanged");

  //std::cout << "debug point 4" << std::endl;

  std::map<std::string, TH1D*> h_occupancy;

  for( std::vector<std::string>::const_iterator component = components.begin();
      component != components.end(); ++component) {
    h_occupancy[*component] =
      new TH1D(("occupancy" + (*component)).c_str(), ("Pixel occupancy " + (*component)).c_str(),
          minLogOccupancy*10,-minLogOccupancy,0.);

    m_tHistSvc->regHist(("/histfile/occupancy" + (*component)).c_str(), h_occupancy[*component]).setChecked();
  }

  for( std::vector<std::string>::const_iterator type = types.begin();
      type != types.end(); ++type){
    h_occupancy[*type] =
      new TH1D(("occupancy" + (*type)).c_str(), ("Pixel occupancy " + (*type)).c_str(),
          minLogOccupancy*10,-minLogOccupancy,0.);

    m_tHistSvc->regHist(("/histfile/occupancy" + (*type)).c_str(), h_occupancy[*type]).setChecked();
  }

  //std::cout << "debug point 5" << std::endl;

  // <== kazuki
  TH2F* nhitsPlotBI=new TH2F("nhitsPlotBI","Number of hits BI;module_eta;module_phi",20,-10,10,14,-0.5,13.5); // ----- IBL ----- //
  m_tHistSvc->regHist("/histfile/nhitsPlotBI",nhitsPlotBI).setChecked();

  TH2F* nhitsPlotB0=new TH2F("nhitsPlotB0","Number of hits B0;module_eta;module_phi",13,-6.5,6.5,22,-0.5,21.5);
  m_tHistSvc->regHist("/histfile/nhitsPlotB0",nhitsPlotB0).setChecked();

  TH2F* nhitsPlotB1=new TH2F("nhitsPlotB1","Number of hits B1;module_eta;module_phi",13,-6.5,6.5,38,-0.5,37.5);
  m_tHistSvc->regHist("/histfile/nhitsPlotB1",nhitsPlotB1).setChecked();

  TH2F* nhitsPlotB2=new TH2F("nhitsPlotB2","Number of hits B2;module_eta;module_phi",13,-6.5,6.5,52,-0.5,51.5);
  m_tHistSvc->regHist("/histfile/nhitsPlotB2",nhitsPlotB2).setChecked();

  TH2F* nhitsPlotEC=new TH2F("nhitsPlotEC","Number of hits Endcap;Disk;module_phi",7,-3.5,3.5,48,-0.5,47.5);
  m_tHistSvc->regHist("/histfile/nhitsPlotEC",nhitsPlotEC).setChecked();

  TH2F* nhitsPlotDBM=new TH2F("nhitsPlotDBM","Number of hits DBM;Layer;module_phi",7,-3.5,3.5,4,-0.5,3.5);
  m_tHistSvc->regHist("/histfile/nhitsPlotDBM",nhitsPlotDBM).setChecked();

  TH2F* nhitsNoNoisePlotBI=new TH2F("nhitsNoNoisePlotBI","Number of hits without Noise BI;module_eta;module_phi",20,-10,10,14,-0.5,13.5); // ----- IBL ----- //
  m_tHistSvc->regHist("/histfile/nhitsNoNoisePlotBI",nhitsNoNoisePlotBI).setChecked();

  TH2F* nhitsNoNoisePlotB0=new TH2F("nhitsNoNoisePlotB0","Number of hits without Noise B0;module_eta;module_phi",13,-6.5,6.5,22,-0.5,21.5);
  m_tHistSvc->regHist("/histfile/nhitsNoNoisePlotB0",nhitsNoNoisePlotB0).setChecked();

  TH2F* nhitsNoNoisePlotB1=new TH2F("nhitsNoNoisePlotB1","Number of hits without Noise B1;module_eta;module_phi",13,-6.5,6.5,38,-0.5,37.5);
  m_tHistSvc->regHist("/histfile/nhitsNoNoisePlotB1",nhitsNoNoisePlotB1).setChecked();

  TH2F* nhitsNoNoisePlotB2=new TH2F("nhitsNoNoisePlotB2","Number of hits without Noise B2;module_eta;module_phi",13,-6.5,6.5,52,-0.5,51.5);
  m_tHistSvc->regHist("/histfile/nhitsNoNoisePlotB2",nhitsNoNoisePlotB2).setChecked();
  // ==> kazuki

  TH1D* maskedPlot= new TH1D("maskedPlot","Disabled pixel per module",50,0.5,50.5);
  m_tHistSvc->regHist("/histfile/maskedPlot",maskedPlot).setChecked();

  TH2F* disablePlotBI=new TH2F("disablePlotBI","Disabled pixels BI;module_eta;module_phi",20,-10,10,14,-0.5,13.5); // ----- IBL ----- //
  m_tHistSvc->regHist("/histfile/disablePlotBI",disablePlotBI).setChecked();

  TH2F* disablePlotB0=new TH2F("disablePlotB0","Disabled pixels B0;module_eta;module_phi",13,-6.5,6.5,22,-0.5,21.5);
  m_tHistSvc->regHist("/histfile/disablePlotB0",disablePlotB0).setChecked();

  TH2F* disablePlotB1=new TH2F("disablePlotB1","Disabled pixels B1;module_eta;module_phi",13,-6.5,6.5,38,-0.5,37.5);
  m_tHistSvc->regHist("/histfile/disablePlotB1",disablePlotB1).setChecked();

  TH2F* disablePlotB2=new TH2F("disablePlotB2","Disabled pixels B2;module_eta;module_phi",13,-6.5,6.5,52,-0.5,51.5);
  m_tHistSvc->regHist("/histfile/disablePlotB2",disablePlotB2).setChecked();

  TH2F* disablePlotEC=new TH2F("disablePlotEC","Disabled pixels Endcap;Disk;module_phi",7,-3.5,3.5,48,-0.5,47.5);
  m_tHistSvc->regHist("/histfile/disablePlotEC",disablePlotEC).setChecked();

  TH2F* disablePlotDBM=new TH2F("disablePlotDBM","Disabled pixels DBM;Layer;module_phi",7,-3.5,3.5,4,-0.5,3.5);
  m_tHistSvc->regHist("/histfile/disablePlotDBM",disablePlotDBM).setChecked();

  //std::cout << "debug point 6" << std::endl;

  int totalDisabledPixels=0,
      totalDisabledModules=0,
      modulesWithHits=0,
      modulesWithDisabledPixels=0;

  // kazuki
  /*
     std::cout << "DEBUG: " << "\n"
     << "m_pixelID->wafer_hash_max(): " << m_pixelID->wafer_hash_max() << "\n"
     << "m_pixelID->pixel_hash_max(): " << m_pixelID->pixel_hash_max()
     << std::endl;
     */
  std::sort(m_moduleHashList.begin(), m_moduleHashList.end());
  InDetDD::SiDetectorElementCollection::const_iterator iter, itermin, itermax;
  itermin = m_pixman->getDetectorElementBegin();
  itermax = m_pixman->getDetectorElementEnd();
  if(m_pixelID->wafer_hash_max() > 1744) m_isIBL = true; // #modules only Pixel is 1744
  //for(unsigned int moduleHash = 0; moduleHash < m_pixelID->wafer_hash_max(); moduleHash++)
  //for(std::vector<int>::iterator it = m_moduleHashList.begin(); it != m_moduleHashList.end(); ++it)
  for( iter = itermin; iter != itermax; ++iter)
  {
    //int moduleHash = *it;

    /*
       if ( std::binary_search(m_moduleHashList.begin(), m_moduleHashList.end(), moduleHash) == false )
       { // kazuki
       std::cout << "DEBUG: " << moduleHash << " is not in m_moduleHashList" << std::endl;
       continue; // kazuki
       }
       */
    const InDetDD::SiDetectorElement* element = *iter;
    if(element == 0) continue;
    Identifier ident = element->identify();
    if(!m_pixelID->is_pixel(ident)) continue;  // OK this Element is included
    const InDetDD::PixelModuleDesign* design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design());
    if(!design) continue;
    //unsigned int mchips = design->numberOfCircuits();
    int barrel     = m_pixelID->barrel_ec (ident);
    int layer      = m_pixelID->layer_disk(ident); // kazuki
    int module_phi = m_pixelID->phi_module(ident);
    int module_eta = m_pixelID->eta_module(ident); // kazuki
    //Identifier moduleID = m_pixelID->wafer_id(IdentifierHash(moduleHash));
    int phi_max    = m_pixelID->phi_index_max(ident);
    int eta_max    = m_pixelID->eta_index_max(ident);
    int moduleHash = m_pixelID->wafer_hash(ident);

    /*
    Identifier moduleID = m_pixelID->wafer_id(IdentifierHash(moduleHash));
    int barrel     = m_pixelID->barrel_ec(moduleID); // -2=ECC, 0=Barrel, 2=ECA
    int layer      = m_pixelID->layer_disk(moduleID);
    int module_phi = m_pixelID->phi_module(moduleID);
    int module_eta = m_pixelID->eta_module(moduleID);
    int phi_max    = m_pixelID->phi_index_max(moduleID);
    int eta_max    = m_pixelID->eta_index_max(moduleID);
    */

    TH2F* nhitsNoNoisePlot = 0; // kazuki
    std::string component;
    double cut = 0.;

    //if(moduleHash == 0 || moduleHash == 1000) std::cout << "debug point 7: " << moduleHash << std::endl;

    if ( barrel != 0 ) { // if Disk (or DBM)

      //if (barrel == -2)
      if (barrel == 2) // kazuki
      { // if A side
        if(layer == 0){cut = m_disk1ACut; component = "Disk1A";}
        else if(layer == 1){cut = m_disk2ACut; component = "Disk2A";}
        else if(layer == 2){cut = m_disk3ACut; component = "Disk3A";}
      }
      //else if (barrel == -2)
      else if (barrel == -2) // kazuki
      { // if C side
        if(layer == 0){cut = m_disk1CCut; component = "Disk1C";}
        else if(layer == 1){cut = m_disk2CCut; component = "Disk2C";}
        else if(layer == 2){cut = m_disk3CCut; component = "Disk3C";}
      }
      else if (barrel ==  4) // kazuki
      {
        cut = m_dbmCut; component = "DBMA";
      }
      else if (barrel == -4) // kazuki
      {
        cut = m_dbmCut; component = "DBMC";
      }
    } else if ( barrel == 0 ) { // if barrel
      if (m_isIBL) { // ----- IBL ----- //
        if(layer == 0){
          cut = m_iblCut;
          nhitsNoNoisePlot = nhitsNoNoisePlotBI;
          component = "IBL";
        }
        else if(layer == 1) { // CAUTION: layer #1 = BLayer since IBL installed
          cut = m_bLayerCut;
          nhitsNoNoisePlot = nhitsNoNoisePlotB0; // kazuki
          component = "B-layer";
        }
        else if(layer == 2) {
          cut = m_layer1Cut;
          nhitsNoNoisePlot = nhitsNoNoisePlotB1; // kazuki
          component = "Layer1";
        }
        else if(layer == 3) {
          cut = m_layer2Cut;
          nhitsNoNoisePlot = nhitsNoNoisePlotB2; // kazuki
          component = "Layer2";
        }
      } else { // only Pixel
        if(layer == 0){
          cut = m_bLayerCut;
          nhitsNoNoisePlot = nhitsNoNoisePlotB0; // kazuki
          component = "B-layer";
        }
        else if(layer == 1) {
          cut = m_layer1Cut;
          nhitsNoNoisePlot = nhitsNoNoisePlotB1; // kazuki
          component = "Layer1";
        }
        else if(layer == 2) {
          cut = m_layer2Cut;
          nhitsNoNoisePlot = nhitsNoNoisePlotB2; // kazuki
          component = "Layer2";
        }
        else continue;
      }
    }

    //if(moduleHash == 0 || moduleHash == 1000) std::cout << "debug point 8: " << moduleHash << std::endl;

    if( m_BSErrorsSvc->getReadEvents(moduleHash)==0 && m_hitMaps[moduleHash]->GetEntries()==0 ) // original
      //if( m_hitMaps[moduleHash]->GetEntries() == 0 ) // m_BSErrorSVc->getReadEvents returns seg fault //////////////////////////////////////////////////////////////////////////////////////////
    {

      //if(moduleHash == 0 || moduleHash == 1000) std::cout << "debug point 9: " << moduleHash << std::endl;

      if (!m_isIBL) {
        unsigned int hashID = ( ((m_pixelID->barrel_ec(ident) + 2) / 2) << 25 ) +
          ( m_pixelID->layer_disk(ident) << 23) +
          ( m_pixelID->phi_module(ident) << 17) +
          ( (m_pixelID->eta_module(ident) + 6) << 13);
        ATH_MSG_INFO( "Disabled module "
            << PixelConvert::OnlineID(hashID) << "\t"
            << PixelConvert::DCSID(PixelConvert::OnlineID(hashID)) );
      }

      //if(moduleHash == 0 || moduleHash == 1000) std::cout << "debug point 10: " << moduleHash << std::endl;

      if (barrel== 0) {
        if (m_isIBL) { // ----- IBL ----- //
          if(layer == 0){
            disablePlotBI->Fill(module_eta,module_phi,-1);
          }
          else if(layer == 1) { // CAUTION: layer #1 = BLayer since IBL installed
            disablePlotB0->Fill(module_eta,module_phi,-1);
          }
          else if(layer == 2) {
            disablePlotB1->Fill(module_eta,module_phi,-1);
          }
          else if(layer == 3) {
            disablePlotB2->Fill(module_eta,module_phi,-1);
          }
        }
        else {
          if(layer == 0){
            disablePlotB0->Fill(module_eta,module_phi,-1);
          }
          else if(layer == 1) {
            disablePlotB1->Fill(module_eta,module_phi,-1);
          }
          else if(layer == 2) {
            disablePlotB2->Fill(module_eta,module_phi,-1);
          }
        }
      }
      else if (barrel== 2) { disablePlotEC->Fill(layer+1,module_phi,-1); }
      else if (barrel==-2) { disablePlotEC->Fill(-(layer+1),module_phi,-1); }
      else if (barrel== 4) { disablePlotDBM->Fill(layer+1,module_phi,-1); }
      else if (barrel==-4) { disablePlotDBM->Fill(-(layer+1),module_phi,-1); }
      totalDisabledModules++;

      continue;
    }
    else if( m_hitMaps[moduleHash]->GetEntries() != 0 ) ///////////////////////////////////////////////////////////////////////////////
    {
      if (barrel== 0) {
        if (m_isIBL) { // ----- IBL ----- //
          if(layer == 0){
            nhitsPlotBI->Fill(module_eta,module_phi,m_hitMaps[moduleHash]->GetEntries());
          }
          else if(layer == 1) { // CAUTION: layer #1 = BLayer since IBL installed
            nhitsPlotB0->Fill(module_eta,module_phi,m_hitMaps[moduleHash]->GetEntries());
          }
          else if(layer == 2) {
            nhitsPlotB1->Fill(module_eta,module_phi,m_hitMaps[moduleHash]->GetEntries());
          }
          else if(layer == 3) {
            nhitsPlotB2->Fill(module_eta,module_phi,m_hitMaps[moduleHash]->GetEntries());
          }
        }
        else {
          if(layer == 0){
            nhitsPlotB0->Fill(module_eta,module_phi,m_hitMaps[moduleHash]->GetEntries());
          }
          else if(layer == 1) {
            nhitsPlotB1->Fill(module_eta,module_phi,m_hitMaps[moduleHash]->GetEntries());
          }
          else if(layer == 2) {
            nhitsPlotB2->Fill(module_eta,module_phi,m_hitMaps[moduleHash]->GetEntries());
          }
        }
      }
      else if (barrel== 2) {
        nhitsPlotEC->Fill(layer+1,module_phi,m_hitMaps[moduleHash]->GetEntries());
      }
      else if (barrel==-2) { 
        nhitsPlotEC->Fill(-(layer+1),module_phi,m_hitMaps[moduleHash]->GetEntries());
      }
      else if (barrel== 4) { 
        nhitsPlotDBM->Fill(layer+1,module_phi,m_hitMaps[moduleHash]->GetEntries());
      }
      else if (barrel==-4) { 
        nhitsPlotDBM->Fill(-(layer+1),module_phi,m_hitMaps[moduleHash]->GetEntries());
      }

      modulesWithHits++;
    }

    //if(moduleHash == 0 || moduleHash == 1000) std::cout << "debug point 11: " << moduleHash << std::endl;

    int thisModuleCut = 0;
    bool isIBL3D = ( m_isIBL && barrel == 0 && layer == 0 && (module_eta <= -7 || module_eta >= 6) ) ? true : false;

    for(int pixel_eta = 0; pixel_eta <= eta_max; pixel_eta++){
      for(int pixel_phi = 0; pixel_phi <= phi_max; pixel_phi++){

        // kazuki added from here
        int pixel_eta_on_chip = (m_isIBL && barrel == 0 && layer == 0) ? pixel_eta % 80 : pixel_eta % 18; // column
        int pixel_phi_on_chip = (pixel_phi <= 163) ? pixel_phi : 327 - pixel_phi; // eta
        if (m_isIBL && barrel == 0 && layer == 0) pixel_phi_on_chip = pixel_phi;
        int pixelType = 0;

        if (m_isIBL && barrel == 0 && layer == 0) { // ----- IBL ----- //
          if( !isIBL3D && (pixel_eta_on_chip == 0 || pixel_eta_on_chip == 80 - 1) ){
            pixelType = 1; // long
          }
          //else if(pixel_eta_on_chip > 0 && pixel_eta_on_chip < 80 - 1) // pixel size = 50x250 um2
          else { // pixel size = 50x250 um2
            pixelType = 0; // normal
          }
        } else { // Pixel
          if(pixel_eta_on_chip > 0 && pixel_eta_on_chip < 18 - 1){ // pixel size = 50x400 um2
            pixelType = 0; // normal
            for(int kk = 0; kk < 3; kk++){
              // row 154,156,158                       = inter-ganged
              // row 153,155,157,159,  160,161,162,163 = ganged
              if(pixel_phi_on_chip == (153 + 2 * kk + 1)){
                pixelType = 5; // inter-ganged (dealt as normal)
                break;
              }
              if(pixel_phi_on_chip == (153 + 2 * kk) || pixel_phi_on_chip >= 159){
                pixelType = 2; // ganged
                break;
              }
            }
          }
          else if(pixel_eta_on_chip == 0 || pixel_eta_on_chip == 18 - 1){
            pixelType =  1; //long
            for(int kk = 0; kk < 3; kk++){
              if(pixel_phi_on_chip == (153 + 2 * kk + 1)){
                pixelType = 6; // long inter-ganged (dealt as long)
                break;
              }
              if(pixel_phi_on_chip == (153 + 2 * kk) || pixel_phi_on_chip >= 159){
                pixelType = 3; // long ganged
                break;
              }
            }
          }
          else
            pixelType =  8; //invalid pixel_phi/pixel_eta pair
        }
        // to here

        std::string type;
         // kazuki commented out
        //int pixel_eta_on_chip = (m_isIBL && barrel == 0 && layer == 0) ? pixel_eta % 80 : pixel_eta % 18; // column
        //int pixel_phi_on_chip = (pixel_phi <= 163) ? pixel_phi : 327 - pixel_phi; // eta
        //if (m_isIBL && barrel == 0 && layer == 0) pixel_phi_on_chip = pixel_phi;
         //  unsigned int pixelType = ModuleSpecialPixelMap::
          // pixelType( pixel_eta_on_chip, pixel_phi_on_chip, mchips );

        switch(pixelType) {
          case 0:
            type = "Normal";
            break;
          case 1:
            type = "Long";
            break;
          case 2:
            type = "Ganged";
            break;
          case 3:
            type = "Long-Ganged";
            break;
          case 5:
            type = "Long-InterGanged";
            break;
          case 6:
            type = "InterGanged";
            break;
          case 8:
          default:
            type = "Invalid";
            break;
        }

        double thiscut = cut;

        if( type == "Ganged" ) thiscut *= m_gangedPixelMultiplier;
        else if( type == "Long" ) thiscut *= m_longPixelMultiplier;
        else if( type == "Long-InterGanged" ) thiscut *= m_longPixelMultiplier;
        else if( type == "Long-Ganged" ) thiscut *= m_longPixelMultiplier * m_gangedPixelMultiplier;

        //if(moduleHash == 0 || moduleHash == 1000) std::cout << "debug point 12: " << moduleHash << std::endl;

        if( type != "Invalid" ){

          double occupancy = static_cast<double>(m_hitMaps[moduleHash]->GetBinContent(pixel_eta+1, pixel_phi+1)) /
            static_cast<double>(m_nEvents);

          if ( occupancy < minOccupancy ) occupancy = minOccupancy;
          globalOccupancy->Fill(log10(occupancy));
          h_occupancy[component]->Fill(log10(occupancy));
          h_occupancy[type]->Fill(log10(occupancy));
          if( occupancy > thiscut ) {
            thisModuleCut++;

            if( m_calculateNoiseMaps ){
              m_noiseMaps[moduleHash]->Fill(pixel_eta, pixel_phi);
              if (m_isIBL && component == "IBL") {
                if (module_eta >= -6 && module_eta <= 5) m_overlayedIBLDCNoiseMap->Fill(pixel_eta, pixel_phi); // Planar
                if (module_eta <= -7 || module_eta >= 6) m_overlayedIBLSCNoiseMap->Fill(pixel_eta, pixel_phi); // 3D
              }
              else m_overlayedPixelNoiseMap->Fill(pixel_eta, pixel_phi);
            }
          } else {
            if ( barrel == 0 ) nhitsNoNoisePlot->Fill(module_eta,module_phi, m_hitMaps[moduleHash]->GetBinContent(pixel_eta+1, pixel_phi+1));
          }
        } // end if ( type != "Invalid" )
      } // end for loop on pixel_phi
    } // end for loop on pixel_eta
    //if(moduleHash == 0 || moduleHash == 1000) std::cout << "debug point 13: " << moduleHash << std::endl;
    if ( thisModuleCut > 0 ) {
      totalDisabledPixels+=thisModuleCut;
      maskedPlot->Fill( static_cast<double>(thisModuleCut) );
      modulesWithDisabledPixels++;
   
      if      (barrel== 0) {
        if (m_isIBL) { // ----- IBL ----- //
          if(layer == 0){
            disablePlotBI->Fill(module_eta,module_phi,thisModuleCut);
          }
          else if(layer == 1) { // CAUTION: layer #1 = BLayer since IBL installed
            disablePlotB0->Fill(module_eta,module_phi,thisModuleCut);
          }
          else if(layer == 2) {
            disablePlotB1->Fill(module_eta,module_phi,thisModuleCut);
          }
          else if(layer == 3) {
            disablePlotB2->Fill(module_eta,module_phi,thisModuleCut);
          }
        }
        else {
          if(layer == 0){
            disablePlotB0->Fill(module_eta,module_phi,thisModuleCut);
          }
          else if(layer == 1) {
            disablePlotB1->Fill(module_eta,module_phi,thisModuleCut);
          }
          else if(layer == 2) {
            disablePlotB2->Fill(module_eta,module_phi,thisModuleCut);
          }
        }
      }
      else if (barrel== 2) { disablePlotEC->Fill(layer+1,module_phi,thisModuleCut); }
      else if (barrel==-2) { disablePlotEC->Fill(-(layer+1),module_phi,thisModuleCut); }

   
   
    }
    //if(moduleHash == 0 || moduleHash == 1000) std::cout << "debug point 14: " << moduleHash << std::endl;
  } // end for loop on moduleHash

  //std::cout << "debug point 999" << std::endl;

  ATH_MSG_INFO( "Modules disabled " << totalDisabledModules );
  ATH_MSG_INFO( "Modules with hits " << modulesWithHits );
  ATH_MSG_INFO( "Modules with disabled pixels " << modulesWithDisabledPixels );
  ATH_MSG_INFO( "Total disabled pixels " << totalDisabledPixels );

  return StatusCode::SUCCESS;
} // end finalize

std::string NoiseMapBuilder::getDCSIDFromPosition (int barrel_ec, int layer, int module_phi, int module_eta){
  for(unsigned int ii = 0; ii < m_pixelMapping.size(); ii++) {
    if (m_pixelMapping[ii].second.size() != 4) {
      std::cout << "getDCSIDFromPosition: Vector size is not 4!" << std::endl;
      return std::string("Error!");
    }
    if (m_pixelMapping[ii].second[0] != barrel_ec) continue;
    if (m_pixelMapping[ii].second[1] != layer) continue;
    if (m_pixelMapping[ii].second[2] != module_phi) continue;
    if (m_pixelMapping[ii].second[3] != module_eta) continue;
    return m_pixelMapping[ii].first;
  }
  std::cout << "Not found!" << std::endl;
  return std::string("Error!");
}
