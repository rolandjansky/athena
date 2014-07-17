/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     JEPSimBSMon.cxx
// PACKAGE:  TrigT1CaloMonitoring  
//
// AUTHOR:   Peter Faulkner
//           
//
// ********************************************************************

#include <utility>

#include "LWHists/LWHist.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH2I_LW.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "SGTools/StlVectorClids.h"

#include "AthenaMonitoring/AthenaMonManager.h"

#include "TrigT1CaloEvent/CMMJetHits.h"
#include "TrigT1CaloEvent/JEMHits.h"
#include "TrigT1CaloEvent/JetElement.h"
#include "TrigT1CaloEvent/JEMRoI.h"
#include "TrigT1CaloEvent/CMMRoI.h"
#include "TrigT1CaloEvent/JEMEtSums.h"
#include "TrigT1CaloEvent/CMMEtSums.h"
#include "TrigT1CaloEvent/RODHeader.h"
#include "TrigT1CaloEvent/TriggerTower.h"
#include "TrigT1CaloUtils/CoordToHardware.h"
#include "TrigT1CaloUtils/JetAlgorithm.h"
#include "TrigT1CaloToolInterfaces/IL1JEPHitsTools.h"
#include "TrigT1CaloToolInterfaces/IL1JetTools.h"
#include "TrigT1CaloToolInterfaces/IL1JetElementTools.h"
#include "TrigT1CaloToolInterfaces/IL1JEPEtSumsTools.h"
#include "TrigT1Interfaces/Coordinate.h"
#include "TrigT1Interfaces/JEPRoIDecoder.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

#include "TrigT1CaloMonitoring/JEPSimBSMon.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloMonErrorTool.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramTool.h"


/*---------------------------------------------------------*/
JEPSimBSMon::JEPSimBSMon(const std::string & type, 
			 const std::string & name,
			 const IInterface* parent)
  : ManagedMonitorToolBase(type, name, parent),
    m_jepHitsTool("LVL1::L1JEPHitsTools/L1JEPHitsTools"),
    m_jetTool("LVL1::L1JetTools/L1JetTools"),
    m_jetElementTool("LVL1::L1JetElementTools/L1JetElementTools"),
    m_etSumsTool("LVL1::L1JEPEtSumsTools/L1JEPEtSumsTools"),
    m_errorTool("TrigT1CaloMonErrorTool"),
    m_histTool("TrigT1CaloLWHistogramTool"),
    m_debug(false), m_rodTES(0), m_limitedRoi(0), m_versionSig(true),
    m_histBooked(false),
    m_h_jem_em_2d_etaPhi_jetEl_SimEqCore(0),
    m_h_jem_em_2d_etaPhi_jetEl_SimNeCore(0),
    m_h_jem_em_2d_etaPhi_jetEl_SimNoCore(0),
    m_h_jem_em_2d_etaPhi_jetEl_CoreNoSim(0),
    m_h_jem_had_2d_etaPhi_jetEl_SimEqCore(0),
    m_h_jem_had_2d_etaPhi_jetEl_SimNeCore(0),
    m_h_jem_had_2d_etaPhi_jetEl_SimNoCore(0),
    m_h_jem_had_2d_etaPhi_jetEl_CoreNoSim(0),
    m_h_jem_em_2d_etaPhi_jetEl_SimEqOverlap(0),
    m_h_jem_em_2d_etaPhi_jetEl_SimNeOverlap(0),
    m_h_jem_em_2d_etaPhi_jetEl_SimNoOverlap(0),
    m_h_jem_em_2d_etaPhi_jetEl_OverlapNoSim(0),
    m_h_jem_had_2d_etaPhi_jetEl_SimEqOverlap(0),
    m_h_jem_had_2d_etaPhi_jetEl_SimNeOverlap(0),
    m_h_jem_had_2d_etaPhi_jetEl_SimNoOverlap(0),
    m_h_jem_had_2d_etaPhi_jetEl_OverlapNoSim(0),
    m_h_jem_2d_roi_SimEqData(0),
    m_h_jem_2d_roi_SimNeData(0),
    m_h_jem_2d_roi_SimNoData(0),
    m_h_jem_2d_roi_DataNoSim(0),
    m_h_jem_2d_roi_ThreshSimEqData(0),
    m_h_jem_2d_roi_ThreshSimNeData(0),
    m_h_jem_2d_etaPhi_roi_SimEqData(0),
    m_h_jem_2d_etaPhi_roi_SimNeData(0),
    m_h_jem_2d_etaPhi_roi_SimNoData(0),
    m_h_jem_2d_etaPhi_roi_DataNoSim(0),
    m_h_jem_2d_thresh_SimEqData(0),
    m_h_jem_2d_thresh_SimNeData(0),
    m_h_jem_2d_thresh_SimNoData(0),
    m_h_jem_2d_thresh_DataNoSim(0),
    m_h_jem_2d_thresh_ThreshSimEqData(0),
    m_h_jem_2d_thresh_ThreshSimNeData(0),
    m_h_cmm_2d_thresh_JemEqCmm(0),
    m_h_cmm_2d_thresh_JemNeCmm(0),
    m_h_cmm_2d_thresh_JemNoCmm(0),
    m_h_cmm_2d_thresh_CmmNoJem(0),
    m_h_cmm_2d_thresh_ThreshJemEqCmm(0),
    m_h_cmm_2d_thresh_ThreshJemNeCmm(0),
    m_h_cmm_1d_thresh_SumsSimEqData(0),
    m_h_cmm_1d_thresh_SumsSimNeData(0),
    m_h_cmm_1d_thresh_SumsSimNoData(0),
    m_h_cmm_1d_thresh_SumsDataNoSim(0),
    m_h_cmm_2d_thresh_SumsThreshSimEqData(0),
    m_h_cmm_2d_thresh_SumsThreshSimNeData(0),
    m_h_jem_2d_energy_SimEqData(0),
    m_h_jem_2d_energy_SimNeData(0),
    m_h_jem_2d_energy_SimNoData(0),
    m_h_jem_2d_energy_DataNoSim(0),
    m_h_cmm_2d_energy_JemEqCmm(0),
    m_h_cmm_2d_energy_JemNeCmm(0),
    m_h_cmm_2d_energy_JemNoCmm(0),
    m_h_cmm_2d_energy_CmmNoJem(0),
    m_h_cmm_2d_energy_SumsSimEqData(0),
    m_h_cmm_2d_energy_SumsSimNeData(0),
    m_h_cmm_2d_energy_SumsSimNoData(0),
    m_h_cmm_2d_energy_SumsDataNoSim(0),
    m_h_cmm_2d_energy_EtMapsThreshSimEqData(0),
    m_h_cmm_2d_energy_EtMapsThreshSimNeData(0),
    m_h_jem_2d_SimEqDataOverview(0),
    m_h_jem_2d_SimNeDataOverview(0),
    m_h_jem_1d_SimNeDataSummary(0),
    m_v_2d_MismatchEvents(0)
/*---------------------------------------------------------*/
{
  declareProperty("JEPHitsTool", m_jepHitsTool);
  declareProperty("JetTool", m_jetTool);
  declareProperty("JetElementTool", m_jetElementTool);
  declareProperty("JEPEtSumsTool", m_etSumsTool);

  declareProperty("JetElementLocation",
                 m_jetElementLocation =
		                 LVL1::TrigT1CaloDefs::JetElementLocation);
  declareProperty("JetElementLocationOverlap",
                 m_jetElementLocationOverlap =
		           LVL1::TrigT1CaloDefs::JetElementLocation+"Overlap");
  declareProperty("JEMHitsLocation",
                 m_jemHitsLocation    = LVL1::TrigT1CaloDefs::JEMHitsLocation);
  declareProperty("CMMJetHitsLocation",
                 m_cmmJetHitsLocation =
		                 LVL1::TrigT1CaloDefs::CMMJetHitsLocation);
  declareProperty("JEMRoILocation",
                 m_jemRoiLocation     = LVL1::TrigT1CaloDefs::JEMRoILocation);
  declareProperty("CMMRoILocation",
                 m_cmmRoiLocation     = LVL1::TrigT1CaloDefs::CMMRoILocation);
  declareProperty("JEMEtSumsLocation",
                 m_jemEtSumsLocation = LVL1::TrigT1CaloDefs::JEMEtSumsLocation);
  declareProperty("CMMEtSumsLocation",
                 m_cmmEtSumsLocation = LVL1::TrigT1CaloDefs::CMMEtSumsLocation);
  declareProperty("TriggerTowerLocation",
                 m_triggerTowerLocation =
		                 LVL1::TrigT1CaloDefs::TriggerTowerLocation);
  declareProperty("RodHeaderLocation",
                 m_rodHeaderLocation = "RODHeaders");

  declareProperty("RootDirectory", m_rootDir = "L1Calo");
}

/*---------------------------------------------------------*/
JEPSimBSMon::~JEPSimBSMon()
/*---------------------------------------------------------*/
{
}

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

/*---------------------------------------------------------*/
StatusCode JEPSimBSMon:: initialize()
/*---------------------------------------------------------*/
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << PACKAGE_VERSION << endreq;
  m_debug = msgLvl(MSG::DEBUG);

  StatusCode sc;

  sc = ManagedMonitorToolBase::initialize();
  if (sc.isFailure()) return sc;

  sc = m_jetElementTool.retrieve();
  if( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool L1JetElementTools"
                    << endreq;
    return sc;
  }

  sc = m_jetTool.retrieve();
  if( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool L1JetTools" << endreq;
    return sc;
  }

  sc = m_jepHitsTool.retrieve();
  if( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool L1JEPHitsTools" << endreq;
    return sc;
  }

  sc = m_etSumsTool.retrieve();
  if( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool L1JEPEtSumsTools" << endreq;
    return sc;
  }

  sc = m_errorTool.retrieve();
  if( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloMonErrorTool"
                    << endreq;
    return sc;
  }

  sc = m_histTool.retrieve();
  if( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloLWHistogramTool"
                    << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode JEPSimBSMon::bookHistogramsRecurrent()
/*---------------------------------------------------------*/
{
  msg(MSG::DEBUG) << "bookHistograms entered" << endreq;

  if( m_environment == AthenaMonManager::online ) {
    // book histograms that are only made in the online environment...
  }
  	
  if( m_dataType == AthenaMonManager::cosmics ) {
    // book histograms that are only relevant for cosmics data...
  }

  if ( newLumiBlock ) { }

  if ( newRun ) {

  MgmtAttr_t attr = ATTRIB_UNMANAGED;
  std::string dir1(m_rootDir + "/JEM/Errors/Transmission_Simulation");
  MonGroup monShift( this, dir1, run, attr );
  MonGroup monExpert( this, dir1, run, attr );
  MonGroup monElements( this, dir1 + "/PPM2Elements", run, attr );
  MonGroup monRoIs( this, dir1 + "/Elements2RoIs", run, attr );
  MonGroup monHits( this, dir1 + "/RoIs2Hits", run, attr );
  MonGroup monEnergy( this, dir1 + "/Elements2Energy", run, attr );
  MonGroup monEvent1( this, dir1 + "/MismatchEventNumbers", run, attr, "",
                                                            "eventSample" );
  std::string dir2(m_rootDir + "/JEM_CMM/Errors/Transmission_Simulation");
  MonGroup monHits2( this, dir2 + "/JEM2CMMHits", run, attr );
  MonGroup monHitSums( this, dir2 + "/Hits2Sums", run, attr );
  MonGroup monEnergy2( this, dir2 + "/JEM2CMMEnergy", run, attr );
  MonGroup monEnergySums( this, dir2 + "/Energy2Sums", run, attr );
  MonGroup monEvent2( this, dir2 + "/MismatchEventNumbers", run, attr, "",
                                                            "eventSample" );

  // JetElements

  m_histTool->setMonGroup(&monElements);

  m_h_jem_em_2d_etaPhi_jetEl_SimEqCore = m_histTool->bookJEMEtaVsPhi(
    "jem_em_2d_etaPhi_jetEl_SimEqCore",
    "Core Jet Elements EM Data/Simulation Non-zero Matches");
  m_h_jem_em_2d_etaPhi_jetEl_SimNeCore = m_histTool->bookJEMEtaVsPhi(
    "jem_em_2d_etaPhi_jetEl_SimNeCore",
    "Core Jet Elements EM Data/Simulation Non-zero Mismatches");
  m_h_jem_em_2d_etaPhi_jetEl_SimNoCore = m_histTool->bookJEMEtaVsPhi(
    "jem_em_2d_etaPhi_jetEl_SimNoCore",
    "Core Jet Elements EM Simulation but no Data");
  m_h_jem_em_2d_etaPhi_jetEl_CoreNoSim = m_histTool->bookJEMEtaVsPhi(
    "jem_em_2d_etaPhi_jetEl_CoreNoSim",
    "Core Jet Elements EM Data but no Simulation");
  m_h_jem_had_2d_etaPhi_jetEl_SimEqCore = m_histTool->bookJEMEtaVsPhi(
    "jem_had_2d_etaPhi_jetEl_SimEqCore",
    "Core Jet Elements HAD Data/Simulation Non-zero Matches");
  m_h_jem_had_2d_etaPhi_jetEl_SimNeCore = m_histTool->bookJEMEtaVsPhi(
    "jem_had_2d_etaPhi_jetEl_SimNeCore",
    "Core Jet Elements HAD Data/Simulation Non-zero Mismatches");
  m_h_jem_had_2d_etaPhi_jetEl_SimNoCore = m_histTool->bookJEMEtaVsPhi(
    "jem_had_2d_etaPhi_jetEl_SimNoCore",
    "Core Jet Elements HAD Simulation but no Data");
  m_h_jem_had_2d_etaPhi_jetEl_CoreNoSim = m_histTool->bookJEMEtaVsPhi(
    "jem_had_2d_etaPhi_jetEl_CoreNoSim",
    "Core Jet Elements HAD Data but no Simulation");
  m_h_jem_em_2d_etaPhi_jetEl_SimEqOverlap = m_histTool->bookJEMEtaVsPhi(
    "jem_em_2d_etaPhi_jetEl_SimEqOverlap",
    "Overlap Jet Elements EM Data/Simulation Non-zero Matches");
  m_h_jem_em_2d_etaPhi_jetEl_SimNeOverlap = m_histTool->bookJEMEtaVsPhi(
    "jem_em_2d_etaPhi_jetEl_SimNeOverlap",
    "Overlap Jet Elements EM Data/Simulation Non-zero Mismatches");
  m_h_jem_em_2d_etaPhi_jetEl_SimNoOverlap = m_histTool->bookJEMEtaVsPhi(
    "jem_em_2d_etaPhi_jetEl_SimNoOverlap",
    "Overlap Jet Elements EM Simulation but no Data");
  m_h_jem_em_2d_etaPhi_jetEl_OverlapNoSim = m_histTool->bookJEMEtaVsPhi(
    "jem_em_2d_etaPhi_jetEl_OverlapNoSim",
    "Overlap Jet Elements EM Data but no Simulation");
  m_h_jem_had_2d_etaPhi_jetEl_SimEqOverlap = m_histTool->bookJEMEtaVsPhi(
    "jem_had_2d_etaPhi_jetEl_SimEqOverlap",
    "Overlap Jet Elements HAD Data/Simulation Non-zero Matches");
  m_h_jem_had_2d_etaPhi_jetEl_SimNeOverlap = m_histTool->bookJEMEtaVsPhi(
    "jem_had_2d_etaPhi_jetEl_SimNeOverlap",
    "Overlap Jet Elements HAD Data/Simulation Non-zero Mismatches");
  m_h_jem_had_2d_etaPhi_jetEl_SimNoOverlap = m_histTool->bookJEMEtaVsPhi(
    "jem_had_2d_etaPhi_jetEl_SimNoOverlap",
    "Overlap Jet Elements HAD Simulation but no Data");
  m_h_jem_had_2d_etaPhi_jetEl_OverlapNoSim = m_histTool->bookJEMEtaVsPhi(
    "jem_had_2d_etaPhi_jetEl_OverlapNoSim",
    "Overlap Jet Elements HAD Data but no Simulation");

  //  RoIs

  m_histTool->setMonGroup(&monRoIs);

  m_h_jem_2d_roi_SimEqData = m_histTool->bookJEMCrateModuleVsFrameLoc(
    "jem_2d_roi_SimEqData", "JEM RoI Data/Simulation Non-zero Matches");
  m_h_jem_2d_roi_SimNeData = m_histTool->bookJEMCrateModuleVsFrameLoc(
    "jem_2d_roi_SimNeData", "JEM RoI Data/Simulation Non-zero Mismatches");
  m_h_jem_2d_roi_SimNoData = m_histTool->bookJEMCrateModuleVsFrameLoc(
    "jem_2d_roi_SimNoData", "JEM RoI Simulation but no Data");
  m_h_jem_2d_roi_DataNoSim = m_histTool->bookJEMCrateModuleVsFrameLoc(
    "jem_2d_roi_DataNoSim", "JEM RoI Data but no Simulation");
  m_h_jem_2d_roi_ThreshSimEqData = m_histTool->bookJEMCrateModuleVsThresholds(
    "jem_2d_roi_ThreshSimEqData", "JEM RoI Data/Simulation Threshold Matches");
  m_h_jem_2d_roi_ThreshSimNeData = m_histTool->bookJEMCrateModuleVsThresholds(
    "jem_2d_roi_ThreshSimNeData",
    "JEM RoI Data/Simulation Threshold Mismatches");
  m_h_jem_2d_etaPhi_roi_SimEqData = m_histTool->bookJEMRoIEtaVsPhi(
    "jem_2d_etaPhi_roi_SimEqData", "JEM RoI Data/Simulation Non-zero Matches");
  m_h_jem_2d_etaPhi_roi_SimNeData = m_histTool->bookJEMRoIEtaVsPhi(
    "jem_2d_etaPhi_roi_SimNeData",
     "JEM RoI Data/Simulation Non-zero Mismatches");
  m_h_jem_2d_etaPhi_roi_SimNoData = m_histTool->bookJEMRoIEtaVsPhi(
    "jem_2d_etaPhi_roi_SimNoData", "JEM RoI Simulation but no Data");
  m_h_jem_2d_etaPhi_roi_DataNoSim = m_histTool->bookJEMRoIEtaVsPhi(
    "jem_2d_etaPhi_roi_DataNoSim", "JEM RoI Data but no Simulation");

  // JEMHits

  m_histTool->setMonGroup(&monHits);

  m_h_jem_2d_thresh_SimEqData = m_histTool->bookJEMModuleVsCrate(
    "jem_2d_thresh_SimEqData", "JEM Hits Data/Simulation Non-zero Matches");
  m_h_jem_2d_thresh_SimNeData = m_histTool->bookJEMModuleVsCrate(
    "jem_2d_thresh_SimNeData", "JEM Hits Data/Simulation Non-zero Mismatches");
  m_h_jem_2d_thresh_SimNoData = m_histTool->bookJEMModuleVsCrate(
    "jem_2d_thresh_SimNoData", "JEM Hits Simulation but no Data");
  m_h_jem_2d_thresh_DataNoSim = m_histTool->bookJEMModuleVsCrate(
    "jem_2d_thresh_DataNoSim", "JEM Hits Data but no Simulation");
  m_h_jem_2d_thresh_ThreshSimEqData = m_histTool->bookJEMCrateModuleVsThresholds(
    "jem_2d_thresh_ThreshSimEqData",
    "JEM Hits Data/Simulation Threshold Matches");
  m_h_jem_2d_thresh_ThreshSimNeData = m_histTool->bookJEMCrateModuleVsThresholds(
    "jem_2d_thresh_ThreshSimNeData",
    "JEM Hits Data/Simulation Threshold Mismatches");

  // CMMHits

  m_histTool->setMonGroup(&monHits2);

  m_h_cmm_2d_thresh_JemEqCmm = m_histTool->bookJEMModuleVsCrate(
    "cmm_2d_thresh_JemEqCmm", "CMM Hits/JEM Hits Non-zero Matches");
  m_h_cmm_2d_thresh_JemNeCmm = m_histTool->bookJEMModuleVsCrate(
    "cmm_2d_thresh_JemNeCmm", "CMM Hits/JEM Hits Non-zero Mismatches");
  m_h_cmm_2d_thresh_JemNoCmm = m_histTool->bookJEMModuleVsCrate(
    "cmm_2d_thresh_JemNoCmm", "JEM Hits but no CMM Hits");
  m_h_cmm_2d_thresh_CmmNoJem = m_histTool->bookJEMModuleVsCrate(
    "cmm_2d_thresh_CmmNoJem", "CMM Hits but no JEM Hits");
  m_h_cmm_2d_thresh_ThreshJemEqCmm = m_histTool->bookJEMCrateModuleVsThresholds(
    "cmm_2d_thresh_ThreshJemEqCmm", "CMM Hits/JEM Hits Threshold Matches");
  m_h_cmm_2d_thresh_ThreshJemNeCmm = m_histTool->bookJEMCrateModuleVsThresholds(
    "cmm_2d_thresh_ThreshJemNeCmm", "CMM Hits/JEM Hits Threshold Mismatches");

  m_histTool->setMonGroup(&monHitSums);

  // Local/Remote/Total sums

  m_h_cmm_1d_thresh_SumsSimEqData = m_histTool->book1F(
    "cmm_1d_thresh_SumsSimEqData",
    "CMM Hit Sums Data/Simulation Non-zero Matches", 6, 0, 6);
  setLabelsSH(m_h_cmm_1d_thresh_SumsSimEqData);
  m_h_cmm_1d_thresh_SumsSimNeData = m_histTool->book1F(
    "cmm_1d_thresh_SumsSimNeData",
    "CMM Hit Sums Data/Simulation Non-zero Mismatches", 6, 0, 6);
  setLabelsSH(m_h_cmm_1d_thresh_SumsSimNeData);
  m_h_cmm_1d_thresh_SumsSimNoData = m_histTool->book1F(
    "cmm_1d_thresh_SumsSimNoData",
    "CMM Hit Sums Simulation but no Data", 6, 0, 6);
  setLabelsSH(m_h_cmm_1d_thresh_SumsSimNoData);
  m_h_cmm_1d_thresh_SumsDataNoSim = m_histTool->book1F(
    "cmm_1d_thresh_SumsDataNoSim",
    "CMM Hit Sums Data but no Simulation", 6, 0, 6);
  setLabelsSH(m_h_cmm_1d_thresh_SumsDataNoSim);
  m_h_cmm_2d_thresh_SumsThreshSimEqData = m_histTool->book2F(
    "cmm_2d_thresh_SumsThreshSimEqData",
    "CMM Hit Sums Data/Simulation Threshold Matches", 6, 0, 6, 20, 0, 20);
  setLabelsSHF(m_h_cmm_2d_thresh_SumsThreshSimEqData);
  m_h_cmm_2d_thresh_SumsThreshSimNeData = m_histTool->book2F(
    "cmm_2d_thresh_SumsThreshSimNeData",
    "CMM Hit Sums Data/Simulation Threshold Mismatches", 6, 0, 6, 20, 0, 20);
  setLabelsSHF(m_h_cmm_2d_thresh_SumsThreshSimNeData);

  // JEMEtSums

  m_histTool->setMonGroup(&monEnergy);

  m_h_jem_2d_energy_SimEqData = m_histTool->bookJEMCrateModuleVsExEyEt(
    "jem_2d_energy_SimEqData", "JEM EtSums Data/Simulation Non-zero Matches");
  m_h_jem_2d_energy_SimNeData = m_histTool->bookJEMCrateModuleVsExEyEt(
    "jem_2d_energy_SimNeData",
    "JEM EtSums Data/Simulation Non-zero Mismatches");
  m_h_jem_2d_energy_SimNoData = m_histTool->bookJEMCrateModuleVsExEyEt(
    "jem_2d_energy_SimNoData", "JEM EtSums Simulation but no Data");
  m_h_jem_2d_energy_DataNoSim = m_histTool->bookJEMCrateModuleVsExEyEt(
    "jem_2d_energy_DataNoSim", "JEM EtSums Data but no Simulation");

  // CMMEtSums

  m_histTool->setMonGroup(&monEnergy2);

  m_h_cmm_2d_energy_JemEqCmm = m_histTool->bookJEMCrateModuleVsExEyEt(
    "cmm_2d_energy_JemEqCmm", "CMM EtSums/JEM EtSums Non-zero Matches");
  m_h_cmm_2d_energy_JemNeCmm = m_histTool->bookJEMCrateModuleVsExEyEt(
    "cmm_2d_energy_JemNeCmm", "CMM EtSums/JEM EtSums Non-zero Mismatches");
  m_h_cmm_2d_energy_JemNoCmm = m_histTool->bookJEMCrateModuleVsExEyEt(
    "cmm_2d_energy_JemNoCmm", "JEM EtSums but no CMM EtSums");
  m_h_cmm_2d_energy_CmmNoJem = m_histTool->bookJEMCrateModuleVsExEyEt(
    "cmm_2d_energy_CmmNoJem", "CMM EtSums but no JEM EtSums");

  m_histTool->setMonGroup(&monEnergySums);

  // Energy Crate/System sums

  m_h_cmm_2d_energy_SumsSimEqData = m_histTool->book2F(
    "cmm_2d_energy_SumsSimEqData",
    "Energy Totals Data/Simulation Non-zero Matches", 5, 0, 5, 6, 0, 6);
  setLabelsEnTot(m_h_cmm_2d_energy_SumsSimEqData);
  m_h_cmm_2d_energy_SumsSimNeData = m_histTool->book2F(
    "cmm_2d_energy_SumsSimNeData",
    "Energy Totals Data/Simulation Non-zero Mismatches", 5, 0, 5, 6, 0, 6);
  setLabelsEnTot(m_h_cmm_2d_energy_SumsSimNeData);
  m_h_cmm_2d_energy_SumsSimNoData = m_histTool->book2F(
    "cmm_2d_energy_SumsSimNoData",
    "Energy Totals Simulation but no Data", 5, 0, 5, 6, 0, 6);
  setLabelsEnTot(m_h_cmm_2d_energy_SumsSimNoData);
  m_h_cmm_2d_energy_SumsDataNoSim = m_histTool->book2F(
    "cmm_2d_energy_SumsDataNoSim",
    "Energy Totals Data but no Simulation", 5, 0, 5, 6, 0, 6);
  setLabelsEnTot(m_h_cmm_2d_energy_SumsDataNoSim);
  m_h_cmm_2d_energy_EtMapsThreshSimEqData = m_histTool->book2F(
    "cmm_2d_energy_EtMapsThreshSimEqData",
    "Et Maps Data/Simulation Threshold Matches", 6, 0, 6, 24, 0, 24);
  setLabelsEnTotThr(m_h_cmm_2d_energy_EtMapsThreshSimEqData);
  m_h_cmm_2d_energy_EtMapsThreshSimNeData = m_histTool->book2F(
    "cmm_2d_energy_EtMapsThreshSimNeData",
    "Et Maps Data/Simulation Threshold Mismatches", 6, 0, 6, 24, 0, 24);
  setLabelsEnTotThr(m_h_cmm_2d_energy_EtMapsThreshSimNeData);

  // Summary

  m_histTool->setMonGroup(&monExpert);

  m_h_jem_2d_SimEqDataOverview = m_histTool->book2F("jem_2d_SimEqDataOverview",
   "JEP Transmission/Comparison with Simulation Overview - Events with Matches;Crate/Module",
             36, 0, 36, NumberOfSummaryBins, 0, NumberOfSummaryBins);
  m_histTool->jemCMMCrateModule(m_h_jem_2d_SimEqDataOverview);
  setLabels(m_h_jem_2d_SimEqDataOverview, false);

  m_h_jem_2d_SimNeDataOverview = m_histTool->book2F("jem_2d_SimNeDataOverview",
"JEP Transmission/Comparison with Simulation Overview - Events with Mismatches;Crate/Module",
             36, 0, 36, NumberOfSummaryBins, 0, NumberOfSummaryBins);
  m_histTool->jemCMMCrateModule(m_h_jem_2d_SimNeDataOverview);
  setLabels(m_h_jem_2d_SimNeDataOverview, false);

  m_histTool->setMonGroup(&monShift);

  m_h_jem_1d_SimNeDataSummary = m_histTool->book1F("jem_1d_SimNeDataSummary",
   "JEP Transmission/Comparison with Simulation Mismatch Summary;;Events",
    NumberOfSummaryBins, 0, NumberOfSummaryBins);
  setLabels(m_h_jem_1d_SimNeDataSummary);

  // Mismatch Event Number Samples

  m_histTool->setMonGroup(&monEvent1);

  TH2I_LW* hist = 0;
  m_v_2d_MismatchEvents.clear();
  m_v_2d_MismatchEvents.resize(9, hist);
  hist = m_histTool->bookJEMEventVsCrateModule(
    "jem_em_2d_jetEl_MismatchEvents",
    "Jet Elements EM Mismatch Event Numbers");
  m_v_2d_MismatchEvents[0] = hist;
  hist = m_histTool->bookJEMEventVsCrateModule(
    "jem_had_2d_jetEl_MismatchEvents",
    "Jet Elements Had Mismatch Event Numbers");
  m_v_2d_MismatchEvents[1] = hist;
  hist = m_histTool->bookJEMEventVsCrateModule(
    "jem_2d_roi_MismatchEvents", "JEM RoIs Mismatch Event Numbers");
  m_v_2d_MismatchEvents[2] = hist;
  hist = m_histTool->bookJEMEventVsCrateModule(
    "jem_2d_thresh_MismatchEvents", "JEM Hits Mismatch Event Numbers");
  m_v_2d_MismatchEvents[3] = hist;
  hist = m_histTool->bookJEMEventVsCrateModule(
    "jem_2d_energy_MismatchEvents", "JEM Energy Mismatch Event Numbers");
  m_v_2d_MismatchEvents[5] = hist;

  m_histTool->setMonGroup(&monEvent2);

  hist = m_histTool->bookJEMEventVsCrateModule(
    "cmm_2d_thresh_MismatchEvents", "CMM Hits Mismatch Event Numbers");
  m_v_2d_MismatchEvents[4] = hist;
  hist = m_histTool->bookJEMEventVsCrateModule(
    "cmm_2d_energy_MismatchEvents", "CMM Energy Mismatch Event Numbers");
  m_v_2d_MismatchEvents[6] = hist;
  hist = m_histTool->bookEventNumbers("cmm_2d_thresh_SumsMismatchEvents",
    "CMM Hit Sums Mismatch Event Numbers", 8, 0., 8.);
  LWHist::LWHistAxis* axis = hist->GetYaxis();
  axis->SetBinLabel(1, "Modules 0");
  axis->SetBinLabel(2, "Modules 1");
  axis->SetBinLabel(3, "Local 0");
  axis->SetBinLabel(4, "Local 1");
  axis->SetBinLabel(5, "Remote");
  axis->SetBinLabel(6, "Total");
  axis->SetBinLabel(7, "JetEt");
  axis->SetBinLabel(8, "JetEt RoI");
  m_v_2d_MismatchEvents[7] = hist;
  hist = m_histTool->bookEventNumbers("cmm_2d_energy_SumsMismatchEvents",
    "CMM Energy Sums Mismatch Event Numbers", 10, 0., 10.);
  axis = hist->GetYaxis();
  axis->SetBinLabel(1, "Modules 0");
  axis->SetBinLabel(2, "Modules 1");
  axis->SetBinLabel(3, "Local 0");
  axis->SetBinLabel(4, "Local 1");
  axis->SetBinLabel(5, "Remote");
  axis->SetBinLabel(6, "Total");
  axis->SetBinLabel(7, "SumEt");
  axis->SetBinLabel(8, "MissingEt");
  axis->SetBinLabel(9, "MissEtSig");
  axis->SetBinLabel(10, "Engy RoIs");
  m_v_2d_MismatchEvents[8] = hist;

  m_histTool->unsetMonGroup();
  m_histBooked = true;

  } // end if (newRun ...

  msg(MSG::DEBUG) << "Leaving bookHistograms" << endreq;
  
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode JEPSimBSMon::fillHistograms()
/*---------------------------------------------------------*/
{
  if (m_debug) msg(MSG::DEBUG) << "fillHistograms entered" << endreq;

  if (!m_histBooked) {
    if (m_debug) msg(MSG::DEBUG) << "Histogram(s) not booked" << endreq;
    return StatusCode::SUCCESS;
  }

  // Skip events believed to be corrupt or with ROB errors

  if (m_errorTool->corrupt() || m_errorTool->robOrUnpackingError()) {
    if (m_debug) msg(MSG::DEBUG) << "Skipping corrupt event" << endreq;
    return StatusCode::SUCCESS;
  }

  StatusCode sc;

  //Retrieve Trigger Towers from SG
  const TriggerTowerCollection* triggerTowerTES = 0; 
  sc = evtStore()->retrieve(triggerTowerTES, m_triggerTowerLocation); 
  if( sc.isFailure()  ||  !triggerTowerTES ) {
    msg(MSG::DEBUG) << "No Trigger Tower container found" << endreq; 
  }

  //Retrieve Core and Overlap Jet Elements from SG
  const JetElementCollection* jetElementTES = 0; 
  const JetElementCollection* jetElementOvTES = 0; 
  sc = evtStore()->retrieve(jetElementTES, m_jetElementLocation); 
  if( sc.isFailure()  ||  !jetElementTES ) {
    msg(MSG::DEBUG) << "No Core Jet Element container found" << endreq; 
  }
  sc = evtStore()->retrieve(jetElementOvTES, m_jetElementLocationOverlap);
  if( sc.isFailure()  ||  !jetElementOvTES ) {
    msg(MSG::DEBUG) << "No Overlap Jet Element container found" << endreq;
  }
  
  //Retrieve JEM RoIs from SG
  const JemRoiCollection* jemRoiTES = 0;
  sc = evtStore()->retrieve( jemRoiTES, m_jemRoiLocation);
  if( sc.isFailure()  ||  !jemRoiTES  ||  jemRoiTES->empty() ) {
    msg(MSG::DEBUG) << "No DAQ JEM RoIs container found" << endreq; 
  }

  //Retrieve ROD Headers from SG
  m_rodTES = 0;
  m_limitedRoi = 0;
  m_versionSig = true;
  sc = evtStore()->retrieve( m_rodTES, m_rodHeaderLocation);
  if( sc.isFailure()  ||  !m_rodTES ) {
    msg(MSG::DEBUG) << "No ROD Header container found" << endreq;
  }
  
  //Retrieve JEM Hits from SG
  const JemHitsCollection* jemHitsTES = 0;
  sc = evtStore()->retrieve( jemHitsTES, m_jemHitsLocation);
  if( sc.isFailure()  ||  !jemHitsTES ) {
    msg(MSG::DEBUG) << "No JEM Hits container found" << endreq; 
  }
  
  //Retrieve CMM-Jet Hits from SG
  const CmmJetHitsCollection* cmmJetHitsTES = 0;
  sc = evtStore()->retrieve( cmmJetHitsTES, m_cmmJetHitsLocation);
  if( sc.isFailure()  ||  !cmmJetHitsTES ) {
    msg(MSG::DEBUG) << "No CMM-Jet Hits container found" << endreq; 
  }
  
  //Retrieve CMM RoIs from SG
  const LVL1::CMMRoI* cmmRoiTES = 0;
  sc = evtStore()->retrieve( cmmRoiTES, m_cmmRoiLocation);
  if( sc.isFailure()  ||  !cmmRoiTES  || ( !cmmRoiTES->jetEtRoiWord() &&
         !cmmRoiTES->energyRoiWord0() && !cmmRoiTES->energyRoiWord1() &&
                                         !cmmRoiTES->energyRoiWord2())) {
    msg(MSG::DEBUG) << "No DAQ CMM RoIs found" << endreq; 
  }

  //Retrieve JEM Et Sums from SG
  const JemEtSumsCollection* jemEtSumsTES = 0;
  sc = evtStore()->retrieve( jemEtSumsTES, m_jemEtSumsLocation);
  if( sc.isFailure()  ||  !jemEtSumsTES ) {
    msg(MSG::DEBUG) << "No JEM Et Sums container found" << endreq;
  }

  //Retrieve CMM Et Sums from SG
  const CmmEtSumsCollection* cmmEtSumsTES = 0;
  sc = evtStore()->retrieve( cmmEtSumsTES, m_cmmEtSumsLocation);
  if( sc.isFailure()  ||  !cmmEtSumsTES ) {
    msg(MSG::DEBUG) << "No CMM-Energy Et Sums container found" << endreq;
  }

  // Maps to simplify comparisons
  
  JetElementMap jeMap;
  JetElementMap ovMap;
  JemRoiMap     jrMap;
  JemHitsMap    jhMap;
  CmmJetHitsMap cmMap;
  JemEtSumsMap  jsMap;
  CmmEtSumsMap  csMap;
  setupMap(jetElementTES,   jeMap);
  setupMap(jetElementOvTES, ovMap);
  setupMap(jemRoiTES,       jrMap);
  setupMap(jemHitsTES,      jhMap);
  setupMap(cmmJetHitsTES,   cmMap);
  setupMap(jemEtSumsTES,    jsMap);
  setupMap(cmmEtSumsTES,    csMap);

  // Vectors for error overview bits;
  const int nCrates = 2;
  const int nJEMs   = 16;
  const int nCMMs   = 2;
  const int vecsizeJem = 2 * nCrates * nJEMs;
  const int vecsizeCmm = 2 * nCrates * nCMMs;
  ErrorVector errorsJEM(vecsizeJem);
  ErrorVector errorsCMM(vecsizeCmm);

  // Compare Jet Elements simulated from Trigger Towers with Jet Elements
  // from data

  JetElementCollection* jetElementSIM = 0;
  if (triggerTowerTES) {
    jetElementSIM = new JetElementCollection;
    simulate(triggerTowerTES, jetElementSIM);
  }
  JetElementMap jeSimMap;
  setupMap(jetElementSIM, jeSimMap);
  bool overlap = false;
  bool mismatchCore = false;
  bool mismatchOverlap = false;
  mismatchCore = compare(jeSimMap, jeMap, errorsJEM, overlap);
  if (jetElementOvTES) {
    overlap = true;
    mismatchOverlap = compare(jeSimMap, ovMap, errorsJEM, overlap);
  }
  jeSimMap.clear();
  delete jetElementSIM;

  // Compare RoIs simulated from Jet Elements with JEM RoIs from data

  JemRoiCollection* jemRoiSIM = 0;
  if (jetElementTES || jetElementOvTES) {
    jemRoiSIM = new JemRoiCollection;
    if (mismatchCore || mismatchOverlap) {
      simulate(jetElementTES, jetElementOvTES, jemRoiSIM);
    } else {
      simulate(jetElementTES, jemRoiSIM);
    }
  }
  JemRoiMap jrSimMap;
  setupMap(jemRoiSIM, jrSimMap);
  compare(jrSimMap, jrMap, errorsJEM);
  jrSimMap.clear();
  delete jemRoiSIM;

  // Compare JEM Hits simulated from JEM RoIs with JEM Hits from data

  JemHitsCollection* jemHitsSIM = 0;
  if (jemRoiTES) {
    jemHitsSIM = new JemHitsCollection;
    simulate(jemRoiTES, jemHitsSIM);
  }
  JemHitsMap jhSimMap;
  setupMap(jemHitsSIM, jhSimMap);
  compare(jhSimMap, jhMap, errorsJEM);
  jhSimMap.clear();
  delete jemHitsSIM;

  // Compare JEM hits with CMM Hits from data

  compare(jhMap, cmMap, errorsJEM, errorsCMM);

  // Compare Local sums simulated from CMM Hits with Local sums from data

  CmmJetHitsCollection* cmmLocalSIM = 0;
  if (cmmJetHitsTES) {
    cmmLocalSIM = new CmmJetHitsCollection;
    simulate(cmmJetHitsTES, cmmLocalSIM, LVL1::CMMJetHits::LOCAL_MAIN);
  }
  CmmJetHitsMap cmmLocalSimMap;
  setupMap(cmmLocalSIM, cmmLocalSimMap);
  compare(cmmLocalSimMap, cmMap, errorsCMM, LVL1::CMMJetHits::LOCAL_MAIN);
  cmmLocalSimMap.clear();
  delete cmmLocalSIM;

  // Compare Local sums with Remote sums from data

  compare(cmMap, cmMap, errorsCMM, LVL1::CMMJetHits::REMOTE_MAIN);

  // Compare Total sums simulated from Remote sums with Total sums from data

  CmmJetHitsCollection* cmmTotalSIM = 0;
  if (cmmJetHitsTES) {
    cmmTotalSIM = new CmmJetHitsCollection;
    simulate(cmmJetHitsTES, cmmTotalSIM, LVL1::CMMJetHits::TOTAL_MAIN);
  }
  CmmJetHitsMap cmmTotalSimMap;
  setupMap(cmmTotalSIM, cmmTotalSimMap);
  compare(cmmTotalSimMap, cmMap, errorsCMM, LVL1::CMMJetHits::TOTAL_MAIN);
  cmmTotalSimMap.clear();
  delete cmmTotalSIM;

  // Compare JetEt Map simulated from Total sums with JetEt Map from data

  CmmJetHitsCollection* cmmJetEtSIM = 0;
  if (cmmJetHitsTES) {
    cmmJetEtSIM = new CmmJetHitsCollection;
    simulate(cmmJetHitsTES, cmmJetEtSIM, LVL1::CMMJetHits::ET_MAP);
  }
  CmmJetHitsMap cmmJetEtSimMap;
  setupMap(cmmJetEtSIM, cmmJetEtSimMap);
  compare(cmmJetEtSimMap, cmMap, errorsCMM, LVL1::CMMJetHits::ET_MAP);
  cmmJetEtSimMap.clear();
  delete cmmJetEtSIM;

  // Compare JetEt Map with JetEt RoI from data

  compare(cmMap, cmmRoiTES, errorsCMM);

  // Compare JEMEtSums simulated from JetElements with JEMEtSums from data

  JemEtSumsCollection* jemEtSumsSIM = 0;
  if (jetElementTES) {
    jemEtSumsSIM = new JemEtSumsCollection;
    simulate(jetElementTES, jemEtSumsSIM);
  }
  JemEtSumsMap jemEtSumsSimMap;
  setupMap(jemEtSumsSIM, jemEtSumsSimMap);
  compare(jemEtSumsSimMap, jsMap, errorsJEM);
  jemEtSumsSimMap.clear();
  delete jemEtSumsSIM;

  // Compare JEMEtSums with CMMEtSums from data

  compare(jsMap, csMap, errorsJEM, errorsCMM);

  // Compare Local sums simulated from CMMEtSums with Local sums from data

  CmmEtSumsCollection* cmmEtLocalSIM = 0;
  if (cmmEtSumsTES) {
    cmmEtLocalSIM = new CmmEtSumsCollection;
    simulate(cmmEtSumsTES, cmmEtLocalSIM, LVL1::CMMEtSums::LOCAL);
  }
  CmmEtSumsMap cmmEtLocalSimMap;
  setupMap(cmmEtLocalSIM, cmmEtLocalSimMap);
  compare(cmmEtLocalSimMap, csMap, errorsCMM, LVL1::CMMEtSums::LOCAL);
  cmmEtLocalSimMap.clear();
  delete cmmEtLocalSIM;

  // Compare Local Energy sums with Remote sums from data

  compare(csMap, csMap, errorsCMM, LVL1::CMMEtSums::REMOTE);

  // Compare Total sums simulated from Remote sums with Total sums from data

  CmmEtSumsCollection* cmmEtTotalSIM = 0;
  if (cmmEtSumsTES) {
    cmmEtTotalSIM = new CmmEtSumsCollection;
    simulate(cmmEtSumsTES, cmmEtTotalSIM, LVL1::CMMEtSums::TOTAL);
  }
  CmmEtSumsMap cmmEtTotalSimMap;
  setupMap(cmmEtTotalSIM, cmmEtTotalSimMap);
  compare(cmmEtTotalSimMap, csMap, errorsCMM, LVL1::CMMEtSums::TOTAL);
  cmmEtTotalSimMap.clear();
  delete cmmEtTotalSIM;

  // Compare Et Maps (sumEt/missingEt/missingEtSig) simulated from Total sums
  // with Et Maps from data

  CmmEtSumsCollection* cmmSumEtSIM = 0;
  if (cmmEtSumsTES) {
    cmmSumEtSIM = new CmmEtSumsCollection;
    simulate(cmmEtSumsTES, cmmSumEtSIM, LVL1::CMMEtSums::SUM_ET_MAP);
  }
  CmmEtSumsMap cmmSumEtSimMap;
  setupMap(cmmSumEtSIM, cmmSumEtSimMap);
  compare(cmmSumEtSimMap, csMap, errorsCMM, LVL1::CMMEtSums::SUM_ET_MAP);
  cmmSumEtSimMap.clear();
  delete cmmSumEtSIM;

  // Compare Total Energy sums and Et Maps with Energy RoIs from data

  compare(csMap, cmmRoiTES, errorsCMM);

  // Update error summary plots

  ErrorVector crateErr(nCrates);
  const int jemBins = nCrates * nJEMs;
  const int cmmBins = nCrates * nCMMs;
  for (int err = 0; err < NumberOfSummaryBins; ++err) {
    int error = 0;
    for (int loc = 0; loc < jemBins; ++loc) {
      if ((errorsJEM[loc] >> err) & 0x1) {
        m_h_jem_2d_SimEqDataOverview->Fill(loc, err, 1.);
      }
      if ((errorsJEM[loc + jemBins] >> err) & 0x1) {
        m_h_jem_2d_SimNeDataOverview->Fill(loc, err, 1.);
	error = 1;
	crateErr[loc/nJEMs] |= (1 << err);
	fillEventSample(err, loc, true);
      }
      if (loc < cmmBins) {
        if ((errorsCMM[loc] >> err) & 0x1) {
          m_h_jem_2d_SimEqDataOverview->Fill(loc+jemBins, err, 1.);
        }
        if ((errorsCMM[loc + cmmBins] >> err) & 0x1) {
          m_h_jem_2d_SimNeDataOverview->Fill(loc+jemBins, err, 1.);
	  error = 1;
	  crateErr[loc/nCMMs] |= (1 << err);
	  fillEventSample(err, loc, false);
        }
      }
    }
    if (error) m_h_jem_1d_SimNeDataSummary->Fill(err);
  }

  // Save error vector for global summary

  ErrorVector* save = new ErrorVector(crateErr);
  sc = evtStore()->record(save, "L1CaloJEMMismatchVector");
  if (sc != StatusCode::SUCCESS) {
    msg(MSG::ERROR) << "Error recording JEM mismatch vector in TES "
                    << endreq;
    return sc;
  }

  if (m_debug) msg(MSG::DEBUG) << "Leaving fillHistograms" << endreq;

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode JEPSimBSMon::procHistograms()
/*---------------------------------------------------------*/
{
  msg(MSG::DEBUG) << "procHistograms entered" << endreq;

  if (endOfLumiBlock || endOfRun) {
  }

  return StatusCode::SUCCESS;
}

//  Compare Simulated JetElements with data

bool JEPSimBSMon::compare(const JetElementMap& jeSimMap,
                          const JetElementMap& jeMap, ErrorVector& errors,
			  bool overlap)
{
  if (m_debug) {
    msg(MSG::DEBUG) << "Compare Simulated JetElements with data" << endreq;
  }

  bool mismatch = false;

  JetElementMap::const_iterator simMapIter    = jeSimMap.begin();
  JetElementMap::const_iterator simMapIterEnd = jeSimMap.end();
  JetElementMap::const_iterator datMapIter    = jeMap.begin();
  JetElementMap::const_iterator datMapIterEnd = jeMap.end();

  while (simMapIter != simMapIterEnd || datMapIter != datMapIterEnd) {

    int simKey = 0;
    int datKey = 0;
    int simEm  = 0;
    int simHad = 0;
    int datEm  = 0;
    int datHad = 0;
    double eta = 0.;
    double phi = 0.;

    if (simMapIter != simMapIterEnd) simKey = simMapIter->first;
    if (datMapIter != datMapIterEnd) datKey = datMapIter->first;

    if ((datMapIter == datMapIterEnd) ||
                ((simMapIter != simMapIterEnd) && (datKey > simKey))) {

      // Simulated JetElement but no data JetElement

      const LVL1::JetElement* je = simMapIter->second;
      simEm  = je->emEnergy();
      simHad = je->hadEnergy();
      eta = je->eta();
      phi = je->phi();
      ++simMapIter;

    } else if ((simMapIter == simMapIterEnd) ||
                ((datMapIter != datMapIterEnd) && (simKey > datKey))) {

      // Data JetElement but no simulated JetElement

      const LVL1::JetElement* je = datMapIter->second;
      datEm  = je->emEnergy();
      datHad = je->hadEnergy();
      eta = je->eta();
      phi = je->phi();
      ++datMapIter;

    } else {

      // Have both

      const LVL1::JetElement* jeS = simMapIter->second;
      const LVL1::JetElement* jeD = datMapIter->second;
      simEm  = jeS->emEnergy();
      simHad = jeS->hadEnergy();
      datEm  = jeD->emEnergy();
      datHad = jeD->hadEnergy();
      eta = jeD->eta();
      phi = jeD->phi();
      ++simMapIter;
      ++datMapIter;
    }

    if (!simEm && !simHad && !datEm && !datHad) continue;
    
    //  Fill in error vectors

    const LVL1::Coordinate coord(phi, eta);
    LVL1::CoordToHardware converter;
    const int crate = (overlap) ? converter.jepCrateOverlap(coord)
                                : converter.jepCrate(coord);
    const int jem   = (overlap) ? converter.jepModuleOverlap(coord)
                                : converter.jepModule(coord);
    if (crate > 1 || jem > 15) continue;
    const int loc   = crate * 16 + jem;
    const int jemBins = 2 * 16;
    const int bitEm  = (1 << EMElementMismatch);
    const int bitHad = (1 << HadElementMismatch);
    if (simEm && simEm == datEm)    errors[loc] |= bitEm;
    if (simHad && simHad == datHad) errors[loc] |= bitHad;
    if (simEm != datEm)     errors[loc+jemBins] |= bitEm;
    if (simHad != datHad)   errors[loc+jemBins] |= bitHad;
    if (simEm != datEm || simHad != datHad) {
      mismatch = true;
      if (m_debug) {
        msg(MSG::VERBOSE) << "JE mismatch, EM data/sim: " << datEm << "/"
              << simEm << " Had data/sim: " << datHad << "/" << simHad
	      << " crate/jem: " << crate << "/" << jem
	      << " eta/phi: " << eta << "/" << phi
	      << endreq;
      }
    }
    TH2F_LW* hist1 = 0;
    TH2F_LW* hist2 = 0;
    if (overlap) {
      if (simEm && simEm == datEm)              hist1 = m_h_jem_em_2d_etaPhi_jetEl_SimEqOverlap;
      if (simEm && datEm && simEm != datEm)     hist1 = m_h_jem_em_2d_etaPhi_jetEl_SimNeOverlap;
      if (simEm && !datEm)                      hist1 = m_h_jem_em_2d_etaPhi_jetEl_SimNoOverlap;
      if (datEm && !simEm)                      hist1 = m_h_jem_em_2d_etaPhi_jetEl_OverlapNoSim;
      if (simHad && simHad == datHad)           hist2 = m_h_jem_had_2d_etaPhi_jetEl_SimEqOverlap;
      if (simHad && datHad && simHad != datHad) hist2 = m_h_jem_had_2d_etaPhi_jetEl_SimNeOverlap;
      if (simHad && !datHad)                    hist2 = m_h_jem_had_2d_etaPhi_jetEl_SimNoOverlap;
      if (datHad && !simHad)                    hist2 = m_h_jem_had_2d_etaPhi_jetEl_OverlapNoSim;
    } else {
      if (simEm && simEm == datEm)              hist1 = m_h_jem_em_2d_etaPhi_jetEl_SimEqCore;
      if (simEm && datEm && simEm != datEm)     hist1 = m_h_jem_em_2d_etaPhi_jetEl_SimNeCore;
      if (simEm && !datEm)                      hist1 = m_h_jem_em_2d_etaPhi_jetEl_SimNoCore;
      if (datEm && !simEm)                      hist1 = m_h_jem_em_2d_etaPhi_jetEl_CoreNoSim;
      if (simHad && simHad == datHad)           hist2 = m_h_jem_had_2d_etaPhi_jetEl_SimEqCore;
      if (simHad && datHad && simHad != datHad) hist2 = m_h_jem_had_2d_etaPhi_jetEl_SimNeCore;
      if (simHad && !datHad)                    hist2 = m_h_jem_had_2d_etaPhi_jetEl_SimNoCore;
      if (datHad && !simHad)                    hist2 = m_h_jem_had_2d_etaPhi_jetEl_CoreNoSim;
    }
    if (hist1) m_histTool->fillJEMEtaVsPhi(hist1, eta, phi);
    if (hist2) m_histTool->fillJEMEtaVsPhi(hist2, eta, phi);
  }

  return mismatch;
}

//  Compare Simulated RoIs with data

void JEPSimBSMon::compare(const JemRoiMap& roiSimMap,
                          const JemRoiMap& roiMap,
                                ErrorVector& errors)
{
  if (m_debug) msg(MSG::DEBUG) << "Compare Simulated RoIs with data" << endreq;

  const int nCrates = 2;
  const int nJEMs = 16;
  LVL1::JEPRoIDecoder decoder;
  JemRoiMap::const_iterator simMapIter    = roiSimMap.begin();
  JemRoiMap::const_iterator simMapIterEnd = roiSimMap.end();
  JemRoiMap::const_iterator datMapIter    = roiMap.begin();
  JemRoiMap::const_iterator datMapIterEnd = roiMap.end();

  while (simMapIter != simMapIterEnd || datMapIter != datMapIterEnd) {

    int simKey = 0;
    int datKey = 0;
    unsigned int simHits = 0;
    unsigned int datHits = 0;
    const LVL1::JEMRoI* roi = 0;

    if (simMapIter != simMapIterEnd) simKey = simMapIter->first;
    if (datMapIter != datMapIterEnd) datKey = datMapIter->first;

    if ((datMapIter == datMapIterEnd) ||
                     ((simMapIter != simMapIterEnd) && (datKey > simKey))) {

      // Simulated RoI but no data RoI

      roi = simMapIter->second;
      simHits = roi->hits();
      ++simMapIter;
      if (m_debug) {
        msg(MSG::VERBOSE)
            << "Sim  RoI crate/jem/frame/loc/fwd/error/hits: "
            << roi->crate() << "/" << roi->jem() << "/" << roi->frame() << "/"
	    << roi->location() << "/" << roi->forward() << "/"
	    << roi->error() << "/" << MSG::hex << roi->hits() << MSG::dec
	    << endreq;
      }

    } else if ((simMapIter == simMapIterEnd) ||
                     ((datMapIter != datMapIterEnd) && (simKey > datKey))) {

      // Data RoI but no simulated RoI

      roi = datMapIter->second;
      datHits = roi->hits();
      ++datMapIter;
      if (m_debug) {
        msg(MSG::VERBOSE)
            << "Data RoI crate/jem/frame/loc/fwd/error/hits: "
            << roi->crate() << "/" << roi->jem() << "/" << roi->frame() << "/"
	    << roi->location() << "/" << roi->forward() << "/"
	    << roi->error() << "/" << MSG::hex << roi->hits() << MSG::dec
	    << endreq;
      }

    } else {

      // Have both

      const LVL1::JEMRoI* roiS = simMapIter->second;
      roi     = datMapIter->second;
      simHits = roiS->hits();
      datHits = roi->hits();
      ++simMapIter;
      ++datMapIter;
      if (m_debug) {
        msg(MSG::VERBOSE)
            << "Sim  RoI crate/jem/frame/loc/fwd/error/hits: "
            << roiS->crate() << "/" << roiS->jem() << "/" << roiS->frame() << "/"
	    << roiS->location() << "/" << roiS->forward() << "/"
	    << roiS->error() << "/" << MSG::hex << roiS->hits() << MSG::dec
	    << endreq;
        msg(MSG::VERBOSE)
            << "Data RoI crate/jem/frame/loc/fwd/error/hits: "
            << roi->crate() << "/" << roi->jem() << "/" << roi->frame() << "/"
	    << roi->location() << "/" << roi->forward() << "/"
	    << roi->error() << "/" << MSG::hex << roi->hits() << MSG::dec
	    << endreq;
      }
    }

    if (!simHits && !datHits) continue;

    //  Check LimitedRoISet bit

    const int crate = roi->crate();
    if (!datHits && limitedRoiSet(crate)) continue;
    
    //  Fill in error plots

    const int jem   = roi->jem();
    const int frame = roi->frame();
    const int local = roi->location();
    const int forward = roi->forward();
    const int locX  = crate * nJEMs + jem;
    const int locY  = frame * 4 + local;
    const int jemBins = nCrates * nJEMs;
    const int bit = (1 << RoIMismatch);
    const LVL1::CoordinateRange coord(decoder.coordinate(roi->roiWord()));
    double eta = coord.eta();
    // Distinguish right forward columns 3 and 4 for checking purposes
    if (forward && eta > 0.0 && frame > 3) eta = (local%2) ? 4.05 : 3.2;
    const double phi = coord.phi();

    TH2F_LW* hist1 = 0;
    TH2F_LW* hist2 = 0;
    if (simHits == datHits) {
      errors[locX] |= bit;
      hist1 = m_h_jem_2d_roi_SimEqData;
      hist2 = m_h_jem_2d_etaPhi_roi_SimEqData;
    } else {
      errors[locX+jemBins] |= bit;
      if (simHits && datHits) {
        hist1 = m_h_jem_2d_roi_SimNeData;
  	hist2 = m_h_jem_2d_etaPhi_roi_SimNeData;
      } else if (!datHits) {
        hist1 = m_h_jem_2d_roi_SimNoData;
  	hist2 = m_h_jem_2d_etaPhi_roi_SimNoData;
      } else {
        hist1 = m_h_jem_2d_roi_DataNoSim;
  	hist2 = m_h_jem_2d_etaPhi_roi_DataNoSim;
      }
    }
    if (hist1) hist1->Fill(locX, locY);
    if (hist2) m_histTool->fillJEMRoIEtaVsPhi(hist2, eta, phi);

    const int nThresh = (forward) ? 4 : 8;
    const int offset  = (forward) ? ((eta < 0.) ? 8 : 12) : 0;
    m_histTool->fillXVsThresholds(m_h_jem_2d_roi_ThreshSimEqData, locX,
                                  datHits & simHits, nThresh, 1, offset);
    m_histTool->fillXVsThresholds(m_h_jem_2d_roi_ThreshSimNeData, locX,
                                  datHits ^ simHits, nThresh, 1, offset);

    if (m_debug && (simHits != datHits)) {
      msg(MSG::VERBOSE) << " RoI Mismatch Crate/JEM DataHits/SimHits: "
                        << crate << "/" << jem << " "
			<< m_histTool->thresholdString(datHits, nThresh)
			<< " / "
			<< m_histTool->thresholdString(simHits, nThresh)
			<< endreq;
    }
  }
}

//  Compare simulated JEM Hits with data

void JEPSimBSMon::compare(const JemHitsMap& jemSimMap,
                          const JemHitsMap& jemMap,
                                ErrorVector& errors)
{
  if (m_debug) {
    msg(MSG::DEBUG) << "Compare simulated JEM Hits with data" << endreq;
  }

  JemHitsMap::const_iterator simMapIter    = jemSimMap.begin();
  JemHitsMap::const_iterator simMapIterEnd = jemSimMap.end();
  JemHitsMap::const_iterator datMapIter    = jemMap.begin();
  JemHitsMap::const_iterator datMapIterEnd = jemMap.end();

  while (simMapIter != simMapIterEnd || datMapIter != datMapIterEnd) {

    int simKey = 0;
    int datKey = 0;
    unsigned int simHits = 0;
    unsigned int datHits = 0;
    int crate = 0;
    int jem   = 0;

    if (simMapIter != simMapIterEnd) simKey = simMapIter->first;
    if (datMapIter != datMapIterEnd) datKey = datMapIter->first;

    if ((datMapIter == datMapIterEnd) ||
                ((simMapIter != simMapIterEnd) && (datKey > simKey))) {

      // Simulation Hits but no data Hits

      const LVL1::JEMHits* simh = simMapIter->second;
      simHits = simh->JetHits();
      crate   = simh->crate();
      jem     = simh->module();
      ++simMapIter;

    } else if ((simMapIter == simMapIterEnd) ||
                ((datMapIter != datMapIterEnd) && (simKey > datKey))) {

      // Data Hits but no simulation Hits

      const LVL1::JEMHits* dath = datMapIter->second;
      datHits = dath->JetHits();
      crate   = dath->crate();
      jem     = dath->module();
      ++datMapIter;

    } else {

      // Have both

      const LVL1::JEMHits* simh = simMapIter->second;
      const LVL1::JEMHits* dath = datMapIter->second;
      simHits = simh->JetHits();
      datHits = dath->JetHits();
      crate   = dath->crate();
      jem     = dath->module();
      ++simMapIter;
      ++datMapIter;
    }

    if (!simHits && !datHits) continue;

    //  Check LimitedRoISet bit

    if ((simHits < datHits) && limitedRoiSet(crate)) continue;
    
    //  Fill in error plots

    const int loc = crate * 16 + jem;
    const int jemBins = 2 * 16;
    const int bit = (1 << JEMHitsMismatch);
    TH2F_LW* hist = 0;
    if (simHits == datHits) {
      errors[loc] |= bit;
      hist = m_h_jem_2d_thresh_SimEqData;
    } else {
      errors[loc+jemBins] |= bit;
      if (simHits && datHits) hist = m_h_jem_2d_thresh_SimNeData;
      else if (simHits)       hist = m_h_jem_2d_thresh_SimNoData;
      else                    hist = m_h_jem_2d_thresh_DataNoSim;
    }
    if (hist) hist->Fill(jem, crate);
    
    const bool forward = (jem == 0 || jem == 7 || jem == 8 || jem == 15);
    const int thrLen = (forward) ? 2 : 3;
    int nThresh = 8;
    int same = m_histTool->thresholdsSame(datHits, simHits, nThresh, thrLen);
    int diff = m_histTool->thresholdsDiff(datHits, simHits, nThresh, thrLen);
    m_histTool->fillXVsThresholds(m_h_jem_2d_thresh_ThreshSimEqData, loc, same,
                                                             nThresh, 1);
    m_histTool->fillXVsThresholds(m_h_jem_2d_thresh_ThreshSimNeData, loc, diff,
                                                             nThresh, 1);
    if (forward) {
      const int shift = nThresh*thrLen;
      nThresh = 4;
      same = m_histTool->thresholdsSame(datHits>>shift, simHits>>shift,
                                                             nThresh, thrLen);
      diff = m_histTool->thresholdsDiff(datHits>>shift, simHits>>shift,
                                                             nThresh, thrLen);
      const int offset = (jem == 7 || jem == 15) ? 12 : 8;
      m_histTool->fillXVsThresholds(m_h_jem_2d_thresh_ThreshSimEqData, loc, same,
                                                         nThresh, 1, offset);
      m_histTool->fillXVsThresholds(m_h_jem_2d_thresh_ThreshSimNeData, loc, diff,
                                                         nThresh, 1, offset);
    }
  }
}

//  Compare JEM Hits and CMM Hits

void JEPSimBSMon::compare(const JemHitsMap& jemMap,
                          const CmmJetHitsMap& cmmMap,
                                ErrorVector& errorsJEM,
				ErrorVector& errorsCMM)
{
  if (m_debug) msg(MSG::DEBUG) << "Compare JEM Hits and CMM Hits" << endreq;

  JemHitsMap::const_iterator    jemMapIter    = jemMap.begin();
  JemHitsMap::const_iterator    jemMapIterEnd = jemMap.end();
  CmmJetHitsMap::const_iterator cmmMapIter    = cmmMap.begin();
  CmmJetHitsMap::const_iterator cmmMapIterEnd = cmmMap.end();

  while (jemMapIter != jemMapIterEnd || cmmMapIter != cmmMapIterEnd) {

    int jemKey = 0;
    int cmmKey = 0;
    unsigned int jemHits = 0;
    unsigned int cmmHits = 0;
    int crate  = 0;
    int jem    = 0;

    if (jemMapIter != jemMapIterEnd) jemKey = jemMapIter->first;
    if (cmmMapIter != cmmMapIterEnd) cmmKey = cmmMapIter->first;

    if ((cmmMapIter == cmmMapIterEnd) ||
                ((jemMapIter != jemMapIterEnd) && (cmmKey > jemKey))) {

      // JEM Hits but no CMM Hits

      const LVL1::JEMHits* jemh = jemMapIter->second;
      jemHits = jemh->JetHits();
      crate   = jemh->crate();
      jem     = jemh->module();
      ++jemMapIter;

    } else if ((jemMapIter == jemMapIterEnd) ||
                ((cmmMapIter != cmmMapIterEnd) && (jemKey > cmmKey))) {

      // CMM Hits but no JEM Hits

      const LVL1::CMMJetHits* cmmh = cmmMapIter->second;
      cmmHits = cmmh->Hits();
      crate   = cmmh->crate();
      jem     = cmmh->dataID();
      ++cmmMapIter;
      if (jem > 15) continue;

    } else {

      // Have both

      const LVL1::JEMHits*    jemh = jemMapIter->second;
      const LVL1::CMMJetHits* cmmh = cmmMapIter->second;
      jemHits = jemh->JetHits();
      cmmHits = cmmh->Hits();
      crate   = jemh->crate();
      jem     = jemh->module();
      ++jemMapIter;
      ++cmmMapIter;
    }

    if (!jemHits && !cmmHits) continue;
    
    //  Fill in error plots

    const int loc  = crate * 16 + jem;
    const int loc2 = crate * 2 + 1; // CMM-Jet is right ==> cmm 1
    const int jemBins = 2 * 16;
    const int cmmBins = 2 * 2;
    const int bit = (1 << CMMJetHitsMismatch);
    TH2F_LW* hist = 0;
    if (jemHits == cmmHits) {
      errorsJEM[loc]  |= bit;
      errorsCMM[loc2] |= bit;
      hist = m_h_cmm_2d_thresh_JemEqCmm;
    } else {
      errorsJEM[loc+jemBins]  |= bit;
      errorsCMM[loc2+cmmBins] |= bit;
      if (jemHits && cmmHits) hist = m_h_cmm_2d_thresh_JemNeCmm;
      else if (jemHits)       hist = m_h_cmm_2d_thresh_JemNoCmm;
      else                    hist = m_h_cmm_2d_thresh_CmmNoJem;
    }
    if (hist) hist->Fill(jem, crate);

    const bool forward = (jem == 0 || jem == 7 || jem == 8 || jem == 15);
    const int thrLen = (forward) ? 2 : 3;
    int nThresh = 8;
    int same = m_histTool->thresholdsSame(cmmHits, jemHits, nThresh, thrLen);
    int diff = m_histTool->thresholdsDiff(cmmHits, jemHits, nThresh, thrLen);
    m_histTool->fillXVsThresholds(m_h_cmm_2d_thresh_ThreshJemEqCmm, loc, same,
                                                                 nThresh, 1);
    m_histTool->fillXVsThresholds(m_h_cmm_2d_thresh_ThreshJemNeCmm, loc, diff,
                                                                 nThresh, 1);
    if (forward) {
      const int shift = nThresh*thrLen;
      nThresh = 4;
      same = m_histTool->thresholdsSame(cmmHits>>shift, jemHits>>shift,
                                                             nThresh, thrLen);
      diff = m_histTool->thresholdsDiff(cmmHits>>shift, jemHits>>shift,
                                                             nThresh, thrLen);
      const int offset = (jem == 7 || jem == 15) ? 12 : 8;
      m_histTool->fillXVsThresholds(m_h_cmm_2d_thresh_ThreshJemEqCmm, loc, same,
                                                         nThresh, 1, offset);
      m_histTool->fillXVsThresholds(m_h_cmm_2d_thresh_ThreshJemNeCmm, loc, diff,
                                                         nThresh, 1, offset);
    }
  }
}

//  Compare Simulated CMM Hit Sums and Data CMM Hit Sums

void JEPSimBSMon::compare(const CmmJetHitsMap& cmmSimMap,
                          const CmmJetHitsMap& cmmMap,
                                ErrorVector& errors, int selection)
{
  if (m_debug) {
    msg(MSG::DEBUG) << "Compare Simulated CMM Hit Sums and Data CMM Hit Sums"
                    << endreq;
  }

  const bool local  = (selection == LVL1::CMMJetHits::LOCAL_MAIN);
  const bool remote = (selection == LVL1::CMMJetHits::REMOTE_MAIN);
  const bool total  = (selection == LVL1::CMMJetHits::TOTAL_MAIN);
  const bool etmap  = (selection == LVL1::CMMJetHits::ET_MAP);
  if (!local && !remote && !total && !etmap) return;
  unsigned int hitsSimMain = 0;
  unsigned int hitsSimFwd  = 0;
  unsigned int hitsDatMain = 0;
  unsigned int hitsDatFwd  = 0;
  CmmJetHitsMap::const_iterator cmmSimMapIter    = cmmSimMap.begin();
  CmmJetHitsMap::const_iterator cmmSimMapIterEnd = cmmSimMap.end();
  CmmJetHitsMap::const_iterator cmmMapIter       = cmmMap.begin();
  CmmJetHitsMap::const_iterator cmmMapIterEnd    = cmmMap.end();

  while (cmmSimMapIter != cmmSimMapIterEnd || cmmMapIter != cmmMapIterEnd) {

    int cmmSimKey = 0;
    int cmmKey    = 0;
    unsigned int cmmSimHits = 0;
    unsigned int cmmHits = 0;
    int crate  = 0;
    int dataId = 0;

    if (cmmSimMapIter != cmmSimMapIterEnd) cmmSimKey = cmmSimMapIter->first;
    if (cmmMapIter    != cmmMapIterEnd)    cmmKey    = cmmMapIter->first;

    if ((cmmMapIter == cmmMapIterEnd) ||
            ((cmmSimMapIter != cmmSimMapIterEnd) && (cmmKey > cmmSimKey))) {

      // Sim CMM Hits but no Data CMM Hits

      const LVL1::CMMJetHits* cmmS = cmmSimMapIter->second;
      ++cmmSimMapIter;
      dataId = cmmS->dataID();
      if (local  && dataId != LVL1::CMMJetHits::LOCAL_MAIN &&
                    dataId != LVL1::CMMJetHits::LOCAL_FORWARD) continue;
      if (remote && dataId != LVL1::CMMJetHits::LOCAL_MAIN &&
                    dataId != LVL1::CMMJetHits::LOCAL_FORWARD) continue;
      if (total  && dataId != LVL1::CMMJetHits::TOTAL_MAIN &&
                    dataId != LVL1::CMMJetHits::TOTAL_FORWARD) continue;
      if (etmap  && dataId != LVL1::CMMJetHits::ET_MAP)        continue;
      cmmSimHits = cmmS->Hits();
      crate      = cmmS->crate();

    } else if ((cmmSimMapIter == cmmSimMapIterEnd) ||
                 ((cmmMapIter != cmmMapIterEnd) && (cmmSimKey > cmmKey))) {

      // Data CMM Hits but no Sim CMM Hits

      const LVL1::CMMJetHits* cmmD = cmmMapIter->second;
      ++cmmMapIter;
      dataId   = cmmD->dataID();
      if (local  && dataId != LVL1::CMMJetHits::LOCAL_MAIN &&
                    dataId != LVL1::CMMJetHits::LOCAL_FORWARD)  continue;
      if (remote && dataId != LVL1::CMMJetHits::REMOTE_MAIN &&
		    dataId != LVL1::CMMJetHits::REMOTE_FORWARD) continue;
      if (total  && dataId != LVL1::CMMJetHits::TOTAL_MAIN &&
                    dataId != LVL1::CMMJetHits::TOTAL_FORWARD)  continue;
      if (etmap  && dataId != LVL1::CMMJetHits::ET_MAP)         continue;
      cmmHits = cmmD->Hits();
      crate   = cmmD->crate();

    } else {

      // Have both

      const LVL1::CMMJetHits* cmmS = cmmSimMapIter->second;
      const LVL1::CMMJetHits* cmmD = cmmMapIter->second;
      ++cmmSimMapIter;
      ++cmmMapIter;
      dataId   = cmmS->dataID();
      if (local  && dataId != LVL1::CMMJetHits::LOCAL_MAIN    &&
                    dataId != LVL1::CMMJetHits::LOCAL_FORWARD)  continue;
      if (remote && dataId != LVL1::CMMJetHits::LOCAL_MAIN    &&
                    dataId != LVL1::CMMJetHits::LOCAL_FORWARD &&
                    dataId != LVL1::CMMJetHits::REMOTE_MAIN   &&
		    dataId != LVL1::CMMJetHits::REMOTE_FORWARD) continue;
      if (total  && dataId != LVL1::CMMJetHits::TOTAL_MAIN    &&
                    dataId != LVL1::CMMJetHits::TOTAL_FORWARD)  continue;
      if (etmap  && dataId != LVL1::CMMJetHits::ET_MAP)         continue;
      cmmSimHits = cmmS->Hits();
      cmmHits    = cmmD->Hits();
      crate      = cmmS->crate();
    }

    if (!cmmSimHits && !cmmHits) continue;
    
    //  Fill in error plots

    if (local || total || etmap) {
      const int loc = crate * 2 + 1;
      const int cmmBins = 2 * 2;
      const int bit = (local) ? (1 << LocalJetMismatch)
                              : (total) ? (1 << TotalJetMismatch)
			                : (1 << JetEtMismatch);
      TH1F_LW* hist = 0;
      if (cmmSimHits == cmmHits) {
        errors[loc] |= bit;
	hist = m_h_cmm_1d_thresh_SumsSimEqData;
      } else {
        errors[loc+cmmBins] |= bit;
	if (cmmSimHits && cmmHits) hist = m_h_cmm_1d_thresh_SumsSimNeData;
	else if (!cmmHits)         hist = m_h_cmm_1d_thresh_SumsSimNoData;
	else                       hist = m_h_cmm_1d_thresh_SumsDataNoSim;
      }
      const int loc1 = (total) ? 3 : (etmap) ? 4 : crate;
      if (hist) hist->Fill(loc1);

      int nThresh = 8;
      int thrLen  = 3;
      int offset  = 0;
      if (dataId == LVL1::CMMJetHits::LOCAL_FORWARD ||
          dataId == LVL1::CMMJetHits::TOTAL_FORWARD) {
        thrLen  = 2;
	offset  = 8;
      } else if (dataId == LVL1::CMMJetHits::ET_MAP) {
        nThresh = 4;
	thrLen  = 1;
	offset  = 16;
      }
      const int same = m_histTool->thresholdsSame(cmmHits, cmmSimHits, nThresh,
                                                                       thrLen);
      const int diff = m_histTool->thresholdsDiff(cmmHits, cmmSimHits, nThresh,
                                                                       thrLen);
      m_histTool->fillXVsThresholds(m_h_cmm_2d_thresh_SumsThreshSimEqData,
                                               loc1, same, nThresh, 1, offset);
      m_histTool->fillXVsThresholds(m_h_cmm_2d_thresh_SumsThreshSimNeData,
                                               loc1, diff, nThresh, 1, offset);
    } else {
      if (dataId == LVL1::CMMJetHits::LOCAL_MAIN) {
        if (crate == 0) hitsSimMain = cmmSimHits;
      } else if (dataId == LVL1::CMMJetHits::LOCAL_FORWARD) {
        if (crate == 0) hitsSimFwd  = cmmSimHits;
      } else if (dataId == LVL1::CMMJetHits::REMOTE_MAIN) {
                        hitsDatMain = cmmHits;
      } else            hitsDatFwd  = cmmHits;
    }
  }
  if (remote) {
    const int crate = 1;
    const int loc = crate * 2 + 1;
    const int cmmBins = 2 * 2;
    const int bit = (1 << RemoteJetMismatch);

    TH1F_LW* hist = 0;
    if (hitsSimMain && hitsSimMain == hitsDatMain) {
      errors[loc] |= bit;
      hist = m_h_cmm_1d_thresh_SumsSimEqData;
    } else if (hitsSimMain != hitsDatMain) {
      errors[loc+cmmBins] |= bit;
      if (hitsSimMain && hitsDatMain) hist = m_h_cmm_1d_thresh_SumsSimNeData;
      else if (!hitsDatMain)          hist = m_h_cmm_1d_thresh_SumsSimNoData;
      else                            hist = m_h_cmm_1d_thresh_SumsDataNoSim;
    }
    const int loc1 = 2;
    if (hist) hist->Fill(loc1);
    hist = 0;
    if (hitsSimFwd && hitsSimFwd == hitsDatFwd) {
      errors[loc] |= bit;
      hist = m_h_cmm_1d_thresh_SumsSimEqData;
    } else if (hitsSimFwd != hitsDatFwd) {
      errors[loc+cmmBins] |= bit;
      if (hitsSimFwd && hitsDatFwd) hist = m_h_cmm_1d_thresh_SumsSimNeData;
      else if (!hitsDatFwd)         hist = m_h_cmm_1d_thresh_SumsSimNoData;
      else                          hist = m_h_cmm_1d_thresh_SumsDataNoSim;
    }
    if (hist) hist->Fill(loc1);

    const int nThresh = 8;
    const int mainLen = 3;
    const int fwdLen  = 2;
    const int offset  = 8;
    int same = m_histTool->thresholdsSame(hitsDatMain, hitsSimMain, nThresh,
                                                                    mainLen);
    int diff = m_histTool->thresholdsDiff(hitsDatMain, hitsSimMain, nThresh,
                                                                    mainLen);
    m_histTool->fillXVsThresholds(m_h_cmm_2d_thresh_SumsThreshSimEqData,
                                                      loc1, same, nThresh, 1);
    m_histTool->fillXVsThresholds(m_h_cmm_2d_thresh_SumsThreshSimNeData,
                                                      loc1, diff, nThresh, 1);
    same = m_histTool->thresholdsSame(hitsDatFwd, hitsSimFwd, nThresh, fwdLen);
    diff = m_histTool->thresholdsDiff(hitsDatFwd, hitsSimFwd, nThresh, fwdLen);
    m_histTool->fillXVsThresholds(m_h_cmm_2d_thresh_SumsThreshSimEqData,
                                               loc1, same, nThresh, 1, offset);
    m_histTool->fillXVsThresholds(m_h_cmm_2d_thresh_SumsThreshSimNeData,
                                               loc1, diff, nThresh, 1, offset);
  }
}

// Compare JetEt Map with JetEt RoI from data

void JEPSimBSMon::compare(const CmmJetHitsMap& cmmMap,
                          const LVL1::CMMRoI* cmmRoi,
                                ErrorVector& errors)
{
  if (m_debug) msg(MSG::DEBUG) << "Compare JetEt Map with JetEt RoI from data"
                               << endreq;

  int etMap = 0;
  int etRoi = 0;
  const int key = 100 + LVL1::CMMJetHits::ET_MAP;
  CmmJetHitsMap::const_iterator iter = cmmMap.find(key);
  if (iter != cmmMap.end()) {
    const LVL1::CMMJetHits* hits = iter->second;
    etMap = hits->Hits();
  }
  if (cmmRoi) etRoi = cmmRoi->jetEtHits();

  //  Check LimitedRoISet bit

  const int crate = 1;
  if ((!etMap && !etRoi) || (!etRoi && limitedRoiSet(crate))) return;
  
  const int loc = crate * 2 + 1;
  const int cmmBins = 2 * 2;
  const int bit = (1 << JetEtRoIMismatch);
  TH1F_LW* hist = 0;
  if (etMap == etRoi) {
    errors[loc] |= bit;
    hist = m_h_cmm_1d_thresh_SumsSimEqData;
  } else {
    errors[loc+cmmBins] |= bit;
    if (etMap && etRoi) hist = m_h_cmm_1d_thresh_SumsSimNeData;
    else if (!etRoi)    hist = m_h_cmm_1d_thresh_SumsSimNoData;
    else                hist = m_h_cmm_1d_thresh_SumsDataNoSim;
  }
  const int loc1 = 5;
  if (hist) hist->Fill(loc1);

  const int nThresh = 4;
  const int thrLen  = 1;
  const int offset  = 16;
  m_histTool->fillXVsThresholds(m_h_cmm_2d_thresh_SumsThreshSimEqData, loc1,
                                etRoi & etMap, nThresh, thrLen, offset);
  m_histTool->fillXVsThresholds(m_h_cmm_2d_thresh_SumsThreshSimNeData, loc1,
                                etRoi ^ etMap, nThresh, thrLen, offset);
}

//  Compare simulated JEM Et Sums with data

void JEPSimBSMon::compare(const JemEtSumsMap& jemSimMap,
                          const JemEtSumsMap& jemMap,
                                ErrorVector& errors)
{
  if (m_debug) msg(MSG::DEBUG) << "Compare simulated JEM Et Sums with data"
                               << endreq;

  JemEtSumsMap::const_iterator simMapIter    = jemSimMap.begin();
  JemEtSumsMap::const_iterator simMapIterEnd = jemSimMap.end();
  JemEtSumsMap::const_iterator datMapIter    = jemMap.begin();
  JemEtSumsMap::const_iterator datMapIterEnd = jemMap.end();

  while (simMapIter != simMapIterEnd || datMapIter != datMapIterEnd) {

    int simKey = 0;
    int datKey = 0;
    unsigned int simEt = 0;
    unsigned int simEx = 0;
    unsigned int simEy = 0;
    unsigned int datEt = 0;
    unsigned int datEx = 0;
    unsigned int datEy = 0;
    int crate = 0;
    int jem   = 0;

    if (simMapIter != simMapIterEnd) simKey = simMapIter->first;
    if (datMapIter != datMapIterEnd) datKey = datMapIter->first;

    if ((datMapIter == datMapIterEnd) ||
               ((simMapIter != simMapIterEnd) && (datKey > simKey))) {

      // Simulation EtSums but no data EtSums

      const LVL1::JEMEtSums* sime = simMapIter->second;
      simEt = sime->Et();
      simEx = sime->Ex();
      simEy = sime->Ey();
      crate = sime->crate();
      jem   = sime->module();
      ++simMapIter;

    } else if ((simMapIter == simMapIterEnd) ||
                 ((datMapIter != datMapIterEnd) && (simKey > datKey))) {

      // Data EtSums but no simulation EtSums

      const LVL1::JEMEtSums* date = datMapIter->second;
      datEt = date->Et();
      datEx = date->Ex();
      datEy = date->Ey();
      crate = date->crate();
      jem   = date->module();
      ++datMapIter;

    } else {

      // Have both

      const LVL1::JEMEtSums* sime = simMapIter->second;
      const LVL1::JEMEtSums* date = datMapIter->second;
      simEt = sime->Et();
      simEx = sime->Ex();
      simEy = sime->Ey();
      datEt = date->Et();
      datEx = date->Ex();
      datEy = date->Ey();
      crate = date->crate();
      jem   = date->module();
      ++simMapIter;
      ++datMapIter;
    }

    if (!simEt && !simEx && !simEy && !datEt && !datEx && !datEy) continue;
    
    //  Fill in error vector

    const int loc = crate * 16 + jem;
    const int jemBins = 2 * 16;
    const int bit = (1 << JEMEtSumsMismatch);
    if (simEt == datEt && simEx == datEx && simEy == datEy) errors[loc] |= bit;
    if (simEt != datEt || simEx != datEx || simEy != datEy)
                                                    errors[loc+jemBins] |= bit;
    if (m_debug && (simEt != datEt || simEx != datEx || simEy != datEy)) {
      msg(MSG::VERBOSE)
            << "EtSums Mismatch Crate/JEM, Data Et/Ex/Ey, Sim Et/Ex/Ey: "
            << crate << "/" << jem << ", "
            << datEt << "/" << datEx << "/" << datEy << ", "
	    << simEt << "/" << simEx << "/" << simEy << endreq;
    }
    TH2F_LW* hist = 0;
    if (simEx && simEx == datEx) hist = m_h_jem_2d_energy_SimEqData;
    else if (simEx != datEx) {
      if (simEx && datEx) hist = m_h_jem_2d_energy_SimNeData;
      else if (!datEx)    hist = m_h_jem_2d_energy_SimNoData;
      else                hist = m_h_jem_2d_energy_DataNoSim;
    }
    if (hist) hist->Fill(loc, 0);
    hist = 0;
    if (simEy && simEy == datEy) hist = m_h_jem_2d_energy_SimEqData;
    else if (simEy != datEy) {
      if (simEy && datEy) hist = m_h_jem_2d_energy_SimNeData;
      else if (!datEy)    hist = m_h_jem_2d_energy_SimNoData;
      else                hist = m_h_jem_2d_energy_DataNoSim;
    }
    if (hist) hist->Fill(loc, 1);
    hist = 0;
    if (simEt && simEt == datEt) hist = m_h_jem_2d_energy_SimEqData;
    else if (simEt != datEt) {
      if (simEt && datEt) hist = m_h_jem_2d_energy_SimNeData;
      else if (!datEt)    hist = m_h_jem_2d_energy_SimNoData;
      else                hist = m_h_jem_2d_energy_DataNoSim;
    }
    if (hist) hist->Fill(loc, 2);
  }
}

//  Compare JEM EtSums and CMM EtSums

void JEPSimBSMon::compare(const JemEtSumsMap& jemMap,
                          const CmmEtSumsMap& cmmMap,
                                ErrorVector& errorsJEM,
				ErrorVector& errorsCMM)
{
  if (m_debug) msg(MSG::DEBUG) << "Compare JEM EtSums and CMM EtSums" << endreq;

  JemEtSumsMap::const_iterator jemMapIter    = jemMap.begin();
  JemEtSumsMap::const_iterator jemMapIterEnd = jemMap.end();
  CmmEtSumsMap::const_iterator cmmMapIter    = cmmMap.begin();
  CmmEtSumsMap::const_iterator cmmMapIterEnd = cmmMap.end();

  while (jemMapIter != jemMapIterEnd || cmmMapIter != cmmMapIterEnd) {

    int jemKey = 0;
    int cmmKey = 0;
    unsigned int jemEt = 0;
    unsigned int jemEx = 0;
    unsigned int jemEy = 0;
    unsigned int cmmEt = 0;
    unsigned int cmmEx = 0;
    unsigned int cmmEy = 0;
    int crate  = 0;
    int jem    = 0;

    if (jemMapIter != jemMapIterEnd) jemKey = jemMapIter->first;
    if (cmmMapIter != cmmMapIterEnd) cmmKey = cmmMapIter->first;

    if ((cmmMapIter == cmmMapIterEnd) ||
                ((jemMapIter != jemMapIterEnd) && (cmmKey > jemKey))) {

      // JEM EtSums but no CMM EtSums

      const LVL1::JEMEtSums* jeme = jemMapIter->second;
      jemEt = jeme->Et();
      jemEx = jeme->Ex();
      jemEy = jeme->Ey();
      crate = jeme->crate();
      jem   = jeme->module();
      ++jemMapIter;

    } else if ((jemMapIter == jemMapIterEnd) ||
                ((cmmMapIter != cmmMapIterEnd) && (jemKey > cmmKey))) {

      // CMM EtSums but no JEM EtSums

      const LVL1::CMMEtSums* cmme = cmmMapIter->second;
      cmmEt = cmme->Et();
      cmmEx = cmme->Ex();
      cmmEy = cmme->Ey();
      crate = cmme->crate();
      jem   = cmme->dataID();
      ++cmmMapIter;
      if (jem > 15) continue;

    } else {

      // Have both

      const LVL1::JEMEtSums* jeme = jemMapIter->second;
      const LVL1::CMMEtSums* cmme = cmmMapIter->second;
      jemEt = jeme->Et();
      jemEx = jeme->Ex();
      jemEy = jeme->Ey();
      cmmEt = cmme->Et();
      cmmEx = cmme->Ex();
      cmmEy = cmme->Ey();
      crate = jeme->crate();
      jem   = jeme->module();
      ++jemMapIter;
      ++cmmMapIter;
    }

    if (!jemEt && !jemEx && !jemEy && !cmmEt && !cmmEx && !cmmEy) continue;
    
    //  Fill in error vectors

    const int loc  = crate * 16 + jem;
    const int loc2 = crate * 2; // CMM-Energy is left ==> cmm 0
    const int jemBins = 2 * 16;
    const int cmmBins = 2 * 2;
    const int bit = (1 << CMMEtSumsMismatch);
    if (jemEt == cmmEt && jemEx == cmmEx && jemEy == cmmEy) {
      errorsJEM[loc]  |= bit;
      errorsCMM[loc2] |= bit;
    } else {
      errorsJEM[loc+jemBins]  |= bit;
      errorsCMM[loc2+cmmBins] |= bit;
    }
    TH2F_LW* hist = 0;
    if (jemEx && jemEx == cmmEx) hist = m_h_cmm_2d_energy_JemEqCmm;
    else if (jemEx != cmmEx) {
      if (jemEx && cmmEx) hist = m_h_cmm_2d_energy_JemNeCmm;
      else if (!cmmEx)    hist = m_h_cmm_2d_energy_JemNoCmm;
      else                hist = m_h_cmm_2d_energy_CmmNoJem;
    }
    if (hist) hist->Fill(loc, 0);
    hist = 0;
    if (jemEy && jemEy == cmmEy) hist = m_h_cmm_2d_energy_JemEqCmm;
    else if (jemEy != cmmEy) {
      if (jemEy && cmmEy) hist = m_h_cmm_2d_energy_JemNeCmm;
      else if (!cmmEy)    hist = m_h_cmm_2d_energy_JemNoCmm;
      else                hist = m_h_cmm_2d_energy_CmmNoJem;
    }
    if (hist) hist->Fill(loc, 1);
    hist = 0;
    if (jemEt && jemEt == cmmEt) hist = m_h_cmm_2d_energy_JemEqCmm;
    else if (jemEt != cmmEt) {
      if (jemEt && cmmEt) hist = m_h_cmm_2d_energy_JemNeCmm;
      else if (!cmmEt)    hist = m_h_cmm_2d_energy_JemNoCmm;
      else                hist = m_h_cmm_2d_energy_CmmNoJem;
    }
    if (hist) hist->Fill(loc, 2);
  }
}

//  Compare Simulated CMM EtSums and Data CMM EtSums

void JEPSimBSMon::compare(const CmmEtSumsMap& cmmSimMap,
                          const CmmEtSumsMap& cmmMap,
                                ErrorVector& errors, int selection)
{
  if (m_debug) {
    msg(MSG::DEBUG) << "Compare Simulated CMM EtSums and Data CMM EtSums"
                    << endreq;
  }

  const bool local  = (selection == LVL1::CMMEtSums::LOCAL);
  const bool remote = (selection == LVL1::CMMEtSums::REMOTE);
  const bool total  = (selection == LVL1::CMMEtSums::TOTAL);
  const bool etmaps = (selection == LVL1::CMMEtSums::SUM_ET_MAP);
  if (!local && !remote && !total && !etmaps) return;
  unsigned int localEt  = 0;
  unsigned int localEx  = 0;
  unsigned int localEy  = 0;
  unsigned int remoteEt = 0;
  unsigned int remoteEx = 0;
  unsigned int remoteEy = 0;
  CmmEtSumsMap::const_iterator cmmSimMapIter    = cmmSimMap.begin();
  CmmEtSumsMap::const_iterator cmmSimMapIterEnd = cmmSimMap.end();
  CmmEtSumsMap::const_iterator cmmMapIter       = cmmMap.begin();
  CmmEtSumsMap::const_iterator cmmMapIterEnd    = cmmMap.end();

  while (cmmSimMapIter != cmmSimMapIterEnd || cmmMapIter != cmmMapIterEnd) {

    int cmmSimKey = 0;
    int cmmKey    = 0;
    unsigned int cmmSimEt = 0;
    unsigned int cmmSimEx = 0;
    unsigned int cmmSimEy = 0;
    unsigned int cmmEt = 0;
    unsigned int cmmEx = 0;
    unsigned int cmmEy = 0;
    int crate  = 0;
    int dataId = 0;

    if (cmmSimMapIter != cmmSimMapIterEnd) cmmSimKey = cmmSimMapIter->first;
    if (cmmMapIter    != cmmMapIterEnd)    cmmKey    = cmmMapIter->first;

    if ((cmmMapIter == cmmMapIterEnd) ||
          ((cmmSimMapIter != cmmSimMapIterEnd) && (cmmKey > cmmSimKey))) {

      // Sim CMM EtSums but no Data CMM EtSums

      const LVL1::CMMEtSums* cmmS = cmmSimMapIter->second;
      ++cmmSimMapIter;
      dataId = cmmS->dataID();
      if (local  && dataId != LVL1::CMMEtSums::LOCAL) continue;
      if (remote && dataId != LVL1::CMMEtSums::LOCAL) continue;
      if (total  && dataId != LVL1::CMMEtSums::TOTAL) continue;
      if (etmaps && dataId != LVL1::CMMEtSums::SUM_ET_MAP &&
                    dataId != LVL1::CMMEtSums::MISSING_ET_MAP &&
                    dataId != LVL1::CMMEtSums::MISSING_ET_SIG_MAP) continue;
      cmmSimEt = cmmS->Et();
      cmmSimEx = cmmS->Ex();
      cmmSimEy = cmmS->Ey();
      if (!etmaps) { // include overflow bit in test
        cmmSimEt |= ((cmmS->EtError() & 0x1) << 15);
        cmmSimEx |= ((cmmS->ExError() & 0x1) << 15);
        cmmSimEy |= ((cmmS->EyError() & 0x1) << 15);
      }
      crate    = cmmS->crate();

    } else if ((cmmSimMapIter == cmmSimMapIterEnd) ||
                ((cmmMapIter != cmmMapIterEnd) && (cmmSimKey > cmmKey))) {

      // Data CMM EtSums but no Sim CMM EtSums

      const LVL1::CMMEtSums* cmmD = cmmMapIter->second;
      ++cmmMapIter;
      dataId   = cmmD->dataID();
      if (local  && dataId != LVL1::CMMEtSums::LOCAL)  continue;
      if (remote && dataId != LVL1::CMMEtSums::REMOTE) continue;
      if (total  && dataId != LVL1::CMMEtSums::TOTAL)  continue;
      if (etmaps && dataId != LVL1::CMMEtSums::SUM_ET_MAP &&
                    dataId != LVL1::CMMEtSums::MISSING_ET_MAP &&
                    dataId != LVL1::CMMEtSums::MISSING_ET_SIG_MAP) continue;
      cmmEt = cmmD->Et();
      cmmEx = cmmD->Ex();
      cmmEy = cmmD->Ey();
      if (!etmaps) {
        cmmEt |= ((cmmD->EtError() & 0x1) << 15);
        cmmEx |= ((cmmD->ExError() & 0x1) << 15);
        cmmEy |= ((cmmD->EyError() & 0x1) << 15);
      }
      crate = cmmD->crate();

    } else {

      // Have both

      const LVL1::CMMEtSums* cmmS = cmmSimMapIter->second;
      const LVL1::CMMEtSums* cmmD = cmmMapIter->second;
      ++cmmSimMapIter;
      ++cmmMapIter;
      dataId   = cmmS->dataID();
      if (local  && dataId != LVL1::CMMEtSums::LOCAL)  continue;
      if (remote && dataId != LVL1::CMMEtSums::LOCAL &&
                    dataId != LVL1::CMMEtSums::REMOTE) continue;
      if (total  && dataId != LVL1::CMMEtSums::TOTAL)  continue;
      if (etmaps && dataId != LVL1::CMMEtSums::SUM_ET_MAP &&
                    dataId != LVL1::CMMEtSums::MISSING_ET_MAP &&
                    dataId != LVL1::CMMEtSums::MISSING_ET_SIG_MAP) continue;
      cmmSimEt = cmmS->Et();
      cmmSimEx = cmmS->Ex();
      cmmSimEy = cmmS->Ey();
      cmmEt    = cmmD->Et();
      cmmEx    = cmmD->Ex();
      cmmEy    = cmmD->Ey();
      if (!etmaps) {
        cmmSimEt |= ((cmmS->EtError() & 0x1) << 15);
        cmmSimEx |= ((cmmS->ExError() & 0x1) << 15);
        cmmSimEy |= ((cmmS->EyError() & 0x1) << 15);
        cmmEt    |= ((cmmD->EtError() & 0x1) << 15);
        cmmEx    |= ((cmmD->ExError() & 0x1) << 15);
        cmmEy    |= ((cmmD->EyError() & 0x1) << 15);
      }
      crate    = cmmS->crate();
    }

    // Simulation will set all map bits for saturation according
    // to new thresholds scheme
    if (cmmSimEt != cmmEt) {
      if (dataId == LVL1::CMMEtSums::SUM_ET_MAP) {
        if (cmmSimEt == 0xff && cmmEt == 0xf) {
	  if (!hasMissingEtSig()) {
	    cmmSimEt = 0xf;
	    cmmSimEx = 0xf;
	    cmmSimEy = 0xf;
          }
        }
      } else if (dataId == LVL1::CMMEtSums::MISSING_ET_SIG_MAP) {
        if (cmmSimEt == 0xff && cmmEt == 0) {
	  if (!hasMissingEtSig()) {
	    cmmSimEt = 0;
	    cmmSimEx = 0;
	    cmmSimEy = 0;
	  }
        }
      // Total Et set to max for overflow in new scheme
      } else if (dataId == LVL1::CMMEtSums::TOTAL) {
        if (cmmSimEt == 0xffff && (cmmEt>>15) == 0x1) {
	  if (!hasMissingEtSig()) cmmSimEt = cmmEt;
        }
      }
    }

    if (!cmmSimEt && !cmmSimEx && !cmmSimEy && !cmmEt && !cmmEx && !cmmEy)
                                                                     continue;
    
    //  Fill in error vectors

    if (local || total || etmaps) {
      const int loc = crate * 2;
      const int cmmBins = 2 * 2;
      const int bit = (local)
                        ? (1 << LocalEnergyMismatch)
                        : (total)
			    ? (1 << TotalEnergyMismatch)
			    : (dataId == LVL1::CMMEtSums::SUM_ET_MAP)
			       ? (1 << SumEtMismatch)
			       : (dataId == LVL1::CMMEtSums::MISSING_ET_MAP)
			          ? (1 << MissingEtMismatch)
			          : (1 << MissingEtSigMismatch);
      if (cmmSimEt == cmmEt && cmmSimEx == cmmEx && cmmSimEy == cmmEy) {
        errors[loc] |= bit;
      } else errors[loc+cmmBins] |= bit;
      const int loc1 = (local) ? crate : 3;
      if (local || total) {
        TH2F_LW* hist = 0;
	if (cmmSimEx && cmmSimEx == cmmEx) hist = m_h_cmm_2d_energy_SumsSimEqData;
	else if (cmmSimEx != cmmEx) {
	  if (cmmSimEx && cmmEx) hist = m_h_cmm_2d_energy_SumsSimNeData;
	  else if (!cmmEx)       hist = m_h_cmm_2d_energy_SumsSimNoData;
	  else                   hist = m_h_cmm_2d_energy_SumsDataNoSim;
        }
	if (hist) hist->Fill(loc1, 0);
        hist = 0;
	if (cmmSimEy && cmmSimEy == cmmEy) hist = m_h_cmm_2d_energy_SumsSimEqData;
	else if (cmmSimEy != cmmEy) {
	  if (cmmSimEy && cmmEy) hist = m_h_cmm_2d_energy_SumsSimNeData;
	  else if (!cmmEy)       hist = m_h_cmm_2d_energy_SumsSimNoData;
	  else                   hist = m_h_cmm_2d_energy_SumsDataNoSim;
        }
	if (hist) hist->Fill(loc1, 1);
        hist = 0;
	if (cmmSimEt && cmmSimEt == cmmEt) hist = m_h_cmm_2d_energy_SumsSimEqData;
	else if (cmmSimEt != cmmEt) {
	  if (cmmSimEt && cmmEt) hist = m_h_cmm_2d_energy_SumsSimNeData;
	  else if (!cmmEt)       hist = m_h_cmm_2d_energy_SumsSimNoData;
	  else                   hist = m_h_cmm_2d_energy_SumsDataNoSim;
        }
	if (hist) hist->Fill(loc1, 2);
      } else {
        const int loc2 = (dataId == LVL1::CMMEtSums::SUM_ET_MAP)
	                  ? 3 : (dataId == LVL1::CMMEtSums::MISSING_ET_MAP)
			      ? 4 : 5;
        TH2F_LW* hist = 0;
	if (cmmSimEt && cmmSimEt == cmmEt) hist = m_h_cmm_2d_energy_SumsSimEqData;
	else if (cmmSimEt != cmmEt) {
	  if (cmmSimEt && cmmEt) hist = m_h_cmm_2d_energy_SumsSimNeData;
	  else if (!cmmEt)       hist = m_h_cmm_2d_energy_SumsSimNoData;
	  else                   hist = m_h_cmm_2d_energy_SumsDataNoSim;
        }
	if (hist) hist->Fill(loc1, loc2);
	if (cmmSimEt || cmmEt) {
	  int loc3 = 0;
	  int nThresh = 8;
	  int offset  = 0;
	  if (dataId == LVL1::CMMEtSums::MISSING_ET_MAP) {
	    loc3 = 2;
	    offset = 8;
          }
	  if (dataId == LVL1::CMMEtSums::MISSING_ET_SIG_MAP) {
	    loc3 = 4;
	    offset = 16;
          }
	  m_histTool->fillXVsThresholds(m_h_cmm_2d_energy_EtMapsThreshSimEqData,
	                            loc3, cmmEt & cmmSimEt, nThresh, 1, offset);
	  m_histTool->fillXVsThresholds(m_h_cmm_2d_energy_EtMapsThreshSimNeData,
	                            loc3, cmmEt ^ cmmSimEt, nThresh, 1, offset);
        }
      }
    } else {
      if (dataId == LVL1::CMMEtSums::LOCAL) {
        if (crate == 0) {
	  localEt = cmmSimEt;
	  localEx = cmmSimEx;
	  localEy = cmmSimEy;
	}
      } else {
        remoteEt = cmmEt;
        remoteEx = cmmEx;
        remoteEy = cmmEy;
      }
    }
  }
  if (remote && (localEt || localEx || localEy ||
                 remoteEt || remoteEx || remoteEy)) {
    const int crate = 1;
    const int loc = crate * 2;
    const int cmmBins = 2 * 2;
    const int bit = (1 << RemoteEnergyMismatch);
    if (localEt == remoteEt && localEx == remoteEx && localEy == remoteEy) {
      errors[loc] |= bit;
    } else errors[loc+cmmBins] |= bit;
    TH2F_LW* hist = 0;
    if (localEx && localEx == remoteEx) hist = m_h_cmm_2d_energy_SumsSimEqData;
    else if (localEx != remoteEx) {
      if (localEx && remoteEx) hist = m_h_cmm_2d_energy_SumsSimNeData;
      else if (!remoteEx)      hist = m_h_cmm_2d_energy_SumsSimNoData;
      else                     hist = m_h_cmm_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(2, 0);
    hist = 0;
    if (localEy && localEy == remoteEy) hist = m_h_cmm_2d_energy_SumsSimEqData;
    else if (localEy != remoteEy) {
      if (localEy && remoteEy) hist = m_h_cmm_2d_energy_SumsSimNeData;
      else if (!remoteEy)      hist = m_h_cmm_2d_energy_SumsSimNoData;
      else                     hist = m_h_cmm_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(2, 1);
    hist = 0;
    if (localEt && localEt == remoteEt) hist = m_h_cmm_2d_energy_SumsSimEqData;
    else if (localEt != remoteEt) {
      if (localEt && remoteEt) hist = m_h_cmm_2d_energy_SumsSimNeData;
      else if (!remoteEt)      hist = m_h_cmm_2d_energy_SumsSimNoData;
      else                     hist = m_h_cmm_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(2, 2);
  }
}

// Compare Et Maps and Energy Totals with Energy RoIs from data

void JEPSimBSMon::compare(const CmmEtSumsMap& cmmMap,
                          const LVL1::CMMRoI* cmmRoi,
                                ErrorVector& errors)
{
  if (m_debug) {
    msg(MSG::DEBUG) << "Compare Et Maps and Energy Totals with RoIs from data"
                    << endreq;
  }

  int sumEtMap = 0;
  int missEtMap = 0;
  int missEtSigMap = 0;
  int et = 0;
  int ex = 0;
  int ey = 0;
  int sumEtRoi = 0;
  int missEtRoi = 0;
  int missEtSigRoi = 0;
  int etRoi = 0;
  int exRoi = 0;
  int eyRoi = 0;
  int key = 100 + LVL1::CMMEtSums::SUM_ET_MAP;
  CmmEtSumsMap::const_iterator iter = cmmMap.find(key);
  if (iter != cmmMap.end()) {
    const LVL1::CMMEtSums* sums = iter->second;
    sumEtMap = sums->Et();
  }
  key = 100 + LVL1::CMMEtSums::MISSING_ET_MAP;
  iter = cmmMap.find(key);
  if (iter != cmmMap.end()) {
    const LVL1::CMMEtSums* sums = iter->second;
    missEtMap = sums->Et();
  }
  key = 100 + LVL1::CMMEtSums::MISSING_ET_SIG_MAP;
  iter = cmmMap.find(key);
  if (iter != cmmMap.end()) {
    const LVL1::CMMEtSums* sums = iter->second;
    missEtSigMap = sums->Et();
  }
  key = 100 + LVL1::CMMEtSums::TOTAL;
  iter = cmmMap.find(key);
  if (iter != cmmMap.end()) {
    const LVL1::CMMEtSums* sums = iter->second;
    et = (sums->Et() | ((sums->EtError() & 0x1) << 15));
    ex = (sums->Ex() | ((sums->ExError() & 0x1) << 15));
    ey = (sums->Ey() | ((sums->EyError() & 0x1) << 15));
  }
  if (cmmRoi) {
    sumEtRoi = cmmRoi->sumEtHits();
    missEtRoi = cmmRoi->missingEtHits();
    missEtSigRoi = cmmRoi->missingEtSigHits();
    etRoi = (cmmRoi->et() | ((cmmRoi->etError() & 0x1) << 15));
    exRoi = (cmmRoi->ex() | ((cmmRoi->exError() & 0x1) << 15));
    eyRoi = (cmmRoi->ey() | ((cmmRoi->eyError() & 0x1) << 15));
  }
  if (sumEtMap || sumEtRoi || missEtMap || missEtRoi ||
      missEtSigMap || missEtSigRoi ||
      et || etRoi || ex || exRoi || ey || eyRoi) {
    const int crate = 1;
    const int loc = crate * 2;
    const int cmmBins = 2 * 2;
    const int bit = (1 << EnergyRoIMismatch);
    if (sumEtMap == sumEtRoi && missEtMap == missEtRoi &&
        missEtSigMap == missEtSigRoi &&
        et == etRoi && ex == exRoi && ey == eyRoi) errors[loc] |= bit;
    else errors[loc+cmmBins] |= bit;
    TH2F_LW* hist = 0;
    if (ex && ex == exRoi)         hist = m_h_cmm_2d_energy_SumsSimEqData;
    else if (ex != exRoi) {
      if (ex && exRoi)             hist = m_h_cmm_2d_energy_SumsSimNeData;
      else if (!exRoi) {
        if (!limitedRoiSet(crate)) hist = m_h_cmm_2d_energy_SumsSimNoData;
      } else                       hist = m_h_cmm_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(4, 0);
    hist = 0;
    if (ey && ey == eyRoi)         hist = m_h_cmm_2d_energy_SumsSimEqData;
    else if (ey != eyRoi) {
      if (ey && eyRoi)             hist = m_h_cmm_2d_energy_SumsSimNeData;
      else if (!eyRoi) {
        if (!limitedRoiSet(crate)) hist = m_h_cmm_2d_energy_SumsSimNoData;
      } else                       hist = m_h_cmm_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(4, 1);
    hist = 0;
    if (et && et == etRoi)         hist = m_h_cmm_2d_energy_SumsSimEqData;
    else if (et != etRoi) {
      if (et && etRoi)             hist = m_h_cmm_2d_energy_SumsSimNeData;
      else if (!etRoi) {
        if (!limitedRoiSet(crate)) hist = m_h_cmm_2d_energy_SumsSimNoData;
      } else                       hist = m_h_cmm_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(4, 2);
    hist = 0;
    if (sumEtMap && sumEtMap == sumEtRoi) hist = m_h_cmm_2d_energy_SumsSimEqData;
    else if (sumEtMap != sumEtRoi) {
      if (sumEtMap && sumEtRoi)           hist = m_h_cmm_2d_energy_SumsSimNeData;
      else if (!sumEtRoi) {
        if (!limitedRoiSet(crate))        hist = m_h_cmm_2d_energy_SumsSimNoData;
      } else                              hist = m_h_cmm_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(4, 3);
    hist = 0;
    if (missEtMap && missEtMap == missEtRoi) hist = m_h_cmm_2d_energy_SumsSimEqData;
    else if (missEtMap != missEtRoi) {
      if (missEtMap && missEtRoi)            hist = m_h_cmm_2d_energy_SumsSimNeData;
      else if (!missEtRoi) {
        if (!limitedRoiSet(crate))           hist = m_h_cmm_2d_energy_SumsSimNoData;
      } else                                 hist = m_h_cmm_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(4, 4);
    hist = 0;
    if (missEtSigMap && missEtSigMap == missEtSigRoi) hist = m_h_cmm_2d_energy_SumsSimEqData;
    else if (missEtSigMap != missEtSigRoi) {
      if (missEtSigMap && missEtSigRoi)               hist = m_h_cmm_2d_energy_SumsSimNeData;
      else if (!missEtSigRoi) {
        if (!limitedRoiSet(crate))                    hist = m_h_cmm_2d_energy_SumsSimNoData;
      } else                                          hist = m_h_cmm_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(4, 5);

    const int thrLen = 1;
    const int nThresh = 8;
    if ((sumEtMap || sumEtRoi) && !(sumEtMap && !sumEtRoi 
                               && limitedRoiSet(crate))) {
      m_histTool->fillXVsThresholds(m_h_cmm_2d_energy_EtMapsThreshSimEqData, 1,
                                    sumEtRoi & sumEtMap, nThresh, thrLen);
      m_histTool->fillXVsThresholds(m_h_cmm_2d_energy_EtMapsThreshSimNeData, 1,
                                    sumEtRoi ^ sumEtMap, nThresh, thrLen);
    }
    if ((missEtMap || missEtRoi) && !(missEtMap && !missEtRoi
                                 && limitedRoiSet(crate))) {
      const int offset  = 8;
      m_histTool->fillXVsThresholds(m_h_cmm_2d_energy_EtMapsThreshSimEqData, 3,
                              missEtRoi & missEtMap,  nThresh, thrLen, offset);
      m_histTool->fillXVsThresholds(m_h_cmm_2d_energy_EtMapsThreshSimNeData, 3,
                              missEtRoi ^ missEtMap,  nThresh, thrLen, offset);
    }
    if ((missEtSigMap || missEtSigRoi) && !(missEtSigMap && !missEtSigRoi
                                       && limitedRoiSet(crate))) {
      const int offset  = 16;
      m_histTool->fillXVsThresholds(m_h_cmm_2d_energy_EtMapsThreshSimEqData, 5,
                              missEtSigRoi & missEtSigMap,  nThresh, thrLen,
			                                               offset);
      m_histTool->fillXVsThresholds(m_h_cmm_2d_energy_EtMapsThreshSimNeData, 5,
                              missEtSigRoi ^ missEtSigMap,  nThresh, thrLen,
			                                               offset);
    }
  }
}

void JEPSimBSMon::fillEventSample(int err, int loc, bool isJem)
{
  int hist = 0;
  int y    = 0;
  if (isJem) {
    hist = (err < 5) ? err : err - 5;
    y    = loc;
  } else {
    hist = (err < 10) ? 7 : 8;
    y = loc/2;
    if (err == LocalJetMismatch  || err == LocalEnergyMismatch)  y += 2;
    if (err == RemoteJetMismatch || err == RemoteEnergyMismatch) y = 4;
    if (err == TotalJetMismatch  || err == TotalEnergyMismatch)  y = 5;
    if (err == JetEtMismatch     || err == SumEtMismatch)        y = 6;
    if (err == JetEtRoIMismatch  || err == MissingEtMismatch)    y = 7;
    if (err == MissingEtSigMismatch)                             y = 8;
    if (err == EnergyRoIMismatch)                                y = 9;
  }
  if (m_v_2d_MismatchEvents[hist]) m_histTool->fillEventNumber(m_v_2d_MismatchEvents[hist], y);
}

void JEPSimBSMon::setLabels(LWHist* hist, bool xAxis)
{
  // Simulation steps in red (#color[2]) depend on Trigger Menu
  LWHist::LWHistAxis* axis = (xAxis) ? hist->GetXaxis() : hist->GetYaxis();
  axis->SetBinLabel(1+EMElementMismatch,    "EM je");
  axis->SetBinLabel(1+HadElementMismatch,   "Had je");
  axis->SetBinLabel(1+RoIMismatch,          "#color[2]{RoIs}");
  axis->SetBinLabel(1+JEMHitsMismatch,      "JEMHits");
  axis->SetBinLabel(1+CMMJetHitsMismatch,   "CMMHits");
  axis->SetBinLabel(1+LocalJetMismatch,     "Local");
  axis->SetBinLabel(1+RemoteJetMismatch,    "Remote");
  axis->SetBinLabel(1+TotalJetMismatch,     "Total");
  axis->SetBinLabel(1+JetEtMismatch,        "#color[2]{JetEt}");
  axis->SetBinLabel(1+JetEtRoIMismatch,     "JetEt RoI");
  axis->SetBinLabel(1+JEMEtSumsMismatch,    "JEMSums");
  axis->SetBinLabel(1+CMMEtSumsMismatch,    "CMMSums");
  axis->SetBinLabel(1+LocalEnergyMismatch,  "Local");
  axis->SetBinLabel(1+RemoteEnergyMismatch, "Remote");
  axis->SetBinLabel(1+TotalEnergyMismatch,  "Total");
  axis->SetBinLabel(1+SumEtMismatch,        "#color[2]{SumEt}");
  axis->SetBinLabel(1+MissingEtMismatch,    "#color[2]{MissingEt}");
  axis->SetBinLabel(1+MissingEtSigMismatch, "#color[2]{MissEtSig}");
  axis->SetBinLabel(1+EnergyRoIMismatch,    "Engy RoIs");
}

void JEPSimBSMon::setLabelsSH(LWHist* hist)
{
  LWHist::LWHistAxis* axis = hist->GetXaxis();
  axis->SetBinLabel(1, "Local0");
  axis->SetBinLabel(2, "Local1");
  axis->SetBinLabel(3, "Remote");
  axis->SetBinLabel(4, "Total");
  axis->SetBinLabel(5, "JetEt");
  axis->SetBinLabel(6, "JetEt RoI");
}

void JEPSimBSMon::setLabelsSHF(LWHist* hist)
{
  setLabelsSH(hist);
  m_histTool->jemThresholds(hist, 0, false);
  m_histTool->jetEtThresholds(hist, 16, false);
}

void JEPSimBSMon::setLabelsEnTot(LWHist* hist)
{
  LWHist::LWHistAxis* axis = hist->GetXaxis();
  axis->SetBinLabel(1, "Local0");
  axis->SetBinLabel(2, "Local1");
  axis->SetBinLabel(3, "Remote");
  axis->SetBinLabel(4, "Total");
  axis->SetBinLabel(5, "RoI");
  axis = hist->GetYaxis();
  axis->SetBinLabel(1, "Ex");
  axis->SetBinLabel(2, "Ey");
  axis->SetBinLabel(3, "Et");
  axis->SetBinLabel(4, "SumEt");
  axis->SetBinLabel(5, "MissingEt");
  axis->SetBinLabel(6, "MissEtSig");
}

void JEPSimBSMon::setLabelsEnTotThr(LWHist* hist)
{
  LWHist::LWHistAxis* axis = hist->GetXaxis();
  axis->SetBinLabel(1, "SumEt");
  axis->SetBinLabel(2, "SumEt RoI");
  axis->SetBinLabel(3, "MissingEt");
  axis->SetBinLabel(4, "MissingEt RoI");
  axis->SetBinLabel(5, "MissingEtSig");
  axis->SetBinLabel(6, "MissingEtSig RoI");
  m_histTool->sumEtThresholds(hist, 0, false);
  m_histTool->missingEtThresholds(hist, 8, false);
  m_histTool->missingEtSigThresholds(hist, 16, false);
}

void JEPSimBSMon::setupMap(const JetElementCollection* coll,
                                 JetElementMap& map)
{
  if (coll) m_jetElementTool->mapJetElements(coll, &map);
}

void JEPSimBSMon::setupMap(const JemRoiCollection* coll, JemRoiMap& map)
{
  if (coll) {
    JemRoiCollection::const_iterator pos  = coll->begin();
    JemRoiCollection::const_iterator posE = coll->end();
    for (; pos != posE; ++pos) {
      const int crate = (*pos)->crate();
      const int jem   = (*pos)->jem();
      const int frame = (*pos)->frame();
      const int loc   = (*pos)->location();
      const int fwd   = (*pos)->forward();
      const int key   =
             (((((((crate << 4) | jem) << 3) | frame) << 3) | loc) << 1) | fwd;
      map.insert(std::make_pair(key, *pos));
    }
  }
}

void JEPSimBSMon::setupMap(const JemHitsCollection* coll, JemHitsMap& map)
{
  if (coll) {
    JemHitsCollection::const_iterator pos  = coll->begin();
    JemHitsCollection::const_iterator posE = coll->end();
    for (; pos != posE; ++pos) {
      const int crate = (*pos)->crate();
      const int jem   = (*pos)->module();
      const int key   = crate * 100 + jem;
      map.insert(std::make_pair(key, *pos));
    }
  }
}

void JEPSimBSMon::setupMap(const CmmJetHitsCollection* coll, CmmJetHitsMap& map)
{
  if (coll) {
    CmmJetHitsCollection::const_iterator pos  = coll->begin();
    CmmJetHitsCollection::const_iterator posE = coll->end();
    for (; pos != posE; ++pos) {
      const int crate  = (*pos)->crate();
      const int dataId = (*pos)->dataID();
      const int key  = crate * 100 + dataId;
      map.insert(std::make_pair(key, *pos));
    }
  }
}

void JEPSimBSMon::setupMap(const JemEtSumsCollection* coll, JemEtSumsMap& map)
{
  if (coll) {
    JemEtSumsCollection::const_iterator pos  = coll->begin();
    JemEtSumsCollection::const_iterator posE = coll->end();
    for (; pos != posE; ++pos) {
      const int crate = (*pos)->crate();
      const int jem   = (*pos)->module();
      const int key   = crate * 100 + jem;
      map.insert(std::make_pair(key, *pos));
    }
  }
}

void JEPSimBSMon::setupMap(const CmmEtSumsCollection* coll, CmmEtSumsMap& map)
{
  if (coll) {
    CmmEtSumsCollection::const_iterator pos  = coll->begin();
    CmmEtSumsCollection::const_iterator posE = coll->end();
    for (; pos != posE; ++pos) {
      const int crate  = (*pos)->crate();
      const int dataId = (*pos)->dataID();
      const int key  = crate * 100 + dataId;
      map.insert(std::make_pair(key, *pos));
    }
  }
}

void JEPSimBSMon::simulate(const TriggerTowerCollection* towers,
                                 JetElementCollection* elements)
{
  if (m_debug) {
    msg(MSG::DEBUG) << "Simulate Jet Elements from Trigger Towers" << endreq;
  }

  // Make zero-suppressed collection to speed up simulation

  TriggerTowerCollection* towersZ =
                              new TriggerTowerCollection(SG::VIEW_ELEMENTS);
  TriggerTowerCollection::const_iterator pos  = towers->begin();
  TriggerTowerCollection::const_iterator posE = towers->end();
  for (; pos != posE; ++pos) {
    if ((*pos)->emEnergy() > 0 || (*pos)->hadEnergy() > 0) {
      towersZ->push_back(*pos);
    }
  }
  m_jetElementTool->makeJetElements(towersZ, elements);
  delete towersZ;
}

void JEPSimBSMon::simulate(const JetElementCollection* elements,
                           const JetElementCollection* elementsOv,
                                 JemRoiCollection* rois)
{
  if (m_debug) {
    msg(MSG::DEBUG) << "Simulate JEM RoIs from Jet Elements" << endreq;
  }

  // Process a crate at a time to use overlap data
  const int ncrates = 2;
  std::vector<JetElementCollection*> crateColl;
  for (int crate = 0; crate < ncrates; ++crate) {
    crateColl.push_back(new JetElementCollection(SG::VIEW_ELEMENTS));
  }
  LVL1::CoordToHardware converter;
  JetElementCollection::const_iterator iter;
  JetElementCollection::const_iterator iterE;
  if (elements) {  // core data
    iter  = elements->begin();
    iterE = elements->end();
    for (; iter != iterE; ++iter) {
      LVL1::JetElement* je = *iter;
      const LVL1::Coordinate coord(je->phi(), je->eta());
      const int crate = converter.jepCrate(coord);
      if (crate < ncrates) crateColl[crate]->push_back(je);
    }
  }
  if (elementsOv) {  // overlap data
    iter  = elementsOv->begin();
    iterE = elementsOv->end();
    for (; iter != iterE; ++iter) {
      LVL1::JetElement* je = *iter;
      const LVL1::Coordinate coord(je->phi(), je->eta());
      const int crate = converter.jepCrateOverlap(coord);
      if (crate < ncrates) crateColl[crate]->push_back(je);
    }
  } else if (elements) {  // take overlap from core
    iter  = elements->begin();
    iterE = elements->end();
    for (; iter != iterE; ++iter) {
      LVL1::JetElement* je = *iter;
      const LVL1::Coordinate coord(je->phi(), je->eta());
      const int crate = converter.jepCrateOverlap(coord);
      if (crate < ncrates) crateColl[crate]->push_back(je);
    }
  }
  for (int crate = 0; crate < ncrates; ++crate) {
    InternalRoiCollection* intRois = new InternalRoiCollection;
    m_jetTool->findRoIs(crateColl[crate], intRois);
    InternalRoiCollection::iterator roiIter  = intRois->begin();
    InternalRoiCollection::iterator roiIterE = intRois->end();
    for (; roiIter != roiIterE; ++roiIter) {
      LVL1::JEMRoI* roi = new LVL1::JEMRoI((*roiIter)->RoIWord());
      if (roi->crate() == crate) rois->push_back(roi);
      else delete roi;
    }
    delete intRois;
    delete crateColl[crate];
  }
}

// Quicker version when core and overlap the same

void JEPSimBSMon::simulate(const JetElementCollection* elements,
                                 JemRoiCollection* rois)
{
  if (m_debug) {
    msg(MSG::DEBUG) << "Simulate JEM RoIs from Jet Elements" << endreq;
  }

  InternalRoiCollection* intRois = new InternalRoiCollection;
  m_jetTool->findRoIs(elements, intRois);
  InternalRoiCollection::iterator roiIter  = intRois->begin();
  InternalRoiCollection::iterator roiIterE = intRois->end();
  for (; roiIter != roiIterE; ++roiIter) {
    LVL1::JEMRoI* roi = new LVL1::JEMRoI((*roiIter)->RoIWord());
    rois->push_back(roi);
  }
  delete intRois;
}

void JEPSimBSMon::simulate(const JemRoiCollection* rois,
                                 JemHitsCollection* hits)
{
  if (m_debug) msg(MSG::DEBUG) << "Simulate JEM Hits from JEM RoIs" << endreq;

  m_jepHitsTool->formJEMHits(rois, hits);
}

void JEPSimBSMon::simulate(const CmmJetHitsCollection* hitsIn,
                                 CmmJetHitsCollection* hitsOut,
				 int selection)
{
  if (m_debug) msg(MSG::DEBUG) << "Simulate CMM-Jet Hit sums from CMM-Jet Hits"
                               << endreq;

  if (selection == LVL1::CMMJetHits::LOCAL_MAIN) {
    m_jepHitsTool->formCMMJetHitsCrate(hitsIn, hitsOut);
  } else if (selection == LVL1::CMMJetHits::TOTAL_MAIN) {
    m_jepHitsTool->formCMMJetHitsSystem(hitsIn, hitsOut);
  } else if (selection == LVL1::CMMJetHits::ET_MAP) {
    m_jepHitsTool->formCMMJetHitsEtMap(hitsIn, hitsOut);
  }
}

void JEPSimBSMon::simulate(const JetElementCollection* elements,
                                 JemEtSumsCollection* sums)
{
  if (m_debug) msg(MSG::DEBUG) << "Simulate JEM EtSums from JetElements"
                               << endreq;

  m_etSumsTool->formJEMEtSums(elements, sums);
}

void JEPSimBSMon::simulate(const CmmEtSumsCollection* sumsIn,
                                 CmmEtSumsCollection* sumsOut,
				 int selection)
{
  if (m_debug) {
    msg(MSG::DEBUG) << "Simulate CMM-Energy Total sums from CMM-Energy Sums"
                    << endreq;
  }

  if (selection == LVL1::CMMEtSums::LOCAL) {
    m_etSumsTool->formCMMEtSumsCrate(sumsIn, sumsOut);
  } else if (selection == LVL1::CMMEtSums::TOTAL) {
    m_etSumsTool->formCMMEtSumsSystem(sumsIn, sumsOut);
  } else if (selection == LVL1::CMMEtSums::SUM_ET_MAP) {
    m_etSumsTool->formCMMEtSumsEtMaps(sumsIn, sumsOut);
  }
}

// Check if LimitedRoISet bit set

bool JEPSimBSMon::limitedRoiSet(int crate)
{
  if (m_rodTES) loadRodHeaders();
  return (((m_limitedRoi>>crate)&0x1) == 1);
}

// Return true if version with Missing-Et-Sig

bool JEPSimBSMon::hasMissingEtSig()
{
  if (m_rodTES) loadRodHeaders();
  return m_versionSig;
}

// Load ROD Headers

void JEPSimBSMon::loadRodHeaders()
{
  if (m_rodTES) {
    m_limitedRoi = 0;
    m_versionSig = true;
    const int nCrates = 2;
    RodHeaderCollection::const_iterator rodIter  = m_rodTES->begin();
    RodHeaderCollection::const_iterator rodIterE = m_rodTES->end();
    for (; rodIter != rodIterE; ++rodIter) {
      const LVL1::RODHeader* rod = *rodIter;
      const int rodCrate = rod->crate() - 12;
      if (rodCrate >= 0 && rodCrate < nCrates && rod->dataType() == 1) {
        if (rod->limitedRoISet()) {
          m_limitedRoi |= (1<<rodCrate);
        }
	m_versionSig = (rod->minorVersion() >= 0x1003);
      }
    }
    m_rodTES = 0;
  }
}
