/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:        JEPCMXMon.cxx
// PACKAGE:     TrigT1CaloMonitoring
//
// DESCRIPTION: Monitoring of the JEP on CMX level
//
// ********************************************************************

#include <cmath>
#include <vector>

#include "LWHists/LWHist.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH2I_LW.h"
#include "LWHists/TProfile2D_LW.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "SGTools/StlVectorClids.h"

#include "AthenaMonitoring/AthenaMonManager.h"

#include "TrigT1CaloMonitoringTools/ITrigT1CaloMonErrorTool.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramTool.h"

#include "xAODTrigL1Calo/CMXJetTobContainer.h"
#include "xAODTrigL1Calo/CMXJetHitsContainer.h"
#include "xAODTrigL1Calo/CMXEtSumsContainer.h"
#include "xAODTrigL1Calo/CMXRoIContainer.h"
#include "TrigT1CaloUtils/DataError.h"
#include "TrigT1CaloUtils/CrateEnergy.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

#include "JEPCMXMon.h"
// ============================================================================
namespace LVL1 {
// ============================================================================
const int JEPCMXMon::s_crates;
const int JEPCMXMon::s_modules;
const int JEPCMXMon::s_tobsPerJEM;
// ============================================================================
static const InterfaceID IID_IJEPCMXMon("LVL1::JEPCMXMon", 1, 1);

const InterfaceID& JEPCMXMon::interfaceID() {
  return IID_IJEPCMXMon;
}
// ============================================================================

// *********************************************************************
// Public Methods
// *********************************************************************

/*---------------------------------------------------------*/
JEPCMXMon::JEPCMXMon( const std::string & type, const std::string & name,
                      const IInterface* parent )
  : ManagedMonitorToolBase( type, name, parent ),
    m_errorTool("LVL1::TrigT1CaloMonErrorTool/TrigT1CaloMonErrorTool"),
    m_histTool("LVL1::TrigT1CaloLWHistogramTool/TrigT1CaloLWHistogramTool"),
    m_histBooked(false),
    m_h_cmx_1d_tob_EnergyLg(0),
    m_h_cmx_1d_tob_EnergySm(0),
    m_h_cmx_1d_tob_TOBsPerJEM(0),
    m_h_cmx_1d_tob_TOBsPerCMX(0),
    m_h_cmx_2d_tob_Hitmap(0),
    m_h_cmx_1d_thresh_TotalMainHits(0),
    //m_h_cmx_1d_thresh_TotalFwdHits(0),
    m_h_cmx_1d_thresh_RoIOverflow(0),
    m_h_cmx_1d_topo_DataChecksum(0),
    m_h_cmx_2d_topo_JEMOccupancyMap(0),
    m_h_cmx_2d_topo_JEMOccupancyCounts(0),
    m_h_cmx_1d_topo_TOBsPerJEM(0),
    m_h_cmx_1d_topo_TOBsPerCMX(0),
    m_h_cmx_1d_energy_SubSumsEx(0),
    m_h_cmx_1d_energy_SubSumsEy(0),
    m_h_cmx_1d_energy_SubSumsEt(0),
    m_h_cmx_1d_energy_TotalEx(0),
    m_h_cmx_1d_energy_TotalEy(0),
    m_h_cmx_1d_energy_TotalEt(0),
    m_h_cmx_1d_energy_MissingEtHits(0),
    m_h_cmx_1d_energy_SumEtHits(0),
    m_h_cmx_1d_energy_MissingEtSigHits(0),
    m_h_cmx_2d_energy_Overflow(0),
    m_h_cmx_1d_energy_TotalExRes(0),
    m_h_cmx_1d_energy_TotalEyRes(0),
    m_h_cmx_1d_energy_TotalEtRes(0),
    m_h_cmx_1d_energy_MissingEtHitsRes(0),
    m_h_cmx_1d_energy_SumEtHitsWt(0),
    m_h_cmx_2d_energy_OverflowRes(0),
    m_h_cmx_1d_roi_SumEtHits(0),
    m_h_cmx_1d_roi_MissingEtHits(0),
    m_h_cmx_1d_roi_MissingEtSigHits(0),
    m_h_cmx_1d_roi_SumEtHitsMask(0),
    m_h_cmx_1d_roi_MissingEtHitsMask(0),
    m_h_cmx_1d_roi_Ex(0),
    m_h_cmx_1d_roi_Ey(0),
    m_h_cmx_1d_roi_Et(0),
    m_h_cmx_1d_roi_ExMask(0),
    m_h_cmx_1d_roi_EyMask(0),
    m_h_cmx_1d_roi_EtMask(0),
    m_h_cmx_2d_thresh_Status(0),
    m_h_cmx_2d_energy_Status(0),
    m_h_cmx_2d_thresh_ParityTob(0),
    m_h_cmx_1d_thresh_ParityRem(0),
    m_h_cmx_2d_energy_ParityMod(0),
    m_h_cmx_1d_energy_ParityRem(0),
    m_h_cmx_1d_ErrorSummary(0),
    m_h_cmx_1d_TriggeredSlices(0),
    m_h_cmx_2d_ErrorEventNumbers(0)
    /*---------------------------------------------------------*/
{
  // This is how you declare the parameters to Gaudi so that
  // they can be over-written via the job options file

  declareProperty("ErrorTool", m_errorTool);
  declareProperty("HistogramTool", m_histTool);

  declareProperty("CMXJetTobLocation",
                  m_CMXJetTobLocation  = LVL1::TrigT1CaloDefs::CMXJetTobLocation);
  declareProperty("CMXJetHitsLocation",
                  m_CMXJetHitsLocation = LVL1::TrigT1CaloDefs::CMXJetHitsLocation);
  declareProperty("CMXEtSumsLocation",
                  m_CMXEtSumsLocation  = LVL1::TrigT1CaloDefs::CMXEtSumsLocation);
  declareProperty("CMXRoILocation",
                  m_CMXRoILocation     = LVL1::TrigT1CaloDefs::CMXRoILocation);
  declareProperty("ErrorLocation",
                  m_errorLocation      = "L1CaloJEMCMXErrorVector");

  declareProperty("RootDirectory", m_rootDir = "L1Calo/JEM_CMX");
  declareProperty("MaxTOBsPerCMX", m_maxTobsPerCmx = 64,
                  "Maximum number of TOBs per CMX plotted");
}

/*---------------------------------------------------------*/
JEPCMXMon::~JEPCMXMon()
/*---------------------------------------------------------*/
{
}

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

/*---------------------------------------------------------*/
StatusCode JEPCMXMon::initialize()
/*---------------------------------------------------------*/
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << PACKAGE_VERSION << endmsg;

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
StatusCode JEPCMXMon::bookHistogramsRecurrent()
/*---------------------------------------------------------*/
{
  msg(MSG::DEBUG) << "in JEPCMXMon::bookHistogramsRecurrent" << endmsg;

  if ( m_environment == AthenaMonManager::online ) {
    // book histograms that are only made in the online environment...
  }

  if ( m_dataType == AthenaMonManager::cosmics ) {
    // book histograms that are only relevant for cosmics data...
  }

  //if ( newEventsBlock || newLumiBlock) { }

  if ( newRun ) {

    MgmtAttr_t attr = ATTRIB_UNMANAGED;
    MonGroup CMX_inputJet( this, m_rootDir + "/Input/Jet", run, attr );
    MonGroup CMX_inputEnergy( this, m_rootDir + "/Input/Energy", run, attr );
    MonGroup CMX_jet( this, m_rootDir + "/Output/Jet", run, attr );
    MonGroup CMX_energy( this, m_rootDir + "/Output/Energy", run, attr );
    MonGroup CMX_RoI( this, m_rootDir + "/Output/RoI", run, attr );
    std::string errorDir(m_rootDir + "/Errors/Hardware");
    MonGroup CMX_error( this, errorDir, run, attr );
    MonGroup CMX_errorDetail( this, errorDir, run, attr );
    MonGroup CMX_errorEvents( this, errorDir, run, attr, "", "eventSample" );

    m_histTool->setMonGroup(&CMX_inputJet);

    const double maxLarge = 1024.;
    const double maxSmall = 512.;
    const int nbins = 256;
    m_h_cmx_1d_tob_EnergyLg = m_histTool->book1F("cmx_1d_tob_EnergyLg",
                              "CMX-Jet TOB Energy Large Window Size", nbins, 0., maxLarge);
    m_h_cmx_1d_tob_EnergySm = m_histTool->book1F("cmx_1d_tob_EnergySm",
                              "CMX-Jet TOB Energy Small Window Size", nbins, 0., maxSmall);
    m_h_cmx_1d_tob_TOBsPerJEM = m_histTool->book1F("cmx_1d_tob_TOBsPerJEM",
                                "CMX-Jet TOBs per JEM;Number of TOBs", s_tobsPerJEM + 1, 1., s_tobsPerJEM + 2);
    m_histTool->numbers(m_h_cmx_1d_tob_TOBsPerJEM, 1, s_tobsPerJEM);
    m_h_cmx_1d_tob_TOBsPerCMX = m_histTool->book1F("cmx_1d_tob_TOBsPerCMX",
                                "CMX-Jet TOBs per CMX;Number of TOBs", m_maxTobsPerCmx, 0., m_maxTobsPerCmx);
    m_h_cmx_2d_tob_Hitmap = m_histTool->bookJEMCrateModuleVsFrameLoc(
                              "cmx_2d_tob_Hitmap", "CMX-Jet TOBs Hit Map");

    m_histTool->setMonGroup(&CMX_inputEnergy);

    const double maxSum = 16384.;
    m_h_cmx_1d_energy_SubSumsEx = m_histTool->book1F(
                                    "cmx_1d_energy_SubSumsEx", "CMX E_{x}^{JEM};Ex [GeV]", nbins, 0., maxSum);
    m_h_cmx_1d_energy_SubSumsEy = m_histTool->book1F(
                                    "cmx_1d_energy_SubSumsEy", "CMX E_{y}^{JEM};Ey [GeV]", nbins, 0., maxSum);
    m_h_cmx_1d_energy_SubSumsEt = m_histTool->book1F(
                                    "cmx_1d_energy_SubSumsEt", "CMX E_{t}^{JEM};Et [GeV]", nbins, 0., maxSum);

    //-------------------------- CMX output to DAQ ---------------------------

    m_histTool->setMonGroup(&CMX_jet);

    m_h_cmx_1d_thresh_TotalMainHits = m_histTool->bookMainJetThresholds(    // 3 bit
                                        "cmx_1d_thresh_TotalMainHits",
                                        "Main Jet Multiplicity per Threshold");

    // We don't need this histogram in RUN2. Fix ATR-11607
    // m_h_cmx_1d_thresh_TotalFwdHits = m_histTool->bookForwardJetThresholds(  // 2 bit
    //   "cmx_1d_thresh_TotalFwdHits",
    //   "Forward Jet Multiplicity per Threshold");

    m_h_cmx_1d_thresh_RoIOverflow = m_histTool->book1F("cmx_1d_thresh_RoIOverflow",
                                    "CMX-Jet Hits RoI Overflow", 3, 0., 3.);
    LWHist::LWHistAxis* axis = m_h_cmx_1d_thresh_RoIOverflow->GetXaxis();
    axis->SetBinLabel(1, "Remote");
    axis->SetBinLabel(2, "Local");
    axis->SetBinLabel(3, "Total");
    m_h_cmx_1d_topo_DataChecksum = m_histTool->book1F("cmx_1d_topo_DataChecksum",
                                   "CMX-Jet Topo Output Data Checksum Non-zero;Crate", 2, 0., 2.);
    m_histTool->numbers(m_h_cmx_1d_topo_DataChecksum, 0, 1);
    m_h_cmx_2d_topo_JEMOccupancyMap = m_histTool->bookJEMModuleVsCrate(
                                        "cmx_2d_topo_JEMOccupancyMap", "CMX-Jet Topo JEM Occupancy Maps");
    m_h_cmx_2d_topo_JEMOccupancyCounts = m_histTool->bookJEMModuleVsCrate(
                                           "cmx_2d_topo_JEMOccupancyCounts", "CMX-Jet Topo JEM Occupancy Counts Weighted");
    m_h_cmx_1d_topo_TOBsPerJEM = m_histTool->book1F("cmx_1d_topo_TOBsPerJEM",
                                 "CMX-Jet Topo TOBs per JEM;Number of TOBs", 7, 1., 8.); // 3 bits
    m_histTool->numbers(m_h_cmx_1d_topo_TOBsPerJEM, 1, s_tobsPerJEM);
    m_h_cmx_1d_topo_TOBsPerCMX = m_histTool->book1F("cmx_1d_topo_TOBsPerCMX",
                                 "CMX-Jet Topo TOBs per CMX;Number of TOBs", m_maxTobsPerCmx, 0., m_maxTobsPerCmx);

    m_histTool->setMonGroup(&CMX_energy);

    m_h_cmx_1d_energy_MissingEtHits = m_histTool->bookMissingEtThresholds(
                                        "cmx_1d_energy_MissingEtHits", "MissingEt Multiplicity per Threshold Standard");
    m_h_cmx_1d_energy_SumEtHits = m_histTool->bookSumEtThresholds(
                                    "cmx_1d_energy_SumEtHits", "SumEt Multiplicity per Threshold Standard");
    m_h_cmx_1d_energy_MissingEtSigHits = m_histTool->bookMissingEtSigThresholds(
                                           "cmx_1d_energy_MissingEtSigHits", "MissingEtSig Multiplicity per Threshold");
    m_h_cmx_1d_energy_MissingEtHitsRes = m_histTool->bookMissingEtThresholds(
                                           "cmx_1d_energy_MissingEtHitsRes", "MissingEt Multiplicity per Threshold Restricted");
    m_h_cmx_1d_energy_SumEtHitsWt = m_histTool->bookSumEtThresholds(
                                      "cmx_1d_energy_SumEtHitsWt", "SumEt Multiplicity per Threshold Weighted");

    m_h_cmx_1d_energy_TotalEx = m_histTool->book1F(
                                  "cmx_1d_energy_TotalEx", "E_{x}^{CMX} Standard;Ex [GeV]", nbins, -maxSum, maxSum);
    m_h_cmx_1d_energy_TotalEy = m_histTool->book1F(
                                  "cmx_1d_energy_TotalEy", "E_{y}^{CMX} Standard;Ey [GeV]", nbins, -maxSum, maxSum);
    m_h_cmx_1d_energy_TotalEt = m_histTool->book1F(
                                  "cmx_1d_energy_TotalEt", "SumE_{t}^{CMX} Standard;Et [GeV]", nbins, 0., 2.*maxSum);
    m_h_cmx_2d_energy_Overflow = m_histTool->bookProfile2D(
                                   "cmx_2d_energy_Overflow",
                                   "CMX Energy Overflow Rates Standard", 3, 0., 3., 3, 0., 3.);
    axis = m_h_cmx_2d_energy_Overflow->GetXaxis();
    axis->SetBinLabel(1, "Ex");
    axis->SetBinLabel(2, "Ey");
    axis->SetBinLabel(3, "Et");
    axis = m_h_cmx_2d_energy_Overflow->GetYaxis();
    axis->SetBinLabel(1, "Remote");
    axis->SetBinLabel(2, "Local");
    axis->SetBinLabel(3, "Total");
    m_h_cmx_1d_energy_TotalExRes = m_histTool->book1F(
                                     "cmx_1d_energy_TotalExRes", "E_{x}^{CMX} Restricted Eta Range;Ex [GeV]",
                                     nbins, -maxSum, maxSum);
    m_h_cmx_1d_energy_TotalEyRes = m_histTool->book1F(
                                     "cmx_1d_energy_TotalEyRes", "E_{y}^{CMX} Restricted Eta Range;Ey [GeV]",
                                     nbins, -maxSum, maxSum);
    m_h_cmx_1d_energy_TotalEtRes = m_histTool->book1F(
                                     "cmx_1d_energy_TotalEtRes", "SumE_{t}^{CMX} Restricted Eta Range;Et [GeV]",
                                     nbins, 0., 2.*maxSum);
    m_h_cmx_2d_energy_OverflowRes = m_histTool->bookProfile2D(
                                      "cmx_2d_energy_OverflowRes",
                                      "CMX Energy Overflow Rates Restricted Eta Range", 3, 0., 3., 3, 0., 3.);
    axis = m_h_cmx_2d_energy_OverflowRes->GetXaxis();
    axis->SetBinLabel(1, "Ex");
    axis->SetBinLabel(2, "Ey");
    axis->SetBinLabel(3, "Et");
    axis = m_h_cmx_2d_energy_OverflowRes->GetYaxis();
    axis->SetBinLabel(1, "Remote");
    axis->SetBinLabel(2, "Local");
    axis->SetBinLabel(3, "Total");

    //--------------------------- CMX output to RoI --------------------------

    m_histTool->setMonGroup(&CMX_RoI);

    m_h_cmx_1d_roi_MissingEtHits = m_histTool->bookMissingEtThresholds(
                                     "cmx_1d_roi_MissingEtHits", "MissingEt Multiplicity per Threshold Normal");
    m_h_cmx_1d_roi_SumEtHits = m_histTool->bookSumEtThresholds(
                                 "cmx_1d_roi_SumEtHits", "SumEt Multiplicity per Threshold Normal");
    m_h_cmx_1d_roi_MissingEtSigHits = m_histTool->bookMissingEtSigThresholds(
                                        "cmx_1d_roi_MissingEtSigHits", "MissingEtSig Multiplicity per Threshold");
    m_h_cmx_1d_roi_MissingEtHitsMask = m_histTool->bookMissingEtThresholds(
                                         "cmx_1d_roi_MissingEtHitsMask", "MissingEt Multiplicity per Threshold Masked");
    m_h_cmx_1d_roi_SumEtHitsMask = m_histTool->bookSumEtThresholds(
                                     "cmx_1d_roi_SumEtHitsMask", "SumEt Multiplicity per Threshold Masked");

    m_h_cmx_1d_roi_Ex = m_histTool->book1F("cmx_1d_roi_Ex",
                                           "E_{x}^{CMX} Normal;Ex [GeV]", nbins, -maxSum, maxSum);
    m_h_cmx_1d_roi_Ey = m_histTool->book1F("cmx_1d_roi_Ey",
                                           "E_{y}^{CMX} Normal;Ey [GeV]", nbins, -maxSum, maxSum);
    m_h_cmx_1d_roi_Et = m_histTool->book1F("cmx_1d_roi_Et",
                                           "SumE_{t}^{CMX} Normal;Et [GeV]", nbins, 0., 2.*maxSum);
    m_h_cmx_1d_roi_ExMask = m_histTool->book1F("cmx_1d_roi_ExMask",
                            "E_{x}^{CMX} Masked;Ex [GeV]", nbins, -maxSum, maxSum);
    m_h_cmx_1d_roi_EyMask = m_histTool->book1F("cmx_1d_roi_EyMask",
                            "E_{y}^{CMX} Masked;Ey [GeV]", nbins, -maxSum, maxSum);
    m_h_cmx_1d_roi_EtMask = m_histTool->book1F("cmx_1d_roi_EtMask",
                            "SumE_{t}^{CMX} Masked;Et [GeV]", nbins, 0., 2.*maxSum);

    //---------------------------- S-Link errors -----------------------------

    m_histTool->setMonGroup(&CMX_errorDetail);

    m_h_cmx_2d_thresh_Status = m_histTool->bookJEMSubStatusVsCrate(
                                 "cmx_2d_thresh_Status", "Errors from CMX Jet SubStatus Word");
    m_h_cmx_2d_energy_Status = m_histTool->bookJEMSubStatusVsCrate(
                                 "cmx_2d_energy_Status", "Errors from CMX Energy SubStatus Word");
    m_h_cmx_2d_thresh_ParityTob = m_histTool->book2F("cmx_2d_thresh_ParityTob",
                                  "CMX Jet TOB Parity Errors;Module;Crate",
                                  16, 0., 16., 2, 0., 2.);
    m_histTool->numbers(m_h_cmx_2d_thresh_ParityTob, 0, 15);
    m_histTool->numbers(m_h_cmx_2d_thresh_ParityTob, 0, 1, 1, 0, false);
    m_h_cmx_1d_thresh_ParityRem = m_histTool->book1F("cmx_1d_thresh_ParityRem",
                                  "CMX Jet Remote Hit Sums Parity Errors;Cable/Phase",
                                  4, 0., 4.);
    m_histTool->numberPairs(m_h_cmx_1d_thresh_ParityRem, 0, 1, 0, 1);
    m_h_cmx_2d_energy_ParityMod = m_histTool->book2F("cmx_2d_energy_ParityMod",
                                  "CMX Energy Module Sums Parity Errors;;Type/Phase",
                                  19, 0., 19., 3, 0., 3.);
    m_histTool->jemCrateModule(m_h_cmx_2d_energy_ParityMod);
    m_h_cmx_2d_energy_ParityMod->GetYaxis()->SetBinLabel(1, "Ex/0");
    m_h_cmx_2d_energy_ParityMod->GetYaxis()->SetBinLabel(2, "Ey/1");
    m_h_cmx_2d_energy_ParityMod->GetYaxis()->SetBinLabel(3, "Et/2");
    m_h_cmx_1d_energy_ParityRem = m_histTool->book1F("cmx_1d_energy_ParityRem",
                                  "CMX Energy Remote Crate Sums Parity Errors;Cable/Phase",
                                  4, 0., 4.);
    m_histTool->numberPairs(m_h_cmx_1d_energy_ParityRem, 0, 1, 0, 1);

    m_h_cmx_1d_TriggeredSlices = m_histTool->book1F("cmx_1d_TriggeredSlices",
                                 "Comparison of CMX Jet and Energy triggered slice numbers;Difference",
                                 5, 0., 5.);
    m_histTool->numbers(m_h_cmx_1d_TriggeredSlices, 0, 4);

    m_histTool->setMonGroup(&CMX_error);

    //Error Summary for all CMXs in system
    m_h_cmx_1d_ErrorSummary = m_histTool->book1F("cmx_1d_ErrorSummary",
                              "Error Summary of CMX Jet and Energy",
                              NumberOfSummaryBins, 0., NumberOfSummaryBins);

    m_histTool->setMonGroup(&CMX_errorEvents);

    m_h_cmx_2d_ErrorEventNumbers = m_histTool->bookEventNumbers(
                                     "cmx_2d_ErrorEventNumbers", "JEM-CMX Error Event Numbers",
                                     NumberOfSummaryBins, 0., NumberOfSummaryBins);

    axis = m_h_cmx_1d_ErrorSummary->GetXaxis();
    for (int i = 0; i < 2; ++i) {
      axis->SetBinLabel(1 + JetStatus,    "Jet status");
      axis->SetBinLabel(1 + EnergyStatus, "Energy status");
      axis->SetBinLabel(1 + JetParity,    "Jet parity");
      axis->SetBinLabel(1 + EnergyParity, "Energy parity");
      axis = m_h_cmx_2d_ErrorEventNumbers->GetYaxis();
    }

    m_histTool->unsetMonGroup();
    m_histBooked = true;
  }

  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
StatusCode JEPCMXMon::fillHistograms()
/*---------------------------------------------------------*/
{
  const bool debug = msgLvl(MSG::DEBUG);

  if (!m_histBooked) {
    if (debug) msg(MSG::DEBUG) << "Histogram(s) not booked" << endmsg;
    return StatusCode::SUCCESS;
  }

  // Skip events believed to be corrupt

  if (m_errorTool->corrupt()) {
    if (debug) msg(MSG::DEBUG) << "Skipping corrupt event" << endmsg;
    return StatusCode::SUCCESS;
  }

  // Error vector for global overview
  std::vector<int> overview(2);

  // triggered slice numbers
  int j_num_slice = -1;
  int e_num_slice = -1;

  using LVL1::DataError;

  // =========================================================================
  // ================= Container: CMX Jet TOBs ===============================
  // =========================================================================

  // retrieve CMX Jet TOBs from Storegate
  const xAOD::CMXJetTobContainer* CMXJetTob = 0;
  StatusCode sc = evtStore()->retrieve(CMXJetTob, m_CMXJetTobLocation);
  if (sc == StatusCode::FAILURE || !CMXJetTob) {
    msg(MSG::INFO) << "No CMX JetTob found in TES at "
                   << m_CMXJetTobLocation << endmsg;
    return StatusCode::SUCCESS;
  }

  if (debug) {
    msg(MSG::DEBUG) << "--------------  CMX Jet TOBs ---------------" << endmsg;
  }

  xAOD::CMXJetTobContainer::const_iterator it_CMXJetTob ;

  // Step over all cells
  std::vector<int> tobCount(s_crates * s_modules);
  std::vector<int> cmxCount(s_crates);
  for (it_CMXJetTob = CMXJetTob->begin(); it_CMXJetTob != CMXJetTob->end();
       ++it_CMXJetTob) {
    const int crate    = (*it_CMXJetTob)->crate();
    const int jem      = (*it_CMXJetTob)->jem();
    const int frame    = (*it_CMXJetTob)->frame();
    const int location = (*it_CMXJetTob)->location();
    const int energyLg = (*it_CMXJetTob)->energyLarge();
    const int energySm = (*it_CMXJetTob)->energySmall();
    const int error    = (*it_CMXJetTob)->error();
    const int x = crate * s_modules + jem;
    const int y = frame * 4 + location;

    if (energyLg) {
      m_h_cmx_1d_tob_EnergyLg->Fill(energyLg);
      m_h_cmx_2d_tob_Hitmap->Fill(x, y);
    }
    if (energySm) m_h_cmx_1d_tob_EnergySm->Fill(energySm);
    if (error) {
      const LVL1::DataError err(error);
      int parity = err.get(LVL1::DataError::Parity);
      if (parity) {
        m_h_cmx_2d_thresh_ParityTob->Fill(jem, crate);
        m_h_cmx_1d_ErrorSummary->Fill(JetParity);
        m_histTool->fillEventNumber(m_h_cmx_2d_ErrorEventNumbers, JetParity);
        overview[crate] |= (1 << JetParity);
      }
    }
    if (energyLg || energySm || error) {
      ++tobCount[x];
      ++cmxCount[crate];
    }
  }
  for (int crate = 0; crate < s_crates; ++crate) {
    for (int jem = 0; jem < s_modules; ++jem) {
      int val = tobCount[crate * s_modules + jem];
      if (val) {
        if (val > s_tobsPerJEM) val = s_tobsPerJEM + 1;
        m_h_cmx_1d_tob_TOBsPerJEM->Fill(val);
      }
    }
    int val = cmxCount[crate];
    if (val) {
      if (val >= m_maxTobsPerCmx) val = m_maxTobsPerCmx - 1;
      m_h_cmx_1d_tob_TOBsPerCMX->Fill(val);
    }
  }

  // =========================================================================
  // ================= Container: CMX Jet Hits ===============================
  // =========================================================================

  // retrieve CMX Jet Hits from Storegate
  const xAOD::CMXJetHitsContainer* CMXJetHits = 0;
  sc = evtStore()->retrieve(CMXJetHits, m_CMXJetHitsLocation);
  if (sc == StatusCode::FAILURE || !CMXJetHits) {
    msg(MSG::INFO) << "No CMX JetHits found in TES at "
                   << m_CMXJetHitsLocation << endmsg;
    return StatusCode::SUCCESS;
  }

  if (debug) {
    msg(MSG::DEBUG) << "--------------  CMX Jet Hits ---------------" << endmsg;
  }

  xAOD::CMXJetHitsContainer::const_iterator it_CMXJetHits ;

  // Step over all cells
  cmxCount.assign(s_crates, 0);
  for (it_CMXJetHits = CMXJetHits->begin(); it_CMXJetHits != CMXJetHits->end();
       ++it_CMXJetHits) {
    const int crate  = (*it_CMXJetHits)-> crate();
    const int source = (*it_CMXJetHits)-> sourceComponent();
    const unsigned int hits0 = (*it_CMXJetHits)->hits0();
    const unsigned int hits1 = (*it_CMXJetHits)->hits1();
    const DataError err0((*it_CMXJetHits)->error0());
    const DataError err1((*it_CMXJetHits)->error1());

    // -----------------------------------------------------------------------
    // --------- Histos with distribution of CMX Hit Multiplicities ----------
    // -----------------------------------------------------------------------

    if (source == xAOD::CMXJetHits::TOTAL_MAIN) {
      m_histTool->fillThresholds(m_h_cmx_1d_thresh_TotalMainHits, hits0, 5, 3);
      m_histTool->fillThresholds(m_h_cmx_1d_thresh_TotalMainHits, hits1, 5, 3, 5);
    }
    else if (source == xAOD::CMXJetHits::TOTAL_FORWARD) {
      // FIX ATR-11607
      // m_histTool->fillThresholds(m_h_cmx_1d_thresh_TotalFwdHits, hits0, 8, 2);
      // m_histTool->fillThresholds(m_h_cmx_1d_thresh_TotalFwdHits, hits1, 7, 2, 8);
    }
    // RoI overflow
    if (crate == s_crates - 1 && err0.get(DataError::Overflow)) {
      const int x = (source == xAOD::CMXJetHits::REMOTE_MAIN)
                    ? 0
                    : (source == xAOD::CMXJetHits::LOCAL_MAIN)
                    ? 1 : (source == xAOD::CMXJetHits::TOTAL_MAIN) ? 2 : 3;
      if (x < 3) m_h_cmx_1d_thresh_RoIOverflow->Fill(x);
    }

    // -----------------------------------------------------------------------
    // ------------------- Histos with Topo information ----------------------
    // -----------------------------------------------------------------------

    else if (source == xAOD::CMXJetHits::TOPO_CHECKSUM) {
      if (hits0) m_h_cmx_1d_topo_DataChecksum->Fill(crate);
    }
    else if (source == xAOD::CMXJetHits::TOPO_OCCUPANCY_MAP) {
      if (hits0) {
        m_histTool->fillThresholdsVsY(m_h_cmx_2d_topo_JEMOccupancyMap,
                                      hits0, crate, s_modules, 1);
      }
    }
    else if (source == xAOD::CMXJetHits::TOPO_OCCUPANCY_COUNTS) {
      if (hits0) {
        m_histTool->fillThresholdsVsY(m_h_cmx_2d_topo_JEMOccupancyCounts,
                                      hits0, crate, s_modules / 2, 3);
        for (int mod = 0; mod < s_modules / 2; ++mod) {
          const int val = (hits0 >> (mod * 3)) & 0x7;
          if (val) {
            m_h_cmx_1d_topo_TOBsPerJEM->Fill(val);
            cmxCount[crate] += val;
          }
        }
      }
      if (hits1) {
        m_histTool->fillThresholdsVsY(m_h_cmx_2d_topo_JEMOccupancyCounts,
                                      hits1, crate, s_modules / 2, 3, s_modules / 2);
        for (int mod = 0; mod < s_modules / 2; ++mod) {
          const int val = (hits1 >> (mod * 3)) & 0x7;
          if (val) {
            m_h_cmx_1d_topo_TOBsPerJEM->Fill(val);
            cmxCount[crate] += val;
          }
        }
      }
    }

    // -----------------------------------------------------------------------
    // ----------------- Histos with SubStatus Word errors -------------------
    // -----------------------------------------------------------------------

    if (j_num_slice < 0) j_num_slice = (*it_CMXJetHits)->peak();

    //Error summary plots
    //substatus word
    const int status = (err0.error() >> LVL1::DataError::GLinkParity) & 0xff;
    if (status) {
      for (int bit = 0; bit < 8; ++bit) {
        if ((status >> bit) & 0x1) m_h_cmx_2d_thresh_Status->Fill(bit, crate);
      }
      m_h_cmx_1d_ErrorSummary->Fill(JetStatus);
      m_histTool->fillEventNumber(m_h_cmx_2d_ErrorEventNumbers, JetStatus);
      overview[crate] |= (1 << JetStatus);
    }

    if (source == xAOD::CMXJetHits::REMOTE_MAIN ||
        source == xAOD::CMXJetHits::REMOTE_FORWARD) {
      // Parity
      const int p0 = err0.get(DataError::Parity);
      const int p1 = err1.get(DataError::Parity);
      if (p0 || p1) {
        if (p0) { // cable 0/phase 0 or cable 1/phase 0
          const int xpos = (source == xAOD::CMXJetHits::REMOTE_MAIN) ? 0 : 2;
          m_h_cmx_1d_thresh_ParityRem->Fill(xpos);
        }
        if (p1) { // cable 0/phase 1 or cable 1/phase 1
          const int xpos = (source == xAOD::CMXJetHits::REMOTE_MAIN) ? 1 : 3;
          m_h_cmx_1d_thresh_ParityRem->Fill(xpos);
        }
        m_h_cmx_1d_ErrorSummary->Fill(JetParity);
        m_histTool->fillEventNumber(m_h_cmx_2d_ErrorEventNumbers, JetParity);
        overview[crate] |= (1 << JetParity);
      }
    }

  }
  for (int crate = 0; crate < s_crates; ++crate) {
    int val = cmxCount[crate];
    if (val) {
      if (val >= m_maxTobsPerCmx) val = m_maxTobsPerCmx - 1;
      m_h_cmx_1d_topo_TOBsPerCMX->Fill(val);
    }
  }

  // =========================================================================
  // ================= Container: CMX Et Sums ================================
  // =========================================================================

  // retrieve CMX Et Sums from Storegate
  const xAOD::CMXEtSumsContainer* CMXEtSums = 0;
  sc = evtStore()->retrieve(CMXEtSums, m_CMXEtSumsLocation);
  if (sc == StatusCode::FAILURE || !CMXEtSums) {
    msg(MSG::INFO) << "No CMXEtSums found in TES at "
                   << m_CMXEtSumsLocation << endmsg ;
    return StatusCode::SUCCESS;
  }

  msg(MSG::DEBUG) << "-------------- CMX Et Sums ---------------" << endmsg;

  // Step over all cells
  xAOD::CMXEtSumsContainer::const_iterator it_CMXEtSums ;
  for (it_CMXEtSums = CMXEtSums->begin(); it_CMXEtSums != CMXEtSums->end();
       ++it_CMXEtSums) {
    const int crate  = (*it_CMXEtSums)->crate();
    const int source = (*it_CMXEtSums)->sourceComponent();
    const unsigned int rawEx = (*it_CMXEtSums)->ex();
    const unsigned int rawEy = (*it_CMXEtSums)->ey();
    const unsigned int rawEt = (*it_CMXEtSums)->et();
    const int exError = (*it_CMXEtSums)->exError();
    const int eyError = (*it_CMXEtSums)->eyError();
    const int etError = (*it_CMXEtSums)->etError();

    // -----------------------------------------------------------------------
    // -------------- Histos with distribution of JEM Energies ---------------
    // -----------------------------------------------------------------------
    // JEM energy sums
    if (source < s_modules) {
      // note: JEM energies are no longer compressed
      const int ex = rawEx;
      const int ey = rawEy;
      const int et = rawEt;

      if (ex) m_h_cmx_1d_energy_SubSumsEx->Fill(ex);
      if (ey) m_h_cmx_1d_energy_SubSumsEy->Fill(ey);
      if (et) m_h_cmx_1d_energy_SubSumsEt->Fill(et);
    }

    // -----------------------------------------------------------------------
    // ---------- Histos with distribution of total Energy per system --------
    // -----------------------------------------------------------------------
    // total energy sums
    else if (source == xAOD::CMXEtSums::TOTAL_STANDARD ||
             source == xAOD::CMXEtSums::TOTAL_RESTRICTED) {
      // Use CrateEnergy object to decode 15-bit twos-complement format
      LVL1::CrateEnergy cen(crate, rawEt, rawEx, rawEy,
                            etError & 0x1, exError & 0x1, eyError & 0x1);
      //const int ex = std::abs(cen.ex());
      //const int ey = std::abs(cen.ey());
      const int ex = cen.ex();
      const int ey = cen.ey();
      const int et = rawEt;

      if (source == xAOD::CMXEtSums::TOTAL_STANDARD) {
        if (ex && !cen.exOverflow()) m_h_cmx_1d_energy_TotalEx->Fill(ex);
        if (ey && !cen.eyOverflow()) m_h_cmx_1d_energy_TotalEy->Fill(ey);
        if (et && !cen.etOverflow()) m_h_cmx_1d_energy_TotalEt->Fill(et);
      } else {
        if (ex && !cen.exOverflow()) m_h_cmx_1d_energy_TotalExRes->Fill(ex);
        if (ey && !cen.eyOverflow()) m_h_cmx_1d_energy_TotalEyRes->Fill(ey);
        if (et && !cen.etOverflow()) m_h_cmx_1d_energy_TotalEtRes->Fill(et);
      }
    }
    //MissingEt/SumEt/MissingEtSig Hitmaps
    else if (source == xAOD::CMXEtSums::MISSING_ET_STANDARD ||
             source == xAOD::CMXEtSums::SUM_ET_STANDARD ||
             source == xAOD::CMXEtSums::MISSING_ET_SIG_STANDARD) {
      const int nHits = 8;
      TH1F_LW* hist = (source == xAOD::CMXEtSums::MISSING_ET_STANDARD)
                      ? m_h_cmx_1d_energy_MissingEtHits
                      : (source == xAOD::CMXEtSums::SUM_ET_STANDARD)
                      ? m_h_cmx_1d_energy_SumEtHits
                      : m_h_cmx_1d_energy_MissingEtSigHits;
      m_histTool->fillThresholds(hist, rawEt, nHits, 1);
    }
    else if (source == xAOD::CMXEtSums::MISSING_ET_RESTRICTED ||
             source == xAOD::CMXEtSums::SUM_ET_RESTRICTED) {
      const int nHits = 8;
      TH1F_LW* hist = (source == xAOD::CMXEtSums::MISSING_ET_RESTRICTED)
                      ? m_h_cmx_1d_energy_MissingEtHitsRes
                      : m_h_cmx_1d_energy_SumEtHitsWt;
      m_histTool->fillThresholds(hist, rawEt, nHits, 1);
    }

    if (e_num_slice < 0) {
      e_num_slice = (*it_CMXEtSums)-> peak();
      if (j_num_slice >= 0) {
        m_h_cmx_1d_TriggeredSlices->Fill(std::abs(e_num_slice - j_num_slice));
      }
    }

    // -----------------------------------------------------------------------
    // --------------- Histos with SubStatus Word errors ---------------------
    // -----------------------------------------------------------------------
    //Error summary plots
    //substatus word
    const DataError exerr(exError);
    const DataError eyerr(eyError);
    const DataError eterr(etError);
    const int status = (eterr.error() >> LVL1::DataError::GLinkParity) & 0xff;
    if (status) {
      for (int bit = 0; bit < 8; ++bit) {
        if ((status >> bit) & 0x1) m_h_cmx_2d_energy_Status->Fill(bit, crate);
      }
      m_h_cmx_1d_ErrorSummary->Fill(EnergyStatus);
      m_histTool->fillEventNumber(m_h_cmx_2d_ErrorEventNumbers, EnergyStatus);
      overview[crate] |= (1 << EnergyStatus);
    }

    if (source < s_modules || source == xAOD::CMXEtSums::REMOTE_STANDARD ||
        source == xAOD::CMXEtSums::REMOTE_RESTRICTED) {
      // Parity
      const int p0 = exerr.get(DataError::Parity);
      const int p1 = eyerr.get(DataError::Parity);
      const int p2 = eterr.get(DataError::Parity);
      if (p0 || p1 || p2) {
        // backplane parity
        if (source < s_modules) {
          const int xpos = crate * s_modules + source;
          if (p0) m_h_cmx_2d_energy_ParityMod->Fill(xpos, 0.);
          if (p1) m_h_cmx_2d_energy_ParityMod->Fill(xpos, 1.);
          if (p2) m_h_cmx_2d_energy_ParityMod->Fill(xpos, 2.);
        }
        // cable transmission parity
        else if (source == xAOD::CMXEtSums::REMOTE_STANDARD) {
          if (p0) m_h_cmx_1d_energy_ParityRem->Fill(0.); // cable 0, phase 0
          if (p1) m_h_cmx_1d_energy_ParityRem->Fill(1.); // cable 0, phase 1
        } else {
          if (p0) m_h_cmx_1d_energy_ParityRem->Fill(2.); // cable 1, phase 0
          if (p1) m_h_cmx_1d_energy_ParityRem->Fill(3.); // cable 1, phase 1
        }
        m_h_cmx_1d_ErrorSummary->Fill(EnergyParity);
        m_histTool->fillEventNumber(m_h_cmx_2d_ErrorEventNumbers, EnergyParity);
        overview[crate] |= (1 << EnergyParity);
      }
    }

    // Overflow - not an error, plot rate
    if (crate == 1 && (source == xAOD::CMXEtSums::REMOTE_STANDARD ||
                       source == xAOD::CMXEtSums::LOCAL_STANDARD  ||
                       source == xAOD::CMXEtSums::TOTAL_STANDARD)) {
      const double ypos = (source == xAOD::CMXEtSums::REMOTE_STANDARD) ? 0.
                          : (source == xAOD::CMXEtSums::LOCAL_STANDARD)  ? 1. : 2.;
      m_h_cmx_2d_energy_Overflow->Fill(0., ypos, exerr.get(DataError::Overflow));
      m_h_cmx_2d_energy_Overflow->Fill(1., ypos, eyerr.get(DataError::Overflow));
      m_h_cmx_2d_energy_Overflow->Fill(2., ypos, eterr.get(DataError::Overflow));
    }
    else if (crate == 1 && (source == xAOD::CMXEtSums::REMOTE_RESTRICTED ||
                            source == xAOD::CMXEtSums::LOCAL_RESTRICTED  ||
                            source == xAOD::CMXEtSums::TOTAL_RESTRICTED)) {
      const double ypos = (source == xAOD::CMXEtSums::REMOTE_RESTRICTED) ? 0.
                          : (source == xAOD::CMXEtSums::LOCAL_RESTRICTED)  ? 1. : 2.;
      m_h_cmx_2d_energy_OverflowRes->Fill(0., ypos, exerr.get(DataError::Overflow));
      m_h_cmx_2d_energy_OverflowRes->Fill(1., ypos, eyerr.get(DataError::Overflow));
      m_h_cmx_2d_energy_OverflowRes->Fill(2., ypos, eterr.get(DataError::Overflow));
    }

  }

  // =========================================================================
  // ================= Container: CMX RoI ====================================
  // =========================================================================

  // retrieve RoI information from Storegate
  const xAOD::CMXRoIContainer* CRCon = 0;
  sc = evtStore()->retrieve (CRCon, m_CMXRoILocation);
  if (sc == StatusCode::FAILURE || !CRCon || !CRCon->size()) {
    msg(MSG::INFO) << "No CMX RoI found in TES at " << m_CMXRoILocation
                   << endmsg;
    return StatusCode::SUCCESS;
  }
  const xAOD::CMXRoI* CR = *CRCon->begin();

  if (debug) {
    msg(MSG::DEBUG) << "-------------- CMX RoI ---------------" << endmsg;
  }

  // -------------------------------------------------------------------------
  // -------------- Histos filled with CMX RoI information -------------------
  // -------------------------------------------------------------------------

  const int rawEx = (CR)->ex();
  const int rawEy = (CR)->ey();
  const int et    = (CR)->et();
  const int exError = (CR)->exError();
  const int eyError = (CR)->eyError();
  const int etError = (CR)->etError();
  const int sumEtHits = (CR)->sumEtHits();
  const int missingEtHits = (CR)->missingEtHits();
  const int missingEtSigHits = (CR)->missingEtSigHits();
  const int rawExM = (CR)->ex(xAOD::CMXRoI::MASKED);
  const int rawEyM = (CR)->ey(xAOD::CMXRoI::MASKED);
  const int etM    = (CR)->et(xAOD::CMXRoI::MASKED);
  const int exErrorM = (CR)->exError(xAOD::CMXRoI::MASKED);
  const int eyErrorM = (CR)->eyError(xAOD::CMXRoI::MASKED);
  const int etErrorM = (CR)->etError(xAOD::CMXRoI::MASKED);
  const int sumEtHitsM = (CR)->sumEtHits(xAOD::CMXRoI::MASKED);
  const int missingEtHitsM = (CR)->missingEtHits(xAOD::CMXRoI::MASKED);

  m_histTool->fillThresholds(m_h_cmx_1d_roi_SumEtHits, sumEtHits, 8, 1);
  m_histTool->fillThresholds(m_h_cmx_1d_roi_MissingEtHits, missingEtHits, 8, 1);
  m_histTool->fillThresholds(m_h_cmx_1d_roi_MissingEtSigHits, missingEtSigHits,
                             8, 1);
  m_histTool->fillThresholds(m_h_cmx_1d_roi_SumEtHitsMask, sumEtHitsM, 8, 1);
  m_histTool->fillThresholds(m_h_cmx_1d_roi_MissingEtHitsMask, missingEtHitsM, 8, 1);

  // Use CrateEnergy object to decode 15-bit twos-complement format
  LVL1::CrateEnergy cen(1, et, rawEx, rawEy,
                        etError & 0x1, exError & 0x1, eyError & 0x1);
  //const int ex = std::abs(cen.ex());
  //const int ey = std::abs(cen.ey());
  const int ex = cen.ex();
  const int ey = cen.ey();

  if (ex && !cen.exOverflow()) m_h_cmx_1d_roi_Ex->Fill(ex);
  if (ey && !cen.eyOverflow()) m_h_cmx_1d_roi_Ey->Fill(ey);
  if (et && !cen.etOverflow()) m_h_cmx_1d_roi_Et->Fill(et);

  LVL1::CrateEnergy cen2(1, etM, rawExM, rawEyM,
                         etErrorM & 0x1, exErrorM & 0x1, eyErrorM & 0x1);
  //const int exM = std::abs(cen2.ex());
  //const int eyM = std::abs(cen2.ey());
  const int exM = cen2.ex();
  const int eyM = cen2.ey();

  if (exM && !cen2.exOverflow()) m_h_cmx_1d_roi_ExMask->Fill(exM);
  if (eyM && !cen2.eyOverflow()) m_h_cmx_1d_roi_EyMask->Fill(eyM);
  if (etM && !cen2.etOverflow()) m_h_cmx_1d_roi_EtMask->Fill(etM);

  // Write overview vector to StoreGate
  std::vector<int>* save = new std::vector<int>(overview);
  sc = evtStore()->record(save, m_errorLocation);
  if (sc != StatusCode::SUCCESS) {
    msg(MSG::ERROR) << "Error recording JEM CMX error vector in TES "
                    << endmsg;
    return sc;
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode JEPCMXMon::procHistograms()
/*---------------------------------------------------------*/
{
  msg(MSG::DEBUG) << "in procHistograms" << endmsg ;

  //if ( endOfEventsBlock || endOfLumiBlock || endOfRun ) { }

  return StatusCode::SUCCESS;
}

} // end namespace
