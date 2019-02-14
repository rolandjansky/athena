/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include "TestLArHardwareID/TestLArHWID_Algo.h"

#include "GaudiKernel/IService.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/IToolSvc.h"

#include "StoreGate/StoreGateSvc.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/LArID_Exception.h"
#include "LArIdentifier/LArIdManager.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArHVLineID.h"
#include "LArIdentifier/LArElectrodeID.h"
#include "LArIdentifier/LArOnlID_Exception.h"

/********************************************************/
TestLArHWID_Algo::TestLArHWID_Algo(const std::string &name , ISvcLocator* pSvcLocator) :
  AthAlgorithm( name , pSvcLocator) ,
  m_Detector("EM") , 
  m_Manager("MGR") ,
  m_HighVoltage("OFF"),
  m_HVlineToOffline("OFF"),
  m_HVelectrodeToOffline("OFF"),
  m_Connected("ALL"),
  m_FeedthroughTest("OFF"),
  m_OnlineTest("OFF"),
  m_OfflineTest("OFF"),
  m_SubDetector("OFF"),
  m_lvl1Helper(0),
  m_emHelper(0),
  m_hecHelper(0),
  m_fcalHelper(0),
  m_onlineHelper(0),
  m_hvHelper(0),
  m_electrodeHelper(0)
{
  declareProperty("Detector", m_Detector ) ; 
  if( m_Detector != "ALL"   &&
      m_Detector != "EM"    && m_Detector != "HEC"  &&
      m_Detector != "FCAL"  && m_Detector != "EMEC" &&
      m_Detector != "EMBPS" && m_Detector != "ECPS" && 
      m_Detector != "EMB"   && m_Detector != "EM1"  &&
      m_Detector != "EM2"   && m_Detector != "EM3"  &&
      m_Detector != "EMBPS" && m_Detector != "EMEC1I" &&
      m_Detector != "EMEC2I"&& m_Detector != "EMEC3O" &&
      m_Detector != "EMEC1O"&& m_Detector != "EMEC2O" &&
      m_Detector != "ECPS"  && 
      m_Detector != "HEC0"  && m_Detector != "HEC1" &&
      m_Detector != "HEC2"  && m_Detector != "HEC3" ) 
    { 
      m_Detector = "EMB" ; 
    }
  
  declareProperty("Helper", m_Manager );

  // Calibration Property :
  // =====================
  declareProperty("Calibration", m_Calibration );
  if( m_Calibration != "ON" && m_Calibration != "OFF" && m_Calibration != "EXCL" ){
    m_Calibration = "OFF";
  } 


  // HighVoltage Property :
  // =====================
  declareProperty("HighVoltage", m_HighVoltage );
  if( m_HighVoltage != "ON" && m_HighVoltage != "OFF" ){
    m_HighVoltage = "OFF";
  } 


  // HVline <-> Offline
  // =====================
  declareProperty("HVlineToOffline", m_HVlineToOffline );
  if( m_HVlineToOffline != "ON" && 
      m_HVlineToOffline != "OFF" ){
    m_HVlineToOffline = "OFF";
  } 

  // Electrode <-> Offline
  // =====================
  declareProperty("HVelectrodeToOffline", m_HVelectrodeToOffline );
  if( m_HVelectrodeToOffline != "ON" && 
      m_HVelectrodeToOffline != "OFF" ){
    m_HVelectrodeToOffline = "OFF";
  } 


  // OnlineTest Property:
  // =======================
  declareProperty("OnlineTest", m_OnlineTest );
  if( m_OnlineTest != "ON" 
      && m_OnlineTest != "OFF" 
      && m_OnlineTest != "DISC" 
      && m_OnlineTest != "FEB" 
      )
    {
      m_OnlineTest = "OFF";
    }


 
  // OfflineTest Property:
  // =====================
  declareProperty("OfflineTest", m_OfflineTest );
  if( m_OfflineTest != "ON" && m_OfflineTest != "OFF" )
    {
      m_OfflineTest = "OFF";
    }

  // Connected Property:
  // =====================
  declareProperty("Connected", m_Connected );
  if( m_Connected != "ON" 
      && m_Connected != "OFF" )
    {
      m_Connected = "ALL";
    }

  // Feedthrough Tests
  // ===================
  declareProperty("FeedthroughTest", m_FeedthroughTest ) ; 
  if( m_FeedthroughTest != "ON" && m_FeedthroughTest != "OFF" )
    {
      m_FeedthroughTest = "OFF";
    }

  // SubSystem chosen 
  // ======================
  declareProperty("SubDetector", m_SubDetector );
  if( m_SubDetector != "S1" &&
      m_SubDetector != "S2" &&
      m_SubDetector != "S3" &&
      m_SubDetector != "S0" &&
      m_SubDetector != "OFF"  )
    {
      m_SubDetector = "ALL";
    }
      
}

TestLArHWID_Algo::~TestLArHWID_Algo(){
}

// ==============================================================
StatusCode TestLArHWID_Algo::initialize(){
// ==============================================================
  ATH_MSG_INFO ( " initializing " );

  ATH_CHECK(m_cablingReadKey.initialize());

  ATH_CHECK(m_HVReadKey.initialize());

  // Calo
  if(m_Manager == "DIRECT") {
    ATH_CHECK( detStore()->retrieve(m_lvl1Helper, "CaloLVL1_ID") );
    ATH_MSG_DEBUG ( " Found the CaloLVL1_ID helper. " );

    ATH_CHECK( detStore()->retrieve(m_emHelper, "LArEM_ID") );
    ATH_MSG_DEBUG ( " Found the LArEM_ID helper. " );

    ATH_CHECK( detStore()->retrieve(m_hecHelper, "LArHEC_ID") );
    ATH_MSG_DEBUG ( " Found the LArHEC_ID helper. " );

    ATH_CHECK( detStore()->retrieve(m_fcalHelper, "LArFCAL_ID") );
    ATH_MSG_DEBUG ( " Found the LArFCAL_ID helper. " );
  }
  else {
    // via Mgr
    const CaloIdManager*	caloIdMgr = nullptr;
    ATH_CHECK( detStore()->retrieve(caloIdMgr) );
    ATH_MSG_INFO ( "Successfully retrieved CaloIdManager from DetectorStore" );

    const CaloLVL1_ID* caloId = caloIdMgr->getLVL1_ID();
    if (!caloId) {
      ATH_MSG_ERROR ( "Could not access calolvl1 ID helper" );
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO ( "Successfully accessed calolvl1 ID helper" );
      m_lvl1Helper = caloId ;
    }

    const LArEM_ID* emId = caloIdMgr->getEM_ID();
    if (!emId) {
      ATH_MSG_ERROR ( "Could not access lar EM ID helper" );
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO ( "Successfully accessed lar EM ID helper" );
      m_emHelper=emId;
    }
    
    const LArHEC_ID* hecId = caloIdMgr->getHEC_ID();
    if (!hecId) {
      ATH_MSG_ERROR ( "Could not access lar HEC ID helper" );
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO ( "Successfully accessed lar HEC ID helper" );
      m_hecHelper=hecId;
    }
    
    const LArFCAL_ID* fcalId = caloIdMgr->getFCAL_ID();
    if (!fcalId) {
      ATH_MSG_ERROR ( "Could not access lar FCAL ID helper" );
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO ( "Successfully accessed lar FCAL ID helper" );
      m_fcalHelper=fcalId;
    }
  }

  // LAr 
  if(m_Manager == "DIRECT") {
    ATH_CHECK( detStore()->retrieve(m_onlineHelper, "LArOnlineID") );
    ATH_MSG_DEBUG ( " Found the LArOnlineID helper. " );
  }
  else {
    // via Mgr
    const LArIdManager*	larIdMgr = nullptr;
    ATH_CHECK(  detStore()->retrieve(larIdMgr) );
    ATH_MSG_INFO ( "Successfully retrieved LArIdManager from DetectorStore" );

    const LArOnlineID* onlineId = larIdMgr->getOnlineID();
    if (!onlineId) {
      ATH_MSG_ERROR ( "Could not access lar ONLINE ID helper" );
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO ( "Successfully accessed lar ONLINE ID helper" );
      m_onlineHelper=onlineId;
    }

  }

  // =============================
  // LArHVLine  
  // =============================
  if(m_Manager == "DIRECT") {
    ATH_CHECK( detStore()->retrieve(m_hvHelper, "LArHVLineID") );
    ATH_MSG_DEBUG ( " Found the LArHVLineID helper. " );
  }
  else {
    // via Mgr
    const LArIdManager*	larIdMgr = nullptr;
    ATH_CHECK( detStore()->retrieve(larIdMgr) );
    ATH_MSG_INFO ( "Successfully retrieved LArIdManager from DetectorStore" );

    const LArHVLineID* hvId = larIdMgr->getHVLineID();
    if (!hvId) {
      ATH_MSG_ERROR ( "Could not access LArHVLineID helper" );
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO ( "Successfully accessed LArHVLineID helper" );
      m_hvHelper=hvId;
    }
  }
  // =============================
  // LArElectrode
  // =============================
  if(m_Manager == "DIRECT") {
    ATH_CHECK( detStore()->retrieve(m_electrodeHelper, "LArElectrodeID") );
    ATH_MSG_DEBUG ( " Found the LArElectrodeID helper. " );
  }
  else {
    // via Mgr
    const LArIdManager*	larIdMgr = nullptr;
    ATH_CHECK( detStore()->retrieve(larIdMgr) );
    ATH_MSG_INFO ( "Successfully retrieved LArIdManager from DetectorStore" );
    const LArElectrodeID* elecId = larIdMgr->getLArElectrodeID();
    if (!elecId) {
      ATH_MSG_ERROR ( "Could not access LArElectrodeID helper" );
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO ( "Successfully accessed LArElectrodeID helper" );
      m_electrodeHelper=elecId;
    }
  }

  return StatusCode::SUCCESS;
}


// ====================================================================================
StatusCode TestLArHWID_Algo::execute(){
// ====================================================================================

  ATH_MSG_INFO ( "=> TestLArHWID_Algo::Executing " );

  // retrieve the cabling
  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingReadKey};
  const LArOnOffIdMapping* cabling{*cablingHdl};
  // retrieve the HV mapping
  SG::ReadCondHandle<LArHVIdMapping> hvHdl{m_HVReadKey};
  const LArHVIdMapping* hvmap{*hvHdl};

  // Output files for all Tests
  //---------------------------
  std::fstream fileError;
  std::fstream fileCalib;

  std::fstream fileFcal;
  std::fstream fileHec;
  std::fstream fileEmec;
  std::fstream fileEmb;

  std::fstream fileFcalOn;
  std::fstream fileHecOn;
  std::fstream fileEmecOn;
  std::fstream fileEmbOn;

  std::fstream fileFcalOff;
  std::fstream fileHecOff;
  std::fstream fileEmecOff;
  std::fstream fileEmbOff;

  std::fstream fileAll;
  std::fstream fileTest;
  std::fstream offlineH8;
  std::fstream offlineH6;
  std::fstream HvCell;
  std::fstream fileTTFCAL;
  std::fstream fileTTEMB;
  std::fstream fileTTEMBE;
  std::fstream fileTTEMEC;
  std::fstream fileTTHEC;

  std::fstream fileHVEMB;
  std::fstream fileHVEMBPS;
  std::fstream fileHVEMEC;
  std::fstream fileHVEMECPS;
  std::fstream fileHVHEC;
  std::fstream fileHVFCAL;

  std::fstream fileHVEMBOFF;
  std::fstream fileHVEMECOFF;
  std::fstream fileHVHECOFF;
  std::fstream fileHVFCALOFF;

  // ===================================================================
  //                    Setup for Output file Names (AL/DEC-2004)
  // ===================================================================
  // Output files
  std::string l_version = m_emHelper->dictionaryVersion();
  std::string Connected = "ALL";
  ATH_MSG_INFO ( " Dictionary Version      = " << l_version );
  ATH_MSG_INFO ( " [INI] m_Detector        = " << m_Detector );
  ATH_MSG_INFO ( " [INI] m_SubDetector     = " << m_SubDetector );
  ATH_MSG_INFO ( " [INI] m_HighVoltage     = " << m_HighVoltage );
  ATH_MSG_INFO ( " [INI] m_HVlineToOffline = " << m_HVlineToOffline );
  ATH_MSG_INFO ( " [INI] m_ElectrodeOffline= " << m_HVelectrodeToOffline );
  ATH_MSG_INFO ( " [INI] m_OnlineTest      = " << m_OnlineTest );
  ATH_MSG_INFO ( " [INI] m_OfflineTest     = " << m_OfflineTest );
  ATH_MSG_INFO ( " [INI] m_Connected       = " << Connected  );

  if(m_OnlineTest != "OFF" || m_OfflineTest != "OFF" || m_HighVoltage != "OFF" )
    {// Online or Offline Test ON 
      if(l_version == "H8TestBeam"){
	ATH_MSG_INFO ( " ========================= " );
	ATH_MSG_INFO ( "  H8 Test Beam Dictionary  " );
	ATH_MSG_INFO ( " ========================= " );
	fileError.open("H8_ERROR.out",std::ios::out);
	if( m_OnlineTest != "OFF" ){
	  fileAll.open("H8_ONLINE.out",std::ios::out);
	  ATH_MSG_INFO ( " --> Online Ids in H8_ONLINE.out " );
	}
	if( m_OfflineTest == "ON" ){
	  offlineH8.open("H8_OFFLINE.out",std::ios::out);
	  ATH_MSG_INFO ( " --> Offline Ids in H8_OFFLINE.out " );
	}
	ATH_MSG_INFO ( " --> error message in H8_ERROR.out " );
      }
      else if(l_version == "H6TestBeam"){
	fileError.open("H6_ERROR.out",std::ios::out);
	fileAll.open("H6_ONLINE.out",std::ios::out);
	fileFcal.open("H6_FCAL.out",std::ios::out);
	fileHec.open("H6_HEC.out",std::ios::out);
	fileEmec.open("H6_EMEC.out",std::ios::out);
	ATH_MSG_INFO ( " ======================== " );
	ATH_MSG_INFO ( " H6 Test Beam Dictionary  " );
	ATH_MSG_INFO ( " ======================== " );
	ATH_MSG_INFO ( " --> error message in H6_ERROR.out " );
	ATH_MSG_INFO ( " --> subdetector messages in H6_subdet.out " );
	ATH_MSG_INFO ( " --> Online/offline Ids in H6_ONLINE.out " );
      }
      else if(l_version == "fullAtlas"){
	// if version == fullAtlas
	if( m_Detector == "ALL" ){ 
	  fileAll.open("ATLAS_ONLINE.out",std::ios::out);
	  fileError.open("ATLAS_ERROR.out",std::ios::out);
	}
	if(m_Detector == "EMB" || m_Detector == "ALL" ){
	  if( m_HighVoltage == "ON" ){
	    fileHVEMB.open("ATLAS_HIGHVOLTAGE_EMB.out",std::ios::out);
	    if( m_OfflineTest == "ON" ){
	      fileHVEMBOFF.open("ATLAS_OFFLINE_TO_HV_EMB.out",std::ios::out);
	    }
	  }
	  if( m_OnlineTest != "OFF"){
	    fileEmbOn.open("ATLAS_EMB_ONLINE.out",std::ios::out);
	  }
	  if( m_OfflineTest == "ON"){
	    fileEmbOff.open("ATLAS_EMB_OFFLINE.out",std::ios::out);
	  }
	  fileError.open("ATLAS_ERROR.out",std::ios::out);
	}

	if( m_Detector == "EMEC" || m_Detector == "ALL" ){
	  if( m_HighVoltage == "ON" ){
	    fileHVEMEC.open("ATLAS_HIGHVOLTAGE_EMEC.out",std::ios::out);
	    if( m_OfflineTest == "ON" ){
	      fileHVEMECOFF.open("ATLAS_OFFLINE_TO_HV_EMEC.out",std::ios::out);
	    }
	  }
	  if( m_OnlineTest != "OFF"){
	    fileEmecOn.open("ATLAS_EMEC_ONLINE.out",std::ios::out);
	  }
	  if( m_OfflineTest == "ON"){
	    fileEmecOff.open("ATLAS_EMEC_OFFLINE.out",std::ios::out);
	  }
	  fileError.open("ATLAS_ERROR.out",std::ios::out);
	}

	if( m_Detector == "HEC" || m_Detector == "ALL" ){
	  if( m_HighVoltage == "ON" ){
	    fileHVHEC.open("ATLAS_HIGHVOLTAGE_HEC.out",std::ios::out);
	    if( m_OfflineTest == "ON" ){
	      fileHVHECOFF.open("ATLAS_OFFLINE_TO_HV_HEC.out",std::ios::out);
	    }
	  }
	  if( m_OnlineTest != "OFF"){
	    fileHecOn.open("ATLAS_HEC_ONLINE.out",std::ios::out);
	  }
	  if( m_OfflineTest == "ON"){
	    fileHecOff.open("ATLAS_HEC_OFFLINE.out",std::ios::out);
	  }

	  fileError.open("ATLAS_ERROR.out",std::ios::out);
	}

	if( m_Detector == "FCAL" || m_Detector == "ALL" ){
	  if( m_HighVoltage == "ON" ){
	    fileHVFCAL.open("ATLAS_HIGHVOLTAGE_FCAL.out",std::ios::out);
	    if( m_OfflineTest == "ON" ){
	      fileHVFCALOFF.open("ATLAS_OFFLINE_TO_HV_FCAL.out",std::ios::out);
	    }
	  }
	  if( m_OnlineTest != "OFF"){
	    fileFcalOn.open("ATLAS_FCAL_ONLINE.out",std::ios::out);
	  }
	  if( m_OfflineTest == "ON"){
	    fileFcalOff.open("ATLAS_FCAL_OFFLINE.out",std::ios::out);
	  }
	  fileError.open("ATLAS_ERROR.out",std::ios::out);
	}
	if( m_Calibration == "ON" ){
	  fileCalib.open("ATLAS_CALIB.out",std::ios::out);
	}
	ATH_MSG_INFO ( " ============================= " );
	ATH_MSG_INFO ( "      ATLAS   Dictionary  " );
	ATH_MSG_INFO ( " ============================= " );
      }
    }
  
  // =====================================================================
  // =====================================================================
  // =====================================================================
  // =====================================================================
  // =====================================================================
  //              Test of New High Voltage Interface (AL/DEC-2006)
  // =====================================================================
  // =====================================================================
  // =====================================================================
  // =====================================================================
  // =====================================================================
  

  
  if( m_HighVoltage == "ON" ){
    // if HighVoltage = ON
    ATH_MSG_INFO ( "   " );
    ATH_MSG_INFO ( "===================================================  " );
    ATH_MSG_INFO ( "  >>> Tests for High Voltage Identifiers <<<   " );
    ATH_MSG_INFO ( "===================================================  " );
    

    
    //=========================================
    //=========================================
    //       Tests of HVLINEID
    //=========================================
    //=========================================
    
    /*
    
    if( m_HVlineToOffline == "ON" && m_HVelectrodeToOffline == "ON"){
      // if m_HVlineToOffline 
      std::vector<HWIdentifier>::const_iterator itFT = m_hvHelper->hvline_begin();
      std::vector<HWIdentifier>::const_iterator itFTEnd = m_hvHelper->hvline_end();    
      int nEMB   = 0;
      int nEMBA   = 0;
      int nEMBC   = 0;
      int nEMBPS = 0;
      
      int nEMEC  = 0;
      int nEMECA = 0;
      int nEMECC = 0;
      
      int nEMECPS= 0;
      int nFCAL  = 0;
      int nFCALA  = 0;
      int nFCALC  = 0;
      
      int nHEC   =0;
      int nHECA   =0;
      int nHECC   =0;
      
      int nEL_EMB = 0;
      int nEL_EMEC= 0;
      int nEL_FCAL= 0;
      int nEL_HEC = 0;
      
      int n_hvline=0;
      int n_hvlineold=0;
      bool error = false;
      
      for(; itFT!=itFTEnd;++itFT)
	{// Start loop over hvlines
	  HWIdentifier hvId = *itFT;      
	  n_hvline++;
	  if(n_hvline==n_hvlineold+400){
	    log << MSG::INFO << "[HVLINE] processing HV line ..# " 
		<< n_hvline << endmsg;
	    n_hvlineold=n_hvline;
	  }
	  if( m_Detector == "EMB" || m_Detector == "ALL" ){
	    if( m_hvHelper->isEMB( hvId ) ){ 
	      if( m_hvHelper->isEMBA( hvId ) ){ nEMBA++;}
	      if( m_hvHelper->isEMBC( hvId ) ){ nEMBC++;}
	      nEMB++;
	      std::vector<HWIdentifier> electrodeIdVec = 
		hvmap->getLArElectrodeIDvec( hvId );
	      // HV line fields
	      int PART= m_hvHelper->partition( hvId );
	      int CANL= m_hvHelper->can_line( hvId );
	      int CANN= m_hvHelper->can_node( hvId );
	      int LINE= m_hvHelper->hv_line( hvId );
	      // Loop over vector of electrodes 
	      std::vector<HWIdentifier>::const_iterator hv = electrodeIdVec.begin();
	      std::vector<HWIdentifier>::const_iterator hvEnd = electrodeIdVec.end();
	      for(; hv!=hvEnd;++hv){
		nEL_EMB++;
		HWIdentifier electrodeId = *hv;
		int DET = m_electrodeHelper->detector( electrodeId);
		int SID = m_electrodeHelper->zside( electrodeId);
		int MOD = m_electrodeHelper->module( electrodeId);
		int PHI = m_electrodeHelper->hv_phi( electrodeId);
		int ETA = m_electrodeHelper->hv_eta( electrodeId);
		int GAP = m_electrodeHelper->gap( electrodeId);
		int ELE = m_electrodeHelper->electrode( electrodeId);
		fileHVEMB << "HVlineID=["
			  << PART << "." 
			  << CANL << "." 
			  << CANN << "." 
			  << LINE << "]" 
			  << " electrodeID=[" 
			  << DET << "." 
			  << SID << "." 
			  << MOD << "." 
			  << PHI << "." 
			  << ETA << "." 
			  << GAP << "." 
			  << ELE << "]" 
			  << std::endl;
	      }
	    }
	  }
	  if( m_Detector == "EMEC" || m_Detector == "ALL" ){
	    if( m_hvHelper->isEMEC( hvId ) ){ 
	      nEMEC++;
	      if( m_hvHelper->isEMECA( hvId ) ){ nEMECA++;}
	      if( m_hvHelper->isEMECC( hvId ) ){ nEMECC++;}
	      std::vector<HWIdentifier> electrodeIdVec = 
		hvmap->getLArElectrodeIDvec( hvId );
	      // HV line fields
	      int PART= m_hvHelper->partition( hvId );
	      int CANL= m_hvHelper->can_line( hvId );
	      int CANN= m_hvHelper->can_node( hvId );
	      int LINE= m_hvHelper->hv_line( hvId );
	      // Loop over vector of electrodes 
	      std::vector<HWIdentifier>::const_iterator hv = electrodeIdVec.begin();
	      std::vector<HWIdentifier>::const_iterator hvEnd = electrodeIdVec.end();
	      for(; hv!=hvEnd;++hv){
		nEL_EMEC++;
		HWIdentifier electrodeId = *hv;
		int DET = m_electrodeHelper->detector( electrodeId);
		int SID = m_electrodeHelper->zside( electrodeId);
		int MOD = m_electrodeHelper->module( electrodeId);
		int PHI = m_electrodeHelper->hv_phi( electrodeId);
		int ETA = m_electrodeHelper->hv_eta( electrodeId);
		int GAP = m_electrodeHelper->gap( electrodeId);
		int ELE = m_electrodeHelper->electrode( electrodeId);
		fileHVEMEC << "HVlineID=["
			   << PART << "." 
			   << CANL << "." 
			   << CANN << "." 
			   << LINE << "]" 
			   << " electrodeID=[" 
			   << DET << "." 
			   << SID << "." 
			   << MOD << "." 
			   << PHI << "." 
			   << ETA << "." 
			   << GAP << "." 
			   << ELE << "]" 
			   << std::endl;
	      }	  
	    }
	  }
	  
	  if( m_Detector == "FCAL" || m_Detector == "ALL" ){
	    if( m_hvHelper->isFCAL( hvId ) ){ 
	      nFCAL++;
	      if( m_hvHelper->isFCALA( hvId ) ){ nFCALA++;}
	      if( m_hvHelper->isFCALC( hvId ) ){ nFCALC++;}
	      std::vector<HWIdentifier> electrodeIdVec = 
		hvmap->getLArElectrodeIDvec( hvId );
	      // HV line fields
	      int PART= m_hvHelper->partition( hvId );
	      int CANL= m_hvHelper->can_line( hvId );
	      int CANN= m_hvHelper->can_node( hvId );
	      int LINE= m_hvHelper->hv_line( hvId );
	      // Loop over vector of electrodes 
	      std::vector<HWIdentifier>::const_iterator hv = electrodeIdVec.begin();
	      std::vector<HWIdentifier>::const_iterator hvEnd = electrodeIdVec.end();
	      for(; hv!=hvEnd;++hv){
		nEL_FCAL++;
		HWIdentifier electrodeId = *hv;
		int DET = m_electrodeHelper->detector( electrodeId);
		int SID = m_electrodeHelper->zside( electrodeId);
		int MOD = m_electrodeHelper->module( electrodeId);
		int PHI = m_electrodeHelper->hv_phi( electrodeId);
		int ETA = m_electrodeHelper->hv_eta( electrodeId);
		int GAP = m_electrodeHelper->gap( electrodeId);
		int ELE = m_electrodeHelper->electrode( electrodeId);
		fileHVFCAL << "HVlineID=["
			   << PART << "." 
			   << CANL << "." 
			   << CANN << "." 
			   << LINE << "]" 
			   << " electrodeID=[" 
			   << DET << "." 
			   << SID << "." 
			   << MOD << "." 
			   << PHI << "." 
			   << ETA << "." 
			   << GAP << "." 
			   << ELE << "]" 
			   << std::endl;
	      }	  
	    }
	  }
	  
	  if( m_Detector == "HEC" || m_Detector == "ALL" ){
	    if( m_hvHelper->isHEC( hvId ) ){ 
	      nHEC++;
	      if( m_hvHelper->isHECA( hvId ) ){ nHECA++;}
	      if( m_hvHelper->isHECC( hvId ) ){ nHECC++;}
	      std::vector<HWIdentifier> electrodeIdVec = 
		hvmap->getLArElectrodeIDvec( hvId );
	      // HV line fields
	      int PART= m_hvHelper->partition( hvId );
	      int CANL= m_hvHelper->can_line( hvId );
	      int CANN= m_hvHelper->can_node( hvId );
	      int LINE= m_hvHelper->hv_line( hvId );
	      // Loop over vector of electrodes 
	      std::vector<HWIdentifier>::const_iterator hv = electrodeIdVec.begin();
	      std::vector<HWIdentifier>::const_iterator hvEnd = electrodeIdVec.end();
	      for(; hv!=hvEnd;++hv){
		nEL_HEC++;
		HWIdentifier electrodeId = *hv;
		int DET = m_electrodeHelper->detector( electrodeId);
		int SID = m_electrodeHelper->zside( electrodeId);
		int MOD = m_electrodeHelper->module( electrodeId);
		int PHI = m_electrodeHelper->hv_phi( electrodeId);
		int ETA = m_electrodeHelper->hv_eta( electrodeId);
		int GAP = m_electrodeHelper->gap( electrodeId);
		int ELE = m_electrodeHelper->electrode( electrodeId);
		fileHVHEC << "HVlineID=["
			  << PART << "." 
			  << CANL << "." 
			  << CANN << "." 
			  << LINE << "]" 
			  << " electrodeID=[" 
			  << DET << "." 
			  << SID << "." 
			  << MOD << "." 
			  << PHI << "." 
			  << ETA << "." 
			  << GAP << "." 
			  << ELE << "]" 
			  << std::endl;
	      }	  
	    }
	  }
	  if( m_hvHelper->isEMBPS( hvId ) ){ nEMBPS++;}
	  if( m_hvHelper->isEMECPS( hvId ) ){ nEMECPS++;}
	  IdentifierHash hashId = m_hvHelper->hvlineHash( hvId );
	  HWIdentifier   hvId2  = m_hvHelper->HVLineId( hashId );
	  if ( hvId2 != hvId ) {
	    log << MSG::INFO
		<< " HvId2 incorrect: " <<  m_hvHelper->show_to_string(hvId2) 
		<< " should be: " <<  m_hvHelper->show_to_string(hvId) 
		<< " hashId: " << hashId
		<< endmsg;
	    error = true;
	  }
	}
      
      int nDET = nFCAL+nHEC+nEMB+nEMEC+nEMBPS+nEMECPS;
      log << MSG::INFO << "============================================================================ " << endmsg;
      if( m_Detector == "ALL" ){
	log << MSG::INFO << " [HVLINE] |       SUMMARY    " << endmsg;
	log << MSG::INFO << "============================================================================ " << endmsg;
      }
      if( m_Detector == "EMB" || m_Detector == "ALL" ){
	log << MSG::INFO << " [HVLINE] |  EMBARREL  SUMMARY (list in ATLAS_HIGHVOLTAGE_EMB.out)   " << endmsg;
	log << MSG::INFO << "============================================================================ " << endmsg;	
	log << MSG::INFO << "  -- EMB          : " << nEMB  << " (ref: 1008)" << endmsg;
	log << MSG::INFO << "        - A-side  :  " << nEMBA  << " (ref: 488)" << endmsg;
	log << MSG::INFO << "        - C-side  :  " << nEMBC  << " (ref: 520)" << endmsg;
	log << MSG::INFO << "  -- EMB electrode: " << nEL_EMB  << " (ref: 28,672)" << endmsg;      
      }
      if( (m_Detector == "EMB" && m_SubDetector == "S0" ) || m_Detector == "ALL"){
	log << MSG::INFO << "  -- EMBPS    :  " << nEMBPS << " (ref:  512)" << endmsg;
      }
      if( (m_Detector == "EMEC" && m_SubDetector == "S0" ) || m_Detector == "ALL"){
	log << MSG::INFO << "  -- EMECPS   :  " << nEMECPS << " (ref:  128)" << endmsg;
      }
      if( m_Detector == "EMEC" || m_Detector == "ALL" ){
	log << MSG::INFO << " [HVLINE] |   EMEC  SUMMARY  (list in ATLAS_HIGHVOLTAGE_EMEC.out)  " << endmsg;
	log << MSG::INFO << "============================================================================ " << endmsg;
	log << MSG::INFO << "  -- EMEC         : " << nEMEC << " (ref: 1493)" << endmsg; 
	log << MSG::INFO << "        - A-side  :   " << nEMECA << " (ref:  754)" << endmsg; 
	log << MSG::INFO << "        - C-side  :   " << nEMECC << " (ref:  739)" << endmsg;
	log << MSG::INFO << "  --EMEC electrode: " << nEL_EMEC << " (ref: 23,552)" << endmsg;      
      }
      if( m_Detector == "FCAL" || m_Detector == "ALL" ){ 
	log << MSG::INFO << " [HVLINE] |   FCAL  SUMMARY (list in ATLAS_HIGHVOLTAGE_FCAL.out)   " << endmsg;
	log << MSG::INFO << "============================================================================ " << endmsg;
	log << MSG::INFO << "  -- FCAL     :  " << nFCAL << " (ref:  224)" << endmsg;
	log << MSG::INFO << "        - A-side  :  " << nFCALA  << " (ref:  112)" << endmsg;
	log << MSG::INFO << "        - C-side  :  " << nFCALC  << " (ref:  112)" << endmsg;
	log << MSG::INFO << "  --FCAL electrode: " << nEL_FCAL  << " (ref: 224)" << endmsg;      
      }
      if( m_Detector == "HEC" || m_Detector == "ALL" ){
	log << MSG::INFO << " [HVLINE] |   HEC  SUMMARY  (list in ATLAS_HIGHVOLTAGE_HEC.out)  " << endmsg;
	log << MSG::INFO << "============================================================================ " << endmsg;
	log << MSG::INFO << "  -- HEC          :  " << nHEC  << " (ref: 1024)" << endmsg;	    
	log << MSG::INFO << "    - A-side      :  " << nHECA  << " (ref: 512)" << endmsg;	    
	log << MSG::INFO << "    - C-side      :  " << nHECC  << " (ref: 512)" << endmsg;
	log << MSG::INFO << "  -- HEC electrode: " << nEL_HEC  << " (ref: 10,240)" << endmsg;      
      }	    
      log << MSG::INFO << "============================================================================ " << endmsg;
      if( m_Detector == "ALL"){
	log << MSG::INFO << "  -- TOTAL    : " << nDET << endmsg; 
	log << MSG::INFO << "  -- TOTAL hash:" << n_hvline << ", hvline_hash_max= " 
	    << m_hvHelper->hvlineHashMax() << endmsg;
	log << MSG::INFO << "============================================================================ " << endmsg;     
      }
    }// if m_HVlineToOffline
    */

    
    if( m_HVlineToOffline == "ON" && m_HVelectrodeToOffline == "ON"){
      {// if m_HVelectrodeToOffline == ON
	//=========================================
	//=========================================
	//=========================================
	//       Tests of ElectrodeID
	//=========================================
	//=========================================
	//=========================================
	
	std::vector<HWIdentifier>::const_iterator itElectrode = m_electrodeHelper->electrode_begin();
	std::vector<HWIdentifier>::const_iterator itElectrodeEnd = m_electrodeHelper->electrode_end();    
	int nelEMB   = 0;
	int nelEMBPS = 0;
	int nelEMEC   = 0;
	int nelEMECin = 0;
	int nelEMECout= 0;
	int nelEMECPS= 0;
	int nelFCAL  = 0;
	int nelHEC   =0;
	int n_electrode=0;
	int n_electrodeold=0;
	
	for(; itElectrode!=itElectrodeEnd;++itElectrode)
	  {// Start loop over electrodes
	    HWIdentifier elecId = *itElectrode;      
	    n_electrode++;
	    if(n_electrode==n_electrodeold+5000){
	      ATH_MSG_INFO ( "[ELECTRODE] processing ElectrodeID ..# "  
                             << n_electrode );
	      n_electrodeold=n_electrode;
	    }
	    if( m_Detector == "EMB" || m_Detector == "ALL"){
	      if( m_electrodeHelper->isEMB( elecId ) ){ 
		nelEMB++;
		HWIdentifier hvlineId = hvmap->getLArHVLineID( elecId );
		int DET = m_electrodeHelper->detector( elecId);
		int SID = m_electrodeHelper->zside( elecId);
		int MOD = m_electrodeHelper->module( elecId);
		int PHI = m_electrodeHelper->hv_phi( elecId);
		int ETA = m_electrodeHelper->hv_eta( elecId);
		int GAP = m_electrodeHelper->gap( elecId);
		int ELE = m_electrodeHelper->electrode( elecId);
		int PART= m_hvHelper->partition( hvlineId );
		int CANL= m_hvHelper->can_line( hvlineId );
		int CANN= m_hvHelper->can_node( hvlineId );
		int LINE= m_hvHelper->hv_line( hvlineId );
		fileHVEMB << " electrodeID=[" 
			  << DET << "." 
			  << SID << "." 
			  << MOD << "." 
			  << PHI << "." 
			  << ETA << "." 
			  << GAP << "." 
			  << ELE << "] HVlineID=[" 
			  << PART << "." 
			  << CANL << "." 
			  << CANN << "." 
			  << LINE << "]" 
			  << std::endl;
	      }
	    }
	    if( m_Detector == "EMEC" || m_Detector == "ALL"){
	      if( m_electrodeHelper->isEMEC( elecId ) ){ 
		nelEMEC++;
		HWIdentifier hvlineId = hvmap->getLArHVLineID( elecId );
		int DET = m_electrodeHelper->detector( elecId);
		int SID = m_electrodeHelper->zside( elecId);
		int MOD = m_electrodeHelper->module( elecId);
		int PHI = m_electrodeHelper->hv_phi( elecId);
		int ETA = m_electrodeHelper->hv_eta( elecId);
		int GAP = m_electrodeHelper->gap( elecId);
		int ELE = m_electrodeHelper->electrode( elecId);
		int PART= m_hvHelper->partition( hvlineId );
		int CANL= m_hvHelper->can_line( hvlineId );
		int CANN= m_hvHelper->can_node( hvlineId );
		int LINE= m_hvHelper->hv_line( hvlineId );
		fileHVEMEC << " electrodeID=[" 
			   << DET << "." 
			   << SID << "." 
			   << MOD << "." 
			   << PHI << "." 
			   << ETA << "." 
			   << GAP << "." 
			   << ELE << "] HVlineID=[" 
			   << PART << "." 
			   << CANL << "." 
			   << CANN << "." 
			   << LINE << "]" 
			   << std::endl; 
	      }
	      if( m_electrodeHelper->isEMECinWHEEL( elecId ) ){ nelEMECin++;}
	      if( m_electrodeHelper->isEMECoutWHEEL( elecId ) ){ nelEMECout++;}
	    }
	    if( m_Detector == "HEC" || m_Detector == "ALL"){
	      if( m_electrodeHelper->isHEC( elecId ) ){ 
		nelHEC++;
		HWIdentifier hvlineId = hvmap->getLArHVLineID( elecId );
		int DET = m_electrodeHelper->detector( elecId);
		int SID = m_electrodeHelper->zside( elecId);
		int MOD = m_electrodeHelper->module( elecId);
		int PHI = m_electrodeHelper->hv_phi( elecId);
		int ETA = m_electrodeHelper->hv_eta( elecId);
		int GAP = m_electrodeHelper->gap( elecId);
		int ELE = m_electrodeHelper->electrode( elecId);
		int PART= m_hvHelper->partition( hvlineId );
		int CANL= m_hvHelper->can_line( hvlineId );
		int CANN= m_hvHelper->can_node( hvlineId );
		int LINE= m_hvHelper->hv_line( hvlineId );
		if( CANN > 180 ){
		  CANN = -1;
		  CANL = -1;
		  LINE = -1;
		  PART = -1;
		}
		fileHVHEC << " electrodeID=[" 
			  << DET << "." 
			  << SID << "." 
			  << MOD << "." 
			  << PHI << "." 
			  << ETA << "." 
			  << GAP << "." 
			  << ELE << "] HVlineID=[" 
			  << PART << "." 
			  << CANL << "." 
			  << CANN << "." 
			  << LINE << "]" 
			  << std::endl;
	      }
	    }
	    if( m_Detector == "FCAL" || m_Detector == "ALL"){
	      if( m_electrodeHelper->isFCAL( elecId ) ){ 
		nelFCAL++;
		HWIdentifier hvlineId = hvmap->getLArHVLineID( elecId );
		int DET = m_electrodeHelper->detector( elecId);
		int SID = m_electrodeHelper->zside( elecId);
		int MOD = m_electrodeHelper->module( elecId);
		int PHI = m_electrodeHelper->hv_phi( elecId);
		int ETA = m_electrodeHelper->hv_eta( elecId);
		int GAP = m_electrodeHelper->gap( elecId);
		int ELE = m_electrodeHelper->electrode( elecId);
		int PART= m_hvHelper->partition( hvlineId );
		int CANL= m_hvHelper->can_line( hvlineId );
		int CANN= m_hvHelper->can_node( hvlineId );
		int LINE= m_hvHelper->hv_line( hvlineId );
		fileHVFCAL << " electrodeID=[" 
			   << DET << "." 
			   << SID << "." 
			   << MOD << "." 
			   << PHI << "." 
			   << ETA << "." 
			   << GAP << "." 
			   << ELE << "] HVlineID=[" 
			   << PART << "." 
			   << CANL << "." 
			   << CANN << "." 
			   << LINE << "]" 
			   << std::endl;
	      }
	    }
	    
	    if( m_Detector == "EMECPS" || m_Detector == "ALL"){
	      if( m_electrodeHelper->isEMBPS( elecId ) ){ nelEMBPS++;}
	    }
	    if( m_Detector == "EMECPS" || m_Detector == "ALL"){
	      if( m_electrodeHelper->isEMECPS( elecId ) ){ nelEMECPS++;}
	    }
	    IdentifierHash hashId = m_electrodeHelper->electrodeHash( elecId );
	    HWIdentifier   elecId2  = m_electrodeHelper->ElectrodeId( hashId );
	    if ( elecId2 != elecId ) {
	      ATH_MSG_INFO 
                ( " elId2 incorrect: " <<  m_electrodeHelper->show_to_string(elecId2) 
		  << " should be: " <<  m_electrodeHelper->show_to_string(elecId) 
		  << " hashId: " << hashId
		  );
	    }
	  }
	int nelDET = nelFCAL+nelHEC+nelEMB+nelEMEC+nelEMBPS+nelEMECPS;
	ATH_MSG_INFO  ( "============================================================================ " );
	if( m_Detector == "ALL" ){
	  ATH_MSG_INFO  ( " [ELECTRODE] | Test of LArElectrodeID's " );
	  ATH_MSG_INFO  ( "============================================================================ " );
	}
	if( m_Detector == "ALL"){
	  ATH_MSG_INFO  ( "  total number Electrode " << n_electrode << ", electrode_hash_max= " 
                          << m_electrodeHelper->electrodeHashMax() );
	}
	if( m_Detector == "EMB" || m_Detector == "ALL"){
	  ATH_MSG_INFO  ( " [ELECTRODE] |  EMBARREL SUMMARY (list in ATLAS_HIGHVOLTAGE_EMB.out)" );
	  ATH_MSG_INFO  ( "============================================================================ " );
	  ATH_MSG_INFO  ( "  -- EMB      : " << nelEMB  << " (ref: 28672 for 1008 HV lines)" );
	}
	if( m_Detector == "EMEC" || m_Detector == "ALL"){ 
	  ATH_MSG_INFO  ( " [ELECTRODE] |  EMEC   SUMMARY (list in ATLAS_HIGHVOLTAGE_EMEC.out)" );
	  ATH_MSG_INFO  ( "============================================================================ " );
	  ATH_MSG_INFO  ( "  -- EMEC     : " << nelEMEC << " (ref: 23552 for 1493 HV lines)" );
	  ATH_MSG_INFO  ( "    - inWheel :  " << nelEMECin << " (ref:  2048)" );
	  ATH_MSG_INFO  ( "    - outWheel: " << nelEMECout << " (ref: 21504)" );
	}
	if( m_Detector == "HEC" || m_Detector == "ALL"){
	  ATH_MSG_INFO  ( " [ELECTRODE] |  HEC  SUMMARY (list in ATLAS_HIGHVOLTAGE_HEC.out)" );
	  ATH_MSG_INFO  ( "============================================================================ " );
	  ATH_MSG_INFO  ( "  -- HEC      : " << nelHEC  << " (ref: 10240 for 1024 HV lines)" );
	}
	if( m_Detector == "FCAL" || m_Detector == "ALL"){
	  ATH_MSG_INFO  ( " [ELECTRODE] |  FCAL   SUMMARY (list in ATLAS_HIGHVOLTAGE_FCAL.out)" );
	  ATH_MSG_INFO  ( "============================================================================ " );
	  ATH_MSG_INFO  ( "  -- FCAL     :   " << nelFCAL << " (ref:  384 for 224 HV lines)" );
	}
	if( m_Detector == "EMBPS" || m_Detector == "ALL"){
	  ATH_MSG_INFO  ( " [ELECTRODE] |  EMBPS SUMMARY (list in ATLAS_HIGHVOLTAGE_EMBPS.out)" );
	  ATH_MSG_INFO  ( "============================================================================ " );
	  ATH_MSG_INFO  ( "  -- EMBPS    :  " << nelEMBPS << " (ref: 1024 for 512 HV lines)" );
	}
	if( m_Detector == "EMECPS" || m_Detector == "ALL"){
	  ATH_MSG_INFO  ( " [ELECTRODE] |  EMECS SUMMARY (list in ATLAS_HIGHVOLTAGE_EMECPS.out)" );
	  ATH_MSG_INFO  ( "============================================================================ " );
	  ATH_MSG_INFO  ( "  -- EMECPS   :   " << nelEMECPS << " (ref:  128 for 128 HV lines)" );
	}
	if( m_Detector == "ALL"){
	  ATH_MSG_INFO  ( "============================================================================ " );
	  ATH_MSG_INFO  ( "  -- TOTAL    : " << nelDET );
	  ATH_MSG_INFO  ( "============================================================================ " );
	}
      }
      ATH_MSG_INFO  ( "============================================================================ " );
      
      // Close() files
      // -------------
      if( m_HighVoltage == "ON" ){
	if( m_Detector == "EMB" || m_Detector == "ALL"){
	  fileHVEMB.close();
	}
	if( m_Detector == "EMBPS" || m_Detector == "ALL"){
	  fileHVEMBPS.close();
	}
	if( m_Detector == "EMEC" || m_Detector == "ALL"){
	  fileHVEMEC.close();
	}
	if( m_Detector == "EMECPS" || m_Detector == "ALL"){
	  fileHVEMECPS.close();
	}
	if( m_Detector == "FCAL" || m_Detector == "ALL"){
	  fileHVFCAL.close();
	}
	if( m_Detector == "HEC" || m_Detector == "ALL"){
	  fileHVHEC.close();
	}
      }
    } // m_HVelectrodeToOffline == ON


    
    // ==============================================
    // ==============================================
    // ==============================================
    //   Test of OfflineID <--> Electrode Identifiers 
    // ==============================================
    // ==============================================
    // ==============================================
    
    
    if( m_HVelectrodeToOffline == "ON" )
      {// if OFFlineTest == ON && TEST_ELECTRODE 
	int nEMB   = 0;

	int nEMB1  = 0;
	int nEMB1r0= 0;
	int nEMB1r1= 0;
	int nEMB2  = 0;
	int nEMB2r0= 0;
	int nEMB2r1= 0;
	int nEMB3  = 0;
	int nEMB0  = 0;
	int nelEMB   = 0;
	int nelEMB0  = 0;
	int nelEMB1  = 0;
	int nelEMB1r0= 0;
	int nelEMB1r1= 0;
	int nelEMB2  = 0;
	int nelEMB2r0= 0;
	int nelEMB2r1= 0;
	int nelEMB3  = 0;
	
	int nEMEC  = 0;
	int nEMEC0 = 0;
	int nEMEC1  = 0;
	int nEMEC1r0 = 0;
	int nEMEC1r0IN= 0;
	int nEMEC1r1 = 0;
	int nEMEC1r2 = 0;
	int nEMEC1r3 = 0;
	int nEMEC1r4 = 0;
	int nEMEC1r5 = 0;
	int nEMEC2     = 0;
	int nEMEC2r0   = 0;
	int nEMEC2r0IN = 0;
	int nEMEC2r1   = 0;
	int nEMEC3 = 0;

	int nelEMEC = 0;
	int nelEMEC0= 0;
	int nelEMEC1= 0;
	int nelEMEC1r0 = 0;
	int nelEMEC1r0IN= 0;
	int nelEMEC1r1 = 0;
	int nelEMEC1r2 = 0;
	int nelEMEC1r3 = 0;
	int nelEMEC1r4 = 0;
	int nelEMEC1r5 = 0;
	int nelEMEC2     = 0;
	int nelEMEC2r0   = 0;
	int nelEMEC2r1   = 0;
	int nelEMEC2r0IN = 0;
	int nelEMEC3= 0;

	int nFCAL  = 0;

	int nHEC   = 0;
	int nHEC0  = 0;
	int nHEC0r0= 0;
	int nHEC0r1= 0;
	int nHEC1  = 0;
	int nHEC1r0= 0;
	int nHEC1r1= 0;
	int nHEC2  = 0;
	int nHEC2r0= 0;
	int nHEC2r1= 0;
	int nHEC3  = 0;
	int nHEC3r0= 0;
	int nHEC3r1= 0;

	int nelFCAL = 0;
	int nelHEC  = 0;
	int nelHEC0  = 0;
	int nelHEC0r0= 0;
	int nelHEC0r1= 0;
	int nelHEC1  = 0;
	int nelHEC1r0= 0;
	int nelHEC1r1= 0;
	int nelHEC2  = 0;
	int nelHEC2r0= 0;
	int nelHEC2r1= 0;
	int nelHEC3  = 0;
	int nelHEC3r0= 0;
	int nelHEC3r1= 0;

	int nOLD    = 0;
	if( m_Detector == "EMB" || m_Detector == "ALL" ) 
	  {// if m_Detector == EM
	    std::vector<Identifier>::const_iterator itId = m_emHelper->em_begin();
	    std::vector<Identifier>::const_iterator itIdEnd = m_emHelper->em_end();
	    for(; itId!=itIdEnd;++itId )
	      {// Loop over EM connected channels
		const Identifier offId = *itId;
		int bec   = m_emHelper->barrel_ec( offId);
		int sam   = m_emHelper->sampling( offId);
		int region= m_emHelper->region( offId );
		//int eta   = m_emHelper->eta( offId);
		int phi   = m_emHelper->phi( offId);
		//int Side  = -9;
		if( abs(bec) == 1 ){
		  //if( bec > 0 ){ 
		  //  Side = 0;}
		  //else{Side = 1;}
		  // ---------
		  // EMB-ALL
		  // ---------
		  if( m_SubDetector == "ALL" ){
		    nEMB++;
		    if( nEMB == nOLD+1000 ){
		      ATH_MSG_INFO ( "[OFFLINE->ELECTRODE] Processing  cell " << nEMB << " in EMB " );
		      nOLD = nEMB;
		    }
		    // Test of getCellElectrodeIdVec()
		    std::vector<HWIdentifier> IdVec;
		    hvmap->getElectrodeInCell( offId, IdVec );
		    std::vector<HWIdentifier> electrodeIdVec = IdVec;
		    nelEMB+=electrodeIdVec.size();
		  }
		  // Test of EMPS
		  // -------------
		  if( (m_SubDetector == "S0" || m_SubDetector== "ALL") && ( sam == 0 )){
		    nEMB0++;
		    ATH_MSG_DEBUG ( "EMBPS Phi=" << phi );
		    if( nEMB0 == nOLD+1000 ){
		      ATH_MSG_INFO ( "[OFFLINE->ELECTRODE] Processing  cell " << nEMB0 << " in EMBPS " );
		      nOLD = nEMB0;
		    }
		    // Test of getCellElectrodeIdVec()
		    std::vector<HWIdentifier> IdVec;
		    hvmap->getElectrodeInCell( offId, IdVec );
		    std::vector<HWIdentifier> electrodeIdVec = IdVec;
		    // Loop over vector of electrodes 
		    std::vector<HWIdentifier>::const_iterator hv = electrodeIdVec.begin();
		    std::vector<HWIdentifier>::const_iterator hvEnd = electrodeIdVec.end();
		    //log << MSG::INFO << "[TestLArHW] electrodeIdSize=" << electrodeIdVec.size() << endmsg;
		    nelEMB0+=electrodeIdVec.size();
		    for(; hv!=hvEnd;++hv){
		      HWIdentifier electrodeId = *hv;
		      int DET = m_electrodeHelper->detector( electrodeId);
		      int SID = m_electrodeHelper->zside( electrodeId);
		      int MOD = m_electrodeHelper->module( electrodeId);
		      int PHI = m_electrodeHelper->hv_phi( electrodeId);
		      int ETA = m_electrodeHelper->hv_eta( electrodeId);
		      int GAP = m_electrodeHelper->gap( electrodeId);
		      int ELE = m_electrodeHelper->electrode( electrodeId);		   
		      fileHVEMBOFF 
			<< "OfflineID " 
			<< m_onlineHelper->show_to_string(offId) 
			<< " electrodeID=[" 
			<< DET << "." 
			<< SID << "." 
			<< MOD << "." 
			<< PHI << "." 
			<< ETA << "." 
			<< GAP << "." 
			<< ELE << "]" 
			<< std::endl;
		    }
		  }
		  // Test of EM1
		  // -----------
		  if( (m_SubDetector == "S1"  || m_SubDetector== "ALL") && ( sam == 1 ) ){
		    nEMB1++;
		    if( region == 0 ){nEMB1r0++;}
		    if( region == 1 ){nEMB1r1++;}
		    if( nEMB1 == nOLD+1000 ){
		      ATH_MSG_INFO ( "[OFFLINE->ELECTRODE] Processing  cell " << nEMB1 << " in EMB1 " );
		      nOLD = nEMB1;
		    }
		    // Test of getCellElectrodeIdVec()
		    std::vector<HWIdentifier> IdVec;
		    hvmap->getElectrodeInCell( offId, IdVec );
		    std::vector<HWIdentifier> electrodeIdVec = IdVec;
		    // Loop over vector of electrodes 
		    std::vector<HWIdentifier>::const_iterator hv = electrodeIdVec.begin();
		    std::vector<HWIdentifier>::const_iterator hvEnd = electrodeIdVec.end();
		    //log << MSG::INFO << "[TestLArHW] electrodeIdSize=" << electrodeIdVec.size() << endmsg;
		    nelEMB1+=electrodeIdVec.size();
		    if( region == 0 ){nelEMB1r0+=electrodeIdVec.size();}
		    if( region == 1 ){nelEMB1r1+=electrodeIdVec.size();}
		    for(; hv!=hvEnd;++hv){
		      HWIdentifier electrodeId = *hv;
		      int DET = m_electrodeHelper->detector( electrodeId);
		      int SID = m_electrodeHelper->zside( electrodeId);
		      int MOD = m_electrodeHelper->module( electrodeId);
		      int PHI = m_electrodeHelper->hv_phi( electrodeId);
		      int ETA = m_electrodeHelper->hv_eta( electrodeId);
		      int GAP = m_electrodeHelper->gap( electrodeId);
		      int ELE = m_electrodeHelper->electrode( electrodeId);		   
		      fileHVEMBOFF 
			<< "OfflineID " 
			<< m_onlineHelper->show_to_string(offId) 
			<< " electrodeID=[" 
			<< DET << "." 
			<< SID << "." 
			<< MOD << "." 
			<< PHI << "." 
			<< ETA << "." 
			<< GAP << "." 
			<< ELE << "]" 
			<< std::endl;
		    }
		  }
		  // Test of EM2
		  // -----------
		  if( (m_SubDetector == "S2" || m_SubDetector== "ALL") && ( sam == 2) ){
		    nEMB2++;
		    if( region == 0 ){nEMB2r0++;}
		    if( region == 1 ){nEMB2r1++;}

		    if( nEMB2 == nOLD+1000 ){
		      ATH_MSG_INFO ( "[OFFLINE->ELECTRODE] Processing  cell " << nEMB2 << " in EMB2 " );
		      nOLD = nEMB2;
		    }
		    // Test of getCellElectrodeIdVec()
		    std::vector<HWIdentifier> IdVec;
		    hvmap->getElectrodeInCell( offId, IdVec );
		    std::vector<HWIdentifier> electrodeIdVec = IdVec;
		    // Loop over vector of electrodes 
		    std::vector<HWIdentifier>::const_iterator hv = electrodeIdVec.begin();
		    std::vector<HWIdentifier>::const_iterator hvEnd = electrodeIdVec.end();
		    //log << MSG::INFO << "[TestLArHW] electrodeIdSize=" << electrodeIdVec.size() << endmsg;
		    nelEMB2+=electrodeIdVec.size();
		    if( region == 0 ){nelEMB2r0+=electrodeIdVec.size();}
		    if( region == 1 ){nelEMB2r1+=electrodeIdVec.size();}
		    for(; hv!=hvEnd;++hv){
		      HWIdentifier electrodeId = *hv;
		      int DET = m_electrodeHelper->detector( electrodeId);
		      int SID = m_electrodeHelper->zside( electrodeId);
		      int MOD = m_electrodeHelper->module( electrodeId);
		      int PHI = m_electrodeHelper->hv_phi( electrodeId);
		      int ETA = m_electrodeHelper->hv_eta( electrodeId);
		      int GAP = m_electrodeHelper->gap( electrodeId);
		      int ELE = m_electrodeHelper->electrode( electrodeId);		   
		      fileHVEMBOFF 
			<< "OfflineID " 
			<< m_onlineHelper->show_to_string(offId) 
			<< " electrodeID=[" 
			<< DET << "." 
			<< SID << "." 
			<< MOD << "." 
			<< PHI << "." 
			<< ETA << "." 
			<< GAP << "." 
			<< ELE << "]" 
			<< std::endl;
		    }
		  }
		  // Test of EM3
		  // -----------
		  if( (m_SubDetector == "S3" || m_SubDetector== "ALL") && ( sam == 3) ){
		    nEMB3++;
		    ATH_MSG_DEBUG ( "EM3 Phi=" << phi );
		    if( nEMB3 == nOLD+1000 ){
		      ATH_MSG_INFO ( "[OFFLINE->ELECTRODE] Processing  cell " << nEMB3 << " in EMB3 " );
		      nOLD = nEMB3;
		    }
		    // Test of getCellElectrodeIdVec()
		    std::vector<HWIdentifier> IdVec;
		    hvmap->getElectrodeInCell( offId, IdVec );
		    std::vector<HWIdentifier> electrodeIdVec = IdVec;
		    // Loop over vector of electrodes 
		    std::vector<HWIdentifier>::const_iterator hv = electrodeIdVec.begin();
		    std::vector<HWIdentifier>::const_iterator hvEnd = electrodeIdVec.end();
		    nelEMB3 += electrodeIdVec.size();
		    for(; hv!=hvEnd;++hv){
		      HWIdentifier electrodeId = *hv;
		      int DET = m_electrodeHelper->detector( electrodeId);
		      int SID = m_electrodeHelper->zside( electrodeId);
		      int MOD = m_electrodeHelper->module( electrodeId);
		      int PHI = m_electrodeHelper->hv_phi( electrodeId);
		      int ETA = m_electrodeHelper->hv_eta( electrodeId);
		      int GAP = m_electrodeHelper->gap( electrodeId);
		      int ELE = m_electrodeHelper->electrode( electrodeId);		   
		      fileHVEMBOFF 
			<< "OfflineID " 
			<< m_onlineHelper->show_to_string(offId) 
			<< " electrodeID=[" 
			<< DET << "." 
			<< SID << "." 
			<< MOD << "." 
			<< PHI << "." 
			<< ETA << "." 
			<< GAP << "." 
			<< ELE << "]" 
			<< std::endl;
		    }
		  }
		}
	      }
	  }
  
	if( m_Detector == "EMEC" || m_Detector == "ALL" ) 
	  {// if m_Detector == EMEC
	    std::vector<Identifier>::const_iterator itId = m_emHelper->em_begin();
	    std::vector<Identifier>::const_iterator itIdEnd = m_emHelper->em_end();
	    for(; itId!=itIdEnd;++itId )
	      {// Loop over EM connected channels
		nEMEC++;
		const Identifier offId = *itId;
		int bec   = m_emHelper->barrel_ec( offId );
		int sam   = m_emHelper->sampling( offId );
		int region= m_emHelper->region( offId );
		//int Side  = -9;
		if( abs(bec) == 2 || abs(bec) == 3 ){
		  //if( bec > 0 ){ 
		  //  Side = 0;}
		  //else{Side = 1;}
		  
		  // -------------
		  // Test of EMECPS
		  // -------------
		  if( (m_SubDetector == "S0" || m_SubDetector== "ALL") && ( sam == 0 ) ){
		    nEMEC0++;		      
		    // Test of individual methods:
		    if( nEMEC0 == nOLD+1000 ){
		      ATH_MSG_INFO ( "[OFFLINE->ELECTRODE] Processing  cell " << nEMEC0 << " in EMEC1 " );
		      nOLD = nEMEC0;
		    }
		    
		    // Test of getCellElectrodeIdVec()
		    std::vector<HWIdentifier> IdVec;
		    hvmap->getElectrodeInCell( offId, IdVec );
		    std::vector<HWIdentifier> electrodeIdVec = IdVec;
		    // Loop over vector of electrodes 
		    std::vector<HWIdentifier>::const_iterator hv = electrodeIdVec.begin();
		    std::vector<HWIdentifier>::const_iterator hvEnd = electrodeIdVec.end();
		    nelEMEC0+=electrodeIdVec.size();
		    for(; hv!=hvEnd;++hv){
		      HWIdentifier electrodeId = *hv;
		      int DET = m_electrodeHelper->detector( electrodeId );
		      int SID = m_electrodeHelper->zside( electrodeId );
		      int MOD = m_electrodeHelper->module( electrodeId );
		      int PHI = m_electrodeHelper->hv_phi( electrodeId );
		      int ETA = m_electrodeHelper->hv_eta( electrodeId );
		      int GAP = m_electrodeHelper->gap( electrodeId );
		      int ELE = m_electrodeHelper->electrode( electrodeId );		   
		      fileHVEMECOFF 
			<< "OfflineID " 
			<< m_onlineHelper->show_to_string(offId) 
			<< " electrodeID=[" 
			<< DET << "." 
			<< SID << "." 
			<< MOD << "." 
			<< PHI << "." 
			<< ETA << "." 
			<< GAP << "." 
			<< ELE << "]" 
			<< std::endl;
		    }
		  }
		  
		  // -------------
		  // Test of EMEC1
		  // -------------
		  if( (m_SubDetector == "S1"|| m_SubDetector== "ALL") && ( sam == 1)  ){
		    // m_SubDetector == EMEC1
		    nEMEC1++;		     
		    if( region == 0 && abs(bec) == 2 ){nEMEC1r0++;}
		    if( region == 0 && abs(bec) == 3 ){nEMEC1r0IN++;}
		    if( region == 1 ){nEMEC1r1++;}
		    if( region == 2 ){nEMEC1r2++;}
		    if( region == 3 ){nEMEC1r3++;}
		    if( region == 4 ){nEMEC1r4++;}
		    if( region == 5 ){nEMEC1r5++;}
		    // Test of individual methods:
		    if( nEMEC1 == nOLD+1000 ){
		      ATH_MSG_INFO ( "[OFFLINE->ELECTRODE] Processing cell " << nEMEC1 << " in EMEC1 " );
		      nOLD = nEMEC1;
		    }		    
		    // Test of getCellElectrodeIdVec()
		    std::vector<HWIdentifier> IdVec;
		    hvmap->getElectrodeInCell( offId, IdVec );
		    std::vector<HWIdentifier> electrodeIdVec = IdVec;
		    // Loop over vector of electrodes 
		    std::vector<HWIdentifier>::const_iterator hv = electrodeIdVec.begin();
		    std::vector<HWIdentifier>::const_iterator hvEnd = electrodeIdVec.end();
		    nelEMEC1+=electrodeIdVec.size();
 		    if( region == 0 && abs(bec) == 2){nelEMEC1r0 +=electrodeIdVec.size();}
		    if( region == 0 && abs(bec) == 3){nelEMEC1r0IN +=electrodeIdVec.size();}
		    if( region == 1 ){nelEMEC1r1+=electrodeIdVec.size();}
		    if( region == 2 ){nelEMEC1r2+=electrodeIdVec.size();}
		    if( region == 3 ){nelEMEC1r3+=electrodeIdVec.size();}
		    if( region == 4 ){nelEMEC1r4+=electrodeIdVec.size();}
		    if( region == 5 ){nelEMEC1r5+=electrodeIdVec.size();}
		    for(; hv!=hvEnd;++hv){
		      HWIdentifier electrodeId = *hv;
		      int DET = m_electrodeHelper->detector( electrodeId );
		      int SID = m_electrodeHelper->zside( electrodeId );
		      int MOD = m_electrodeHelper->module( electrodeId );
		      int PHI = m_electrodeHelper->hv_phi( electrodeId );
		      int ETA = m_electrodeHelper->hv_eta( electrodeId );
		      int GAP = m_electrodeHelper->gap( electrodeId );
		      int ELE = m_electrodeHelper->electrode( electrodeId );		   
		      fileHVEMECOFF 
			<< "OfflineID " 
			<< m_onlineHelper->show_to_string(offId) 
			<< " electrodeID=[" 
			<< DET << "." 
			<< SID << "." 
			<< MOD << "." 
			<< PHI << "." 
			<< ETA << "." 
			<< GAP << "." 
			<< ELE << "]" 
			<< std::endl;
		    }
		  }
		  // --------------
		  // Test of EMEC2
		  // --------------
		  if( (m_SubDetector == "S2" || m_SubDetector== "ALL") && ( sam == 2 ) ){
		    // m_SubDetector == EMEC2
		    nEMEC2++;
		    if( region == 0 && abs(bec) == 2 ){nEMEC2r0++;}
		    if( region == 0 && abs(bec) == 3 ){nEMEC2r0IN++;}
		    if( region == 1 ){nEMEC2r1++;}
		    // Test of individual methods:
		    if( nEMEC2 == nOLD+1000 ){
		      ATH_MSG_INFO ( "[OFFLINE->ELECTRODE] Processing cell " << nEMEC2 << " in EMEC2 " );
		      nOLD = nEMEC2;
		    }
		    HWIdentifier onId = cabling->createSignalChannelID( offId ) ; 
		    // Test of getCellElectrodeIdVec()
		    std::vector<HWIdentifier> IdVec;
		    hvmap->getElectrodeInCell( offId, IdVec );
		    std::vector<HWIdentifier> electrodeIdVec = IdVec;
		    // Loop over vector of electrodes 
		    std::vector<HWIdentifier>::const_iterator hv = electrodeIdVec.begin();
		    std::vector<HWIdentifier>::const_iterator hvEnd = electrodeIdVec.end();
		    nelEMEC2+=electrodeIdVec.size();
 		    if( region == 0 && abs(bec) == 2){nelEMEC2r0 +=electrodeIdVec.size();}
		    if( region == 0 && abs(bec) == 3){nelEMEC2r0IN +=electrodeIdVec.size();}
		    if( region == 1 ){nelEMEC2r1+=electrodeIdVec.size();}
		    for(; hv!=hvEnd;++hv){
		      HWIdentifier electrodeId = *hv;
		      int DET = m_electrodeHelper->detector( electrodeId );
		      int SID = m_electrodeHelper->zside( electrodeId );
		      int MOD = m_electrodeHelper->module( electrodeId );
		      int PHI = m_electrodeHelper->hv_phi( electrodeId );
		      int ETA = m_electrodeHelper->hv_eta( electrodeId );
		      int GAP = m_electrodeHelper->gap( electrodeId );
		      int ELE = m_electrodeHelper->electrode( electrodeId );		   
		      fileHVEMECOFF 
			<< "OfflineID=" 
			<< m_onlineHelper->show_to_string(offId)
			<< " OnlineID="
			<< m_onlineHelper->show_to_string(onId)
			<< " electrodeID=[" 
			<< DET << "." 
			<< SID << "." 
			<< MOD << "." 
			<< PHI << "." 
			<< ETA << "." 
			<< GAP << "." 
			<< ELE << "]" 
			<< std::endl;
		    }
		  }
		  // --------------
		  // Test of EMEC3
		  // --------------
		  if( (m_SubDetector == "S3" || m_SubDetector== "ALL") && ( sam == 3) ){
		    // m_SubDetector == EMEC3
		    nEMEC3++;
		    // Test of individual methods:
		    if( nEMEC3 == nOLD+1000 ){
		      ATH_MSG_INFO ( "[OFFLINE->ELECTRODE] Processing cell " << nEMEC3 << " in EMEC3" );
		      nOLD = nEMEC3;
		    }
		    
		    // Test of getCellElectrodeIdVec()
		    std::vector<HWIdentifier> IdVec;
		    hvmap->getElectrodeInCell( offId, IdVec );
		    std::vector<HWIdentifier> electrodeIdVec = IdVec;
		    // Loop over vector of electrodes 
		    std::vector<HWIdentifier>::const_iterator hv = electrodeIdVec.begin();
		    std::vector<HWIdentifier>::const_iterator hvEnd = electrodeIdVec.end();
		    nelEMEC3+=electrodeIdVec.size();
		    HWIdentifier onId = cabling->createSignalChannelID( offId ) ; 
		    for(; hv!=hvEnd;++hv){
		      HWIdentifier electrodeId = *hv;
		      int DET = m_electrodeHelper->detector( electrodeId );
		      int SID = m_electrodeHelper->zside( electrodeId );
		      int MOD = m_electrodeHelper->module( electrodeId );
		      int PHI = m_electrodeHelper->hv_phi( electrodeId );
		      int ETA = m_electrodeHelper->hv_eta( electrodeId );
		      int GAP = m_electrodeHelper->gap( electrodeId );
		      int ELE = m_electrodeHelper->electrode( electrodeId );		   
		      fileHVEMECOFF 
			<< "OfflineID " 
			<< m_onlineHelper->show_to_string(offId) 
			<< " OnlineID="
			<< m_onlineHelper->show_to_string(onId)
			<< " electrodeID=[" 
			<< DET << "." 
			<< SID << "." 
			<< MOD << "." 
			<< PHI << "." 
			<< ETA << "." 
			<< GAP << "." 
			<< ELE << "]" 
			<< std::endl;
		    }
		  }
		  // -----------------
		  // Test of EMEC-ALL
		  // -----------------
		  if( m_SubDetector == "ALL"  ){
		    // m_SubDetector == ALL
		    nEMEC++;
		    if( nEMEC == nOLD+1000 ){
		      ATH_MSG_INFO ( "[OFFLINE->ELECTRODE] Processing cell " << nEMEC << " in EMEC " );
		      nOLD = nEMEC;
		    }    
		    // Test of getCellElectrodeIdVec()
		    std::vector<HWIdentifier> IdVec;
		    hvmap->getElectrodeInCell( offId, IdVec );
		    std::vector<HWIdentifier> electrodeIdVec = IdVec;
		    nelEMEC+=electrodeIdVec.size();
		  } // EMEC-ALL
		}// Side
	      }// Loop over EMEC channels
	  }// m_Detector== EMEC
	
	//-------------------------
	// HEC
	//-------------------------
	if( m_Detector == "HEC" || m_Detector == "ALL" ) 
	  {// if m_Detector == HEC
	    std::vector<Identifier>::const_iterator itId = m_hecHelper->hec_begin();
	    std::vector<Identifier>::const_iterator itIdEnd = m_hecHelper->hec_end();
	    for(; itId!=itIdEnd;++itId )
	      {// Loop over HEC channels
		const Identifier offId = *itId;
		HWIdentifier onId = cabling->createSignalChannelID( offId ) ; 
		int bec   = m_hecHelper->pos_neg( offId);
		int sam   = m_hecHelper->sampling( offId);
		int region= m_hecHelper->region( offId);
		//int Side  = -9;
		if( abs(bec) == 2 ){
		  //if( bec > 0 ){ 
		  //  Side = 0;}
		  //else{Side = 1;}
		  // ---------
		  //   HEC-1
		  // ---------
		  if( (m_SubDetector == "S1" || m_SubDetector == "ALL" )&& sam == 1){
		    // HEC1 
		    nHEC1++;
		    if( region == 0 ){nHEC1r0++;}
		    if( region == 1 ){nHEC1r1++;}		    
		    if( nHEC1 == nOLD+100 ){
		      ATH_MSG_INFO ( "[OFFLINE->ELECTRODE] Processing cell " << nHEC1 << " in HEC " );
		      nOLD = nHEC1;
		    }    
		    // Test of getCellElectrodeIdVec()
		    std::vector<HWIdentifier> IdVec;
		    hvmap->getElectrodeInCell( offId, IdVec );
		    std::vector<HWIdentifier> electrodeIdVec = IdVec;
		    nelHEC1+= electrodeIdVec.size();
		    if( region == 0 ){nelHEC1r0 +=electrodeIdVec.size();}
		    if( region == 1 ){nelHEC1r1 +=electrodeIdVec.size();}
		    // Loop over vector of electrodes 
		    std::vector<HWIdentifier>::const_iterator hv = electrodeIdVec.begin();
		    std::vector<HWIdentifier>::const_iterator hvEnd = electrodeIdVec.end();
		    for(; hv!=hvEnd;++hv){
		      HWIdentifier electrodeId = *hv;
		      int DET = m_electrodeHelper->detector( electrodeId );
		      int SID = m_electrodeHelper->zside( electrodeId );
		      int MOD = m_electrodeHelper->module( electrodeId );
		      int PHI = m_electrodeHelper->hv_phi( electrodeId );
		      int ETA = m_electrodeHelper->hv_eta( electrodeId );
		      int GAP = m_electrodeHelper->gap( electrodeId );
		      int ELE = m_electrodeHelper->electrode( electrodeId );		   
		      fileHVHECOFF 
			<< "OfflineID=" 
			<< m_onlineHelper->show_to_string(offId) 
			<< " OnlineID="
			<< m_onlineHelper->show_to_string(onId) 
			<< " electrodeID=[" 
			<< DET << "." 
			<< SID << "." 
			<< MOD << "." 
			<< PHI << "." 
			<< ETA << "." 
			<< GAP << "." 
			<< ELE << "]" 
			<< std::endl;
		    }
		  }
		  // ---------
		  //   HEC-2
		  // ---------
		  if( (m_SubDetector == "S3" || m_SubDetector == "ALL" )&& sam == 3){
		    // HEC2 
		    nHEC2++;
		    if( region == 0 ){nHEC2r0++;}
		    if( region == 1 ){nHEC2r1++;}		    
		    if( nHEC2 == nOLD+100 ){
		      ATH_MSG_INFO ( "[OFFLINE->ELECTRODE] Processing cell " << nHEC2 << " in HEC " );
		      nOLD = nHEC2;
		    }    
		    // Test of getCellElectrodeIdVec()
		    std::vector<HWIdentifier> IdVec;
		    hvmap->getElectrodeInCell( offId, IdVec );
		    std::vector<HWIdentifier> electrodeIdVec = IdVec;
		    nelHEC2+= electrodeIdVec.size();
		    if( region == 0 ){nelHEC2r0 +=electrodeIdVec.size();}
		    if( region == 1 ){nelHEC2r1 +=electrodeIdVec.size();}
		    // Loop over vector of electrodes 
		    std::vector<HWIdentifier>::const_iterator hv = electrodeIdVec.begin();
		    std::vector<HWIdentifier>::const_iterator hvEnd = electrodeIdVec.end();
		    for(; hv!=hvEnd;++hv){
		      HWIdentifier electrodeId = *hv;
		      int DET = m_electrodeHelper->detector( electrodeId );
		      int SID = m_electrodeHelper->zside( electrodeId );
		      int MOD = m_electrodeHelper->module( electrodeId );
		      int PHI = m_electrodeHelper->hv_phi( electrodeId );
		      int ETA = m_electrodeHelper->hv_eta( electrodeId );
		      int GAP = m_electrodeHelper->gap( electrodeId );
		      int ELE = m_electrodeHelper->electrode( electrodeId );		   
		      fileHVHECOFF 
			<< "OfflineID=" 
			<< m_onlineHelper->show_to_string(offId) 
			<< " OnlineID="
			<< m_onlineHelper->show_to_string(onId) 
			<< " electrodeID=[" 
			<< DET << "." 
			<< SID << "." 
			<< MOD << "." 
			<< PHI << "." 
			<< ETA << "." 
			<< GAP << "." 
			<< ELE << "]" 
			<< std::endl;
		    }
		  }

		  // ---------
		  //   HEC-3
		  // ---------
		  if( (m_SubDetector == "S2" || m_SubDetector == "ALL" )&& sam == 2){
		    // HEC3 
		    nHEC3++;
		    if( region == 0 ){nHEC3r0++;}
		    if( region == 1 ){nHEC3r1++;}		    
		    if( nHEC3 == nOLD+100 ){
		      ATH_MSG_INFO ( "[OFFLINE->ELECTRODE] Processing cell " << nHEC3 << " in HEC " );
		      nOLD = nHEC3;
		    }    
		    // Test of getCellElectrodeIdVec()
		    std::vector<HWIdentifier> IdVec;
		    hvmap->getElectrodeInCell( offId, IdVec );
		    std::vector<HWIdentifier> electrodeIdVec = IdVec;
		    nelHEC3+= electrodeIdVec.size();
		    if( region == 0 ){nelHEC3r0 +=electrodeIdVec.size();}
		    if( region == 1 ){nelHEC3r1 +=electrodeIdVec.size();}
		    // Loop over vector of electrodes 
		    std::vector<HWIdentifier>::const_iterator hv = electrodeIdVec.begin();
		    std::vector<HWIdentifier>::const_iterator hvEnd = electrodeIdVec.end();
		    for(; hv!=hvEnd;++hv){
		      HWIdentifier electrodeId = *hv;
		      int DET = m_electrodeHelper->detector( electrodeId );
		      int SID = m_electrodeHelper->zside( electrodeId );
		      int MOD = m_electrodeHelper->module( electrodeId );
		      int PHI = m_electrodeHelper->hv_phi( electrodeId );
		      int ETA = m_electrodeHelper->hv_eta( electrodeId );
		      int GAP = m_electrodeHelper->gap( electrodeId );
		      int ELE = m_electrodeHelper->electrode( electrodeId );		   
		      fileHVHECOFF 
			<< "OfflineID=" 
			<< m_onlineHelper->show_to_string(offId) 
			<< " OnlineID="
			<< m_onlineHelper->show_to_string(onId) 
			<< " electrodeID=[" 
			<< DET << "." 
			<< SID << "." 
			<< MOD << "." 
			<< PHI << "." 
			<< ETA << "." 
			<< GAP << "." 
			<< ELE << "]" 
			<< std::endl;
		    }
		  }
		  // ---------
		  //   HEC-0
		  // ---------
		  if( (m_SubDetector == "S0" || m_SubDetector == "ALL" )&& sam == 0){
		    // HEC0 
		    nHEC0++;
		    if( region == 0 ){nHEC0r0++;}
		    if( region == 1 ){nHEC0r1++;}		    
		    if( nHEC0 == nOLD+100 ){
		      ATH_MSG_INFO ( "[OFFLINE->ELECTRODE] Processing cell " << nHEC0 << " in HEC " );
		      nOLD = nHEC0;
		    }    
		    // Test of getCellElectrodeIdVec()
		    std::vector<HWIdentifier> IdVec;
		    hvmap->getElectrodeInCell( offId, IdVec );
		    std::vector<HWIdentifier> electrodeIdVec = IdVec;
		    nelHEC0+= electrodeIdVec.size();
		    if( region == 0 ){nelHEC0r0 +=electrodeIdVec.size();}
		    if( region == 1 ){nelHEC0r1 +=electrodeIdVec.size();}
		    // Loop over vector of electrodes 
		    std::vector<HWIdentifier>::const_iterator hv = electrodeIdVec.begin();
		    std::vector<HWIdentifier>::const_iterator hvEnd = electrodeIdVec.end();
		    for(; hv!=hvEnd;++hv){
		      HWIdentifier electrodeId = *hv;
		      int DET = m_electrodeHelper->detector( electrodeId );
		      int SID = m_electrodeHelper->zside( electrodeId );
		      int MOD = m_electrodeHelper->module( electrodeId );
		      int PHI = m_electrodeHelper->hv_phi( electrodeId );
		      int ETA = m_electrodeHelper->hv_eta( electrodeId );
		      int GAP = m_electrodeHelper->gap( electrodeId );
		      int ELE = m_electrodeHelper->electrode( electrodeId );		   
		      fileHVHECOFF 
			<< "OfflineID=" 
			<< m_onlineHelper->show_to_string(offId) 
			<< " OnlineID="
			<< m_onlineHelper->show_to_string(onId) 
			<< " electrodeID=[" 
			<< DET << "." 
			<< SID << "." 
			<< MOD << "." 
			<< PHI << "." 
			<< ETA << "." 
			<< GAP << "." 
			<< ELE << "]" 
			<< std::endl;
		    }
		  }
		  // ---------
		  //   HEC-0
		  // ---------
		  if( (m_SubDetector == "ALL" )){
		    // HEC - ALL
		    nHEC++;
		    if( nHEC == nOLD+100 ){
		      ATH_MSG_INFO ( "[OFFLINE->ELECTRODE] Processing cell " << nHEC << " in HEC " );
		      nOLD = nHEC;
		    }    
		    // Test of getCellElectrodeIdVec()
		    std::vector<HWIdentifier> IdVec;
		    hvmap->getElectrodeInCell( offId, IdVec );
		    std::vector<HWIdentifier> electrodeIdVec = IdVec;
		    nelHEC+= electrodeIdVec.size();
		  }

		}
	      }
	  }

	if( m_Detector == "FCAL" || m_Detector == "ALL" ) 
	  {// if m_Detector == FCAL
	    std::vector<Identifier>::const_iterator itId = m_fcalHelper->fcal_begin();
	    std::vector<Identifier>::const_iterator itIdEnd = m_fcalHelper->fcal_end();
	    for(; itId!=itIdEnd;++itId )
	      {// Loop over FCAL channels
		nFCAL++;
		const Identifier offId = *itId;
		int bec   = m_fcalHelper->pos_neg( offId);
		//int mod   = m_fcalHelper->module( offId);
		//int Side  = -9;
		if(( abs(bec) == 2)){
		  //if( bec > 0 ){ 
		  //  Side = 0;}
                  //else{
		  //Side = 1;
                  //}  
		  if( nFCAL == nOLD+100 ){
		    ATH_MSG_INFO ( "[OFFLINE->ELECTRODE] Processing cell " << nFCAL << " in FCAL " );
		    nOLD = nFCAL;
		  }
		  
		  // Test of getCellElectrodeIdVec()
		  std::vector<HWIdentifier> IdVec;
		  hvmap->getElectrodeInCell( offId, IdVec );
		  std::vector<HWIdentifier> electrodeIdVec = IdVec;
		  
		  // Loop over vector of electrodes 
		  std::vector<HWIdentifier>::const_iterator hv = electrodeIdVec.begin();
		  std::vector<HWIdentifier>::const_iterator hvEnd = electrodeIdVec.end();
		  nelFCAL += electrodeIdVec.size();
		  for(; hv!=hvEnd;++hv){
		    HWIdentifier electrodeId = *hv;
		    int DET = m_electrodeHelper->detector( electrodeId );
		    int SID = m_electrodeHelper->zside( electrodeId );
		    int MOD = m_electrodeHelper->module( electrodeId );
		    int PHI = m_electrodeHelper->hv_phi( electrodeId );
		    int ETA = m_electrodeHelper->hv_eta( electrodeId );
		    int GAP = m_electrodeHelper->gap( electrodeId );
		    int ELE = m_electrodeHelper->electrode( electrodeId );		   
		    fileHVFCALOFF 
		      << "OfflineID " 
		      << m_onlineHelper->show_to_string(offId) 
		      << " electrodeID=[" 
		      << DET << "." 
		      << SID << "." 
		      << MOD << "." 
		      << PHI << "." 
		      << ETA << "." 
		      << GAP << "." 
		      << ELE << "]" 
		      << std::endl;
		  }
		}
	      }
	  }
	ATH_MSG_INFO  ( "============================================================================ " );
	ATH_MSG_INFO  ( " [OFFLINE->ELECTRODE] :  List of electrodeId " );
	ATH_MSG_INFO  ( "============================================================================ " );
	if( m_Detector == "EMB" || m_Detector == "ALL" ){
	  if( m_SubDetector == "ALL"){
	    ATH_MSG_INFO  ( " - Offline channels in EMB :" << nEMB );
	    ATH_MSG_INFO  ( "    - nb electrodes in EMB :" << nelEMB );
	  }
	  if( m_SubDetector == "S0" || m_SubDetector == "ALL"){
	    ATH_MSG_INFO  ( "    - nb offline   in EMB0:" << nEMB0 );
	    ATH_MSG_INFO  ( "    - nb electrode in EMB0:" << nelEMB0 );
	  }
	  if( m_SubDetector == "S1" || m_SubDetector == "ALL"){
	    ATH_MSG_INFO  ( "    - nb offline   in EMB1:" << nEMB1 );
	    ATH_MSG_INFO  ( "    - nb electrode in EMB1:" << nelEMB1 );
	    ATH_MSG_INFO  ( "      - region 0 N(cell)= " << nEMB1r0 
                            << " N(el)= " << nelEMB1r0 );
	    ATH_MSG_INFO  ( "      - region 1 N(cell)= " << nEMB1r1 
                            << " N(el)= " << nelEMB1r1 );
	  }
	  if( m_SubDetector == "S2" || m_SubDetector == "ALL"){
	    ATH_MSG_INFO  ( "    - nb offline   in EMB2:" << nEMB2 );
	    ATH_MSG_INFO  ( "    - nb electrode in EMB2:" << nelEMB2 );
	    ATH_MSG_INFO  ( "      - region 0 N(cell)= " << nEMB2r0 
                            << " N(el)= " << nelEMB2r0 );
	    ATH_MSG_INFO  ( "      - region 1 N(cell)= " << nEMB2r1 
                            << " N(el)= " << nelEMB2r1 );
	  }
	  if( m_SubDetector == "S3" || m_SubDetector == "ALL"){
	    ATH_MSG_INFO  ( "    - nb offline   in EMB3:" << nEMB3 );
	    ATH_MSG_INFO  ( "    - nb electrode in EMB3:" << nelEMB3 );
	  }
	}
	if( m_Detector == "EMEC" || m_Detector == "ALL" ){
	  if( m_SubDetector == "ALL"){
	    ATH_MSG_INFO  ( " - Offline channels in EMEC:" << nEMEC );
	    ATH_MSG_INFO  ( "    - nb electrodes in EMEC:" << nelEMEC );
	  }
	  if( m_SubDetector == "S1" || m_SubDetector == "ALL"){
	    ATH_MSG_INFO  ( "    - nb offline   in EMEC1:" << nEMEC1 );
	    ATH_MSG_INFO  ( "    - nb electrode in EMEC1:" << nelEMEC1 );
	    ATH_MSG_INFO  ( "      - OUTER region 0 N(cell)= " << nEMEC1r0 
                            << " N(el)= " << nelEMEC1r0 );
	    ATH_MSG_INFO  ( "      - OUTER region 1 N(cell)= " << nEMEC1r1 
                            << " N(el)= " << nelEMEC1r1 );
	    ATH_MSG_INFO  ( "      - OUTER region 2 N(cell)= " << nEMEC1r2 
                            << " N(el)= " << nelEMEC1r2 );
	    ATH_MSG_INFO  ( "      - OUTER region 3 N(cell)= " << nEMEC1r3 
                            << " N(el)= " << nelEMEC1r3 );
	    ATH_MSG_INFO  ( "      - OUTER region 4 N(cell)= " << nEMEC1r4 
                            << " N(el)= " << nelEMEC1r4 );
	    ATH_MSG_INFO  ( "      - OUTER region 5 N(cell)= " << nEMEC1r5 
                            << " N(el)= " << nelEMEC1r5 );
	    ATH_MSG_INFO  ( "      - INNER region 0 N(cell)= " << nEMEC1r0IN 
                            << " N(el)= " << nelEMEC1r0IN );

	  }
	  if( m_SubDetector == "S2" || m_SubDetector == "ALL"){
	    ATH_MSG_INFO  ( "    - nb offline   in EMEC2:" << nEMEC2 );
	    ATH_MSG_INFO  ( "    - nb electrode in EMEC2:" << nelEMEC2 );
	    ATH_MSG_INFO  ( "      - OUTER region 0 N(cell)= " << nEMEC2r0 
                            << " N(el)= " << nelEMEC2r0 );
	    ATH_MSG_INFO  ( "      - OUTER region 1 N(cell)= " << nEMEC2r1 
                            << " N(el)= " << nelEMEC2r1 );
	    ATH_MSG_INFO  ( "      - INNER region 0 N(cell)= " << nEMEC2r0IN 
                            << " N(el)= " << nelEMEC2r0IN );

	  }
	  if( m_SubDetector == "S3" || m_SubDetector == "ALL"){
	    ATH_MSG_INFO  ( "    - nb offline   in EMEC3:" << nEMEC3 );
	    ATH_MSG_INFO  ( "    - nb electrode in EMEC3:" << nelEMEC3 );
	  }
	  if( m_SubDetector == "S0" || m_SubDetector == "ALL"){
	    ATH_MSG_INFO  ( "    - nb offline   in EMEC0:" << nEMEC0 );
	    ATH_MSG_INFO  ( "    - nb electrode in EMEC0:" << nelEMEC0 );
	  }
	}
	if( m_Detector == "HEC" || m_Detector == "ALL" ){
	  if( m_SubDetector == "S0" || m_SubDetector == "ALL"){
	    ATH_MSG_INFO  ( " - Offline channels in HEC0  :" << nHEC0 );
	    ATH_MSG_INFO  ( " - nb electrodes in HEC0     :" << nelHEC0 );
	    ATH_MSG_INFO  ( "      - region 0 N(cell)= " << nHEC0r0 
                            << " N(el)= " << nelHEC0r0 );
	    ATH_MSG_INFO  ( "      - region 1 N(cell)= " << nHEC0r1 
                            << " N(el)= " << nelHEC0r1 );
	  }
	  if( m_SubDetector == "S1" || m_SubDetector == "ALL"){
	    ATH_MSG_INFO  ( " - Offline channels in HEC1  :" << nHEC1 );
	    ATH_MSG_INFO  ( " - nb electrodes in HEC1     :" << nelHEC1 );
	    ATH_MSG_INFO  ( "      - region 0 N(cell)= " << nHEC1r0 
                            << " N(el)= " << nelHEC1r0 );
	    ATH_MSG_INFO  ( "      - region 1 N(cell)= " << nHEC1r1 
                            << " N(el)= " << nelHEC1r1 );
	  }
	  if( m_SubDetector == "S2" || m_SubDetector == "ALL"){
	    ATH_MSG_INFO  ( " - Offline channels in HEC2  :" << nHEC2 );
	    ATH_MSG_INFO  ( " - nb electrodes in HEC2     :" << nelHEC2 );
	    ATH_MSG_INFO  ( "      - region 0 N(cell)= " << nHEC2r0 
                            << " N(el)= " << nelHEC2r0 );
	    ATH_MSG_INFO  ( "      - region 1 N(cell)= " << nHEC2r1 
                            << " N(el)= " << nelHEC2r1 );
	  }
	  if( m_SubDetector == "S3" || m_SubDetector == "ALL"){
	    ATH_MSG_INFO  ( " - Offline channels in HEC3  :" << nHEC3 );
	    ATH_MSG_INFO  ( " - nb electrodes in HEC3     :" << nelHEC3 );
	    ATH_MSG_INFO  ( "      - region 0 N(cell)= " << nHEC3r0 
                            << " N(el)= " << nelHEC3r0 );
	    ATH_MSG_INFO  ( "      - region 1 N(cell)= " << nHEC3r1 
                            << " N(el)= " << nelHEC3r1 );
	  }


	}
	if( m_Detector == "FCAL" || m_Detector == "ALL" ){
	  ATH_MSG_INFO  ( " - Offline channels in FCAL :" << nFCAL );
	  ATH_MSG_INFO  ( "    - nb electrodes in FCAL :" << nelFCAL );
	}
	if( m_Detector == "ALL"){
	  ATH_MSG_INFO  ( "============================================================================ " );
	  //log << MSG::INFO  << "  -- TOTAL    : " << nelDET << endmsg; 
          ATH_MSG_INFO  ( "============================================================================ " );
	}
      } // m_HVelectrodeToOffline == ON
    ATH_MSG_INFO  ( "============================================================================ " );



    // ==============================================    
    // ==============================================
    // ==============================================
    //   Test of OfflineID <--> HVLINEID Identifiers 
    // ==============================================
    // ==============================================
    // ==============================================
    
    
    if( m_OfflineTest == "ON" && m_HVlineToOffline == "ON" )   
      {// if m_HVLineToOffline 
	int nEMB1  = 0;
	int nEMB1r0= 0;
	int nEMB1r1= 0;
	int nEMB2  = 0;
	int nEMB2r0= 0;
	int nEMB2r1= 0;
	int nEMB3  = 0;
	int nEMB0  = 0;
	int nEMB   = 0;

	int nelEMB   = 0;
	int nelEMB0  = 0;
	int nelEMB1  = 0;
	int nelEMB1r0= 0;
	int nelEMB1r1= 0;
	int nelEMB2  = 0;
	int nelEMB2r0= 0;
	int nelEMB2r1= 0;
	int nelEMB3  = 0;
 
	int nEMEC  = 0;
	int nEMEC0 = 0;
	int nEMEC1  = 0;
	int nEMEC1r0 = 0;
	int nEMEC1r0IN= 0;
	int nEMEC1r1 = 0;
	int nEMEC1r2 = 0;
	int nEMEC1r3 = 0;
	int nEMEC1r4 = 0;
	int nEMEC1r5 = 0;
	int nEMEC2     = 0;
	int nEMEC2r0   = 0;
	int nEMEC2r0IN = 0;
	int nEMEC2r1   = 0;
	int nEMEC3 = 0;
	int nelEMEC = 0;
	int nelEMEC0= 0;
	int nelEMEC1= 0;
	int nelEMEC1r0 = 0;
	int nelEMEC1r0IN= 0;
	int nelEMEC1r1 = 0;
	int nelEMEC1r2 = 0;
	int nelEMEC1r3 = 0;
	int nelEMEC1r4 = 0;
	int nelEMEC1r5 = 0;
	int nelEMEC2     = 0;
	int nelEMEC2r0   = 0;
	int nelEMEC2r1   = 0;
	int nelEMEC2r0IN = 0;
	int nelEMEC3= 0;

	int nHEC   = 0;
	int nHEC0  = 0;
	int nHEC0r0= 0;
	int nHEC0r1= 0;
	int nHEC1  = 0;
	int nHEC1r0= 0;
	int nHEC1r1= 0;
	int nHEC2  = 0;
	int nHEC2r0= 0;
	int nHEC2r1= 0;
	int nHEC3  = 0;
	int nHEC3r0= 0;
	int nHEC3r1= 0;

	int nelHEC  = 0;
	int nelHEC0  = 0;
	int nelHEC0r0= 0;
	int nelHEC0r1= 0;
	int nelHEC1  = 0;
	int nelHEC1r0= 0;
	int nelHEC1r1= 0;
	int nelHEC2  = 0;
	int nelHEC2r0= 0;
	int nelHEC2r1= 0;
	int nelHEC3  = 0;
	int nelHEC3r0= 0;
	int nelHEC3r1= 0;


	int nFCAL  = 0;
	int nelFCAL = 0;
	int nOLD    = 0;
	
	std::vector<HWIdentifier> hvRefFCALvec;

	if( m_Detector == "EMB" || m_Detector == "ALL" ) 
	  {// if m_Detector == EM
	    std::vector<Identifier>::const_iterator itId = m_emHelper->em_begin();
	    std::vector<Identifier>::const_iterator itIdEnd = m_emHelper->em_end();
	    for(; itId!=itIdEnd;++itId )
	      {// Loop over EM connected channels
		const Identifier offId = *itId;
		int bec   = m_emHelper->barrel_ec( offId);
		int sam   = m_emHelper->sampling( offId);
		int region= m_emHelper->region( offId);
		//int Side  = -9;
		if( abs(bec) == 1 ){
		  //if( bec > 0 ){ 
		  //  Side = 0;}
		  //else{Side = 1;}
		  //------------------
		  //    EMB--PS
		  // ------------------
		  if( (m_SubDetector == "S0"|| m_SubDetector == "ALL" ) && sam == 0 ){
		    nEMB0++;
		    if( nEMB0 == nOLD+100 ){
		      ATH_MSG_INFO ( "[OFFLINE->HVLINE] Processing cell " << nEMB0 
                                     << " in EMB0 " );
		      nOLD = nEMB0;
		    }
		    // Test of getCellElectrodeIdVec()
		    std::vector<HWIdentifier> IdVec;
		    hvmap->getHVLineInCell( offId, IdVec );
		    std::vector<HWIdentifier> hvlineIdVec = IdVec;
		    nelEMB0+= hvlineIdVec.size();
		    // Loop over vector of electrodes 
		    std::vector<HWIdentifier>::const_iterator hv = hvlineIdVec.begin();
		    std::vector<HWIdentifier>::const_iterator hvEnd = hvlineIdVec.end();
		    ATH_MSG_DEBUG ( "[TestLArHW] hvlineIdSize=" << hvlineIdVec.size() );
		    fileHVEMBOFF 
		      << "OfflineID " 
		      << m_onlineHelper->show_to_string(offId);
		    for(; hv!=hvEnd;++hv){
		      HWIdentifier hvlineId = *hv;
		      int PART= m_hvHelper->partition( hvlineId );
		      int CANL= m_hvHelper->can_line( hvlineId );
		      int CANN= m_hvHelper->can_node( hvlineId );
		      int LINE= m_hvHelper->hv_line( hvlineId );
		      // Loop over vector of electrodes 
		      fileHVEMBOFF 
			<< " HVlineID=["
			<< PART << "." 
			<< CANL << "." 
			<< CANN << "." 
			<< LINE << "]";
		    }
		    fileHVEMBOFF << std::endl;
		  }
		  //------------------
		  //    EMB--EM1
		  // ------------------
		  if( (m_SubDetector == "S1"|| m_SubDetector == "ALL" ) && sam == 1 ){
		    nEMB1++;
		    if( region == 0 ){nEMB1r0++;}
		    if( region == 1 ){nEMB1r1++;}
		    if( nEMB1 == nOLD+100 ){
		      ATH_MSG_INFO ( "[OFFLINE->HVLINE] Processing cell " << nEMB1 
                                     << " in EMB1 " );
		      nOLD = nEMB1;
		    }
		    // Test of getCellElectrodeIdVec()
		    std::vector<HWIdentifier> IdVec;
		    hvmap->getHVLineInCell( offId, IdVec );
		    std::vector<HWIdentifier> hvlineIdVec = IdVec;
		    nelEMB1+= hvlineIdVec.size();
		    if( region == 0 ){nelEMB1r0+=hvlineIdVec.size();}
		    if( region == 1 ){nelEMB1r1+=hvlineIdVec.size();}
		    // Loop over vector of electrodes 
		    std::vector<HWIdentifier>::const_iterator hv = hvlineIdVec.begin();
		    std::vector<HWIdentifier>::const_iterator hvEnd = hvlineIdVec.end();
		    ATH_MSG_DEBUG ( "[TestLArHW] hvlineIdSize=" << hvlineIdVec.size() );
		    fileHVEMBOFF 
		      << "OfflineID " 
		      << m_onlineHelper->show_to_string(offId);
		    for(; hv!=hvEnd;++hv){
		      HWIdentifier hvlineId = *hv;
		      int PART= m_hvHelper->partition( hvlineId );
		      int CANL= m_hvHelper->can_line( hvlineId );
		      int CANN= m_hvHelper->can_node( hvlineId );
		      int LINE= m_hvHelper->hv_line( hvlineId );
		      // Loop over vector of electrodes 
		      fileHVEMBOFF 
			<< " HVlineID=["
			<< PART << "." 
			<< CANL << "." 
			<< CANN << "." 
			<< LINE << "]";
		    }
		    fileHVEMBOFF << std::endl;
		  }
		  // -----------------
		  //    EMB--EM2
		  // ------------------
		  if( (m_SubDetector == "S2"|| m_SubDetector == "ALL" ) && ( sam == 2 )){
		    nEMB2++;
		    if( region == 0 ){nEMB2r0++;}
		    if( region == 1 ){nEMB2r1++;}
		    if( nEMB2 == nOLD+100 ){
		      ATH_MSG_INFO ( "[OFFLINE->HVLINE] Processing cell " 
                                     << nEMB2 << " in EMB2 " );
		      nOLD = nEMB2;
		    }
		    // Test of getCellElectrodeIdVec()
		    std::vector<HWIdentifier> IdVec;
		    hvmap->getHVLineInCell( offId, IdVec );
		    std::vector<HWIdentifier> hvlineIdVec = IdVec;
		    // Loop over vector of electrodes 
		    std::vector<HWIdentifier>::const_iterator hv = hvlineIdVec.begin();
		    std::vector<HWIdentifier>::const_iterator hvEnd = hvlineIdVec.end();
		    nelEMB2 += hvlineIdVec.size();
		    if( region == 0 ){nelEMB2r0+=hvlineIdVec.size();}
		    if( region == 1 ){nelEMB2r1+=hvlineIdVec.size();}
		    for(; hv!=hvEnd;++hv){
		      HWIdentifier hvlineId = *hv;
		      int PART= m_hvHelper->partition( hvlineId );
		      int CANL= m_hvHelper->can_line( hvlineId );
		      int CANN= m_hvHelper->can_node( hvlineId );
		      int LINE= m_hvHelper->hv_line( hvlineId );
		      // Loop over vector of electrodes 
		      fileHVEMBOFF 
			<< "OfflineID " 
			<< m_onlineHelper->show_to_string(offId) 
			<< " HVlineID=["
			<< PART << "." 
			<< CANL << "." 
			<< CANN << "." 
			<< LINE << "]" 
			<< std::endl;
		    }
		  }
		  // -----------------
		  //     EMB--EM3
		  // ------------------
		  if( (m_SubDetector == "S3" || m_SubDetector == "ALL" ) && ( sam == 3 )){
		    nEMB3++;
		    if( nEMB3 == nOLD+100 ){
		      ATH_MSG_INFO ( "[OFFLINE->HVLINE] Processing cell " << nEMB3 
                                     << " in EMB3 " );
		      nOLD = nEMB3;
		    }
		    // Test of getCellElectrodeIdVec()
		    std::vector<HWIdentifier> IdVec;
		    ATH_MSG_DEBUG ( "Processing offline Cell=" 
                                    << m_onlineHelper->show_to_string( offId ) << "..-> getHVLine()"
                                    );
		    hvmap->getHVLineInCell( offId, IdVec );
		    std::vector<HWIdentifier> hvlineIdVec = IdVec;
		    // Loop over vector of electrodes 
		    std::vector<HWIdentifier>::const_iterator hv = hvlineIdVec.begin();
		    std::vector<HWIdentifier>::const_iterator hvEnd = hvlineIdVec.end();
		    nelEMB3 += hvlineIdVec.size();
		    ATH_MSG_DEBUG ( "[TestLArHW] hvlineIdSize=" << hvlineIdVec.size() );
		    for(; hv!=hvEnd;++hv){
		      HWIdentifier hvlineId = *hv;
		      int PART= m_hvHelper->partition( hvlineId );
		      int CANL= m_hvHelper->can_line( hvlineId );
		      int CANN= m_hvHelper->can_node( hvlineId );
		      int LINE= m_hvHelper->hv_line( hvlineId );
		      // Loop over vector of electrodes 
		      fileHVEMBOFF 
			<< "OfflineID " 
			<< m_onlineHelper->show_to_string(offId) 
			<< " HVlineID=["
			<< PART << "." 
			<< CANL << "." 
			<< CANN << "." 
			<< LINE << "]" 
			<< std::endl;
		    }
		  }
		  // ------------------
		  //    EMB--ALL
		  // ------------------
		  if( m_SubDetector == "ALL" ){
		    nEMB++;
		    // Test of getCellElectrodeIdVec()
		    std::vector<HWIdentifier> IdVec;
		    hvmap->getHVLineInCell( offId, IdVec );
		    std::vector<HWIdentifier> hvlineIdVec = IdVec;
		    nelEMB += hvlineIdVec.size();      
		  }
		}
	      }
	  }

	if( m_Detector == "EMEC" || m_Detector == "ALL" ) 
	  {// if m_Detector == EMEC
	    std::vector<Identifier>::const_iterator itId = m_emHelper->em_begin();
	    std::vector<Identifier>::const_iterator itIdEnd = m_emHelper->em_end();
	    for(; itId!=itIdEnd;++itId )
	      {// Loop over EM connected channels
		const Identifier offId = *itId;
		int bec   = m_emHelper->barrel_ec( offId);
		int sam   = m_emHelper->sampling( offId);
		int region= m_emHelper->region( offId);
		//int Side  = -9;
		if( abs(bec) == 2 || abs(bec) == 3 ){
		  //if( bec > 0 ){ 
		  //  Side = 0;}
		  //else{Side = 1;}
		}
		// -------------
		// Test of EMECPS
		// -------------
		if( (m_SubDetector == "S0"|| m_SubDetector == "ALL" ) && (abs(bec) == 2 || abs(bec) == 3) ){
		  // m_SubDetector == EMEC0
		  if( sam == 0 ){
		    nEMEC0++;		      
		    // Test of individual methods:
		    if( nEMEC0 == nOLD+100 ){
		      ATH_MSG_INFO ( "[OFFLINE->HVLINE] Processing  cell " 
                                     << nEMEC0 << " in EMEC1 " );
		      nOLD = nEMEC0;
		    }		      
		    // Test of getCellElectrodeIdVec()
		    std::vector<HWIdentifier> IdVec;
		    hvmap->getHVLineInCell( offId, IdVec );
		    std::vector<HWIdentifier> hvlineIdVec = IdVec;
		    // Loop over vector of electrodes 
		    std::vector<HWIdentifier>::const_iterator hv = hvlineIdVec.begin();
		    std::vector<HWIdentifier>::const_iterator hvEnd = hvlineIdVec.end();
		    nelEMEC0+=hvlineIdVec.size();
		    for(; hv!=hvEnd;++hv){
		      HWIdentifier hvlineId = *hv;
		      int PART= m_hvHelper->partition( hvlineId );
		      int CANL= m_hvHelper->can_line( hvlineId );
		      int CANN= m_hvHelper->can_node( hvlineId );
		      int LINE= m_hvHelper->hv_line( hvlineId );
		      fileHVEMECOFF 
			<< "OfflineID " 
			<< m_onlineHelper->show_to_string(offId) 
			<< " HVlineID=["
			<< PART << "." 
			<< CANL << "." 
			<< CANN << "." 
			<< LINE << "]" 
			<< std::endl;
		    }
		  }
		}
		
		// -------------
		// Test of EMEC1
		// -------------
		if( ( m_SubDetector == "S1"|| m_SubDetector == "ALL" ) && ( abs(bec) == 2 || abs(bec) == 3 ) ){
		  // m_SubDetector == EMEC1
		  if( sam == 1 ){
		    nEMEC1++;		      
		    if( region == 0 && abs(bec) == 2 ){nEMEC1r0++;}
		    if( region == 0 && abs(bec) == 3 ){nEMEC1r0IN++;}
		    if( region == 1 ){nEMEC1r1++;}
		    if( region == 2 ){nEMEC1r2++;}
		    if( region == 3 ){nEMEC1r3++;}
		    if( region == 4 ){nEMEC1r4++;}
		    if( region == 5 ){nEMEC1r5++;}
		    // Test of individual methods:
		    if( nEMEC1 == nOLD+100 ){
		      ATH_MSG_INFO ( "[OFFLINE->HVLINE] Processing cell " << nEMEC1 << " in EMEC1 " );
		      nOLD = nEMEC1;
		    }
		    
		    // Test of getCellElectrodeIdVec()
		    std::vector<HWIdentifier> IdVec;
		    hvmap->getHVLineInCell( offId, IdVec );
		    std::vector<HWIdentifier> hvlineIdVec = IdVec;
		    // Loop over vector of electrodes 
		    std::vector<HWIdentifier>::const_iterator hv = hvlineIdVec.begin();
		    std::vector<HWIdentifier>::const_iterator hvEnd = hvlineIdVec.end();
		    nelEMEC1+=hvlineIdVec.size();
 		    if( region == 0 && abs(bec) == 2){nelEMEC1r0 +=hvlineIdVec.size();}
		    if( region == 0 && abs(bec) == 3){nelEMEC1r0IN +=hvlineIdVec.size();}
		    if( region == 1 ){nelEMEC1r1+=hvlineIdVec.size();}
		    if( region == 2 ){nelEMEC1r2+=hvlineIdVec.size();}
		    if( region == 3 ){nelEMEC1r3+=hvlineIdVec.size();}
		    if( region == 4 ){nelEMEC1r4+=hvlineIdVec.size();}
		    if( region == 5 ){nelEMEC1r5+=hvlineIdVec.size();}
		    for(; hv!=hvEnd;++hv){
		      HWIdentifier hvlineId = *hv;
		      int PART= m_hvHelper->partition( hvlineId );
		      int CANL= m_hvHelper->can_line( hvlineId );
		      int CANN= m_hvHelper->can_node( hvlineId );
		      int LINE= m_hvHelper->hv_line( hvlineId );
		      fileHVEMECOFF 
			<< "OfflineID " 
			<< m_onlineHelper->show_to_string(offId) 
			<< " HVlineID=["
			<< PART << "." 
			<< CANL << "." 
			<< CANN << "." 
			<< LINE << "]" 
			<< std::endl;
		    }
		  }
		}
		// --------------
		// Test of EMEC2
		// --------------
		if( (m_SubDetector == "S2" || m_SubDetector == "ALL" )&& ( abs(bec) == 2 && abs(bec) == 3 ) ){
		  // m_SubDetector == EMEC2
		  if( sam == 2 ){
		    // sam==2
		    nEMEC2++;
		    if( region == 0 && abs(bec) == 2 ){nEMEC2r0++;}
		    if( region == 0 && abs(bec) == 3 ){nEMEC2r0IN++;}
		    if( region == 1 ){nEMEC2r1++;}
		    // Test of individual methods:
		    if( nEMEC2 == nOLD+100 ){
		      ATH_MSG_INFO ( "[OFFLINE->HVLINE] Processing cell " << nEMEC2 << " in EMEC2 " );
		      nOLD = nEMEC2;
		    }
		    
		    // Test of getCellElectrodeIdVec()
		    std::vector<HWIdentifier> IdVec;
		    hvmap->getHVLineInCell( offId, IdVec );
		    std::vector<HWIdentifier> hvlineIdVec = IdVec;
		    // Loop over vector of electrodes 
		    std::vector<HWIdentifier>::const_iterator hv = hvlineIdVec.begin();
		    std::vector<HWIdentifier>::const_iterator hvEnd = hvlineIdVec.end();
		    nelEMEC2+=hvlineIdVec.size();
 		    if( region == 0 && abs(bec) == 2){nelEMEC2r0 +=hvlineIdVec.size();}
		    if( region == 0 && abs(bec) == 3){nelEMEC2r0IN +=hvlineIdVec.size();}
		    if( region == 1 ){nelEMEC2r1+=hvlineIdVec.size();}
		    for(; hv!=hvEnd;++hv){
		      HWIdentifier hvlineId = *hv;
		      int PART= m_hvHelper->partition( hvlineId );
		      int CANL= m_hvHelper->can_line( hvlineId );
		      int CANN= m_hvHelper->can_node( hvlineId );
		      int LINE= m_hvHelper->hv_line( hvlineId );
		      fileHVEMECOFF 
			<< "OfflineID " 
			<< m_onlineHelper->show_to_string(offId) 
			<< " HVlineID=["
			<< PART << "." 
			<< CANL << "." 
			<< CANN << "." 
			<< LINE << "]" 
			<< std::endl;
		    }
		  }
		}
		// --------------
		// Test of EMEC3
		// --------------
		if( (m_SubDetector == "S3" || m_SubDetector == "ALL" )&& ( abs(bec) == 2 && sam == 3) ){
		  // m_SubDetector == EMEC3
		  if( sam == 3){
		    // sam = 3
		    nEMEC3++;
		    // Test of individual methods:
		    if( nEMEC3 == nOLD+100 ){
		      ATH_MSG_INFO ( "[OFFLINE->HVLINE] Processing cell " << nEMEC3 << " in EMEC3" );
		      nOLD = nEMEC3;
		    }
		    
		    // Test of getCellElectrodeIdVec()
		    std::vector<HWIdentifier> IdVec;
		    hvmap->getHVLineInCell( offId, IdVec );
		    std::vector<HWIdentifier> hvlineIdVec = IdVec;
		    // Loop over vector of electrodes 
		    std::vector<HWIdentifier>::const_iterator hv = hvlineIdVec.begin();
		    std::vector<HWIdentifier>::const_iterator hvEnd = hvlineIdVec.end();
		    nelEMEC3+=hvlineIdVec.size();
		    fileHVEMECOFF 
		      << "OfflineID " 
		      << m_onlineHelper->show_to_string(offId); 
		    for(; hv!=hvEnd;++hv){
		      HWIdentifier hvlineId = *hv;
		      int PART= m_hvHelper->partition( hvlineId );
		      int CANL= m_hvHelper->can_line( hvlineId );
		      int CANN= m_hvHelper->can_node( hvlineId );
		      int LINE= m_hvHelper->hv_line( hvlineId );
		      fileHVEMECOFF 
			<< " HVlineID=["
			<< PART << "." 
			<< CANL << "." 
			<< CANN << "." 
			<< LINE << "]" ;
		    }
		    fileHVEMECOFF << std::endl;
		  }
		}
		// -----------------
		// Test of EMEC-ALL
		// -----------------
		if( m_SubDetector == "ALL" && ( abs(bec) == 2 || abs(bec) == 3) ){
		  // m_SubDetector == ALL
		  nEMEC++;
		  if( nEMEC == nOLD+100 ){
		    ATH_MSG_INFO ( "[OFFLINE->HVLINE] Processing cell " << nEMEC << " in EMEC " );
		    nOLD = nEMEC;
		  }    
		  // Test of getCellElectrodeIdVec()
		  std::vector<HWIdentifier> IdVec;
		  hvmap->getHVLineInCell( offId, IdVec );
		  std::vector<HWIdentifier> hvlineIdVec = IdVec;
		  nelEMEC+=hvlineIdVec.size();
		}
	      }// For Loop over EMEC channels
	  }// Loop over EMEC channels
  	
	//-------------------------
	// HEC
	//-------------------------
	if( m_Detector == "HEC" || m_Detector == "ALL" ) 
	  {// if m_Detector == HEC
	    std::vector<Identifier>::const_iterator itId = m_hecHelper->hec_begin();
	    std::vector<Identifier>::const_iterator itIdEnd = m_hecHelper->hec_end();
	    for(; itId!=itIdEnd;++itId )
	      {// Loop over HEC channels
		const Identifier offId = *itId;
		HWIdentifier onId = cabling->createSignalChannelID( offId ) ; 
		int bec   = m_hecHelper->pos_neg( offId);
		int sam   = m_hecHelper->sampling( offId);
		int region= m_hecHelper->region( offId);
		//int Side  = -9;
		if( abs(bec) == 2 ){
		  //if( bec > 0 ){ 
		  //  Side = 0;}
		  //else{Side = 1;}
		  // ---------
		  //   HEC-0
		  // ---------
		  if( (m_SubDetector == "S0" || m_SubDetector == "ALL" )&& sam == 0){
		    // HEC0 
		    nHEC0++;
		    if( region == 0 ){nHEC0r0++;}
		    if( region == 1 ){nHEC0r1++;}		    
		    if( nHEC0 == nOLD+100 ){
		      ATH_MSG_INFO ( "[OFFLINE->ELECTRODE] Processing cell " << nHEC0 << " in HEC " );
		      nOLD = nHEC0;
		    }    
		    // Test of getCellElectrodeIdVec()
		    std::vector<HWIdentifier> IdVec;
		    hvmap->getHVLineInCell( offId, IdVec );
		    std::vector<HWIdentifier> hvlineIdVec = IdVec;
		    nelHEC0+= hvlineIdVec.size();
		    if( region == 0 ){nelHEC0r0 +=hvlineIdVec.size();}
		    if( region == 1 ){nelHEC0r1 +=hvlineIdVec.size();}
		    // Loop over vector of electrodes 
		    std::vector<HWIdentifier>::const_iterator hv = hvlineIdVec.begin();
		    std::vector<HWIdentifier>::const_iterator hvEnd = hvlineIdVec.end();
		    // Get Module and HV eta sector info
		    int AMOD= hvmap->getCellModule( offId );
		    int AETA= hvmap->getCellEtaSector( offId );
		    int AMIN= (hvmap->getCellElectrodeMinMax( offId )).first;
		    int AMAX= (hvmap->getCellElectrodeMinMax( offId )).second;
		    int SID= m_hecHelper->pos_neg(offId);
		    std::string ASID;
		    if( SID > 0 ){ASID = "A";}
		    else{ASID = "C";}
		    std::string APART;
		    if(region == 0 ){APART = "OUTER";}
		    else{APART= "INNER";}
		    int IC=0;
		    for(; hv!=hvEnd;++hv){
		      HWIdentifier hvlineId = *hv;
		      int PART= m_hvHelper->partition( hvlineId );
		      int CANL= m_hvHelper->can_line( hvlineId );
		      int CANN= m_hvHelper->can_node( hvlineId );
		      int LINE= m_hvHelper->hv_line( hvlineId );
		      std::string PAD;
		      if( IC == 0 ){PAD = "PAD1";}
		      if( IC == 1 ){PAD = "EST1";}
		      if( IC == 2 ){PAD = "PAD2";}
		      if( IC == 3 ){PAD = "EST1";}
		      IC++;
		      fileHVHECOFF 
			<< "Front wheel HEC1 "
			<< APART << " " 
			<< ASID
			<< AMOD 
			<< " HV_eta=S" << AETA
			<< " Gap=[" << AMIN << "-" << AMAX << "] "
			<< PAD << " " 
			<< " OfflineID=" 
			<< m_onlineHelper->show_to_string(offId) 
			<< " OnlineID="
			<< m_onlineHelper->show_to_string(onId) 
			<< " HVlineID=["
			<< PART << "." 
			<< CANL << "." 
			<< CANN << "." 
			<< LINE << "]" 
			<< std::endl;
		    }
		  }
		  // ---------
		  //   HEC-1
		  // ---------
		  if( (m_SubDetector == "S1" || m_SubDetector == "ALL" )&& sam == 1){
		    // HEC1 
		    nHEC1++;
		    if( region == 0 ){nHEC1r0++;}
		    if( region == 1 ){nHEC1r1++;}		    
		    if( nHEC1 == nOLD+100 ){
		      ATH_MSG_INFO ( "[OFFLINE->ELECTRODE] Processing cell " << nHEC1 << " in HEC " );
		      nOLD = nHEC1;
		    }    
		    // Test of getCellElectrodeIdVec()
		    std::vector<HWIdentifier> IdVec;
		    hvmap->getHVLineInCell( offId, IdVec );
		    std::vector<HWIdentifier> hvlineIdVec = IdVec;
		    nelHEC1+= hvlineIdVec.size();
		    if( region == 0 ){nelHEC1r0 +=hvlineIdVec.size();}
		    if( region == 1 ){nelHEC1r1 +=hvlineIdVec.size();}
		    // Loop over vector of electrodes 
		    std::vector<HWIdentifier>::const_iterator hv = hvlineIdVec.begin();
		    std::vector<HWIdentifier>::const_iterator hvEnd = hvlineIdVec.end();
		    // Get Module and HV eta sector info
		    int AMOD= hvmap->getCellModule( offId );
		    int AETA= hvmap->getCellEtaSector( offId );
		    int AMIN= (hvmap->getCellElectrodeMinMax( offId )).first;
		    int AMAX= (hvmap->getCellElectrodeMinMax( offId )).second;
		    int SID= m_hecHelper->pos_neg(offId);
		    std::string ASID;
		    if( SID > 0 ){ASID = "A";}
		    else{ASID = "C";}
		    std::string APART;
		    if(region == 0 ){APART = "OUTER";}
		    else{APART = "INNER";}
		    int IC = 0;
		    for(; hv!=hvEnd;++hv){
		      HWIdentifier hvlineId = *hv;
		      int PART= m_hvHelper->partition( hvlineId );
		      int CANL= m_hvHelper->can_line( hvlineId );
		      int CANN= m_hvHelper->can_node( hvlineId );
		      int LINE= m_hvHelper->hv_line( hvlineId );
		      std::string PAD;
		      if( IC == 0 ){PAD = "PAD1";}
		      if( IC == 1 ){PAD = "EST1";}
		      if( IC == 2 ){PAD = "PAD2";}
		      if( IC == 3 ){PAD = "EST1";}
		      IC++;
		      fileHVHECOFF 
			<< "Rear wheel HEC1 "
			<< APART << " "  
			<< ASID
			<< AMOD 
			<< " HV_eta=S" << AETA
			<< " Gap=[" << AMIN << "-" << AMAX << "] "
			<< PAD << " " 
			<< " OfflineID=" 
			<< m_onlineHelper->show_to_string(offId) 
			<< " OnlineID="
			<< m_onlineHelper->show_to_string(onId) 
			<< " HVlineID=["
			<< PART << "." 
			<< CANL << "." 
			<< CANN << "." 
			<< LINE << "]" 
			<< std::endl;
		    }
		  }
		  // ---------
		  //   HEC-2
		  // ---------
		  if( (m_SubDetector == "S2" || m_SubDetector == "ALL" )&& sam == 2){
		    // HEC2 
		    nHEC2++;
		    if( region == 0 ){nHEC2r0++;}
		    if( region == 1 ){nHEC2r1++;}		    
		    if( nHEC2 == nOLD+100 ){
		      ATH_MSG_INFO ( "[OFFLINE->ELECTRODE] Processing cell " << nHEC2 << " in HEC " );
		      nOLD = nHEC2;
		    }    
		    // Test of getCellElectrodeIdVec()
		    std::vector<HWIdentifier> IdVec;
		    hvmap->getHVLineInCell( offId, IdVec );
		    std::vector<HWIdentifier> hvlineIdVec = IdVec;
		    nelHEC2+= hvlineIdVec.size();
		    if( region == 0 ){nelHEC2r0 +=hvlineIdVec.size();}
		    if( region == 1 ){nelHEC2r1 +=hvlineIdVec.size();}
		    // Loop over vector of electrodes 
		    std::vector<HWIdentifier>::const_iterator hv = hvlineIdVec.begin();
		    std::vector<HWIdentifier>::const_iterator hvEnd = hvlineIdVec.end();
		    // Get Module and HV eta sector info
		    int AMOD= hvmap->getCellModule( offId );
		    int AETA= hvmap->getCellEtaSector( offId );
		    int AMIN= (hvmap->getCellElectrodeMinMax( offId )).first;
		    int AMAX= (hvmap->getCellElectrodeMinMax( offId )).second;
		    int SID= m_hecHelper->pos_neg(offId);
		    std::string ASID;
		    if( SID > 0 ){ASID = "A";}
		    else{ASID = "C";}
		    std::string APART;
		    if(region == 0 ){APART = "OUTER";}
		    else{APART= "INNER";}
		    int IC = 0;
		    for(; hv!=hvEnd;++hv){
		      HWIdentifier hvlineId = *hv;
		      int PART= m_hvHelper->partition( hvlineId );
		      int CANL= m_hvHelper->can_line( hvlineId );
		      int CANN= m_hvHelper->can_node( hvlineId );
		      int LINE= m_hvHelper->hv_line( hvlineId );
		      std::string PAD;
		      if( IC == 0 ){PAD = "PAD1";}
		      if( IC == 1 ){PAD = "EST1";}
		      if( IC == 2 ){PAD = "PAD2";}
		      if( IC == 3 ){PAD = "EST1";}
		      IC++;
		      fileHVHECOFF 
			<< "Front wheel of HEC2 "
			<< APART << " " 
			<< ASID
			<< AMOD 
			<< " HV_eta=S" << AETA
			<< " Gap=[" << AMIN << "-" << AMAX << "] "
			<< PAD << " " 
			<< " OfflineID=" 
			<< m_onlineHelper->show_to_string(offId) 
			<< " OnlineID="
			<< m_onlineHelper->show_to_string(onId) 
			<< " HVlineID=["
			<< PART << "." 
			<< CANL << "." 
			<< CANN << "." 
			<< LINE << "]" 
			<< std::endl;
		    }
		  }

		  // ---------
		  //   HEC-3
		  // ---------
		  if( (m_SubDetector == "S3" || m_SubDetector == "ALL" )&& sam == 3){
		    // HEC3 
		    nHEC3++;
		    if( region == 0 ){nHEC3r0++;}
		    if( region == 1 ){nHEC3r1++;}		    
		    if( nHEC3 == nOLD+100 ){
		      ATH_MSG_INFO ( "[OFFLINE->ELECTRODE] Processing cell " << nHEC3 << " in HEC " );
		      nOLD = nHEC3;
		    }    
		    // Test of getCellElectrodeIdVec()
		    std::vector<HWIdentifier> IdVec;
		    hvmap->getHVLineInCell( offId, IdVec );
		    std::vector<HWIdentifier> hvlineIdVec = IdVec;
		    nelHEC3+= hvlineIdVec.size();
		    if( region == 0 ){nelHEC3r0 +=hvlineIdVec.size();}
		    if( region == 1 ){nelHEC3r1 +=hvlineIdVec.size();}
		    // Loop over vector of electrodes 
		    std::vector<HWIdentifier>::const_iterator hv = hvlineIdVec.begin();
		    std::vector<HWIdentifier>::const_iterator hvEnd = hvlineIdVec.end();
		    // Get Module and HV eta sector info
		    int AMOD= hvmap->getCellModule( offId );
		    int AETA= hvmap->getCellEtaSector( offId );
		    int AMIN= (hvmap->getCellElectrodeMinMax( offId )).first;
		    int AMAX= (hvmap->getCellElectrodeMinMax( offId )).second;
		    int SID= m_hecHelper->pos_neg(offId);
		    std::string ASID;
		    if( SID > 0 ){ASID = "A";}
		    else{ASID = "C";}
		    std::string APART;
		    if(region == 0 ){APART = "OUTER";}
		    else{APART= "INNER";}
		    int IC = 0;
		    for(; hv!=hvEnd;++hv){
		      HWIdentifier hvlineId = *hv;
		      int PART= m_hvHelper->partition( hvlineId );
		      int CANL= m_hvHelper->can_line( hvlineId );
		      int CANN= m_hvHelper->can_node( hvlineId );
		      int LINE= m_hvHelper->hv_line( hvlineId );
		      std::string PAD;
		      if( IC == 0 ){PAD = "PAD1";}
		      if( IC == 1 ){PAD = "EST1";}
		      if( IC == 2 ){PAD = "PAD2";}
		      if( IC == 3 ){PAD = "EST1";}
		      IC++;
		      fileHVHECOFF 
			<< "Rear wheel of HEC2 "
			<< APART << " " 
			<< ASID
			<< AMOD 
			<< " HV_eta=S" << AETA
			<< " Gap=[" << AMIN << "-" << AMAX << "] "
			<< PAD << " " 
			<< " OfflineID=" 
			<< m_onlineHelper->show_to_string(offId) 
			<< " OnlineID="
			<< m_onlineHelper->show_to_string(onId) 
			<< " HVlineID=["
			<< PART << "." 
			<< CANL << "." 
			<< CANN << "." 
			<< LINE << "]" 
			<< std::endl;
		    }
		  }

		  // ---------
		  //   HEC-ALL
		  // ---------
		  if( (m_SubDetector == "ALL" )){
		    // HEC - ALL
		    nHEC++;
		    if( nHEC == nOLD+100 ){
		      ATH_MSG_INFO ( "[OFFLINE->ELECTRODE] Processing cell " << nHEC << " in HEC " );
		      nOLD = nHEC;
		    }    
		    // Test of getCellElectrodeIdVec()
		    std::vector<HWIdentifier> IdVec;
		    hvmap->getHVLineInCell( offId, IdVec );
		    std::vector<HWIdentifier> hvlineIdVec = IdVec;
		    nelHEC+= hvlineIdVec.size();
		  }

		}
	      }
	  }
	if( m_Detector == "FCAL" || m_Detector == "ALL" ) 
	  {// if m_Detector == FCAL
	    std::vector<Identifier>::const_iterator itId = m_fcalHelper->fcal_begin();
	    std::vector<Identifier>::const_iterator itIdEnd = m_fcalHelper->fcal_end();
	    for(; itId!=itIdEnd;++itId )
	      {// Loop over EM connected channels
		nFCAL++;
		const Identifier offId = *itId;
		int bec   = m_fcalHelper->pos_neg( offId);
		//int sam   = m_fcalHelper->module( offId);
		//int Side  = -9;
		if( abs(bec )== 2  ){
		  //if( bec > 0 ){ 
		  //  Side = 0;}
		  //else{Side = 1;
		  //}
		}
		if( abs(bec) == 2 ){
		  if( nFCAL == nOLD+100 ){
		    ATH_MSG_INFO ( "[OFFLINE->HVLINE] Processing cell " 
                                   << nFCAL << " in FCAL " );
		    nOLD = nFCAL;
		  }
		  // Test of getCellElectrodeIdVec()
		  std::vector<HWIdentifier> IdVec;
		  hvmap->getHVLineInCell( offId, IdVec );
		  std::vector<HWIdentifier> hvlineIdVec = IdVec;
		  // Loop over vector of electrodes 
		  std::vector<HWIdentifier>::const_iterator hv = hvlineIdVec.begin();
		  std::vector<HWIdentifier>::const_iterator hvEnd = hvlineIdVec.end();
		  for(; hv!=hvEnd;++hv){
		    HWIdentifier hvlineId = *hv;
	
		    // Compare hvlineId with exiting list of hvlineId
		    if( hvRefFCALvec.size() == 0){
		      hvRefFCALvec.push_back( hvlineId );
		    }
		    else{
		      bool foundHvId = false;
		      std::vector<HWIdentifier>::const_iterator hvRef = hvRefFCALvec.begin();
		      std::vector<HWIdentifier>::const_iterator hvRefEnd = hvRefFCALvec.end(); 
		      for(; hvRef!=hvRefEnd;++hvRef){
			HWIdentifier hv_ref = *hvRef;
			if( hv_ref == hvlineId ){
			  foundHvId = true;
			  break;
			}
		      }
		      if( !foundHvId ){
			hvRefFCALvec.push_back( hvlineId );
		      }
		    }
		    int PART= m_hvHelper->partition( hvlineId );
		    int CANL= m_hvHelper->can_line( hvlineId );
		    int CANN= m_hvHelper->can_node( hvlineId );
		    int LINE= m_hvHelper->hv_line( hvlineId );
		    // Loop over vector of electrodes 
		    fileHVFCALOFF 
		      << "OfflineID " 
		      << m_onlineHelper->show_to_string(offId) 
		      << " HVlineID=["
		      << PART << "." 
		      << CANL << "." 
		      << CANN << "." 
		      << LINE << "]" 
		      << std::endl;
		  }
		}
	      }
	  }
	nelFCAL+= hvRefFCALvec.size();
	ATH_MSG_INFO  ( " " );
	ATH_MSG_INFO  ( "============================================================================ " );
	ATH_MSG_INFO  ( " [OFFLINE->HVLINE] : List of hvlineId vs offlineId " );
	ATH_MSG_INFO  ( "============================================================================ " );
	if( m_Detector == "EMB" || m_Detector == "ALL" ){
	  if( m_SubDetector == "ALL"){
	    ATH_MSG_INFO  ( " - Offline channels in EMB :" << nEMB );
	    ATH_MSG_INFO  ( "    - nb hvlines in EMB :" << nelEMB );
	  }
	  if( m_SubDetector == "S0" || m_SubDetector == "ALL"){
	    ATH_MSG_INFO  ( "    - nb offline   in EMB0:" << nEMB0 );
	    ATH_MSG_INFO  ( "    - nb hvline in EMB0:" << nelEMB0 );
	  }
	  if( m_SubDetector == "S1" || m_SubDetector == "ALL"){
	    ATH_MSG_INFO  ( "    - nb offline   in EMB1:" << nEMB1 );
	    ATH_MSG_INFO  ( "    - nb hvline in EMB1:" << nelEMB1 );
	    ATH_MSG_INFO  ( "      - region 0 N(cell)= " << nEMB1r0 
                            << " N(HV)= " << nelEMB1r0 );
	    ATH_MSG_INFO  ( "      - region 1 N(cell)= " << nEMB1r1 
                            << " N(HV)= " << nelEMB1r1 );
	  }
	  if( m_SubDetector == "S2" || m_SubDetector == "ALL"){
	    ATH_MSG_INFO  ( "    - nb offline   in EMB2:" << nEMB2 );
	    ATH_MSG_INFO  ( "    - nb hvline in EMB2:" << nelEMB2 );
	    ATH_MSG_INFO  ( "      - region 0 N(cell)= " << nEMB2r0 
                            << " N(HV)= " << nelEMB2r0 );
	    ATH_MSG_INFO  ( "      - region 1 N(cell)= " << nEMB2r1 
                            << " N(HV)= " << nelEMB2r1 );
	  }
	  if( m_SubDetector == "S3" || m_SubDetector == "ALL"){
	    ATH_MSG_INFO  ( "    - nb offline   in EMB3:" << nEMB3 );
	    ATH_MSG_INFO  ( "    - nb hvline in EMB3:" << nelEMB3 );
	  }
	}
	// EMEC 
	if( m_Detector == "EMEC" || m_Detector == "ALL" ){
	  if( m_SubDetector == "ALL"){
	    ATH_MSG_INFO  ( " - Offline channels in EMEC:" << nEMEC );
	    ATH_MSG_INFO  ( "    - nb hvlines in EMEC:" << nelEMEC );
	  }
	  if( m_SubDetector == "S1" || m_SubDetector == "ALL"){
	    ATH_MSG_INFO  ( "    - nb offline   in EMEC1:" << nEMEC1 );
	    ATH_MSG_INFO  ( "    - nb hvline in EMEC1:" << nelEMEC1 );
	    ATH_MSG_INFO  ( "      - OUTER region 0 N(cell)= " << nEMEC1r0 
                            << " N(HV)= " << nelEMEC1r0 );
	    ATH_MSG_INFO  ( "      - OUTER region 1 N(cell)= " << nEMEC1r1 
                            << " N(HV)= " << nelEMEC1r1 );
	    ATH_MSG_INFO  ( "      - OUTER region 2 N(cell)= " << nEMEC1r2 
                            << " N(HV)= " << nelEMEC1r2 );
	    ATH_MSG_INFO  ( "      - OUTER region 3 N(cell)= " << nEMEC1r3 
                            << " N(HV)= " << nelEMEC1r3 );
	    ATH_MSG_INFO  ( "      - OUTER region 4 N(cell)= " << nEMEC1r4 
                            << " N(HV)= " << nelEMEC1r4 );
	    ATH_MSG_INFO  ( "      - OUTER region 5 N(cell)= " << nEMEC1r5 
                            << " N(HV)= " << nelEMEC1r5 );
	    ATH_MSG_INFO  ( "      - INNER region 0 N(cell)= " << nEMEC1r0IN 
                            << " N(HV)= " << nelEMEC1r0IN );
	  }
	  if( m_SubDetector == "S2" || m_SubDetector == "ALL"){
	    ATH_MSG_INFO  ( "    - nb offline   in EMEC2:" << nEMEC2 );
	    ATH_MSG_INFO  ( "    - nb hvline in EMEC2:" << nelEMEC2 );
	    ATH_MSG_INFO  ( "      - OUTER region 0 N(cell)= " << nEMEC2r0 
                            << " N(HV)= " << nelEMEC2r0 );
	    ATH_MSG_INFO  ( "      - OUTER region 1 N(cell)= " << nEMEC2r1 
                            << " N(HV)= " << nelEMEC2r1 );
	    ATH_MSG_INFO  ( "      - INNER region 0 N(cell)= " << nEMEC2r0IN 
                            << " N(HV)= " << nelEMEC2r0IN );

	  }
	  if( m_SubDetector == "S3" || m_SubDetector == "ALL"){
	    ATH_MSG_INFO  ( "    - nb offline   in EMEC3:" << nEMEC3 );
	    ATH_MSG_INFO  ( "    - nb hvline in EMEC3:" << nelEMEC3 );
	  }
	  if( m_SubDetector == "S0" || m_SubDetector == "ALL"){
	    ATH_MSG_INFO  ( "    - nb offline   in EMEC0:" << nEMEC0 );
	    ATH_MSG_INFO  ( "    - nb hvline in EMEC0:" << nelEMEC0 );
	  }
	}
	if( m_Detector == "HEC" || m_Detector == "ALL" ){
	  if( m_SubDetector == "S0" || m_SubDetector == "ALL"){
	    ATH_MSG_INFO  ( " - Offline channels in HEC0  :" << nHEC0 );
	    ATH_MSG_INFO  ( " - nb hvlines in HEC0     :" << nelHEC0 );
	    ATH_MSG_INFO  ( "      - region 0 N(cell)= " << nHEC0r0 
                            << " N(el)= " << nelHEC0r0 );
	    ATH_MSG_INFO  ( "      - region 1 N(cell)= " << nHEC0r1 
                            << " N(el)= " << nelHEC0r1 );
	  }
	  if( m_SubDetector == "S1" || m_SubDetector == "ALL"){
	    ATH_MSG_INFO  ( " - Offline channels in HEC1  :" << nHEC1 );
	    ATH_MSG_INFO  ( " - nb hvlines in HEC1     :" << nelHEC1 );
	    ATH_MSG_INFO  ( "      - region 0 N(cell)= " << nHEC1r0 
                            << " N(el)= " << nelHEC1r0 );
	    ATH_MSG_INFO  ( "      - region 1 N(cell)= " << nHEC1r1 
                            << " N(el)= " << nelHEC1r1 );
	  }
	  if( m_SubDetector == "S2" || m_SubDetector == "ALL"){
	    ATH_MSG_INFO  ( " - Offline channels in HEC2  :" << nHEC2 );
	    ATH_MSG_INFO  ( " - nb hvlines in HEC2     :" << nelHEC2 );
	    ATH_MSG_INFO  ( "      - region 0 N(cell)= " << nHEC2r0 
                            << " N(el)= " << nelHEC2r0 );
	    ATH_MSG_INFO  ( "      - region 1 N(cell)= " << nHEC2r1 
                            << " N(el)= " << nelHEC2r1 );
	  }
	  if( m_SubDetector == "S3" || m_SubDetector == "ALL"){
	    ATH_MSG_INFO  ( " - Offline channels in HEC3  :" << nHEC3 );
	    ATH_MSG_INFO  ( " - nb hvlines in HEC3     :" << nelHEC3 );
	    ATH_MSG_INFO  ( "      - region 0 N(cell)= " << nHEC3r0 
                            << " N(el)= " << nelHEC3r0 );
	    ATH_MSG_INFO  ( "      - region 1 N(cell)= " << nHEC3r1 
                            << " N(el)= " << nelHEC3r1 );
	  }

	}
	if( m_Detector == "FCAL" || m_Detector == "ALL" ){
	  ATH_MSG_INFO  ( " - Offline channels in FCAL :" << nFCAL );
	  ATH_MSG_INFO  ( "    - nb hvlines in FCAL :" << nelFCAL );
	}
	if( m_Detector == "ALL"){
	  ATH_MSG_INFO  ( "============================================================================ " );
	  //log << MSG::INFO  << "  -- TOTAL    : " << nelDET << endmsg; 
	  ATH_MSG_INFO  ( "============================================================================ " );
	}
      }  
    }// if m_HVelectrodeToOffline == ON


  // ============================================================================
  //              Test of Online --> Offline Identifiers 
  // ============================================================================
  //
  // 03-AUG-2006   : AL corrected bug in LArOnline for EMEC/HEC counting
  // 
  // Last Modified : AL in MAR-2006
  //                 added tests per sub-detector
  // Last Modified : AL in august 2004  
  // Last Updated  : AL in DEC 2004 
  //                 added tests to compare HV with online/offline
  // ============================================================================
  
  if( m_OnlineTest == "ON" ){// is m_OnlineTest is ON 
    // Initialization for counters
    int nConn=0;
    int nDisc=0;
    int nH8Disc=0;
    int nH8Conn=0;
    int nH6Disc=0;
    int nH6Conn=0;
    int nerror=0;

    int nEmec=0;
    int nEmba=0;
    int nHec =0;
    int nFcal=0;

    int nEmbaConn=0;
    int nEmbaWC=0;
    int nEmecConn=0;
    int nEmecWC=0;
    int nHecConn=0;
    int nFcalConn=0;

    int nEmbaDisc=0;
    int nEmecDisc=0;
    int nFcalDisc=0;
    int nHecDisc=0;
    int nNoCounterPart=0;

    int nch=0;
    int nch_old = 0;
    int nch_2nd=0;

    int nH8S0=0;
    int nH8S1=0;
    int nH8S2=0;
    int nH8S3=0;
    // Display on screen
    ATH_MSG_INFO ( "========================================  " );
    ATH_MSG_INFO ( "  >>> Tests for LArOnline Identifiers <<< " );
    ATH_MSG_INFO ( "========================================  " );
    std::string mychDisc;
    std::string mychStat;
    std::string myDet="unknown";
    std::vector<HWIdentifier>::const_iterator itOnId = m_onlineHelper->channel_begin();
    std::vector<HWIdentifier>::const_iterator itOnIdEnd = m_onlineHelper->channel_end();  
    
    if( m_Calibration != "EXCL" ){
      // if Calibration Not Exclusive 
      for(; itOnId!=itOnIdEnd;++itOnId)
	{// Loop over onlineHelper->channel_begin()
	  const HWIdentifier ch_id = *itOnId;      
	  
	  // Require channels to be connected 
	  //=================================
	  if( (l_version=="H6TestBeam" || l_version=="H8TestBeam" || l_version=="fullAtlas" ))
	    {//if version==H6TestBeam or H8TestBeam or fullAtlas 
	      
	      
	      // ================================================
	      //  
	      //           H6TestBeam dictionary
	      // 
	      //=================================================
	      //	      
	      if( l_version=="H6TestBeam" && m_onlineHelper->is_H6( ch_id ) )
		{//if is_H6() && isH6loop
		  nch++;
		  mychDisc="Unknown";
		  Identifier ch_offl ; 
		  try 
		    {
		      ch_offl = cabling->cnvToIdentifier(ch_id) ; 
		    }
		  catch(LArID_Exception & except)
		    {
		      nerror++;
		      fileError << "ERROR " 
				<< "[H6 ONLINE] # " << nerror << " LArId-Exc Converting OnlineId " 
				<< m_onlineHelper->show_to_string(ch_id) << "   "
				<< (std::string)except
				<< std::endl;
		    }
		  // Reverse Test
		  HWIdentifier id_test;
		  try {
		    id_test = cabling->createSignalChannelID( ch_offl ) ;
		  }
		  catch(LArID_Exception & except){
		    fileError << "ERROR " 
			      <<  "[H6 ONLINE] LArId exception converting offline id  " 
			      << m_emHelper->show_to_string(ch_offl) << "   "
			      <<  "[H6 ONLINE] initial online id  " 
			      << m_emHelper->show_to_string(ch_id) << "   "
			      << (std::string)except
			      << std::endl;
		  }
		  if( ch_id != id_test ) 
		    { 
		      fileError <<"ERROR "<<"[H6 ONLINE] Error in mapping for online id= "<< m_onlineHelper->show_to_string( ch_id )  << std::endl;
		      fileError <<"ERROR "<<"[H6 ONLINE] Offline ID, returned online ID= " << m_onlineHelper->show_to_string(ch_offl) << ", " << m_onlineHelper->show_to_string(id_test) << std::endl ;
		    }
		  
		  if(cabling->isOnlineConnected(ch_id))
		    {// if isOnlineConnected()
		      nH6Conn++;
		      mychDisc="connected";
		      Identifier ch_offl ; 
		      try 
			{
			  ch_offl = cabling->cnvToIdentifier(ch_id) ; 
			}
		      catch(LArID_Exception & except)
			{
			  nerror++;
			  fileError << "(h6) [" << nerror << "] "  
				    << " LArId exception Converting OnlineId  " 
				    << m_emHelper->show_to_string(ch_id) << " "
				    << (std::string)except << " "
				    << m_emHelper->show_to_string(ch_offl) << " "
				    << std::endl;
			}
		    }
		  else
		    {
		      mychDisc="disconnected";
		      nH6Disc++;
		    }
		  if(m_onlineHelper->is_H6fcal(ch_id))
		    {
		      nFcal++; 
		      mychStat="FCAL ";
		      mychDisc="Unknown";
		      if( cabling->isOnlineConnected(ch_id))
			{
			  nFcalConn++;
			  mychDisc="connected";
			}
		      else 
			{
			  nFcalDisc++;
			  mychDisc="disconnected";
			}
		      fileFcal << "OnlineID= " << m_emHelper->show_to_string(ch_id)
			       << " Online Compact ID= " 
			       << ch_id.getString()
			       << " OfflineID= " 
			       << m_emHelper->show_to_string(ch_offl) 
			       << " Offline Compact ID= " 
			       << ch_offl.getString()
			       << " Status=" << mychDisc
			       << std::endl;
		    }
		  if(m_onlineHelper->is_H6hec(ch_id))
		    {
		      nHec++; 
		      mychStat="HEC  ";
		      mychDisc="Unknown";
		      if( cabling->isOnlineConnected(ch_id))
			{
			  nHecConn++;
			  mychDisc="connected";
			}
		      else 
			{
			  nHecDisc++;
			  mychDisc="disconnected";
			}
		      fileHec << "OnlineID= " << m_emHelper->show_to_string(ch_id)
			      << " Online Compact ID= " 
			      << ch_id.getString()
			      << " OfflineID= " 
			      << m_emHelper->show_to_string(ch_offl) 
			      << " Offline Compact ID= " 
			      << ch_offl.getString()
			      << " Status=" << mychDisc
			      << std::endl;
		    }
		  if(m_onlineHelper->is_H6emec(ch_id))
		    {
		      nEmec++; 
		      mychStat="EMEC ";
		      mychDisc="Unknown";
		      if( cabling->isOnlineConnected(ch_id))
			{
			  nEmecConn++;
			  mychDisc="connected";
			}
		      else 
			{
			  nEmecDisc++;
			  mychDisc="disconnected";
			}
		      fileEmec << "OnlineID= " << m_emHelper->show_to_string(ch_id)
			       << " Online Compact ID= " 
			       << ch_id.getString()
			       << " OfflineID= " 
			       << m_emHelper->show_to_string(ch_offl) 
			       << " Offline Compact ID= " 
			       << ch_offl.getString()
			       << " Status=" << mychDisc
			       << std::endl;
		    }
		  fileAll << "OnlineID= " 
			  << m_emHelper->show_to_string(ch_id)
			  << " Online Compact ID= " 
			  << ch_id.getString()
			  << " OfflineID= " 
			  << m_emHelper->show_to_string(ch_offl) 
			  << " Offline Compact ID= " 
			  << ch_offl.getString()
			  << " Det= " << mychStat << " Status=" << mychDisc 
			  << std::endl;
		}
	      
	      // ================================================
	      //  
	      //         H8TestBeam dictionary
	      // 
	      //=================================================
	      //	      
	      else if( (l_version=="H8TestBeam" && m_onlineHelper->is_H8( ch_id ) )
		       && cabling->isOnlineConnected(ch_id) )
		{//if is_H8()
		  nch++;
		  if( m_onlineHelper->is_H8onlineOnly(ch_id))
		    {
		      nNoCounterPart++;
		    }
		  Identifier ch_offl ; 
		  ch_offl = cabling->cnvToIdentifier(ch_id); 
		  try 
		    {
		      ch_offl = cabling->cnvToIdentifier(ch_id) ; 
		    }
		  catch(LArID_Exception & except)
		    {
		      nerror++;
		      fileError << "ERROR " << "[H8 ONLINE] # " << nerror << "LArId-Exc Converting OnlineId " 
				<< m_emHelper->show_to_string(ch_id) << "   "
				<< (std::string)except
				<< std::endl;
		    }
		  // Reverse Test
		  HWIdentifier id_test;
		  try 
		    {
		      id_test = cabling->createSignalChannelID( ch_offl ) ;
		    }
		  catch(LArID_Exception & except){
		    fileError <<  "ERROR [H8 ONLINE] LArId exception converting offline id  " 
			      << m_emHelper->show_to_string(ch_offl) << "   "
			      <<  "[H8 ONLINE] initial online id  " 
			      << m_emHelper->show_to_string(ch_id) << "   "
			      << (std::string)except
			      << std::endl;
		  }
		  if( ch_id != id_test ) 
		    { 
		      fileError <<"ERROR "<<"[H8 ONLINE] Error in mapping for online id= "<< m_onlineHelper->show_to_string( ch_id )  << std::endl ;
		      fileError <<"ERROR "<<"[H8 ONLINE] Offline ID, returned online ID= " << m_onlineHelper->show_to_string(ch_offl) << ", " << m_onlineHelper->show_to_string(id_test) << std::endl ;
		    }	  
		  mychDisc="Unknown";
		  if( !cabling->isOnlineConnected(ch_id))
		    {
		      nH8Disc++;
		      mychDisc="disconnected";
		    }
		  else
		    {
		      nH8Conn++;
		      mychDisc="connected";
		    }
		  if( m_onlineHelper->isEmBarrelOnline(ch_id) ){myDet="(EM Barrel)";}
		  if( m_onlineHelper->isEmEndcapOnline(ch_id) ){myDet="(EM Endcap)";}
		  if( m_onlineHelper->isHecOnline(ch_id) ){myDet="(HEC)";}
		  if( m_onlineHelper->isFcalOnline(ch_id) ){myDet="(FCAL)";}
		  fileAll << "OnlineID= " 
			  << m_emHelper->show_to_string(ch_id)
			  << " Online Compact ID= " 
			  << ch_id.getString()
			  << " OfflineID= " 
			  << m_emHelper->show_to_string(ch_offl) 
			  << " Offline Compact ID= " 
			  << ch_offl.getString()
			  << " Det= " << myDet << " Status=" << mychDisc 
			  << std::endl;
		  if( m_emHelper->sampling(ch_offl) == 0){nH8S0++;}
		  if( m_emHelper->sampling(ch_offl) == 1){nH8S1++;}
		  if( m_emHelper->sampling(ch_offl) == 2){nH8S2++;}
		  if( m_emHelper->sampling(ch_offl) == 3){nH8S3++;}
		}
	      //
	      //
	      // ================================================
	      //  
	      //           fullAtlas Dictionary 
	      // 
	      //=================================================
	      //
	      else if( l_version=="fullAtlas" )
		{//if isATLAS
		  //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
		  // Online Tests per sub-system : EMB
		  //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
		  if( m_Detector == "EMB" || m_Detector == "ALL" ){
		    // if Detector==EMB or ALL
		    if( m_onlineHelper->isEmBarrelOnline(ch_id) ){
		      // isEMBarrelOnline()
		      nch++;
		      nEmba++;
		      if(nch==nch_old+2000){
			ATH_MSG_INFO ( "[ATLAS] processing EMB online-channel ..# " 
                                       << nch );
			nch_old=nch;
		      }
		      mychDisc="Unknown";
		      if( !cabling->isOnlineConnected(ch_id)){
			nDisc++;
			nEmbaDisc++;
			mychDisc="disconnected";
		      }
		      else{
			nConn++;
			nEmbaConn++;
			mychDisc="connected";
		      }
		      myDet="(EM Barrel)";
		      if( !m_onlineHelper->isNotWarmCableConnected(ch_id) ){
			Identifier ch_offl ; 
			try {
			  ch_offl = cabling->cnvToIdentifier(ch_id) ; 
			}
			catch(LArID_Exception & except){
			  nerror++;
			  fileError << "ERROR " 
				    << "[ATLAS ONLINE]==> Error # " << nerror 
				    << " LArId-Exc Converting OnlineId " 
				    << m_emHelper->show_to_string(ch_id) << "   "
				    << (std::string)except
				    << std::endl;
			}
			fileEmbOn << " OnlineID= " 
				  << m_onlineHelper->show_to_string(ch_id) 
				  << " Compact OnlineID= "
				  << ch_id.getString()
				  << " OfflineID= "
				  << m_emHelper->show_to_string(ch_offl) 
				  << " Compact OfflineID= " 
				  << ch_offl.getString()
				  << " " << mychDisc << " " << myDet  
				  << std::endl;
			fileAll   << " OnlineID= " 
				  << m_onlineHelper->show_to_string(ch_id) 
				  << " Compact OnlineID= "
				  << ch_id.getString()
				  << " OfflineID= "
				  << m_emHelper->show_to_string(ch_offl) 
				  << " Compact OfflineID= " 
				  << ch_offl.getString()
				  << " " << mychDisc << " " << myDet  
				  << std::endl;
			// Reverse Test
			HWIdentifier id_test;
			try {
			  id_test = cabling->createSignalChannelID( ch_offl ) ;
			}
			catch(LArID_Exception & except){
			  fileError << "ERROR " 
				    <<  "[ATLAS ONLINE] LArId exception converting offline id  " 
				    << m_emHelper->show_to_string(ch_offl) << "   "
				    <<  "[ATLAS ONLINE] initial online id  " 
				    << m_emHelper->show_to_string(ch_id) << "   "
				    << (std::string)except
				    << std::endl;
			}
			if( ch_id != id_test ) { 
			  nch_2nd++;
			  fileError <<"ERROR "
				    <<"[ATLAS ONLINE] Error in mapping for online id= "
				    << m_onlineHelper->show_to_string( ch_id )  << std::endl;
			  fileError <<"ERROR "<<"[ATLAS ONLINE] Offline ID, returned online ID= " 
				    << m_onlineHelper->show_to_string(ch_offl) 
				    << ", " 
				    << m_onlineHelper->show_to_string(id_test) << std::endl;
			}
		      }
		      else{
			nEmbaWC++;
			mychDisc = "Not connected to Warm cable";
			fileEmbOn << " OnlineID= " 
				  << m_onlineHelper->show_to_string(ch_id) 
				  << " Compact OnlineID= "
				  << ch_id.getString()
				  << " No OfflineID "
				  << " " << mychDisc << " " << myDet  
				  << std::endl;
			fileAll   << " OnlineID= " 
				  << m_onlineHelper->show_to_string(ch_id) 
				  << " Compact OnlineID= "
				  << ch_id.getString()
				  << " No OfflineID "
				  << " " << mychDisc << " " << myDet  
				  << std::endl;
		      }
		    }
		  }
		  //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
		  // Online Tests per sub-system : EMEC
		  //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
		  if( m_Detector == "EMEC" || m_Detector == "ALL" ){
		    // Detector==EMEC or ALL 
		    if( m_onlineHelper->isEMECchannel(ch_id) ){
		      // isEMBarrelOnline()
		      nch++;
		      nEmec++;
		      if(nch==nch_old+1000){
			ATH_MSG_INFO ( "[ATLAS] processing EMEC online-channel ..# " 
                                       << nch );
			nch_old=nch;
		      }
		      mychDisc="Unknown";
		      if( !cabling->isOnlineConnected(ch_id)){
			nDisc++;
			nEmecDisc++;
			mychDisc="disconnected";
		      }
		      else{
			nConn++;
			nEmecConn++;
			mychDisc="connected";
		      }
		      if( !m_onlineHelper->isNotWarmCableConnected(ch_id) ){
			// if ! NotWarmConnected()
			Identifier ch_offl ; 
			try {
			  ch_offl = cabling->cnvToIdentifier(ch_id) ; 
			}
			catch(LArID_Exception & except){
			  nerror++;
			  fileError << "ERROR " 
				    << "[ATLAS ONLINE]==> Error # " << nerror 
				    << " LArId-Exc Converting OnlineId " 
				    << m_emHelper->show_to_string(ch_id) << "   "
				    << (std::string)except
				    << std::endl;
			}
			// Reverse Test
			HWIdentifier id_test;
			try {
			  id_test = cabling->createSignalChannelID( ch_offl ) ;
			}
			catch(LArID_Exception & except){
			  fileError << "ERROR " 
				    <<  "[ATLAS ONLINE] LArId exception converting online id :  " 
				    << m_emHelper->show_to_string(ch_id) << "   "
				    <<  " into offline id : " 
				    << m_emHelper->show_to_string(ch_offl) << "   "
				    << " and back the offline id into online id..." 
				    << (std::string)except
				    << std::endl;
			}
			// Output for EMEC
			myDet="(EMEC)";
			fileEmecOn << " OnlineID= " 
				   <<  m_onlineHelper->show_to_string(ch_id) 
				   << " Compact OnlineID= "
				   << ch_id.getString()
				   << " OfflineID= "
				   << m_emHelper->show_to_string(ch_offl) 
				   << " Compact OfflineID= " 
				   << ch_offl.getString()
				   << " " << mychDisc << " " << myDet  
				   << std::endl;
			fileAll   << " OnlineID= " 
				  << m_onlineHelper->show_to_string(ch_id) 
				  << " Compact OnlineID= "
				  << ch_id.getString()
				  << " OfflineID= "
				  << m_emHelper->show_to_string(ch_offl) 
				  << " Compact OfflineID= " 
				  << ch_offl.getString()
				  << " " << mychDisc << " " << myDet  
				  << std::endl;
		      }
		      else{
			nEmecWC++;
			mychDisc = "Not connected to Warm cable";
			fileEmbOn << " OnlineID= " 
				  << m_onlineHelper->show_to_string(ch_id) 
				  << " Compact OnlineID= "
				  << ch_id.getString()
				  << " No OfflineID "
				  << " " << mychDisc << " " << myDet  
				  << std::endl;
			fileAll   << " OnlineID= " 
				  << m_onlineHelper->show_to_string(ch_id) 
				  << " Compact OnlineID= "
				  << ch_id.getString()
				  << " No OfflineID "
				  << " " << mychDisc << " " << myDet  
				  << std::endl;
		      }
		    }
		  }
		  
		  
		  //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
		  // Online Tests per sub-system : HEC
		  //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
		  if( m_Detector == "HEC" || m_Detector == "ALL" ){
		    // Detector==HEC or ALL
		    if( m_onlineHelper->isHECchannel(ch_id) ){
		      // isHecOnline()
		      nch++;
		      nHec++;
		      if(nch==nch_old+1000){
			ATH_MSG_INFO ( "[ATLAS] processing HEC online-channel ..# " 
                                       << nch );
			nch_old=nch;
		      }
		      mychDisc="Unknown";
		      if( !cabling->isOnlineConnected(ch_id)){
			nDisc++;
			nHecDisc++;
			mychDisc="disconnected";
		      }
		      else{
			nConn++;
			nHecConn++;
			mychDisc="connected";
		      }

		      Identifier ch_offl ; 
		      try {
			ch_offl = cabling->cnvToIdentifier(ch_id) ; 
		      }
		      catch(LArID_Exception & except){
			nerror++;
			fileError << "ERROR " 
				  << "[ATLAS ONLINE]==> Error # " << nerror 
				  << " LArId-Exc Converting OnlineId " 
				  << m_emHelper->show_to_string(ch_id) << "   "
				  << (std::string)except
				  << std::endl;
		      }
		      myDet="(HEC)";
		      fileHecOn << " OnlineID= " 
				<< m_onlineHelper->show_to_string(ch_id) 
				<< " Compact OnlineID= "
				<< ch_id.getString()
				<< " OfflineID= "
				<< m_hecHelper->show_to_string(ch_offl) 
				<< " Compact OfflineID= " 
				<< ch_offl.getString()
				<< " " << mychDisc << " " << myDet  
				<< std::endl;
		      fileAll   << " OnlineID= " 
				<< m_onlineHelper->show_to_string(ch_id) 
				<< " Compact OnlineID= "
				<< ch_id.getString()
				<< " OfflineID= "
				<< m_hecHelper->show_to_string(ch_offl) 
				<< " Compact OfflineID= " 
				<< ch_offl.getString()
				<< " " << mychDisc << " " << myDet  
				<< std::endl;
		      
		      // Reverse Test
		      HWIdentifier id_test;
		      try {
			id_test = cabling->createSignalChannelID( ch_offl ) ;
		      }
		      catch(LArID_Exception & except){
			fileError << "ERROR " 
				  <<  "[ATLAS ONLINE] LArId exception converting offline id  " 
				  << m_emHelper->show_to_string(ch_offl) << "   "
				  <<  "[ATLAS ONLINE] initial online id  " 
				  << m_emHelper->show_to_string(ch_id) << "   "
				  << (std::string)except
				  << std::endl;
		      }
		      if( ch_id != id_test ) { 
			nch_2nd++;
			fileError <<"ERROR "
				  <<"[ATLAS ONLINE] Error in mapping for online id= "
				  << m_onlineHelper->show_to_string( ch_id )  << std::endl;
			fileError <<"ERROR "<<"[ATLAS ONLINE] Offline ID, returned online ID= " 
				  << m_onlineHelper->show_to_string(ch_offl) 
				  << ", " 
				  << m_onlineHelper->show_to_string(id_test) << std::endl;
		      }
		    }
		  }// if m_Detector == HEC
		  
		  

		  //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
		  // Online Tests per sub-system : FCAL
		  //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
		  if( m_Detector == "FCAL" || m_Detector == "ALL" ){
		    // Detector==FCAL or ALL
		    if( m_onlineHelper->isFcalOnline(ch_id) ){
		      // isFcalOnline()
		      nch++;
		      nFcal++;
		      if(nch==nch_old+1000){
			ATH_MSG_INFO ( "[ATLAS] processing FCAL online-channel ..# " 
                                       << nch );
			nch_old=nch;
		      }
		      mychDisc="Unknown";
		      if( !cabling->isOnlineConnected(ch_id)){
			nDisc++;
			nFcalDisc++;
			mychDisc="disconnected";
		      }
		      else{
			nConn++;
			nFcalConn++;
			mychDisc="connected";
		      }
		      Identifier ch_offl ; 
		      try {
			ch_offl = cabling->cnvToIdentifier(ch_id) ; 
		      }
		      catch(LArID_Exception & except){
			nerror++;
			fileError << "ERROR " 
				  << "[ATLAS ONLINE]==> Error # " << nerror 
				  << " LArId-Exc Converting OnlineId " 
				  << m_fcalHelper->show_to_string(ch_id) << "   "
				  << (std::string)except
				  << std::endl;
		      }
		      myDet="(FCAL)";
		      fileFcalOn << " OnlineID= " 
				 << m_onlineHelper->show_to_string(ch_id) 
				 << " Compact OnlineID= "
				 << ch_id.getString()
				 << " OfflineID= "
				 << m_emHelper->show_to_string(ch_offl) 
				 << " Compact OfflineID= " 
				 << ch_offl.getString()
				 << " " << mychDisc << " " << myDet  
				 << std::endl;
		      fileAll   << " OnlineID= " 
				<< m_onlineHelper->show_to_string(ch_id) 
				<< " Compact OnlineID= "
				<< ch_id.getString()
				<< " OfflineID= "
				<< m_emHelper->show_to_string(ch_offl) 
				<< " Compact OfflineID= " 
				<< ch_offl.getString()
				<< " " << mychDisc << " " << myDet  
				<< std::endl;
		      // Reverse Test
		      HWIdentifier id_test;
		      try {
			id_test = cabling->createSignalChannelID( ch_offl ) ;
		      }
		      catch(LArID_Exception & except){
			fileError << "ERROR " 
				  <<  "[ATLAS ONLINE] LArId exception converting offline id  " 
				  << m_emHelper->show_to_string(ch_offl) << "   "
				  <<  "[ATLAS ONLINE] initial online id  " 
				  << m_emHelper->show_to_string(ch_id) << "   "
				  << (std::string)except
				  << std::endl;
		      }
		      if( ch_id != id_test ) { 
			nch_2nd++;
			fileError <<"ERROR "
				  <<"[ATLAS ONLINE] Error in mapping for online id= "
				  << m_onlineHelper->show_to_string( ch_id )  << std::endl;
			fileError <<"ERROR "<<"[ATLAS ONLINE] Offline ID, returned online ID= " 
				  << m_onlineHelper->show_to_string(ch_offl) 
				  << ", " 
				  << m_onlineHelper->show_to_string(id_test) << std::endl;
		      }
		    } // isFcalOnline()
		  }  // if m_Detector==FCAL

		} // if version=fullAtlas
	      else
		{
		  ATH_MSG_INFO ( "===>>>> Channels not in dictionary " );
		}
	    }     // if version==H6TestBeam or H8TestBeam or fullAtlas 
	}         // Loop over itOnId
    }             // if( m_Calibration != EXCL)
    if( l_version=="H6TestBeam" )
      {
	ATH_MSG_INFO ( "=============================================" );
	ATH_MSG_INFO ( "   Statistics for H6 TestBeam  " );
	ATH_MSG_INFO ( "===> Total number of FCAL channels = " << nFcal );
	ATH_MSG_INFO ( "       #connected    channels = " << nFcalConn );
	ATH_MSG_INFO ( "       #disconnected channels = " << nFcalDisc );
	ATH_MSG_INFO ( "===> Total number of HEC  channels = " << nHec );
	ATH_MSG_INFO ( "       #connected    channels = " << nHecConn );
	ATH_MSG_INFO ( "       #disconnected channels = " << nHecDisc );
	ATH_MSG_INFO ( "===> Total number of EMEC channels = " << nEmec );
	ATH_MSG_INFO ( "       #connected    channels = " << nEmecConn );
	ATH_MSG_INFO ( "       #disconnected channels = " << nEmecDisc );
	ATH_MSG_INFO ( "===> Total    connected channels " << nH6Conn );
	ATH_MSG_INFO ( "===> Total disconnected channels " << nH6Disc );
	// on files
	fileAll << "===> Total number of FCAL channels = " << nFcal << std::endl;
	fileAll << "       #connected    channels = " << nFcalConn << std::endl;
	fileAll << "       #disconnected channels = " << nFcalDisc << std::endl;
	// FCAL
	fileFcal << "===> Total number of FCAL channels = " << nFcal << std::endl;
	fileFcal << "       #connected    channels = " << nFcalConn << std::endl;
	fileFcal << "       #disconnected channels = " << nFcalDisc << std::endl;
	  // HEC
	fileAll << "===> Total number of HEC  channels = " << nHec << std::endl;
	fileAll << "       #connected    channels = " << nHecConn << std::endl;
	fileAll << "       #disconnected channels = " << nHecDisc << std::endl;
	
	fileHec << "===> Total number of HEC  channels = " << nHec << std::endl;
	fileHec << "       #connected    channels = " << nHecConn << std::endl;
	fileHec << "       #disconnected channels = " << nHecDisc << std::endl;
	// EMEC
	fileAll << "===> Total number of EMEC channels = " << nEmec << std::endl;
	fileAll << "       #connected    channels = " << nEmecConn << std::endl;
	fileAll << "       #disconnected channels = " << nEmecDisc << std::endl;
	fileEmec << "===> Total number of EMEC channels = " << nEmec << std::endl;
	fileEmec << "       #connected    channels = " << nEmecConn << std::endl;
	fileEmec << "       #disconnected channels = " << nEmecDisc << std::endl;
	// All 
	fileAll << "===> Total    connected channels " << nH6Conn << std::endl;
	fileAll << "===> Total disconnected channels " << nH6Disc << std::endl;
	ATH_MSG_INFO ( "=============================================" );
	fileEmec.close();
	fileFcal.close();
	fileHec.close();
	fileAll.close();
	fileError.close();
      }  
    else if( l_version=="H8TestBeam" )
      {
	fileAll << "===============================================" << std::endl;
	fileAll << "    Total number of H8 channels = " << nch << std::endl;
	fileAll << "         #connected    channels = " << nH8Conn << std::endl;
	fileAll << "         #disconnected channels = " << nH8Disc << std::endl;
	fileAll << "    Channels with no counterpart= " << nNoCounterPart << std::endl;
	fileAll << "===============================================" << std::endl;
	ATH_MSG_INFO ( "===============================================" );
	ATH_MSG_INFO ( "    Total number of H8 channels = " << nch );
	ATH_MSG_INFO ( "         #connected    channels = " << nH8Conn );
	ATH_MSG_INFO ( "         #disconnected channels = " << nH8Disc );
	ATH_MSG_INFO ( "    Channels with no counterpart= " << nNoCounterPart );
	ATH_MSG_INFO ( "    H8 Channels distribution :    " );
	ATH_MSG_INFO ( "                 Sampling 0  : " << nH8S0 );
	ATH_MSG_INFO ( "                 Sampling 1  : " << nH8S1 );
	ATH_MSG_INFO ( "                 Sampling 2  : " << nH8S2 );
	ATH_MSG_INFO ( "                 Sampling 3  : " << nH8S3 );
	ATH_MSG_INFO ( "====================================================" );
	fileAll.close();
	fileError.close();
      }  
    else if( l_version=="fullAtlas" )
      {// version == fullAtlas
	ATH_MSG_INFO ( "====================================================" );
	if( m_Detector == "ALL"){
	  ATH_MSG_INFO ( "    Total ATLAS channels    = " << nch );
	  ATH_MSG_INFO ( "    - connected channels    = " << nConn << " (ref=182468)" );
	  ATH_MSG_INFO ( "    - disconnected channels =  " << nDisc << " (ref= 12604)" );
	  ATH_MSG_INFO ( "====================================================" );
	}
	if( m_Detector == "EMB" || m_Detector == "ALL" ){
	  ATH_MSG_INFO ( "    Total  EMB channels     = " << nEmba << " (ref=114688)" );
	  ATH_MSG_INFO ( "    - connected    channels = " << nEmbaConn << " (ref=109568)" );
	  ATH_MSG_INFO ( "    - disconnected channels =   " << nEmbaDisc << " (ref=5120)" );
	  ATH_MSG_INFO ( "      without warm cable    =   " << nEmbaWC << " (ref=4096)" );
	  ATH_MSG_INFO ( " +++++++++++++++++++++++++++++++++++++++++++++++++" );
	}
	if( m_Detector == "EMEC" || m_Detector == "ALL" ){
	  ATH_MSG_INFO ( "    Total  EMEC channels    =  " << nEmec << " (ref=70656)" );
	  ATH_MSG_INFO ( "    - connected    channels =  " << nEmecConn << " (ref=63744)" );
	  ATH_MSG_INFO ( "    - disconnected channels =   " << nEmecDisc << " (ref= 6912)" );
	  ATH_MSG_INFO ( "      without warm cable    =    " << nEmecWC << " (ref=  512)" );
	  ATH_MSG_INFO ( " +++++++++++++++++++++++++++++++++++++++++++++++++" );
	}
	if( m_Detector == "HEC"|| m_Detector == "ALL" ){
	  ATH_MSG_INFO ( "    Total  HEC channels     =   " << nHec << " (ref=6144)" );
	  ATH_MSG_INFO ( "    - connected    channels =   " << nHecConn << " (ref=5632)" );
	  ATH_MSG_INFO ( "    - disconnected channels =    " << nHecDisc << " (ref= 512)" );
	  ATH_MSG_INFO ( " +++++++++++++++++++++++++++++++++++++++++++++++++" );
	}
	if( m_Detector == "FCAL"|| m_Detector == "ALL" ){
	  ATH_MSG_INFO ( "    Total FCAL channels     =   " << nFcal << " (ref=3584)" );
	  ATH_MSG_INFO ( "    - connected    channels =   " << nFcalConn << " (ref=3524)" );
	  ATH_MSG_INFO ( "    - disconnected channels =     " << nFcalDisc << " (ref=  60)" );
	}
	ATH_MSG_INFO ( "==================================================" );
	fileEmecOn.close();
	fileHecOn.close();
	fileFcalOn.close();
	fileEmbOn.close();
	fileAll.close();
	fileError.close();
      }	
    
      // ===================================================
      // 
      //   H8TestBeam FEB IDS Online-wise loop (1) 
      // 
      // ===================================================
      if( l_version == "H8TestBeam" ){
	nch=0;
	itOnId = m_onlineHelper->feb_begin();
	itOnIdEnd = m_onlineHelper->feb_end();
	for(; itOnId!=itOnIdEnd;++itOnId)
	  {
	    const HWIdentifier feb_id = *itOnId;
	    nch++;
	    if(m_onlineHelper->is_H8(feb_id))
	      {
		ATH_MSG_INFO(  "FEB Ids = " 
                               << m_onlineHelper->show_to_string(feb_id) 
                               << " Compact Ids= " 
                               << feb_id.getString() );
	      }
	  }
      }
  } // if (m_OnlineTest == ON/OFF)
  
  
  
  // ===================================================
  //   FEB counts for ATLAS
  // ===================================================
  // 
  // Added by AL on 05-OCT-2006
  //
  //
  // ===================================================
  if( m_OnlineTest == "FEB" ){
    // m_OnlineTest== FEB 
    if( l_version == "fullAtlas" ){
      // version = fullAtlas
      int nch=0;
      int nFEBfcal=0;
      int nFEBhec=0;
      int nFEBemec=0;
      int nFEBemb=0;
      std::vector<HWIdentifier>::const_iterator itOnId;
      std::vector<HWIdentifier>::const_iterator itOnIdEnd;
      itOnId = m_onlineHelper->feb_begin();
      itOnIdEnd = m_onlineHelper->feb_end();
      for(; itOnId!=itOnIdEnd;++itOnId){
	const HWIdentifier feb_id = *itOnId;
	nch++;
	if( m_Detector == "FCAL"|| m_Detector == "ALL" ){
	  if( m_onlineHelper->isFcalOnline(feb_id) ){
	    nFEBfcal++;
	    fileFcalOn << " FEB OnlineID= " 
		       << m_onlineHelper->show_to_string(feb_id) 
		       << " Compact OnlineID= "
		       << feb_id.getString()
		       << std::endl;
	  }
	}
	if( m_Detector == "HEC"|| m_Detector == "ALL" ){
	  if( m_onlineHelper->isHECchannel(feb_id) ){ 
	    nFEBhec++;
	    fileHecOn << " FEB OnlineID= " 
		      << m_onlineHelper->show_to_string(feb_id) 
		      << " Compact OnlineID= "
		      << feb_id.getString()
		      << std::endl;
	  }
	}
	if( m_Detector == "EMB" || m_Detector == "ALL" ){
	  if( m_onlineHelper->isEmBarrelOnline(feb_id) ){ 
	    nFEBemb++;
	    fileEmbOn << " FEB OnlineID= " 
		      << m_onlineHelper->show_to_string(feb_id) 
		      << " Compact OnlineID= "
		      << feb_id.getString()
		      << " OfflineID= "
		      << std::endl;
	  }
	}
	if( m_Detector == "EMEC" || m_Detector == "ALL" ){
	  if( m_onlineHelper->isEMECchannel(feb_id) ){
	    nFEBemec++;
	    fileEmecOn << " FEB OnlineID= " 
		       <<  m_onlineHelper->show_to_string(feb_id) 
		       << " Compact OnlineID= "
		       << feb_id.getString()
		       << std::endl;
	  }
	}
      }
      ATH_MSG_INFO ( " m_OnlineTest= " << m_OnlineTest );
      ATH_MSG_INFO ( "=========================================" );
      ATH_MSG_INFO ( " - Total Number of FEBs in ATLAS: " << nch );
      ATH_MSG_INFO ( "=========================================" );
      ATH_MSG_INFO ( "    - EMB : " << nFEBemb );
      ATH_MSG_INFO ( "    - EMEC: " << nFEBemec );
      ATH_MSG_INFO ( "    - HEC : " << nFEBhec );
      ATH_MSG_INFO ( "    - FCAL: " << nFEBfcal );
      ATH_MSG_INFO ( "=========================================" );
    }
    fileEmecOn.close();
    fileFcalOn.close();
    fileHecOn.close();
    fileEmbOn.close();
  }

  //
  // =========================================
  //     Tests of Feedthrough Numbering 
  // =========================================
  //  
  // AL : added on Tue-18-apr-2006
  //
  // =========================================
  if( m_FeedthroughTest == "ON" ){
    // if test FeedThrough Numbering
    if( l_version == "fullAtlas" ){
      // if version == fullAtlas
      std::vector<HWIdentifier>::const_iterator itOnId;
      std::vector<HWIdentifier>::const_iterator itOnIdEnd;
      itOnId = m_onlineHelper->feedthrough_begin();
      itOnIdEnd = m_onlineHelper->feedthrough_end();
      for(; itOnId!=itOnIdEnd;++itOnId)
	{// Loop over feedthrough
	  const HWIdentifier ft_id = *itOnId;
	  int ftNb = m_onlineHelper->feedthrough(ft_id);
	  std::string ftName = m_onlineHelper->feedthrough_name(ft_id);
	  int side = m_onlineHelper->pos_neg(ft_id);
	  int bec  = m_onlineHelper->barrel_ec(ft_id);
	  if( bec == 0 ){
	    // BARREL
	    if( side == 0 ){
	      // EMBA
	      ATH_MSG_INFO ( " EMBA Feedthrough: " << ftNb 
                             << " Numbering : " << ftName );
	    }
	    else{
	      // EMBC
	      ATH_MSG_INFO ( " EMBC Feedthrough: " << ftNb 
                             << " Numbering : " << ftName );
	    }
	  }
	  else if( bec == 1){
	    if( side == 0){
	      // EMEC-A
	      ATH_MSG_INFO ( " EMEC-A Feedthrough: " << ftNb 
                             << " Numbering : " << ftName );
	    }
	    else{
	      // EMEC-C
	      ATH_MSG_INFO ( " EMEC-C Feedthrough: " << ftNb 
                             << " Numbering : " << ftName );
	    }
	  }	    
	}
      ATH_MSG_INFO ( "----------------------------------------" );
    }
  }
  
  // =========================================
  //     CALIB Online-wise loop (1)
  // =========================================
  // 
  //
  // =========================================
  if( m_Calibration == "ON" || m_Calibration == "EXCL" ){
    // m_Calibration==ON
    std::vector<HWIdentifier>::const_iterator itOnId;
    std::vector<HWIdentifier>::const_iterator itOnIdEnd;
    int nch=0;
    int nerror=0;
    // Test of old CalibSlot 8 identifiers
    // -----------------------------------
    HWIdentifier testId;
    try{
      testId = m_onlineHelper->channel_Id(1,0,6,8,32);
      ATH_MSG_INFO ( "[TEST] Created old FCAL OnlineID= " 
                     << m_onlineHelper->show_to_string(testId) 
                     << " Compact OnlineID= "
                     << testId.getString() );
    }
    catch(LArOnlID_Exception & except ){
      ATH_MSG_ERROR 
        (  "CALIB ....LArOnlId exception creating online id with feb and ichan " 
	  << m_emHelper->show_to_string(testId) << "   " 
	  << (std::string)except );
    }
    testId = m_onlineHelper->channel_Id(1,0,6,8,32);
    Identifier ch_offl ; 
    try {
      ch_offl = cabling->cnvToIdentifier(testId) ; 
    }
    catch(LArID_Exception & except){
      nerror++;
    }
    ATH_MSG_INFO ( "[TEST] Created old FCAL OnlineID= " 
                   << m_onlineHelper->show_to_string(testId) 
                   << " Compact OnlineID= "
                   << testId.getString()
                   << m_emHelper->show_to_string(ch_offl) 
                   << " Compact OfflineID= " 
                   << ch_offl.getString() );
    
    // Loop over Calib Modules
    // -------------------------
    itOnId = m_onlineHelper->calib_module_begin();
    itOnIdEnd = m_onlineHelper->calib_module_end();
    for(; itOnId!=itOnIdEnd;++itOnId){
      const HWIdentifier feb_id = *itOnId;
      nch++;
      for(int ichan=0; ichan<m_onlineHelper->channelInSlotMax(feb_id); ++ichan) 
	{//for ichan=0,channel_slot_max
	  HWIdentifier ch_id;
	  if( m_onlineHelper->isCalibration(feb_id) )
	    {//isOnlineConnected
	      try 
		{
		  ch_id = m_onlineHelper->channel_Id(feb_id,ichan);
		}
	      catch(LArOnlID_Exception & except)
		{
		  ATH_MSG_ERROR 
                    (  "CALIB ....LArOnlId exception creating online id with feb and ichan " 
                       << m_emHelper->show_to_string(feb_id) << "   " << ichan << " " 
                       << (std::string)except );
		}
	    }
	}
    }
    ATH_MSG_INFO ( " Tests for ATLAS Calibration Lines : " );
    ATH_MSG_INFO ( " --> Nb of Calib Modules = " << nch << "(122 expected)" );
   
    // CALIB Online-wise loop (2)
    nch=0;
    int nEmbaCalib=0;
    int nEmecCalib=0;
    int nHecCalib=0;
    int nFcalCalib=0;
    std::string mychDisc;
    int nDisc=0;
    int nConn=0;
    std::string myDet;
    if( l_version=="fullAtlas")
      {//if version==ATLAS
	itOnId = m_onlineHelper->calib_channel_begin();
	itOnIdEnd = m_onlineHelper->calib_channel_end();
	for(; itOnId!=itOnIdEnd;++itOnId)
	  {
	    const HWIdentifier ch_id = *itOnId;
	    if( m_onlineHelper->isCalibration(ch_id) )
	      {//isCalibration
		nch++;
		Identifier ch_offl ; 
		try {
		  ch_offl = cabling->cnvToIdentifier(ch_id) ; 
		}
		catch(LArID_Exception & except){
		  nerror++;
		  fileError << "ERROR " 
			    << "[ATLAS ONLINE]==> Error # " << nerror 
			    << " LArId-Exc Converting OnlineId " 
			    << m_emHelper->show_to_string(ch_id) << "   "
			    << (std::string)except
			    << std::endl;
		}
		
		mychDisc="Unknown";
		if( !cabling->isOnlineConnected(ch_id)){
		  nDisc++;
		  mychDisc="disconnected";
		}
		else{
		  nConn++;
		  mychDisc="connected";
		}
		if( m_onlineHelper->isEmBarrelOnline(ch_id) )
		  {
		    nEmbaCalib++;
		    myDet = "EMB";
		  }
		else if( m_onlineHelper->isEmEndcapOnline(ch_id) )
		  {
		    nEmecCalib++;
		    myDet = "EMEC";
		  }
		else if( m_onlineHelper->isFcalOnline(ch_id) )
		  {
		    nFcalCalib++;
		    myDet = "FCAL";
		  }
		else if( m_onlineHelper->isHecOnline(ch_id) )
		  {
		    nHecCalib++;
		    myDet = "HEC";
		  }
		fileCalib<< " Calib OnlineID= " 
			 << m_onlineHelper->show_to_string(ch_id) 
			 << " Compact OnlineID= "
			 << ch_id.getString()
			 << " OfflineID= "
			 << m_emHelper->show_to_string(ch_offl) 
			 << " Compact OfflineID= " 
			 << ch_offl.getString()
			 << " " << mychDisc << " " << myDet  
			 << std::endl;
	      }
	  }
      }
    if( l_version=="H6TestBeam")
      {//if version==H6
	itOnId = m_onlineHelper->calib_channel_begin();
	itOnIdEnd = m_onlineHelper->calib_channel_end();
	for(; itOnId!=itOnIdEnd;++itOnId)
	  {
	    const HWIdentifier ch_id = *itOnId;
	    if( m_onlineHelper->isCalibration(ch_id) )
	      {//isCalibration
		nch++;
		if( m_onlineHelper->isEmBarrelOnline(ch_id) )
		  {
		    nEmbaCalib++;
		  }
		else if( m_onlineHelper->isEmEndcapOnline(ch_id) )
		  {
		    nEmecCalib++;
		  }
		else if( m_onlineHelper->isFcalOnline(ch_id) )
		  {
		    nFcalCalib++;
		  }
		else if( m_onlineHelper->isHecOnline(ch_id) )
		  {
		    nHecCalib++;
		  }
		
	      }
	  }
      }
    if( l_version=="H6TestBeam")
      {//if version==H6
	itOnId = m_onlineHelper->calib_module_begin();
	itOnIdEnd = m_onlineHelper->calib_module_end();
	for(; itOnId!=itOnIdEnd;++itOnId)
	  {
	    const HWIdentifier feb_id = *itOnId;
	    ATH_MSG_DEBUG(  "OnlineID= "
                            << m_onlineHelper->show_to_string(feb_id) 
                            << " CompactID= "
                            <<feb_id.getString() );
	  }
      }
    if( l_version=="H8TestBeam")
      {//if version==H8
	itOnId = m_onlineHelper->calib_module_begin();
	itOnIdEnd = m_onlineHelper->calib_module_end();
	for(; itOnId!=itOnIdEnd;++itOnId)
	  {
	    const HWIdentifier feb_id = *itOnId;
	    ATH_MSG_DEBUG(  "OnlineID= "
                            << m_onlineHelper->show_to_string(feb_id) 
                            << " CompactID= "
                            <<feb_id.getString() );
	  }
      }
    
    if( l_version=="fullAtlas")
      {
	ATH_MSG_INFO ( " --> Nb of Calib Channels = " << nch 
                       << "(15,616 expected)" );
	ATH_MSG_INFO ( "       EM Barrel Channels = " << nEmbaCalib 
                       << "(8,192 expected)" );
	ATH_MSG_INFO ( "       EM Endcap Channels = " << nEmecCalib 
                       << "(4,096 expected)" );
	ATH_MSG_INFO ( "       HEC       Channels = " << nHecCalib 
                       << "(3,072 expected)" );
	ATH_MSG_INFO ( "       FCAL      Channels = " << nFcalCalib 
                       << "(256 expected)" );
	ATH_MSG_INFO ( "===============================================" );
      }
    if( l_version=="H6TestBeam")
      {
	ATH_MSG_INFO ( " --> Nb of Calib Channels = " << nch 
                       << "(384 expected)" );
	ATH_MSG_INFO ( "       EM Endcap Channels = " << nEmecCalib 
                       << "(128 expected)" );
	ATH_MSG_INFO ( "       HEC       Channels = " << nHecCalib 
                       << "(128 expected)" );
	ATH_MSG_INFO ( "       FCAL      Channels = " << nFcalCalib 
                       << "(128 expected)" );
	ATH_MSG_INFO ( "===============================================" );
      }
  } // if m_Calibration = "ON"     
  

      
  // ============================================================================
  //
  //              Test of Offline Identifiers 
  // ============================================================================
  // 
  //   - 06-OCT-2006 AL removed LArTTChannelID reference. Should now be LArTTOnlineID 
  //
  //   - 02-AUG-2006 AL added disconnected channels tests / subdetectors
  //                    added new property to run over disc channels only
  //   - 18-APR-2006 AL debugged decodeSignalChannelFCAL()
  //
  // ============================================================================
  //
  if( m_OfflineTest == "ON" && m_HighVoltage == "OFF" )
    {// if OFFlineTest == ON
    // Display on screen
      ATH_MSG_INFO ( "========================================  " );
      ATH_MSG_INFO ( "  >>> Tests for LArOffline Identifiers <<< " );
      ATH_MSG_INFO ( "========================================  " );
      int nch=0;
      int nSam0=0;
      int nSam1=1;
      int nSam2=2;
      int nSam3=3;
      int nch_old = 0;
      int nConn=0;
      int nDisc=0;
      int nATLASconn=0;
      int nATLASdisc=0;
      int nEMconn=0;
      int nEMBconn=0;
      int nEMECconn=0;
      int nFCALconn=0;
      int nHECconn=0;
      int nEMdisc=0;
      int nEMBdisc=0;
      int nEMECdisc=0;
      int nFCALdisc=0;
      int nHECdisc=0;
      std::string mychDisc;
      std::string mychStat;
      std::string myDet="unknown";
      if( m_Detector == "ALL" || m_Detector == "EMB" || m_Detector == "EMEC" || m_Detector == "EM" ) 
	{// if m_Detector == EM or EMB or EMEC
	  if( Connected == "ON" || Connected == "ALL" ){
	    // if m_connected == ON 
	    std::vector<Identifier>::const_iterator itId = m_emHelper->em_begin();
	    std::vector<Identifier>::const_iterator itIdEnd = m_emHelper->em_end();
	    for(; itId!=itIdEnd;++itId)
	      {// Loop over EM connected channels
		nATLASconn++;
		const Identifier ch_id = *itId;
		HWIdentifier ch_onl = cabling->createSignalChannelID(ch_id) ; 
		Identifier id_test = cabling->cnvToIdentifier( ch_onl ) ;
		ATH_MSG_VERBOSE( m_emHelper->show_to_string( ch_onl )
                                 <<"  "<< m_emHelper->show_to_string(ch_id) );
		if( ch_id != id_test ) { 
		  ATH_MSG_ERROR("online ID, returned offline ID= " 
                                << m_emHelper->show_to_string(ch_onl) << ", " 
                                << m_emHelper->show_to_string(id_test) );
		  break ; 
		}
		// test hash and neighbours
		std::vector<IdentifierHash> neighbourList;
		IdentifierHash hashId=m_emHelper->channel_hash(ch_id);
		m_emHelper->get_neighbours(hashId, LArNeighbours::all3D, neighbourList);
		std::vector<IdentifierHash>::iterator first=neighbourList.begin();
		std::vector<IdentifierHash>::iterator last=neighbourList.end();
		for (;last!=first; first++){
                  ATH_MSG_VERBOSE ( "  neighbour list EM = " << (unsigned int)(*first) );
		}


		if( m_Detector == "EM" || m_Detector == "ALL" ){
		  nEMconn++;
		  if(nEMconn==nch_old+1000){
		    ATH_MSG_INFO ( "[ATLAS] processing EM connected offline channel ..# " 
                                   << nEMconn );
		    nch_old=nEMconn;
		  }
		  if( m_emHelper->sampling(ch_id) == 0 ){
		    nSam0++;
		  }
		  if( m_emHelper->sampling(ch_id) == 1 ){
		    nSam1++;
		  }
		  if( m_emHelper->sampling(ch_id) == 2 ){
		    nSam2++;
		  }
		  if( m_emHelper->sampling(ch_id) == 3 ){
		    nSam3++;
		  }
		  if( cabling->isOnlineConnected( ch_onl )){
		    nConn++;
		  }
		  else{
		    nDisc++;
		  }
		}
		if( m_Detector == "EMB"  || m_Detector == "ALL"){
		  if( m_emHelper->barrel_ec(ch_id) == -1 || 
		      m_emHelper->barrel_ec(ch_id) == 1 ){
		    // if EMBARREL
		    nEMBconn++;
		    if(nEMBconn==nch_old+1000){
		      ATH_MSG_INFO ( "[ATLAS] processing EMB connected offline channel ..# " 
                                     << nEMBconn );
		      nch_old=nEMBconn;
		    }
		    if( m_Detector != "ALL"){
		      if( m_emHelper->sampling(ch_id) == 0 ){
			nSam0++;
		      }
		      if( m_emHelper->sampling(ch_id) == 1 ){
			nSam1++;
		      }
		      if( m_emHelper->sampling(ch_id) == 2 ){
			nSam2++;
		      }
		      if( m_emHelper->sampling(ch_id) == 3 ){
			nSam3++;
		      }
		    }
		    if( cabling->isOnlineConnected( ch_onl )){
		      nConn++;
		      mychDisc="Connected";
		    }
		    else{
		      mychDisc="disconnected";
		      nDisc++;
		    }
		    // Output...
		    myDet="(EMB)";
		    fileEmbOff << " OfflineID= "
			       << m_emHelper->show_to_string(ch_id) 
			       << " Compact OfflineID= " 
			       << ch_id.getString()
			       << " OnlineID= " 
			       << m_onlineHelper->show_to_string(ch_onl) 
			       << " Compact OnlineID= "
			       << ch_onl.getString()
			       << " " << mychDisc << " " << myDet  
			       << std::endl;
		  }
		}
		if( m_Detector == "EMEC"  || m_Detector == "ALL"){
		  if( m_emHelper->barrel_ec(ch_id)== -2 || 
		      m_emHelper->barrel_ec(ch_id) == 2 || 
		      m_emHelper->barrel_ec(ch_id) ==-3 || 
		      m_emHelper->barrel_ec(ch_id) == 3 ){
		    // if EMEC
		    nEMECconn++;
		    if(nEMECconn==nch_old+1000){
		      ATH_MSG_INFO ( "[ATLAS] processing EMEC connected offline channel ..# " 
                                     << nEMECconn );
		      nch_old=nEMECconn;
		    }
		    if( m_Detector != "ALL"){
		      if( m_emHelper->sampling(ch_id) == 0 ){
			nSam0++;
		      }
		      if( m_emHelper->sampling(ch_id) == 1 ){
			nSam1++;
		      }
		      if( m_emHelper->sampling(ch_id) == 2 ){
			nSam2++;
		      }
		      if( m_emHelper->sampling(ch_id) == 3 ){
			nSam3++;
		      }
		    }
		    if( cabling->isOnlineConnected( ch_onl )){
		      nConn++;
		      mychDisc="Connected";
		    }
		    else{
		      nDisc++;
		      mychDisc="disconnected";
		    }
		    // Output...
		    myDet="(EMEC)";
		    fileEmecOff << " OfflineID= "
				<< m_emHelper->show_to_string(ch_id) 
				<< " Compact OfflineID= " 
				<< ch_id.getString()
				<< " OnlineID= " 
				<< m_onlineHelper->show_to_string(ch_onl) 
				<< " Compact OnlineID= "
				<< ch_onl.getString()
				<< " " << mychDisc << " " << myDet  
				<< std::endl;
		  }
		}
	      }
	  }
	}
      if(  m_Detector == "HEC" || m_Detector == "ALL" ) 
	{
	  // OFFLINE loop over all HEC channels

	  if( Connected == "ON" || Connected == "ALL" ){
	    // if Connected == ON
	    // Offline : loop over connected HEC channels
	    // ------------------------------------------
	    std::vector<Identifier>::const_iterator itId = m_hecHelper->hec_begin();
	    std::vector<Identifier>::const_iterator itIdEnd = m_hecHelper->hec_end();
	    for(; itId!=itIdEnd;++itId){
	      // loop over connected HEC channels
	      nch++;
	      nHECconn++;
	      nATLASconn++;
	      if(nATLASconn==nch_old+200){
		ATH_MSG_INFO ( "[ATLAS] processing HEC connected offline channel ..# " 
                               << nATLASconn );
		nch_old=nATLASconn;
	      }
	      Identifier ch_id = *itId;
	      HWIdentifier ch_onl = cabling->createSignalChannelID(ch_id) ; 
	      Identifier id_test = cabling->cnvToIdentifier( ch_onl ) ;
	      ATH_MSG_VERBOSE( m_hecHelper->show_to_string( ch_onl )
                               <<"  "<< m_hecHelper->show_to_string(ch_id) );
	      if( ch_id != id_test ) { 
                ATH_MSG_ERROR(" HEC: Error in mapping for offline id=  " << m_hecHelper->show_to_string( ch_id ) );
                ATH_MSG_ERROR("online ID, returned offline ID= " << m_hecHelper->show_to_string(ch_onl) << ", " << m_hecHelper->show_to_string(id_test) );
		break ; 
	      }

	      // test hash and neighbours
	      std::vector<IdentifierHash> neighbourList;
	      IdentifierHash hashId=m_hecHelper->channel_hash(ch_id);
	      m_hecHelper->get_neighbours(hashId, LArNeighbours::all3D, neighbourList);
	      std::vector<IdentifierHash>::iterator first=neighbourList.begin();
	      std::vector<IdentifierHash>::iterator last=neighbourList.end();
	      for (;last!=first; first++){
		ATH_MSG_VERBOSE ( "  neighbour list HEC = " << (unsigned int)(*first) );
	      }

	      // Output...
	      mychDisc="Connected";
	      myDet="(HEC)";
	      fileHecOff << " OfflineID= "
			 << m_emHelper->show_to_string(ch_id) 
			 << " Compact OfflineID= " 
			 << ch_id.getString()
			 << " OnlineID= " 
			 << m_onlineHelper->show_to_string(ch_onl) 
			 << " Compact OnlineID= "
			 << ch_onl.getString()
			 << " " << mychDisc << " " << myDet  
			 << std::endl;	    
	    }
	  }
	} // HEC channels

      if( m_Detector == "ALL" || m_Detector == "FCAL" ) {
	// Offline : Loop over all FCAL channels
	
	if( Connected == "ON" || Connected == "ALL" ){
	  // if Connected == ON
	  // Offline : Loop over disconnected FCAL channels
	  // ---------------------------------
	  nch_old = 0;
	  std::vector<Identifier>::const_iterator itId = m_fcalHelper->fcal_begin();
	  std::vector<Identifier>::const_iterator itIdEnd = m_fcalHelper->fcal_end();
	  for(; itId!=itIdEnd;++itId){
	    // Loop over FCAL connected channels
	    nch++;
	    nFCALconn++;
	    nATLASconn++;
	    if(nATLASconn==nch_old+500){
	      ATH_MSG_INFO ( "[ATLAS] processing FCAL connected offline channel ..# " 
                             << nATLASconn );
	      nch_old=nATLASconn;
	    }
	    Identifier ch_id = *itId;
	    HWIdentifier ch_onl = cabling->createSignalChannelID(ch_id) ; 
	    Identifier id_test = cabling->cnvToIdentifier( ch_onl ) ;
	    ATH_MSG_VERBOSE(  m_fcalHelper->show_to_string( ch_onl ) 
                              <<"  "<< m_fcalHelper->show_to_string(ch_id) );
	    if( ch_id != id_test ) { 
              ATH_MSG_ERROR(" FCAL: Error in mapping for offline id= " << m_fcalHelper->show_to_string( ch_id ));
              ATH_MSG_ERROR("online ID, returned offline ID= " 
                            << m_fcalHelper->show_to_string(ch_onl) << ", " 
                            << m_fcalHelper->show_to_string(id_test) );
	      break ; 
	    }

	    // test hash and neighbours
	    std::vector<IdentifierHash> neighbourList;
	    IdentifierHash hashId=m_fcalHelper->channel_hash(ch_id);
	    m_fcalHelper->get_neighbours(hashId, LArNeighbours::all3D, neighbourList);
	    std::vector<IdentifierHash>::iterator first=neighbourList.begin();
	    std::vector<IdentifierHash>::iterator last=neighbourList.end();
	    for (;last!=first; first++){
              ATH_MSG_VERBOSE ( "  neighbour list FCAL = " << (unsigned int)(*first) );
	    }
	    
	    // Output...
	    mychDisc="Connected";
	    myDet="(FCAL)";
	    fileFcalOff << " OfflineID= "
			<< m_emHelper->show_to_string(ch_id) 
			<< " Compact OfflineID= " 
			<< ch_id.getString()
			<< " OnlineID= " 
			<< m_onlineHelper->show_to_string(ch_onl) 
			<< " Compact OnlineID= "
			<< ch_onl.getString()
			<< " " << mychDisc << " " << myDet  
			<< std::endl;
	  }
	}
      } // FCAL channels

      if( l_version=="fullAtlas" )
	{// if version == fullAtlas
	  ATH_MSG_INFO ( "===============================================" );
	  if( m_Detector == "ALL"){
	    ATH_MSG_INFO ( "    Total number of ATLAS channels " );
	    ATH_MSG_INFO ( "    - connected channels    : " << nATLASconn << " (ref=182468)" );
	    ATH_MSG_INFO ( "    - discconnected channels:   " << nATLASdisc << " (ref=  5948)" );
	    ATH_MSG_INFO ( "===============================================" );
	  }
	  if( m_Detector == "EMB" || m_Detector == "ALL" ){
	    ATH_MSG_INFO ( "    Total number of channels in EMB : " );
	    ATH_MSG_INFO ( "    - connected channels    = " << nEMBconn <<  " (ref=109568)" );
	    ATH_MSG_INFO ( "    - disconnected channels =   " << nEMBdisc <<  " (ref=  1024)" );
	    if( m_Detector != "ALL" ){
	      ATH_MSG_INFO ( "    - channels in sampling 0= " << nSam0 << " (ref= 7808)" );
	      ATH_MSG_INFO ( "    - channels in sampling 1= " << nSam1 << " (ref=58753)" );
	      ATH_MSG_INFO ( "    - channels in sampling 2= " << nSam2 << " (ref=29186)" );
	      ATH_MSG_INFO ( "    - channels in sampling 3= " << nSam3 << " (ref=13827)" );
	    }
	  }
	  if( m_Detector == "EMEC" || m_Detector == "ALL" ){
	    ATH_MSG_INFO ( " +++++++++++++++++++++++++++++++++++++++++++++++++" );
	    ATH_MSG_INFO ( "    Total number of channels in EMEC : " );
	    ATH_MSG_INFO ( "    - connected channels    =  " << nEMECconn <<  " (ref=63744)" );
	    ATH_MSG_INFO ( "    - disconnected channels =   " << nEMECdisc <<  " (ref= 4352)" );
	    if( m_Detector != "ALL" ){
	      ATH_MSG_INFO ( "    - channels in sampling 0= " << nSam0 << " (ref= 1536)" );
	      ATH_MSG_INFO ( "    - channels in sampling 1= " << nSam1 << " (ref=28545)" );
	      ATH_MSG_INFO ( "    - channels in sampling 2= " << nSam2 << " (ref=23426)" );
	      ATH_MSG_INFO ( "    - channels in sampling 3= " << nSam3 << " (ref=10243)" );
	    }
	  }
	  if( m_Detector == "FCAL" || m_Detector == "ALL" ){
	    ATH_MSG_INFO ( " +++++++++++++++++++++++++++++++++++++++++++++++++" );
	    ATH_MSG_INFO ( "    Total channels in FCAL : " );
	    ATH_MSG_INFO ( "    - connected channels    =   " << nFCALconn << " (ref=3524)" );
	    ATH_MSG_INFO ( "    - disconnected channels =     " << nFCALdisc << " (ref=  60)" );
	  }
	  if( m_Detector == "HEC" || m_Detector == "ALL" ){
	    ATH_MSG_INFO ( " +++++++++++++++++++++++++++++++++++++++++++++++++" );
	    ATH_MSG_INFO ( "    Total number of channels in HEC : " );
	    ATH_MSG_INFO ( "    - connected channels    =   " << nHECconn <<  " (ref=5632)" );
	    ATH_MSG_INFO ( "    - disconnected channels =    " << nHECdisc <<  " (ref= 512)" );
	  }
	  if( m_Detector == "EM" || m_Detector == "ALL" ){
	    ATH_MSG_INFO ( " +++++++++++++++++++++++++++++++++++++++++++++++++" );
	    ATH_MSG_INFO ( "    Total number of channels in EM : " );
	    ATH_MSG_INFO ( "    - connected channels    = " << nEMconn << " (ref=173312)" );
	    ATH_MSG_INFO ( "    - disconnected channels =   " << nEMdisc << " (ref=  5376)" );
	    ATH_MSG_INFO ( "    - channels in sampling 0= " << nSam0 << " (ref= 9344)" );
	    ATH_MSG_INFO ( "    - channels in sampling 1= " << nSam1 << " (ref=87297)" );
	    ATH_MSG_INFO ( "    - channels in sampling 2= " << nSam2 << " (ref=52610)" );
	    ATH_MSG_INFO ( "    - channels in sampling 3= " << nSam3 << " (ref=24067)" );
	    ATH_MSG_INFO ( "===============================================" );
	  }    
	  fileEmecOff.close();
	  fileHecOff.close();
	  fileFcalOff.close();
	  fileEmbOff.close();
	  ATH_MSG_INFO ( "===============================================" );
	}
    }
  


  
  
  return StatusCode::SUCCESS ;
  
}

/********************************************************/
StatusCode TestLArHWID_Algo::finalize(){
	
  ATH_MSG_INFO ( " finalizing " );
  return StatusCode::SUCCESS ; 
}
