/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     T2MissingET.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigL2MissingET
//
// AUTHOR:   Till Eifert, Diego Casadei
// CREATED:  May 9, 2007
//
// Description: Level2 MissingET AllTEAlgo (FEX) algorithm. Uses LVL1 MET
//
// ********************************************************************

#include "T2MissingET.h"
#include "TrigT1Interfaces/RecEnergyRoI.h"
#include "TrigSteeringEvent/Enums.h"
#include "TrigMissingEtEvent/TrigMissingET.h"
#include "TrigNavigation/Navigation.h"
#include "CxxUtils/sincos.h"
#include "CxxUtils/sincosf.h"

#include "xAODEventInfo/EventInfo.h"
#include "eformat/DetectorMask.h"
#include "eformat/SourceIdentifier.h"

#include <cmath>
#include <cstdio>

#define NCOM 1

using namespace PESA;

T2MissingET::T2MissingET(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::AllTEAlgo(name, pSvcLocator),
    m_useCachedResult(false),
    m_met_feature(0),
    m_cachedTE(0)
{

  m_badRegions.resize(4);
  m_badRegions[0].clear();
  m_badRegions[1].clear();
  m_badRegions[2].clear();
  m_badRegions[3].clear();

  declareProperty("METLabel", m_featureLabel = "T2MissingET", "label for the MET feature in the HLT Navigation");
  declareProperty("BadRegionsPhiMin", m_badRegions[0], "list of inf(bad phi interval)");
  declareProperty("BadRegionsPhiMax", m_badRegions[1], "list of sup(bad phi interval)");
  declareProperty("BadRegionsEtaMin", m_badRegions[2], "list of inf(bad eta interval)");
  declareProperty("BadRegionsEtaMax", m_badRegions[3], "list of sup(bad eta interval)");
  declareProperty("DecodeDetMask", m_decodeDetMask = false, "switch on/off DetMask decoding");
  declareProperty("GlobMaxMEtSumEtRatio", m_GlobMaxMEtSumEtRatio = 0.9, "max reasonable |MET/SumET|");

  declareMonitoredVariable("L2_METx",     m_lvl2_mex);
  declareMonitoredVariable("L2_METy",     m_lvl2_mey);
  declareMonitoredVariable("L2_MET",      m_lvl2_met);
  declareMonitoredVariable("L2_SumEt",    m_lvl2_set);
  declareMonitoredVariable("L2_MET_phi",  m_lvl2_phi);
  declareMonitoredVariable("L1_METx",     m_lvl1_mex);
  declareMonitoredVariable("L1_METy",     m_lvl1_mey);
  declareMonitoredVariable("L1_MET",      m_lvl1_met);
  declareMonitoredVariable("L1_SumEt",    m_lvl1_set);
  declareMonitoredVariable("L1_MET_phi",  m_lvl1_phi);

  declareMonitoredVariable("L2_MEx_log",      m_lvl2_mex_log);
  declareMonitoredVariable("L2_MEy_log",      m_lvl2_mey_log);
  declareMonitoredVariable("L2_MET_log",      m_lvl2_met_log);
  declareMonitoredVariable("L2_SumEt_log",    m_lvl2_set_log);
  declareMonitoredVariable("L1_MEx_log",      m_lvl1_mex_log);
  declareMonitoredVariable("L1_MEy_log",      m_lvl1_mey_log);
  declareMonitoredVariable("L1_MET_log",      m_lvl1_met_log);
  declareMonitoredVariable("L1_SumEt_log",    m_lvl1_set_log);
  
  declareMonitoredVariable("L1_XS",           m_lvl1_xs);
  declareMonitoredVariable("L2_XS",           m_lvl2_xs);

  m_status_flag.reserve(32); // to map a 32-bit word
  m_status_flag.assign(32, 0);
  declareMonitoredStdContainer("L2_MET_status", m_status_flag);

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
  m_L1Calo=true;



  /** definition of the meaning for the component flag bits **/
  m_maskErrParityL1        = 0x0001; // bit  0
  m_maskErrL1mult          = 0x0002; // bit  1
  m_maskErrMuon            = 0x0004; // bit  2
                // spare   = 0x0008; // bit  3
  m_maskL1OverflowExEy     = 0x0010; // bit  4
  m_maskL1OverflowSumEt    = 0x0020; // bit  5
                // spare   = 0x0040; // bit  6
  m_maskMETinBadPhiRegion  = 0x0080; // bit  7
  m_maskMETinBadRegion     = 0x0100; // bit  8
  m_maskObjInPhiRegion     = 0x0200; // bit  9
  m_maskObjInRegion        = 0x0400; // bit 10
  m_maskObjInCrack         = 0x0800; // bit 11
  m_maskPhiCorrJet1        = 0x1000; // bit 12
  m_maskPhiCorrJet2        = 0x2000; // bit 13
  m_maskPhiCorrJet3        = 0x4000; // bit 14
  m_maskCompErrors         = 0x8000; // bit 15

  /** definition of the meaning for the global flag bits (highest ones only) **/
  m_maskEMB_A_Missing        = 0x00010000; // bit 16
  m_maskEMB_C_Missing        = 0x00020000; // bit 17
  m_maskEME_A_Missing        = 0x00040000; // bit 18
  m_maskEME_C_Missing        = 0x00080000; // bit 19
  m_maskHEC_A_Missing        = 0x00100000; // bit 20
  m_maskHEC_C_Missing        = 0x00200000; // bit 21
  m_maskFCAL_A_Missing       = 0x00400000; // bit 22
  m_maskFCAL_C_Missing       = 0x00800000; // bit 23
  m_maskTileB_A_Missing      = 0x01000000; // bit 24
  m_maskTileB_C_Missing      = 0x02000000; // bit 25
  m_maskTileE_A_Missing      = 0x04000000; // bit 26
  m_maskTileE_C_Missing      = 0x08000000; // bit 27
  m_maskL1Calo_Missing       = 0x10000000; // bit 28
  m_maskGlobBigMEtSEtRatio   = 0x20000000; // bit 29
                    // spare = 0x40000000; // bit 30
  m_maskGlobErrors           = 0x80000000; // bit 31

  //initialization to prevent coverity error:
  m_firsteventinrun=false;

}


//////////////////////////////////////////////////////////

HLT::ErrorCode T2MissingET::hltStart() {

  // access StoreGate
  m_StoreGate = store();
  if (m_StoreGate==0) {
    ATH_MSG_ERROR( "Can not access StoreGate"  );
    return HLT::SG_ERROR;
  }

  m_firsteventinrun = true;

  return HLT::OK; 
}


//////////////////////////////////////////////////////////




HLT::TriggerElement* T2MissingET::makeSeeding(std::vector<std::vector<HLT::TriggerElement*> >& tes_in, unsigned int type_out ) {

  HLT::TEVec allTEs;
  for ( unsigned type = 0; type < tes_in.size(); ++ type ) {
    std::vector<HLT::TriggerElement*>& tes = tes_in.at(type);

    for ( unsigned teIdx = 0; teIdx < tes.size(); ++teIdx ) {

      const LVL1::RecEnergyRoI* lvl1_energyRoi(0);
      if ( getFeature(tes.at(teIdx), lvl1_energyRoi) == HLT::OK) { // for the seeding use onlt XE/TE/XS trigger elements (i.e. those having the RecEnergy obj)
	if ( lvl1_energyRoi ) {
	  allTEs.push_back(tes.at(teIdx)); 
	}
      }	     
    }
  } 
  return  config()->getNavigation()->addNode(allTEs, type_out);
}

HLT::ErrorCode T2MissingET::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in, unsigned int type_out)
{

  // CACHING
  // first check whether we executed this instance before:
  if (m_useCachedResult) {
    ATH_MSG_DEBUG( "Executing this T2MissingET " << name() << " in cached mode"  );

    HLT::TriggerElement* outputTE = makeSeeding(tes_in, type_out);


    // save (cached) met feature to output TE:
    m_config->getNavigation()->copyAllFeatures( m_cachedTE, outputTE );

    return HLT::OK;
  }

  // event status flag
  int flag=0;

  if(m_firsteventinrun) {
    ATH_MSG_DEBUG( "REGTEST: First event in run"  );
    // get EventInfo
    const xAOD::EventInfo* pEvent(0);
    StatusCode sc = m_StoreGate->retrieve(pEvent);
    if ( sc.isFailure() ) {
      ATH_MSG_ERROR( "Cannot find xAODEventInfo object"  );
      return HLT::SG_ERROR;
    }
    m_current_run_id = pEvent->runNumber();
    m_current_lbk_id = pEvent->lumiBlock();
    m_current_evt_id = pEvent->eventNumber();
    m_current_bcg_id = pEvent->bcid();

    if(msgLvl() <= MSG::DEBUG){
      char buff[512];
      snprintf(buff,512,
            "REGTEST: Run number = %11u, luminosity block = %11u, event number = %11u, bunch crossing = %11u",
            m_current_run_id, m_current_lbk_id, m_current_evt_id, m_current_bcg_id);
      ATH_MSG_DEBUG( buff  );
    }

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
    m_L1Calo=true;


    if(m_decodeDetMask) { 
      uint64_t mask64 = pEvent->detectorMask();
      if(msgLvl() <= MSG::DEBUG){
        char buff[512];
        snprintf(buff,512,"REGTEST: DetMask = 0x%08lu",mask64);
        msg() << MSG::DEBUG << buff << endmsg;
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

        if(!m_LArEMbarrelAside) ATH_MSG_WARNING( "LAR_EM_BARREL_A_SIDE is absent!"  );
        if(!m_LArEMbarrelCside) ATH_MSG_WARNING( "LAR_EM_BARREL_C_SIDE is absent!"  );
        if(!m_LArEMendCapAside) ATH_MSG_WARNING( "LAR_EM_ENDCAP_A_SIDE is absent!"  );
        if(!m_LArEMendCapCside) ATH_MSG_WARNING( "LAR_EM_ENDCAP_C_SIDE is absent!"  );
        if(!m_LArHECendCapAside) ATH_MSG_WARNING( "LAR_HAD_ENDCAP_A_SIDE is absent!"  );
        if(!m_LArHECendCapCside) ATH_MSG_WARNING( "LAR_HAD_ENDCAP_C_SIDE is absent!"  );
        if(!m_LArFCalAside) ATH_MSG_WARNING( "LAR_FCAL_A_SIDE is absent!"  );
        if(!m_LArFCalCside) ATH_MSG_WARNING( "LAR_FCAL_C_SIDE is absent!"  );
        if(!m_TileBarrelAside) ATH_MSG_WARNING( "TILECAL_BARREL_A_SIDE is absent!"  );
        if(!m_TileBarrelCside) ATH_MSG_WARNING( "TILECAL_BARREL_C_SIDE is absent!"  );
        if(!m_TileExtBarAside) ATH_MSG_WARNING( "TILECAL_EXT_A_SIDE is absent!"  );
        if(!m_TileExtBarCside) ATH_MSG_WARNING( "TILECAL_EXT_C_SIDE is absent!"  );

        m_L1Calo=true;
        if (!dm.is_set(eformat::TDAQ_CALO_PREPROC)) m_L1Calo=false;
        if (!dm.is_set(eformat::TDAQ_CALO_CLUSTER_PROC_DAQ)) m_L1Calo=false;
        if (!dm.is_set(eformat::TDAQ_CALO_CLUSTER_PROC_ROI)) m_L1Calo=false;
        if (!dm.is_set(eformat::TDAQ_CALO_JET_PROC_DAQ)) m_L1Calo=false;
        if (!dm.is_set(eformat::TDAQ_CALO_JET_PROC_ROI)) m_L1Calo=false;

        if(!m_L1Calo) ATH_MSG_WARNING( "L1Calo is absent!"  );

      } // Finshed dealing with non zero detector mask

    } // finished decoding detector mask
    m_firsteventinrun = false;
  } // end processing of first run in event 

  // if L1Calo is missing, produce an empty feature
  if(!m_L1Calo){
    m_met_feature = new xAOD::TrigMissingET(); m_met_feature->makePrivateStore();
 
    std::vector <std::string> vs_aux;
    for(int i = 0; i < NCOM; i++)
     vs_aux.push_back("");    
    m_met_feature->defineComponents(vs_aux); 
    
    if (m_met_feature==0) {
      if(msgLvl() <= MSG::WARNING)
	msg() << MSG::ERROR //WARNING
	      << "cannot create the TrigMissingET object!" << endmsg;
      return HLT::NO_HLT_RESULT;
    }

    flag |= m_maskL1Calo_Missing | m_maskGlobErrors;
    m_met_feature->setFlag(flag);

    HLT::TriggerElement* outputTE = makeSeeding(tes_in, type_out);

    HLT::ErrorCode hltStatus = attachFeature(outputTE, m_met_feature, m_featureLabel);

    if ( hltStatus != HLT::OK ) {
      ATH_MSG_WARNING( "Write of TrigMissingET feature into outputTE failed" );
      return hltStatus;
    }

    init(m_met_feature);

    m_useCachedResult = true;
    m_cachedTE = outputTE;

    return HLT::OK;
  } // end if L1Calo is absent

  // flag event accordingly to DetMask
  if (!m_LArEMbarrelAside)  flag |= m_maskEMB_A_Missing;
  if (!m_LArEMbarrelCside)  flag |= m_maskEMB_C_Missing;
  if (!m_LArEMendCapAside)  flag |= m_maskEME_A_Missing;
  if (!m_LArEMendCapCside)  flag |= m_maskEME_C_Missing;
  if (!m_LArHECendCapAside) flag |= m_maskHEC_A_Missing;
  if (!m_LArHECendCapCside) flag |= m_maskHEC_C_Missing;
  if (!m_LArFCalAside)      flag |= m_maskFCAL_A_Missing;    
  if (!m_LArFCalCside)      flag |= m_maskFCAL_C_Missing;    
  if (!m_TileBarrelAside)   flag |= m_maskTileB_A_Missing; 
  if (!m_TileBarrelCside)   flag |= m_maskTileB_C_Missing; 
  if (!m_TileExtBarAside)   flag |= m_maskTileE_A_Missing; 
  if (!m_TileExtBarCside)   flag |= m_maskTileE_C_Missing;

  // start monitoring
  beforeExecMonitors().ignore();

  m_lvl1_mex = -9e9;
  m_lvl1_mey = -9e9;
  m_lvl1_met = -9e9;
  m_lvl1_set = -9e9;
  m_lvl1_phi = -9e9;
  m_lvl2_mex = -9e9;
  m_lvl2_mey = -9e9;
  m_lvl2_met = -9e9;
  m_lvl2_set = -9e9;
  m_lvl2_phi = -9e9;

  m_lvl2_mex_log = -9e9;
  m_lvl2_mey_log = -9e9;
  m_lvl2_met_log = -9e9;
  m_lvl2_set_log = -9e9;
  m_lvl1_mex_log = -9e9;
  m_lvl1_mey_log = -9e9;
  m_lvl1_met_log = -9e9;
  m_lvl1_set_log = -9e9;
  
  m_lvl1_xs = -9e9;
  m_lvl2_xs = -9e9;

  bool seeded=true; // default mode: seeded by L1
  unsigned int tes_in_size=tes_in.size(); // = 1 (seeded) or 0 (unseeded)
  unsigned int tes_in0_size=0;  // size of L1 result (must be = 1)

  if (msgLvl() <= MSG::DEBUG) {
    ATH_MSG_DEBUG( "tes_in.size() = " << tes_in_size  );

    for (unsigned u=0; u<tes_in_size; ++u) {
      ATH_MSG_DEBUG( "tes_in[" << u << "].size() = " << tes_in[u].size()  );
    }
  }

  switch (tes_in_size) {
  case 0: // unseeded mode
    seeded=false;
    ATH_MSG_DEBUG( "Running in unseeded mode"  );
    break;

  case 1: // seeded mode
    seeded=true;
    ATH_MSG_DEBUG( "Running in seeded mode"  );
    tes_in0_size=tes_in[0].size();
    if (tes_in0_size != 1) {
      msg() << MSG::ERROR
	    << "Configuration error: expecting exactly 1 L1 result.  Aborting chain" << endmsg;
      return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::BAD_JOB_SETUP);
    }
    break;

  default:
    msg() << MSG::ERROR
	  << "Configuration error: tes_in.size() is " << tes_in_size
	  << " but can only be 1 or 0 in unseeded mode.  Aborting chain" << endmsg;
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::BAD_JOB_SETUP);
  }

  /// get LVL1 result ///
  const LVL1::RecEnergyRoI* lvl1_energyRoi = 0;

  if (seeded) { // seeded mode (default)
    std::vector<const LVL1::RecEnergyRoI*> vectorOfEnergyRoI;
    HLT::ErrorCode status = getFeatures(tes_in[0][0], vectorOfEnergyRoI);
    if(status == HLT::OK) {
      if ( vectorOfEnergyRoI.size() < 1 ) {
	msg() << MSG::ERROR << "Cannot find L1 result! Aborting" << endmsg;
        return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
      }
      else if ( vectorOfEnergyRoI.size() > 1 ) {
	ATH_MSG_WARNING( "found " << vectorOfEnergyRoI.size() 
                         << " RecEnergyRoI features but they should be exactly 1!  Taking first one with fingers crossed"  );
      }
      lvl1_energyRoi = vectorOfEnergyRoI.front();
    } else {
      msg() << MSG::ERROR
	    << "RecEnergyRoI feature not found.  Aborting" << endmsg;
      return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
    }
  } else { // unseeded mode: get all RoIs and look for the good one
    HLT::Navigation* nav = config()->getNavigation();
    HLT::TriggerElement* initial = nav->getInitialNode();
    const std::vector<HLT::TriggerElement*>& rois = nav->getDirectSuccessors(initial);
    for (unsigned u=0; u<rois.size(); ++u) {
      if ( nav->getFeature( rois[u], lvl1_energyRoi ) ) {
	if (lvl1_energyRoi) break;
      }
    }
    if(!lvl1_energyRoi) {
      msg() << MSG::ERROR
            << "No RecEnergyRoI object found!  Aborting" << endmsg;
      return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
    }
  }

  // this is in ~ GeV units
  // (scale factor will be contained in LVL1ConfigSvc at one point ...)
  m_lvl1_mex = - static_cast<float>(lvl1_energyRoi->energyX());
  m_lvl1_mey = - static_cast<float>(lvl1_energyRoi->energyY());
  m_lvl1_met = sqrt( m_lvl1_mex * m_lvl1_mex + m_lvl1_mey* m_lvl1_mey);
  m_lvl1_set = static_cast<float>(lvl1_energyRoi->energyT());
  m_lvl1_phi = atan2f(m_lvl1_mey, m_lvl1_mex);

  if(msgLvl() <= MSG::DEBUG){
    if (m_StoreGate) {
      const xAOD::EventInfo* pEvent(0);
      StatusCode sc = m_StoreGate->retrieve(pEvent);
      if ( sc.isFailure() ) {
        ATH_MSG_ERROR( "Cannot find xAOD::EventInfo object"  );
      } else {
        m_current_run_id = pEvent->runNumber();
        m_current_lbk_id = pEvent->lumiBlock();
        m_current_evt_id = pEvent->eventNumber();
        m_current_bcg_id = pEvent->bcid();
        char buff[512];
        snprintf(buff,512,
             "REGTEST: Run number = %11u, luminosity block = %11u, event number = %11u, bunch crossing = %11u",
             m_current_run_id, m_current_lbk_id, m_current_evt_id, m_current_bcg_id);
        ATH_MSG_DEBUG( buff  );
      }
    }
    ATH_MSG_DEBUG( "REGTEST: (LVL1) Lvl1Id = " << config()->getLvl1Id()  );
    ATH_MSG_DEBUG( "REGTEST: (LVL1) et = " <<  m_lvl1_met << " GeV"  );
    ATH_MSG_DEBUG( "REGTEST: (LVL1) ex = " <<  m_lvl1_mex << " GeV"  );
    ATH_MSG_DEBUG( "REGTEST: (LVL1) ey = " <<  m_lvl1_mey << " GeV"  );
    ATH_MSG_DEBUG( "REGTEST: (LVL1) SumEt = " <<  m_lvl1_set << " GeV"  );
  }

  // convert energy from GeV to MeV
  m_lvl2_mex = m_lvl1_mex * 1e3;
  m_lvl2_mey = m_lvl1_mey * 1e3;
  m_lvl2_met = m_lvl1_met * 1e3;
  m_lvl2_set = m_lvl1_set * 1e3;
  m_lvl2_phi = m_lvl1_phi;

  /// flagging the event based on L1 ///
  unsigned int bitParityError    =  0x8000000; // bit 27 - roiWord parity error

  // check for L1 overflows and parity errors
  if ( lvl1_energyRoi->roiWord0() & bitParityError ) flag |= m_maskErrParityL1 | m_maskGlobErrors;
  if ( lvl1_energyRoi->roiWord1() & bitParityError ) flag |= m_maskErrParityL1 | m_maskGlobErrors;
  if ( lvl1_energyRoi->roiWord2() & bitParityError ) flag |= m_maskErrParityL1 | m_maskGlobErrors;
  if ( lvl1_energyRoi->overflowX() ) flag |= m_maskL1OverflowExEy;
  if ( lvl1_energyRoi->overflowY() ) flag |= m_maskL1OverflowExEy;
  if ( lvl1_energyRoi->overflowT() ) flag |= m_maskL1OverflowSumEt;


  // flag events with strange MET/SumET ratio
  if (m_lvl1_set>0.1 && fabsf(m_lvl2_met/m_lvl2_set)>m_GlobMaxMEtSumEtRatio)
    flag |= m_maskGlobBigMEtSEtRatio;

  
  int Nphi = m_badRegions[0].size();
  if (Nphi>0 && Nphi==(int)m_badRegions[1].size()) { // check if phi belongs to a bad region
    for (int kk=0; kk<Nphi; ++kk) { // loop over bad regions
      float phiMin = (m_badRegions[0])[kk];
      float phiMax = (m_badRegions[1])[kk];
      if (msgLvl() <= MSG::INFO)
	msg() << MSG::INFO << "Bad region: phi between " 
	      << phiMin << " and " << phiMax ;
      // the following is needed to accept the case phiMin>phiMax
      // and angles defined both in [0, 2 PI] and [-PI, PI]
      float aa, bb, xx=m_lvl2_phi;
      if (phiMin<phiMax) {
	aa = fmodf(phiMin, 2*M_PI);
	bb = fmodf(phiMax, 2*M_PI);
      } else {
	aa = fmodf(phiMin, 2*M_PI) + 2*M_PI;
	bb = fmodf(phiMax, 2*M_PI) + 2*M_PI;
	xx += 2*M_PI;
      }
      if (xx>aa && xx<bb) {
	flag |= m_maskMETinBadPhiRegion; // flag event
        ATH_MSG_INFO( " -- measured phi = " << m_lvl2_phi
                      << " --> event flagged"  );

	/*
NO CHECK ON ETA CAN BE DONE ON THE RESULT BY L1Calo: keep this code for the future :-)
	// check also eta
	int Neta = m_badRegions[2].size();
	if (Neta>0 && Neta==Nphi && Neta==(int)m_badRegions[3].size()) {
	  float etaMin = (m_badRegions[2])[kk];
	  float etaMax = (m_badRegions[3])[kk];
	  msg() << MSG::INFO << "Bad region: eta between " 
		<< etaMin << " and " << etaMax ;
	  float MEeta=0; // no L1Calo eta is available!
	  if (MEeta>etaMin && MEeta<etaMax) {
	    flag |= m_maskMETinBadRegion; // flag event
	    ATH_MSG_INFO( " -- measured eta = " << MEeta
	 	  << " --> event flagged"  );
 	  }
	  else {
            ATH_MSG_INFO( " -- event does not point here"  );
	  }
	} // end eta check
	*/

      } else {
        ATH_MSG_INFO( " -- event does not point here"  );
      } // end of region check
    } // loop over bad regions
  } // end of phi check


  /// TO DO: USE TRACKS AND JETS                                       ///
  ///                                                                  ///
  /// 1. find TRT tracks                                               ///
  /// 2. check that they are not muons                                 ///
  /// 3. flag if they enter the phi region: m_maskObjInPhiRegion       ///
  /// 5. flag if they enter the (eta, phi) region: m_maskObjInRegion   ///
  ///                                                                  ///
  /// 6. find jets                                                     ///
  /// 7. flag if they enter the phi region: m_maskObjInPhiRegion       ///
  /// 8. flag if they enter the (eta, phi) region: m_maskObjInRegion   ///
  /// 9. flag if MET is correlated with jet Pt: m_maskPhiCorrJet1, ... ///

  /// create MET feature ///
  m_met_feature = new xAOD::TrigMissingET(); m_met_feature->makePrivateStore();
  std::vector <std::string> vs_aux;
  for(int i = 0; i < NCOM; i++)
   vs_aux.push_back("");    
  m_met_feature->defineComponents(vs_aux); 
    
  if (m_met_feature==0) {
    ATH_MSG_WARNING( "cannot create the TrigMissingET object!"  );
    return HLT::NO_HLT_RESULT;
  }
  init(m_met_feature);
  
  // basic info:
  m_met_feature->setEx(m_lvl2_mex);
  m_met_feature->setEy(m_lvl2_mey);
  m_met_feature->setSumEt(m_lvl2_set);

  // update event status flag
  m_met_feature->setFlag(flag); 

  // convert back to GeV for monitoring histograms
  m_lvl2_mex *= 1e-3;
  m_lvl2_mey *= 1e-3;
  m_lvl2_set *= 1e-3;
  m_lvl2_met = sqrt(m_lvl2_mex * m_lvl2_mex + m_lvl2_mey * m_lvl2_mey);

  if (msgLvl() <= MSG::DEBUG) {
    ATH_MSG_DEBUG( "REGTEST: (LVL2) et = " <<  m_lvl2_met << " GeV"  );
    ATH_MSG_DEBUG( "REGTEST: (LVL2) ex = " <<  m_lvl2_mex << " GeV"  );
    ATH_MSG_DEBUG( "REGTEST: (LVL2) ey = " <<  m_lvl2_mey << " GeV"  );
    ATH_MSG_DEBUG( "REGTEST: (LVL2) SumEt = " <<  m_lvl2_set << " GeV"  );
    char buff[128];
    std::snprintf(buff,128,"REGTEST: (LVL2) Event status = 0x%08x", (unsigned)flag);
    ATH_MSG_DEBUG( buff  );
  }

  // monitoring: status flag
  for (int i=0; i<32; ++i) {
    unsigned mask = (1<<i);
    if (flag & mask)
      m_status_flag[i] = 1;
    else
      m_status_flag[i] = 0;
  }

  // monitoring: log-scale quantities
  float epsilon = 1e-6;  // 1 keV

  if (fabsf(m_lvl2_mex)>epsilon)
    m_lvl2_mex_log = copysign(log10(fabsf(m_lvl2_mex)), m_lvl2_mex);
  else
    m_lvl2_mex_log = 0;

  if (fabsf(m_lvl2_mey)>epsilon)
    m_lvl2_mey_log = copysign(log10(fabsf(m_lvl2_mey)), m_lvl2_mey);
  else
    m_lvl2_mey_log = 0;

  if (fabsf(m_lvl2_met)>epsilon)
    m_lvl2_met_log = copysign(log10(fabsf(m_lvl2_met)), m_lvl2_met);
  else
    m_lvl2_met_log = 0;

  if (fabsf(m_lvl2_set)>epsilon)
    m_lvl2_set_log = copysign(log10(fabsf(m_lvl2_set)), m_lvl2_set);
  else
    m_lvl2_set_log = 0;

  if (fabsf(m_lvl1_mex)>epsilon)
    m_lvl1_mex_log = copysign(log10(fabsf(m_lvl1_mex)), m_lvl1_mex);
  else
    m_lvl1_mex_log = 0;

  if (fabsf(m_lvl1_mey)>epsilon)
    m_lvl1_mey_log = copysign(log10(fabsf(m_lvl1_mey)), m_lvl1_mey);
  else
    m_lvl1_mey_log = 0;

  if (fabsf(m_lvl1_met)>epsilon)
    m_lvl1_met_log = copysign(log10(fabsf(m_lvl1_met)), m_lvl1_met);
  else
    m_lvl1_met_log = 0;

  if (fabsf(m_lvl1_set)>epsilon)
    m_lvl1_set_log = copysign(log10(fabsf(m_lvl1_set)), m_lvl1_set);
  else
    m_lvl1_set_log = 0;

  if (m_lvl1_set>epsilon)
    m_lvl1_xs = m_lvl1_met / sqrt(m_lvl1_set);  
    
  if (m_lvl2_set>epsilon)
    m_lvl2_xs = m_lvl2_met / sqrt(m_lvl2_set);  

  // create output TE:
  // Create an output TE seeded by the inputs
  HLT::TriggerElement* outputTE = makeSeeding(tes_in, type_out);  

  // save feature to output TE:
  HLT::ErrorCode hltStatus = attachFeature(outputTE, m_met_feature, m_featureLabel);

  if ( hltStatus != HLT::OK ) {
    ATH_MSG_WARNING( "Write of TrigMissingET feature into outputTE failed" );
     return hltStatus;
  }

  // CACHING
  // if we got here, everything was okay. so, we cache the feature for further execution of this instance in e.g. other MET Sequences:
  m_useCachedResult = true;
  m_cachedTE = outputTE;

  // stop monitoring
  afterExecMonitors().ignore();

  return HLT::OK;
}

//////////////////////////////////////////////////////////
// Initializing the TrigMissingET object
// This should be done in the TrigMissingEtEvent definition
HLT::ErrorCode T2MissingET::init(xAOD::TrigMissingET *met){
  int ncom=met->getNumberOfComponents();
  if(ncom!=NCOM){
    ATH_MSG_ERROR( "Wrong number of TrigMissingET dimension."  );
    return HLT::NO_HLT_RESULT;
  } 
  met->setNameOfComponent(0,"Muons    ");
  for(int index=0;index<ncom;index++){
    met->setUsedChannelsComponent(index,1); // Suggestion by Diego
  } 
  return HLT::OK;
}
