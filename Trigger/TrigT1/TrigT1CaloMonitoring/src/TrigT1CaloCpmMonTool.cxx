/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigT1CaloCpmMonTool.cxx
// PACKAGE:  TrigT1CaloMonitoring  
//
// AUTHOR:   Peter Faulkner
//           
//
// ********************************************************************

#include <numeric>
#include <utility>

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
#include "TrigT1CaloEvent/TriggerTower.h"
#include "TrigT1CaloUtils/CoordToHardware.h"
#include "TrigT1CaloUtils/DataError.h"
#include "TrigT1CaloUtils/TriggerTowerKey.h"
#include "TrigT1Interfaces/CoordinateRange.h"
#include "TrigT1Interfaces/CPRoIDecoder.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

#include "TrigT1CaloMonitoringTools/ITrigT1CaloMonErrorTool.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramToolV1.h"

#include "TrigT1CaloCpmMonTool.h"
// ============================================================================
namespace LVL1 {
// ============================================================================
const int TrigT1CaloCpmMonTool::s_crates;
const int TrigT1CaloCpmMonTool::s_modules;
const int TrigT1CaloCpmMonTool::s_maxSlices;
const int TrigT1CaloCpmMonTool::s_thresholds;
const int TrigT1CaloCpmMonTool::s_threshBits;
const int TrigT1CaloCpmMonTool::s_threshMask;
// ============================================================================
TrigT1CaloCpmMonTool::TrigT1CaloCpmMonTool(const std::string & type, 
				           const std::string & name,
				           const IInterface* parent)
  : ManagedMonitorToolBase(type, name, parent),
    m_errorTool("LVL1::TrigT1CaloMonErrorToolV1/TrigT1CaloMonErrorToolV1"),
    m_histTool("LVL1::TrigT1CaloLWHistogramToolV1/TrigT1CaloLWHistogramToolV1"),
    m_events(0),
    m_emBitMask(0),
    m_tauBitMask(0),
    m_histBooked(false),
    m_h_cpm_2d_tt_Slices(0),
    m_h_cmm_2d_thresh_Slices(0),
    m_h_cpm_2d_tt_SliceMatch(0),
    m_h_cmm_2d_thresh_SliceMatch(0),
    m_h_ppm_em_2d_etaPhi_tt_Hitmap(0),
    m_h_ppm_had_2d_etaPhi_tt_Hitmap(0),
    m_h_cpm_em_1d_tt_Et(0),
    m_h_cpm_had_1d_tt_Et(0),
    m_h_cpm_em_1d_tt_Eta(0),
    m_h_cpm_had_1d_tt_Eta(0),
    m_h_cpm_em_1d_tt_Phi(0),
    m_h_cpm_had_1d_tt_Phi(0),
    m_h_cpm_em_2d_etaPhi_tt_Hitmap(0),
    m_h_cpm_had_2d_etaPhi_tt_Hitmap(0),
    m_h_cpm_em_2d_etaPhi_tt_EtWeighted(0),
    m_h_cpm_had_2d_etaPhi_tt_EtWeighted(0),
    m_h_cpm_em_2d_etaPhi_tt_Parity(0),
    m_h_cpm_had_2d_etaPhi_tt_Parity(0),
    m_h_cpm_em_2d_etaPhi_tt_LinkDown(0),
    m_h_cpm_had_2d_etaPhi_tt_LinkDown(0),
    m_h_cpm_2d_Status(0),
    m_h_cpm_2d_roi_Thresholds(0),
    m_h_cpm_2d_etaPhi_roi_HitmapAll(0),
    m_h_cpm_2d_etaPhi_roi_HitmapEm(0),
    m_h_cpm_2d_etaPhi_roi_HitmapTau(0),
    m_h_cpm_2d_etaPhi_roi_Hitmap(0),
    m_h_cpm_2d_etaPhi_roi_EmHitmap(0),
    m_h_cpm_2d_etaPhi_roi_TauHitmap(0),
    m_h_cpm_2d_etaPhi_roi_Saturation(0),
    m_h_cpm_2d_etaPhi_roi_Parity(0),
    m_h_cpm_2d_thresh_Weighted(0),
    m_h_cmm_2d_thresh_Weighted(0),
    m_h_cmm_2d_thresh_SumsWeighted(0),
    m_h_cmm_2d_thresh_Parity(0),
    m_h_cmm_2d_Status(0),
    m_h_cpm_1d_ErrorSummary(0),
    m_h_cpm_2d_ErrorOverview(0),
    m_h_cpm_2d_ErrorEventNumbers(0)
/*---------------------------------------------------------*/
{

  declareProperty("CPMTowerLocation",
                 m_cpmTowerLocation  = LVL1::TrigT1CaloDefs::CPMTowerLocation);
  declareProperty("CPMTowerLocationOverlap",
                 m_cpmTowerLocationOverlap =
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

  declareProperty("RootDirectory", m_rootDir = "L1Calo");
  declareProperty("MaxEnergyRange", m_maxEnergyRange = 256,
                  "Maximum energy plotted");

}

/*---------------------------------------------------------*/
TrigT1CaloCpmMonTool::~TrigT1CaloCpmMonTool()
/*---------------------------------------------------------*/
{
}

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

/*---------------------------------------------------------*/
StatusCode TrigT1CaloCpmMonTool:: initialize()
/*---------------------------------------------------------*/
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << PACKAGE_VERSION << endmsg;

  StatusCode sc;

  sc = ManagedMonitorToolBase::initialize();
  if (sc.isFailure()) return sc;

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
StatusCode TrigT1CaloCpmMonTool:: finalize()
/*---------------------------------------------------------*/
{
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TrigT1CaloCpmMonTool::bookHistogramsRecurrent()
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

  if ( newRun ) {

  MgmtAttr_t attr = ATTRIB_UNMANAGED;
  std::string dir1(m_rootDir + "/CPM");
  MonGroup monShift( this, dir1 + "/Errors/Hardware", run, attr );
  MonGroup monExpert( this, dir1 + "/Errors/Hardware", run, attr );
  MonGroup monDetail( this, dir1 + "/Errors/Hardware/Detail", run, attr );
  MonGroup monEvents( this, dir1 + "/Errors/Hardware/Detail", run, attr, "",
                                                               "eventSample" );
  MonGroup monCPMin( this, dir1 + "/Input", run, attr );
  MonGroup monRoIs( this, dir1 + "/Output/RoI", run, attr );
  MonGroup monCPMout( this, dir1 + "/Output/Thresholds", run, attr);
  std::string dir2(m_rootDir + "/CPM_CMM");
  MonGroup monCMM( this, dir2 + "/Errors/Hardware",  run, attr );
  MonGroup monCMMin( this, dir2 + "/Input",  run, attr );
  MonGroup monCMMout( this, dir2 + "/Output",  run, attr );

  //  Timeslice checks

  m_histTool->setMonGroup(&monCPMin);

  const int xbins = s_crates*s_maxSlices;
  m_h_cpm_2d_tt_Slices = m_histTool->book2F("cpm_2d_tt_Slices",
       "CPM Slices and Triggered Slice;Crate/Number of Slices;Triggered Slice",
        xbins, 0, xbins, s_maxSlices, 0, s_maxSlices);
  m_histTool->numberPairs(m_h_cpm_2d_tt_Slices, 0, s_crates-1, 1, s_maxSlices);
  m_histTool->numbers(m_h_cpm_2d_tt_Slices, 0, s_maxSlices-1, 1, 0, false);
  m_h_cpm_2d_tt_SliceMatch = m_histTool->book2F("cpm_2d_tt_SliceMatch",
        "PPM/CPM Tower Slice Match;Crate/CPM Slice;PPM Slice",
        xbins, 0, xbins, s_maxSlices, 0, s_maxSlices);
  m_histTool->numberPairs(m_h_cpm_2d_tt_SliceMatch, 0, s_crates-1, 0, s_maxSlices-1);
  m_histTool->numbers(m_h_cpm_2d_tt_SliceMatch, 0, s_maxSlices-1, 1, 0, false);

  m_histTool->setMonGroup(&monCMMin);

  m_h_cmm_2d_thresh_Slices = m_histTool->book2F("cmm_2d_thresh_Slices",
       "CMM Slices and Triggered Slice;Crate/Number of Slices;Triggered Slice",
        xbins, 0, xbins, s_maxSlices, 0, s_maxSlices);
  m_histTool->numberPairs(m_h_cmm_2d_thresh_Slices, 0, s_crates-1, 1, s_maxSlices);
  m_histTool->numbers(m_h_cmm_2d_thresh_Slices, 0, s_maxSlices-1, 1, 0, false);
  m_h_cmm_2d_thresh_SliceMatch = m_histTool->book2F("cmm_2d_thresh_SliceMatch",
        "CPM/CMM Hits Slice Match;Crate/CMM Slice;CPM Slice",
	xbins, 0, xbins, s_maxSlices, 0, s_maxSlices);
  m_histTool->numberPairs(m_h_cmm_2d_thresh_SliceMatch, 0, s_crates-1, 0, s_maxSlices-1);
  m_histTool->numbers(m_h_cmm_2d_thresh_SliceMatch, 0, s_maxSlices-1, 1, 0, false);

  //  CPM Tower - Trigger Tower Histos

  m_histTool->setMonGroup(&monCPMin);

  m_h_ppm_em_2d_etaPhi_tt_Hitmap = m_histTool->bookCPMEtaVsPhi(
    "ppm_em_2d_etaPhi_tt_Hitmap", "PPM Trigger Tower EM eta/phi");
  m_h_ppm_had_2d_etaPhi_tt_Hitmap = m_histTool->bookCPMEtaVsPhi(
    "ppm_had_2d_etaPhi_tt_Hitmap", "PPM Trigger Tower HAD eta/phi");

  m_h_cpm_em_1d_tt_Et = m_histTool->book1F("cpm_em_1d_tt_Et", "CPM Tower EM Et",
                                    m_maxEnergyRange, 0, m_maxEnergyRange);
  m_h_cpm_had_1d_tt_Et = m_histTool->book1F("cpm_had_1d_tt_Et", "CPM Tower HAD Et",
                                    m_maxEnergyRange, 0, m_maxEnergyRange);
  m_h_cpm_em_1d_tt_Eta = m_histTool->book1F("cpm_em_1d_tt_Eta",
                                     "CPM Tower EM eta",50,-2.5,2.5);
  m_h_cpm_had_1d_tt_Eta = m_histTool->book1F("cpm_had_1d_tt_Eta",
                                      "CPM Tower HAD eta",50,-2.5,2.5);
  m_h_cpm_em_1d_tt_Phi = m_histTool->book1F("cpm_em_1d_tt_Phi",
                                     "CPM Tower EM phi",64,0.,2.*M_PI);
  m_h_cpm_had_1d_tt_Phi = m_histTool->book1F("cpm_had_1d_tt_Phi",
                                      "CPM Tower HAD phi", 64,0.,2.*M_PI);
  m_h_cpm_em_2d_etaPhi_tt_Hitmap = m_histTool->bookCPMEtaVsPhi(
    "cpm_em_2d_etaPhi_tt_Hitmap", "CPM Tower EM eta/phi");
  m_h_cpm_had_2d_etaPhi_tt_Hitmap = m_histTool->bookCPMEtaVsPhi(
    "cpm_had_2d_etaPhi_tt_Hitmap", "CPM Tower HAD eta/phi");
  m_h_cpm_em_2d_etaPhi_tt_EtWeighted = m_histTool->bookCPMEtaVsPhi(
    "cpm_em_2d_etaPhi_tt_EtWeighted", "CPM Tower EM eta/phi weighted");
  m_h_cpm_had_2d_etaPhi_tt_EtWeighted = m_histTool->bookCPMEtaVsPhi(
    "cpm_had_2d_etaPhi_tt_EtWeighted", "CPM Tower HAD eta/phi weighted");

  //  CPM Tower error bits

  m_histTool->setMonGroup(&monDetail);

  m_h_cpm_em_2d_etaPhi_tt_Parity = m_histTool->bookCPMEtaVsPhi(
    "cpm_em_2d_etaPhi_tt_Parity", "CPM Tower EM Parity Errors");
  m_h_cpm_had_2d_etaPhi_tt_Parity = m_histTool->bookCPMEtaVsPhi(
    "cpm_had_2d_etaPhi_tt_Parity", "CPM Tower HAD Parity Errors");
  m_h_cpm_em_2d_etaPhi_tt_LinkDown = m_histTool->bookCPMEtaVsPhi(
    "cpm_em_2d_etaPhi_tt_LinkDown", "CPM Tower EM Link Down Errors");
  m_h_cpm_had_2d_etaPhi_tt_LinkDown = m_histTool->bookCPMEtaVsPhi(
    "cpm_had_2d_etaPhi_tt_LinkDown", "CPM Tower HAD Link Down Errors");
  m_h_cpm_2d_Status = m_histTool->bookCPMSubStatusVsCrateModule(
                               "cpm_2d_Status", "CPM Sub-status bits");

  //  CPM RoIs

  m_histTool->setMonGroup(&monRoIs);

  m_h_cpm_2d_roi_Thresholds = m_histTool->bookCPMCrateModuleVsThreshold(
                       "cpm_2d_roi_Thresholds", "CPM RoI Thresholds");
  m_h_cpm_2d_etaPhi_roi_HitmapAll = m_histTool->bookCPMRoIEtaVsPhi(
    "cpm_2d_etaPhi_roi_HitmapAll", "CPM RoIs All Eta-Phi Hit Map");
  m_h_cpm_2d_etaPhi_roi_HitmapEm = m_histTool->bookCPMRoIEtaVsPhi(
    "cpm_2d_etaPhi_roi_HitmapEm", "CPM RoIs EM Eta-Phi Hit Map");
  m_h_cpm_2d_etaPhi_roi_HitmapTau = m_histTool->bookCPMRoIEtaVsPhi(
    "cpm_2d_etaPhi_roi_HitmapTau", "CPM RoIs Tau Eta-Phi Hit Map");
  m_emBitMask  = m_histTool->thresholdMaskEm();
  m_tauBitMask = m_histTool->thresholdMaskTau();
  // Delete these duplicates when DQ updated:
  m_h_cpm_2d_etaPhi_roi_Hitmap = m_histTool->bookCPMRoIEtaVsPhi(
    "cpm_2d_etaPhi_roi_Hitmap", "CPM RoIs All Eta-Phi Hit Map (Duplicate)");
  m_h_cpm_2d_etaPhi_roi_EmHitmap = m_histTool->bookCPMRoIEtaVsPhi(
    "cpm_2d_etaPhi_roi_EmHitmap", "CPM RoIs EM Eta-Phi Hit Map (Duplicate)");
  m_h_cpm_2d_etaPhi_roi_TauHitmap = m_histTool->bookCPMRoIEtaVsPhi(
    "cpm_2d_etaPhi_roi_TauHitmap", "CPM RoIs Tau Eta-Phi Hit Map (Duplicate)");
  // end delete
  m_h_cpm_2d_etaPhi_roi_Saturation = m_histTool->bookCPMRoIEtaVsPhi(
    "cpm_2d_etaPhi_roi_Saturation", "CPM RoI Tower Saturation");

  m_histTool->setMonGroup(&monDetail);

  m_h_cpm_2d_etaPhi_roi_Parity = m_histTool->bookCPMRoIEtaVsPhi(
    "cpm_2d_etaPhi_roi_Parity", "CPM RoI Parity Errors");

  //  CPM Hits
  
  m_histTool->setMonGroup(&monCPMout);

  m_h_cpm_2d_thresh_Weighted = m_histTool->bookCPMCrateModuleVsThreshold(
    "cpm_2d_thresh_Weighted", "CPM Hits Thresholds Weighted");

  //  CMM-CP Hits

  m_histTool->setMonGroup(&monCMMin);

  m_h_cmm_2d_thresh_Weighted = m_histTool->bookCPMCrateModuleVsThreshold(
    "cmm_2d_thresh_Weighted", "CMM-CP Hits Thresholds Weighted");

  m_histTool->setMonGroup(&monCMMout);

  m_h_cmm_2d_thresh_SumsWeighted = m_histTool->bookCPMSumVsThreshold(
    "cmm_2d_thresh_SumsWeighted", "CMM-CP Hit Sums Thresholds Weighted");

  //  CMM error bits

  m_histTool->setMonGroup(&monCMM);

  m_h_cmm_2d_thresh_Parity = m_histTool->book2F("cmm_2d_thresh_Parity",
                          "CMM Parity Errors;Module or Remote;Crate/CMM",
		           15, 1, 16, 8, 0, 8);
  m_histTool->numbers(m_h_cmm_2d_thresh_Parity, 1, s_modules);
  m_h_cmm_2d_thresh_Parity->GetXaxis()->SetBinLabel(15, "REM");
  m_histTool->numberPairs(m_h_cmm_2d_thresh_Parity, 0, s_crates-1, 0, 1, 1, 0, false);
 
  m_h_cmm_2d_Status = m_histTool->book2F("cmm_2d_Status",
                          "CMM Sub-status bits;;Crate/CMM",
                           8, 0., 8., 8, 0., 8.);
  m_histTool->subStatus(m_h_cmm_2d_Status);
  m_histTool->numberPairs(m_h_cmm_2d_Status, 0, s_crates-1, 0, 1, 1, 0, false);
  
  //  Error Overview and Summary

  m_histTool->setMonGroup(&monExpert);

  m_h_cpm_2d_ErrorOverview = m_histTool->book2F("cpm_2d_ErrorOverview",
                           "CP Error Overview", 64, 0, 64,
			    NumberOfSummaryBins, 0, NumberOfSummaryBins);
  m_histTool->cpmCMMCrateModule(m_h_cpm_2d_ErrorOverview);

  m_histTool->setMonGroup(&monShift);

  m_h_cpm_1d_ErrorSummary = m_histTool->book1F("cpm_1d_ErrorSummary",
                         "CP Error Summary;;Events",
                          NumberOfSummaryBins, 0, NumberOfSummaryBins);

  m_histTool->setMonGroup(&monEvents);

  m_h_cpm_2d_ErrorEventNumbers = m_histTool->bookEventNumbers("cpm_2d_ErrorEventNumbers",
                         "CP Error Event Numbers",
			 NumberOfSummaryBins, 0, NumberOfSummaryBins);

  LWHist* hist = m_h_cpm_2d_ErrorOverview;
  LWHist::LWHistAxis* axis = hist->GetYaxis();
  for (int i = 0; i < 3; ++i) {
    axis->SetBinLabel(1+EMParity,  "EM parity");
    axis->SetBinLabel(1+EMLink,    "EM link");
    axis->SetBinLabel(1+HadParity, "Had parity");
    axis->SetBinLabel(1+HadLink,   "Had link");
    axis->SetBinLabel(1+CPMStatus, "CPM status");
    axis->SetBinLabel(1+RoIParity, "RoI parity");
    axis->SetBinLabel(1+CMMParity, "CMM parity");
    axis->SetBinLabel(1+CMMStatus, "CMM status");
    if (i == 0) {
      hist = m_h_cpm_1d_ErrorSummary;
      axis = hist->GetXaxis();
    } else {
      hist = m_h_cpm_2d_ErrorEventNumbers;
      axis = hist->GetYaxis();
    }
  }

  m_histTool->unsetMonGroup();

  m_events = 0;
  m_histBooked = true;

  } // end if (newRun ...

  msg(MSG::DEBUG) << "Leaving bookHistograms" << endmsg;

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TrigT1CaloCpmMonTool::fillHistograms()
/*---------------------------------------------------------*/
{
  const bool debug = msgLvl(MSG::DEBUG);
  if (debug) msg(MSG::DEBUG) << "fillHistograms entered" << endmsg;

  if (!m_histBooked) {
    if (debug) msg(MSG::DEBUG) << "Histogram(s) not booked" << endmsg;
    return StatusCode::SUCCESS;
  }

  // Skip events believed to be corrupt

  if (m_errorTool->corrupt()) {
    if (debug) msg(MSG::DEBUG) << "Skipping corrupt event" << endmsg;
    return StatusCode::SUCCESS;
  }

  //Retrieve Trigger Towers from SG
  const TriggerTowerCollection* triggerTowerTES = 0; 
  StatusCode sc = evtStore()->retrieve(triggerTowerTES,
                                                     m_triggerTowerLocation); 
  if( sc.isFailure()  ||  !triggerTowerTES ) {
    msg(MSG::DEBUG) << "No Trigger Tower container found"<< endmsg; 
  }

  //Retrieve Core CPM Towers from SG
  const CpmTowerCollection* cpmTowerTES = 0; 
  sc = evtStore()->retrieve(cpmTowerTES, m_cpmTowerLocation); 
  if( sc.isFailure()  ||  !cpmTowerTES ) {
    msg(MSG::DEBUG) << "No Core CPM Tower container found"<< endmsg; 
  }

  //Retrieve Overlap CPM Towers from SG
  const CpmTowerCollection* cpmTowerOverlapTES = 0; 
  if (evtStore()->contains<CpmTowerCollection>(m_cpmTowerLocationOverlap)) {
    sc = evtStore()->retrieve(cpmTowerOverlapTES, m_cpmTowerLocationOverlap); 
  } else sc = StatusCode::FAILURE;
  if( sc.isFailure()  ||  !cpmTowerOverlapTES ) {
    msg(MSG::DEBUG) << "No Overlap CPM Tower container found"<< endmsg; 
  }
  
  //Retrieve CPM RoIs from SG
  const CpmRoiCollection* cpmRoiTES = 0;
  sc = evtStore()->retrieve( cpmRoiTES, m_cpmRoiLocation);
  if( sc.isFailure()  ||  !cpmRoiTES ) {
    msg(MSG::DEBUG) << "No CPM RoIs container found"<< endmsg;
  }
  
  //Retrieve CPM Hits from SG
  const CpmHitsCollection* cpmHitsTES = 0;
  sc = evtStore()->retrieve( cpmHitsTES, m_cpmHitsLocation);
  if( sc.isFailure()  ||  !cpmHitsTES ) {
    msg(MSG::DEBUG) << "No CPM Hits container found"<< endmsg; 
  }
  
  //Retrieve CMM-CP Hits from SG
  const CmmCpHitsCollection* cmmCpHitsTES = 0;
  sc = evtStore()->retrieve( cmmCpHitsTES, m_cmmCpHitsLocation);
  if( sc.isFailure()  ||  !cmmCpHitsTES ) {
    msg(MSG::DEBUG) << "No CMM-CP Hits container found"<< endmsg; 
  }

  // Vectors for error overview bits;
  std::vector<int> errorsCPM(s_crates*s_modules);
  std::vector<int> errorsCMM(s_crates*2); // L/R

  //=============================================
  //   CPM Tower - Trigger Tower comparison plots
  //=============================================

  // Maps for slice match
  TriggerTowerMap ttMap;
  CpmTowerMap     cpMap;
  LVL1::TriggerTowerKey towerKey;
  unsigned int maxKey = 0;

  // Global plots

  if (triggerTowerTES) {
    TriggerTowerCollection::const_iterator ttIterator =
                                                      triggerTowerTES->begin(); 
    TriggerTowerCollection::const_iterator ttIteratorEnd =
                                                      triggerTowerTES->end(); 
    for (; ttIterator != ttIteratorEnd; ++ttIterator) {
      LVL1::TriggerTower* tt = *ttIterator;
      const double eta = tt->eta();
      if (eta < -2.5 || eta > 2.5) continue;
      const std::vector<int>& emLut(tt->emLUT());
      const std::vector<int>& hadLut(tt->hadLUT());
      if (std::accumulate(emLut.begin(), emLut.end(), 0) == 0 &&
          std::accumulate(hadLut.begin(), hadLut.end(), 0) == 0) continue;
      const int    em  = tt->emEnergy();
      const int    had = tt->hadEnergy();
      const double phi = tt->phi();
      if (em)  m_histTool->fillCPMEtaVsPhi(m_h_ppm_em_2d_etaPhi_tt_Hitmap, eta, phi);
      if (had) m_histTool->fillCPMEtaVsPhi(m_h_ppm_had_2d_etaPhi_tt_Hitmap, eta, phi);
      const unsigned int key = towerKey.ttKey(phi, eta);
      if (key > maxKey) maxKey = key;
      ttMap.insert(std::make_pair(key, tt));
    }
  }

  for (int i = 0; i < 2; ++i) {
    const bool core = (i == 0);
    const CpmTowerCollection* cpmTwrTES = (core) ? cpmTowerTES
                                                 : cpmTowerOverlapTES;
    if (cpmTwrTES) {
      CpmTowerCollection::const_iterator ctIterator    = cpmTwrTES->begin(); 
      CpmTowerCollection::const_iterator ctIteratorEnd = cpmTwrTES->end(); 

      for (; ctIterator != ctIteratorEnd; ++ctIterator) {
	LVL1::CPMTower* ct = *ctIterator;
        const int    em  = ct->emEnergy();
        const int    had = ct->hadEnergy();
        const double eta = ct->eta();
        const double phi = ct->phi();
        const LVL1::Coordinate coord(phi, eta);
        LVL1::CoordToHardware converter;
        const int crate  = (core) ? converter.cpCrate(coord)
	                          : converter.cpCrateOverlap(coord);
        const int cpm    = (core) ? converter.cpModule(coord)
	                          : converter.cpModuleOverlap(coord);
        const int loc    = crate * s_modules + cpm - 1;
        const int peak   = ct->peak();
        const int slices = (ct->emEnergyVec()).size();
        m_h_cpm_2d_tt_Slices->Fill(crate*s_maxSlices + slices - 1, peak, 1.);
        if (em && core) {
          m_h_cpm_em_1d_tt_Et->Fill(em, 1.);
          m_h_cpm_em_1d_tt_Eta->Fill(eta, 1.);
          m_h_cpm_em_1d_tt_Phi->Fill(phi, 1.);
	  m_histTool->fillCPMEtaVsPhi(m_h_cpm_em_2d_etaPhi_tt_Hitmap, eta, phi);
	  m_histTool->fillCPMEtaVsPhi(m_h_cpm_em_2d_etaPhi_tt_EtWeighted, eta, phi, em);
        }
        if (had && core) {
          m_h_cpm_had_1d_tt_Et->Fill(had, 1.);
          m_h_cpm_had_1d_tt_Eta->Fill(eta, 1.);
          m_h_cpm_had_1d_tt_Phi->Fill(phi, 1.);
	  m_histTool->fillCPMEtaVsPhi(m_h_cpm_had_2d_etaPhi_tt_Hitmap, eta, phi);
	  m_histTool->fillCPMEtaVsPhi(m_h_cpm_had_2d_etaPhi_tt_EtWeighted, eta, phi, had);
        }
        // Errors
	int error = ct->emError();
	if (error) {
	  const LVL1::DataError emError(error);
	  if (emError.get(LVL1::DataError::Parity)) {
	    m_histTool->fillCPMEtaVsPhi(m_h_cpm_em_2d_etaPhi_tt_Parity, eta, phi);
	    errorsCPM[loc] |= (1 << EMParity);
	  }
          if (emError.get(LVL1::DataError::LinkDown)) {
	    m_histTool->fillCPMEtaVsPhi(m_h_cpm_em_2d_etaPhi_tt_LinkDown, eta, phi);
	    errorsCPM[loc] |= (1 << EMLink);
          }
	  const int status = (error >> LVL1::DataError::GLinkParity) & 0xff;
	  if (status) {
	    for (int bit = 0; bit < 8; ++bit) {
	      if ((status >> bit) & 0x1) m_h_cpm_2d_Status->Fill(bit, loc);
            }
	    errorsCPM[loc] |= (1 << CPMStatus);
          }
        }
	error = ct->hadError();
	if (error) {
	  const LVL1::DataError hadError(error);
	  if (hadError.get(LVL1::DataError::Parity)) {
	    m_histTool->fillCPMEtaVsPhi(m_h_cpm_had_2d_etaPhi_tt_Parity, eta, phi);
	    errorsCPM[loc] |= (1 << HadParity);
	  }
          if (hadError.get(LVL1::DataError::LinkDown)) {
	    m_histTool->fillCPMEtaVsPhi(m_h_cpm_had_2d_etaPhi_tt_LinkDown, eta, phi);
	    errorsCPM[loc] |= (1 << HadLink);
          }
        }

        if (core) {
          const unsigned int key = towerKey.ttKey(phi, eta);
          if (key > maxKey) maxKey = key;
          cpMap.insert(std::make_pair(key, ct));
        }
      }
    }
  }

  // Slice match

  TriggerTowerMap::const_iterator ttMapIter    = ttMap.begin();
  TriggerTowerMap::const_iterator ttMapIterEnd = ttMap.end();
  CpmTowerMap::const_iterator     cpMapIter    = cpMap.begin();
  CpmTowerMap::const_iterator     cpMapIterEnd = cpMap.end();

  while (ttMapIter != ttMapIterEnd && cpMapIter != cpMapIterEnd) {

    const unsigned int ttKey = ttMapIter->first;
    const unsigned int cpKey = cpMapIter->first;

    if      (cpKey > ttKey) ++ttMapIter;
    else if (ttKey > cpKey) ++cpMapIter;

    else {
      const LVL1::TriggerTower* tt = ttMapIter->second;
      const LVL1::CPMTower*     cp = cpMapIter->second;
      const std::vector<int>& emLut(tt->emLUT());
      const std::vector<int>& hadLut(tt->hadLUT());
      const std::vector<int>& emVec(cp->emEnergyVec());
      const std::vector<int>& hadVec(cp->hadEnergyVec());
      const int sliceEmLut = emLut.size();
      const int sliceHadLut = hadLut.size();
      const int sliceEmVec = emVec.size();
      const int sliceHadVec = hadVec.size();
      const int crate = static_cast<int>(tt->phi()/(M_PI/2.));
      for (int slice = 0; slice < sliceEmLut; ++slice) {
        if (emLut[slice] > 0) {
	  for (int slice2 = 0; slice2 < sliceEmVec; ++slice2) {
	    if (emLut[slice] == emVec[slice2]) {
	      m_h_cpm_2d_tt_SliceMatch->Fill(crate*s_maxSlices+slice2, slice, 1.);
            }
          }
        }
      }
      for (int slice = 0; slice < sliceHadLut; ++slice) {
        if (hadLut[slice] > 0) {
	  for (int slice2 = 0; slice2 < sliceHadVec; ++slice2) {
	    if (hadLut[slice] == hadVec[slice2]) {
	      m_h_cpm_2d_tt_SliceMatch->Fill(crate*s_maxSlices+slice2, slice, 1.);
            }
          }
        }
      }
      ++ttMapIter;
      ++cpMapIter;
    }
  }

  //=============================================
  //  CPM RoIs
  //=============================================

  if (cpmRoiTES) {
    LVL1::CPRoIDecoder decoder;
    CpmRoiCollection::const_iterator crIterator    = cpmRoiTES->begin(); 
    CpmRoiCollection::const_iterator crIteratorEnd = cpmRoiTES->end(); 
    for (; crIterator != crIteratorEnd; ++crIterator) {
      const int hits  = (*crIterator)->hits();
      const LVL1::CoordinateRange coord(
                                decoder.coordinate((*crIterator)->roiWord()));
      const double eta = coord.eta();
      const double phi = coord.phi();
      const int crate = (*crIterator)->crate();
      const int cpm   = (*crIterator)->cpm();
      const int bin   = crate * s_modules + cpm - 1;
      if (hits) {
        m_histTool->fillXVsThresholds(m_h_cpm_2d_roi_Thresholds, bin, hits,
                                                             s_thresholds, 1);
        m_histTool->fillCPMRoIEtaVsPhi(m_h_cpm_2d_etaPhi_roi_HitmapAll, eta, phi);
	//To be deleted:
        m_histTool->fillCPMRoIEtaVsPhi(m_h_cpm_2d_etaPhi_roi_Hitmap, eta, phi);
	if (hits & m_emBitMask) {
	  m_histTool->fillCPMRoIEtaVsPhi(m_h_cpm_2d_etaPhi_roi_HitmapEm, eta, phi);
	  //To be deleted:
	  m_histTool->fillCPMRoIEtaVsPhi(m_h_cpm_2d_etaPhi_roi_EmHitmap, eta, phi);
        }
	if (hits & m_tauBitMask ) {
	  m_histTool->fillCPMRoIEtaVsPhi(m_h_cpm_2d_etaPhi_roi_HitmapTau, eta, phi);
	  //To be deleted:
	  m_histTool->fillCPMRoIEtaVsPhi(m_h_cpm_2d_etaPhi_roi_TauHitmap, eta, phi);
        }
      }
      const LVL1::DataError err((*crIterator)->error());
      if (err.get(LVL1::DataError::Overflow)) {
	m_histTool->fillCPMRoIEtaVsPhi(m_h_cpm_2d_etaPhi_roi_Saturation, eta, phi);
      }
      if (err.get(LVL1::DataError::Parity)) {
	m_histTool->fillCPMRoIEtaVsPhi(m_h_cpm_2d_etaPhi_roi_Parity, eta, phi);
	errorsCPM[bin] |= (1 << RoIParity);
      }
    }
  }

  //=============================================
  //  CPM Hits
  //=============================================

  CpmHitsMap cpmMap;

  if (cpmHitsTES) {
    CpmHitsCollection::const_iterator chIterator    = cpmHitsTES->begin(); 
    CpmHitsCollection::const_iterator chIteratorEnd = cpmHitsTES->end(); 
    for (; chIterator != chIteratorEnd; ++chIterator) {
      const unsigned int hits0 = (*chIterator)->HitWord0();
      const unsigned int hits1 = (*chIterator)->HitWord1();
      const int crate = (*chIterator)->crate();
      const int cpm   = (*chIterator)->module();
      const int bin   = crate * s_modules + cpm - 1;
      const int peak   = (*chIterator)->peak();
      const int slices = ((*chIterator)->HitsVec0()).size();
      m_h_cpm_2d_tt_Slices->Fill(crate*s_maxSlices + slices -1, peak, 1.);
      const int nThresh = s_thresholds/2;
      if (hits0) m_histTool->fillXVsThresholds(m_h_cpm_2d_thresh_Weighted, bin, hits0,
                                               nThresh, s_threshBits);
      if (hits1) m_histTool->fillXVsThresholds(m_h_cpm_2d_thresh_Weighted, bin, hits1,
                                               nThresh, s_threshBits, nThresh);
      const unsigned int key = crate * s_modules + cpm;
      cpmMap.insert(std::make_pair(key, *chIterator));
    }
  }

  //=============================================
  //  CMM-CP Hits
  //=============================================

  CmmCpHitsMap cmmMap;

  if (cmmCpHitsTES) {
    CmmCpHitsCollection::const_iterator cmIterator    = cmmCpHitsTES->begin(); 
    CmmCpHitsCollection::const_iterator cmIteratorEnd = cmmCpHitsTES->end(); 
    for (; cmIterator != cmIteratorEnd; ++cmIterator) {
      const unsigned int hits0 = (*cmIterator)->HitWord0();
      const unsigned int hits1 = (*cmIterator)->HitWord1();
      const int crate  = (*cmIterator)->crate();
      const int dataId = (*cmIterator)->dataID();
      const int peak   = (*cmIterator)->peak();
      const int slices = ((*cmIterator)->HitsVec0()).size();
      m_h_cmm_2d_thresh_Slices->Fill(crate*s_maxSlices + slices -1, peak, 1.);
      int bin = 0;
      if (dataId <= s_modules) bin = crate*s_modules + dataId - 1;
      else {
	bin = crate;
	if (dataId == LVL1::CMMCPHits::REMOTE_0) bin = s_crates;
	if (dataId == LVL1::CMMCPHits::REMOTE_1) bin = s_crates + 1;
	if (dataId == LVL1::CMMCPHits::REMOTE_2) bin = s_crates + 2;
	if (dataId == LVL1::CMMCPHits::TOTAL)    bin = s_crates + 3;
      }
      const int nThresh = s_thresholds/2;
      TH2F_LW* hist = (dataId <= s_modules) ? m_h_cmm_2d_thresh_Weighted
                                            : m_h_cmm_2d_thresh_SumsWeighted;
      if (hits0) m_histTool->fillXVsThresholds(hist, bin, hits0, nThresh,
                                                     s_threshBits);
      if (hits1) m_histTool->fillXVsThresholds(hist, bin, hits1, nThresh,
                                                     s_threshBits, nThresh);
      // Errors
      const int error0 = (*cmIterator)->Error0();
      const int error1 = (*cmIterator)->Error1();
      if (error0 || error1) {
        const LVL1::DataError hit0Err(error0);
        const LVL1::DataError hit1Err(error1);
	const int parity0 = hit0Err.get(LVL1::DataError::Parity);
	const int parity1 = hit1Err.get(LVL1::DataError::Parity);
	if (parity0 || parity1) {
          if (dataId <= s_modules) {
	    if (parity1) m_h_cmm_2d_thresh_Parity->Fill(dataId, 2*crate);
	    if (parity0) m_h_cmm_2d_thresh_Parity->Fill(dataId, 2*crate + 1);
	    errorsCPM[bin] |= (1 << CMMParity);
          } else {
  	    const int remBin = s_modules + 1;
	    int remCrate = -1;
            if      (dataId == LVL1::CMMCPHits::REMOTE_0) remCrate = 0;
            else if (dataId == LVL1::CMMCPHits::REMOTE_1) remCrate = 1;
            else if (dataId == LVL1::CMMCPHits::REMOTE_2) remCrate = 2;
	    if (remCrate >= 0) {
	      if (parity1) m_h_cmm_2d_thresh_Parity->Fill(remBin, 2*remCrate);
	      if (parity0) m_h_cmm_2d_thresh_Parity->Fill(remBin, 2*remCrate + 1);
	    }
          }
	  if (parity1) errorsCMM[crate*2]     |= (1 << CMMParity);
	  if (parity0) errorsCMM[crate*2 + 1] |= (1 << CMMParity);
        }
        // Sub-status errors
        const int status0 = error0 >> LVL1::DataError::GLinkParity;
        const int status1 = error1 >> LVL1::DataError::GLinkParity;
	if (status0 || status1) {
          for (int bit = 0; bit < 8; ++bit) {
	    if ((status1 >> bit) & 0x1) m_h_cmm_2d_Status->Fill(bit, 2*crate);
	    if ((status0 >> bit) & 0x1) m_h_cmm_2d_Status->Fill(bit, 2*crate + 1);
	  }
	  if (status1) errorsCMM[crate*2]     |= (1 << CMMStatus);
	  if (status0) errorsCMM[crate*2 + 1] |= (1 << CMMStatus);
        }
      }

      if (dataId <= s_modules) {
        const unsigned int key = crate * s_modules + dataId;
        cmmMap.insert(std::make_pair(key, *cmIterator));
      }
    }
  }

  // Slice match

  CpmHitsMap::const_iterator   cpmMapIter    = cpmMap.begin();
  CpmHitsMap::const_iterator   cpmMapIterEnd = cpmMap.end();
  CmmCpHitsMap::const_iterator cmmMapIter    = cmmMap.begin();
  CmmCpHitsMap::const_iterator cmmMapIterEnd = cmmMap.end();

  while (cpmMapIter != cpmMapIterEnd && cmmMapIter != cmmMapIterEnd) {

    const unsigned int cpmKey = cpmMapIter->first;
    const unsigned int cmmKey = cmmMapIter->first;

    if      (cmmKey > cpmKey) ++cpmMapIter;
    else if (cpmKey > cmmKey) ++cmmMapIter;

    else {
      const LVL1::CPMHits*   cpmh = cpmMapIter->second;
      const LVL1::CMMCPHits* cmmh = cmmMapIter->second;
      const std::vector<unsigned int>& cpmVec0(cpmh->HitsVec0());
      const std::vector<unsigned int>& cpmVec1(cpmh->HitsVec1());
      const std::vector<unsigned int>& cmmVec0(cmmh->HitsVec0());
      const std::vector<unsigned int>& cmmVec1(cmmh->HitsVec1());
      const int sliceCpmVec0 = cpmVec0.size();
      const int sliceCpmVec1 = cpmVec1.size();
      const int sliceCmmVec0 = cmmVec0.size();
      const int sliceCmmVec1 = cmmVec1.size();
      const int crate = cpmh->crate();
      if (sliceCpmVec0 == sliceCpmVec1 && sliceCmmVec0 == sliceCmmVec1) {
        for (int slice = 0; slice < sliceCpmVec0; ++slice) {
          if (cpmVec0[slice] > 0 || cpmVec1[slice] > 0) {
	    for (int slice2 = 0; slice2 < sliceCmmVec0; ++slice2) {
	      if (cpmVec0[slice] == cmmVec0[slice2] &&
	          cpmVec1[slice] == cmmVec1[slice2]) {
		m_h_cmm_2d_thresh_SliceMatch->Fill(crate*s_maxSlices+slice2, slice, 1.);
	      }
            }
          }
        }
      }
      ++cpmMapIter;
      ++cmmMapIter;
    }
  }

  // Update error summary plot

  std::vector<int> crateErr(4);
  for (int err = 0; err < NumberOfSummaryBins; ++err) {
    int error = 0;
    for (int loc = 0; loc < s_crates*s_modules; ++loc) {
      if ((errorsCPM[loc] >> err) & 0x1) {
        m_h_cpm_2d_ErrorOverview->Fill(loc, err, 1.);
	error = 1;
	crateErr[loc/s_modules] |= (1 << err);
      }
      if (loc < s_crates*2) {
        if ((errorsCMM[loc] >> err) & 0x1) {
          m_h_cpm_2d_ErrorOverview->Fill(loc+s_crates*s_modules, err, 1.);
	  error = 1;
	  crateErr[loc/2] |= (1 << err);
        }
      }
    }
    if (error) {
      m_h_cpm_1d_ErrorSummary->Fill(err);
      m_histTool->fillEventNumber(m_h_cpm_2d_ErrorEventNumbers, err);
    }
  }
  ++m_events;

  // Save error vector for global summary

  std::vector<int>* save = new std::vector<int>(crateErr);
  sc = evtStore()->record(save, "L1CaloCPMErrorVector");
  if (sc != StatusCode::SUCCESS) {
    msg(MSG::ERROR) << "Error recording CPM error vector in TES " << endmsg;
    return sc;
  }

  msg(MSG::DEBUG) << "Leaving fillHistograms" << endmsg;

  return StatusCode::SUCCESS;

}

/*---------------------------------------------------------*/
StatusCode TrigT1CaloCpmMonTool::procHistograms()
/*---------------------------------------------------------*/
{
  msg(MSG::DEBUG) << "procHistograms entered" << endmsg;

  //if (endOfLumiBlock || endOfRun) {
  //}

  return StatusCode::SUCCESS;
}
// ============================================================================
} // end namespace
// ============================================================================
