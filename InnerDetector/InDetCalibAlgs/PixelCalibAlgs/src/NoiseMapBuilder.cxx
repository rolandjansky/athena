/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PixelCalibAlgs
#include "PixelCalibAlgs/NoiseMapBuilder.h"
#include "PixelCalibAlgs/PixelConvert.h"

// PixelConditions
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
#include "PixelConditionsServices/IPixelByteStreamErrorsSvc.h"
#include "PixelConditionsData/SpecialPixelMap.h"

// Gaudi
#include "GaudiKernel/ITHistSvc.h"

// EDM
#include "InDetRawData/PixelRDO_Container.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

// geometry
#include "InDetIdentifier/PixelID.h"

// ROOT
#include "TH2.h"

// standard library
#include<string>
#include<sstream>
#include<algorithm>



NoiseMapBuilder::NoiseMapBuilder(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_tHistSvc("THistSvc", name),
  m_pixelConditionsSummarySvc("PixelConditionsSummarySvc", name),
  m_BSErrorsSvc("PixelByteStreamErrorsSvc",name),
  m_pixelRDOKey("PixelRDOs"),
  m_nEvents(0.),
  m_pixelID(0),
  m_disk1ACut(1.e-5),
  m_disk2ACut(1.e-5),
  m_disk3ACut(1.e-5),
  m_disk1CCut(1.e-5),
  m_disk2CCut(1.e-5),
  m_disk3CCut(1.e-5),
  m_bLayerCut(1.e-5),
  m_layer1Cut(1.e-5),
  m_layer2Cut(1.e-5),
  m_longPixelMultiplier(1.),
  m_gangedPixelMultiplier(1.),
  m_occupancyPerBC(true),
  m_nBCReadout(2),
  m_lbMin(0),
  m_lbMax(-1),
  m_calculateNoiseMaps(false),
  m_nEventsHist(0),
  m_disabledModules(0)
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
}


NoiseMapBuilder::~NoiseMapBuilder(){}



StatusCode NoiseMapBuilder::initialize(){
  ATH_MSG_INFO( "Initializing NoiseMapBuilder" );
  
  StatusCode sc = detStore()->retrieve( m_pixelID, "PixelID" );
  if( !sc.isSuccess() ){
    ATH_MSG_FATAL( "Unable to retrieve pixel ID helper" );
    return StatusCode::FAILURE;
  }

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
  
  if( m_calculateNoiseMaps ){
    m_noiseMaps.resize(m_pixelID->wafer_hash_max());
  }

  m_nEventsHist = new TH1D("NEvents", "NEvents", 1, 0, 1);
  m_tHistSvc->regHist("/histfile/NEvents", m_nEventsHist).setChecked();

  
  // endcap A
    
  for(int layer = 0; layer < 3; layer++){
    for(int module_phi = 0; module_phi < 48; module_phi++){

      Identifier moduleID = m_pixelID->wafer_id(2, layer, module_phi, 0);
      IdentifierHash moduleHash = m_pixelID->wafer_hash(moduleID);
      
      unsigned int hashID = ( ((m_pixelID->barrel_ec(moduleID) + 2) / 2) << 25 ) +
	( m_pixelID->layer_disk(moduleID) << 23) +
	( m_pixelID->phi_module(moduleID) << 17) +
	( (m_pixelID->eta_module(moduleID) + 6) << 13);

      std::string onlineID = PixelConvert::DCSID(PixelConvert::OnlineID(hashID));

      std::ostringstream names;
      
      m_hitMaps[moduleHash] = new TH2D(onlineID.c_str(), onlineID.c_str(), 144, -0., 144., 328, 0., 328.);
      
      names << "/histfile/hitMaps_endcapA/Disk" << (layer + 1) << "/" << onlineID;
      m_tHistSvc->regHist(names.str().c_str(), m_hitMaps[moduleHash]).setChecked();
      names.str(""); names.clear();

      m_LBdependence[moduleHash] = new TH1D(onlineID.c_str(), onlineID.c_str(), 2001, -0.5, 2000.5);
      
      names << "/histfile/LBdep_endcapA/Disk" << (layer + 1) << "/" << onlineID;
      m_tHistSvc->regHist(names.str().c_str(), m_LBdependence[moduleHash]).setChecked();
      names.str(""); names.clear();

      if( m_calculateNoiseMaps ){
	m_noiseMaps[moduleHash] = new TH2C(onlineID.c_str(), onlineID.c_str(), 144, -0., 144., 328, 0., 328.);
	
	names << "/histfile/noiseMaps_endcapA/Disk" << (layer + 1) << "/" << onlineID;
	m_tHistSvc->regHist(names.str().c_str(), m_noiseMaps[moduleHash]).setChecked();
	names.str(""); names.clear();
      }
    }
  }


  // endcap C

  for(int layer = 0; layer < 3; layer++){
    for(int module_phi = 0; module_phi < 48; module_phi++){

      Identifier moduleID = m_pixelID->wafer_id(-2, layer, module_phi, 0);
      IdentifierHash moduleHash = m_pixelID->wafer_hash(moduleID);

      unsigned int hashID = ( ((m_pixelID->barrel_ec(moduleID) + 2) / 2) << 25 ) +
	( m_pixelID->layer_disk(moduleID) << 23) +
	( m_pixelID->phi_module(moduleID) << 17) +
	( (m_pixelID->eta_module(moduleID) + 6) << 13);

      std::string onlineID = PixelConvert::DCSID(PixelConvert::OnlineID(hashID));

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


  // barrel

  for(int layer = 0; layer < 3; layer++){
    
    int module_phi_max = 0;
    if(layer == 0) module_phi_max = 22;
    if(layer == 1) module_phi_max = 38;
    if(layer == 2) module_phi_max = 52;
    
    for(int module_phi = 0; module_phi < module_phi_max; module_phi++){
      for(int module_eta = -6; module_eta < 7; module_eta++){

	Identifier moduleID = m_pixelID->wafer_id(0, layer, module_phi, module_eta);
	IdentifierHash moduleHash = m_pixelID->wafer_hash(moduleID);
	
	unsigned int hashID = ( ((m_pixelID->barrel_ec(moduleID) + 2) / 2) << 25 ) +
	  ( m_pixelID->layer_disk(moduleID) << 23) +
	  ( m_pixelID->phi_module(moduleID) << 17) +
	  ( (m_pixelID->eta_module(moduleID) + 6) << 13);
	
	std::string onlineID = PixelConvert::DCSID(PixelConvert::OnlineID(hashID));

	std::ostringstream names;
      
	m_hitMaps[moduleHash] = new TH2D(onlineID.c_str(), onlineID.c_str(), 144, -0., 144., 328, 0., 328.); 
	
	if(layer == 0) names << "/histfile/hitMaps_barrel/B-layer/" << onlineID;
	else names << "/histfile/hitMaps_barrel/Layer" << layer << "/" << onlineID;
	m_tHistSvc->regHist(names.str().c_str(), m_hitMaps[moduleHash]).setChecked();
	names.str(""); names.clear();

	m_LBdependence[moduleHash] = new TH1D(onlineID.c_str(), onlineID.c_str(), 2001, -0.5, 2000.5);
      
	if(layer == 0) names << "/histfile/LBdep_barrel/B-layer/" << onlineID;
	else names << "/histfile/LBdep_barrel/Layer" << layer << "/" << onlineID;
	m_tHistSvc->regHist(names.str().c_str(), m_LBdependence[moduleHash]).setChecked();
	names.str(""); names.clear();

	if( m_calculateNoiseMaps ){
	  m_noiseMaps[moduleHash] = new TH2C(onlineID.c_str(), onlineID.c_str(), 144, -0., 144., 328, 0., 328.);
	  
	  if(layer == 0) names << "/histfile/noiseMaps_barrel/B-layer/" << onlineID;
	  else names << "/histfile/noiseMaps_barrel/Layer" << layer << "/" << onlineID;
	  m_tHistSvc->regHist(names.str().c_str(), m_noiseMaps[moduleHash]).setChecked();
	  names.str(""); names.clear();
	}
      }
    }
  }

  m_disabledModules = new TH1D("DisabledModules", "Number of events disabled vs. IdentifierHash", 1744, 0, 1744);
  m_tHistSvc->regHist("/histfile/DisabledModules", m_disabledModules).setChecked();

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
      if  ( !(m_pixelConditionsSummarySvc->isGood(moduleHash)) ) continue;// exclude bad modules
      int errors = m_BSErrorsSvc->getModuleErrors(moduleHash);
      if ( ( errors & 0x0001C000 ) ) continue;                            // exclude FE synch errors
      for(DataVector<PixelRDORawData>::const_iterator rdo = PixelRDOCollection->begin(); 
	  rdo!=PixelRDOCollection->end(); ++rdo){
	
	Identifier rdoID = (*rdo)->identify();
	unsigned int pixel_eta = m_pixelID->eta_index(rdoID);
	unsigned int pixel_phi = m_pixelID->phi_index(rdoID);
      
	m_hitMaps[moduleHash]->Fill(pixel_eta, pixel_phi);
        m_LBdependence[moduleHash]->Fill(LB);
      }
    }  
  }

  
  for(unsigned int moduleHash=0; moduleHash<m_pixelID->wafer_hash_max(); moduleHash++) {
    if( !m_pixelConditionsSummarySvc->isActive( moduleHash ) ){
      m_disabledModules->Fill( moduleHash );
    }
  }

  m_nEvents++;
  m_nEventsHist->Fill(.5);
  
  return StatusCode::SUCCESS;
}


StatusCode NoiseMapBuilder::finalize(){ 
  
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
  components.push_back("BLayer");
  components.push_back("Layer1");
  components.push_back("Layer2");
  
  std::vector<std::string> types;
  types.push_back("Normal");
  types.push_back("Ganged");
  types.push_back("InterGanged");
  types.push_back("Long");

  std::map<std::string, TH1D*> h_occupancy;

  for( std::vector<std::string>::const_iterator component = components.begin();
       component != components.end(); ++component){
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


  TH1D* maskedPlot= new TH1D("maskedPlot","Disabled pixel per module",50,0.5,50.5);
  m_tHistSvc->regHist("/histfile/maskedPlot",maskedPlot).setChecked();

  TH2F* disablePlotB0=new TH2F("disablePlotB0","Disabled pixels B0",13,-6.5,6.5,22,-0.5,21.5);
  m_tHistSvc->regHist("/histfile/disablePlotB0",disablePlotB0).setChecked();

  TH2F* disablePlotB1=new TH2F("disablePlotB1","Disabled pixels B1",13,-6.5,6.5,38,-0.5,37.5);
  m_tHistSvc->regHist("/histfile/disablePlotB1",disablePlotB1).setChecked();

  TH2F* disablePlotB2=new TH2F("disablePlotB2","Disabled pixels B2",13,-6.5,6.5,52,-0.5,51.5);
  m_tHistSvc->regHist("/histfile/disablePlotB2",disablePlotB2).setChecked();

  TH2F* disablePlotEC=new TH2F("disablePlotEC","Disabled pixels Endcap",7,-3.5,3.5,48,-0.5,47.5);
  m_tHistSvc->regHist("/histfile/disablePlotEC",disablePlotEC).setChecked();

  int totalDisabledPixels=0, 
    totalDisabledModules=0, 
    modulesWithHits=0,
    modulesWithDisabledPixels=0;

  for(unsigned int moduleHash=0; moduleHash<m_pixelID->wafer_hash_max(); moduleHash++) {

    Identifier moduleID=m_pixelID->wafer_id(IdentifierHash(moduleHash));
    int barrel     = m_pixelID->barrel_ec(moduleID);
    int layer      = m_pixelID->layer_disk(moduleID);
    int module_phi = m_pixelID->phi_module(moduleID);
    int module_eta = m_pixelID->eta_module(moduleID);
    int phi_max    = m_pixelID->phi_index_max(moduleID);
    int eta_max    = m_pixelID->eta_index_max(moduleID);

    TH2F* disablePlot = 0;
    std::string component;
    double cut = 0.;
    
    if ( barrel!=0 ) {
      
      disablePlot = disablePlotEC;
      
      if (barrel==-2) {
	if(layer == 0){cut = m_disk1ACut; component = "Disk1A";}
	else if(layer == 1){cut = m_disk2ACut; component = "Disk2A";}
	else if(layer == 2){cut = m_disk3ACut; component = "Disk3A";}
      } else {
	if(layer == 0){cut = m_disk1CCut; component = "Disk1C";}
	else if(layer == 1){cut = m_disk2CCut; component = "Disk2C";}
	else if(layer == 2){cut = m_disk3CCut; component = "Disk3C";}
      }
    } else {
      if(layer == 0){
	cut = m_bLayerCut; 
	disablePlot = disablePlotB0;
	component = "BLayer";
      }
      else if(layer == 1) {
	cut = m_layer1Cut; 
	disablePlot = disablePlotB1;
	component = "Layer1";
      }
      else if(layer == 2) {
	cut = m_layer2Cut; 
	disablePlot = disablePlotB2;
	component = "Layer2";
      }
      else continue;
    }    

    if( m_BSErrorsSvc->getReadEvents(moduleHash)==0 && m_hitMaps[moduleHash]->GetEntries()==0 ){

      unsigned int hashID = ( ((m_pixelID->barrel_ec(moduleID) + 2) / 2) << 25 ) +
	( m_pixelID->layer_disk(moduleID) << 23) +
	( m_pixelID->phi_module(moduleID) << 17) +
	( (m_pixelID->eta_module(moduleID) + 6) << 13);

      ATH_MSG_INFO( "Disabled module " 
		    << PixelConvert::OnlineID(hashID) << "\t" 
		    << PixelConvert::DCSID(PixelConvert::OnlineID(hashID)) );
      
      if ( barrel==0) disablePlot->Fill(module_eta,module_phi,-1);
      else if ( barrel>0 ) disablePlot->Fill(layer+1,module_phi,-1);
      else disablePlot->Fill(-(layer+1),module_phi,-1);
      totalDisabledModules++;

      continue;
    }
    else if( m_hitMaps[moduleHash]->GetEntries() != 0 ){
      modulesWithHits++;
    }
    
    int thisModuleCut=0;
    for(int pixel_eta = 0; pixel_eta <= eta_max; pixel_eta++){
      for(int pixel_phi = 0; pixel_phi <= phi_max; pixel_phi++){
	
	std::string type;
	unsigned int pixelType = ModuleSpecialPixelMap::
	  pixelType( pixel_eta % 18, (pixel_phi <= phi_max / 2) ? pixel_phi : phi_max - pixel_phi );

	switch(pixelType) {
	case 0:
	  type = "Normal";
	  break;
	case 1:
	  type = "Long";
	  break;
	case 2:
	case 3:
	  type = "Ganged";
	  break;
	case 5:
	case 6:
	  type = "InterGanged";
	  break;
	case 8:
	default:
	  type = "Invalid";
	  break;
	}

	double thiscut=cut;
	
	if( type == "Ganged" ) thiscut *= m_gangedPixelMultiplier;
	else if( type == "Long" ) thiscut *= m_longPixelMultiplier;
	
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
	    }
	  }
	}
      }
    }
    if ( thisModuleCut>0 ) {
      totalDisabledPixels+=thisModuleCut;
      maskedPlot->Fill( static_cast<double>(thisModuleCut) );
      modulesWithDisabledPixels++;
      if ( barrel==0) disablePlot->Fill(module_eta,module_phi,thisModuleCut);
      else if ( barrel>0 ) disablePlot->Fill(layer+1,module_phi,thisModuleCut);
      else disablePlot->Fill(-(layer+1),module_phi,thisModuleCut);
    }
  }

  ATH_MSG_INFO( "Modules disabled " << totalDisabledModules );
  ATH_MSG_INFO( "Modules with hits " << modulesWithHits );
  ATH_MSG_INFO( "Modules with disabled pixels " << modulesWithDisabledPixels );
  ATH_MSG_INFO( "Total disabled pixels " << totalDisabledPixels );
  
  return StatusCode::SUCCESS;
}
