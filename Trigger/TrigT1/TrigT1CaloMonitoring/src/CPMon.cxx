/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     CPMon.cxx
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

#include "TrigT1CaloMonitoringTools/ITrigT1CaloMonErrorTool.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramTool.h"

// #include "TrigT1CaloEvent/CPMTower.h"
// #include "TrigT1CaloEvent/CMXCPTob.h"
// #include "TrigT1CaloEvent/CMXCPHits.h"
// ============================================================================
// L1 objects
// ============================================================================
#include "TrigT1CaloUtils/CoordToHardware.h"
#include "TrigT1CaloUtils/DataError.h"
#include "TrigT1CaloUtils/TriggerTowerKey.h"
#include "TrigT1Interfaces/CoordinateRange.h"
#include "TrigT1Interfaces/CPRoIDecoder.h"                          // <<== Will change
#include "TrigT1Interfaces/TrigT1CaloDefs.h"                        // Ditto
// ============================================================================
// xAOD
// ============================================================================
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "xAODTrigL1Calo/CPMTowerContainer.h"
#include "xAODTrigL1Calo/CMXCPTobContainer.h"
#include "xAODTrigL1Calo/CMXCPHitsContainer.h"
#include "xAODTrigL1Calo/CPMTobRoIContainer.h"
// ============================================================================


#include "CPMon.h"
// ============================================================================
namespace LVL1 {
// ============================================================================
const int CPMon::s_crates;
const int CPMon::s_modules;
const int CPMon::s_maxSlices;
const int CPMon::s_thresholds;
const int CPMon::s_threshBits;
const int CPMon::s_threshMask;
const int CPMon::s_tobsPerCPM;
const int CPMon::s_isolRange;
const int CPMon::s_isolBits;
// ============================================================================
CPMon::CPMon(const std::string & type, const std::string & name,
             const IInterface* parent)
  : ManagedMonitorToolBase(type, name, parent),
    m_errorTool("LVL1::TrigT1CaloMonErrorTool/TrigT1CaloMonErrorTool"),
    m_histTool("LVL1::TrigT1CaloLWHistogramTool/TrigT1CaloLWHistogramTool"),
    m_histBooked(false),
    m_h_cpm_2d_tt_Slices(0),
    m_h_cmx_2d_thresh_Slices(0),
    m_h_cpm_2d_tt_SliceMatch(0),
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
    m_h_cpm_1d_roi_EnergyEm(0),
    m_h_cpm_1d_roi_EnergyTau(0),
    m_h_cpm_1d_roi_IsolationEm(0),
    m_h_cpm_1d_roi_IsolationTau(0),
    m_h_cpm_1d_roi_IsolationBitsEm(0),
    m_h_cpm_1d_roi_IsolationBitsTau(0),
    m_h_cpm_1d_roi_TOBsPerCPMEm(0),
    m_h_cpm_1d_roi_TOBsPerCPMTau(0),
    m_h_cpm_2d_etaPhi_roi_HitmapEm(0),
    m_h_cpm_2d_etaPhi_roi_HitmapTau(0),
    m_h_cpm_2d_etaPhi_roi_EtWeightedEm(0),
    m_h_cpm_2d_etaPhi_roi_EtWeightedTau(0),
    m_h_cpm_2d_etaPhi_roi_HitmapIsolEm(0),
    m_h_cpm_2d_etaPhi_roi_HitmapIsolTau(0),
    m_h_cmx_1d_tob_EnergyLeft(0),
    m_h_cmx_1d_tob_EnergyRight(0),
    m_h_cmx_1d_tob_IsolationLeft(0),
    m_h_cmx_1d_tob_IsolationRight(0),
    m_h_cmx_1d_tob_IsolationBitsLeft(0),
    m_h_cmx_1d_tob_IsolationBitsRight(0),
    m_h_cmx_1d_tob_TOBsPerCPMLeft(0),
    m_h_cmx_1d_tob_TOBsPerCPMRight(0),
    m_h_cmx_1d_tob_TOBsPerCMXLeft(0),
    m_h_cmx_1d_tob_TOBsPerCMXRight(0),
    m_h_cmx_2d_tob_HitmapLeft(0),
    m_h_cmx_2d_tob_HitmapRight(0),
    m_h_cmx_2d_tob_HitmapIsolLeft(0),
    m_h_cmx_2d_tob_HitmapIsolRight(0),
    m_h_cmx_2d_tob_Overflow(0),
    m_h_cmx_2d_thresh_SumsWeightedLeft(0),
    m_h_cmx_2d_thresh_SumsWeightedRight(0),
    m_h_cmx_1d_topo_OutputChecksum(0),
    m_h_cmx_2d_topo_CPMOccupancyMap(0),
    m_h_cmx_2d_topo_CPMOccupancyCounts(0),
    m_h_cmx_1d_topo_TOBsPerCPMLeft(0),
    m_h_cmx_1d_topo_TOBsPerCPMRight(0),
    m_h_cmx_1d_topo_TOBsPerCMXLeft(0),
    m_h_cmx_1d_topo_TOBsPerCMXRight(0),
    m_h_cmx_2d_tob_Parity(0),
    m_h_cmx_2d_thresh_Parity(0),
    m_h_cmx_2d_Status(0),
    m_h_cpm_1d_ErrorSummary(0),
    m_h_cpm_2d_ErrorOverview(0),
    m_h_cpm_2d_ErrorEventNumbers(0)
    /*---------------------------------------------------------*/
{
  declareProperty("ErrorTool", m_errorTool);
  declareProperty("HistogramTool", m_histTool);
  declareProperty("BS_xAODTriggerTowerContainer",
                  //m_xAODTriggerTowerContainerName = "xAODTriggerTowers");
                  m_xAODTriggerTowerContainerName = LVL1::TrigT1CaloDefs::xAODTriggerTowerLocation);

  declareProperty("TriggerTowerLocation",  //not used anymore, has been replaced by xAODTriggerTower
                  m_triggerTowerLocation =
                    LVL1::TrigT1CaloDefs::TriggerTowerLocation);
  declareProperty("CPMTowerLocation",
                  m_cpmTowerLocation  = LVL1::TrigT1CaloDefs::CPMTowerLocation);
  declareProperty("CPMTowerLocationOverlap",
                  m_cpmTowerLocationOverlap =
                    LVL1::TrigT1CaloDefs::CPMTowerLocation + "Overlap");
  declareProperty("CMXCPTobLocation",
                  m_cmxCpTobLocation  = LVL1::TrigT1CaloDefs::CMXCPTobLocation);
  declareProperty("CMXCPHitsLocation",
                  m_cmxCpHitsLocation = LVL1::TrigT1CaloDefs::CMXCPHitsLocation);
  declareProperty("CPMTobRoILocation",
                  m_cpmTobRoiLocation = LVL1::TrigT1CaloDefs::CPMTobRoILocation);
  declareProperty("ErrorLocation",
                  m_errorLocation = "L1CaloCPMErrorVector");

  declareProperty("RootDirectory", m_rootDir = "L1Calo/CPM");
  declareProperty("MaxEnergyRange", m_maxEnergyRange = 256,
                  "Maximum energy plotted");
  declareProperty("MaxTOBsPerCMX", m_maxTobsPerCmx = 70,
                  "Maximum number of TOBs per CMX plotted");
}

/*---------------------------------------------------------*/
CPMon::~CPMon()
/*---------------------------------------------------------*/
{
}

/*---------------------------------------------------------*/
StatusCode CPMon:: initialize()
/*---------------------------------------------------------*/
{
  msg(MSG::INFO) << "Initializing " << name() << endmsg;

  StatusCode sc;

  sc = ManagedMonitorToolBase::initialize();
  if (sc.isFailure()) return sc;

  sc = m_errorTool.retrieve();
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloMonErrorTool"
                    << endmsg;
    return sc;
  }

  sc = m_histTool.retrieve();
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloLWHistogramTool"
                    << endmsg;
    return sc;
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode CPMon:: finalize()
/*---------------------------------------------------------*/
{
  return StatusCode::SUCCESS;
}


StatusCode CPMon::bookHistogramsRecurrent()
{
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in CPMMon::bookHistograms " << endmsg;

  if ( m_environment == AthenaMonManager::online ) {
    // book histograms that are only made in the online environment...
  }

  if ( m_dataType == AthenaMonManager::cosmics ) {
    // book histograms that are only relevant for cosmics data...
  }

  MgmtAttr_t attr = ATTRIB_UNMANAGED;

  if ( newRunFlag() ) {

    std::string errorDir(m_rootDir + "/Errors/Hardware");

    MonGroup monShift( this, errorDir, run, attr );
    MonGroup monExpert( this, errorDir, run, attr );
    MonGroup monDetail( this, errorDir + "/Detail", run, attr );
    MonGroup monEvents( this, errorDir + "/Detail", run, attr, "", "eventSample" );
    MonGroup monCPMin( this, m_rootDir + "/Input", run, attr );
    MonGroup monRoIs( this, m_rootDir + "/Output", run, attr );

    std::string cmxDir(m_rootDir + "_CMX");
    MonGroup monCMX( this, cmxDir + "/Errors/Hardware", run, attr );
    MonGroup monCMXin( this, cmxDir + "/Input", run, attr );
    MonGroup monCMXout( this, cmxDir + "/Output", run, attr );

    //  Timeslice checks

    m_histTool->setMonGroup(&monCPMin);

    const int xbins = s_crates * s_maxSlices;
    m_h_cpm_2d_tt_Slices = m_histTool->book2F("cpm_2d_tt_Slices",
                           "CPM Slices and Triggered Slice;Crate/Number of Slices;Triggered Slice",
                           xbins, 0, xbins, s_maxSlices, 0, s_maxSlices);
    m_histTool->numberPairs(m_h_cpm_2d_tt_Slices, 0, s_crates - 1, 1, s_maxSlices);
    m_histTool->numbers(m_h_cpm_2d_tt_Slices, 0, s_maxSlices - 1, 1, 0, false);
    m_h_cpm_2d_tt_SliceMatch = m_histTool->book2F("cpm_2d_tt_SliceMatch",
                               "PPM/CPM Tower Slice Match;Crate/CPM Slice;PPM Slice",
                               xbins, 0, xbins, s_maxSlices, 0, s_maxSlices);
    m_histTool->numberPairs(m_h_cpm_2d_tt_SliceMatch, 0, s_crates - 1, 0, s_maxSlices - 1);
    m_histTool->numbers(m_h_cpm_2d_tt_SliceMatch, 0, s_maxSlices - 1, 1, 0, false);

    m_histTool->setMonGroup(&monCMXout);

    m_h_cmx_2d_thresh_Slices = m_histTool->book2F("cmx_2d_thresh_Slices",
                               "CMX Slices and Triggered Slice;Crate/Number of Slices;Triggered Slice",
                               xbins, 0, xbins, s_maxSlices, 0, s_maxSlices);
    m_histTool->numberPairs(m_h_cmx_2d_thresh_Slices, 0, s_crates - 1, 1, s_maxSlices);
    m_histTool->numbers(m_h_cmx_2d_thresh_Slices, 0, s_maxSlices - 1, 1, 0, false);

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
                           "CPM Tower EM eta", 50, -2.5, 2.5);
    m_h_cpm_had_1d_tt_Eta = m_histTool->book1F("cpm_had_1d_tt_Eta",
                            "CPM Tower HAD eta", 50, -2.5, 2.5);
    m_h_cpm_em_1d_tt_Phi = m_histTool->book1F("cpm_em_1d_tt_Phi",
                           "CPM Tower EM phi", 64, 0., 2.*M_PI);
    m_h_cpm_had_1d_tt_Phi = m_histTool->book1F("cpm_had_1d_tt_Phi",
                            "CPM Tower HAD phi", 64, 0., 2.*M_PI);
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

    //  CPM TOB RoIs

    m_histTool->setMonGroup(&monRoIs);

    m_h_cpm_1d_roi_EnergyEm = m_histTool->book1F("cpm_1d_roi_EnergyEm",
                              "CPM TOB RoI Cluster Energy EM", m_maxEnergyRange, 0, m_maxEnergyRange);
    m_h_cpm_1d_roi_EnergyTau = m_histTool->book1F("cpm_1d_roi_EnergyTau",
                               "CPM TOB RoI Cluster Energy Tau", m_maxEnergyRange, 0, m_maxEnergyRange);
    m_h_cpm_1d_roi_IsolationEm = m_histTool->book1F("cpm_1d_roi_IsolationEm",
                                 "CPM TOB RoI Encoded Isolation Value EM", s_isolRange, 0, s_isolRange);
    m_h_cpm_1d_roi_IsolationTau = m_histTool->book1F("cpm_1d_roi_IsolationTau",
                                  "CPM TOB RoI Encoded Isolation Value Tau", s_isolRange, 0, s_isolRange);
    m_h_cpm_1d_roi_IsolationBitsEm = m_histTool->book1F("cpm_1d_roi_IsolationBitsEm",
                                     "CPM TOB RoI Encoded Isolation Bits EM;Bit", s_isolBits, 0, s_isolBits);
    m_histTool->numbers(m_h_cpm_1d_roi_IsolationBitsEm, 0, s_isolBits - 1);
    m_h_cpm_1d_roi_IsolationBitsTau = m_histTool->book1F("cpm_1d_roi_IsolationBitsTau",
                                      "CPM TOB RoI Encoded Isolation Bits Tau;Bit", s_isolBits, 0, s_isolBits);
    m_histTool->numbers(m_h_cpm_1d_roi_IsolationBitsTau, 0, s_isolBits - 1);
    m_h_cpm_1d_roi_TOBsPerCPMEm = m_histTool->book1F("cpm_1d_roi_TOBsPerCPMEm",
                                  "CPM TOB RoI TOBs per CPM EM;Number of TOBs", s_tobsPerCPM + 1, 1, s_tobsPerCPM + 2);
    m_histTool->numbers(m_h_cpm_1d_roi_TOBsPerCPMEm, 1, s_tobsPerCPM);
    m_h_cpm_1d_roi_TOBsPerCPMEm->GetXaxis()->SetBinLabel(s_tobsPerCPM + 1, "More");
    m_h_cpm_1d_roi_TOBsPerCPMTau = m_histTool->book1F("cpm_1d_roi_TOBsPerCPMTau",
                                   "CPM TOB RoI TOBs per CPM Tau;Number of TOBs", s_tobsPerCPM + 1, 1, s_tobsPerCPM + 2);
    m_histTool->numbers(m_h_cpm_1d_roi_TOBsPerCPMTau, 1, s_tobsPerCPM);
    m_h_cpm_1d_roi_TOBsPerCPMTau->GetXaxis()->SetBinLabel(s_tobsPerCPM + 1, "More");
    m_h_cpm_2d_etaPhi_roi_HitmapEm = m_histTool->bookCPMRoIEtaVsPhi(
                                       "cpm_2d_etaPhi_roi_HitmapEm", "CPM TOB RoIs EM Hit Map");
    m_h_cpm_2d_etaPhi_roi_HitmapTau = m_histTool->bookCPMRoIEtaVsPhi(
                                        "cpm_2d_etaPhi_roi_HitmapTau", "CPM TOB RoIs Tau Hit Map");
    m_h_cpm_2d_etaPhi_roi_EtWeightedEm = m_histTool->bookCPMRoIEtaVsPhi(
                                           "cpm_2d_etaPhi_roi_EtWeightedEm", "CPM TOB RoIs EM Weighted by Energy");
    m_h_cpm_2d_etaPhi_roi_EtWeightedTau = m_histTool->bookCPMRoIEtaVsPhi(
                                            "cpm_2d_etaPhi_roi_EtWeightedTau", "CPM TOB RoIs Tau Weighted by Energy");
    m_h_cpm_2d_etaPhi_roi_HitmapIsolEm = m_histTool->bookCPMRoIEtaVsPhi(
                                           "cpm_2d_etaPhi_roi_HitmapIsolEm", "CPM TOB RoIs EM Non-zero Isolation Hit Map");
    m_h_cpm_2d_etaPhi_roi_HitmapIsolTau = m_histTool->bookCPMRoIEtaVsPhi(
                                            "cpm_2d_etaPhi_roi_HitmapIsolTau", "CPM TOB RoIs Tau Non-zero Isolation Hit Map");

    //  CMX-CP TOBs

    m_histTool->setMonGroup(&monCMXin);

    m_h_cmx_1d_tob_EnergyLeft = m_histTool->book1F("cmx_1d_tob_EnergyLeft",
                                "CMX-CP TOBs Cluster Energy Left CMX", m_maxEnergyRange, 0, m_maxEnergyRange);
    m_h_cmx_1d_tob_EnergyRight = m_histTool->book1F("cmx_1d_tob_EnergyRight",
                                 "CMX-CP TOBs Cluster Energy Right CMX", m_maxEnergyRange, 0, m_maxEnergyRange);
    m_h_cmx_1d_tob_IsolationLeft = m_histTool->book1F("cmx_1d_tob_IsolationLeft",
                                   "CMX-CP TOBs Encoded Isolation Value Left CMX", s_isolRange, 0, s_isolRange);
    m_h_cmx_1d_tob_IsolationRight = m_histTool->book1F("cmx_1d_tob_IsolationRight",
                                    "CMX-CP TOBs Encoded Isolation Value Right CMX", s_isolRange, 0, s_isolRange);
    m_h_cmx_1d_tob_IsolationBitsLeft = m_histTool->book1F("cmx_1d_tob_IsolationBitsLeft",
                                       "CMX-CP TOBs Encoded Isolation Bits Left CMX;Bit", s_isolBits, 0, s_isolBits);
    m_histTool->numbers(m_h_cmx_1d_tob_IsolationBitsLeft, 0, s_isolBits - 1);
    m_h_cmx_1d_tob_IsolationBitsRight = m_histTool->book1F("cmx_1d_tob_IsolationBitsRight",
                                        "CMX-CP TOBs Encoded Isolation Bits Right CMX;Bit", s_isolBits, 0, s_isolBits);
    m_histTool->numbers(m_h_cmx_1d_tob_IsolationBitsRight, 0, s_isolBits - 1);
    m_h_cmx_1d_tob_TOBsPerCPMLeft = m_histTool->book1F("cmx_1d_tob_TOBsPerCPMLeft",
                                    "CMX-CP TOBs per CPM Left CMX;Number of TOBs", s_tobsPerCPM + 1, 1, s_tobsPerCPM + 2);
    m_histTool->numbers(m_h_cmx_1d_tob_TOBsPerCPMLeft, 1, s_tobsPerCPM);
    m_h_cmx_1d_tob_TOBsPerCPMLeft->GetXaxis()->SetBinLabel(s_tobsPerCPM + 1, "Overflow");
    m_h_cmx_1d_tob_TOBsPerCPMRight = m_histTool->book1F("cmx_1d_tob_TOBsPerCPMRight",
                                     "CMX-CP TOBs per CPM Right CMX;Number of TOBs", s_tobsPerCPM + 1, 1, s_tobsPerCPM + 2);
    m_histTool->numbers(m_h_cmx_1d_tob_TOBsPerCPMRight, 1, s_tobsPerCPM);
    m_h_cmx_1d_tob_TOBsPerCPMRight->GetXaxis()->SetBinLabel(s_tobsPerCPM + 1, "Overflow");
    m_h_cmx_1d_tob_TOBsPerCMXLeft = m_histTool->book1F("cmx_1d_tob_TOBsPerCMXLeft",
                                    "CMX-CP TOBs per CMX Left;Number of TOBs", m_maxTobsPerCmx, 0, m_maxTobsPerCmx);
    m_h_cmx_1d_tob_TOBsPerCMXRight = m_histTool->book1F("cmx_1d_tob_TOBsPerCMXRight",
                                     "CMX-CP TOBs per CMX Right;Number of TOBs", m_maxTobsPerCmx, 0, m_maxTobsPerCmx);
    m_h_cmx_2d_tob_HitmapLeft = m_histTool->bookCPMCrateModuleVsTobChipLocalCoord(
                                  "cmx_2d_tob_HitmapLeft", "CMX-CP TOBs Left CMX Hit Map");
    m_h_cmx_2d_tob_HitmapRight = m_histTool->bookCPMCrateModuleVsTobChipLocalCoord(
                                   "cmx_2d_tob_HitmapRight", "CMX-CP TOBs Right CMX Hit Map");
    m_h_cmx_2d_tob_HitmapIsolLeft = m_histTool->bookCPMCrateModuleVsTobChipLocalCoord(
                                      "cmx_2d_tob_HitmapIsolLeft", "CMX-CP TOBs Left CMX Non-zero Isolation Hit Map");
    m_h_cmx_2d_tob_HitmapIsolRight = m_histTool->bookCPMCrateModuleVsTobChipLocalCoord(
                                       "cmx_2d_tob_HitmapIsolRight", "CMX-CP TOBs Right CMX Non-zero Isolation Hit Map");
    m_h_cmx_2d_tob_Overflow = m_histTool->bookCPMCrateModuleVsCMX(
                                "cmx_2d_tob_Overflow", "CMX-CP TOBs Overflow");

    //  CMX-CP Hits

    m_histTool->setMonGroup(&monCMXout);

    m_h_cmx_2d_thresh_SumsWeightedLeft = m_histTool->bookCPMSumVsThreshold(
                                           "cmx_2d_thresh_SumsWeightedLeft", "CMX-CP Hit Sums Thresholds Weighted Left CMX", 0);
    m_h_cmx_2d_thresh_SumsWeightedRight = m_histTool->bookCPMSumVsThreshold(
                                            "cmx_2d_thresh_SumsWeightedRight", "CMX-CP Hit Sums Thresholds Weighted Right CMX", 1);
    m_h_cmx_1d_topo_OutputChecksum = m_histTool->book1F("cmx_1d_topo_OutputChecksum",
                                     "CMX-CP Topo Output Checksum Non-zero", 8, 0, 8);
    m_histTool->cpmCrateCMX(m_h_cmx_1d_topo_OutputChecksum);
    m_h_cmx_2d_topo_CPMOccupancyMap = m_histTool->bookCPMModuleVsCrateCMX(
                                        "cmx_2d_topo_CPMOccupancyMap", "CMX-CP Topo CPM Occupancy Maps");
    m_h_cmx_2d_topo_CPMOccupancyCounts = m_histTool->bookCPMModuleVsCrateCMX(
                                           "cmx_2d_topo_CPMOccupancyCounts", "CMX-CP Topo CPM Occupancy Counts Weighted");
    m_h_cmx_1d_topo_TOBsPerCPMLeft = m_histTool->book1F("cmx_1d_topo_TOBsPerCPMLeft",
                                     "CMX-CP Topo TOBs per CPM Left CMX;Number of TOBs", 7, 1, 8); // 3 bits
    m_histTool->numbers(m_h_cmx_1d_topo_TOBsPerCPMLeft, 1, s_tobsPerCPM);
    m_h_cmx_1d_topo_TOBsPerCPMRight = m_histTool->book1F("cmx_1d_topo_TOBsPerCPMRight",
                                      "CMX-CP Topo TOBs per CPM Right CMX;Number of TOBs", 7, 1, 8);
    m_histTool->numbers(m_h_cmx_1d_topo_TOBsPerCPMRight, 1, s_tobsPerCPM);
    m_h_cmx_1d_topo_TOBsPerCMXLeft = m_histTool->book1F("cmx_1d_topo_TOBsPerCMXLeft",
                                     "CMX-CP Topo TOBs per CMX Left;Number of TOBs", m_maxTobsPerCmx, 0., m_maxTobsPerCmx);
    m_h_cmx_1d_topo_TOBsPerCMXRight = m_histTool->book1F("cmx_1d_topo_TOBsPerCMXRight",
                                      "CMX-CP Topo TOBs per CMX Right;Number of TOBs", m_maxTobsPerCmx, 0., m_maxTobsPerCmx);

    //  CMX error bits

    m_histTool->setMonGroup(&monCMX);

    m_h_cmx_2d_tob_Parity = m_histTool->book2F("cmx_2d_tob_Parity",
                            "CMX-CP TOB Parity Errors;;CMX/Phase", 56, 0, 56, 10, 0, 10);
    m_histTool->cpmCrateModule(m_h_cmx_2d_tob_Parity);
    LWHist::LWHistAxis* axis = m_h_cmx_2d_tob_Parity->GetYaxis();
    axis->SetBinLabel(1, "0/Merge");
    axis->SetBinLabel(6, "1/Merge");
    m_histTool->numberPairs(m_h_cmx_2d_tob_Parity, 0, 0, 0, 3, 1, 1, false);
    m_histTool->numberPairs(m_h_cmx_2d_tob_Parity, 1, 1, 0, 3, 1, 6, false);

    m_h_cmx_2d_thresh_Parity = m_histTool->book2F("cmx_2d_thresh_Parity",
                               "CMX Remote Hit Sums Parity Errors;Remote Sum;CMX/Phase", 3, 0, 3, 4, 0, 4);
    axis = m_h_cmx_2d_thresh_Parity->GetXaxis();
    axis->SetBinLabel(1, "R0");
    axis->SetBinLabel(2, "R1");
    axis->SetBinLabel(3, "R2");
    m_histTool->numberPairs(m_h_cmx_2d_thresh_Parity, 0, 1, 0, 1, 1, 0, false);

    m_h_cmx_2d_Status = m_histTool->book2F("cmx_2d_Status",
                                           "CMX Sub-status bits;;Crate/CMX",
                                           8, 0., 8., 8, 0., 8.);
    m_histTool->subStatus(m_h_cmx_2d_Status);
    m_histTool->cpmCrateCMX(m_h_cmx_2d_Status, 0, false);

    //  Error Overview and Summary

    m_histTool->setMonGroup(&monExpert);

    m_h_cpm_2d_ErrorOverview = m_histTool->book2F("cpm_2d_ErrorOverview",
                               "CP Error Overview", 64, 0, 64,
                               NumberOfSummaryBins, 0, NumberOfSummaryBins);
    m_histTool->cpmCMXCrateModule(m_h_cpm_2d_ErrorOverview);

    m_histTool->setMonGroup(&monShift);

    m_h_cpm_1d_ErrorSummary = m_histTool->book1F("cpm_1d_ErrorSummary",
                              "CP Error Summary;;Events",
                              NumberOfSummaryBins, 0, NumberOfSummaryBins);

    m_histTool->setMonGroup(&monEvents);

    m_h_cpm_2d_ErrorEventNumbers = m_histTool->bookEventNumbers(
                                     "cpm_2d_ErrorEventNumbers",
                                     "CP Error Event Numbers",
                                     NumberOfSummaryBins, 0, NumberOfSummaryBins);

    LWHist* hist = m_h_cpm_2d_ErrorOverview;
    axis = hist->GetYaxis();
    for (int i = 0; i < 3; ++i) {
      axis->SetBinLabel(1 + EMParity,  "EM parity");
      axis->SetBinLabel(1 + EMLink,    "EM link");
      axis->SetBinLabel(1 + HadParity, "Had parity");
      axis->SetBinLabel(1 + HadLink,   "Had link");
      axis->SetBinLabel(1 + CPMStatus, "CPM status");
      axis->SetBinLabel(1 + TOBParity, "TOB parity");
      axis->SetBinLabel(1 + SumParity, "Sum parity");
      axis->SetBinLabel(1 + CMXStatus, "CMX status");
      if (i == 0) {
        hist = m_h_cpm_1d_ErrorSummary;
        axis = hist->GetXaxis();
      } else {
        hist = m_h_cpm_2d_ErrorEventNumbers;
        axis = hist->GetYaxis();
      }
    }

    m_histTool->unsetMonGroup();

    m_histBooked = true;

  } // end if (isNewRun ...

  msg(MSG::DEBUG) << "Leaving bookHistograms" << endmsg;

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode CPMon::fillHistograms()
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
  const xAOD::TriggerTowerContainer* triggerTowerTES = 0;
  StatusCode sc = evtStore()->retrieve(triggerTowerTES,
                                       m_xAODTriggerTowerContainerName);

  if ( sc.isFailure()  ||  !triggerTowerTES ) {
    if (debug) msg(MSG::DEBUG) << "No Trigger Tower container found in TES " << m_xAODTriggerTowerContainerName << endmsg;
  }

  //Retrieve Core CPM Towers from SG
  const xAOD::CPMTowerContainer* cpmTowerTES = 0;
  sc = evtStore()->retrieve(cpmTowerTES, m_cpmTowerLocation);
  if ( sc.isFailure()  ||  !cpmTowerTES ) {
    msg(MSG::DEBUG) << "No Core CPM Tower container found" << endmsg;
  }

  //Retrieve Overlap CPM Towers from SG
  const xAOD::CPMTowerContainer* cpmTowerOverlapTES = 0;
  if (evtStore()->contains<xAOD::CPMTowerContainer>(m_cpmTowerLocationOverlap)) {
    sc = evtStore()->retrieve(cpmTowerOverlapTES, m_cpmTowerLocationOverlap);
  } else sc = StatusCode::FAILURE;
  if ( sc.isFailure()  ||  !cpmTowerOverlapTES ) {
    msg(MSG::DEBUG) << "No Overlap CPM Tower container found" << endmsg;
  }

  //Retrieve CPM TOB RoIs from SG
  const xAOD::CPMTobRoIContainer* cpmTobRoiTES = 0;
  sc = evtStore()->retrieve( cpmTobRoiTES, m_cpmTobRoiLocation);
  if ( sc.isFailure()  ||  !cpmTobRoiTES ) {
    msg(MSG::DEBUG) << "No CPM TOB RoIs container found" << endmsg;
  }

  //Retrieve CMX-CP TOBs from SG
  const xAOD::CMXCPTobContainer* cmxCpTobTES = 0;
  sc = evtStore()->retrieve( cmxCpTobTES, m_cmxCpTobLocation);
  if ( sc.isFailure()  ||  !cmxCpTobTES ) {
    msg(MSG::DEBUG) << "No CMX-CP TOBs container found" << endmsg;
  }

  //Retrieve CMX-CP Hits from SG
  const xAOD::CMXCPHitsContainer* cmxCpHitsTES = 0;
  sc = evtStore()->retrieve( cmxCpHitsTES, m_cmxCpHitsLocation);
  if ( sc.isFailure()  ||  !cmxCpHitsTES ) {
    msg(MSG::DEBUG) << "No CMX-CP Hits container found" << endmsg;
  }

  // Vectors for error overview bits;
  std::vector<int> errorsCPM(s_crates * s_modules);
  std::vector<int> errorsCMX(s_crates * 2); // L/R

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
    xAOD::TriggerTowerContainer::const_iterator ttIterator =
      triggerTowerTES->begin();
    xAOD::TriggerTowerContainer::const_iterator ttIteratorEnd =
      triggerTowerTES->end();
    for (; ttIterator != ttIteratorEnd; ++ttIterator) {

      const int layer = (*ttIterator)->layer();
      const xAOD::TriggerTower_v2* tt = *ttIterator; 
      const double eta = (*ttIterator)->eta();
      if (eta < -2.5 || eta > 2.5) continue;
      const double phi = (*ttIterator)->phi();
      
      if (!tt->cpET()) continue; 
      //check if the TriggerTower is in EM or HAD layer
      if (layer == 0) { //EM
        const int em = int(tt->cpET()); 
        if (em) m_histTool->fillCPMEtaVsPhi(m_h_ppm_em_2d_etaPhi_tt_Hitmap, eta, phi); 
      }
      if (layer == 1) { //HAD
        const int had = int(tt->cpET()); 
        if (had)  m_histTool->fillCPMEtaVsPhi(m_h_ppm_had_2d_etaPhi_tt_Hitmap, eta, phi); 
      }
      
      //if (std::accumulate(emLut.begin(), emLut.end(), 0) == 0 &&
      //  std::accumulate(hadLut.begin(), hadLut.end(), 0) == 0) continue;

      // will see later what to do with the next 3 lines @@vkousk
      const unsigned int key = towerKey.ttKey(phi, eta);
      if (key > maxKey) maxKey = key;
      ttMap.insert(std::make_pair(key, *ttIterator));
    } //ttIterator
  } //if (triggerTowerTES)


  for (int i = 0; i < 2; ++i) {
    const bool core = (i == 0);
    const xAOD::CPMTowerContainer* cpmTwrTES = (core) ? cpmTowerTES
                                          : cpmTowerOverlapTES;
    if (cpmTwrTES) {
      xAOD::CPMTowerContainer::const_iterator ctIterator    = cpmTwrTES->begin();
      xAOD::CPMTowerContainer::const_iterator ctIteratorEnd = cpmTwrTES->end();

      for (; ctIterator != ctIteratorEnd; ++ctIterator) {
        const xAOD::CPMTower* ct = *ctIterator;
        const uint8_t    em  = ct->emEnergy();
        const uint8_t    had = ct->hadEnergy();
        const double eta = ct->eta();
        const double phi = ct->phi();
        //std::cout<<"em: " << em << " had: " << had << " eta: " << eta << " phi: " << phi << std::endl; //@@
        const LVL1::Coordinate coord(phi, eta);
        LVL1::CoordToHardware converter;
        const int crate  = (core) ? converter.cpCrate(coord)
                           : converter.cpCrateOverlap(coord);
        const int cpm    = (core) ? converter.cpModule(coord)
                           : converter.cpModuleOverlap(coord);
        const int loc    = crate * s_modules + cpm - 1;
        const int peak   = ct->peak();
        const int slices = (ct->emEnergyVec()).size();
        m_h_cpm_2d_tt_Slices->Fill(crate * s_maxSlices + slices - 1, peak, 1.);
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
        uint32_t error = ct->emError();
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

  /*
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
     //const LVL1::TriggerTower* tt = ttMapIter->second;
     const xAOD::TriggerTower_v2* tt = ttMapIter->second;
     const LVL1::CPMTower*     cp = cpMapIter->second;
     const int layer = tt->layer();
     std::vector<uint_least8_t> emLut(0);
     if (layer == 0)
  emLut = tt->lut_cp();
     std::vector<uint_least8_t> hadLut(0);
     if (layer == 1)
  hadLut = tt->lut_cp();

     const std::vector<int> emVec(0);
     emVec = cp->emEnergyVec();
     //const std::vector<uint_least8_t> hadVec(0);
     const std::vector<int> hadVec(0);
     hadVec = cp->hadEnergyVec();

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

  */


  //=============================================
  //  CPM TOB RoIs
  //=============================================

  const int vecSize = s_crates * s_modules * 2;
  std::vector<int> tobCount(vecSize);
  if (cpmTobRoiTES) {
    LVL1::CPRoIDecoder decoder;                                             //<<== Will change
    xAOD::CPMTobRoIContainer::const_iterator crIterator    = cpmTobRoiTES->begin();
    xAOD::CPMTobRoIContainer::const_iterator crIteratorEnd = cpmTobRoiTES->end();
    for (; crIterator != crIteratorEnd; ++crIterator) {
      const int type      = (*crIterator)->type();  // 0=EM, 1=Tau
      const int energy    = (*crIterator)->energy();
      const int isolation = (*crIterator)->isolation();
      const LVL1::CoordinateRange coord(decoder.coordinate((*crIterator)->roiWord())); 
      const double eta = coord.eta();
      const double phi = coord.phi();
      if (energy) {
        TH1F_LW* hist1 = (type) ? m_h_cpm_1d_roi_EnergyTau
                         : m_h_cpm_1d_roi_EnergyEm;
        TH2F_LW* hist2 = (type) ? m_h_cpm_2d_etaPhi_roi_HitmapTau
                         : m_h_cpm_2d_etaPhi_roi_HitmapEm;
        TH2F_LW* hist3 = (type) ? m_h_cpm_2d_etaPhi_roi_EtWeightedTau
                         : m_h_cpm_2d_etaPhi_roi_EtWeightedEm;
        hist1->Fill(energy);
        m_histTool->fillCPMRoIEtaVsPhi(hist2, eta, phi);
        m_histTool->fillCPMRoIEtaVsPhi(hist3, eta, phi, energy);
      }
      if (isolation) {
        TH1F_LW* hist1 = (type) ? m_h_cpm_1d_roi_IsolationTau
                         : m_h_cpm_1d_roi_IsolationEm;
        TH1F_LW* hist2 = (type) ? m_h_cpm_1d_roi_IsolationBitsTau
                         : m_h_cpm_1d_roi_IsolationBitsEm;
        TH2F_LW* hist3 = (type) ? m_h_cpm_2d_etaPhi_roi_HitmapIsolTau
                         : m_h_cpm_2d_etaPhi_roi_HitmapIsolEm;
        hist1->Fill(isolation);
        m_histTool->fillThresholds(hist2, isolation, s_isolBits, 1);
        m_histTool->fillCPMRoIEtaVsPhi(hist3, eta, phi);
      }
      const int crate = (*crIterator)->crate();
      const int cpm   = (*crIterator)->cpm();
      ++tobCount[(crate * s_modules + cpm - 1) * 2 + type];
    }
    for (int crate = 0; crate < s_crates; ++crate) {
      for (int cpm = 1; cpm <= s_modules; ++cpm) {
        for (int type = 0; type < 2; ++type) {
          int val = tobCount[(crate * s_modules + cpm - 1) * 2 + type];
          if (val) {
            if (val > s_tobsPerCPM) val = s_tobsPerCPM + 1;
            if (type == 0) m_h_cpm_1d_roi_TOBsPerCPMEm->Fill(val);
            else           m_h_cpm_1d_roi_TOBsPerCPMTau->Fill(val);
          }
        }
      }
    }
  }


  //=============================================
  //  CMX-CP TOBs
  //=============================================

  tobCount.assign(vecSize, 0);
  std::vector<int> cmxCount(s_crates * 2);
  if (cmxCpTobTES) {
    xAOD::CMXCPTobContainer::const_iterator crIterator    = cmxCpTobTES->begin();
    xAOD::CMXCPTobContainer::const_iterator crIteratorEnd = cmxCpTobTES->end();
    for (; crIterator != crIteratorEnd; ++crIterator) {
      const uint8_t crate     = (*crIterator)->crate();
      const uint8_t cpm       = (*crIterator)->cpm();     // 1-14
      const uint8_t cmx       = (*crIterator)->cmx();     // 0=Left, 1=Right  (Assumed in Sim to be Left Tau, Right EM)
      const uint8_t chip      = (*crIterator)->chip();    // 4 bits
      const uint8_t location  = (*crIterator)->location();// 2 bits
      const uint8_t energy    = (*crIterator)->energy();
      const uint8_t isolation = (*crIterator)->isolation();
      const uint32_t error     = (*crIterator)->error();
      const uint8_t x = crate * s_modules + cpm - 1;
      const uint8_t y = chip * 4 + location;
      if (energy) {
        TH1F_LW* hist1 = (cmx) ? m_h_cmx_1d_tob_EnergyRight
                         : m_h_cmx_1d_tob_EnergyLeft;
        TH2F_LW* hist2 = (cmx) ? m_h_cmx_2d_tob_HitmapRight
                         : m_h_cmx_2d_tob_HitmapLeft;
        hist1->Fill(energy);
        hist2->Fill(x, y);
      }
      if (isolation) {
        TH1F_LW* hist1 = (cmx) ? m_h_cmx_1d_tob_IsolationRight
                         : m_h_cmx_1d_tob_IsolationLeft;
        TH1F_LW* hist2 = (cmx) ? m_h_cmx_1d_tob_IsolationBitsRight
                         : m_h_cmx_1d_tob_IsolationBitsLeft;
        TH2F_LW* hist3 = (cmx) ? m_h_cmx_2d_tob_HitmapIsolRight
                         : m_h_cmx_2d_tob_HitmapIsolLeft;
        hist1->Fill(isolation);
        m_histTool->fillThresholds(hist2, isolation, s_isolBits, 1);
        hist3->Fill(x, y);
      }
      if (error) {
        const LVL1::DataError err(error);
        if (err.get(LVL1::DataError::Overflow)) {
          m_h_cmx_2d_tob_Overflow->Fill(x, cmx);
          tobCount[x * 2 + cmx] = s_tobsPerCPM + 1;
        }
        const int ybase = cmx * 5;
        bool parity = false;
        if (err.get(LVL1::DataError::ParityMerge)) {
          m_h_cmx_2d_tob_Parity->Fill(x, ybase);
          parity = true;
        }
        if (err.get(LVL1::DataError::ParityPhase0)) {
          m_h_cmx_2d_tob_Parity->Fill(x, ybase + 1);
          parity = true;
        }
        if (err.get(LVL1::DataError::ParityPhase1)) {
          m_h_cmx_2d_tob_Parity->Fill(x, ybase + 2);
          parity = true;
        }
        if (err.get(LVL1::DataError::ParityPhase2)) {
          m_h_cmx_2d_tob_Parity->Fill(x, ybase + 3);
          parity = true;
        }
        if (err.get(LVL1::DataError::ParityPhase3)) {
          m_h_cmx_2d_tob_Parity->Fill(x, ybase + 4);
          parity = true;
        }
        if (parity) errorsCMX[crate * 2 + cmx] |= (1 << TOBParity);
      }
      if (energy || isolation || error) {
        ++tobCount[x * 2 + cmx];
        ++cmxCount[crate * 2 + cmx];
      }
    }
    for (int crate = 0; crate < s_crates; ++crate) {
      for (int cpm = 1; cpm <= s_modules; ++cpm) {
        for (int cmx = 0; cmx < 2; ++cmx) {
          int val = tobCount[(crate * s_modules + cpm - 1) * 2 + cmx];
          if (val) {
            if (val > s_tobsPerCPM) val = s_tobsPerCPM + 1;
            if (cmx == 0) m_h_cmx_1d_tob_TOBsPerCPMLeft->Fill(val);
            else          m_h_cmx_1d_tob_TOBsPerCPMRight->Fill(val);
          }
        }
      }
      for (int cmx = 0; cmx < 2; ++cmx) {
        int val = cmxCount[crate * 2 + cmx];
        if (val) {
          if (val >= m_maxTobsPerCmx) val = m_maxTobsPerCmx - 1;
          if (cmx == 0) m_h_cmx_1d_tob_TOBsPerCMXLeft->Fill(val);
          else          m_h_cmx_1d_tob_TOBsPerCMXRight->Fill(val);
        }
      }
    }
  }


  //=============================================
  //  CMX-CP Hits
  //=============================================

  cmxCount.assign(s_crates * 2, 0);
  if (cmxCpHitsTES) {
    xAOD::CMXCPHitsContainer::const_iterator cmIterator    = cmxCpHitsTES->begin();
    xAOD::CMXCPHitsContainer::const_iterator cmIteratorEnd = cmxCpHitsTES->end();
    for (; cmIterator != cmIteratorEnd; ++cmIterator) {
      const uint32_t hits0 = (*cmIterator)->hits0();
      const uint32_t hits1 = (*cmIterator)->hits1();
      const uint8_t crate  = (*cmIterator)->crate();
      const uint8_t cmx    = (*cmIterator)->cmx();
      const uint8_t source = (*cmIterator)->sourceComponent();
      const uint8_t peak   = (*cmIterator)->peak();
      const uint8_t slices = ((*cmIterator)->hitsVec0()).size();
      const uint8_t crateCmx = crate * 2 + cmx;
      m_h_cmx_2d_thresh_Slices->Fill(crate * s_maxSlices + slices - 1, peak, 1.);
      if (source == xAOD::CMXCPHits::TOPO_CHECKSUM) {
        if (hits0) m_h_cmx_1d_topo_OutputChecksum->Fill(crateCmx);
      } else if (source == xAOD::CMXCPHits::TOPO_OCCUPANCY_MAP) {
        if (hits0) {
          m_histTool->fillThresholdsVsY(m_h_cmx_2d_topo_CPMOccupancyMap,
                                        hits0, crateCmx, s_modules, 1, 1);
        }
      } else if (source == xAOD::CMXCPHits::TOPO_OCCUPANCY_COUNTS) {
        TH1F_LW* hist = (cmx) ? m_h_cmx_1d_topo_TOBsPerCPMRight
                        : m_h_cmx_1d_topo_TOBsPerCPMLeft;
        if (hits0) {
          m_histTool->fillThresholdsVsY(m_h_cmx_2d_topo_CPMOccupancyCounts,
                                        hits0, crateCmx, s_modules / 2, 3, 1);
          for (int mod = 0; mod < s_modules / 2; ++mod) {
            const int val = (hits0 >> (mod * 3)) & 0x7;
            if (val) hist->Fill(val);
            cmxCount[crate * 2 + cmx] += val;
          }
        }
        if (hits1) {
          m_histTool->fillThresholdsVsY(m_h_cmx_2d_topo_CPMOccupancyCounts,
                                        hits1, crateCmx, s_modules / 2, 3, s_modules / 2 + 1);
          for (int mod = 0; mod < s_modules / 2; ++mod) {
            const int val = (hits1 >> (mod * 3)) & 0x7;
            if (val) hist->Fill(val);
            cmxCount[crate * 2 + cmx] += val;
          }
        }
      } else {
        int bin = 0;
        if      (source == xAOD::CMXCPHits::LOCAL)    bin = crate;
        else if (source == xAOD::CMXCPHits::REMOTE_0) bin = s_crates;
        else if (source == xAOD::CMXCPHits::REMOTE_1) bin = s_crates + 1;
        else if (source == xAOD::CMXCPHits::REMOTE_2) bin = s_crates + 2;
        else if (source == xAOD::CMXCPHits::TOTAL)    bin = s_crates + 3;
        TH2F_LW* hist = (cmx) ? m_h_cmx_2d_thresh_SumsWeightedRight
                        : m_h_cmx_2d_thresh_SumsWeightedLeft;
        const int nthresh = s_thresholds / 2;
        if (hits0) {
          m_histTool->fillXVsThresholds(hist, bin, hits0, nthresh,
                                        s_threshBits);
        }
        if (hits1) {
          m_histTool->fillXVsThresholds(hist, bin, hits1, nthresh,
                                        s_threshBits, nthresh);
        }
      }
      // Errors
      /*
      const int error0 = (*cmIterator)->error0();
      const int error1 = (*cmIterator)->error1();
      if (error0 || error1) {
        const LVL1::DataError hit0Err(error0);
        const LVL1::DataError hit1Err(error1);
      const int parity0 = hit0Err.get(LVL1::DataError::Parity);
      const int parity1 = hit1Err.get(LVL1::DataError::Parity);
      if (parity0 || parity1) {
      const int remBin = 2*cmx;
      int remCrate = -1;
          if      (source == LVL1::CMXCPHits::REMOTE_0) remCrate = 0;
          else if (source == LVL1::CMXCPHits::REMOTE_1) remCrate = 1;
          else if (source == LVL1::CMXCPHits::REMOTE_2) remCrate = 2;
      if (remCrate >= 0) {
      if (parity0) m_h_cmx_2d_thresh_Parity->Fill(remCrate, remBin);
      if (parity1) m_h_cmx_2d_thresh_Parity->Fill(remCrate, remBin + 1);
      errorsCMX[crate*2 + cmx] |= (1 << SumParity);
          }
        }
        // Sub-status errors
        const int status = error0 >> LVL1::DataError::GLinkParity;
      if (status) {
          for (int bit = 0; bit < 8; ++bit) {
      if ((status >> bit) & 0x1) m_h_cmx_2d_Status->Fill(bit, 2*crate+cmx);
      }
      if (status) errorsCMX[crate*2 + cmx] |= (1 << CMXStatus);
        }
      } //if error0 || error1
      */
    } //CmxCpHitsCollection iterator
  }  //cmxCpHitsTES
  for (int crate = 0; crate < s_crates; ++crate) {
    for (int cmx = 0; cmx < 2; ++cmx) {
      int val = cmxCount[crate * 2 + cmx];
      if (val) {
        if (val >= m_maxTobsPerCmx) val = m_maxTobsPerCmx - 1;
        if (cmx == 0) m_h_cmx_1d_topo_TOBsPerCMXLeft->Fill(val);
        else          m_h_cmx_1d_topo_TOBsPerCMXRight->Fill(val);
      }
    }
  }

  // Update error summary plot

  std::vector<int> crateErr(4);
  for (int err = 0; err < NumberOfSummaryBins; ++err) {
    int error = 0;
    for (int loc = 0; loc < s_crates * s_modules; ++loc) {
      if ((errorsCPM[loc] >> err) & 0x1) {
        m_h_cpm_2d_ErrorOverview->Fill(loc, err, 1.);
        error = 1;
        crateErr[loc / s_modules] |= (1 << err);
      }
      if (loc < s_crates * 2) {
        if ((errorsCMX[loc] >> err) & 0x1) {
          m_h_cpm_2d_ErrorOverview->Fill(loc + s_crates * s_modules, err, 1.);
          error = 1;
          crateErr[loc / 2] |= (1 << err);
        }
      }
    }
    if (error) {
      m_h_cpm_1d_ErrorSummary->Fill(err);
      m_histTool->fillEventNumber(m_h_cpm_2d_ErrorEventNumbers, err);
    }
  }


  // Save error vector for global summary

  std::vector<int>* save = new std::vector<int>(crateErr);
  sc = evtStore()->record(save, m_errorLocation);
  if (sc != StatusCode::SUCCESS) {
    msg(MSG::ERROR) << "Error recording CPM error vector in TES " << endmsg;
    return sc;
  }

  msg(MSG::DEBUG) << "Leaving fillHistograms" << endmsg;

  return StatusCode::SUCCESS;

}

/*---------------------------------------------------------*/
//StatusCode CPMon::procHistograms()
/*---------------------------------------------------------*/
StatusCode CPMon::procHistograms() {
  msg(MSG::DEBUG) << "procHistograms entered" << endmsg;

  //if ( endOfLumiBlock || endOfRun) {
  // }

  return StatusCode::SUCCESS;
}
// ============================================================================
} // end namespace
// ============================================================================
