/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     OverviewMon.cxx
// PACKAGE:  TrigT1CaloMonitoring
//
// AUTHOR:   Peter Faulkner
//
//
// ********************************************************************

#include <ctime>
#include <sstream>

#include "TAxis.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TList.h"

#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "SGTools/StlVectorClids.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventInfo.h"

#include "TrigT1CaloMonitoringTools/ITrigT1CaloMonErrorTool.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramTool.h"

#include "OverviewMon.h"

namespace LVL1 {

// Interface ID
// static const InterfaceID IID_IOverviewMon("LVL1::OverviewMon", 1, 1);
// const InterfaceID& OverviewMon::interfaceID() {
// return IID_IOverviewMon;
//}

/*---------------------------------------------------------*/
OverviewMon::OverviewMon(const std::string &type, const std::string &name,
                         const IInterface *parent)
    : ManagedMonitorToolBase(type, name, parent),
      m_errorTool("LVL1::TrigT1CaloMonErrorTool/TrigT1CaloMonErrorTool"),
      m_histTool("LVL1::TrigT1CaloLWHistogramTool/TrigT1CaloLWHistogramTool"),
      m_lumiNo(0), m_lumipos(0), m_h_l1calo_2d_GlobalOverview(0),
      m_h_l1calo_2d_CurrentEventOverview(0),
      m_h_l1calo_2d_GlobalOverviewRecent(0), m_h_l1calo_1d_ErrorsByLumiblock(0),
      m_h_l1calo_1d_ErrorsByTime(0), m_h_l1calo_1d_RejectedEvents(0),
      m_h_l1calo_1d_NumberOfEvents(0),
      m_registred_h_l1calo_1d_ErrorsByLumiblock(false),
      m_registred_h_l1calo_1d_ErrorsByTime(false),
      m_registred_h_l1calo_1d_RejectedEvents(false)

/*---------------------------------------------------------*/
{
  declareProperty("ErrorTool", m_errorTool);
  declareProperty("HistogramTool", m_histTool);

  declareProperty("PPMErrorLocation",
                  m_ppmErrorLocation = "L1CaloPPMErrorVector");
  declareProperty("PPMSpareErrorLocation",
                  m_ppmSpareErrorLocation = "L1CaloPPMSpareErrorVector");
  declareProperty("CPMErrorLocation",
                  m_cpmErrorLocation = "L1CaloCPMErrorVector");
  declareProperty("JEMErrorLocation",
                  m_jemErrorLocation = "L1CaloJEMErrorVector");
  declareProperty("JEMCMXErrorLocation",
                  m_jemCmxErrorLocation = "L1CaloJEMCMXErrorVector");
  declareProperty("RODErrorLocation",
                  m_rodErrorLocation = "L1CaloRODErrorVector");
  declareProperty("PPMMismatchLocation",
                  m_ppmMismatchLocation = "L1CaloPPMMismatchVector");
  declareProperty("CPMMismatchLocation",
                  m_cpmMismatchLocation = "L1CaloCPMMismatchVector");
  declareProperty("JEMMismatchLocation",
                  m_jemMismatchLocation = "L1CaloJEMMismatchVector");
  declareProperty("RootDirectory", m_rootDir = "L1Calo/Overview");
  declareProperty("RecentLumiBlocks", m_recentLumi = 10,
                  "Number of lumiblocks in recent lumiblocks plot");
  declareProperty("OnlineTest", m_onlineTest = false,
                  "Test online code when running offline");
}

/*---------------------------------------------------------*/
OverviewMon::~OverviewMon()
/*---------------------------------------------------------*/
{}

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

/*---------------------------------------------------------*/
StatusCode OverviewMon::initialize()
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
    msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloHistogramTool"
                    << endmsg;
    return sc;
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode OverviewMon::finalize()
/*---------------------------------------------------------*/
{
  delete m_h_l1calo_2d_CurrentEventOverview;
  if (!m_v_l1calo_2d_GlobalOverviewBlock.empty()) {
    for (int i = 0; i < m_recentLumi; ++i) {
      delete m_v_l1calo_2d_GlobalOverviewBlock[i];
    }
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode OverviewMon::bookHistogramsRecurrent()
/*---------------------------------------------------------*/
{
  msg(MSG::DEBUG) << "bookHistogramsRecurrent entered" << endmsg;

  if (m_environment == AthenaMonManager::online) {
    // book histograms that are only made in the online environment...
  }

  if (m_dataType == AthenaMonManager::cosmics) {
    // book histograms that are only relevant for cosmics data...
  }

  //if (newEventsBlock || newLumiBlock) {
  //}

  bool online = (m_onlineTest || m_environment == AthenaMonManager::online);

  if (newRun || newLumiBlock) {
    // Get lumiblock number

    m_lumiNo = 0;
    const EventInfo *evtInfo = 0;
    StatusCode sc = evtStore()->retrieve(evtInfo);
    if (sc.isSuccess()) {
      m_lumiNo = evtInfo->event_ID()->lumi_block();
    }
  }

  if ((newLumiBlock && !online) || newRun) {
    std::string dir(m_rootDir + "/Errors");
    MonGroup monGlobal(this, dir, (newLumiBlock && !online) ? lumiBlock : run,
                       ATTRIB_UNMANAGED);

    // Global Error Overview

    m_histTool->setMonGroup(&monGlobal);

    m_h_l1calo_2d_GlobalOverview = bookOverview("l1calo_2d_GlobalOverview",
                                                "L1Calo Global Error Overview");

    m_histTool->unsetMonGroup();

    if (!m_h_l1calo_2d_CurrentEventOverview) { // temporary plot for current
                                               // event
      m_h_l1calo_2d_CurrentEventOverview =
          bookOverview("l1calo_2d_CurrentEventOverview",
                       "L1Calo Current Event Error Overview");
    }

    if (online) {
      // Overview for last few lumiblocks

      m_histTool->setMonGroup(&monGlobal);

      std::ostringstream luminum;
      luminum << m_recentLumi;
      m_h_l1calo_2d_GlobalOverviewRecent = bookOverview(
          "l1calo_2d_GlobalOverviewRecent",
          "L1Calo Global Error Overview Last " + luminum.str() + " Lumiblocks");

      m_histTool->unsetMonGroup();

      // Temporary plots for each recent lumiblock

      if (m_v_l1calo_2d_GlobalOverviewBlock.empty()) {
        for (int i = 0; i < m_recentLumi; ++i) {
          std::ostringstream cnum;
          cnum << i;
          TH2F *hist =
              bookOverview("l1calo_2d_GlobalOverviewBlock" + cnum.str(),
                           "L1Calo Global Error Overview Block " + cnum.str());
          m_v_l1calo_2d_GlobalOverviewBlock.push_back(hist);
          m_luminumbers.push_back(0);
        }
      } else {
        for (int i = 0; i < m_recentLumi; ++i) {
          m_v_l1calo_2d_GlobalOverviewBlock[i]->Reset();
          m_luminumbers[i] = 0;
        }
      }
      m_lumipos = 0;
      m_luminumbers[m_lumipos] = m_lumiNo;
    }
  } else if (newLumiBlock && online) {
    // Update last few lumiblocks plots

    m_lumipos = -1;
    for (int i = 0; i < m_recentLumi; ++i) {
      if (m_luminumbers[i] == m_lumiNo)
        m_lumipos = i;
    }
    if (m_lumipos == -1) {
      double entries =
          m_v_l1calo_2d_GlobalOverviewBlock[m_recentLumi - 1]->GetEntries();
      if (entries > 0.)
        m_h_l1calo_2d_GlobalOverviewRecent->Reset();
      m_v_l1calo_2d_GlobalOverviewBlock[m_recentLumi - 1]->Reset();
      TH2F *tmpHist = m_v_l1calo_2d_GlobalOverviewBlock[m_recentLumi - 1];
      for (int i = m_recentLumi - 2; i >= 0; --i) {
        if (entries > 0. &&
            m_v_l1calo_2d_GlobalOverviewBlock[i]->GetEntries() > 0.) {
          m_h_l1calo_2d_GlobalOverviewRecent->Add(
              m_v_l1calo_2d_GlobalOverviewBlock[i]);
        }
        m_v_l1calo_2d_GlobalOverviewBlock[i + 1] =
            m_v_l1calo_2d_GlobalOverviewBlock[i];
        m_luminumbers[i + 1] = m_luminumbers[i];
      }
      m_v_l1calo_2d_GlobalOverviewBlock[0] = tmpHist;
      m_lumipos = 0;
      m_luminumbers[m_lumipos] = m_lumiNo;
    }

  } // end if ((newLumiBlock && ...

  if (newRun || newLumiBlock) {
    // Errors by lumiblock/time plots
    // On Tier0 only kept if non-empty

    if (m_lumiNo) {
      if (newRun) {
        std::string dir(m_rootDir + "/Errors");
        MonGroup monLumi(this, dir, run, ATTRIB_UNMANAGED);
        if (online)
          m_histTool->setMonGroup(&monLumi);
        else
          m_histTool->unsetMonGroup();
        m_h_l1calo_1d_ErrorsByLumiblock = m_histTool->bookTH1F(
            "l1calo_1d_ErrorsByLumiblock",
            "Events with Errors by Lumiblock;Lumi Block;Number of Events", 1,
            m_lumiNo, m_lumiNo + 1);
        if (online) { // Would be merge problems offline
          m_h_l1calo_1d_ErrorsByTime = m_histTool->bookTH1F(
              "l1calo_1d_ErrorsByTime", "Time of First Event in Lumiblock with "
                                        "Error;Lumi Block;Time (h.mmss)",
              1, m_lumiNo, m_lumiNo + 1);
        } else {
          m_h_l1calo_1d_ErrorsByTime = m_histTool->bookTH1F(
              "l1calo_1d_ErrorsByTime",
              "Events with Errors by Time;Time (h.mm);Number of Events", 2400,
              0., 24.);
        }
        if (m_errorTool->flagCorruptEvents() != "None") {
          m_h_l1calo_1d_RejectedEvents = m_histTool->bookTH1F(
              "l1calo_1d_RejectedEvents",
              "Rejected Events by Lumiblock;Lumi Block;Number of Events", 1,
              m_lumiNo, m_lumiNo + 1);
        } else
          m_h_l1calo_1d_RejectedEvents = 0;

      } else if (m_lumiNo <
                     m_h_l1calo_1d_ErrorsByLumiblock->GetXaxis()->GetXmin() ||
                 m_lumiNo >=
                     m_h_l1calo_1d_ErrorsByLumiblock->GetXaxis()->GetXmax()) {
        m_histTool->unsetMonGroup();
        TH1F *tmphist = m_histTool->bookTH1F(
            "l1calo_1d_Tmp", "Errors by Lumiblock", 1, m_lumiNo, m_lumiNo + 1);
        tmphist->Fill(m_lumiNo);
        TList *list = new TList;
        list->Add(tmphist);
        // All this rigmarole is to get Merge() to behave as we want,
        // especially online.
        TH1F *hist = m_h_l1calo_1d_ErrorsByLumiblock;
        for (int i = 0; i < 3; ++i) {
          if (!hist)
            continue;
          double entries = hist->GetEntries();
          bool earlier = (m_lumiNo < hist->GetXaxis()->GetXmin());
          double content = 0.;
          if (earlier) {
            int lastBin = hist->GetXaxis()->GetNbins();
            content = hist->GetBinContent(lastBin);
            if (content == 0.)
              hist->SetBinContent(lastBin, 1.);
          } else {
            content = hist->GetBinContent(1);
            if (content == 0.)
              hist->SetBinContent(1, 1.);
          }
          if (hist->Merge(list) != -1) {
            int bin = hist->GetXaxis()->FindBin(m_lumiNo);
            hist->SetBinContent(bin, 0.);
            if (content == 0.) {
              if (earlier) {
                int lastBin = hist->GetXaxis()->GetNbins();
                hist->SetBinContent(lastBin, 0.);
              } else
                hist->SetBinContent(1, 0.);
            }
            hist->SetEntries(entries);
          }
          if (i == 0)
            hist = (online) ? m_h_l1calo_1d_ErrorsByTime : 0;
          else
            hist = m_h_l1calo_1d_RejectedEvents;
        }
        delete tmphist;
        delete list;
      }
    }
  }

  // Total events processed and total rejected as corrupt

  if (newRun) {
    MonGroup monEvents(this, m_rootDir, run, ATTRIB_UNMANAGED);
    m_histTool->setMonGroup(&monEvents);
    int bins = (m_errorTool->flagCorruptEvents() == "None") ? 1 : 2;
    m_h_l1calo_1d_NumberOfEvents =
        m_histTool->bookTH1F("l1calo_1d_NumberOfEvents",
                             "Number of processed events", bins, 0., bins);
    m_h_l1calo_1d_NumberOfEvents->GetXaxis()->SetBinLabel(1,
                                                          "Processed Events");
    if (bins > 1) {
      m_h_l1calo_1d_NumberOfEvents->GetXaxis()->SetBinLabel(
          2, "Corrupt Events Skipped");
    }
  }

  m_histTool->unsetMonGroup();

  msg(MSG::DEBUG) << "Leaving bookHistogramsRecurrent" << endmsg;

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode OverviewMon::fillHistograms()
/*---------------------------------------------------------*/
{
  const bool debug = msgLvl(MSG::DEBUG);
  if (debug)
    msg(MSG::DEBUG) << "fillHistograms entered" << endmsg;

  if (!m_h_l1calo_2d_GlobalOverview) {
    if (debug)
      msg(MSG::DEBUG) << "Histograms not booked" << endmsg;
    return StatusCode::SUCCESS;
  }

  const bool online =
      (m_onlineTest || m_environment == AthenaMonManager::online);

  // Total events and corrupt event by lumiblock plots

  if (m_errorTool->corrupt()) {
    m_h_l1calo_1d_NumberOfEvents->Fill(1.);
    if (m_lumiNo && m_h_l1calo_1d_RejectedEvents) {
      if (!online && m_h_l1calo_1d_RejectedEvents->GetEntries() == 0.) {
        if (!m_registred_h_l1calo_1d_RejectedEvents) {
          m_registred_h_l1calo_1d_RejectedEvents = true;
          std::string dir(m_rootDir + "/Errors");
          MonGroup monLumi(this, dir, run, ATTRIB_UNMANAGED, "",
                           "mergeRebinned");
          m_histTool->setMonGroup(&monLumi);
          m_histTool->registerHist(m_h_l1calo_1d_RejectedEvents);
        }
      }
      m_h_l1calo_1d_RejectedEvents->Fill(m_lumiNo);
    }
    return StatusCode::SUCCESS;
  } else
    m_h_l1calo_1d_NumberOfEvents->Fill(0.);

  StatusCode sc;

  // Update Global overview plot

  const int ppmCrates = 8;
  const int cpmCrates = 4;
  const int jemCrates = 2;
  m_h_l1calo_2d_CurrentEventOverview->Reset();

  // PPM Error data
  const ErrorVector *errTES = 0;
  if (evtStore()->contains<ErrorVector>(m_ppmErrorLocation)) {
    sc = evtStore()->retrieve(errTES, m_ppmErrorLocation);
  } else
    sc = StatusCode::FAILURE;
  if (sc.isFailure() || !errTES || errTES->size() != size_t(ppmCrates)) {
    if (debug)
      msg(MSG::DEBUG) << "No PPM error vector of expected size" << endmsg;
  } else {
    for (int crate = 0; crate < ppmCrates; ++crate) {
      const int err = (*errTES)[crate];
      if (err == 0)
        continue;
      if ((err >> DataStatus) & 0x1) {
        m_h_l1calo_2d_CurrentEventOverview->Fill(PPMDataStatus, crate);
      }
      if ((err >> DataError) & 0x1) {
        m_h_l1calo_2d_CurrentEventOverview->Fill(PPMDataError, crate);
      }
      if ((err >> PPMSubStatus) & 0x1) {
        m_h_l1calo_2d_CurrentEventOverview->Fill(SubStatus, crate);
      }
    }
  }

  // Spare PPM Channels Error data
  errTES = 0;
  if (evtStore()->contains<ErrorVector>(m_ppmSpareErrorLocation)) {
    sc = evtStore()->retrieve(errTES, m_ppmSpareErrorLocation);
  } else
    sc = StatusCode::FAILURE;
  if (sc.isFailure() || !errTES || errTES->size() != size_t(ppmCrates)) {
    if (debug)
      msg(MSG::DEBUG) << "No PPMSpare error vector of expected size" << endmsg;
  } else {
    for (int crate = 0; crate < ppmCrates; ++crate) {
      const int err = (*errTES)[crate];
      if (err == 0)
        continue;
      if ((err >> DataStatus) & 0x1) {
        m_h_l1calo_2d_CurrentEventOverview->Fill(PPMDataStatus, crate);
      }
      if ((err >> DataError) & 0x1) {
        m_h_l1calo_2d_CurrentEventOverview->Fill(PPMDataError, crate);
      }
      if ((err >> PPMSubStatus) & 0x1) {
        m_h_l1calo_2d_CurrentEventOverview->Fill(SubStatus, crate);
      }
    }
  }

  // CPM and CPM CMX Error data
  errTES = 0;
  if (evtStore()->contains<ErrorVector>(m_cpmErrorLocation)) {
    sc = evtStore()->retrieve(errTES, m_cpmErrorLocation);
  } else
    sc = StatusCode::FAILURE;
  if (sc.isFailure() || !errTES || errTES->size() != size_t(cpmCrates)) {
    if (debug)
      msg(MSG::DEBUG) << "No CPM error vector of expected size" << endmsg;
  } else {
    for (int crate = 0; crate < cpmCrates; ++crate) {
      const int err = (*errTES)[crate];
      if (err == 0)
        continue;
      const int cr = crate + ppmCrates;
      if ((err >> CPMStatus) & 0x1)
        m_h_l1calo_2d_CurrentEventOverview->Fill(SubStatus, cr);
      if (((err >> CPMEMParity) & 0x1) || ((err >> CPMHadParity) & 0x1))
        m_h_l1calo_2d_CurrentEventOverview->Fill(Parity, cr);
      if (((err >> CPMEMLink) & 0x1) || ((err >> CPMHadLink) & 0x1))
        m_h_l1calo_2d_CurrentEventOverview->Fill(LinkDown, cr);
      if ((err >> CMXCPTobParity) & 0x1)
        m_h_l1calo_2d_CurrentEventOverview->Fill(GbCMXParity, cr);
      if ((err >> CMXCPSumParity) & 0x1)
        m_h_l1calo_2d_CurrentEventOverview->Fill(GbCMXParity, cr);
      if ((err >> CMXCPStatus) & 0x1)
        m_h_l1calo_2d_CurrentEventOverview->Fill(CMXSubStatus, cr);
    }
  }

  // JEM Error data
  errTES = 0;
  if (evtStore()->contains<ErrorVector>(m_jemErrorLocation)) {
    sc = evtStore()->retrieve(errTES, m_jemErrorLocation);
  } else
    sc = StatusCode::FAILURE;
  if (sc.isFailure() || !errTES || errTES->size() != size_t(jemCrates)) {
    if (debug)
      msg(MSG::DEBUG) << "No JEM error vector of expected size" << endmsg;
  } else {
    for (int crate = 0; crate < jemCrates; ++crate) {
      const int err = (*errTES)[crate];
      if (err == 0)
        continue;
      const int cr = crate + ppmCrates + cpmCrates;
      if ((err >> JEMStatus) & 0x1)
        m_h_l1calo_2d_CurrentEventOverview->Fill(SubStatus, cr);
      if (((err >> JEMEMParity) & 0x1) || ((err >> JEMHadParity) & 0x1))
        m_h_l1calo_2d_CurrentEventOverview->Fill(Parity, cr);
      if (((err >> JEMEMLink) & 0x1) || ((err >> JEMHadLink) & 0x1))
        m_h_l1calo_2d_CurrentEventOverview->Fill(LinkDown, cr);
    }
  }

  // JEM CMX Error data
  errTES = 0;
  if (evtStore()->contains<ErrorVector>(m_jemCmxErrorLocation)) {
    sc = evtStore()->retrieve(errTES, m_jemCmxErrorLocation);
  } else
    sc = StatusCode::FAILURE;
  if (sc.isFailure() || !errTES || errTES->size() != size_t(jemCrates)) {
    if (debug)
      msg(MSG::DEBUG) << "No JEM CMX error vector of expected size" << endmsg;
  } else {
    for (int crate = 0; crate < jemCrates; ++crate) {
      const int err = (*errTES)[crate];
      if (err == 0)
        continue;
      const int cr = crate + ppmCrates + cpmCrates;
      if (((err >> JEMCMXJetStatus) & 0x1) ||
          ((err >> JEMCMXEnergyStatus) & 0x1)) {
        m_h_l1calo_2d_CurrentEventOverview->Fill(CMXSubStatus, cr);
      }
      if (((err >> JEMCMXJetParity) & 0x1) ||
          ((err >> JEMCMXEnergyParity) & 0x1)) {
        m_h_l1calo_2d_CurrentEventOverview->Fill(GbCMXParity, cr);
      }
    }
  }

  // ROD Error data
  errTES = 0;
  if (evtStore()->contains<ErrorVector>(m_rodErrorLocation)) {
    sc = evtStore()->retrieve(errTES, m_rodErrorLocation);
  } else
    sc = StatusCode::FAILURE;
  if (sc.isFailure() || !errTES ||
      errTES->size() != size_t(ppmCrates + cpmCrates + jemCrates)) {
    if (debug)
      msg(MSG::DEBUG) << "No ROD error vector of expected size" << endmsg;
  } else {
    for (int crate = 0; crate < ppmCrates + cpmCrates + jemCrates; ++crate) {
      const int err = (*errTES)[crate];
      if (err == 0)
        continue;
      // if (err & 0x7f) m_h_l1calo_2d_CurrentEventOverview->Fill(RODStatus,
      // crate);
      if (err & 0x3f)
        m_h_l1calo_2d_CurrentEventOverview->Fill(RODStatus, crate);
      if (((err >> NoFragment) & 0x1) || ((err >> NoPayload) & 0x1))
        m_h_l1calo_2d_CurrentEventOverview->Fill(RODMissing, crate);
      if ((err >> ROBStatusError) & 0x1)
        m_h_l1calo_2d_CurrentEventOverview->Fill(ROBStatus, crate);
      if ((err >> UnpackingError) & 0x1)
        m_h_l1calo_2d_CurrentEventOverview->Fill(Unpacking, crate);
    }
  }

  // PPM Mismatch data
  errTES = 0;
  if (evtStore()->contains<ErrorVector>(m_ppmMismatchLocation)) {
    sc = evtStore()->retrieve(errTES, m_ppmMismatchLocation);
  } else
    sc = StatusCode::FAILURE;
  if (sc.isFailure() || !errTES || errTES->size() != size_t(ppmCrates)) {
    if (debug)
      msg(MSG::DEBUG) << "No PPM mismatch vector of expected size" << endmsg;
  } else {
    for (int crate = 0; crate < ppmCrates; ++crate) {
      const int err = (*errTES)[crate];
      if (err == 0)
        continue;
      if (((err >> LUTMismatch) & 0x1))
        m_h_l1calo_2d_CurrentEventOverview->Fill(Simulation, crate);
    }
  }

  // CPM Mismatch data
  errTES = 0;
  if (evtStore()->contains<ErrorVector>(m_cpmMismatchLocation)) {
    sc = evtStore()->retrieve(errTES, m_cpmMismatchLocation);
  } else
    sc = StatusCode::FAILURE;
  if (sc.isFailure() || !errTES || errTES->size() != size_t(cpmCrates)) {
    if (debug)
      msg(MSG::DEBUG) << "No CPM mismatch vector of expected size" << endmsg;
  } else {
    for (int crate = 0; crate < cpmCrates; ++crate) {
      const int err = (*errTES)[crate];
      if (err == 0)
        continue;
      const int cr = crate + ppmCrates;
      if (((err >> EMTowerMismatch) & 0x1) || ((err >> HadTowerMismatch) & 0x1))
        m_h_l1calo_2d_CurrentEventOverview->Fill(Transmission, cr);
      if (((err >> EMRoIMismatch) & 0x1) || ((err >> TauRoIMismatch) & 0x1))
        m_h_l1calo_2d_CurrentEventOverview->Fill(Simulation, cr);
      if (((err >> LeftCMXTobMismatch) & 0x1) ||
          ((err >> RightCMXTobMismatch) & 0x1) ||
          ((err >> RemoteSumMismatch) & 0x1))
        m_h_l1calo_2d_CurrentEventOverview->Fill(CMXTransmission, cr);
      if (((err >> LocalSumMismatch) & 0x1) ||
          ((err >> TotalSumMismatch) &
           0x1) /*|| ((err >> TopoMismatch) & 0x1)*/)
        m_h_l1calo_2d_CurrentEventOverview->Fill(CMXSimulation, cr);
    }
  }

  // JEM Mismatch data
  errTES = 0;
  if (evtStore()->contains<ErrorVector>(m_jemMismatchLocation)) {
    sc = evtStore()->retrieve(errTES, m_jemMismatchLocation);
  } else
    sc = StatusCode::FAILURE;
  if (sc.isFailure() || !errTES || errTES->size() != size_t(jemCrates)) {
    if (debug)
      msg(MSG::DEBUG) << "No JEM mismatch vector of expected size" << endmsg;
  } else {
    for (int crate = 0; crate < jemCrates; ++crate) {
      const int err = (*errTES)[crate];
      if (err == 0)
        continue;
      const int cr = crate + ppmCrates + cpmCrates;
      if (((err >> EMElementMismatch) & 0x1) ||
          ((err >> HadElementMismatch) & 0x1) ||
          ((err >> JEMRoIMismatch) & 0x1) || ((err >> JEMEtSumsMismatch) & 0x1))
        m_h_l1calo_2d_CurrentEventOverview->Fill(Simulation, cr);
      if (((err >> RemoteJetMismatch) & 0x1) ||
          ((err >> CMXEtSumsMismatch) & 0x1) ||
          ((err >> RemoteEnergyMismatch) & 0x1) ||
          ((err >> EnergyRoIMismatch) & 0x1))
        m_h_l1calo_2d_CurrentEventOverview->Fill(CMXTransmission, cr);
      if (((err >> CMXJetTobMismatch) & 0x1) ||
          ((err >> LocalJetMismatch) & 0x1) ||
          ((err >> TotalJetMismatch) & 0x1) ||
          ((err >> LocalEnergyMismatch) & 0x1) ||
          ((err >> TotalEnergyMismatch) & 0x1) ||
          /*((err >> CMXJetTopoMismatch) & 0x1) ||*/
          ((err >> SumEtMismatch) & 0x1) ||
          ((err >> MissingEtMismatch) & 0x1) ||
          ((err >> MissingEtSigMismatch) & 0x1))
        m_h_l1calo_2d_CurrentEventOverview->Fill(CMXSimulation, cr);
    }
  }

  if (m_h_l1calo_2d_CurrentEventOverview->GetEntries() > 0.) {
    m_h_l1calo_2d_GlobalOverview->Add(m_h_l1calo_2d_CurrentEventOverview);
    if (online) {
      m_h_l1calo_2d_GlobalOverviewRecent->Add(
          m_h_l1calo_2d_CurrentEventOverview);
      m_v_l1calo_2d_GlobalOverviewBlock[m_lumipos]->Add(
          m_h_l1calo_2d_CurrentEventOverview);
    }
    if (m_lumiNo && m_h_l1calo_1d_ErrorsByLumiblock) {
      if (!online && m_h_l1calo_1d_ErrorsByLumiblock->GetEntries() == 0.) {
        if (!m_registred_h_l1calo_1d_ErrorsByLumiblock) {
          m_registred_h_l1calo_1d_ErrorsByLumiblock = true;
          std::string dir(m_rootDir + "/Errors");
          MonGroup monLumi(this, dir, run, ATTRIB_UNMANAGED, "",
                           "mergeRebinned");
          m_histTool->setMonGroup(&monLumi);
          m_histTool->registerHist(m_h_l1calo_1d_ErrorsByLumiblock);
        }
      }
      m_h_l1calo_1d_ErrorsByLumiblock->Fill(m_lumiNo);
    }
    if (m_lumiNo && m_h_l1calo_1d_ErrorsByTime) {
      const EventInfo *evtInfo = 0;
      StatusCode sc = evtStore()->retrieve(evtInfo);
      if (sc.isSuccess()) {
        time_t timeStamp = evtInfo->event_ID()->time_stamp();
        std::tm *local = localtime(&timeStamp);
        int itime =
            local->tm_hour * 10000 + local->tm_min * 100 + local->tm_sec;
        if (itime == 0)
          itime = 1;
        double time = itime / 10000.;
        if (online) {
          int bin = m_h_l1calo_1d_ErrorsByTime->GetXaxis()->FindBin(m_lumiNo);
          if (m_h_l1calo_1d_ErrorsByTime->GetBinContent(bin) == 0.) {
            m_h_l1calo_1d_ErrorsByTime->Fill(m_lumiNo, time);
          }
        } else {
          if (m_h_l1calo_1d_ErrorsByTime->GetEntries() == 0.) {
            if (!m_registred_h_l1calo_1d_ErrorsByTime) {
              m_registred_h_l1calo_1d_ErrorsByTime = true;
              std::string dir(m_rootDir + "/Errors");
              MonGroup monLumi(this, dir, run, ATTRIB_UNMANAGED);
              m_histTool->setMonGroup(&monLumi);
              m_histTool->registerHist(m_h_l1calo_1d_ErrorsByTime);
            }
          }
          m_h_l1calo_1d_ErrorsByTime->Fill(time);
        }
      }
    }
  }

  if (debug)
    msg(MSG::DEBUG) << "Leaving fillHistograms" << endmsg;

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode OverviewMon::procHistograms()
/*---------------------------------------------------------*/
{
  msg(MSG::DEBUG) << "procHistograms entered" << endmsg;

  //if (endOfEventsBlock || endOfLumiBlock) {
  //}

  bool online = (m_onlineTest || m_environment == AthenaMonManager::online);
  if (endOfRun && !online) {
    if (m_h_l1calo_1d_ErrorsByLumiblock &&
        m_h_l1calo_1d_ErrorsByLumiblock->GetEntries() == 0.) {
      delete m_h_l1calo_1d_ErrorsByLumiblock;
      m_h_l1calo_1d_ErrorsByLumiblock = 0;
      delete m_h_l1calo_1d_ErrorsByTime;
      m_h_l1calo_1d_ErrorsByTime = 0;
    }
    if (m_h_l1calo_1d_RejectedEvents &&
        m_h_l1calo_1d_RejectedEvents->GetEntries() == 0.) {
      delete m_h_l1calo_1d_RejectedEvents;
      m_h_l1calo_1d_RejectedEvents = 0;
    }
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
TH2F *OverviewMon::bookOverview(const std::string &name,
                                const std::string &title)
/*---------------------------------------------------------*/
{
  TH2F *hist = m_histTool->bookTH2F(name, title, NumberOfGlobalErrors, 0,
                                    NumberOfGlobalErrors, 14, 0, 14);
  TAxis *axis = hist->GetXaxis();
  axis->SetBinLabel(1 + PPMDataStatus, "PPMDataStatus");
  axis->SetBinLabel(1 + PPMDataError, "PPMDataError");
  axis->SetBinLabel(1 + SubStatus, "SubStatus");
  axis->SetBinLabel(1 + Parity, "Parity");
  axis->SetBinLabel(1 + LinkDown, "LinkDown");
  axis->SetBinLabel(1 + Transmission, "Transmission");
  axis->SetBinLabel(1 + Simulation, "Simulation");
  axis->SetBinLabel(1 + CMXSubStatus, "CMXSubStatus");
  axis->SetBinLabel(1 + GbCMXParity, "CMXParity");
  axis->SetBinLabel(1 + CMXTransmission, "CMXTransmission");
  axis->SetBinLabel(1 + CMXSimulation, "CMXSimulation");
  axis->SetBinLabel(1 + RODStatus, "RODStatus");
  axis->SetBinLabel(1 + RODMissing, "RODMissing");
  axis->SetBinLabel(1 + ROBStatus, "ROBStatus");
  axis->SetBinLabel(1 + Unpacking, "Unpacking");

  axis = hist->GetYaxis();
  for (int crate = 0; crate < 14; ++crate) {
    int cr = crate;
    if (cr >= 12)
      cr -= 12;
    if (cr >= 8)
      cr -= 8;
    std::string type = (crate < 8) ? "PP " : (crate < 12) ? "CP " : "JEP ";
    std::ostringstream cnum;
    cnum << type << cr;
    axis->SetBinLabel(crate + 1, cnum.str().c_str());
  }

  return hist;
}

} // end namespace
