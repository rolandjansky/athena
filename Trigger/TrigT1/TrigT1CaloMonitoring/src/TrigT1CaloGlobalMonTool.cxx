/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigT1CaloGlobalMonTool.cxx
// PACKAGE:  TrigT1CaloMonitoring  
//
// AUTHOR:   Peter Faulkner
//           
//
// ********************************************************************

#include <ctime>
#include <sstream>

#include "TH1F.h"
#include "TH2F.h"
#include "TAxis.h"
#include "TList.h"

#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "SGTools/StlVectorClids.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrigT1CaloMonitoringTools/ITrigT1CaloMonErrorTool.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramToolV1.h"

#include "TrigT1CaloGlobalMonTool.h"
// ============================================================================
namespace LVL1 {
// ============================================================================
TrigT1CaloGlobalMonTool::TrigT1CaloGlobalMonTool(const std::string & type, 
				                 const std::string & name,
				                 const IInterface* parent)
  : ManagedMonitorToolBase(type, name, parent),
    m_errorTool("LVL1::TrigT1CaloMonErrorToolV1/TrigT1CaloMonErrorToolV1"),
    m_histTool("LVL1::TrigT1CaloLWHistogramToolV1/TrigT1CaloLWHistogramToolV1"),
    m_lumiNo(0),
    m_lumipos(0),
    m_h_l1calo_2d_GlobalOverview(0),
    m_h_l1calo_2d_CurrentEventOverview(0),
    m_h_l1calo_2d_GlobalOverviewRecent(0),
    m_h_l1calo_1d_ErrorsByLumiblock(0),
    m_h_l1calo_1d_ErrorsByTime(0),
    m_h_l1calo_1d_RejectedEvents(0),
    m_h_l1calo_1d_NumberOfEvents(0)

/*---------------------------------------------------------*/
{

  declareProperty("RootDirectory", m_rootDir = "L1Calo");
  declareProperty("RecentLumiBlocks", m_recentLumi = 10,
                  "Number of lumiblocks in recent lumiblocks plot");
  declareProperty("OnlineTest", m_onlineTest = false,
                  "Test online code when running offline");

}

/*---------------------------------------------------------*/
TrigT1CaloGlobalMonTool::~TrigT1CaloGlobalMonTool()
/*---------------------------------------------------------*/
{
}

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

/*---------------------------------------------------------*/
StatusCode TrigT1CaloGlobalMonTool::initialize()
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
    msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloHistogramTool"
                    << endmsg;
    return sc;
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TrigT1CaloGlobalMonTool::finalize()
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
StatusCode TrigT1CaloGlobalMonTool::bookHistogramsRecurrent()
/*---------------------------------------------------------*/
{
  msg(MSG::DEBUG) << "bookHistograms entered" << endmsg;

  if( m_environment == AthenaMonManager::online ) {
    // book histograms that are only made in the online environment...
  }
  	
  if( m_dataType == AthenaMonManager::cosmics ) {
    // book histograms that are only relevant for cosmics data...
  }

  bool online = (m_onlineTest || m_environment == AthenaMonManager::online);
  MgmtAttr_t attr = ATTRIB_UNMANAGED;
 
  if ( newRun || newLumiBlock ) {

    // Get lumiblock number

    m_lumiNo = 0;
    const EventInfo* evtInfo = 0;
    StatusCode sc = evtStore()->retrieve(evtInfo);
    if( sc.isSuccess() ) {
      m_lumiNo = evtInfo->event_ID()->lumi_block();
    }
  }

  if ((newLumiBlock && !online) || newRun ) {

    std::string dir(m_rootDir + "/Overview/Errors");
    MonGroup monGlobal( this, dir,
             ((newLumiBlock && !online) ? lumiBlock : run), attr );

    // Global Error Overview

    m_histTool->setMonGroup(&monGlobal);

    m_h_l1calo_2d_GlobalOverview = bookOverview("l1calo_2d_GlobalOverview",
                              "L1Calo Global Error Overview");

    m_histTool->unsetMonGroup();

    if (!m_h_l1calo_2d_CurrentEventOverview) { // temporary plot for current event
      m_h_l1calo_2d_CurrentEventOverview = bookOverview(
        "l1calo_2d_CurrentEventOverview", "L1Calo Current Event Error Overview");
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
	  TH2F* hist = bookOverview("l1calo_2d_GlobalOverviewBlock" + cnum.str(),
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
      if (m_luminumbers[i] == m_lumiNo) m_lumipos = i;
    }
    if (m_lumipos == -1) {
      double entries = m_v_l1calo_2d_GlobalOverviewBlock[m_recentLumi-1]->GetEntries();
      if (entries > 0.) m_h_l1calo_2d_GlobalOverviewRecent->Reset();
      m_v_l1calo_2d_GlobalOverviewBlock[m_recentLumi-1]->Reset();
      TH2F* tmpHist = m_v_l1calo_2d_GlobalOverviewBlock[m_recentLumi-1];
      for (int i = m_recentLumi-2; i >= 0; --i) {
        if (entries > 0. && m_v_l1calo_2d_GlobalOverviewBlock[i]->GetEntries() > 0.) {
          m_h_l1calo_2d_GlobalOverviewRecent->Add(m_v_l1calo_2d_GlobalOverviewBlock[i]);
        }
        m_v_l1calo_2d_GlobalOverviewBlock[i+1] = m_v_l1calo_2d_GlobalOverviewBlock[i];
	m_luminumbers[i+1] = m_luminumbers[i];
      }
      m_v_l1calo_2d_GlobalOverviewBlock[0] = tmpHist;
      m_lumipos = 0;
      m_luminumbers[m_lumipos] = m_lumiNo;
    }

  } // end if ((newLumiBlock && ...

  if ( newRun || newLumiBlock ) {

    // Errors by lumiblock/time plots
    // On Tier0 only kept if non-empty

    if( m_lumiNo ) {
      if (newRun) {
        std::string dir(m_rootDir + "/Overview/Errors");
	MonGroup monLumi( this, dir, run, attr);
        if (online) m_histTool->setMonGroup(&monLumi);
	else        m_histTool->unsetMonGroup();
        m_h_l1calo_1d_ErrorsByLumiblock = m_histTool->bookTH1F(
	             "l1calo_1d_ErrorsByLumiblock",
	             "Events with Errors by Lumiblock;Lumi Block;Number of Events",
		     1, m_lumiNo, m_lumiNo+1);
	if (online) { // Would be merge problems offline
	  m_h_l1calo_1d_ErrorsByTime = m_histTool->bookTH1F(
	    "l1calo_1d_ErrorsByTime",
	    "Time of First Event in Lumiblock with Error;Lumi Block;Time (h.mmss)",
	     1, m_lumiNo, m_lumiNo+1);
        } else {
	  m_h_l1calo_1d_ErrorsByTime = m_histTool->bookTH1F("l1calo_1d_ErrorsByTime",
	             "Events with Errors by Time;Time (h.mm);Number of Events",
		     2400, 0., 24.);
        }
	if (m_errorTool->flagCorruptEvents() != "None") {
	  m_h_l1calo_1d_RejectedEvents = m_histTool->bookTH1F(
	             "l1calo_1d_RejectedEvents",
	             "Rejected Events by Lumiblock;Lumi Block;Number of Events",
		     1, m_lumiNo, m_lumiNo+1);
        } else m_h_l1calo_1d_RejectedEvents = 0;

      } else if (m_lumiNo < m_h_l1calo_1d_ErrorsByLumiblock->GetXaxis()->GetXmin() ||
                 m_lumiNo >= m_h_l1calo_1d_ErrorsByLumiblock->GetXaxis()->GetXmax()) {
        m_histTool->unsetMonGroup();
        TH1F* tmphist = m_histTool->bookTH1F("l1calo_1d_Tmp",
	                                     "Errors by Lumiblock",
					     1, m_lumiNo, m_lumiNo+1);
	tmphist->Fill(m_lumiNo);
	TList* list = new TList;
	list->Add(tmphist);
	// All this rigmarole is to get Merge() to behave as we want,
	// especially online.
	TH1F* hist = m_h_l1calo_1d_ErrorsByLumiblock;
	for (int i = 0; i < 3; ++i) {
	  if (!hist) continue;
	  double entries = hist->GetEntries();
	  bool earlier = (m_lumiNo < hist->GetXaxis()->GetXmin());
	  double content = 0.;
	  if (earlier) {
	    int lastBin = hist->GetXaxis()->GetNbins();
	    content = hist->GetBinContent(lastBin);
	    if (content == 0.) hist->SetBinContent(lastBin, 1.);
          } else {
	    content = hist->GetBinContent(1);
	    if (content == 0.) hist->SetBinContent(1, 1.);
          }
          if (hist->Merge(list) != -1) {
	    int bin = hist->GetXaxis()->FindBin(m_lumiNo);
	    hist->SetBinContent(bin, 0.);
	    if (content == 0.) {
	      if (earlier) {
	        int lastBin = hist->GetXaxis()->GetNbins();
	        hist->SetBinContent(lastBin, 0.);
	      } else hist->SetBinContent(1, 0.);
	    }
	    hist->SetEntries(entries);
          }
	  if (i == 0) hist = (online) ? m_h_l1calo_1d_ErrorsByTime : 0;
	  else        hist = m_h_l1calo_1d_RejectedEvents;
        }
	delete tmphist;
	delete list;
      }
    }
  }

  // Total events processed and total rejected as corrupt

  if ( newRun ) {
    std::string dir(m_rootDir + "/Overview");
    MonGroup monEvents( this, dir, run, attr);
    m_histTool->setMonGroup(&monEvents);
    int bins = (m_errorTool->flagCorruptEvents() == "None") ? 1 : 2;
    m_h_l1calo_1d_NumberOfEvents = m_histTool->bookTH1F("l1calo_1d_NumberOfEvents",
      "Number of processed events", bins, 0., bins);
    m_h_l1calo_1d_NumberOfEvents->GetXaxis()->SetBinLabel(1,"Processed Events");
    if (bins > 1) {
      m_h_l1calo_1d_NumberOfEvents->GetXaxis()->SetBinLabel(2,"Corrupt Events Skipped");
    }
  }

  m_histTool->unsetMonGroup();

  msg(MSG::DEBUG) << "Leaving bookHistograms" << endmsg;

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TrigT1CaloGlobalMonTool::fillHistograms()
/*---------------------------------------------------------*/
{
  const bool debug = msgLvl(MSG::DEBUG);
  if (debug) msg(MSG::DEBUG) << "fillHistograms entered" << endmsg;

  if (!m_h_l1calo_2d_GlobalOverview) {
    if (debug) msg(MSG::DEBUG) << "Histograms not booked" << endmsg;
    return StatusCode::SUCCESS;
  }

  const bool online = (m_onlineTest || m_environment == AthenaMonManager::online);
  MgmtAttr_t attr = ATTRIB_UNMANAGED;

  // Total events and corrupt event by lumiblock plots

  if (m_errorTool->corrupt()) {
    m_h_l1calo_1d_NumberOfEvents->Fill(1.);
    if (m_lumiNo && m_h_l1calo_1d_RejectedEvents) {
      if (!online && m_h_l1calo_1d_RejectedEvents->GetEntries() == 0.) {
        std::string dir(m_rootDir + "/Overview/Errors");
        MonGroup monLumi( this, dir, run, attr, "", "mergeRebinned");
        m_histTool->setMonGroup(&monLumi);
        m_histTool->registerHist(m_h_l1calo_1d_RejectedEvents);
      }
      m_h_l1calo_1d_RejectedEvents->Fill(m_lumiNo);
    }
    return StatusCode::SUCCESS;
  } else m_h_l1calo_1d_NumberOfEvents->Fill(0.);

  StatusCode sc;

  // Update Global overview plot

  const int ppmCrates = 8;
  const int cpmCrates = 4;
  const int jemCrates = 2;
  m_h_l1calo_2d_CurrentEventOverview->Reset();

  // PPM Error data
  const ErrorVector* errTES = 0; 
  if (evtStore()->contains<ErrorVector>("L1CaloPPMErrorVector")) {
    sc = evtStore()->retrieve(errTES, "L1CaloPPMErrorVector"); 
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !errTES || errTES->size() != size_t(ppmCrates)) {
    if (debug) msg(MSG::DEBUG) << "No PPM error vector of expected size"
                               << endmsg;
  } else {
    for (int crate = 0; crate < ppmCrates; ++crate) {
      const int err = (*errTES)[crate];
      if (err == 0) continue;
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
  if (evtStore()->contains<ErrorVector>("L1CaloPPMSpareErrorVector")) {
    sc = evtStore()->retrieve(errTES, "L1CaloPPMSpareErrorVector"); 
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !errTES || errTES->size() != size_t(ppmCrates)) {
    if (debug) msg(MSG::DEBUG) << "No PPMSpare error vector of expected size"
                               << endmsg;
  } else {
    for (int crate = 0; crate < ppmCrates; ++crate) {
      const int err = (*errTES)[crate];
      if (err == 0) continue;
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

  // CPM and CPM CMM Error data
  errTES = 0; 
  if (evtStore()->contains<ErrorVector>("L1CaloCPMErrorVector")) {
    sc = evtStore()->retrieve(errTES, "L1CaloCPMErrorVector"); 
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !errTES || errTES->size() != size_t(cpmCrates)) {
    if (debug) msg(MSG::DEBUG) << "No CPM error vector of expected size"
                               << endmsg;
  } else {
    for (int crate = 0; crate < cpmCrates; ++crate) {
      const int err = (*errTES)[crate];
      if (err == 0) continue;
      const int cr = crate + ppmCrates;
      if ((err >> CPMStatus) & 0x1) m_h_l1calo_2d_CurrentEventOverview->Fill(SubStatus, cr);
      if (((err >> CPMEMParity) & 0x1) || ((err >> CPMHadParity) & 0x1))
                                             m_h_l1calo_2d_CurrentEventOverview->Fill(Parity, cr);
      if (((err >> CPMEMLink) & 0x1) || ((err >> CPMHadLink) & 0x1))
                                             m_h_l1calo_2d_CurrentEventOverview->Fill(LinkDown, cr);
      if ((err >> CPMRoIParity) & 0x1) m_h_l1calo_2d_CurrentEventOverview->Fill(RoIParity, cr);
      if ((err >> CMMCPStatus) & 0x1)  m_h_l1calo_2d_CurrentEventOverview->Fill(CMMSubStatus, cr);
      if ((err >> CMMCPParity) & 0x1)  m_h_l1calo_2d_CurrentEventOverview->Fill(GbCMMParity, cr);
    }
  }

  // JEM Error data
  errTES = 0; 
  if (evtStore()->contains<ErrorVector>("L1CaloJEMErrorVector")) {
    sc = evtStore()->retrieve(errTES, "L1CaloJEMErrorVector"); 
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !errTES || errTES->size() != size_t(jemCrates)) {
    if (debug) msg(MSG::DEBUG) << "No JEM error vector of expected size"
                               << endmsg;
  } else {
    for (int crate = 0; crate < jemCrates; ++crate) {
      const int err = (*errTES)[crate];
      if (err == 0) continue;
      const int cr = crate + ppmCrates + cpmCrates;
      if ((err >> JEMStatus) & 0x1) m_h_l1calo_2d_CurrentEventOverview->Fill(SubStatus, cr);
      if (((err >> JEMEMParity) & 0x1) || ((err >> JEMHadParity) & 0x1))
                                             m_h_l1calo_2d_CurrentEventOverview->Fill(Parity, cr);
      if (((err >> JEMEMLink) & 0x1) || ((err >> JEMHadLink) & 0x1))
                                             m_h_l1calo_2d_CurrentEventOverview->Fill(LinkDown, cr);
      if ((err >> JEMRoIParity) & 0x1) m_h_l1calo_2d_CurrentEventOverview->Fill(RoIParity, cr);
    }
  }

  // JEM CMM Error data
  errTES = 0; 
  if (evtStore()->contains<ErrorVector>("L1CaloJEMCMMErrorVector")) {
    sc = evtStore()->retrieve(errTES, "L1CaloJEMCMMErrorVector"); 
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !errTES || errTES->size() != size_t(jemCrates)) {
    if (debug) msg(MSG::DEBUG) << "No JEM CMM error vector of expected size"
                               << endmsg;
  } else {
    for (int crate = 0; crate < jemCrates; ++crate) {
      const int err = (*errTES)[crate];
      if (err == 0) continue;
      const int cr = crate + ppmCrates + cpmCrates;
      if (((err >> JEMCMMJetStatus) & 0x1) ||
          ((err >> JEMCMMEnergyStatus) & 0x1)) {
        m_h_l1calo_2d_CurrentEventOverview->Fill(CMMSubStatus, cr);
      }
      if (((err >> JEMCMMJetParity) & 0x1) ||
          ((err >> JEMCMMEnergyParity) & 0x1)) {
        m_h_l1calo_2d_CurrentEventOverview->Fill(GbCMMParity, cr);
      }
      if ((err >> JEMCMMRoIParity) & 0x1) m_h_l1calo_2d_CurrentEventOverview->Fill(RoIParity, cr);
    }
  }

  // ROD Error data
  errTES = 0; 
  if (evtStore()->contains<ErrorVector>("L1CaloRODErrorVector")) {
    sc = evtStore()->retrieve(errTES, "L1CaloRODErrorVector"); 
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !errTES || 
                errTES->size() != size_t(ppmCrates + cpmCrates + jemCrates)) {
    if (debug) msg(MSG::DEBUG) << "No ROD error vector of expected size"
                               << endmsg;
  } else {
    for (int crate = 0; crate < ppmCrates+cpmCrates+jemCrates; ++crate) {
      const int err = (*errTES)[crate];
      if (err == 0) continue;
      //if (err & 0x7f) m_h_l1calo_2d_CurrentEventOverview->Fill(RODStatus, crate);
      if (err & 0x3f) m_h_l1calo_2d_CurrentEventOverview->Fill(RODStatus, crate);
      if (((err >> NoFragment) & 0x1) || ((err >> NoPayload) & 0x1))
                      m_h_l1calo_2d_CurrentEventOverview->Fill(RODMissing, crate);
      if ((err >> ROBStatusError) & 0x1) m_h_l1calo_2d_CurrentEventOverview->Fill(ROBStatus, crate);
      if ((err >> UnpackingError) & 0x1) m_h_l1calo_2d_CurrentEventOverview->Fill(Unpacking, crate);
    }
  }

  // PPM Mismatch data
  errTES = 0; 
  if (evtStore()->contains<ErrorVector>("L1CaloPPMMismatchVector")) {
    sc = evtStore()->retrieve(errTES, "L1CaloPPMMismatchVector"); 
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !errTES || errTES->size() != size_t(ppmCrates)) {
    if (debug) msg(MSG::DEBUG) << "No PPM mismatch vector of expected size"
                               << endmsg;
  } else {
    for (int crate = 0; crate < ppmCrates; ++crate) {
      const int err = (*errTES)[crate];
      if (err == 0) continue;
      if (((err >> LUTMismatch) & 0x1)) m_h_l1calo_2d_CurrentEventOverview->Fill(Simulation, crate);
    }
  }

  // CPM Mismatch data
  errTES = 0; 
  if (evtStore()->contains<ErrorVector>("L1CaloCPMMismatchVector")) {
    sc = evtStore()->retrieve(errTES, "L1CaloCPMMismatchVector"); 
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !errTES || errTES->size() != size_t(cpmCrates)) {
    if (debug) msg(MSG::DEBUG) << "No CPM mismatch vector of expected size"
                               << endmsg;
  } else {
    for (int crate = 0; crate < cpmCrates; ++crate) {
      const int err = (*errTES)[crate];
      if (err == 0) continue;
      const int cr = crate + ppmCrates;
      if (((err >> EMTowerMismatch) & 0x1) || ((err >> HadTowerMismatch) & 0x1))
                                        m_h_l1calo_2d_CurrentEventOverview->Fill(Transmission, cr);
      if (((err >> CPMRoIMismatch) & 0x1) || ((err >> CPMHitsMismatch) & 0x1))
                                        m_h_l1calo_2d_CurrentEventOverview->Fill(Simulation, cr);
      if (((err >> CMMHitsMismatch) & 0x1) || ((err >> RemoteSumMismatch) & 0x1))
                                        m_h_l1calo_2d_CurrentEventOverview->Fill(CMMTransmission, cr);
      if (((err >> LocalSumMismatch) & 0x1) || ((err >> TotalSumMismatch) & 0x1))
                                        m_h_l1calo_2d_CurrentEventOverview->Fill(CMMSimulation, cr);
    }
  }

  // JEM Mismatch data
  errTES = 0; 
  if (evtStore()->contains<ErrorVector>("L1CaloJEMMismatchVector")) {
    sc = evtStore()->retrieve(errTES, "L1CaloJEMMismatchVector"); 
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !errTES || errTES->size() != size_t(jemCrates)) {
    if (debug) msg(MSG::DEBUG) << "No JEM mismatch vector of expected size"
                               << endmsg;
  } else {
    for (int crate = 0; crate < jemCrates; ++crate) {
      const int err = (*errTES)[crate];
      if (err == 0) continue;
      const int cr = crate + ppmCrates + cpmCrates;
      if (((err >> EMElementMismatch) & 0x1)  ||
          ((err >> HadElementMismatch) & 0x1) ||
          ((err >> JEMRoIMismatch) & 0x1)     ||
          ((err >> JEMHitsMismatch) & 0x1)    ||
          ((err >> JEMEtSumsMismatch) & 0x1)) m_h_l1calo_2d_CurrentEventOverview->Fill(Simulation, cr);
      if (((err >> CMMJetHitsMismatch) & 0x1)   ||
          ((err >> RemoteJetMismatch) & 0x1)    ||
	  ((err >> JetEtRoIMismatch) & 0x1)     ||
	  ((err >> CMMEtSumsMismatch) & 0x1)    ||
	  ((err >> RemoteEnergyMismatch) & 0x1) ||
	  ((err >> EnergyRoIMismatch) & 0x1))
	                              m_h_l1calo_2d_CurrentEventOverview->Fill(CMMTransmission, cr);
      if (((err >> LocalJetMismatch) & 0x1)    ||
          ((err >> TotalJetMismatch) & 0x1)    ||
	  ((err >> JetEtMismatch) & 0x1)       ||
          ((err >> LocalEnergyMismatch) & 0x1) ||
          ((err >> TotalEnergyMismatch) & 0x1) ||
	  ((err >> SumEtMismatch) & 0x1)       ||
	  ((err >> MissingEtMismatch) & 0x1)   ||
	  ((err >> MissingEtSigMismatch) & 0x1))
	                                m_h_l1calo_2d_CurrentEventOverview->Fill(CMMSimulation, cr);
    }
  }

  if (m_h_l1calo_2d_CurrentEventOverview->GetEntries() > 0.) {
    m_h_l1calo_2d_GlobalOverview->Add(m_h_l1calo_2d_CurrentEventOverview);
    if (online) {
      m_h_l1calo_2d_GlobalOverviewRecent->Add(m_h_l1calo_2d_CurrentEventOverview);
      m_v_l1calo_2d_GlobalOverviewBlock[m_lumipos]->Add(m_h_l1calo_2d_CurrentEventOverview);
    }
    if (m_lumiNo && m_h_l1calo_1d_ErrorsByLumiblock) {
      if (!online && m_h_l1calo_1d_ErrorsByLumiblock->GetEntries() == 0.) {
        std::string dir(m_rootDir + "/Overview/Errors");
	MonGroup monLumi( this, dir, run, attr, "", "mergeRebinned");
        m_histTool->setMonGroup(&monLumi);
	m_histTool->registerHist(m_h_l1calo_1d_ErrorsByLumiblock);
      }
      m_h_l1calo_1d_ErrorsByLumiblock->Fill(m_lumiNo);
    }  
    if (m_lumiNo && m_h_l1calo_1d_ErrorsByTime) {
      const EventInfo* evtInfo = 0;
      StatusCode sc = evtStore()->retrieve(evtInfo);
      if( sc.isSuccess() ) {
        time_t timeStamp = evtInfo->event_ID()->time_stamp();
        std::tm* local = localtime(&timeStamp);
	int itime = local->tm_hour*10000 + local->tm_min*100 + local->tm_sec;
	if (itime == 0) itime = 1;
	double time = itime/10000.;
        if (online) {
          int bin = m_h_l1calo_1d_ErrorsByTime->GetXaxis()->FindBin(m_lumiNo);
          if (m_h_l1calo_1d_ErrorsByTime->GetBinContent(bin) == 0.) {
	    m_h_l1calo_1d_ErrorsByTime->Fill(m_lumiNo, time);
	  }
        } else {
          if (m_h_l1calo_1d_ErrorsByTime->GetEntries() == 0.) {
            std::string dir(m_rootDir + "/Overview/Errors");
	    MonGroup monLumi( this, dir, run, attr);
            m_histTool->setMonGroup(&monLumi);
	    m_histTool->registerHist(m_h_l1calo_1d_ErrorsByTime);
          }
	  m_h_l1calo_1d_ErrorsByTime->Fill(time);
        }
      }
    }
  }

  if (debug) msg(MSG::DEBUG) << "Leaving fillHistograms" << endmsg;

  return StatusCode::SUCCESS;

}

/*---------------------------------------------------------*/
StatusCode TrigT1CaloGlobalMonTool::procHistograms()
/*---------------------------------------------------------*/
{
  msg(MSG::DEBUG) << "procHistograms entered" << endmsg;

  //if (endOfLumiBlock) {
  //}

  bool online = (m_onlineTest || m_environment == AthenaMonManager::online);
  if (endOfRun && !online) {
    if (m_h_l1calo_1d_ErrorsByLumiblock && m_h_l1calo_1d_ErrorsByLumiblock->GetEntries() == 0.) {
      delete m_h_l1calo_1d_ErrorsByLumiblock;
      m_h_l1calo_1d_ErrorsByLumiblock = 0;
      delete m_h_l1calo_1d_ErrorsByTime;
      m_h_l1calo_1d_ErrorsByTime = 0;
    }
    if (m_h_l1calo_1d_RejectedEvents && m_h_l1calo_1d_RejectedEvents->GetEntries() == 0.) {
      delete m_h_l1calo_1d_RejectedEvents;
      m_h_l1calo_1d_RejectedEvents = 0;
    }
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
TH2F* TrigT1CaloGlobalMonTool::bookOverview(const std::string& name,
                                            const std::string& title)
/*---------------------------------------------------------*/
{
  TH2F* hist = m_histTool->bookTH2F(name, title,
	              NumberOfGlobalErrors, 0, NumberOfGlobalErrors,
		      14, 0, 14);
  TAxis* axis = hist->GetXaxis();
  axis->SetBinLabel(1+PPMDataStatus,   "PPMDataStatus");
  axis->SetBinLabel(1+PPMDataError,    "PPMDataError");
  axis->SetBinLabel(1+SubStatus,       "SubStatus");
  axis->SetBinLabel(1+Parity,          "Parity");
  axis->SetBinLabel(1+LinkDown,        "LinkDown");
  axis->SetBinLabel(1+RoIParity,       "RoIParity");
  axis->SetBinLabel(1+Transmission,    "Transmission");
  axis->SetBinLabel(1+Simulation,      "Simulation");
  axis->SetBinLabel(1+CMMSubStatus,    "CMMSubStatus");
  axis->SetBinLabel(1+GbCMMParity,     "CMMParity");
  axis->SetBinLabel(1+CMMTransmission, "CMMTransmission");
  axis->SetBinLabel(1+CMMSimulation,   "CMMSimulation");
  axis->SetBinLabel(1+RODStatus,       "RODStatus");
  axis->SetBinLabel(1+RODMissing,      "RODMissing");
  axis->SetBinLabel(1+ROBStatus,       "ROBStatus");
  axis->SetBinLabel(1+Unpacking,       "Unpacking");

  axis = hist->GetYaxis();
  for (int crate = 0; crate < 14; ++crate) {
    int cr = crate;
    if (cr >= 12) cr -= 12;
    if (cr >= 8)  cr -= 8;
    std::string type = (crate < 8) ? "PP " : (crate < 12) ? "CP " : "JEP ";
    std::ostringstream cnum;
    cnum << type << cr;
    axis->SetBinLabel(crate+1, cnum.str().c_str());
  }

  return hist;
}
// ============================================================================
}  // end namespace
// ============================================================================
