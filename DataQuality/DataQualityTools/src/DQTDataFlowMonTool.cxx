/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     DQTDataFlowMonTool.cxx
// PACKAGE:  DataQualityTools  
//
// AUTHORS:  Peter Onyisi (ponyisi@cern.ch)
//
//
// ********************************************************************

#include "DataQualityTools/DQTDataFlowMonTool.h"

///#include "TH1.h"
///#include "TH2.h"
#include "TGraph.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"

static const char* envstrings[AthenaMonManager::altprod+1] = { "user", "noOutput", "online", "tier0", 
			       "tier0Raw", "tier0ESD", "altprod" };
static const char* eventflagdets[] = { "Pixel", "SCT", "TRT",
				       "LAr", "Tile", "Muon",
				       "ForwardDet", "Core", 
				       "Background", "Lumi"};

using xAOD::EventInfo;

//----------------------------------------------------------------------------------
DQTDataFlowMonTool::DQTDataFlowMonTool(const std::string & type, 
		   const std::string & name,
		   const IInterface* parent)
  : DataQualityFatherMonTool(type, name, parent),
    m_releaseString(""),
    m_releaseStageString(""),
    m_events_lb(0),
    m_release_stage_lowStat(0),
    m_eventflag_summary_lowStat(0),
    m_eventflag_summary_lb(0)

//----------------------------------------------------------------------------------
{
  declareInterface<IMonitorToolBase>(this);
  m_path = "GLOBAL/DQTDataFlow";
  declareProperty("releaseString", m_releaseString);

  for (int i=0; i < EventInfo::nDets; i++) {
    m_eventflag_run[i] = 0;
    m_eventflag_vec[i] = 0;
  }
}

//----------------------------------------------------------------------------------
DQTDataFlowMonTool::~DQTDataFlowMonTool()
//----------------------------------------------------------------------------------
{
  for (int i=0; i < EventInfo::nDets; i++) {
    delete m_eventflag_vec[i];
    m_eventflag_vec[i] = 0;
  }
}

//----------------------------------------------------------------------------------
StatusCode DQTDataFlowMonTool::bookHistograms(  )
//----------------------------------------------------------------------------------
{
  bool failure(false);

  if (m_releaseStageString == "") {
    m_releaseStageString = m_releaseString + "-" + envstrings[m_environment];
  }

    msg(MSG::DEBUG) << "in bookHistograms()" << endreq;
    msg(MSG::DEBUG) << "Using base path " << m_path << endreq;
  
    MonGroup lb_hists( this, m_path, lumiBlock, ATTRIB_MANAGED );
    MonGroup lowStat_alpha_hists( this, m_path, lowStat, ATTRIB_MANAGED, "", "mergeRebinned" );
    
      //failure |= lb_hists.regHist(m_events_lb = TH1I_LW::create("events_lb", "Event Count", AthenaMonManager::altprod+1, -0.5, AthenaMonManager::altprod+0.5)).isFailure();
      failure |= lb_hists.regHist(m_events_lb = new TH1I("events_lb", "Event Count", AthenaMonManager::altprod+1, -0.5, AthenaMonManager::altprod+0.5)).isFailure();
      
      if (m_events_lb) {
	      for (int i = 1; i <= AthenaMonManager::altprod+1; i++) {
	          m_events_lb->GetXaxis()->SetBinLabel(i, envstrings[i-1]);
	      }
      } 
	//failure |= lowStat_alpha_hists.regHist(m_release_stage_lowStat = TH1I_LW::create("m_release_stage_lowStat", "Release - Event Count", AthenaMonManager::altprod+1, -0.5, AthenaMonManager::altprod+0.5)).isFailure();
	      failure |= lowStat_alpha_hists.regHist(m_release_stage_lowStat = new TH1I("m_release_stage_lowStat", "Release - Event Count", AthenaMonManager::altprod+1, -0.5, AthenaMonManager::altprod+0.5)).isFailure();
	      if (m_release_stage_lowStat) {
	          m_release_stage_lowStat->GetXaxis()->SetBinLabel(m_environment+1, m_releaseStageString.c_str());
	      }

  if (failure) {return  StatusCode::FAILURE;}
  else {return StatusCode::SUCCESS;}

}	

StatusCode 
DQTDataFlowMonTool::bookHistogramsRecurrent() {

    bool failure(false);
    MonGroup lowStat_hists( this, m_path, lowStat, ATTRIB_MANAGED );
    MonGroup run_hists( this, m_path, run, ATTRIB_MANAGED );
    
    if( m_environment == AthenaMonManager::tier0
    || m_environment == AthenaMonManager::tier0ESD ) {
    //  if (newRun) {
    for (int i=0; i < EventInfo::nDets; i++) {
      m_eventflag_run[i] = new TGraph();
      m_eventflag_run[i]->SetTitle((std::string("Nonzero Warning/Error Event Flags for ")
                       + eventflagdets[i]).c_str());
      m_eventflag_run[i]->SetName((std::string("eventflag_run_")
                      + eventflagdets[i]).c_str());
      failure |= run_hists.regGraph(m_eventflag_run[i]).isFailure();
      delete m_eventflag_vec[i];
      m_eventflag_vec[i] = new std::vector<EvFlagPt_t>;
      m_eventflag_vec[i]->reserve(1000);
    }
    //  }

    // if (newLumiBlock && newLowStatInterval) {
    //failure |= lowStat_hists.regHist(m_eventflag_summary_lowStat = TH2I_LW::create("eventflag_summary_lowStat", "Event Flag Summary", EventInfo::nDets+1, -0.5, EventInfo::nDets+0.5, 3, -0.5, 2.5)).isFailure();
    failure |= lowStat_hists.regHist(m_eventflag_summary_lowStat = new TH2I("eventflag_summary_lowStat", "Event Flag Summary", EventInfo::nDets+1, -0.5, EventInfo::nDets+0.5, 3, -0.5, 2.5)).isFailure();

    if (m_eventflag_summary_lowStat) {
      m_eventflag_summary_lowStat->GetYaxis()->SetBinLabel(1, "OK");
      m_eventflag_summary_lowStat->GetYaxis()->SetBinLabel(2, "Warning");
      m_eventflag_summary_lowStat->GetYaxis()->SetBinLabel(3, "Error");
      for (int i = 1; i <= EventInfo::nDets; i++) {
        m_eventflag_summary_lowStat->GetXaxis()->SetBinLabel(i, eventflagdets[i-1]);
      }
      m_eventflag_summary_lowStat->GetXaxis()->SetBinLabel(EventInfo::nDets+1, "All");
    }
    //  }
    }
  //else if (isNewEventsBlock) {
  //  return StatusCode::SUCCESS;
  //}
  if (failure) {return  StatusCode::FAILURE;}
  else {return StatusCode::SUCCESS;}
}

StatusCode 
DQTDataFlowMonTool::fillHistograms()
{

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in fillHists()" << endreq;
  //StatusCode sc;  
  
  m_events_lb->Fill(m_environment);
  m_release_stage_lowStat->Fill(m_environment);

  if( m_environment == AthenaMonManager::tier0 
      || m_environment == AthenaMonManager::tier0ESD ) {
    const EventInfo* evtinfo;
    StatusCode sc(evtStore()->retrieve(evtinfo));
    if (sc.isFailure()) {
      msg(MSG::WARNING) << "Could not retrieve EventInfo" << endreq;
    } else {
      EventInfo::EventFlagErrorState worststate = EventInfo::NotSet;
      for (int i = 0; i < EventInfo::nDets; i++) {
	      unsigned int flag = evtinfo->eventFlags((EventInfo::EventFlagSubDet) i);
	      EventInfo::EventFlagErrorState detstate = evtinfo->errorState((EventInfo::EventFlagSubDet) i);
	      if (flag != 0 && detstate > EventInfo::NotSet) {
	          m_eventflag_vec[i]->push_back(EvFlagPt_t(evtinfo->eventNumber(), flag));
	      }
	      if (detstate > worststate) worststate = detstate;
	          m_eventflag_summary_lowStat->Fill(i, detstate);
          }
          m_eventflag_summary_lowStat->Fill(EventInfo::nDets, worststate);
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode 
DQTDataFlowMonTool::procHistograms( )
{
  if ( endOfLumiBlock || endOfRun ) {
  
  if (endOfRun) {
    for (int i = 0; i < EventInfo::nDets; i++) {
      if (! m_eventflag_vec[i]) { continue ; }
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "processing Histograms " << i << endreq;
      m_eventflag_run[i]->Expand(m_eventflag_vec[i]->size(), 1);
      for(unsigned int j=0; j<m_eventflag_vec[i]->size(); j++) {
	      EvFlagPt_t & pt = (*(m_eventflag_vec[i]))[j];
	      m_eventflag_run[i]->SetPoint(j, pt.first, pt.second);
      }
    }
  }

  msg(MSG::DEBUG) << "in procHistograms()" << endreq;
  }
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------------
StatusCode DQTDataFlowMonTool::checkHists(bool /* fromFinalize */)
//----------------------------------------------------------------------------------
{

  msg(MSG::DEBUG) << "in checkHists()" << endreq;

  return StatusCode::SUCCESS;
}

