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
  declareProperty("doTracks", m_doTracks = false, "run with or without tracks");
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
  unsigned char N=42; // number of components
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

  m_StoreGate=nullptr;
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

  m_caloCluster=nullptr;
  m_jets=nullptr;
  m_tracks=nullptr;
  m_vertices=nullptr;

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

  m_met=nullptr;
  m_firsteventinrun=false;
  m_n_sizePers=0;

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
      ATH_MSG_WARNING( "not able to initialize timer!" );
    }
  }

  ATH_MSG_DEBUG( "Initializing EFMissingET" );

  if(m_tools.retrieve().isFailure() )
    ATH_MSG_ERROR( "Failed retrieve tools " << m_tools );
  else
    ATH_MSG_DEBUG( "Successfully retrieve tools " << m_tools );
  

  int Nc = m_met_help->GetElements();
  int Nf = m_flags.size();
  if(Nf>0){
    ATH_MSG_INFO( "ComponentFlags has size " << Nf );
    if (Nf!=Nc) {
      ATH_MSG_ERROR( "ComponentFlags has size different from the number " << Nc << " of components.  Aborting" );
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
    ATH_MSG_INFO( "ComponentCalib0 has size " << N0 );
    if (N0!=Nc) {
      ATH_MSG_ERROR( "ComponentCalib0 has size different from the number " << Nc << " of components.  Aborting" );
      return HLT::ERROR;
    } else {
      for (unsigned char i=0; i<N0; ++i) {
        (m_met_help->GetComponent(i))->m_calib0 = m_calib0[i];
      }
    }
  }

  int N1 = m_calib1.size();
  if (N1>0) {
    ATH_MSG_INFO( "ComponentCalib1 has size " << N1 );
    if (N1!=Nc) {
      ATH_MSG_ERROR( "ComponentCalib1 has size different from the number " << Nc << " of components.  Aborting" );
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
    m_met_help = nullptr;
  }
}

//////////////////////////////////////////////////////////

HLT::ErrorCode EFMissingET::hltBeginRun() {

  ATH_MSG_DEBUG( " EFMissingET::hltBeginRun() called " );

  // access StoreGate
  m_StoreGate = store();
  if (m_StoreGate==0) {
    ATH_MSG_ERROR( "Can not access StoreGate" );
    return HLT::SG_ERROR;
  }

  // Check if configuration is sane
  bool foundTopo = false; bool foundFEB = false; bool foundCell = false; bool foundJets = false;

  ATH_MSG_DEBUG( " loop through helper tools start " );

  for (auto& tool : m_tools) {

    ATH_MSG_DEBUG( tool->name() );

    if(tool->getFexType() == FexType::TOPO) foundTopo = true;
    if(tool->getFexType() == FexType::CELL) foundCell = true;
    if(tool->getFexType() == FexType::FEB)  foundFEB = true;
    if(tool->getFexType() == FexType::JET)  foundJets = true;

  }

  if(foundTopo && (foundFEB || foundCell)) {
    ATH_MSG_ERROR( "found topo. clusters AND Cells OR FEB - not a valid configuration .. aborting " );
  	return HLT::ERROR;
  }

  if(m_doTopoClusters && !foundTopo) {
    ATH_MSG_ERROR( "found topo. clusters config but no ClusterTool .. aborting " );
  	return HLT::ERROR;
  }

  if(m_doJets && !foundJets) {
    ATH_MSG_ERROR( "found jet config but no JetTool .. aborting " );
    return HLT::ERROR;
  }


  // --

  m_firsteventinrun = true;

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
    ATH_MSG_DEBUG( "Executing this EFMissingET " << name() << " in cached mode" );

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

  m_jets = nullptr;
  m_caloCluster = nullptr;
  m_tracks=nullptr;
  m_vertices=nullptr;

  ATH_MSG_DEBUG( "Executing EFMissingET::hltExecute()" );

  if(m_firsteventinrun) {
    ATH_MSG_DEBUG( "REGTEST: First event in run" );
    if (m_StoreGate) {
      const xAOD::EventInfo* pEvent(0);
      StatusCode sc = m_StoreGate->retrieve(pEvent);
      if ( sc.isFailure() ) {
        ATH_MSG_ERROR( "Cannot find xAOD::EventInfo object" );
      } else {
        m_current_run_id = pEvent->runNumber();
        m_current_lbk_id = pEvent->lumiBlock();
        m_current_evt_id = pEvent->eventNumber();
        m_current_bcg_id = pEvent->bcid();
        char buff[512];
        snprintf(buff,512,
             "REGTEST: Run number = %11u, luminosity block = %11u, event number = %11u, bunch crossing = %11u",
             m_current_run_id, m_current_lbk_id, m_current_evt_id, m_current_bcg_id);
        ATH_MSG_DEBUG( buff );

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
          if(msgLvl(MSG::DEBUG) ){
            char buff[512];
            snprintf(buff,512,"REGTEST: DetMask_1 = 0x%08lu",mask64);
            ATH_MSG_DEBUG( buff );
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

            if (msgLvl(MSG::WARNING) ) {
              if(!m_LArEMbarrelAside)  ATH_MSG_WARNING( "LAR_EM_BARREL_A_SIDE is absent!" );
              if(!m_LArEMbarrelCside)  ATH_MSG_WARNING( "LAR_EM_BARREL_C_SIDE is absent!" );
              if(!m_LArEMendCapAside)  ATH_MSG_WARNING( "LAR_EM_ENDCAP_A_SIDE is absent!" );
              if(!m_LArEMendCapCside)  ATH_MSG_WARNING( "LAR_EM_ENDCAP_C_SIDE is absent!" );
              if(!m_LArHECendCapAside) ATH_MSG_WARNING( "LAR_HAD_ENDCAP_A_SIDE is absent!" );
              if(!m_LArHECendCapCside) ATH_MSG_WARNING( "LAR_HAD_ENDCAP_C_SIDE is absent!" );
              if(!m_LArFCalAside)      ATH_MSG_WARNING( "LAR_FCAL_A_SIDE is absent!" );
              if(!m_LArFCalCside)      ATH_MSG_WARNING( "LAR_FCAL_C_SIDE is absent!" );
              if(!m_TileBarrelAside)   ATH_MSG_WARNING( "TILECAL_BARREL_A_SIDE is absent!" );
              if(!m_TileBarrelCside)   ATH_MSG_WARNING( "TILECAL_BARREL_C_SIDE is absent!" );
              if(!m_TileExtBarAside)   ATH_MSG_WARNING( "TILECAL_EXT_A_SIDE is absent!" );
              if(!m_TileExtBarCside)   ATH_MSG_WARNING( "TILECAL_EXT_C_SIDE is absent!" );
            }

          } // end of setting flags for non-zero mask
        } // end of decoding detector mask
      }
    }
    m_firsteventinrun=false;
  } // done processing for first event in the run

  // do the real job
  HLT::ErrorCode status = makeMissingET(tes_in);

  if(status != HLT::OK){
    delete m_met;
    m_met = nullptr;
    ATH_MSG_ERROR( "EFMissingET::makeOutputTE returned Failure" );
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
//        << endmsg;
//  }
//
//  return HLT::OK;
//}

//////////////////////////////////////////////////////////////////////
HLT::ErrorCode EFMissingET::makeMissingET(std::vector<std::vector<HLT::TriggerElement*> >& tes_in)
{

  if(m_doTopoClusters == false && m_doJets == false && m_doJets == false)
     m_n_sizePers = 25;
   else if(m_doJets == true)
     m_n_sizePers = 6;
   else if(m_doPUC == true)
     m_n_sizePers = 3;
   else
     m_n_sizePers = 9;


  // Setup xAOD EDM
  m_met = new xAOD::TrigMissingET(); m_met->makePrivateStore();

  std::vector <std::string> vs_aux;
  for(int i = 0; i < m_n_sizePers; i++)
     vs_aux.push_back("");

  m_met->defineComponents(vs_aux);

  ATH_MSG_DEBUG (" Created pers. object of size " << m_n_sizePers);

   // fetch topo. clusters for later use
   if (m_doTopoClusters && tes_in.size() > 0) { // safe-guard
      for (const auto& te_in : tes_in.at(0) ) {
         HLT::ErrorCode status = getFeature(  te_in , m_caloCluster );

         if(status!=HLT::OK || !m_caloCluster) {
            // Changed to prevent abortions of combined chains during cosmic data taking
            // This should not be in during collisions
            //ATH_MSG_ERROR( "Failed to get ClusterContainer" ); return HLT::NAV_ERROR;
            ATH_MSG_ERROR( "Failed to get ClusterContainer" ); return HLT::OK;
         } else {
           ATH_MSG_DEBUG( "size of cluster container " << m_caloCluster->size() );
         }

      } // end loop over topoclusters
   } // fetched all topo. clusters

   // fetch jets for later use
   if (m_doJets && tes_in.size() > 0) { // safe-guard
      for (const auto& te_in : tes_in.at(0) ) {
         HLT::ErrorCode status = getFeature(  te_in , m_jets );

         if(status!=HLT::OK || !m_jets) {
            ATH_MSG_ERROR( "Failed to get Jets" ); return HLT::NAV_ERROR;
         } else {
            if (msgLvl(MSG::DEBUG) ) {
               ATH_MSG_DEBUG( "size of jet container " << m_jets->size() );
               for (const auto& ijet : *m_jets) 
                  ATH_MSG_DEBUG( " Jet E, eta, phi: " << ijet->e()<<", "<< ijet->eta()<<", "<< ijet->phi() );
            }
         }

      } // end loop over topoclusters
   } // fetched all topo. clusters


   //fetch tracks for later use
   if (m_doJets && m_doTracks && tes_in.size() > 0) { // safe-guard
      for (const auto& te_in : tes_in.at(1) ) {
         HLT::ErrorCode status = getFeature(  te_in , m_tracks );

         if(status!=HLT::OK || !m_tracks) {
            ATH_MSG_ERROR( "Failed to get tracks" ); return HLT::NAV_ERROR;
         } else {
            if (msgLvl(MSG::DEBUG) ) {
               ATH_MSG_DEBUG( "size of track container " << m_tracks->size() );
               for (const auto& itrack : *m_tracks) 
                 ATH_MSG_DEBUG( " Track pt, eta, phi, vertex, z0, vz: " << itrack->pt()<<", "<< itrack->eta()<<", "<< itrack->phi() << ", "
                                << itrack->vertex() << ", " <<  fabs(itrack->z0()) << ", " << itrack->vz() );
            }
         }

      } // end loop over topoclusters
   } // fetched all topo. clusters



   //fetch vertex for later use
   if (m_doJets  && m_doTracks && tes_in.size() > 0) { // safe-guard
      for (const auto& te_in : tes_in.at(1) ) {
         HLT::ErrorCode status = getFeature(  te_in , m_vertices );

         if(status!=HLT::OK || !m_vertices) {
            ATH_MSG_ERROR( "Failed to get vertices" ); return HLT::NAV_ERROR;
         } else {
            if (msgLvl(MSG::DEBUG) ) {
               ATH_MSG_DEBUG( "size of vertex container " << m_vertices->size() );
               for (const xAOD::Vertex* ivtx : *m_vertices) 
                 ATH_MSG_DEBUG( " Vertex x, y, z, ntracks: " << ivtx->x()<<", "<< ivtx->y()<<", "<< ivtx->z() << ", "
                                << ivtx->nTrackParticles() );
            }
         }

      } // end loop over topoclusters
   } // fetched all topo. clusters



  if(m_doTopoClusters && !m_caloCluster) {  // check if one should process topo. clusters and if pointer is present
     ATH_MSG_INFO( " Error: configured to run over topo. clusters but no TriggerElement was passed to the FEX -- check menu configuration!! " );
     return HLT::ERROR;
  }

  if(m_doJets && !m_jets) {  // check if one should process jets and if pointer is present
     ATH_MSG_INFO( " Error: configured to run over jets but no TriggerElement was passed to the FEX -- check menu configuration!! " );
     return HLT::ERROR;
  }

  if(m_doTracks && (!m_tracks || !m_vertices)) {  // check if one should process jets and if pointer is present
     ATH_MSG_INFO( " Error: configured to run over tracks and jets but no TriggerElement was passed to the FEX -- check menu configuration!! " );
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
  for (auto& tool : m_tools) {

     if ( tool->execute(m_met, m_met_help, m_caloCluster,m_jets, m_tracks, m_vertices).isFailure() ) {
           ATH_MSG_ERROR( "EFMissingET AlgTool returned Failure" );
           return HLT::ERROR;
     }


    if (doTiming() && m_doTimers) {
      float timer[4][3]={{0,0,0},{0,0,0},{0,0,0},{0,0,0}};

      for (int iDet=0; iDet<4; ++iDet){ // loop on detector
        for (int iStep=0; iStep<3; ++iStep) { // loop on step
          if ( tool->getTime(iDet, iStep, &timer[iDet][iStep]).isFailure() ) {
            ATH_MSG_ERROR( "EFMissingET AlgTool timer returned Failure" );
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

  if(msgLvl(MSG::DEBUG) ) {
    if (m_StoreGate) {
      const xAOD::EventInfo* pEvent(0);
      StatusCode sc = m_StoreGate->retrieve(pEvent);
      if ( sc.isFailure() ) {
        ATH_MSG_ERROR( "Cannot find xAOD::EventInfo object" );
      } else {
        m_current_run_id = pEvent->runNumber();
        m_current_lbk_id = pEvent->lumiBlock();
        m_current_evt_id = pEvent->eventNumber();
        m_current_bcg_id = pEvent->bcid();
        char buff[512];
        snprintf(buff,512,
             "REGTEST: Run number = %11u, luminosity block = %11u, event number = %11u, bunch crossing = %11u",
             m_current_run_id, m_current_lbk_id, m_current_evt_id, m_current_bcg_id);
        ATH_MSG_DEBUG( buff );
      }
    }
    ATH_MSG_DEBUG( "Transient object follows " );
    ATH_MSG_DEBUG( "REGTEST: (EF) Lvl1Id = " << config()->getLvl1Id() );
    ATH_MSG_DEBUG( *m_met_help ); // transient object
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
    ATH_MSG_ERROR( "Write into outputTE failed" );
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
  " eta " << newRoiDescriptor->eta0() << endmsg;
  }

  status = attachFeature(outputTE, newRoiDescriptor,"TrigEFMissingET");

*/

  // Some debug output:
  ATH_MSG_DEBUG( "We assume success, set TE with label active to signal positive result." );

  // CACHING
  // if we got here, everything was okay. so, we cache the feature for further execution of this instance in e.g. other MET Sequences:
  m_useCachedResult = true;
  m_cachedTE = outputTE;

  return HLT::OK;
}

