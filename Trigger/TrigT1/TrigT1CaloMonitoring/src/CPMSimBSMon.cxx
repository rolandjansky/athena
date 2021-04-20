/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     CPMSimBSMon.cxx
// PACKAGE:  TrigT1CaloMonitoring  
//
// AUTHOR:   Peter Faulkner
//           
//
// ********************************************************************

#include <utility>
#include <cmath>

#include "LWHists/LWHist.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH2I_LW.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "SGTools/StlVectorClids.h"

#include "AthenaMonitoring/AthenaMonManager.h"

#include "TrigT1CaloEvent/CMMCPHits.h"
#include "TrigT1CaloEvent/CPMHits.h"
#include "TrigT1CaloEvent/CPMTower.h"
#include "TrigT1CaloEvent/CPMRoI.h"
#include "TrigT1CaloEvent/RODHeader.h"
#include "TrigT1CaloEvent/TriggerTower.h"
#include "TrigT1CaloUtils/CoordToHardware.h"
#include "TrigT1CaloUtils/CPAlgorithm.h"
#include "TrigT1CaloUtils/DataError.h"
#include "TrigT1CaloUtils/TriggerTowerKey.h"
#include "TrigT1CaloToolInterfaces/IL1EmTauTools.h"
#include "TrigT1CaloToolInterfaces/IL1CPHitsTools.h"
#include "TrigT1Interfaces/Coordinate.h"
#include "TrigT1Interfaces/CoordinateRange.h"
#include "TrigT1Interfaces/CPRoIDecoder.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

#include "TrigT1CaloMonitoringTools/ITrigT1CaloMonErrorTool.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramToolV1.h"

#include "CPMSimBSMon.h"
// ============================================================================
namespace LVL1 {
// ============================================================================
CPMSimBSMon::CPMSimBSMon(const std::string & type, 
			 const std::string & name,
			 const IInterface* parent)
  : ManagedMonitorToolBase(type, name, parent),
    m_emTauTool("LVL1::L1EmTauTools/L1EmTauTools"),
    m_cpHitsTool("LVL1::L1CPHitsTools/L1CPHitsTools"),
    m_errorTool("LVL1::TrigT1CaloMonErrorToolV1/TrigT1CaloMonErrorToolV1"),
    m_histTool("LVL1::TrigT1CaloLWHistogramToolV1/TrigT1CaloLWHistogramToolV1"),
    m_debug(false), m_rodTES(0), m_overlapPresent(false), m_limitedRoi(0),
    m_histBooked(false),
    m_h_cpm_em_2d_etaPhi_tt_PpmEqCore(0),
    m_h_cpm_em_2d_etaPhi_tt_PpmNeCore(0),
    m_h_cpm_em_2d_etaPhi_tt_PpmNoCore(0),
    m_h_cpm_em_2d_etaPhi_tt_CoreNoPpm(0),
    m_h_cpm_had_2d_etaPhi_tt_PpmEqCore(0),
    m_h_cpm_had_2d_etaPhi_tt_PpmNeCore(0),
    m_h_cpm_had_2d_etaPhi_tt_PpmNoCore(0),
    m_h_cpm_had_2d_etaPhi_tt_CoreNoPpm(0),
    m_h_cpm_em_2d_etaPhi_tt_PpmEqOverlap(0),
    m_h_cpm_em_2d_etaPhi_tt_PpmNeOverlap(0),
    m_h_cpm_em_2d_etaPhi_tt_PpmNoOverlap(0),
    m_h_cpm_em_2d_etaPhi_tt_OverlapNoPpm(0),
    m_h_cpm_had_2d_etaPhi_tt_PpmEqOverlap(0),
    m_h_cpm_had_2d_etaPhi_tt_PpmNeOverlap(0),
    m_h_cpm_had_2d_etaPhi_tt_PpmNoOverlap(0),
    m_h_cpm_had_2d_etaPhi_tt_OverlapNoPpm(0),
    m_h_cpm_2d_tt_PpmEqCpmFpga(0),
    m_h_cpm_2d_tt_PpmNeCpmFpga(0),
    m_h_cpm_2d_tt_PpmNoCpmFpga(0),
    m_h_cpm_2d_tt_CpmNoPpmFpga(0),
    m_h_cpm_2d_roi_SimEqData(0),
    m_h_cpm_2d_roi_SimNeData(0),
    m_h_cpm_2d_roi_SimNoData(0),
    m_h_cpm_2d_roi_DataNoSim(0),
    m_h_cpm_2d_roi_ThreshSimEqData(0),
    m_h_cpm_2d_roi_ThreshSimNeData(0),
    m_h_cpm_2d_etaPhi_roi_SimEqData(0),
    m_h_cpm_2d_etaPhi_roi_SimNeData(0),
    m_h_cpm_2d_etaPhi_roi_SimNoData(0),
    m_h_cpm_2d_etaPhi_roi_DataNoSim(0),
    m_h_cpm_2d_thresh_SimEqData(0),
    m_h_cpm_2d_thresh_SimNeData(0),
    m_h_cpm_2d_thresh_SimNoData(0),
    m_h_cpm_2d_thresh_DataNoSim(0),
    m_h_cpm_2d_thresh_ThreshSimEqData(0),
    m_h_cpm_2d_thresh_ThreshSimNeData(0),
    m_h_cmm_2d_thresh_CpmEqCmm(0),
    m_h_cmm_2d_thresh_CpmNeCmm(0),
    m_h_cmm_2d_thresh_CpmNoCmm(0),
    m_h_cmm_2d_thresh_CmmNoCpm(0),
    m_h_cmm_2d_thresh_ThreshCpmEqCmm(0),
    m_h_cmm_2d_thresh_ThreshCpmNeCmm(0),
    m_h_cmm_1d_thresh_SumsSimEqData(0),
    m_h_cmm_1d_thresh_SumsSimNeData(0),
    m_h_cmm_1d_thresh_SumsSimNoData(0),
    m_h_cmm_1d_thresh_SumsDataNoSim(0),
    m_h_cmm_2d_thresh_SumsThreshSimEqData(0),
    m_h_cmm_2d_thresh_SumsThreshSimNeData(0),
    m_h_cpm_2d_SimEqDataOverview(0),
    m_h_cpm_2d_SimNeDataOverview(0),
    m_h_cpm_1d_SimNeDataSummary(0),
    m_v_2d_MismatchEvents(0)
/*---------------------------------------------------------*/
{
  declareProperty("EmTauTool", m_emTauTool);
  declareProperty("CPHitsTool", m_cpHitsTool);

  declareProperty("CPMTowerLocation",
                 m_cpmTowerLocation  = LVL1::TrigT1CaloDefs::CPMTowerLocation);
  declareProperty("CPMTowerLocationOverlap",
                 m_cpmTowerLocationOverlap  =
		           LVL1::TrigT1CaloDefs::CPMTowerLocation+"Overlap");
  declareProperty("CPMHitsLocation",
                 m_cpmHitsLocation   = LVL1::TrigT1CaloDefs::CPMHitsLocation);
  declareProperty("CMMCPHitsLocation",
                 m_cmmCpHitsLocation = LVL1::TrigT1CaloDefs::CMMCPHitsLocation);
  declareProperty("CPMRoILocation",
                 m_cpmRoiLocation    = LVL1::TrigT1CaloDefs::CPMRoILocation);
  declareProperty("TriggerTowerLocation",
                 m_triggerTowerLocation =
		                 LVL1::TrigT1CaloDefs::TriggerTowerLocation);
  declareProperty("RodHeaderLocation",
                 m_rodHeaderLocation = "RODHeaders");

  declareProperty("RootDirectory", m_rootDir = "L1Calo");
}

/*---------------------------------------------------------*/
CPMSimBSMon::~CPMSimBSMon()
/*---------------------------------------------------------*/
{
}

/*---------------------------------------------------------*/
StatusCode CPMSimBSMon::initialize()
/*---------------------------------------------------------*/
{
  msg(MSG::INFO) << "Initializing " << name() << endmsg;

  m_debug = msgLvl(MSG::DEBUG);

  StatusCode sc;

  sc = ManagedMonitorToolBase::initialize();
  if (sc.isFailure()) return sc;

  sc = m_emTauTool.retrieve();
  if( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool L1EmTauTools" << endmsg;
    return sc;
  }

  sc = m_cpHitsTool.retrieve();
  if( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool L1CPHitsTools" << endmsg;
    return sc;
  }

  sc = m_errorTool.retrieve();
  if( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloMonErrorTool"
                    << endmsg;
    return sc;
  }

  sc = m_histTool.retrieve();
  if( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloLWHistogramToolV1"
                    << endmsg;
    return sc;
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode CPMSimBSMon::finalize()
/*---------------------------------------------------------*/
{
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode CPMSimBSMon::bookHistogramsRecurrent()
/*---------------------------------------------------------*/
{
  msg(MSG::DEBUG) << "bookHistograms entered" << endmsg;

  if( m_environment == AthenaMonManager::online ) {
    // book histograms that are only made in the online environment...
  }
  	
  if( m_dataType == AthenaMonManager::cosmics ) {
    // book histograms that are only relevant for cosmics data...
  }

  //if ( newLumiBlock ) { }

  if ( newRunFlag() ) {

  MgmtAttr_t attr = ATTRIB_UNMANAGED;
  std::string dir1(m_rootDir + "/CPM/Errors/Transmission_Simulation");
  MonGroup monShift( this, dir1, run, attr );
  MonGroup monExpert( this, dir1, run, attr );
  MonGroup monCPMin( this, dir1 + "/PPM2CPMTowers", run, attr );
  MonGroup monRoIs( this, dir1 + "/Towers2RoIs", run, attr );
  MonGroup monCPMout( this, dir1 + "/RoIs2Hits", run, attr );
  MonGroup monEvent1( this, dir1 + "/MismatchEventNumbers", run,
                                      attr, "", "eventSample" );
  std::string dir2(m_rootDir + "/CPM_CMM/Errors/Transmission_Simulation");
  MonGroup monCMMin( this, dir2 + "/CPM2CMMHits", run, attr );
  MonGroup monCMMout( this, dir2 + "/Hits2Sums", run, attr );
  MonGroup monEvent2( this, dir2 + "/MismatchEventNumbers", run,
                                      attr, "", "eventSample" );

  // CPMTowers

  m_histTool->setMonGroup(&monCPMin);

  m_h_cpm_em_2d_etaPhi_tt_PpmEqCore = m_histTool->bookCPMEtaVsPhi(
    "cpm_em_2d_etaPhi_tt_PpmEqCore",
    "EM Core CPM Tower/PPM Tower Non-zero Matches");
  m_h_cpm_em_2d_etaPhi_tt_PpmNeCore = m_histTool->bookCPMEtaVsPhi(
    "cpm_em_2d_etaPhi_tt_PpmNeCore",
    "EM Core CPM Tower/PPM Tower Non-zero Mismatches");
  m_h_cpm_em_2d_etaPhi_tt_PpmNoCore = m_histTool->bookCPMEtaVsPhi(
    "cpm_em_2d_etaPhi_tt_PpmNoCore",
    "EM PPM Towers but no Core CPM Towers");
  m_h_cpm_em_2d_etaPhi_tt_CoreNoPpm = m_histTool->bookCPMEtaVsPhi(
    "cpm_em_2d_etaPhi_tt_CoreNoPpm",
    "EM Core CPM Towers but no PPM Towers");
  m_h_cpm_had_2d_etaPhi_tt_PpmEqCore = m_histTool->bookCPMEtaVsPhi(
    "cpm_had_2d_etaPhi_tt_PpmEqCore",
    "HAD Core CPM Tower/PPM Tower Non-zero Matches");
  m_h_cpm_had_2d_etaPhi_tt_PpmNeCore = m_histTool->bookCPMEtaVsPhi(
    "cpm_had_2d_etaPhi_tt_PpmNeCore",
    "HAD Core CPM Tower/PPM Tower Non-zero Mismatches");
  m_h_cpm_had_2d_etaPhi_tt_PpmNoCore = m_histTool->bookCPMEtaVsPhi(
    "cpm_had_2d_etaPhi_tt_PpmNoCore",
    "HAD PPM Towers but no Core CPM Towers");
  m_h_cpm_had_2d_etaPhi_tt_CoreNoPpm = m_histTool->bookCPMEtaVsPhi(
    "cpm_had_2d_etaPhi_tt_CoreNoPpm",
    "HAD Core CPM Towers but no PPM Towers");
  m_h_cpm_em_2d_etaPhi_tt_PpmEqOverlap = m_histTool->bookCPMEtaVsPhi(
    "cpm_em_2d_etaPhi_tt_PpmEqOverlap",
    "EM Overlap CPM Tower/PPM Tower Non-zero Matches");
  m_h_cpm_em_2d_etaPhi_tt_PpmNeOverlap = m_histTool->bookCPMEtaVsPhi(
    "cpm_em_2d_etaPhi_tt_PpmNeOverlap",
    "EM Overlap CPM Tower/PPM Tower Non-zero Mismatches");
  m_h_cpm_em_2d_etaPhi_tt_PpmNoOverlap = m_histTool->bookCPMEtaVsPhi(
    "cpm_em_2d_etaPhi_tt_PpmNoOverlap",
    "EM PPM Towers but no Overlap CPM Towers");
  m_h_cpm_em_2d_etaPhi_tt_OverlapNoPpm = m_histTool->bookCPMEtaVsPhi(
    "cpm_em_2d_etaPhi_tt_OverlapNoPpm",
    "EM Overlap CPM Towers but no PPM Towers");
  m_h_cpm_had_2d_etaPhi_tt_PpmEqOverlap = m_histTool->bookCPMEtaVsPhi(
    "cpm_had_2d_etaPhi_tt_PpmEqOverlap",
    "HAD Overlap CPM Tower/PPM Tower Non-zero Matches");
  m_h_cpm_had_2d_etaPhi_tt_PpmNeOverlap = m_histTool->bookCPMEtaVsPhi(
    "cpm_had_2d_etaPhi_tt_PpmNeOverlap",
    "HAD Overlap CPM Tower/PPM Tower Non-zero Mismatches");
  m_h_cpm_had_2d_etaPhi_tt_PpmNoOverlap = m_histTool->bookCPMEtaVsPhi(
    "cpm_had_2d_etaPhi_tt_PpmNoOverlap",
    "HAD PPM Towers but no Overlap CPM Towers");
  m_h_cpm_had_2d_etaPhi_tt_OverlapNoPpm = m_histTool->bookCPMEtaVsPhi(
    "cpm_had_2d_etaPhi_tt_OverlapNoPpm",
    "HAD Overlap CPM Towers but no PPM Towers");

  m_h_cpm_2d_tt_PpmEqCpmFpga = m_histTool->bookCPMCrateModuleVsFPGA(
    "cpm_2d_tt_PpmEqCpmFpga", "CPM Tower/PPM Tower Non-zero Matches by FPGA");
  m_h_cpm_2d_tt_PpmNeCpmFpga = m_histTool->bookCPMCrateModuleVsFPGA(
   "cpm_2d_tt_PpmNeCpmFpga", "CPM Tower/PPM Tower Non-zero Mismatches by FPGA");
  m_h_cpm_2d_tt_PpmNoCpmFpga = m_histTool->bookCPMCrateModuleVsFPGA(
    "cpm_2d_tt_PpmNoCpmFpga", "PPM Towers but no CPM Towers by FPGA");
  m_h_cpm_2d_tt_CpmNoPpmFpga = m_histTool->bookCPMCrateModuleVsFPGA(
    "cpm_2d_tt_CpmNoPpmFpga", "CPM Towers but no PPM Towers by FPGA");

  // RoIs

  m_histTool->setMonGroup(&monRoIs);

  m_h_cpm_2d_roi_SimEqData = m_histTool->bookCPMCrateModuleVsChipLocalCoord(
    "cpm_2d_roi_SimEqData", "CPM RoI Data/Simulation Non-zero Matches");
  m_h_cpm_2d_roi_SimNeData = m_histTool->bookCPMCrateModuleVsChipLocalCoord(
    "cpm_2d_roi_SimNeData", "CPM RoI Data/Simulation Non-zero Mismatches");
  m_h_cpm_2d_roi_SimNoData = m_histTool->bookCPMCrateModuleVsChipLocalCoord(
    "cpm_2d_roi_SimNoData", "CPM RoI Simulation but no Data");
  m_h_cpm_2d_roi_DataNoSim = m_histTool->bookCPMCrateModuleVsChipLocalCoord(
    "cpm_2d_roi_DataNoSim", "CPM RoI Data but no Simulation");

  m_h_cpm_2d_roi_ThreshSimEqData = m_histTool->bookCPMCrateModuleVsThreshold(
    "cpm_2d_roi_ThreshSimEqData", "CPM RoI Data/Simulation Threshold Matches");
  m_h_cpm_2d_roi_ThreshSimNeData = m_histTool->bookCPMCrateModuleVsThreshold(
    "cpm_2d_roi_ThreshSimNeData",
    "CPM RoI Data/Simulation Threshold Mismatches");

  m_h_cpm_2d_etaPhi_roi_SimEqData = m_histTool->bookCPMRoIEtaVsPhi(
    "cpm_2d_etaPhi_roi_SimEqData", "CPM RoI Data/Simulation Non-zero Matches");
  m_h_cpm_2d_etaPhi_roi_SimNeData = m_histTool->bookCPMRoIEtaVsPhi(
    "cpm_2d_etaPhi_roi_SimNeData",
    "CPM RoI Data/Simulation Non-zero Mismatches");
  m_h_cpm_2d_etaPhi_roi_SimNoData = m_histTool->bookCPMRoIEtaVsPhi(
    "cpm_2d_etaPhi_roi_SimNoData", "CPM RoI Simulation but no Data");
  m_h_cpm_2d_etaPhi_roi_DataNoSim = m_histTool->bookCPMRoIEtaVsPhi(
    "cpm_2d_etaPhi_roi_DataNoSim", "CPM RoI Data but no Simulation");

  // CPMHits

  m_histTool->setMonGroup(&monCPMout);

  m_h_cpm_2d_thresh_SimEqData = m_histTool->bookCPMModuleVsCrate(
    "cpm_2d_thresh_SimEqData", "CPM Hits Data/Simulation Non-zero Matches");
  m_h_cpm_2d_thresh_SimNeData = m_histTool->bookCPMModuleVsCrate(
    "cpm_2d_thresh_SimNeData", "CPM Hits Data/Simulation Non-zero Mismatches");
  m_h_cpm_2d_thresh_SimNoData = m_histTool->bookCPMModuleVsCrate(
    "cpm_2d_thresh_SimNoData", "CPM Hits Simulation but no Data");
  m_h_cpm_2d_thresh_DataNoSim = m_histTool->bookCPMModuleVsCrate(
    "cpm_2d_thresh_DataNoSim", "CPM Hits Data but no Simulation");

  m_h_cpm_2d_thresh_ThreshSimEqData = m_histTool->bookCPMCrateModuleVsThreshold(
    "cpm_2d_thresh_ThreshSimEqData",
    "CPM Hits Data/Simulation Threshold Matches");
  m_h_cpm_2d_thresh_ThreshSimNeData = m_histTool->bookCPMCrateModuleVsThreshold(
    "cpm_2d_thresh_ThreshSimNeData",
    "CPM Hits Data/Simulation Threshold Mismatches");

  // CMMHits

  m_histTool->setMonGroup(&monCMMin);

  m_h_cmm_2d_thresh_CpmEqCmm = m_histTool->bookCPMModuleVsCrateCMM(
    "cmm_2d_thresh_CpmEqCmm", "CMM Hits/CPM Hits Non-zero Matches");
  m_h_cmm_2d_thresh_CpmNeCmm = m_histTool->bookCPMModuleVsCrateCMM(
    "cmm_2d_thresh_CpmNeCmm", "CMM Hits/CPM Hits Non-zero Mismatches");
  m_h_cmm_2d_thresh_CpmNoCmm = m_histTool->bookCPMModuleVsCrateCMM(
    "cmm_2d_thresh_CpmNoCmm", "CPM Hits but no CMM Hits");
  m_h_cmm_2d_thresh_CmmNoCpm = m_histTool->bookCPMModuleVsCrateCMM(
    "cmm_2d_thresh_CmmNoCpm", "CMM Hits but no CPM Hits");

  m_h_cmm_2d_thresh_ThreshCpmEqCmm = m_histTool->bookCPMCrateModuleVsThreshold(
    "cmm_2d_thresh_ThreshCpmEqCmm", "CMM Hits/CPM Hits Threshold Matches");
  m_h_cmm_2d_thresh_ThreshCpmNeCmm = m_histTool->bookCPMCrateModuleVsThreshold(
    "cmm_2d_thresh_ThreshCpmNeCmm", "CMM Hits/CPM Hits Threshold Mismatches");

  m_histTool->setMonGroup(&monCMMout);

  // Local/Remote/Total sums

  m_h_cmm_1d_thresh_SumsSimEqData = m_histTool->bookCPMSumCMM(
    "cmm_1d_thresh_SumsSimEqData",
    "CMM Hit Sums Data/Simulation Non-zero Matches");
  m_h_cmm_1d_thresh_SumsSimNeData = m_histTool->bookCPMSumCMM(
    "cmm_1d_thresh_SumsSimNeData",
    "CMM Hit Sums Data/Simulation Non-zero Mismatches");
  m_h_cmm_1d_thresh_SumsSimNoData = m_histTool->bookCPMSumCMM(
    "cmm_1d_thresh_SumsSimNoData", "CMM Hit Sums Simulation but no Data");
  m_h_cmm_1d_thresh_SumsDataNoSim = m_histTool->bookCPMSumCMM(
    "cmm_1d_thresh_SumsDataNoSim", "CMM Hit Sums Data but no Simulation");
  m_h_cmm_2d_thresh_SumsThreshSimEqData = m_histTool->bookCPMSumVsThreshold(
    "cmm_2d_thresh_SumsThreshSimEqData",
    "CMM Hit Sums Data/Simulation Threshold Matches");
  m_h_cmm_2d_thresh_SumsThreshSimNeData = m_histTool->bookCPMSumVsThreshold(
    "cmm_2d_thresh_SumsThreshSimNeData",
    "CMM Hit Sums Data/Simulation Threshold Mismatches");

  // Summary

  m_histTool->setMonGroup(&monExpert);

  m_h_cpm_2d_SimEqDataOverview = m_histTool->book2F("cpm_2d_SimEqDataOverview",
   "CP Transmission/Comparison with Simulation Overview - Events with Matches",
             64, 0, 64, NumberOfSummaryBins, 0, NumberOfSummaryBins);
  m_histTool->cpmCMMCrateModule(m_h_cpm_2d_SimEqDataOverview);
  setLabels(m_h_cpm_2d_SimEqDataOverview, false);

  m_h_cpm_2d_SimNeDataOverview = m_histTool->book2F("cpm_2d_SimNeDataOverview",
   "CP Transmission/Comparison with Simulation Overview - Events with Mismatches",
             64, 0, 64, NumberOfSummaryBins, 0, NumberOfSummaryBins);
  m_histTool->cpmCMMCrateModule(m_h_cpm_2d_SimNeDataOverview);
  setLabels(m_h_cpm_2d_SimNeDataOverview, false);

  m_histTool->setMonGroup(&monShift);

  m_h_cpm_1d_SimNeDataSummary = m_histTool->book1F("cpm_1d_SimNeDataSummary",
   "CP Transmission/Comparison with Simulation Mismatch Summary;;Events",
    NumberOfSummaryBins, 0, NumberOfSummaryBins);
  setLabels(m_h_cpm_1d_SimNeDataSummary);

  // Mismatch Event Number Samples

  m_histTool->setMonGroup(&monEvent1);

  TH2I_LW* hist = 0;
  m_v_2d_MismatchEvents.clear();
  m_v_2d_MismatchEvents.resize(6, hist);
  hist = m_histTool->bookCPMEventVsCrateModule("cpm_em_2d_tt_MismatchEvents",
                                     "CPM Towers EM Mismatch Event Numbers");
  m_v_2d_MismatchEvents[0] = hist;
  hist = m_histTool->bookCPMEventVsCrateModule("cpm_had_2d_tt_MismatchEvents",
                                     "CPM Towers Had Mismatch Event Numbers");
  m_v_2d_MismatchEvents[1] = hist;
  hist = m_histTool->bookCPMEventVsCrateModule("cpm_2d_roi_MismatchEvents",
                                     "CPM RoIs Mismatch Event Numbers");
  m_v_2d_MismatchEvents[2] = hist;
  hist = m_histTool->bookCPMEventVsCrateModule("cpm_2d_thresh_MismatchEvents",
                                     "CPM Hits Mismatch Event Numbers");
  m_v_2d_MismatchEvents[3] = hist;

  m_histTool->setMonGroup(&monEvent2);

  hist = m_histTool->bookCPMEventVsCrateModule("cmm_2d_thresh_MismatchEvents",
                                     "CMM Hits Mismatch Event Numbers");
  m_v_2d_MismatchEvents[4] = hist;
  hist = m_histTool->bookEventNumbers("cmm_2d_thresh_SumsMismatchEvents",
                     "CMM Hit Sums Mismatch Event Numbers", 24, 0., 24.);
  m_histTool->cpmCrateCMM(hist, 0, false);
  m_histTool->cpmCrateCMM(hist, 8, false);
  m_histTool->cpmCrateCMM(hist, 16, false);
  hist->GetYaxis()->SetBinLabel(1, "Module 0/0");
  hist->GetYaxis()->SetBinLabel(9, "Local 0/0");
  hist->GetYaxis()->SetBinLabel(17, "Remote 0/0");
  hist->GetYaxis()->SetBinLabel(23, "Total 3/0");
  hist->GetYaxis()->SetTitle("");
  m_v_2d_MismatchEvents[5] = hist;

  m_histTool->unsetMonGroup();
  m_histBooked = true;

  } // end if (newRun ...

  msg(MSG::DEBUG) << "Leaving bookHistograms" << endmsg;
  
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode CPMSimBSMon::fillHistograms()
/*---------------------------------------------------------*/
{
  if (m_debug) msg(MSG::DEBUG) << "fillHistograms entered" << endmsg;

  if (!m_histBooked) {
    if (m_debug) msg(MSG::DEBUG) << "Histogram(s) not booked" << endmsg;
    return StatusCode::SUCCESS;
  }
  
  // Skip events believed to be corrupt or with ROB errors

  if (m_errorTool->corrupt() || m_errorTool->robOrUnpackingError()) {
    if (m_debug) msg(MSG::DEBUG) << "Skipping corrupt event" << endmsg;
    return StatusCode::SUCCESS;
  }

  // NB. Collection retrieves wrapped in m_storeGate->contains<..>(..)
  // are for those not expected to be on ESD. They should be on bytestream.

  StatusCode sc;

  //Retrieve Trigger Towers from SG
  const TriggerTowerCollection* triggerTowerTES = 0; 
  sc = evtStore()->retrieve(triggerTowerTES, m_triggerTowerLocation); 
  if( sc.isFailure()  ||  !triggerTowerTES ) {
    msg(MSG::DEBUG) << "No Trigger Tower container found" << endmsg; 
  }

  //Retrieve Core and Overlap CPM Towers from SG
  const CpmTowerCollection* cpmTowerTES = 0; 
  const CpmTowerCollection* cpmTowerOvTES = 0; 
  sc = evtStore()->retrieve(cpmTowerTES, m_cpmTowerLocation); 
  if( sc.isFailure()  ||  !cpmTowerTES ) {
    msg(MSG::DEBUG) << "No Core CPM Tower container found" << endmsg; 
  }
  if (evtStore()->contains<CpmTowerCollection>(m_cpmTowerLocationOverlap)) {
    sc = evtStore()->retrieve(cpmTowerOvTES, m_cpmTowerLocationOverlap); 
  } else sc = StatusCode::FAILURE;
  if( sc.isFailure()  ||  !cpmTowerOvTES ) {
    msg(MSG::DEBUG) << "No Overlap CPM Tower container found" << endmsg; 
  }
  m_overlapPresent = cpmTowerOvTES != 0;
  
  //Retrieve CPM RoIs from SG
  const CpmRoiCollection* cpmRoiTES = 0;
  sc = evtStore()->retrieve( cpmRoiTES, m_cpmRoiLocation);
  if( sc.isFailure()  ||  !cpmRoiTES ) {
    msg(MSG::DEBUG) << "No DAQ CPM RoIs container found" << endmsg;
  }

  //Retrieve ROD Headers from SG
  m_limitedRoi = 0;
  m_rodTES = 0;
  if (evtStore()->contains<RodHeaderCollection>(m_rodHeaderLocation)) {
    sc = evtStore()->retrieve( m_rodTES, m_rodHeaderLocation);
  } else sc = StatusCode::FAILURE;
  if( sc.isFailure()  ||  !m_rodTES ) {
    msg(MSG::DEBUG) << "No ROD Header container found" << endmsg;
  }
  
  //Retrieve CPM Hits from SG
  const CpmHitsCollection* cpmHitsTES = 0;
  sc = evtStore()->retrieve( cpmHitsTES, m_cpmHitsLocation);
  if( sc.isFailure()  ||  !cpmHitsTES ) {
    msg(MSG::DEBUG) << "No CPM Hits container found" << endmsg; 
  }
  
  //Retrieve CMM-CP Hits from SG
  const CmmCpHitsCollection* cmmCpHitsTES = 0;
  sc = evtStore()->retrieve( cmmCpHitsTES, m_cmmCpHitsLocation);
  if( sc.isFailure()  ||  !cmmCpHitsTES ) {
    msg(MSG::DEBUG) << "No CMM-CP Hits container found" << endmsg; 
  }

  // Maps to simplify comparisons
  
  TriggerTowerMap ttMap;
  CpmTowerMap     cpMap;
  CpmTowerMap     ovMap;
  CpmRoiMap       crMap;
  CpmHitsMap      chMap;
  CmmCpHitsMap    cmMap;
  setupMap(triggerTowerTES, ttMap);
  setupMap(cpmTowerTES, cpMap);
  setupMap(cpmTowerOvTES, ovMap);
  setupMap(cpmRoiTES, crMap);
  setupMap(cpmHitsTES, chMap);
  setupMap(cmmCpHitsTES, cmMap);

  // Vectors for error overview bits;
  const int nCrates = 4;
  const int nCPMs   = 14;
  const int nCMMs   = 2;
  const int vecsizeCpm = 2 * nCrates * nCPMs;
  const int vecsizeCmm = 2 * nCrates * nCMMs;
  ErrorVector errorsCPM(vecsizeCpm);
  ErrorVector errorsCMM(vecsizeCmm);

  // Compare Trigger Towers and CPM Towers from data

  bool overlap = false;
  bool mismatchCore = false;
  bool mismatchOverlap = false;
  mismatchCore = compare(ttMap, cpMap, errorsCPM, overlap);
  if (m_overlapPresent) {
    overlap = true;
    mismatchOverlap = compare(ttMap, ovMap, errorsCPM, overlap);
  }

  // Compare RoIs simulated from CPM Towers with CPM RoIs from data

  CpmRoiCollection* cpmRoiSIM = 0;
  if (cpmTowerTES || cpmTowerOvTES) {
    cpmRoiSIM = new CpmRoiCollection;
    if (mismatchCore || mismatchOverlap) {
      simulate(cpMap, ovMap, cpmRoiSIM);
    } else {
      simulate(cpMap, cpmRoiSIM);
    }
  }
  CpmRoiMap crSimMap;
  setupMap(cpmRoiSIM, crSimMap);
  compare(crSimMap, crMap, errorsCPM);
  crSimMap.clear();
  delete cpmRoiSIM;

  // Compare CPM Hits simulated from CPM RoIs with CPM Hits from data

  CpmHitsCollection* cpmHitsSIM = 0;
  if (cpmRoiTES) {
    cpmHitsSIM = new CpmHitsCollection;
    simulate(cpmRoiTES, cpmHitsSIM);
  }
  CpmHitsMap chSimMap;
  setupMap(cpmHitsSIM, chSimMap);
  compare(chSimMap, chMap, errorsCPM);
  chSimMap.clear();
  delete cpmHitsSIM;

  // Compare CPM hits with CMM Hits from data

  compare(chMap, cmMap, errorsCPM, errorsCMM);

  // Compare Local sums simulated from CMM Hits with Local sums from data

  CmmCpHitsCollection* cmmLocalSIM = 0;
  if (cmmCpHitsTES) {
    cmmLocalSIM = new CmmCpHitsCollection;
    simulate(cmmCpHitsTES, cmmLocalSIM, LVL1::CMMCPHits::LOCAL);
  }
  CmmCpHitsMap cmmLocalSimMap;
  setupMap(cmmLocalSIM, cmmLocalSimMap);
  compare(cmmLocalSimMap, cmMap, errorsCMM, LVL1::CMMCPHits::LOCAL);
  cmmLocalSimMap.clear();
  delete cmmLocalSIM;

  // Compare Local sums with Remote sums from data

  compare(cmMap, cmMap, errorsCMM, LVL1::CMMCPHits::REMOTE_0);

  // Compare Total sums simulated from Remote sums with Total sums from data

  CmmCpHitsCollection* cmmTotalSIM = 0;
  if (cmmCpHitsTES) {
    cmmTotalSIM = new CmmCpHitsCollection;
    simulate(cmmCpHitsTES, cmmTotalSIM, LVL1::CMMCPHits::TOTAL);
  }
  CmmCpHitsMap cmmTotalSimMap;
  setupMap(cmmTotalSIM, cmmTotalSimMap);
  compare(cmmTotalSimMap, cmMap, errorsCMM, LVL1::CMMCPHits::TOTAL);
  cmmTotalSimMap.clear();
  delete cmmTotalSIM;

  // Update error summary plots

  ErrorVector crateErr(nCrates);
  const int cpmBins = nCrates * nCPMs;
  const int cmmBins = nCrates * nCMMs;
  for (int err = 0; err < NumberOfSummaryBins; ++err) {
    int error = 0;
    for (int loc = 0; loc < cpmBins; ++loc) {
      if ((errorsCPM[loc] >> err) & 0x1) {
        m_h_cpm_2d_SimEqDataOverview->Fill(loc, err, 1.);
      }
      if ((errorsCPM[loc + cpmBins] >> err) & 0x1) {
        m_h_cpm_2d_SimNeDataOverview->Fill(loc, err, 1.);
	error = 1;
	crateErr[loc/nCPMs] |= (1 << err);
	if (m_v_2d_MismatchEvents[err]) {
	  m_histTool->fillEventNumber(m_v_2d_MismatchEvents[err], loc);
        }
      }
      if (loc < cmmBins) {
        if ((errorsCMM[loc] >> err) & 0x1) {
          m_h_cpm_2d_SimEqDataOverview->Fill(loc+cpmBins, err, 1.);
        }
        if ((errorsCMM[loc + cmmBins] >> err) & 0x1) {
          m_h_cpm_2d_SimNeDataOverview->Fill(loc+cpmBins, err, 1.);
	  error = 1;
	  crateErr[loc/nCMMs] |= (1 << err);
	  int offset = 0;
	  if (err == LocalSumMismatch) offset = 8;
	  else if (err == RemoteSumMismatch || err == TotalSumMismatch)
	                               offset = 16;
	  if (m_v_2d_MismatchEvents[5]) {
	    m_histTool->fillEventNumber(m_v_2d_MismatchEvents[5], loc+offset);
	  }
        }
      }
    }
    if (error) m_h_cpm_1d_SimNeDataSummary->Fill(err);
  }

  // Save error vector for global summary

  ErrorVector* save = new ErrorVector(crateErr);
  sc = evtStore()->record(save, "L1CaloCPMMismatchVector");
  if (sc != StatusCode::SUCCESS) {
    msg(MSG::ERROR) << "Error recording CPM mismatch vector in TES "
                    << endmsg;
    return sc;
  }

  if (m_debug) msg(MSG::DEBUG) << "Leaving fillHistograms" << endmsg;

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode CPMSimBSMon::procHistograms()
/*---------------------------------------------------------*/
{
  msg(MSG::DEBUG) << "procHistograms entered" << endmsg;

  //if (endOfLumiBlock || endOfRun) {
  //}

  return StatusCode::SUCCESS;
}

//  Compare Trigger Towers and CPM Towers

bool CPMSimBSMon::compare(const TriggerTowerMap& ttMap,
                          const CpmTowerMap& cpMap, ErrorVector& errors,
			  bool overlap)
{
  if (m_debug) {
    msg(MSG::DEBUG) << "Compare Trigger Towers and CPM Towers" << endmsg;
  }
 
  bool mismatch = false;
  const int nCrates = 4;
  const int nCPMs   = 14;
  LVL1::CoordToHardware converter;
  TriggerTowerMap::const_iterator ttMapIter    = ttMap.begin();
  TriggerTowerMap::const_iterator ttMapIterEnd = ttMap.end();
  CpmTowerMap::const_iterator     cpMapIter    = cpMap.begin();
  CpmTowerMap::const_iterator     cpMapIterEnd = cpMap.end();

  while (ttMapIter != ttMapIterEnd || cpMapIter != cpMapIterEnd) {

    int ttKey = 0;
    int cpKey = 0;
    int ttEm  = 0;
    int ttHad = 0;
    int cpEm  = 0;
    int cpHad = 0;
    double eta = 0.;
    double phi = 0.;
    int key = 0;

    if (ttMapIter != ttMapIterEnd) ttKey = ttMapIter->first;
    if (cpMapIter != cpMapIterEnd) cpKey = cpMapIter->first;

    if ((cpMapIter == cpMapIterEnd) ||
                 ((ttMapIter != ttMapIterEnd) && (cpKey > ttKey))) {

      // TriggerTower but no CPMTower

      const LVL1::TriggerTower* tt = ttMapIter->second;
      ++ttMapIter;
      eta = tt->eta();
      phi = tt->phi();
      if (overlap) { // skip non-overlap TTs
        const LVL1::Coordinate coord(phi, eta);
	const int crate = converter.cpCrateOverlap(coord);
        if (crate >= nCrates) continue;
      }
      ttEm  = tt->emEnergy();
      ttHad = tt->hadEnergy();
      key = ttKey;

    } else if ((ttMapIter == ttMapIterEnd) ||
                 ((cpMapIter != cpMapIterEnd) && (ttKey > cpKey))) {

      // CPMTower but no TriggerTower

      const LVL1::CPMTower* cp = cpMapIter->second;
      ++cpMapIter;
      eta = cp->eta();
      phi = cp->phi();
      cpEm  = cp->emEnergy();
      cpHad = cp->hadEnergy();
      key = cpKey;

    } else {

      // Have both

      const LVL1::TriggerTower* tt = ttMapIter->second;
      const LVL1::CPMTower*     cp = cpMapIter->second;
      ++ttMapIter;
      ++cpMapIter;
      eta = tt->eta();
      phi = tt->phi();
      ttEm  = tt->emEnergy();
      ttHad = tt->hadEnergy();
      cpEm  = cp->emEnergy();
      cpHad = cp->hadEnergy();
      key = ttKey;
    }

    if (!ttEm && !ttHad && !cpEm && !cpHad) continue;
    
    //  Fill in error plots

    const LVL1::Coordinate coord(phi, eta);
    const int crate = (overlap) ? converter.cpCrateOverlap(coord)
                                : converter.cpCrate(coord);
    const int cpm   = (overlap) ? converter.cpModuleOverlap(coord)
                                : converter.cpModule(coord);
    if (crate >= nCrates || cpm > nCPMs) continue;
    const int loc = crate * nCPMs + cpm - 1;
    const int cpmBins = nCrates * nCPMs;
    const int bitEm  = (1 << EMTowerMismatch);
    const int bitHad = (1 << HadTowerMismatch);
    double phiFPGA = phi;
    if (overlap) {
      const double twoPi    = 2.*M_PI;
      const double piByFour = M_PI/4.;
      if (phi > 7.*piByFour)   phiFPGA -= twoPi;
      else if (phi < piByFour) phiFPGA += twoPi;
    }
    const int loc2 = fpga(crate, phiFPGA);

    TH2F_LW* hist1 = 0;
    TH2F_LW* hist2 = 0;
    if (ttEm && ttEm == cpEm) { // non-zero match
      errors[loc] |= bitEm;
      hist1 = (overlap) ? m_h_cpm_em_2d_etaPhi_tt_PpmEqOverlap
                        : m_h_cpm_em_2d_etaPhi_tt_PpmEqCore;
      hist2 = m_h_cpm_2d_tt_PpmEqCpmFpga;
    } else if (ttEm != cpEm) {  // mis-match
      mismatch = true;
      errors[loc+cpmBins] |= bitEm;
      if (ttEm && cpEm) {       // non-zero mis-match
        hist1 = (overlap) ? m_h_cpm_em_2d_etaPhi_tt_PpmNeOverlap
	                  : m_h_cpm_em_2d_etaPhi_tt_PpmNeCore;
        hist2 = m_h_cpm_2d_tt_PpmNeCpmFpga;
      } else if (!cpEm) {       // no cp
	hist1 = (overlap) ? m_h_cpm_em_2d_etaPhi_tt_PpmNoOverlap
	                  : m_h_cpm_em_2d_etaPhi_tt_PpmNoCore;
	hist2 = m_h_cpm_2d_tt_PpmNoCpmFpga;
      } else {                  // no tt
	hist1 = (overlap) ? m_h_cpm_em_2d_etaPhi_tt_OverlapNoPpm
	                  : m_h_cpm_em_2d_etaPhi_tt_CoreNoPpm;
	hist2 = m_h_cpm_2d_tt_CpmNoPpmFpga;
      }
      if (m_debug) {
        msg(MSG::DEBUG) << " EMTowerMismatch key/eta/phi/crate/cpm/tt/cp: "
                        << key << "/" << eta << "/" << phi << "/" << crate
			<< "/" << cpm << "/" << ttEm << "/" << cpEm << endmsg;
      }
    }
    if (hist1) m_histTool->fillCPMEtaVsPhi(hist1, eta, phi);
    if (hist2) hist2->Fill(loc, loc2);

    hist1 = 0;
    hist2 = 0;
    if (ttHad && ttHad == cpHad) { // non-zero match
      errors[loc] |= bitHad;
      hist1 = (overlap) ? m_h_cpm_had_2d_etaPhi_tt_PpmEqOverlap
                        : m_h_cpm_had_2d_etaPhi_tt_PpmEqCore;
      hist2 = m_h_cpm_2d_tt_PpmEqCpmFpga;
    } else if (ttHad != cpHad) {   // mis-match
      mismatch = true;
      errors[loc+cpmBins] |= bitHad;
      if (ttHad && cpHad) {        // non-zero mis-match
        hist1 = (overlap) ? m_h_cpm_had_2d_etaPhi_tt_PpmNeOverlap
	                  : m_h_cpm_had_2d_etaPhi_tt_PpmNeCore;
        hist2 = m_h_cpm_2d_tt_PpmNeCpmFpga;
      } else if (!cpHad) {         // no cp
	hist1 = (overlap) ? m_h_cpm_had_2d_etaPhi_tt_PpmNoOverlap
	                  : m_h_cpm_had_2d_etaPhi_tt_PpmNoCore;
	hist2 = m_h_cpm_2d_tt_PpmNoCpmFpga;
      } else {                     // no tt
	hist1 = (overlap) ? m_h_cpm_had_2d_etaPhi_tt_OverlapNoPpm
	                  : m_h_cpm_had_2d_etaPhi_tt_CoreNoPpm;
	hist2 = m_h_cpm_2d_tt_CpmNoPpmFpga;
      }
      if (m_debug) {
        msg(MSG::DEBUG) << " HadTowerMismatch key/eta/phi/crate/cpm/tt/cp: "
                        << key << "/" << eta << "/" << phi << "/" << crate
			<< "/" << cpm << "/" << ttHad << "/" << cpHad << endmsg;
      }
    }
    if (hist1) m_histTool->fillCPMEtaVsPhi(hist1, eta, phi);
    if (hist2) hist2->Fill(loc, loc2+1);
  }

  return mismatch;
}

//  Compare Simulated RoIs with data

void CPMSimBSMon::compare(const CpmRoiMap& roiSimMap, const CpmRoiMap& roiMap,
                                                           ErrorVector& errors)
{
  if (m_debug) msg(MSG::DEBUG) << "Compare Simulated RoIs with data" << endmsg;

  const int nCrates = 4;
  const int nCPMs = 14;
  LVL1::CPRoIDecoder decoder;
  CpmRoiMap::const_iterator simMapIter    = roiSimMap.begin();
  CpmRoiMap::const_iterator simMapIterEnd = roiSimMap.end();
  CpmRoiMap::const_iterator datMapIter    = roiMap.begin();
  CpmRoiMap::const_iterator datMapIterEnd = roiMap.end();

  while (simMapIter != simMapIterEnd || datMapIter != datMapIterEnd) {

    int simKey = 0;
    int datKey = 0;
    unsigned int simHits = 0;
    unsigned int datHits = 0;
    const LVL1::CPMRoI* roi = 0;

    if (simMapIter != simMapIterEnd) simKey = simMapIter->first;
    if (datMapIter != datMapIterEnd) datKey = datMapIter->first;

    if ((datMapIter == datMapIterEnd) ||
                ((simMapIter != simMapIterEnd) && (datKey > simKey))) {

      // Simulated RoI but no data RoI

      roi     = simMapIter->second;
      simHits = roi->hits();
      ++simMapIter;

    } else if ((simMapIter == simMapIterEnd) ||
                ((datMapIter != datMapIterEnd) && (simKey > datKey))) {

      // Data RoI but no simulated RoI

      roi     = datMapIter->second;
      datHits = roi->hits();
      ++datMapIter;

    } else {

      // Have both

      const LVL1::CPMRoI* roiS = simMapIter->second;
      roi     = datMapIter->second;
      simHits = roiS->hits();
      datHits = roi ->hits();
      ++simMapIter;
      ++datMapIter;
    }

    if (!simHits && !datHits) continue;

    //  Check LimitedRoISet bit

    const int crate = roi->crate();
    if (!datHits && limitedRoiSet(crate)) continue;
    
    //  Fill in error plots

    const int cpm   = roi->cpm();
    const int chip  = roi->chip();
    const int local = roi->location();
    const int locX  = crate * nCPMs + cpm - 1;
    const int locY  = chip * 8 + local;
    const int cpmBins = nCrates * nCPMs;
    const int bit = (1 << RoIMismatch);
    const LVL1::CoordinateRange coord(decoder.coordinate(roi->roiWord()));
    const double eta = coord.eta();
    const double phi = coord.phi();

    TH2F_LW* hist1 = 0;
    TH2F_LW* hist2 = 0;
    if (simHits == datHits) {
      errors[locX] |= bit;
      hist1 = m_h_cpm_2d_roi_SimEqData;
      hist2 = m_h_cpm_2d_etaPhi_roi_SimEqData;
    } else {
      errors[locX+cpmBins] |= bit;
      if (simHits && datHits) {
        hist1 = m_h_cpm_2d_roi_SimNeData;
	hist2 = m_h_cpm_2d_etaPhi_roi_SimNeData;
      } else if (!datHits) {
        hist1 = m_h_cpm_2d_roi_SimNoData;
	hist2 = m_h_cpm_2d_etaPhi_roi_SimNoData;
      } else {
        hist1 = m_h_cpm_2d_roi_DataNoSim;
	hist2 = m_h_cpm_2d_etaPhi_roi_DataNoSim;
      }
    }
    if (hist1) hist1->Fill(locX, locY);
    if (hist2) m_histTool->fillCPMRoIEtaVsPhi(hist2, eta, phi);

    const int nThresh = 16;
    m_histTool->fillXVsThresholds(m_h_cpm_2d_roi_ThreshSimEqData, locX,
                                                datHits & simHits, nThresh, 1);
    m_histTool->fillXVsThresholds(m_h_cpm_2d_roi_ThreshSimNeData, locX,
                                                datHits ^ simHits, nThresh, 1);

    if (m_debug) {
      msg(MSG::DEBUG) << "DataHits/SimHits: "
                      << m_histTool->thresholdString(datHits, nThresh) << " / "
		      << m_histTool->thresholdString(simHits, nThresh)
		      << endmsg;
    }
  }
}

//  Compare simulated CPM Hits with data

void CPMSimBSMon::compare(const CpmHitsMap& cpmSimMap, const CpmHitsMap& cpmMap,
                                                       ErrorVector& errors)
{
  if (m_debug) {
    msg(MSG::DEBUG) << "Compare simulated CPM Hits with data" << endmsg;
  }

  CpmHitsMap::const_iterator simMapIter    = cpmSimMap.begin();
  CpmHitsMap::const_iterator simMapIterEnd = cpmSimMap.end();
  CpmHitsMap::const_iterator datMapIter    = cpmMap.begin();
  CpmHitsMap::const_iterator datMapIterEnd = cpmMap.end();

  while (simMapIter != simMapIterEnd || datMapIter != datMapIterEnd) {

    int simKey = 0;
    int datKey = 0;
    unsigned int simHits0 = 0;
    unsigned int simHits1 = 0;
    unsigned int datHits0 = 0;
    unsigned int datHits1 = 0;
    int crate = 0;
    int cpm   = 0;

    if (simMapIter != simMapIterEnd) simKey = simMapIter->first;
    if (datMapIter != datMapIterEnd) datKey = datMapIter->first;

    if ((datMapIter == datMapIterEnd) ||
                ((simMapIter != simMapIterEnd) && (datKey > simKey))) {

      // Simulation Hits but no data Hits

      const LVL1::CPMHits* simh = simMapIter->second;
      simHits0 = simh->HitWord0();
      simHits1 = simh->HitWord1();
      crate    = simh->crate();
      cpm      = simh->module();
      ++simMapIter;

    } else if ((simMapIter == simMapIterEnd) ||
                ((datMapIter != datMapIterEnd) && (simKey > datKey))) {

      // Data Hits but no simulation Hits

      const LVL1::CPMHits* dath = datMapIter->second;
      datHits0 = dath->HitWord0();
      datHits1 = dath->HitWord1();
      crate    = dath->crate();
      cpm      = dath->module();
      ++datMapIter;

    } else {

      // Have both

      const LVL1::CPMHits* simh = simMapIter->second;
      const LVL1::CPMHits* dath = datMapIter->second;
      simHits0 = simh->HitWord0();
      simHits1 = simh->HitWord1();
      datHits0 = dath->HitWord0();
      datHits1 = dath->HitWord1();
      crate    = dath->crate();
      cpm      = dath->module();
      ++simMapIter;
      ++datMapIter;
    }

    if (!simHits0 && !simHits1 && !datHits0 && !datHits1) continue;

    //  Check LimitedRoISet bit

    if (((simHits0 < datHits0) || (simHits1 < datHits1))
                               && limitedRoiSet(crate)) continue;
    
    //  Fill in error plots

    const int nCrates = 4;
    const int nCPMs = 14;
    const int loc = crate * nCPMs + cpm - 1;
    const int cpmBins = nCrates * nCPMs;
    const int bit = (1 << CPMHitsMismatch);

    if ((simHits0 && simHits0 == datHits0) ||
        (simHits1 && simHits1 == datHits1)) errors[loc] |= bit;
    if (simHits0 != datHits0 || simHits1 != datHits1)
                                            errors[loc+cpmBins] |= bit;

    if ((simHits0 || simHits1) && simHits0 == datHits0
                               && simHits1 == datHits1) {
      m_h_cpm_2d_thresh_SimEqData->Fill(cpm, crate);
    }
    if ((simHits0 || simHits1) && (datHits0 || datHits1) &&
        (simHits0 != datHits0 || simHits1 != datHits1)) {
      m_h_cpm_2d_thresh_SimNeData->Fill(cpm, crate);
    }
    if ((simHits0 || simHits1) && !datHits0 && !datHits1) {
      m_h_cpm_2d_thresh_SimNoData->Fill(cpm, crate);
    }
    if ((datHits0 || datHits1) && !simHits0 && !simHits1) {
      m_h_cpm_2d_thresh_DataNoSim->Fill(cpm, crate);
    }

    const int nThresh = 8;
    const int thrLen  = 3;
    int same = m_histTool->thresholdsSame(datHits0, simHits0, nThresh, thrLen);
    int diff = m_histTool->thresholdsDiff(datHits0, simHits0, nThresh, thrLen);
    m_histTool->fillXVsThresholds(m_h_cpm_2d_thresh_ThreshSimEqData, loc, same,
                                                                  nThresh, 1);
    m_histTool->fillXVsThresholds(m_h_cpm_2d_thresh_ThreshSimNeData, loc, diff,
                                                                  nThresh, 1);
    same = m_histTool->thresholdsSame(datHits1, simHits1, nThresh, thrLen);
    diff = m_histTool->thresholdsDiff(datHits1, simHits1, nThresh, thrLen);
    const int offset = nThresh;
    m_histTool->fillXVsThresholds(m_h_cpm_2d_thresh_ThreshSimEqData, loc, same,
                                                          nThresh, 1, offset);
    m_histTool->fillXVsThresholds(m_h_cpm_2d_thresh_ThreshSimNeData, loc, diff,
                                                          nThresh, 1, offset);
  }
}

//  Compare CPM Hits and CMM Hits

void CPMSimBSMon::compare(const CpmHitsMap& cpmMap, const CmmCpHitsMap& cmmMap,
                          ErrorVector& errorsCPM, ErrorVector& errorsCMM)
{
  if (m_debug) msg(MSG::DEBUG) << "Compare CPM Hits and CMM Hits" << endmsg;

  const int nCrates = 4;
  const int nCPMs = 14;
  const int nCMMs = 2;
  CpmHitsMap::const_iterator   cpmMapIter    = cpmMap.begin();
  CpmHitsMap::const_iterator   cpmMapIterEnd = cpmMap.end();
  CmmCpHitsMap::const_iterator cmmMapIter    = cmmMap.begin();
  CmmCpHitsMap::const_iterator cmmMapIterEnd = cmmMap.end();

  while (cpmMapIter != cpmMapIterEnd || cmmMapIter != cmmMapIterEnd) {

    int cpmKey = 0;
    int cmmKey = 0;
    unsigned int cpmHits0 = 0;
    unsigned int cpmHits1 = 0;
    unsigned int cmmHits0 = 0;
    unsigned int cmmHits1 = 0;
    int crate  = 0;
    int cpm    = 0;

    if (cpmMapIter != cpmMapIterEnd) cpmKey = cpmMapIter->first;
    if (cmmMapIter != cmmMapIterEnd) cmmKey = cmmMapIter->first;

    if ((cmmMapIter == cmmMapIterEnd) ||
                  ((cpmMapIter != cpmMapIterEnd) && (cmmKey > cpmKey))) {

      // CPM Hits but no CMM Hits

      const LVL1::CPMHits* cpmh = cpmMapIter->second;
      cpmHits0 = cpmh->HitWord0();
      cpmHits1 = cpmh->HitWord1();
      crate    = cpmh->crate();
      cpm      = cpmh->module();
      ++cpmMapIter;

    } else if ((cpmMapIter == cpmMapIterEnd) ||
                  ((cmmMapIter != cmmMapIterEnd) && (cpmKey > cmmKey))) {

      // CMM Hits but no CPM Hits

      const LVL1::CMMCPHits* cmmh = cmmMapIter->second;
      cmmHits0 = cmmh->HitWord0();
      cmmHits1 = cmmh->HitWord1();
      crate    = cmmh->crate();
      cpm      = cmmh->dataID();
      ++cmmMapIter;
      if (cpm > nCPMs) continue;

    } else {

      // Have both

      const LVL1::CPMHits*   cpmh = cpmMapIter->second;
      const LVL1::CMMCPHits* cmmh = cmmMapIter->second;
      cpmHits0 = cpmh->HitWord0();
      cpmHits1 = cpmh->HitWord1();
      cmmHits0 = cmmh->HitWord0();
      cmmHits1 = cmmh->HitWord1();
      crate    = cpmh->crate();
      cpm      = cpmh->module();
      ++cpmMapIter;
      ++cmmMapIter;
    }

    if (!cpmHits0 && !cpmHits1 && !cmmHits0 && !cmmHits1) continue;
    
    //  Fill in error plots

    const int loc  = crate * nCPMs + cpm - 1;
    const int loc2 = crate * nCMMs;
    const int cpmBins = nCrates * nCPMs;
    const int cmmBins = nCrates * nCMMs;
    const int bit = (1 << CMMHitsMismatch);
    
    if ((cpmHits0 && cpmHits0 == cmmHits0) ||
        (cpmHits1 && cpmHits1 == cmmHits1)) errorsCPM[loc] |= bit;
    if (cpmHits0 != cmmHits0 || cpmHits1 != cmmHits1)
                                            errorsCPM[loc+cpmBins] |= bit;

    TH2F_LW* hist = 0;
    if (cpmHits1 && cpmHits1 == cmmHits1) { // hits1==>cmm 0
      errorsCMM[loc2] |= bit;
      hist = m_h_cmm_2d_thresh_CpmEqCmm;
    } else if (cpmHits1 != cmmHits1) {
      errorsCMM[loc2+cmmBins] |= bit;
      if (cpmHits1 && cmmHits1) hist = m_h_cmm_2d_thresh_CpmNeCmm;
      else if (!cmmHits1)       hist = m_h_cmm_2d_thresh_CpmNoCmm;
      else                      hist = m_h_cmm_2d_thresh_CmmNoCpm;
    }
    if (hist) hist->Fill(cpm, loc2);

    hist = 0;
    if (cpmHits0 && cpmHits0 == cmmHits0) { // hits0==>cmm 1
      errorsCMM[loc2+1] |= bit;
      hist = m_h_cmm_2d_thresh_CpmEqCmm;
    } else if (cpmHits0 != cmmHits0) {
      errorsCMM[loc2+cmmBins+1] |= bit;
      if (cpmHits0 && cmmHits0) hist = m_h_cmm_2d_thresh_CpmNeCmm;
      else if (!cmmHits0)       hist = m_h_cmm_2d_thresh_CpmNoCmm;
      else                      hist = m_h_cmm_2d_thresh_CmmNoCpm;
    }
    if (hist) hist->Fill(cpm, loc2+1);

    const int nThresh = 8;
    const int thrLen  = 3;
    int same = m_histTool->thresholdsSame(cmmHits0, cpmHits0, nThresh, thrLen);
    int diff = m_histTool->thresholdsDiff(cmmHits0, cpmHits0, nThresh, thrLen);
    m_histTool->fillXVsThresholds(m_h_cmm_2d_thresh_ThreshCpmEqCmm, loc, same,
                                                                  nThresh, 1);
    m_histTool->fillXVsThresholds(m_h_cmm_2d_thresh_ThreshCpmNeCmm, loc, diff,
                                                                  nThresh, 1);
    same = m_histTool->thresholdsSame(cmmHits1, cpmHits1, nThresh, thrLen);
    diff = m_histTool->thresholdsDiff(cmmHits1, cpmHits1, nThresh, thrLen);
    const int offset = nThresh;
    m_histTool->fillXVsThresholds(m_h_cmm_2d_thresh_ThreshCpmEqCmm, loc, same,
                                                          nThresh, 1, offset);
    m_histTool->fillXVsThresholds(m_h_cmm_2d_thresh_ThreshCpmNeCmm, loc, diff,
                                                          nThresh, 1, offset);
  }
}

//  Compare Simulated CMM Hit Sums and Data CMM Hit Sums

void CPMSimBSMon::compare(const CmmCpHitsMap& cmmSimMap,
                          const CmmCpHitsMap& cmmMap,
                          ErrorVector& errors, int selection)
{
  msg(MSG::DEBUG) << "Compare Simulated CMM Hit Sums and Data CMM Hit Sums"
                  << endmsg;

  const bool local  = (selection == LVL1::CMMCPHits::LOCAL);
  const bool remote = (selection == LVL1::CMMCPHits::REMOTE_0);
  const bool total  = (selection == LVL1::CMMCPHits::TOTAL);
  if (!local && !remote && !total) return;
  std::vector<unsigned int> hits0Sim(3);
  std::vector<unsigned int> hits1Sim(3);
  std::vector<unsigned int> hits0(3);
  std::vector<unsigned int> hits1(3);
  const int nCrates = 4;
  const int nCMMs = 2;
  CmmCpHitsMap::const_iterator cmmSimMapIter    = cmmSimMap.begin();
  CmmCpHitsMap::const_iterator cmmSimMapIterEnd = cmmSimMap.end();
  CmmCpHitsMap::const_iterator cmmMapIter       = cmmMap.begin();
  CmmCpHitsMap::const_iterator cmmMapIterEnd    = cmmMap.end();

  while (cmmSimMapIter != cmmSimMapIterEnd || cmmMapIter != cmmMapIterEnd) {

    int cmmSimKey = 0;
    int cmmKey    = 0;
    unsigned int cmmSimHits0 = 0;
    unsigned int cmmSimHits1 = 0;
    unsigned int cmmHits0 = 0;
    unsigned int cmmHits1 = 0;
    int crate  = 0;
    int dataId = 0;

    if (cmmSimMapIter != cmmSimMapIterEnd) cmmSimKey = cmmSimMapIter->first;
    if (cmmMapIter    != cmmMapIterEnd)    cmmKey    = cmmMapIter->first;

    if ((cmmMapIter == cmmMapIterEnd) ||
             ((cmmSimMapIter != cmmSimMapIterEnd) && (cmmKey > cmmSimKey))) {

      // Sim CMM Hits but no Data CMM Hits

      const LVL1::CMMCPHits* cmmS = cmmSimMapIter->second;
      ++cmmSimMapIter;
      dataId = cmmS->dataID();
      if (local  && dataId != LVL1::CMMCPHits::LOCAL) continue;
      if (remote && dataId != LVL1::CMMCPHits::LOCAL) continue;
      if (total  && dataId != LVL1::CMMCPHits::TOTAL) continue;
      cmmSimHits0 = cmmS->HitWord0();
      cmmSimHits1 = cmmS->HitWord1();
      crate       = cmmS->crate();

    } else if ((cmmSimMapIter == cmmSimMapIterEnd) ||
                 ((cmmMapIter != cmmMapIterEnd) && (cmmSimKey > cmmKey))) {

      // Data CMM Hits but no Sim CMM Hits

      const LVL1::CMMCPHits* cmmD = cmmMapIter->second;
      ++cmmMapIter;
      dataId   = cmmD->dataID();
      if (local  && dataId != LVL1::CMMCPHits::LOCAL)    continue;
      if (remote && dataId != LVL1::CMMCPHits::REMOTE_0 &&
                    dataId != LVL1::CMMCPHits::REMOTE_1 &&
		    dataId != LVL1::CMMCPHits::REMOTE_2) continue;
      if (total  && dataId != LVL1::CMMCPHits::TOTAL)    continue;
      cmmHits0 = cmmD->HitWord0();
      cmmHits1 = cmmD->HitWord1();
      crate    = cmmD->crate();

    } else {

      // Have both

      const LVL1::CMMCPHits* cmmS = cmmSimMapIter->second;
      const LVL1::CMMCPHits* cmmD = cmmMapIter->second;
      ++cmmSimMapIter;
      ++cmmMapIter;
      dataId   = cmmS->dataID();
      if (local  && dataId != LVL1::CMMCPHits::LOCAL)    continue;
      if (remote && dataId != LVL1::CMMCPHits::LOCAL    &&
                    dataId != LVL1::CMMCPHits::REMOTE_0 &&
                    dataId != LVL1::CMMCPHits::REMOTE_1 &&
		    dataId != LVL1::CMMCPHits::REMOTE_2) continue;
      if (total  && dataId != LVL1::CMMCPHits::TOTAL)    continue;
      cmmSimHits0 = cmmS->HitWord0();
      cmmSimHits1 = cmmS->HitWord1();
      cmmHits0    = cmmD->HitWord0();
      cmmHits1    = cmmD->HitWord1();
      crate       = cmmS->crate();
    }

    if (!cmmSimHits0 && !cmmSimHits1 && !cmmHits0 && !cmmHits1) continue;
    
    //  Fill in error plots

    if (local || total) {
      int loc = crate * nCMMs;
      const int cmmBins = nCrates * nCMMs;
      const int bit = (local) ? (1 << LocalSumMismatch)
                              : (1 << TotalSumMismatch);
      TH1F_LW* hist1 = 0;
      if (cmmSimHits1 && cmmSimHits1 == cmmHits1) {
        errors[loc] |= bit;
	hist1 = m_h_cmm_1d_thresh_SumsSimEqData;
      } else if (cmmSimHits1 != cmmHits1) {
        errors[loc+cmmBins] |= bit;
	if (cmmSimHits1 && cmmHits1) hist1 = m_h_cmm_1d_thresh_SumsSimNeData;
	else if (!cmmHits1)          hist1 = m_h_cmm_1d_thresh_SumsSimNoData;
	else                         hist1 = m_h_cmm_1d_thresh_SumsDataNoSim;
      }
      TH1F_LW* hist0 = 0;
      if (cmmSimHits0 && cmmSimHits0 == cmmHits0) {
        errors[loc+1] |= bit;
	hist0 = m_h_cmm_1d_thresh_SumsSimEqData;
      } else if (cmmSimHits0 != cmmHits0) {
        errors[loc+cmmBins+1] |= bit;
	if (cmmSimHits0 && cmmHits0) hist0 = m_h_cmm_1d_thresh_SumsSimNeData;
	else if (!cmmHits0)          hist0 = m_h_cmm_1d_thresh_SumsSimNoData;
	else                         hist0 = m_h_cmm_1d_thresh_SumsDataNoSim;
      }
      loc = (local) ? loc : 14;
      if (hist1) hist1->Fill(loc);
      if (hist0) hist0->Fill(loc+1);

      loc /= 2;
      const int nThresh = 8;
      const int thrLen  = 3;
      int same = m_histTool->thresholdsSame(cmmHits0, cmmSimHits0, nThresh,
                                                                   thrLen);
      int diff = m_histTool->thresholdsDiff(cmmHits0, cmmSimHits0, nThresh,
                                                                   thrLen);
      m_histTool->fillXVsThresholds(m_h_cmm_2d_thresh_SumsThreshSimEqData,
                                                    loc, same, nThresh, 1);
      m_histTool->fillXVsThresholds(m_h_cmm_2d_thresh_SumsThreshSimNeData,
                                                    loc, diff, nThresh, 1);
      same = m_histTool->thresholdsSame(cmmHits1, cmmSimHits1, nThresh, thrLen);
      diff = m_histTool->thresholdsDiff(cmmHits1, cmmSimHits1, nThresh, thrLen);
      const int offset = nThresh;
      m_histTool->fillXVsThresholds(m_h_cmm_2d_thresh_SumsThreshSimEqData,
                                              loc, same, nThresh, 1, offset);
      m_histTool->fillXVsThresholds(m_h_cmm_2d_thresh_SumsThreshSimNeData,
                                              loc, diff, nThresh, 1, offset);
    } else {
      if (dataId == LVL1::CMMCPHits::LOCAL) {
        if (crate != nCrates-1) {
	  hits0Sim[crate] = cmmSimHits0;
	  hits1Sim[crate] = cmmSimHits1;
        }
      } else if (dataId == LVL1::CMMCPHits::REMOTE_0) {
        hits0[0] = cmmHits0;
        hits1[0] = cmmHits1;
      } else if (dataId == LVL1::CMMCPHits::REMOTE_1) {
        hits0[1] = cmmHits0;
        hits1[1] = cmmHits1;
      } else {
        hits0[2] = cmmHits0;
        hits1[2] = cmmHits1;
      }
    }
  }
  if (remote) {
    for (int crate = 0; crate < nCrates-1; ++crate) {
      int loc = crate * nCMMs;
      const int cmmBins = nCrates * nCMMs;
      const int bit = (1 << RemoteSumMismatch);
      const unsigned int hd0 = hits0[crate];
      const unsigned int hd1 = hits1[crate];
      const unsigned int hs0 = hits0Sim[crate];
      const unsigned int hs1 = hits1Sim[crate];

      if (!hd0 && !hd1 && !hs0 && !hs1) continue;

      TH1F_LW* hist1 = 0;
      if (hs1 && hs1 == hd1) {
        errors[loc] |= bit;
	hist1 = m_h_cmm_1d_thresh_SumsSimEqData;
      } else if (hs1 != hd1) {
        errors[loc+cmmBins] |= bit;
	if (hs1 && hd1) hist1 = m_h_cmm_1d_thresh_SumsSimNeData;
	else if (!hd1)  hist1 = m_h_cmm_1d_thresh_SumsSimNoData;
	else            hist1 = m_h_cmm_1d_thresh_SumsDataNoSim;
      }
      TH1F_LW* hist0 = 0;
      if (hs0 && hs0 == hd0) {
        errors[loc+1] |= bit;
	hist0 = m_h_cmm_1d_thresh_SumsSimEqData;
      } else if (hs0 != hd0) {
        errors[loc+cmmBins+1] |= bit;
	if (hs0 && hd0) hist0 = m_h_cmm_1d_thresh_SumsSimNeData;
	else if (!hd0)  hist0 = m_h_cmm_1d_thresh_SumsSimNoData;
	else            hist0 = m_h_cmm_1d_thresh_SumsDataNoSim;
      }
      loc += 8;
      if (hist1) hist1->Fill(loc);
      if (hist0) hist0->Fill(loc+1);

      loc /= 2;
      const int nThresh = 8;
      const int thrLen  = 3;
      int same = m_histTool->thresholdsSame(hd0, hs0, nThresh, thrLen);
      int diff = m_histTool->thresholdsDiff(hd0, hs0, nThresh, thrLen);
      m_histTool->fillXVsThresholds(m_h_cmm_2d_thresh_SumsThreshSimEqData,
                                                       loc, same, nThresh, 1);
      m_histTool->fillXVsThresholds(m_h_cmm_2d_thresh_SumsThreshSimNeData,
                                                       loc, diff, nThresh, 1);
      same = m_histTool->thresholdsSame(hd1, hs1, nThresh, thrLen);
      diff = m_histTool->thresholdsDiff(hd1, hs1, nThresh, thrLen);
      const int offset = nThresh;
      m_histTool->fillXVsThresholds(m_h_cmm_2d_thresh_SumsThreshSimEqData,
                                               loc, same, nThresh, 1, offset);
      m_histTool->fillXVsThresholds(m_h_cmm_2d_thresh_SumsThreshSimNeData,
                                               loc, diff, nThresh, 1, offset);
    }
  }
}

void CPMSimBSMon::setLabels(LWHist* hist, bool xAxis)
{
  LWHist::LWHistAxis* axis = (xAxis) ? hist->GetXaxis() : hist->GetYaxis();
  // Simulation steps in red (#color[2]) depend on Trigger Menu
  axis->SetBinLabel(1+EMTowerMismatch,  "EM tt");
  axis->SetBinLabel(1+HadTowerMismatch, "Had tt");
  axis->SetBinLabel(1+RoIMismatch,      "#color[2]{RoIs}");
  axis->SetBinLabel(1+CPMHitsMismatch,  "CPMHits");
  axis->SetBinLabel(1+CMMHitsMismatch,  "CMMHits");
  axis->SetBinLabel(1+LocalSumMismatch, "#splitline{Local}{Sums}");
  axis->SetBinLabel(1+RemoteSumMismatch,"#splitline{Remote}{Sums}");
  axis->SetBinLabel(1+TotalSumMismatch, "#splitline{Total}{Sums}");
}

void CPMSimBSMon::setupMap(const TriggerTowerCollection* coll,
                                 TriggerTowerMap& map)
{
  if (coll) {
    LVL1::TriggerTowerKey towerKey;
    TriggerTowerCollection::const_iterator pos  = coll->begin();
    TriggerTowerCollection::const_iterator posE = coll->end();
    for (; pos != posE; ++pos) {
      const double eta = (*pos)->eta();
      if (eta > -2.5 && eta < 2.5 &&
                     ((*pos)->emEnergy() > 0 || (*pos)->hadEnergy() > 0)) {
        const double phi = (*pos)->phi();
        const int key = towerKey.ttKey(phi, eta);
        map.insert(std::make_pair(key, *pos));
      }
    }
  }
}

void CPMSimBSMon::setupMap(const CpmTowerCollection* coll, CpmTowerMap& map)
{
  if (coll) {
    LVL1::TriggerTowerKey towerKey;
    CpmTowerCollection::const_iterator pos  = coll->begin();
    CpmTowerCollection::const_iterator posE = coll->end();
    for (; pos != posE; ++pos) {
      const double eta = (*pos)->eta();
      const double phi = (*pos)->phi();
      const int key = towerKey.ttKey(phi, eta);
      map.insert(std::make_pair(key, *pos));
    }
  }
}

void CPMSimBSMon::setupMap(const CpmRoiCollection* coll, CpmRoiMap& map)
{
  if (coll) {
    CpmRoiCollection::const_iterator pos  = coll->begin();
    CpmRoiCollection::const_iterator posE = coll->end();
    for (; pos != posE; ++pos) {
      const int crate = (*pos)->crate();
      const int cpm   = (*pos)->cpm();
      const int chip  = (*pos)->chip();
      const int loc   = (*pos)->location();
      const int key   = (((((crate << 4) | cpm) << 3) | chip) << 3) | loc;
      map.insert(std::make_pair(key, *pos));
    }
  }
}

void CPMSimBSMon::setupMap(const CpmHitsCollection* coll, CpmHitsMap& map)
{
  if (coll) {
    CpmHitsCollection::const_iterator pos  = coll->begin();
    CpmHitsCollection::const_iterator posE = coll->end();
    for (; pos != posE; ++pos) {
      const int crate = (*pos)->crate();
      const int cpm   = (*pos)->module();
      const int key   = crate * 100 + cpm;
      map.insert(std::make_pair(key, *pos));
    }
  }
}

void CPMSimBSMon::setupMap(const CmmCpHitsCollection* coll, CmmCpHitsMap& map)
{
  if (coll) {
    CmmCpHitsCollection::const_iterator pos  = coll->begin();
    CmmCpHitsCollection::const_iterator posE = coll->end();
    for (; pos != posE; ++pos) {
      const int crate  = (*pos)->crate();
      const int dataId = (*pos)->dataID();
      const int key  = crate * 100 + dataId;
      map.insert(std::make_pair(key, *pos));
    }
  }
}

void CPMSimBSMon::simulate(const CpmTowerMap towers, const CpmTowerMap towersOv,
                                 CpmRoiCollection* rois)
{
  if (m_debug) msg(MSG::DEBUG) << "Simulate CPM RoIs from CPM Towers" << endmsg;

  // Process a crate at a time to use overlap data
  const int ncrates = 4;
  std::vector<CpmTowerMap> crateMaps(ncrates);
  LVL1::CoordToHardware converter;
  CpmTowerCollection* tempColl = new CpmTowerCollection;
  CpmTowerMap::const_iterator iter  = towers.begin();
  CpmTowerMap::const_iterator iterE = towers.end();
  for (; iter != iterE; ++iter) {
    CpmTowerMap::mapped_type tt = ttCheck(iter->second, tempColl);
    const LVL1::Coordinate coord(tt->phi(), tt->eta());
    const int crate = converter.cpCrate(coord);
    if (crate >= ncrates) continue;
    crateMaps[crate].insert(std::make_pair(iter->first, tt));
  }
  // If overlap data not present take from core data
  iter  = (m_overlapPresent) ? towersOv.begin() : towers.begin();
  iterE = (m_overlapPresent) ? towersOv.end()   : towers.end();
  for (; iter != iterE; ++iter) {
    CpmTowerMap::mapped_type tt = ttCheck(iter->second, tempColl);
    const LVL1::Coordinate coord(tt->phi(), tt->eta());
    const int crate = converter.cpCrateOverlap(coord);
    if (crate >= ncrates) continue;
    crateMaps[crate].insert(std::make_pair(iter->first, tt));
  }
  for (int crate = 0; crate < ncrates; ++crate) {
    InternalRoiCollection* intRois = new InternalRoiCollection;
    m_emTauTool->findRoIs(&crateMaps[crate], intRois);
    InternalRoiCollection::iterator roiIter  = intRois->begin();
    InternalRoiCollection::iterator roiIterE = intRois->end();
    for (; roiIter != roiIterE; ++roiIter) {
      LVL1::CPMRoI* roi = new LVL1::CPMRoI((*roiIter)->RoIWord());
      if (roi->crate() == crate) rois->push_back(roi);
      else delete roi;
    }
    delete intRois;
  }
  delete tempColl;
}

// Quicker version when overlap same as core

void CPMSimBSMon::simulate(const CpmTowerMap towers, CpmRoiCollection* rois)
{
  if (m_debug) msg(MSG::DEBUG) << "Simulate CPM RoIs from CPM Towers" << endmsg;

  InternalRoiCollection* intRois = new InternalRoiCollection;
  m_emTauTool->findRoIs(&towers, intRois);
  InternalRoiCollection::iterator roiIter  = intRois->begin();
  InternalRoiCollection::iterator roiIterE = intRois->end();
  for (; roiIter != roiIterE; ++roiIter) {
    LVL1::CPMRoI* roi = new LVL1::CPMRoI((*roiIter)->RoIWord());
    rois->push_back(roi);
  }
  delete intRois;
}

void CPMSimBSMon::simulate(const CpmRoiCollection* rois,
                                 CpmHitsCollection* hits)
{
  if (m_debug) msg(MSG::DEBUG) << "Simulate CPM Hits from CPM RoIs" << endmsg;

  m_cpHitsTool->formCPMHits(rois, hits);
}

void CPMSimBSMon::simulate(const CmmCpHitsCollection* hitsIn,
                                 CmmCpHitsCollection* hitsOut, int selection)
{
  if (m_debug) msg(MSG::DEBUG) << "Simulate CMM Hit sums from CMM Hits"
                               << endmsg;

  if (selection == LVL1::CMMCPHits::LOCAL) {
    m_cpHitsTool->formCMMCPHitsCrate(hitsIn, hitsOut);
  } else if (selection == LVL1::CMMCPHits::TOTAL) {
    m_cpHitsTool->formCMMCPHitsSystem(hitsIn, hitsOut);
  }
}

// Return EM FPGA for given crate/phi

int CPMSimBSMon::fpga(int crate, double phi)
{
  const double phiGran = M_PI/32.;
  const double phiBase = M_PI/2. * double(crate);
  const int phiBin = int(floor((phi - phiBase) / phiGran)) + 2;
  return 2 * (phiBin/2);
}

// Return a tower with zero energy if parity bit is set

CPMSimBSMon::CpmTowerMap::mapped_type
CPMSimBSMon::ttCheck(CpmTowerMap::mapped_type tt,
                     CpmTowerCollection* coll)
{
  const LVL1::DataError emError(tt->emError());
  const LVL1::DataError hadError(tt->hadError());
  const int emParity  = emError.get(LVL1::DataError::Parity);
  const int hadParity = hadError.get(LVL1::DataError::Parity);
  if ((emParity && tt->emEnergy()) || (hadParity && tt->hadEnergy())) {
    const int peak = tt->peak();
    std::vector<int> emEnergyVec(tt->emEnergyVec());
    std::vector<int> hadEnergyVec(tt->hadEnergyVec());
    std::vector<int> emErrorVec(tt->emErrorVec());
    std::vector<int> hadErrorVec(tt->hadErrorVec());
    if (emParity)  emEnergyVec[peak]  = 0;
    if (hadParity) hadEnergyVec[peak] = 0;
    LVL1::CPMTower* ct = new LVL1::CPMTower(tt->phi(), tt->eta(),
        emEnergyVec, emErrorVec, hadEnergyVec, hadErrorVec, peak);
    coll->push_back(ct);
    return ct;
  }
  return tt;
}

// Check if LimitedRoISet bit set

bool CPMSimBSMon::limitedRoiSet(int crate)
{
  if (m_rodTES) {
    m_limitedRoi = 0;
    const int nCrates = 4;
    RodHeaderCollection::const_iterator rodIter  = m_rodTES->begin();
    RodHeaderCollection::const_iterator rodIterE = m_rodTES->end();
    for (; rodIter != rodIterE; ++rodIter) {
      const LVL1::RODHeader* rod = *rodIter;
      const int rodCrate = rod->crate() - 8;
      if (rodCrate >= 0 && rodCrate < nCrates
          && rod->dataType() == 1 && rod->limitedRoISet()) {
        m_limitedRoi |= (1<<rodCrate);
      }
    }
    m_rodTES = 0;
  }
  return (((m_limitedRoi>>crate)&0x1) == 1);
}

// ============================================================================
}  // end namespace
// ============================================================================
