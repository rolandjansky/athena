/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:        JEPJEMMon.cxx
// PACKAGE:     TrigT1CaloMonitoring
//
// AUTHOR:      Johanna Fleckner (Johanna.Fleckner@uni-mainz.de)
//
// DESCRIPTION: Monitoring of the JEP on JEM level
//
// ********************************************************************

#include <cmath>
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
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramTool.h"

#include "xAODTrigL1Calo/JEMEtSumsContainer.h"
#include "xAODTrigL1Calo/JetElementContainer.h"
#include "xAODTrigL1Calo/JEMTobRoIContainer.h"
#include "TrigT1CaloUtils/DataError.h"
#include "TrigT1CaloUtils/CoordToHardware.h"
#include "TrigT1Interfaces/CoordinateRange.h"
#include "TrigT1Interfaces/Coordinate.h"
#include "TrigT1Interfaces/JEPRoIDecoder.h"                               // <<== Will change
#include "TrigT1Interfaces/TrigT1CaloDefs.h"                              // Ditto

#include "JEPJEMMon.h"
// ============================================================================
namespace LVL1 {
// ============================================================================
const int JEPJEMMon::s_crates;
const int JEPJEMMon::s_modules;
const int JEPJEMMon::s_tobsPerJEM;
// ============================================================================
JEPJEMMon::JEPJEMMon( const std::string & type, const std::string & name,
                      const IInterface* parent )
  : ManagedMonitorToolBase( type, name, parent ),
    m_errorTool("LVL1::TrigT1CaloMonErrorTool/TrigT1CaloMonErrorTool"),
    m_histTool("LVL1::TrigT1CaloLWHistogramTool/TrigT1CaloLWHistogramTool"),
    m_histBooked(false),
    m_h_jem_em_1d_jetEl_Eta(0),
    m_h_jem_had_1d_jetEl_Eta(0),
    m_h_jem_em_1d_jetEl_Phi(0),
    m_h_jem_had_1d_jetEl_Phi(0),
    m_h_jem_em_1d_jetEl_Energy(0),
    m_h_jem_had_1d_jetEl_Energy(0),
    m_h_jem_em_2d_etaPhi_jetEl_HitMapWeighted(0),
    m_h_jem_had_2d_etaPhi_jetEl_HitMapWeighted(0),
    m_v_jem_em_2d_etaPhi_jetEl_HitMapSlice(0),
    m_v_jem_had_2d_etaPhi_jetEl_HitMapSlice(0),
    m_h_jem_2d_Status(0),
    m_h_jem_em_2d_etaPhi_jetEl_Parity(0),
    m_h_jem_had_2d_etaPhi_jetEl_Parity(0),
    m_h_jem_em_2d_etaPhi_jetEl_LinkDown(0),
    m_h_jem_had_2d_etaPhi_jetEl_LinkDown(0),
    m_h_jem_1d_TriggeredSlice(0),
    m_h_jem_1d_energy_SubSumsEx(0),
    m_h_jem_1d_energy_SubSumsEy(0),
    m_h_jem_1d_energy_SubSumsEt(0),
    m_h_jem_1d_roi_EnergyLg(0),
    m_h_jem_1d_roi_EnergySm(0),
    m_h_jem_1d_roi_TOBsPerJEM(0),
    m_h_jem_2d_etaPhi_roi_Hitmap(0),
    m_h_jem_2d_etaPhi_roi_EtWeighted(0),
    m_h_jem_1d_ErrorSummary(0),
    m_h_jem_2d_ErrorEventNumbers(0)
    /*---------------------------------------------------------*/
{
  // This is how you declare the parameters to Gaudi so that
  // they can be over-written via the job options file

  declareProperty( "ErrorTool", m_errorTool);
  declareProperty( "HistogramTool", m_histTool);

  declareProperty( "JetElementLocation",
                   m_JetElementLocation = LVL1::TrigT1CaloDefs::JetElementLocation);
  declareProperty( "JEMEtSumsLocation",
                   m_JEMEtSumsLocation  = LVL1::TrigT1CaloDefs::JEMEtSumsLocation) ;
  declareProperty( "JEMTobRoILocation",
                   m_JEMRoILocation     = LVL1::TrigT1CaloDefs::JEMTobRoILocation) ;
  declareProperty( "ErrorLocation",
                   m_errorLocation      = "L1CaloJEMErrorVector");

  declareProperty( "NumberOfSlices", m_SliceNo = 5,
                   "Maximum possible number of slices");
  declareProperty( "MaxEnergyRange", m_MaxEnergyRange = 1024,
                   "Maximum energy in JetElement energy plots") ;

  declareProperty( "RootDirectory", m_rootDir = "L1Calo/JEM") ;

}


/*---------------------------------------------------------*/
JEPJEMMon::~JEPJEMMon()
/*---------------------------------------------------------*/
{
}

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

/*---------------------------------------------------------*/
StatusCode JEPJEMMon::initialize()
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
StatusCode JEPJEMMon::bookHistogramsRecurrent()
/*---------------------------------------------------------*/
{
  msg(MSG::DEBUG) << "in JEPJEMMon::bookHistogramsRecurrent" << endmsg;

  if ( m_environment == AthenaMonManager::online ) {
    // book histograms that are only made in the online environment...
  }

  if ( m_dataType == AthenaMonManager::cosmics ) {
    // book histograms that are only relevant for cosmics data...
  }

  //if ( newEventsBlock || newLumiBlock) { }

  if ( newRun ) {

    MgmtAttr_t attr = ATTRIB_UNMANAGED;
    MonGroup JetElements_expert(this, m_rootDir + "/Input", run, attr);
    MonGroup JetElements_shift(this, m_rootDir + "/Input", run, attr);
    MonGroup JEM_EnergySums(this, m_rootDir + "/Output/EnergySums", run, attr);
    MonGroup JEM_RoI(this, m_rootDir + "/Output/RoI", run, attr);
    std::string errorDir(m_rootDir + "/Errors/Hardware");
    MonGroup JEM_Error(this, errorDir, run, attr );
    MonGroup JEM_ErrorDetail(this, errorDir, run, attr );
    MonGroup JEM_ErrorEvents(this, errorDir, run, attr, "", "eventSample" );

    //-------------------------- JetElements histos --------------------------

    m_histTool->setMonGroup(&JetElements_expert);

    m_h_jem_em_1d_jetEl_Eta = m_histTool->bookJEMEta("jem_em_1d_jetEl_Eta",
                              "EM TowerSum distribution per #eta  --  JEM input");
    m_h_jem_had_1d_jetEl_Eta = m_histTool->bookJEMEta("jem_had_1d_jetEl_Eta",
                               "HAD TowerSum distribution per #eta  --  JEM input");

    m_h_jem_em_1d_jetEl_Phi = m_histTool->book1F("jem_em_1d_jetEl_Phi",
                              "EM TowerSum distribution per #phi  --  JEM input;phi", 32, 0., 2.*M_PI);
    m_h_jem_had_1d_jetEl_Phi = m_histTool->book1F("jem_had_1d_jetEl_Phi",
                               "HAD TowerSum distribution per #phi  --  JEM input;phi", 32, 0., 2.*M_PI);

    int jebins = 256;
    if (m_MaxEnergyRange < jebins) jebins = m_MaxEnergyRange;
    m_h_jem_em_1d_jetEl_Energy = m_histTool->book1F("jem_em_1d_jetEl_Energy",
                                 "TowerSum EM energy distribution  --  JEM input;em energy [GeV]",
                                 jebins - 1, 1, m_MaxEnergyRange);
    m_h_jem_had_1d_jetEl_Energy = m_histTool->book1F("jem_had_1d_jetEl_Energy",
                                  "TowerSum HAD energy distribution  --  JEM input;had energy [GeV]",
                                  jebins - 1, 1, m_MaxEnergyRange);

    // number of triggered slice
    m_h_jem_1d_TriggeredSlice = m_histTool->book1F("jem_1d_TriggeredSlice",
                                "Number of the Triggered Slice for JE;#Slice", m_SliceNo, 0, m_SliceNo);
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

      name  = "jem_em_2d_etaPhi_jetEl_HitMapSlice" + buffer.str();
      title = "#eta - #phi map of EM TowerSum for Timeslice " + buffer.str()
              +  "  --  JEM input";
      m_v_jem_em_2d_etaPhi_jetEl_HitMapSlice.push_back(
        m_histTool->bookJEMEtaVsPhi(name, title));

      name  = "jem_had_2d_etaPhi_jetEl_HitMapSlice" + buffer.str();
      title = "#eta - #phi map of HAD TowerSum for Timeslice " + buffer.str()
              +  "  --  JEM input";
      m_v_jem_had_2d_etaPhi_jetEl_HitMapSlice.push_back(
        m_histTool->bookJEMEtaVsPhi(name, title));
    }

    //---------------------------- DAQ histos -----------------------------

    m_histTool->setMonGroup(&JEM_EnergySums);

    const double maxSum = 16384.;
    const int nbins = 256;
    m_h_jem_1d_energy_SubSumsEx = m_histTool->book1F(
                                    "jem_1d_energy_SubSumsEx", "JEM E_{x}^{JEM}  --  JEM DAQ;Ex [GeV]",
                                    nbins, 0., maxSum);
    m_h_jem_1d_energy_SubSumsEy = m_histTool->book1F(
                                    "jem_1d_energy_SubSumsEy", "JEM E_{y}^{JEM}  --  JEM DAQ;Ey [GeV]",
                                    nbins, 0., maxSum);
    m_h_jem_1d_energy_SubSumsEt = m_histTool->book1F(
                                    "jem_1d_energy_SubSumsEt", "JEM E_{t}^{JEM}  --  JEM DAQ;Et [GeV]",
                                    nbins, 0., maxSum);

    //---------------------------- RoI histos -----------------------------

    m_histTool->setMonGroup(&JEM_RoI);

    const double maxLarge = 1024.;
    const double maxSmall = 512.;
    m_h_jem_1d_roi_EnergyLg = m_histTool->book1F("jem_1d_roi_EnergyLg",
                              "JEM TOB RoI Energy Large Window Size", nbins, 0., maxLarge);
    m_h_jem_1d_roi_EnergySm = m_histTool->book1F("jem_1d_roi_EnergySm",
                              "JEM TOB RoI Energy Small Window Size", nbins, 0., maxSmall);
    m_h_jem_1d_roi_TOBsPerJEM = m_histTool->book1F("jem_1d_roi_TOBsPerJEM",
                                "JEM TOB RoI TOBs per JEM;Number of TOBs", s_tobsPerJEM + 1, 1, s_tobsPerJEM + 2);
    m_histTool->numbers(m_h_jem_1d_roi_TOBsPerJEM, 1, s_tobsPerJEM);
    m_h_jem_1d_roi_TOBsPerJEM->GetXaxis()->SetBinLabel(s_tobsPerJEM + 1, "More");
    m_h_jem_2d_etaPhi_roi_Hitmap = m_histTool->bookJEMRoIEtaVsPhi(
                                     "jem_2d_etaPhi_roi_Hitmap", "JEM TOB RoIs Hit Map");
    m_h_jem_2d_etaPhi_roi_EtWeighted = m_histTool->bookJEMRoIEtaVsPhi(
                                         "jem_2d_etaPhi_roi_EtWeighted", "JEM TOB RoIs Hit Map Weighted by Energy");

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

    m_histTool->setMonGroup(&JEM_Error);

    m_h_jem_1d_ErrorSummary = m_histTool->book1F(
                                "jem_1d_ErrorSummary", "Summary of JEM Data Errors",
                                NumberOfSummaryBins, 0., NumberOfSummaryBins);

    m_histTool->setMonGroup(&JEM_ErrorEvents);

    m_h_jem_2d_ErrorEventNumbers = m_histTool->bookEventNumbers(
                                     "jem_2d_ErrorEventNumbers", "JEM Error Event Numbers",
                                     NumberOfSummaryBins, 0., NumberOfSummaryBins);

    LWHist::LWHistAxis* axis = m_h_jem_1d_ErrorSummary->GetXaxis();
    for (int i = 0; i < 2; ++i) {
      axis->SetBinLabel(1 + EMParity,  "EM parity");
      axis->SetBinLabel(1 + HadParity, "Had parity");
      axis->SetBinLabel(1 + EMLink,    "EM link");
      axis->SetBinLabel(1 + HadLink,   "Had link");
      axis->SetBinLabel(1 + JEMStatus, "JEM status");
      axis = m_h_jem_2d_ErrorEventNumbers->GetYaxis();
    }

    m_histTool->unsetMonGroup();
    m_histBooked = true;
  }

  return StatusCode::SUCCESS;
}



/*---------------------------------------------------------*/
StatusCode JEPJEMMon::fillHistograms()
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

  using LVL1::DataError;

  // =========================================================================
  // ================= Container: JetElements ================================
  // =========================================================================

  // retrieve JetElements
  const xAOD::JetElementContainer* jetElements = 0;
  StatusCode sc = evtStore()->retrieve(jetElements, m_JetElementLocation);

  if (sc == StatusCode::FAILURE || !jetElements) {
    msg(MSG::INFO) << "No JetElements found in TES at " << m_JetElementLocation
                   << endmsg;
    return StatusCode::SUCCESS;
  }

  // Step over all cells
  LVL1::CoordToHardware ToHW;
  xAOD::JetElementContainer::const_iterator it_je ;
  for (it_je = jetElements->begin(); it_je != jetElements->end(); ++it_je) {
    const double eta = (*it_je)->eta();
    const double phi = (*it_je)->phi();
    const LVL1::Coordinate coord(phi, eta);
    const int crate  = ToHW.jepCrate(coord);
    const int module = ToHW.jepModule(coord);
    const int cord   = ToHW.jepCoordinateWord(coord);
    const int emEnergy  = (*it_je)->emJetElementET();
    const int hadEnergy = (*it_je)->hadJetElementET();

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
      m_h_jem_had_1d_jetEl_Eta -> Fill(eta, 1.);
      m_histTool->fillJEMPhi(m_h_jem_had_1d_jetEl_Phi, eta, phi, 1.);
      m_h_jem_had_1d_jetEl_Energy->Fill(hadEnergy, 1.);
      m_histTool->fillJEMEtaVsPhi(m_h_jem_had_2d_etaPhi_jetEl_HitMapWeighted,
                                  eta, phi, hadEnergy);
    }

    // number of triggered slice
    m_h_jem_1d_TriggeredSlice->Fill((*it_je)->peak(), 1);

    // ----------------- HitMaps per time slice ------------------------------
    const std::vector<uint16_t>& emEnergyVec((*it_je)->emJetElementETVec());
    const std::vector<uint16_t>& hadEnergyVec((*it_je)->hadJetElementETVec());
    const int slicesEm  = emEnergyVec.size();
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
    const DataError err((*it_je)->emJetElementError());
    const DataError haderr((*it_je)->hadJetElementError());

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
      m_histTool->fillJEMEtaVsPhi(m_h_jem_em_2d_etaPhi_jetEl_LinkDown, eta, phi);
      m_histTool->fillEventNumber(m_h_jem_2d_ErrorEventNumbers, EMLink);
      m_h_jem_1d_ErrorSummary->Fill(EMLink);
      overview[crate] |= (1 << EMLink);
    }
    // PPM Link down: had.
    if (haderr.get(DataError::LinkDown)) {
      m_histTool->fillJEMEtaVsPhi(m_h_jem_had_2d_etaPhi_jetEl_LinkDown, eta, phi);
      m_histTool->fillEventNumber(m_h_jem_2d_ErrorEventNumbers, HadLink);
      m_h_jem_1d_ErrorSummary->Fill(HadLink);
      overview[crate] |= (1 << HadLink);
    }

    //Errors from substatus word from ROD: JEM
    const int status = (err.error() >> LVL1::DataError::GLinkParity) & 0xff;
    if (status) {
      for (int bit = 0; bit < 8; ++bit) {
        if ((status >> bit) & 0x1) m_h_jem_2d_Status->Fill(bit, ypos);
      }
      m_histTool->fillEventNumber(m_h_jem_2d_ErrorEventNumbers, JEMStatus);
      m_h_jem_1d_ErrorSummary->Fill(JEMStatus);
      overview[crate] |= (1 << JEMStatus);
    }
  }

  // =========================================================================
  // ================= Container: JEM Et Sums ================================
  // =========================================================================

  const xAOD::JEMEtSumsContainer* JEMEtSums = 0;
  sc = evtStore()->retrieve(JEMEtSums, m_JEMEtSumsLocation);
  if (sc == StatusCode::FAILURE || !JEMEtSums) {
    msg(MSG::INFO) << "No JEMEtSums found in TES at " << m_JEMEtSumsLocation
                   << endmsg ;
    return StatusCode::SUCCESS;
  }

  if (debug) {
    msg(MSG::DEBUG) << "-------------- JEM Et Sums ---------------" << endmsg;
  }

  // Step over all cells
  xAOD::JEMEtSumsContainer::const_iterator it_JEMEtSums ;

  for (it_JEMEtSums = JEMEtSums->begin(); it_JEMEtSums != JEMEtSums->end();
       ++it_JEMEtSums) {
    const int ex = (*it_JEMEtSums)->ex();
    const int ey = (*it_JEMEtSums)->ey();
    const int et = (*it_JEMEtSums)->et();

    if (ex != 0) m_h_jem_1d_energy_SubSumsEx->Fill(ex, 1.);
    if (ey != 0) m_h_jem_1d_energy_SubSumsEy->Fill(ey, 1.);
    if (et != 0) m_h_jem_1d_energy_SubSumsEt->Fill(et, 1.);

    if (debug) {
      msg(MSG::DEBUG) << " JEMEtSums Crate: " << (*it_JEMEtSums)->crate()
                      << "  Module: "         << (*it_JEMEtSums)->module()
                      << "   Ex: "            <<  ex
                      << "   Ey: "            <<  ey
                      << "   Et: "            <<  et
                      << endmsg;
    }
  }

  // =========================================================================
  // ================= Container: JEM RoI ====================================
  // =========================================================================

  const xAOD::JEMTobRoIContainer* JEMRoIs = 0;
  sc = evtStore()->retrieve (JEMRoIs, m_JEMRoILocation);
  if (sc == StatusCode::FAILURE || !JEMRoIs) {
    msg(MSG::INFO) << "No JEM RoIs found in TES at" << m_JEMRoILocation
                   << endmsg;
    return StatusCode::SUCCESS;
  }

  if (debug) {
    msg(MSG::DEBUG) << "-------------- JEM RoIs ---------------" << endmsg;
  }

  // Step over all cells
  xAOD::JEMTobRoIContainer::const_iterator it_JEMRoIs ;

  const int vecSize = s_crates * s_modules;
  std::vector<int> tobCount(vecSize);
  for (it_JEMRoIs = JEMRoIs->begin(); it_JEMRoIs != JEMRoIs->end();
       ++it_JEMRoIs) {
    const int crate    = (*it_JEMRoIs)->crate();
    const int module   = (*it_JEMRoIs)->jem();
    const int energyLg = (*it_JEMRoIs)->energyLarge();
    const int energySm = (*it_JEMRoIs)->energySmall();
    LVL1::JEPRoIDecoder decoder;
    const LVL1::CoordinateRange coordRange =
      decoder.coordinate((*it_JEMRoIs)->roiWord());        //<<== CHECK
    const double eta = coordRange.eta();
    const double phi = coordRange.phi();

    if (energyLg) {
      m_h_jem_1d_roi_EnergyLg->Fill(energyLg);
      m_histTool->fillJEMRoIEtaVsPhi(m_h_jem_2d_etaPhi_roi_Hitmap, eta, phi);
      m_histTool->fillJEMRoIEtaVsPhi(m_h_jem_2d_etaPhi_roi_EtWeighted,
                                     eta, phi, energyLg);
    }
    if (energySm) {
      m_h_jem_1d_roi_EnergySm->Fill(energySm);
    }
    ++tobCount[crate * s_modules + module];
  }

  for (int crate = 0; crate < s_crates; ++crate) {
    for (int jem = 0; jem < s_modules; ++jem) {
      int val = tobCount[crate * s_modules + jem];
      if (val) {
        if (val > s_tobsPerJEM) val = s_tobsPerJEM + 1;
        m_h_jem_1d_roi_TOBsPerJEM->Fill(val);
      }
    }
  }

  // Write overview vector to StoreGate
  std::vector<int>* save = new std::vector<int>(overview);
  sc = evtStore()->record(save, m_errorLocation);
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
StatusCode JEPJEMMon::procHistograms()
/*---------------------------------------------------------*/
{
  msg(MSG::DEBUG) << "in procHistograms" << endmsg ;

  //if ( endOfEventsBlock || endOfLumiBlock || endOfRun ) { }

  return StatusCode::SUCCESS;
}

} // end namespace
