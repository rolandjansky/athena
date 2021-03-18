/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ===========================================================================
// $Id$
// ===========================================================================
// @file
// Monitoring of the JEP on JEM level
//
// @author Johanna Fleckner (Johanna.Fleckner@uni-mainz.de)
// @author Sasha Mazurov (alexander.mazurov@cern.ch)
//
//
// $Revision$
// Last modification $Date$
// by  $Author$
// ============================================================================

#include <cmath>
#include <set>
#include <sstream>

#include "LWHists/LWHist.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH2I_LW.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "SGTools/StlVectorClids.h"

#include "AthenaMonitoring/AthenaMonManager.h"

#include "TrigT1CaloMonitoringTools/ITrigT1CaloMonErrorTool.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramToolV1.h"

#include "TrigT1CaloEvent/JEMHits.h"
#include "TrigT1CaloEvent/JEMEtSums.h"
#include "TrigT1CaloEvent/JetElement.h"
#include "TrigT1CaloEvent/JEMRoI.h"
#include "TrigT1CaloUtils/QuadLinear.h"
#include "TrigT1CaloUtils/DataError.h"
#include "TrigT1CaloUtils/CoordToHardware.h"
#include "TrigT1Interfaces/JEPRoIDecoder.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1Interfaces/Coordinate.h"
#include "TrigConfL1Data/L1DataDef.h"

#include "JEMMon.h"
// ============================================================================
namespace LVL1 {
// ============================================================================

JEMMon::JEMMon(const std::string &type, const std::string &name,
               const IInterface *parent)
    : ManagedMonitorToolBase(type, name, parent),
      m_errorTool("LVL1::TrigT1CaloMonErrorToolV1/TrigT1CaloMonErrorToolV1"),
      m_histTool(
          "LVL1::TrigT1CaloLWHistogramToolV1/TrigT1CaloLWHistogramToolV1"),
      m_histBooked(false), m_h_jem_em_1d_jetEl_Eta(0),
      m_h_jem_had_1d_jetEl_Eta(0), m_h_jem_em_1d_jetEl_Phi(0),
      m_h_jem_had_1d_jetEl_Phi(0), m_h_jem_em_1d_jetEl_Energy(0),
      m_h_jem_had_1d_jetEl_Energy(0),
      m_h_jem_em_2d_etaPhi_jetEl_HitMapWeighted(0),
      m_h_jem_had_2d_etaPhi_jetEl_HitMapWeighted(0),
      m_v_jem_em_2d_etaPhi_jetEl_HitMapSlice(0),
      m_v_jem_had_2d_etaPhi_jetEl_HitMapSlice(0), m_h_jem_2d_Status(0),
      m_h_jem_em_2d_etaPhi_jetEl_Parity(0),
      m_h_jem_had_2d_etaPhi_jetEl_Parity(0),
      m_h_jem_em_2d_etaPhi_jetEl_LinkDown(0),
      m_h_jem_had_2d_etaPhi_jetEl_LinkDown(0), m_h_jem_1d_TriggeredSlice(0),
      m_h_jem_1d_thresh_MainHits(0), m_h_jem_1d_thresh_FwdHitsRight(0),
      m_h_jem_1d_thresh_FwdHitsLeft(0), m_h_jem_2d_thresh_HitsPerJem(0),
      m_h_jem_1d_energy_SubSumsEx(0), m_h_jem_1d_energy_SubSumsEy(0),
      m_h_jem_1d_energy_SubSumsEt(0), m_h_jem_1d_roi_MainHits(0),
      m_h_jem_1d_roi_FwdHitsRight(0), m_h_jem_1d_roi_FwdHitsLeft(0),
      m_v_jem_2d_etaPhi_roi_MainThresh(0), m_v_jem_2d_etaPhi_roi_FwdThresh(0),
      m_h_jem_2d_etaPhi_roi_Parity(0), m_h_jem_2d_etaPhi_roi_Saturation(0),
      m_h_jem_1d_ErrorSummary(0), m_h_jem_2d_ErrorEventNumbers(0)
/*---------------------------------------------------------*/
{
  // This is how you declare the parameters to Gaudi so that
  // they can be over-written via the job options file

  declareProperty("JetElementLocation",
                  m_JetElementLocation =
                      LVL1::TrigT1CaloDefs::JetElementLocation);
  declareProperty("JEMHitsLocation",
                  m_JEMHitsLocation = LVL1::TrigT1CaloDefs::JEMHitsLocation);
  declareProperty("JEMEtSumsLocation",
                  m_JEMEtSumsLocation =
                      LVL1::TrigT1CaloDefs::JEMEtSumsLocation);
  declareProperty("JEMRoILocation",
                  m_JEMRoILocation = LVL1::TrigT1CaloDefs::JEMRoILocation);

  declareProperty("NumberOfSlices", m_SliceNo = 5,
                  "Maximum possible number of slices");
  declareProperty("MaxEnergyRange", m_MaxEnergyRange = 1024,
                  "Maximum energy in JetElement energy plots");

  declareProperty("PathInRootFile", m_PathInRootFile = "L1Calo/JEM");
  declareProperty("ErrorPathInRootFile",
                  m_ErrorPathInRootFile = "L1Calo/JEM/Errors/Hardware");
}

/*---------------------------------------------------------*/
JEMMon::~JEMMon()
/*---------------------------------------------------------*/
{}

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

/*---------------------------------------------------------*/
StatusCode JEMMon::initialize()
/*---------------------------------------------------------*/
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << PACKAGE_VERSION << endmsg;

  StatusCode sc;

  sc = ManagedMonitorToolBase::initialize();
  if (sc.isFailure())
    return sc;

  sc = m_errorTool.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloMonErrorTool" << endmsg;
    return sc;
  }

  sc = m_histTool.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloLWHistogramToolV1"
                    << endmsg;
    return sc;
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode JEMMon::bookHistogramsRecurrent()
/*---------------------------------------------------------*/
{
  msg(MSG::DEBUG) << "in JEMMon::bookHistograms" << endmsg;

  if (m_environment == AthenaMonManager::online) {
    // book histograms that are only made in the online environment...
  }

  if (m_dataType == AthenaMonManager::cosmics) {
    // book histograms that are only relevant for cosmics data...
  }

  // if ( newLumiBlock) { }

  if (newRunFlag()) {

    MgmtAttr_t attr = ATTRIB_UNMANAGED;
    MonGroup JetElements_expert(this, m_PathInRootFile + "/Input", run, attr);
    MonGroup JetElements_shift(this, m_PathInRootFile + "/Input", run, attr);
    MonGroup JEM_Thresholds(this, m_PathInRootFile + "/Output/Thresholds", run,
                            attr);
    MonGroup JEM_EnergySums(this, m_PathInRootFile + "/Output/EnergySums", run,
                            attr);
    MonGroup JEM_RoI(this, m_PathInRootFile + "/Output/RoI", run, attr);
    MonGroup JEM_Error(this, m_ErrorPathInRootFile, run, attr);
    MonGroup JEM_ErrorDetail(this, m_ErrorPathInRootFile, run, attr);
    MonGroup JEM_ErrorEvents(this, m_ErrorPathInRootFile, run, attr, "",
                             "eventSample");

    //-------------------------- JetElements histos --------------------------

    m_histTool->setMonGroup(&JetElements_expert);

    m_h_jem_em_1d_jetEl_Eta = m_histTool->bookJEMEta(
        "jem_em_1d_jetEl_Eta",
        "EM TowerSum distribution per #eta  --  JEM input");
    m_h_jem_had_1d_jetEl_Eta = m_histTool->bookJEMEta(
        "jem_had_1d_jetEl_Eta",
        "HAD TowerSum distribution per #eta  --  JEM input");

    m_h_jem_em_1d_jetEl_Phi = m_histTool->book1F(
        "jem_em_1d_jetEl_Phi",
        "EM TowerSum distribution per #phi  --  JEM input;phi", 32, 0.,
        2. * M_PI);
    m_h_jem_had_1d_jetEl_Phi = m_histTool->book1F(
        "jem_had_1d_jetEl_Phi",
        "HAD TowerSum distribution per #phi  --  JEM input;phi", 32, 0.,
        2. * M_PI);

    int jebins = 256;
    if (m_MaxEnergyRange < jebins)
      jebins = m_MaxEnergyRange;
    m_h_jem_em_1d_jetEl_Energy = m_histTool->book1F(
        "jem_em_1d_jetEl_Energy",
        "TowerSum EM energy distribution  --  JEM input;em energy [GeV]",
        jebins - 1, 1, m_MaxEnergyRange);
    m_h_jem_had_1d_jetEl_Energy = m_histTool->book1F(
        "jem_had_1d_jetEl_Energy",
        "TowerSum HAD energy distribution  --  JEM input;had energy [GeV]",
        jebins - 1, 1, m_MaxEnergyRange);

    // number of triggered slice
    m_h_jem_1d_TriggeredSlice = m_histTool->book1F(
        "jem_1d_TriggeredSlice", "Number of the Triggered Slice for JE;#Slice",
        m_SliceNo, 0, m_SliceNo);
    m_histTool->numbers(m_h_jem_1d_TriggeredSlice, 0, m_SliceNo - 1);

    m_histTool->setMonGroup(&JetElements_shift);

    m_h_jem_em_2d_etaPhi_jetEl_HitMapWeighted = m_histTool->bookJEMEtaVsPhi(
        "jem_em_2d_etaPhi_jetEl_HitMapWeighted",
        "#eta - #phi map of EM TowerSum weighted with energy  --  JEM input");
    m_h_jem_had_2d_etaPhi_jetEl_HitMapWeighted = m_histTool->bookJEMEtaVsPhi(
        "jem_had_2d_etaPhi_jetEl_HitMapWeighted",
        "#eta - #phi map of HAD TowerSum weighted with energy  --  JEM input");

    std::string name, title;
    std::stringstream buffer;

    m_v_jem_em_2d_etaPhi_jetEl_HitMapSlice.clear();
    m_v_jem_had_2d_etaPhi_jetEl_HitMapSlice.clear();
    for (int i = 0; i < m_SliceNo; i++) {
      buffer.str("");
      buffer << i;

      name = "jem_em_2d_etaPhi_jetEl_HitMapSlice" + buffer.str();
      title = "#eta - #phi map of EM TowerSum for Timeslice " + buffer.str() +
              "  --  JEM input";
      m_v_jem_em_2d_etaPhi_jetEl_HitMapSlice.push_back(
          m_histTool->bookJEMEtaVsPhi(name, title));

      name = "jem_had_2d_etaPhi_jetEl_HitMapSlice" + buffer.str();
      title = "#eta - #phi map of HAD TowerSum for Timeslice " + buffer.str() +
              "  --  JEM input";
      m_v_jem_had_2d_etaPhi_jetEl_HitMapSlice.push_back(
          m_histTool->bookJEMEtaVsPhi(name, title));
    }

    //---------------------------- DAQ histos -----------------------------

    m_histTool->setMonGroup(&JEM_Thresholds);

    m_h_jem_1d_thresh_MainHits = m_histTool->bookMainJetThresholds(
        "jem_1d_thresh_MainHits",
        "Main Jet Hit Multiplicity per Threshold  --  JEM DAQ");
    m_h_jem_1d_thresh_FwdHitsRight = m_histTool->bookForwardJetThresholds(
        "jem_1d_thresh_FwdHitsRight",
        "Fwd Right Jet Hit Multiplicity per Threshold  --  JEM DAQ");
    m_h_jem_1d_thresh_FwdHitsLeft = m_histTool->bookBackwardJetThresholds(
        "jem_1d_thresh_FwdHitsLeft",
        "Fwd Left Jet Hit Multiplicity per Threshold  --  JEM DAQ");

    m_h_jem_2d_thresh_HitsPerJem = m_histTool->bookJEMCrateModuleVsThresholds(
        "jem_2d_thresh_HitsPerJem", "HitMap of Hits per JEM");

    m_histTool->setMonGroup(&JEM_EnergySums);

    m_h_jem_1d_energy_SubSumsEx = m_histTool->bookJEMQuadLinear(
        "jem_1d_energy_SubSumsEx", "JEM E_{x}^{JEM}  --  JEM DAQ;Ex [GeV]");
    m_h_jem_1d_energy_SubSumsEy = m_histTool->bookJEMQuadLinear(
        "jem_1d_energy_SubSumsEy", "JEM E_{y}^{JEM}  --  JEM DAQ;Ey [GeV]");
    m_h_jem_1d_energy_SubSumsEt = m_histTool->bookJEMQuadLinear(
        "jem_1d_energy_SubSumsEt", "JEM E_{t}^{JEM}  --  JEM DAQ;Et [GeV]");

    //---------------------------- RoI histos -----------------------------

    m_histTool->setMonGroup(&JEM_RoI);

    m_h_jem_1d_roi_MainHits = m_histTool->bookMainJetThresholds(
        "jem_1d_roi_MainHits",
        "Main Jet Hit Multiplicity per Threshold  --  JEM RoI");
    m_h_jem_1d_roi_FwdHitsRight = m_histTool->bookForwardJetThresholds(
        "jem_1d_roi_FwdHitsRight",
        "Forward Right Jet Hit Multiplicity per Threshold  --  JEM RoI");
    m_h_jem_1d_roi_FwdHitsLeft = m_histTool->bookBackwardJetThresholds(
        "jem_1d_roi_FwdHitsLeft",
        "Forward Left Jet Hit Multiplicity per Threshold  --  JEM RoI");
    m_h_jem_2d_etaPhi_roi_Saturation = m_histTool->bookJEMRoIEtaVsPhi(
        "jem_2d_etaPhi_roi_Saturation", "JEM RoI Saturation");

    //----------------------- HitThreshold per Eta-Phi -----------------------

    m_v_jem_2d_etaPhi_roi_MainThresh.clear();
    m_v_jem_2d_etaPhi_roi_FwdThresh.clear();
    std::vector<std::string> jetNames;
    std::vector<std::string> jfNames;
    std::vector<std::string> jbNames;
    m_histTool->thresholdNames(TrigConf::L1DataDef::jetType(), jetNames);
    m_histTool->thresholdNames(TrigConf::L1DataDef::jfType(), jfNames);
    m_histTool->thresholdNames(TrigConf::L1DataDef::jbType(), jbNames);

    // Fix ATLASRECTS-781 bug https://its.cern.ch/jira/browse/ATLASRECTS-781
    // L1 theresholds for RUN2 (l1version==1) have 0  threshholds for JET, FJ
    // and JB
    // This code is back compativle with RUN1 (l1version==0)
    // More on theresholds configuration see:
    //    TrigConf::L1DataDef::setMaxThresholdsFromL1Versionl1version
    for (size_t i = 0; i < jetNames.size(); i++) {
      buffer.str("");
      buffer << i;
      name = "jem_2d_etaPhi_roi_MainThresh" + buffer.str();
      title = "#eta - #phi Map of Main Hits passing Threshold " + jetNames[i] +
              "  --  JEM RoI";
      m_v_jem_2d_etaPhi_roi_MainThresh.push_back(
          m_histTool->bookJEMRoIEtaVsPhi(name.c_str(), title.c_str()));

      if ((i >= jfNames.size()) || (i >= jbNames.size()))
        continue;

      name = "jem_2d_etaPhi_roi_FwdThresh" + buffer.str();
      title = "#eta - #phi Map of Fwd Hits passing Threshold " + jfNames[i];
      if (jfNames[i] != jbNames[i])
        title += "/" + jbNames[i];
      title += "  --  JEM RoI";
      m_v_jem_2d_etaPhi_roi_FwdThresh.push_back(
          m_histTool->bookJEMRoIEtaVsPhi(name.c_str(), title.c_str()));
    }

    //--------------------------- Error Histos -------------------------------

    m_histTool->setMonGroup(&JEM_ErrorDetail);

    m_h_jem_2d_Status = m_histTool->bookJEMSubStatusVsCrateModule(
        "jem_2d_Status", "Error reports from JEM SubStatus Word");
    m_h_jem_em_2d_etaPhi_jetEl_Parity = m_histTool->bookJEMEtaVsPhi(
        "jem_em_2d_etaPhi_jetEl_Parity", "Jet Element EM Parity Errors");
    m_h_jem_had_2d_etaPhi_jetEl_Parity = m_histTool->bookJEMEtaVsPhi(
        "jem_had_2d_etaPhi_jetEl_Parity", "Jet Element Had Parity Errors");
    m_h_jem_em_2d_etaPhi_jetEl_LinkDown = m_histTool->bookJEMEtaVsPhi(
        "jem_em_2d_etaPhi_jetEl_LinkDown", "Jet Element EM Link Down Errors");
    m_h_jem_had_2d_etaPhi_jetEl_LinkDown = m_histTool->bookJEMEtaVsPhi(
        "jem_had_2d_etaPhi_jetEl_LinkDown", "Jet Element Had Link Down Errors");

    m_h_jem_2d_etaPhi_roi_Parity = m_histTool->bookJEMRoIEtaVsPhi(
        "jem_2d_etaPhi_roi_Parity", "JEM RoI Parity Errors");

    m_histTool->setMonGroup(&JEM_Error);

    m_h_jem_1d_ErrorSummary =
        m_histTool->book1F("jem_1d_ErrorSummary", "Summary of JEM Data Errors",
                           NumberOfSummaryBins, 0., NumberOfSummaryBins);

    m_histTool->setMonGroup(&JEM_ErrorEvents);

    m_h_jem_2d_ErrorEventNumbers = m_histTool->bookEventNumbers(
        "jem_2d_ErrorEventNumbers", "JEM Error Event Numbers",
        NumberOfSummaryBins, 0., NumberOfSummaryBins);

    LWHist::LWHistAxis *axis = m_h_jem_1d_ErrorSummary->GetXaxis();
    for (int i = 0; i < 2; ++i) {
      axis->SetBinLabel(1 + EMParity, "EM parity");
      axis->SetBinLabel(1 + HadParity, "Had parity");
      axis->SetBinLabel(1 + EMLink, "EM link");
      axis->SetBinLabel(1 + HadLink, "Had link");
      axis->SetBinLabel(1 + JEMStatus, "JEM status");
      axis->SetBinLabel(1 + RoIParity, "RoI parity");
      axis = m_h_jem_2d_ErrorEventNumbers->GetYaxis();
    }

    m_histTool->unsetMonGroup();
    m_histBooked = true;
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode JEMMon::fillHistograms()
/*---------------------------------------------------------*/
{
  const bool debug = msgLvl(MSG::DEBUG);

  if (!m_histBooked) {
    if (debug)
      msg(MSG::DEBUG) << "Histogram(s) not booked" << endmsg;
    return StatusCode::SUCCESS;
  }

  // Skip events believed to be corrupt

  if (m_errorTool->corrupt()) {
    if (debug)
      msg(MSG::DEBUG) << "Skipping corrupt event" << endmsg;
    return StatusCode::SUCCESS;
  }

  // Error vector for global overview
  std::vector<int> overview(2);

  using LVL1::DataError;

  // =========================================================================
  // ================= Container: JetElements ================================
  // =========================================================================

  // retrieve JetElements
  const JECollection *jetElements = 0;
  StatusCode sc = evtStore()->retrieve(jetElements, m_JetElementLocation);

  if (sc == StatusCode::FAILURE || !jetElements) {
    msg(MSG::INFO) << "No JetElements found in TES at " << m_JetElementLocation
                   << endmsg;
    return StatusCode::SUCCESS;
  }

  // Step over all cells
  LVL1::CoordToHardware ToHW;
  JECollection::const_iterator it_je;
  for (it_je = jetElements->begin(); it_je != jetElements->end(); ++it_je) {
    const double eta = (*it_je)->eta();
    const double phi = (*it_je)->phi();
    const LVL1::Coordinate coord(phi, eta);
    const int crate = ToHW.jepCrate(coord);
    const int module = ToHW.jepModule(coord);
    const int cord = ToHW.jepCoordinateWord(coord);
    const int emEnergy = (*it_je)->emEnergy();
    const int hadEnergy = (*it_je)->hadEnergy();

    if (debug) {
      msg(MSG::VERBOSE) << "JE has coords (eta,phi): " << eta << ", " << phi
                        << " and energies (Em,Had): " << emEnergy << ", "
                        << hadEnergy << " HW Crate:" << crate
                        << " Module: " << module << " " << cord << endmsg;
    }

    if (emEnergy > 0) {
      m_h_jem_em_1d_jetEl_Eta->Fill(eta, 1.);
      m_histTool->fillJEMPhi(m_h_jem_em_1d_jetEl_Phi, eta, phi, 1.);
      m_h_jem_em_1d_jetEl_Energy->Fill(emEnergy, 1.);
      m_histTool->fillJEMEtaVsPhi(m_h_jem_em_2d_etaPhi_jetEl_HitMapWeighted,
                                  eta, phi, emEnergy);
    }
    if (hadEnergy > 0) {
      m_h_jem_had_1d_jetEl_Eta->Fill(eta, 1.);
      m_histTool->fillJEMPhi(m_h_jem_had_1d_jetEl_Phi, eta, phi, 1.);
      m_h_jem_had_1d_jetEl_Energy->Fill(hadEnergy, 1.);
      m_histTool->fillJEMEtaVsPhi(m_h_jem_had_2d_etaPhi_jetEl_HitMapWeighted,
                                  eta, phi, hadEnergy);
    }

    // number of triggered slice
    m_h_jem_1d_TriggeredSlice->Fill((*it_je)->peak(), 1);

    // ----------------- HitMaps per time slice ------------------------------
    const std::vector<int> &emEnergyVec((*it_je)->emEnergyVec());
    const std::vector<int> &hadEnergyVec((*it_je)->hadEnergyVec());
    const int slicesEm = emEnergyVec.size();
    const int slicesHad = hadEnergyVec.size();
    for (int i = 0; i < m_SliceNo; i++) {
      if (i < slicesEm && emEnergyVec[i] > 0) {
        m_histTool->fillJEMEtaVsPhi(m_v_jem_em_2d_etaPhi_jetEl_HitMapSlice[i],
                                    eta, phi, 1.);
      }
      if (i < slicesHad && hadEnergyVec[i] > 0) {
        m_histTool->fillJEMEtaVsPhi(m_v_jem_had_2d_etaPhi_jetEl_HitMapSlice[i],
                                    eta, phi, 1.);
      }
    }

    // ----------------- Error Histos ----------------------------------------
    const DataError err((*it_je)->emError());
    const DataError haderr((*it_je)->hadError());

    const int ypos = crate * 16 + module;
    // EM Parity
    if (err.get(DataError::Parity)) {
      m_histTool->fillJEMEtaVsPhi(m_h_jem_em_2d_etaPhi_jetEl_Parity, eta, phi);
      m_histTool->fillEventNumber(m_h_jem_2d_ErrorEventNumbers, EMParity);
      m_h_jem_1d_ErrorSummary->Fill(EMParity);
      overview[crate] |= (1 << EMParity);
    }
    // HAD Parity
    if (haderr.get(DataError::Parity)) {
      m_histTool->fillJEMEtaVsPhi(m_h_jem_had_2d_etaPhi_jetEl_Parity, eta, phi);
      m_histTool->fillEventNumber(m_h_jem_2d_ErrorEventNumbers, HadParity);
      m_h_jem_1d_ErrorSummary->Fill(HadParity);
      overview[crate] |= (1 << HadParity);
    }
    // PPM Link down: em.
    if (err.get(DataError::LinkDown)) {
      m_histTool->fillJEMEtaVsPhi(m_h_jem_em_2d_etaPhi_jetEl_LinkDown, eta,
                                  phi);
      m_histTool->fillEventNumber(m_h_jem_2d_ErrorEventNumbers, EMLink);
      m_h_jem_1d_ErrorSummary->Fill(EMLink);
      overview[crate] |= (1 << EMLink);
    }
    // PPM Link down: had.
    if (haderr.get(DataError::LinkDown)) {
      m_histTool->fillJEMEtaVsPhi(m_h_jem_had_2d_etaPhi_jetEl_LinkDown, eta,
                                  phi);
      m_histTool->fillEventNumber(m_h_jem_2d_ErrorEventNumbers, HadLink);
      m_h_jem_1d_ErrorSummary->Fill(HadLink);
      overview[crate] |= (1 << HadLink);
    }

    // Errors from substatus word from ROD: JEM
    const int status = (err.error() >> LVL1::DataError::GLinkParity) & 0xff;
    if (status) {
      for (int bit = 0; bit < 8; ++bit) {
        if ((status >> bit) & 0x1)
          m_h_jem_2d_Status->Fill(bit, ypos);
      }
      m_histTool->fillEventNumber(m_h_jem_2d_ErrorEventNumbers, JEMStatus);
      m_h_jem_1d_ErrorSummary->Fill(JEMStatus);
      overview[crate] |= (1 << JEMStatus);
    }
  }

  // =========================================================================
  // ================= Container: JEM Hits ===================================
  // =========================================================================

  // retrieve JEMHits collection from storegate
  const JEMHitsCollection *JEMHits = 0;
  sc = evtStore()->retrieve(JEMHits, m_JEMHitsLocation);
  if (sc == StatusCode::FAILURE || !JEMHits) {
    msg(MSG::INFO) << "No JEMHits found in TES at " << m_JEMHitsLocation
                   << endmsg;
    return StatusCode::SUCCESS;
  }

  if (debug) {
    msg(MSG::DEBUG) << "-------------- JEM Hits ---------------" << endmsg;
  }

  // Step over all cells and process
  JEMHitsCollection::const_iterator it_JEMHits;
  for (it_JEMHits = JEMHits->begin(); it_JEMHits != JEMHits->end();
       ++it_JEMHits) {
    const int crate = (*it_JEMHits)->crate();
    const int module = (*it_JEMHits)->module();
    const int xpos = crate * 16 + module;
    const bool forward = (*it_JEMHits)->forward();
    const unsigned int jetHits = (*it_JEMHits)->JetHits();

    const int nBits = (forward) ? 2 : 3;
    m_histTool->fillThresholds(m_h_jem_1d_thresh_MainHits, jetHits, 8, nBits);
    m_histTool->fillXVsThresholds(m_h_jem_2d_thresh_HitsPerJem, xpos, jetHits,
                                  8, nBits);
    if (forward) {
      const int fwdHits = jetHits >> 16;
      const int offset = (module % 8 == 0) ? 8 : 12;
      TH1F_LW *fwdHist = (module % 8 == 0) ? m_h_jem_1d_thresh_FwdHitsLeft
                                           : m_h_jem_1d_thresh_FwdHitsRight;
      m_histTool->fillThresholds(fwdHist, fwdHits, 4, nBits);
      m_histTool->fillXVsThresholds(m_h_jem_2d_thresh_HitsPerJem, xpos, fwdHits,
                                    4, nBits, offset);
    }

    if (debug) {
      msg(MSG::DEBUG) << "Crate: " << crate << "  Module: " << module
                      << "  JetHits: "
                      << m_histTool->thresholdString(
                             jetHits, (forward) ? 12 : 8, nBits) << endmsg;
    }
  }

  // =========================================================================
  // ================= Container: JEM Et Sums ================================
  // =========================================================================

  const JEMEtSumsCollection *JEMEtSums = 0;
  sc = evtStore()->retrieve(JEMEtSums, m_JEMEtSumsLocation);
  if (sc == StatusCode::FAILURE || !JEMEtSums) {
    msg(MSG::INFO) << "No JEMEtSums found in TES at " << m_JEMEtSumsLocation
                   << endmsg;
    return StatusCode::SUCCESS;
  }

  if (debug) {
    msg(MSG::DEBUG) << "-------------- JEM Et Sums ---------------" << endmsg;
  }

  // Step over all cells
  JEMEtSumsCollection::const_iterator it_JEMEtSums;

  for (it_JEMEtSums = JEMEtSums->begin(); it_JEMEtSums != JEMEtSums->end();
       ++it_JEMEtSums) {
    // note: the energy values are compressed -> expand!
    const int ex = LVL1::QuadLinear::Expand((*it_JEMEtSums)->Ex());
    const int ey = LVL1::QuadLinear::Expand((*it_JEMEtSums)->Ey());
    const int et = LVL1::QuadLinear::Expand((*it_JEMEtSums)->Et());

    if (ex != 0)
      m_h_jem_1d_energy_SubSumsEx->Fill(ex, 1.);
    if (ey != 0)
      m_h_jem_1d_energy_SubSumsEy->Fill(ey, 1.);
    if (et != 0)
      m_h_jem_1d_energy_SubSumsEt->Fill(et, 1.);

    if (debug) {
      msg(MSG::DEBUG) << " JEMEtSums Crate: " << (*it_JEMEtSums)->crate()
                      << "  Module: " << (*it_JEMEtSums)->module()
                      << "   Ex: " << ex << "   Ey: " << ey << "   Et: " << et
                      << "   Et compressed: " << (*it_JEMEtSums)->Et()
                      << endmsg;
    }
  }

  // =========================================================================
  // ================= Container: JEM RoI ====================================
  // =========================================================================

  const JemRoiCollection *JEMRoIs = 0;
  sc = evtStore()->retrieve(JEMRoIs, m_JEMRoILocation);
  if (sc == StatusCode::FAILURE || !JEMRoIs) {
    msg(MSG::INFO) << "No JEM RoIs found in TES at" << m_JEMRoILocation
                   << endmsg;
    return StatusCode::SUCCESS;
  }

  if (debug) {
    msg(MSG::DEBUG) << "-------------- JEM RoIs ---------------" << endmsg;
  }

  // Step over all cells
  JemRoiCollection::const_iterator it_JEMRoIs;

  for (it_JEMRoIs = JEMRoIs->begin(); it_JEMRoIs != JEMRoIs->end();
       ++it_JEMRoIs) {
    const int crate = (*it_JEMRoIs)->crate();
    const int module = (*it_JEMRoIs)->jem();
    const int forward = (*it_JEMRoIs)->forward();
    const int roiHits = (*it_JEMRoIs)->hits();
    LVL1::JEPRoIDecoder decoder;
    const LVL1::CoordinateRange coordRange =
        decoder.coordinate((*it_JEMRoIs)->roiWord());
    const double eta = coordRange.eta();
    const double phi = coordRange.phi();

    uint8_t nHits = 8;
    TH1F_LW *hist = m_h_jem_1d_roi_MainHits;
    if (forward) {
      nHits = 4;
      hist = (module % 8 == 0) ? m_h_jem_1d_roi_FwdHitsLeft
                               : m_h_jem_1d_roi_FwdHitsRight;
    }
    m_histTool->fillThresholds(hist, roiHits, nHits, 1);

    // FIX ATR-11487
    if ((forward && m_v_jem_2d_etaPhi_roi_FwdThresh.size() >= nHits) ||
        (!forward && m_v_jem_2d_etaPhi_roi_MainThresh.size() >= nHits)) {
      for (uint8_t thr = 0; thr < nHits; ++thr) {
        const int hit = (roiHits >> thr) & 0x1;
        if (hit) {

          TH2F_LW *hist2 = (forward) ? m_v_jem_2d_etaPhi_roi_FwdThresh[thr]
                                     : m_v_jem_2d_etaPhi_roi_MainThresh[thr];
          m_histTool->fillJEMRoIEtaVsPhi(hist2, eta, phi, 1.);
        }
      }
    }

    if (debug) {
      msg(MSG::DEBUG) << "JEMRoI Word: " << MSG::hex << (*it_JEMRoIs)->roiWord()
                      << MSG::dec << "; Crate: " << crate << "; JEM: " << module
                      << "; forward: " << forward << "; Hits: "
                      << m_histTool->thresholdString(roiHits, nHits) << endmsg;
    }

    const DataError err((*it_JEMRoIs)->error());

    if (err.get(DataError::Parity)) {
      m_histTool->fillJEMRoIEtaVsPhi(m_h_jem_2d_etaPhi_roi_Parity, eta, phi);
      m_histTool->fillEventNumber(m_h_jem_2d_ErrorEventNumbers, RoIParity);
      m_h_jem_1d_ErrorSummary->Fill(RoIParity);
      overview[crate] |= (1 << RoIParity);
    }
    // saturation
    if (err.get(DataError::Overflow)) {
      m_histTool->fillJEMRoIEtaVsPhi(m_h_jem_2d_etaPhi_roi_Saturation, eta,
                                     phi);
    }
  }

  // Write overview vector to StoreGate
  std::vector<int> *save = new std::vector<int>(overview);
  sc = evtStore()->record(save, "L1CaloJEMErrorVector");
  if (sc != StatusCode::SUCCESS) {
    msg(MSG::ERROR) << "Error recording JEM error vector in TES " << endmsg;
    return sc;
  }

  if (debug) {
    msg(MSG::DEBUG) << "--------------------------------------" << endmsg;
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode JEMMon::procHistograms()
/*---------------------------------------------------------*/
{
  msg(MSG::DEBUG) << "in procHistograms" << endmsg;

  // if( endOfLumiBlock || endOfRun ) { }

  return StatusCode::SUCCESS;
}

// ============================================================================
} // end namespace
// ============================================================================
