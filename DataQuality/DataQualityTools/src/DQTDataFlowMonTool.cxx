/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

#include "TGraph.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "StoreGate/ReadHandle.h"

static const char* envstrings[AthenaMonManager::altprod+1] = { "user", "noOutput", "online", "tier0", 
							       "tier0Raw", "tier0ESD", "AOD", "altprod" };
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
    //    m_eventflag_summary_lowStat(0),
    //    m_eventflag_summary_lb(0),
    m_sumweights(0)
//----------------------------------------------------------------------------------
{
  declareInterface<IMonitorToolBase>(this);
  m_path = "GLOBAL/DQTDataFlow";
  declareProperty("releaseString", m_releaseString);

  /*
  for (int i=0; i < EventInfo::nDets; i++) {
    m_eventflag_run[i] = 0;
    m_eventflag_vec[i] = 0;
  }
  */
}

//----------------------------------------------------------------------------------
DQTDataFlowMonTool::~DQTDataFlowMonTool()
//----------------------------------------------------------------------------------
{
  /*
  for (int i=0; i < EventInfo::nDets; i++) {
    delete m_eventflag_vec[i];
    m_eventflag_vec[i] = 0;
  }
  */
}

//----------------------------------------------------------------------------------
StatusCode DQTDataFlowMonTool::initialize()
//----------------------------------------------------------------------------------
{
  ATH_CHECK( m_eventInfoKey.initialize() );
  return DataQualityFatherMonTool::initialize();
}

//----------------------------------------------------------------------------------
StatusCode DQTDataFlowMonTool::bookHistograms(  )
//----------------------------------------------------------------------------------
{
  const EventContext& ctx = Gaudi::Hive::currentContext();
  bool failure(false);

  if (m_releaseStageString == "") {
    m_releaseStageString = m_releaseString + "-" + envstrings[m_environment];
  }

  ATH_MSG_DEBUG("in bookHistograms()");
  ATH_MSG_DEBUG("Using base path " << m_path);
  
  MonGroup lb_hists( this, m_path, lumiBlock, ATTRIB_MANAGED );
  MonGroup lowStat_alpha_hists( this, m_path, lowStat, ATTRIB_MANAGED );
  MonGroup run_hists( this, m_path, run, ATTRIB_MANAGED );
  MonGroup rolling_hists( this, m_path, run, ATTRIB_X_VS_LB, "", "merge" );
   
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

  SG::ReadHandle<xAOD::EventInfo> evtinfo (m_eventInfoKey, ctx);
  if (evtinfo->eventType(xAOD::EventInfo::IS_SIMULATION)) {
    failure |= rolling_hists.regHist(m_sumweights = new TH1D("m_sumweights", "Sum of MC event weights", 50, 0.5, 50.5)).isFailure();
  }

  if (failure) {return  StatusCode::FAILURE;}
  else {return StatusCode::SUCCESS;}

}	

StatusCode 
DQTDataFlowMonTool::bookHistogramsRecurrent() {

    bool failure(false);
    MonGroup lowStat_hists( this, m_path, lowStat, ATTRIB_UNMANAGED );
    MonGroup run_hists( this, m_path, run, ATTRIB_UNMANAGED );
    
    if( m_environment != AthenaMonManager::tier0Raw ) {
      /*
      if (newRunFlag()) {
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
      }
      */

      if (newLumiBlockFlag() && newLowStatIntervalFlag()) {
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
      }
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
  ATH_MSG_DEBUG("in fillHists()");
  //StatusCode sc;  
  
  m_events_lb->Fill(m_environment);
  m_release_stage_lowStat->Fill(m_environment);

  if( m_environment != AthenaMonManager::tier0Raw ) {
    SG::ReadHandle<xAOD::EventInfo> evtinfo(m_eventInfoKey);
    if (! evtinfo.isValid()) {
      ATH_MSG_ERROR("Could not retrieve EventInfo");
      return StatusCode::FAILURE;
    } else {
      if (m_sumweights) { 
	m_sumweights->Fill(evtinfo->lumiBlock(), evtinfo->mcEventWeight()); 
      }
      EventInfo::EventFlagErrorState worststate = EventInfo::NotSet;
      for (int i = 0; i < EventInfo::nDets; i++) {
	//unsigned int flag = evtinfo->eventFlags((EventInfo::EventFlagSubDet) i);
	EventInfo::EventFlagErrorState detstate = evtinfo->errorState((EventInfo::EventFlagSubDet) i);
	/*
	if (flag != 0 && detstate > EventInfo::NotSet) {
	  m_eventflag_vec[i]->push_back(EvFlagPt_t(evtinfo->eventNumber(), flag));
	}
	*/
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
  /*
  if ( endOfLumiBlockFlag() || endOfRunFlag() ) {
  
    if (endOfRunFlag()) {
      for (int i = 0; i < EventInfo::nDets; i++) {
	if (! m_eventflag_vec[i]) { continue ; }
	ATH_MSG_DEBUG("processing Histograms " << i);
	  m_eventflag_run[i]->Expand(m_eventflag_vec[i]->size(), 1);
	  for(unsigned int j=0; j<m_eventflag_vec[i]->size(); j++) {
	  EvFlagPt_t & pt = (*(m_eventflag_vec[i]))[j];
	  m_eventflag_run[i]->SetPoint(j, pt.first, pt.second);
	  }
	
      }
    }

    ATH_MSG_DEBUG("in procHistograms()");
  }
  */
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------------
StatusCode DQTDataFlowMonTool::checkHists(bool /* fromFinalize */)
//----------------------------------------------------------------------------------
{
  /*
  ATH_MSG_DEBUG("in checkHists()");
  */
  return StatusCode::SUCCESS;
}

