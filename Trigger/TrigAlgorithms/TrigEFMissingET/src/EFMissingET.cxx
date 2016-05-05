/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     EFMissingET.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET
// 
// AUTHOR:   Rashid Djilkibaev, Diego Casadei
// CREATED:  March 1, 2006
//
// Description: EF MissingET algorithm.
// ********************************************************************


#include "TrigEFMissingET/EFMissingET.h"

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigEFMissingET/EFMissingETBaseTool.h"
#include "TrigMissingEtEvent/TrigMissingET.h"
#include "TrigMissingEtEvent/TrigMissingETContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "xAODTrigMissingET/TrigMissingET.h"
#include "xAODTrigMissingET/TrigMissingETContainer.h"
//#include "TrigNavigation/Navigation.h" // Added for "Hack"
//#include "TrigNavigation/NavigationCore.icc" // Added for "Hack"

#include "xAODEventInfo/EventInfo.h"

// #include "TrigSteeringEvent/detectormask.h"
#include "eformat/DetectorMask.h"
#include "eformat/SourceIdentifier.h"

#include <cstdio>
using namespace std;

class ISvcLocator;

EFMissingET::EFMissingET(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::AllTEAlgo(name, pSvcLocator),
  m_cachedTE(0), m_useCachedResult(false), m_tools( this )
{
  declareProperty("MissingETOutputKey",m_metOutputKey = "TrigEFMissingET", "label for the MET feature in the HLT navigation");
  declareProperty("Tools", m_tools, "list of missEt tools");
  declareProperty("doTimers", m_doTimers = true, "switch on/off internal timers");
  declareProperty("DecodeDetMask", m_decodeDetMask = false, "switch on/off DetMask decoding");
  declareProperty("doTopoClusters", m_doTopoClusters = false, "run with or without topo. clusters");
  declareProperty("doJets", m_doJets = false, "run with or without jets");
  declareProperty("doPUC", m_doPUC = false, "run with or without pile-up correction fit");
  declareProperty("ComponentFlags",  m_flags,  "(vector) set to -1 to switch off a component");
  declareProperty("ComponentCalib0", m_calib0, "(vector) additive calibration constants");
  declareProperty("ComponentCalib1", m_calib1, "(vector) multiplicative calib. constants");

  declareMonitoredVariable("EF_MEx_log",   m_mex_log);
  declareMonitoredVariable("EF_MEy_log",   m_mey_log);
  declareMonitoredVariable("EF_MEz_log",   m_mez_log);
  declareMonitoredVariable("EF_MET_log",   m_met_log);
  declareMonitoredVariable("EF_ME_log",    m_me_log);
  declareMonitoredVariable("EF_SumEt_log", m_set_log);
  declareMonitoredVariable("EF_SumE_log",  m_se_log);

  declareMonitoredVariable("EF_MEx_lin",   m_mex_lin);
  declareMonitoredVariable("EF_MEy_lin",   m_mey_lin);
  declareMonitoredVariable("EF_MEz_lin",   m_mez_lin);
  declareMonitoredVariable("EF_MET_lin",   m_met_lin);
  declareMonitoredVariable("EF_ME_lin",    m_me_lin);
  declareMonitoredVariable("EF_SumEt_lin", m_set_lin);
  declareMonitoredVariable("EF_SumE_lin",  m_se_lin);
  declareMonitoredVariable("EF_XS", m_xs);  
  declareMonitoredVariable("EF_MET_phi",   m_phi);

  declareMonitoredVariable("EMRegSelTime",     m_tool_time_00);
  declareMonitoredVariable("EMLoadColTime",    m_tool_time_01);
  declareMonitoredVariable("EMLoopTime",       m_tool_time_02);
  declareMonitoredVariable("HECRegSelTime",    m_tool_time_10);
  declareMonitoredVariable("HECLoadColTime",   m_tool_time_11);
  declareMonitoredVariable("HECLoopTime",      m_tool_time_12);
  declareMonitoredVariable("TileRegSelTime",   m_tool_time_20);
  declareMonitoredVariable("TileLoadColTime",  m_tool_time_21);
  declareMonitoredVariable("TileLoopTime",     m_tool_time_22);
  declareMonitoredVariable("FCalRegSelTime",   m_tool_time_30);
  declareMonitoredVariable("FCalLoadColTime",  m_tool_time_31);
  declareMonitoredVariable("FCalLoopTime",     m_tool_time_32);

//  declareMonitoredVariable("TotalTime",        m_tool_time_TOT);
  declareMonitoredVariable("TotalEMTime",      m_tool_time_EM);
  declareMonitoredVariable("TotalHECTime",     m_tool_time_HEC);
  declareMonitoredVariable("TotalTileTime",    m_tool_time_Tile);
  declareMonitoredVariable("TotalFCalTime",    m_tool_time_FCal);
  declareMonitoredVariable("TotalRegSelTime",  m_tool_time_RegSel);
  declareMonitoredVariable("TotalLoadColTime", m_tool_time_LoadCol);
  declareMonitoredVariable("TotalLoopTime",    m_tool_time_Loop);

  // helper object
  unsigned char N=38; // number of components   
  m_met_help = new TrigEFMissingEtHelper(N);
  m_flags.reserve(N);  
  m_calib0.reserve(N);
  m_calib1.reserve(N);
  m_comp_index.reserve(N);
  m_comp_ex_log.reserve(N);
  m_comp_ey_log.reserve(N);
  m_comp_ez_log.reserve(N);
  m_comp_et_log.reserve(N);
  m_comp_sum_et_log.reserve(N);
  m_comp_sum_e_log.reserve(N);
  m_comp_status_flag.reserve(N);
  m_comp_et_lin.reserve(N);
  m_comp_sum_et_lin.reserve(N);

  for (int i=0; i<N; ++i) {
    m_comp_index.push_back(i);
    m_comp_ex_log.push_back(0);
    m_comp_ey_log.push_back(0);
    m_comp_ez_log.push_back(0);
    m_comp_et_log.push_back(0);
    m_comp_sum_et_log.push_back(0);
    m_comp_sum_e_log.push_back(0);
    m_comp_status_flag.push_back(0);
    m_comp_et_lin.push_back(0);
    m_comp_sum_et_lin.push_back(0);
  }

  declareMonitoredStdContainer("compN",      m_comp_index);
  declareMonitoredStdContainer("compEx",     m_comp_ex_log);
  declareMonitoredStdContainer("compEy",     m_comp_ey_log);
  declareMonitoredStdContainer("compEz",     m_comp_ez_log);
  declareMonitoredStdContainer("compEt",     m_comp_et_log);
  declareMonitoredStdContainer("compSumEt",  m_comp_sum_et_log);
  declareMonitoredStdContainer("compSumE",   m_comp_sum_e_log);
  declareMonitoredStdContainer("compEt_lin",     m_comp_et_lin);
  declareMonitoredStdContainer("compSumEt_lin",  m_comp_sum_et_lin);

  declareMonitoredStdContainer("compStatus", m_comp_status_flag);
//   declareMonitoredCustomVariable("compStatus", m_comp_status_flag);

  m_status_flag.reserve(32); // to map a 32-bit word
  m_status_flag.assign(32, 0);
  declareMonitoredStdContainer("EF_MET_status", m_status_flag);

  m_totTime=0;
  m_algTime=0;

  m_StoreGate=0;
  m_current_run_id=0;
  m_current_lbk_id=0;
  m_current_evt_id=0;
  m_current_bcg_id=0;
  m_LArEMbarrelAside=true;
  m_LArEMbarrelCside=true;
  m_LArEMendCapAside=true;
  m_LArEMendCapCside=true;
  m_LArHECendCapAside=true;
  m_LArHECendCapCside=true;
  m_LArFCalAside=true;
  m_LArFCalCside=true;
  m_TileBarrelAside=true;
  m_TileBarrelCside=true;
  m_TileExtBarAside=true;
  m_TileExtBarCside=true;

  m_caloCluster=0; 
  m_jets=0;

  // // see the base tool for all other status bits
  // m_maskEMB_A_Missing      = 0x00010000; // bit 16
  // m_maskEMB_C_Missing      = 0x00020000; // bit 17
  // m_maskEME_A_Missing      = 0x00040000; // bit 18
  // m_maskEME_C_Missing      = 0x00080000; // bit 19
  // m_maskHEC_A_Missing      = 0x00100000; // bit 20
  // m_maskHEC_C_Missing      = 0x00200000; // bit 21
  // m_maskFCAL_A_Missing     = 0x00400000; // bit 22
  // m_maskFCAL_C_Missing     = 0x00800000; // bit 23
  // m_maskTileB_A_Missing    = 0x01000000; // bit 24
  // m_maskTileB_C_Missing    = 0x02000000; // bit 25
  // m_maskTileE_A_Missing    = 0x04000000; // bit 26
  // m_maskTileE_C_Missing    = 0x08000000; // bit 27

  //Initialize some variables to make coverity happy,
  //even though not needed in principle
  
  m_met=0;
  firsteventinrun=false;
  n_sizePers=0;

}

//////////////////////////////////////////////////////////
HLT::ErrorCode EFMissingET::hltFinalize()
{
  return HLT::OK;
}
//////////////////////////////////////////////////////////
HLT::ErrorCode EFMissingET::hltInitialize()
{

  // Initialize timers
  if (doTiming() && m_doTimers) {
    m_totTime=addTimer("EFMissingETInternalTotalTime");
    m_algTime=addTimer("EFMissingETtotalAlgToolTime");
    if (m_totTime==0 || m_algTime==0) {
      msg() << MSG::WARNING << "not able to initialize timer!" << endreq;
    }
  }

  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG <<"Initializing EFMissingET" << endreq;
  }

  if(m_tools.retrieve().isFailure() ) {
    msg() << MSG::ERROR << "Failed retrieve tools " << m_tools << endreq;
  } else {
    if(msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG <<"Successfully retrieve tools " << m_tools << endreq;
    }
  }

  int Nc = m_met_help->GetElements();
  int Nf = m_flags.size();
  if(Nf>0){
    msg() << MSG::INFO << "ComponentFlags has size " << Nf << endreq;
    if (Nf!=Nc) {
      msg() << MSG::ERROR 
        << "ComponentFlags has size different from the number "
        << Nc << " of components.  Aborting" << endreq;
      return HLT::ERROR;
    } else {
      for (unsigned char i=0; i<Nf; ++i) {
        if (m_flags[i]<0)
          (m_met_help->GetComponent(i))->m_skip = true;
      }
    }
  } 

  int N0 = m_calib0.size();
  if (N0>0) {
    msg() << MSG::INFO << "ComponentCalib0 has size " << N0 << endreq;
    if (N0!=Nc) {
      msg() << MSG::ERROR 
        << "ComponentCalib0 has size different from the number "
        << Nc << " of components.  Aborting" << endreq;
      return HLT::ERROR;
    } else {
      for (unsigned char i=0; i<N0; ++i) {
        (m_met_help->GetComponent(i))->m_calib0 = m_calib0[i];
      }
    }
  }

  int N1 = m_calib1.size();
  if (N1>0) {
    msg() << MSG::INFO << "ComponentCalib1 has size " << N1 << endreq;
    if (N1!=Nc) {
      msg() << MSG::ERROR 
        << "ComponentCalib1 has size different from the number "
        << Nc << " of components.  Aborting" << endreq;
      return HLT::ERROR;
    } else {
      for (unsigned char i=0; i<N1; ++i) {
        (m_met_help->GetComponent(i))->m_calib1 = m_calib1[i];
      }
    }
  }

  return HLT::OK;
}

//////////////////////////////////////////////////////////

EFMissingET::~EFMissingET(){
  if (m_met_help) {
    delete m_met_help; 
    m_met_help = 0;
  }
}

//////////////////////////////////////////////////////////

HLT::ErrorCode EFMissingET::hltBeginRun() {

  msg() << MSG::DEBUG << " EFMissingET::hltBeginRun() called " << endreq;

  // access StoreGate
  m_StoreGate = store();
  if (m_StoreGate==0) {
    msg() << MSG::ERROR << "Can not access StoreGate" << endreq;
    return HLT::SG_ERROR;
  }

  // Check if configuration is sane
  bool foundTopo = false; bool foundFEB = false; bool foundCell = false; bool foundJets = false;

  if(msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << " loop through helper tools start " << endreq;

  for (ToolHandleArray<EFMissingETBaseTool>::iterator it = m_tools.begin(); it != m_tools.end(); ++it ) {
  
   if(msgLvl() <= MSG::DEBUG)
     msg() << MSG::DEBUG << (*it)->name() << endreq;
  
      if((*it)->getFexType() == FexType::TOPO) foundTopo = true;
      if((*it)->getFexType() == FexType::CELL) foundCell = true;
      if((*it)->getFexType() == FexType::FEB)  foundFEB = true;
      if((*it)->getFexType() == FexType::JET)  foundJets = true;
      
  }
  
  if(foundTopo && (foundFEB || foundCell)) {
  
    if(msgLvl() <= MSG::DEBUG)
     msg() << MSG::DEBUG << "found topo. clusters AND Cells OR FEB - not a valid configuration .. aborting " << endreq;   
  
  	return HLT::ERROR;  
  }
  
  if(m_doTopoClusters && !foundTopo) {

    if(msgLvl() <= MSG::DEBUG)
     msg() << MSG::DEBUG << "found topo. clusters config but no ClusterTool .. aborting " << endreq;     
  	
  	return HLT::ERROR;  
  }
  
  if(m_doJets && !foundJets) {

    if(msgLvl() <= MSG::DEBUG)
     msg() << MSG::DEBUG << "found jet config but no JetTool .. aborting " << endreq;     
  	
  	return HLT::ERROR;  
  }  
 
  
  // --

  firsteventinrun = true;

  return HLT::OK; 
}

//////////////////////////////////////////////////////////

HLT::ErrorCode EFMissingET::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in,
    unsigned int type_out) {

  if (doTiming() && m_doTimers && m_totTime) {
    m_totTime->start();
  }

  // CACHING
  if (m_useCachedResult) { // check whether we executed this instance before
    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Executing this EFMissingET " << name() << " in cached mode" << endreq;
    }

    // Only count MET as an input TE (for seeding relation of navigation structure)
    HLT::TEVec allTEs;
    if ( (tes_in.size()>0) && (tes_in[0].size()>0) ) allTEs.push_back( tes_in[0][0] );

    // Create an output TE seeded by the inputs
    HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(allTEs, type_out);
    outputTE->setActiveState(true);

    // save (cached) met feature to output TE:
    m_config->getNavigation()->copyAllFeatures( m_cachedTE, outputTE );

    return HLT::OK;
  }

  // start monitoring
  beforeExecMonitors().ignore();

  //initialize monitored variables to impossible values
  m_mex_log = -9e9;
  m_mey_log = -9e9;
  m_mez_log = -9e9;
  m_met_log = -9e9;
  m_me_log  = -9e9;
  m_set_log = -9e9;
  m_se_log  = -9e9;
  m_mex_lin = -9e9;
  m_mey_lin = -9e9;
  m_mez_lin = -9e9;
  m_met_lin = -9e9;
  m_me_lin  = -9e9;
  m_set_lin = -9e9;
  m_se_lin  = -9e9;
  
  m_xs      = -9e9;
  
  m_phi = -9e9;
  
  m_tool_time_00 = 0;
  m_tool_time_01 = 0;
  m_tool_time_02 = 0;
  m_tool_time_10 = 0;
  m_tool_time_11 = 0;
  m_tool_time_12 = 0;
  m_tool_time_20 = 0;
  m_tool_time_21 = 0;
  m_tool_time_22 = 0;
  m_tool_time_30 = 0;
  m_tool_time_31 = 0;
  m_tool_time_32 = 0;
  //   m_tool_time_TOT = 0;
  m_tool_time_EM = 0;
  m_tool_time_HEC = 0;
  m_tool_time_Tile = 0;
  m_tool_time_FCal = 0;
  m_tool_time_RegSel = 0;
  m_tool_time_LoadCol = 0;
  m_tool_time_Loop = 0;

  m_jets = 0;
  m_caloCluster = 0;

  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Executing EFMissingET::hltExecute()" << endreq;
    //      msg() << MSG::DEBUG << "outputTE->label(): " /* << outputTE->label() */
    //            << endreq;
  }

  if(firsteventinrun) {
    msg() << MSG::DEBUG << "REGTEST: First event in run" << endreq;
    if (m_StoreGate) {
      const xAOD::EventInfo* pEvent(0);
      StatusCode sc = m_StoreGate->retrieve(pEvent);
      if ( sc.isFailure() ) {
        msg() << MSG::ERROR << "Cannot find xAOD::EventInfo object" << endreq;
      } else {
        m_current_run_id = pEvent->runNumber();
        m_current_lbk_id = pEvent->lumiBlock();
        m_current_evt_id = pEvent->eventNumber();
        m_current_bcg_id = pEvent->bcid();
        char buff[512];
        snprintf(buff,512,
             "REGTEST: Run number = %11u, luminosity block = %11u, event number = %11u, bunch crossing = %11u",
             m_current_run_id, m_current_lbk_id, m_current_evt_id, m_current_bcg_id);
        msg() << MSG::DEBUG << buff << endreq;

        m_LArEMbarrelAside=true;
        m_LArEMbarrelCside=true;
        m_LArEMendCapAside=true;
        m_LArEMendCapCside=true;
        m_LArHECendCapAside=true;
        m_LArHECendCapCside=true;
        m_LArFCalAside=true;
        m_LArFCalCside=true;
        m_TileBarrelAside=true;
        m_TileBarrelCside=true;
        m_TileExtBarAside=true;
        m_TileExtBarCside=true;

        if (m_decodeDetMask) {
	  uint64_t mask64 = pEvent->detectorMask();
          if(msgLvl() <= MSG::DEBUG){
            char buff[512];
            snprintf(buff,512,"REGTEST: DetMask_1 = 0x%08lu",mask64);
            msg() << MSG::DEBUG << buff << endreq;
          }

          if (!(mask64==0)) {  // 0 means present

            eformat::helper::DetectorMask dm(mask64);
            m_LArEMbarrelAside  = dm.is_set(eformat::LAR_EM_BARREL_A_SIDE);
            m_LArEMbarrelCside  = dm.is_set(eformat::LAR_EM_BARREL_C_SIDE);
            m_LArEMendCapAside  = dm.is_set(eformat::LAR_EM_ENDCAP_A_SIDE);
            m_LArEMendCapCside  = dm.is_set(eformat::LAR_EM_ENDCAP_C_SIDE);
            m_LArHECendCapAside = dm.is_set(eformat::LAR_HAD_ENDCAP_A_SIDE);
            m_LArHECendCapCside = dm.is_set(eformat::LAR_HAD_ENDCAP_C_SIDE);
            m_LArFCalAside      = dm.is_set(eformat::LAR_FCAL_A_SIDE);
            m_LArFCalCside      = dm.is_set(eformat::LAR_FCAL_C_SIDE);
            m_TileBarrelAside   = dm.is_set(eformat::TILECAL_BARREL_A_SIDE);
            m_TileBarrelCside   = dm.is_set(eformat::TILECAL_BARREL_C_SIDE);
            m_TileExtBarAside   = dm.is_set(eformat::TILECAL_EXT_A_SIDE);
            m_TileExtBarCside   = dm.is_set(eformat::TILECAL_EXT_C_SIDE);

            if(!m_LArEMbarrelAside && msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "LAR_EM_BARREL_A_SIDE is absent!" << endreq;
            if(!m_LArEMbarrelCside && msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "LAR_EM_BARREL_C_SIDE is absent!" << endreq;
            if(!m_LArEMendCapAside && msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "LAR_EM_ENDCAP_A_SIDE is absent!" << endreq;
            if(!m_LArEMendCapCside && msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "LAR_EM_ENDCAP_C_SIDE is absent!" << endreq;
            if(!m_LArHECendCapAside && msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "LAR_HAD_ENDCAP_A_SIDE is absent!" << endreq;
            if(!m_LArHECendCapCside && msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "LAR_HAD_ENDCAP_C_SIDE is absent!" << endreq;
            if(!m_LArFCalAside && msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "LAR_FCAL_A_SIDE is absent!" << endreq;
            if(!m_LArFCalCside && msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "LAR_FCAL_C_SIDE is absent!" << endreq;
            if(!m_TileBarrelAside && msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "TILECAL_BARREL_A_SIDE is absent!" << endreq;
            if(!m_TileBarrelCside && msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "TILECAL_BARREL_C_SIDE is absent!" << endreq;
            if(!m_TileExtBarAside && msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "TILECAL_EXT_A_SIDE is absent!" << endreq;
            if(!m_TileExtBarCside && msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "TILECAL_EXT_C_SIDE is absent!" << endreq;

          } // end of setting flags for non-zero mask
        } // end of decoding detector mask
      }
    }
    firsteventinrun=false;
  } // done processing for first event in the run

  // do the real job
  HLT::ErrorCode status = makeMissingET(tes_in);

  if(status != HLT::OK){
    delete m_met; 
    m_met = 0;
    msg() << MSG::ERROR << "EFMissingET::makeOutputTE returned Failure" << endreq;
    return HLT::ERROR;
  }

  // create output trigger element
  status = makeOutputTE(tes_in, type_out);

  if (doTiming() && m_doTimers && m_totTime) {
    m_totTime->stop();
  }

  // stop monitoring
  afterExecMonitors().ignore();

  return HLT::OK;

}
///////////////////////////////////////////
//HLT::ErrorCode EFMissingET::getROI()    {
//
//  // Some debug output:
//  if(msgLvl() <= MSG::DEBUG){
//    msg() << MSG::DEBUG
//      << "m_inputTE->label(): "
//        //Migration << m_inputTE->label()
//        << endreq;
//  }
//
//  return HLT::OK;
//}

//////////////////////////////////////////////////////////////////////
HLT::ErrorCode EFMissingET::makeMissingET(std::vector<std::vector<HLT::TriggerElement*> >& tes_in)
{

  if(m_doTopoClusters == false && m_doJets == false && m_doJets == false) 
     n_sizePers = 25;
   else if(m_doJets == true)
     n_sizePers = 2;   
   else if(m_doPUC == true)
     n_sizePers = 3;
   else  
     n_sizePers = 9;    
     
  
  // Setup xAOD EDM
  m_met = new xAOD::TrigMissingET(); m_met->makePrivateStore();
  
  std::vector <std::string> vs_aux;
  for(int i = 0; i < n_sizePers; i++)
     vs_aux.push_back("");
     
  m_met->defineComponents(vs_aux); 
  
  if(msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG  << " Created pers. object of size " << n_sizePers << endreq;

   // fetch topo. clusters for later use
   if (m_doTopoClusters && tes_in.size() > 0) { // safe-guard 
      for (HLT::TEVec::const_iterator it = tes_in[0].begin(); it != tes_in[0].end(); ++it) {
         HLT::ErrorCode status = getFeature(  (*it) , m_caloCluster );
         
         if(status!=HLT::OK || !m_caloCluster) {      	 
            // Changed to prevent abortions of combined chains during cosmic data taking
            // This should not be in during collisions
            //msg() << MSG::ERROR <<"Failed to get ClusterContainer" << endreq; return HLT::NAV_ERROR; 
            msg() << MSG::ERROR <<"Failed to get ClusterContainer" << endreq; return HLT::OK; 
         } else {     
            if (msgLvl() <= MSG::DEBUG) {
               msg() << MSG::DEBUG << "size of cluster container " << m_caloCluster->size() << endreq;  	
               //for (xAOD::CaloClusterContainer::const_iterator it = m_caloCluster->begin(); it != m_caloCluster->end(); ++it )
               //   msg() << MSG::DEBUG << " Cluster E, eta, phi: " << (*it)->e()<<", "<< (*it)->eta()<<", "<< (*it)->phi() << endreq;   
            }     
         }
         
      } // end loop over topoclusters
   } // fetched all topo. clusters

   // fetch jets for later use
   if (m_doJets && tes_in.size() > 0) { // safe-guard 
      for (HLT::TEVec::const_iterator it = tes_in[0].begin(); it != tes_in[0].end(); ++it) {
         HLT::ErrorCode status = getFeature(  (*it) , m_jets );
         
         if(status!=HLT::OK || !m_jets) {      	  
            msg() << MSG::ERROR <<"Failed to get Jets" << endreq; return HLT::NAV_ERROR; 
         } else {     
            if (msgLvl() <= MSG::DEBUG) {
               msg() << MSG::DEBUG << "size of jet container " << m_jets->size() << endreq;  	
               for (xAOD::JetContainer::const_iterator it = m_jets->begin(); it != m_jets->end(); ++it )
                  msg() << MSG::DEBUG << " Jet E, eta, phi: " << (*it)->e()<<", "<< (*it)->eta()<<", "<< (*it)->phi() << endreq;   
            }     
         }
         
      } // end loop over topoclusters
   } // fetched all topo. clusters

    
  if(m_doTopoClusters && !m_caloCluster) {  // check if one should process topo. clusters and if pointer is present  
     msg() << MSG::INFO << " Error: configured to run over topo. clusters but no TriggerElement was passed to the FEX -- check menu configuration!! " << endreq;
     return HLT::ERROR;    
  }

  if(m_doJets && !m_jets) {  // check if one should process jets and if pointer is present  
     msg() << MSG::INFO << " Error: configured to run over jets but no TriggerElement was passed to the FEX -- check menu configuration!! " << endreq;
     return HLT::ERROR;    
  }
      
  if (doTiming() && m_doTimers && m_algTime) {
    m_algTime->start();
  }

  /// clear transient-only helper object
  m_met_help->Reset();

  // // encode DetMask info in the status flag
  // int flag=m_met_help->GetStatus();

  // if (!m_LArEMbarrelAside)  flag |= m_maskEMB_A_Missing;
  // if (!m_LArEMbarrelCside)  flag |= m_maskEMB_C_Missing;
  // if (!m_LArEMendCapAside)  flag |= m_maskEME_A_Missing;
  // if (!m_LArEMendCapCside)  flag |= m_maskEME_C_Missing;
  // if (!m_LArHECendCapAside) flag |= m_maskHEC_A_Missing;
  // if (!m_LArHECendCapCside) flag |= m_maskHEC_C_Missing;
  // if (!m_LArFCalAside)      flag |= m_maskFCAL_A_Missing;    
  // if (!m_LArFCalCside)      flag |= m_maskFCAL_C_Missing;    
  // if (!m_TileBarrelAside)   flag |= m_maskTileB_A_Missing; 
  // if (!m_TileBarrelCside)   flag |= m_maskTileB_C_Missing; 
  // if (!m_TileExtBarAside)   flag |= m_maskTileE_A_Missing; 
  // if (!m_TileExtBarCside)   flag |= m_maskTileE_C_Missing;

  // m_met_help->SetStatus(flag);

  //unsigned short m_tmp_maskProcessing         = 0x0001; // bit  0
  //unsigned short m_tmp_maskProcessed          = 0x4000; // bit 14

  /// loop over tools 
  ToolHandleArray<EFMissingETBaseTool>::iterator it = m_tools.begin();
  for (; it < m_tools.end(); ++it) {
     
     if ( (*it)->execute(m_met, m_met_help, m_caloCluster,m_jets).isFailure() ) {
           msg() << MSG::ERROR << "EFMissingET AlgTool returned Failure" << endreq;
           return HLT::ERROR;
     }       
     
             	
    if (doTiming() && m_doTimers) {
      float timer[4][3]={{0,0,0},{0,0,0},{0,0,0},{0,0,0}};

      for (int iDet=0; iDet<4; ++iDet){ // loop on detector
        for (int iStep=0; iStep<3; ++iStep) { // loop on step
          if ( (*it)->getTime(iDet, iStep, &timer[iDet][iStep]).isFailure() ) {
            msg() << MSG::ERROR << "EFMissingET AlgTool timer returned Failure" << endreq;
            return HLT::ERROR;
          }
        } // loop on step
      } // loop on detector

      m_tool_time_00 += timer[0][0];
      m_tool_time_01 += timer[0][1];
      m_tool_time_02 += timer[0][2];
      m_tool_time_10 += timer[1][0];
      m_tool_time_11 += timer[1][1];
      m_tool_time_12 += timer[1][2];
      m_tool_time_20 += timer[2][0];
      m_tool_time_21 += timer[2][1];
      m_tool_time_22 += timer[2][2];
      m_tool_time_30 += timer[3][0];
      m_tool_time_31 += timer[3][1];
      m_tool_time_32 += timer[3][2];

      m_tool_time_EM +=   timer[0][0] + timer[0][1] + timer[0][2];
      m_tool_time_HEC +=  timer[1][0] + timer[1][1] + timer[1][2];
      m_tool_time_Tile += timer[2][0] + timer[2][1] + timer[2][2];
      m_tool_time_FCal += timer[3][0] + timer[3][1] + timer[3][2];
      m_tool_time_RegSel +=  timer[0][0] + timer[1][0] + timer[2][0] + timer[3][0];
      m_tool_time_LoadCol += timer[0][1] + timer[1][1] + timer[2][1] + timer[3][1];
      m_tool_time_Loop +=    timer[0][2] + timer[1][2] + timer[2][2] + timer[3][2];

    } // timers
  } // loop on tools

  //   m_tool_time_TOT=m_tool_time_RegSel+m_tool_time_LoadCol+m_tool_time_Loop;

  if (doTiming() && m_doTimers && m_algTime) {
    m_algTime->stop();
  }

  float ETx = m_met->ex();
  float ETy = m_met->ey();
  float MET = sqrt(ETx*ETx+ETy*ETy);
  float SET = m_met->sumEt();

  if(msgLvl() <= MSG::DEBUG){
    if (m_StoreGate) {
      const xAOD::EventInfo* pEvent(0);
      StatusCode sc = m_StoreGate->retrieve(pEvent);
      if ( sc.isFailure() ) {
        msg() << MSG::ERROR << "Cannot find xAOD::EventInfo object" << endreq;
      } else {
        m_current_run_id = pEvent->runNumber();
        m_current_lbk_id = pEvent->lumiBlock();
        m_current_evt_id = pEvent->eventNumber();
        m_current_bcg_id = pEvent->bcid();
        char buff[512];
        snprintf(buff,512,
             "REGTEST: Run number = %11u, luminosity block = %11u, event number = %11u, bunch crossing = %11u",
             m_current_run_id, m_current_lbk_id, m_current_evt_id, m_current_bcg_id);
        msg() << MSG::DEBUG << buff << endreq;
      }
    }
    msg() << MSG::DEBUG << "Transient object follows " << endreq;
    msg() << MSG::DEBUG << "REGTEST: (EF) Lvl1Id = " << config()->getLvl1Id() << endreq;
    msg() << MSG::DEBUG << *m_met_help << endreq; // transient object
  }

  // monitored values (in GeV or rad)
  m_phi = atan2f(ETy, ETx);
  m_mex_lin = ETx * 1e-3;
  m_mey_lin = ETy * 1e-3;
  m_mez_lin = m_met->ez() * 1e-3;
  m_met_lin = MET * 1e-3;
  m_me_lin  = sqrt(m_mex_lin*m_mex_lin+m_mey_lin*m_mey_lin+m_mez_lin*m_mez_lin);
  m_set_lin = SET * 1e-3;  // NB: might be negative
  m_se_lin  = m_met->sumE() * 1e-3;  // NB: might be negative

  float epsilon = 1e-6;  // 1 keV
  if (m_set_lin > epsilon) m_xs = m_met_lin / sqrt(m_set_lin); 
  
  // never compare a float with zero:
  epsilon = 1e-6;  // 1 keV
  if (m_me_lin>epsilon)  m_me_log  = log10(fabsf(m_me_lin)); // underflow otherwise

  epsilon=1.189; // GeV (see TrigEFMissingETMonitoring.py)
  if (fabsf(m_mex_lin)>epsilon)
    m_mex_log = copysign(log10f(fabsf(m_mex_lin)), m_mex_lin);
  else
    m_mex_log = 0;
  if (fabsf(m_mey_lin)>epsilon)
    m_mey_log = copysign(log10f(fabsf(m_mey_lin)), m_mey_lin);
  else
    m_mey_log = 0;
  if (fabsf(m_mez_lin)>epsilon)
    m_mez_log = copysign(log10f(fabsf(m_mez_lin)), m_mez_lin);
  else
    m_mez_log = 0;
  if (fabsf(m_set_lin)>epsilon)
    m_set_log = copysign(log10f(fabsf(m_set_lin)), m_set_lin);
  else
    m_set_log = 0;
  if (fabsf(m_se_lin)>epsilon)
    m_se_log  = copysign(log10f(fabsf(m_se_lin)), m_se_lin);
  else
    m_se_log = 0;
  if (fabsf(m_met_lin)>epsilon) // positive definite
    m_met_log = log10f(fabsf(m_met_lin));
  else
    m_met_log = -9e9; // underflow

  // monitoring: status flag
  for (int i=0; i<32; ++i) {
    unsigned mask = (1<<i);
    if (m_met->flag() & mask)
      m_status_flag[i] = 1;
    else
      m_status_flag[i] = 0;
  }

  for (unsigned char i=0; i<m_met_help->GetElements(); ++i) {
    TrigEFMissingEtComponent* metComp=m_met_help->GetComponent(i);
    float ex = metComp->m_ex * 1e-3; // GeV
    float ey = metComp->m_ey * 1e-3; // GeV
    float ez = metComp->m_ez * 1e-3; // GeV
    float et = sqrt(ex*ex+ey*ey);
    float sumEt = metComp->m_sumEt * 1e-3; // GeV
    float sumE  = metComp->m_sumE * 1e-3;  // GeV

    if (fabsf(ex)>epsilon)
      m_comp_ex_log[i] = copysign(log10f(fabsf(ex)), ex);
    else
      m_comp_ex_log[i] = 0;
    if (fabsf(ey)>epsilon)
      m_comp_ey_log[i] = copysign(log10f(fabsf(ey)), ey);
    else
      m_comp_ey_log[i] = 0;
    if (fabsf(ez)>epsilon)
      m_comp_ez_log[i] = copysign(log10f(fabsf(ez)), ez);
    else
      m_comp_ez_log[i] = 0;
    m_comp_et_lin[i] = et;
    if (et>1e-6) // i.e. et > 0
      m_comp_et_log[i] = log10f(et);
    else
      m_comp_et_log[i] = -9e9; // underflow
    m_comp_sum_et_lin[i] = sumEt;
    if (fabsf(sumEt)>epsilon)
      m_comp_sum_et_log[i] = copysign(log10f(fabsf(sumEt)), sumEt);
    else
      m_comp_sum_et_log[i] = 0;
    if (fabsf(sumE)>epsilon)
      m_comp_sum_e_log[i] = copysign(log10f(fabsf(sumE)), sumE);
    else
      m_comp_sum_e_log[i] = 0;
  }

  return HLT::OK;
}


//////////////////////////////////////////////////////////
HLT::ErrorCode EFMissingET::makeOutputTE(std::vector<std::vector<HLT::TriggerElement*> >& tes_in,
    unsigned int type_out) {


  // Only count MET as an input TE (for seeding relation of navigation structure)
  HLT::TEVec allTEs;
  if ( (tes_in.size()>0) && (tes_in[0].size()>0) ) allTEs.push_back( tes_in[0][0] );

  // create output TE:
  // Create an output TE seeded by the inputs
  HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(allTEs, type_out);
  HLT::ErrorCode status = attachFeature(outputTE, m_met, m_metOutputKey);

  if (status != HLT::OK){
    if(msgLvl() <= MSG::DEBUG){
      msg() << MSG::ERROR
        << "Write into outputTE failed"
        << endreq;
    }
    return status;
  }

  /*

  // Create a new RoiDescriptor with updated eta and phi.
  // Note that the steering will propagate l1Id and roiId automatically
  // so no need to set them.
  TrigRoiDescriptor* newRoiDescriptor=new TrigRoiDescriptor(m_roiDescriptor->l1Id(),
  m_roiDescriptor->roiId(),
  m_roiDescriptor->eta0(),
  m_roiDescriptor->phi0());

  if(msgLvl() <= MSG::DEBUG) {
  msg() << MSG::DEBUG  << "Recorded an RoiDescriptor " <<
  " phi " <<  newRoiDescriptor->phi0() <<
  " eta " << newRoiDescriptor->eta0() << endreq;
  }

  status = attachFeature(outputTE, newRoiDescriptor,"TrigEFMissingET");

*/

  // Some debug output:
  if(msgLvl() <= MSG::DEBUG){
    msg() << MSG::DEBUG
      << "We assume success, set TE with label "
      //Migration << outputTE->label()
      << " active to signal positive result."
      << endreq;
  }

  // CACHING
  // if we got here, everything was okay. so, we cache the feature for further execution of this instance in e.g. other MET Sequences:
  m_useCachedResult = true;
  m_cachedTE = outputTE;

  return HLT::OK;
}

