/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     LArCollisionTimeMonTool.cxx
// PACKAGE:  LArMonTools
//
// AUTHOR:   Vikas Bansal
//
// Class for monitoring : EC timing from LArCollisionTime Container 

//
// ********************************************************************


#include <sstream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <functional>
#include <map>
#include <utility>

#include "LArMonTools/LArCollisionTimeMonTool.h"
#include "LArRecEvent/LArCollisionTime.h"

#include "xAODEventInfo/EventInfo.h"

using namespace std;

/*---------------------------------------------------------*/
LArCollisionTimeMonTool::LArCollisionTimeMonTool(const std::string& type, 
						 const std::string& name,
						 const IInterface* parent)
  : ManagedMonitorToolBase(type, name, parent), 
    m_lumi_blocks( 1000 ),
    m_ECTimeDiff(0),
    m_ECTimeAvg(0),
    m_timeCut(5.0),
    m_minCells(2),
    m_eWeighted(true),
    newrun(true)
{
  declareProperty( "m_lumi_blocks"	,      m_lumi_blocks = 1000 );
  declareProperty( "timeDiffCut"	,      m_timeCut = 5.0 );
  declareProperty( "nCells"		,      m_minCells = 2 );
  declareProperty( "eWeighted"		,      m_eWeighted = true );
  
  m_eventsCounter = 0;  
  
  m_LArCollTime_h			= NULL; 
  m_LArCollTime_lb_h			= NULL;
  m_LArCollTime_lb_timeCut_h		= NULL;
  m_LArCollTime_lb_singlebeam_timeCut_h = NULL;
  m_LArCollTime_vs_LB_h			= NULL; 
  m_LArCollTime_vs_BCID_h		= NULL;
  m_LArCollAvgTime_h			= NULL; 
  m_LArCollAvgTime_vs_LB_h		= NULL; 
}

/*---------------------------------------------------------*/
LArCollisionTimeMonTool::~LArCollisionTimeMonTool()
{
}

/*---------------------------------------------------------*/
StatusCode 
LArCollisionTimeMonTool::initialize() {

  ManagedMonitorToolBase::initialize().ignore();
  msg(MSG::DEBUG) << "Successful Initialize LArCollisionTimeMonTool " << endreq;
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode 
LArCollisionTimeMonTool::bookHistograms() {
  
  MonGroup generalGroupShift( this, "/LAr/LArCollisionTime/", run, ATTRIB_MANAGED);
  //if(isNewRun ){ // Commented by B.Trocme to comply with new ManagedMonitorToolBase
    newrun=true;
    //
    // Create top folder for histos
    //
    
    m_LArCollTime_h = new TH1F("LArCollTime", 
			       "LArCollisionTime - difference of avg time from ECC and ECA",
			       101, -50.5, 50.5 );			
    
    m_LArCollTime_h->GetYaxis()->SetTitle("Number of events (weighted by energy/GeV) per ns");
    m_LArCollTime_h->GetXaxis()->SetTitle("<t_{C}> - <t_{A}> (ns)");
    generalGroupShift.regHist(m_LArCollTime_h).ignore();
    
    m_LArCollTime_lb_h = new TH1F("LArCollTimeLumiBlock", 
				  "LArCollisionTime - difference of avg time from ECC and ECA",
				  101, -50.5, 50.5 );			
    
    m_LArCollTime_lb_h->GetYaxis()->SetTitle("Number of events (weighted by energy/GeV) per ns");
    m_LArCollTime_lb_h->GetXaxis()->SetTitle("<t_{C}> - <t_{A}> (ns)");
    generalGroupShift.regHist(m_LArCollTime_lb_h).ignore();
    
    
    
    m_LArCollTime_vs_LB_h = new TH2F("LArCollTime_vs_LB", 
				     "LArCollisionTime Vs Luminosity Block - difference of avg time of ECC and ECA as a function of luminosity block",
				     m_lumi_blocks, 0.5, double(m_lumi_blocks)+0.5, 101, -50.5, 50.5 );			
    
    m_LArCollTime_vs_LB_h->GetXaxis()->SetTitle("Luminosity Block Number");
    m_LArCollTime_vs_LB_h->GetYaxis()->SetTitle("<t_{C}> - <t_{A}> (ns)");
    m_LArCollTime_vs_LB_h->GetZaxis()->SetTitle("Number of events (weighted by energy/GeV)");
    generalGroupShift.regHist(m_LArCollTime_vs_LB_h).ignore();
    
    m_LArCollTime_vs_BCID_h = new TH2F("LArCollTime_vs_BCID", 
				       "LArCollisionTime Vs BCID - difference of avg time of ECC and ECA as a function of BCID",
				       3564, 0.5, 3564.5, 101, -50.5, 50.5 );			
    
    m_LArCollTime_vs_BCID_h->GetXaxis()->SetTitle("Bunch Crossing Number");
    m_LArCollTime_vs_BCID_h->GetYaxis()->SetTitle("<t_{C}> - <t_{A}> (ns)");
    m_LArCollTime_vs_BCID_h->GetZaxis()->SetTitle("Number of events (weighted by energy/GeV)");
    generalGroupShift.regHist(m_LArCollTime_vs_BCID_h).ignore();  
    
    // Average Time
    
    m_LArCollAvgTime_h = new TH1F("LArCollAvgTime", 
				  "LArCollisionAverageTime - avg time of ECC and ECA",
				  320, -40., 40. );			
    
    m_LArCollAvgTime_h->GetYaxis()->SetTitle("Number of events (weighted by energy/GeV) per ns");
    m_LArCollAvgTime_h->GetXaxis()->SetTitle("(<t_{C}> + <t_{A}>) / 2 (ns)");
    generalGroupShift.regHist(m_LArCollAvgTime_h).ignore();
    
    m_LArCollAvgTime_vs_LB_h = new TH2F("LArCollAvgTime_vs_LB", 
					"LArCollisionAvgTime Vs Luminosity Block - avg time of ECC and ECA as a function of luminosity block",
					m_lumi_blocks, 0.5, double(m_lumi_blocks)+0.5, 320, -40., 40. );			
    
    m_LArCollAvgTime_vs_LB_h->GetXaxis()->SetTitle("Luminosity Block Number");
    m_LArCollAvgTime_vs_LB_h->GetYaxis()->SetTitle("(<t_{C}> + <t_{A}>) / 2 (ns)");
    m_LArCollAvgTime_vs_LB_h->GetZaxis()->SetTitle("Number of events (weighted by energy/GeV)");
    generalGroupShift.regHist(m_LArCollAvgTime_vs_LB_h).ignore();
    
    //Monitoring events as a function of LB when events lie in a time window of +/-10ns
    m_LArCollTime_lb_timeCut_h = new TH1F("LArCollTimeLumiBlockTimeCut", 
					  "Events with abs(<t_{C}> - <t_{A}>) < 10ns as a function of LB",
					  m_lumi_blocks, 0.5, double(m_lumi_blocks)+0.5 );			
    
    m_LArCollTime_lb_timeCut_h->GetYaxis()->SetTitle("Number of events per LB");
    m_LArCollTime_lb_timeCut_h->GetXaxis()->SetTitle("Luminosity Block Number");
    generalGroupShift.regHist(m_LArCollTime_lb_timeCut_h).ignore();
    
    //Monitoring events as a function of LB when events lie in a time window of +/-[20-30]ns
    m_LArCollTime_lb_singlebeam_timeCut_h = new TH1F("LArCollTimeLumiBlockSingleBeamTimeCut", 
						     "Events with 20 ns < abs(<t_{C}> - <t_{A}>) < 30ns as a function of LB",
						     m_lumi_blocks, 0.5, double(m_lumi_blocks)+0.5 );			
    
    m_LArCollTime_lb_singlebeam_timeCut_h->GetYaxis()->SetTitle("Number of events per LB");
    m_LArCollTime_lb_singlebeam_timeCut_h->GetXaxis()->SetTitle("Luminosity Block Number");
    generalGroupShift.regHist(m_LArCollTime_lb_singlebeam_timeCut_h).ignore();
    
    return StatusCode::SUCCESS;
    
    //  }// end isNewRun
  
}


/*---------------------------------------------------------*/
StatusCode 
LArCollisionTimeMonTool::fillHistograms()
{
  msg(MSG::DEBUG) << "in fillHists()" << endreq;
  
  // Increment event counter
  m_eventsCounter++;
  
  // --- retrieve event information ---
  const xAOD::EventInfo* event_info;
  unsigned bunch_crossing_id = 0;
  unsigned lumi_block        = 0;
  //double event_time_minutes = -1;
  if (evtStore()->retrieve( event_info ).isFailure()) {
    msg(MSG::ERROR) << "Failed to retrieve EventInfo object" << endreq;
    return StatusCode::FAILURE;
  }

  // bunch crossing ID:
  bunch_crossing_id = event_info->bcid();
    
  // luminosity block number
  lumi_block = event_info->lumiBlock();
    
  
  // Retrieve LArCollision Timing information
  const LArCollisionTime * larTime;
  if(evtStore()->retrieve(larTime,"LArCollisionTime").isFailure())
  {
    msg(MSG::WARNING) << "Unable to retrieve LArCollisionTime event store" << endreq;
    return StatusCode::SUCCESS; // Check if failure shd be returned. VB
  } else {
    msg(MSG::DEBUG) << "LArCollisionTime successfully retrieved from event store" << endreq;
  }

  if (larTime and !(event_info->isEventFlagBitSet(xAOD::EventInfo::LAr,3))) {// Do not fill histo if noise burst suspected
    // Calculate the time diff between ECC and ECA
    m_ECTimeDiff = larTime->timeC() - larTime->timeA();
    m_ECTimeAvg  = (larTime->timeC() + larTime->timeA()) / 2.0;
    if (larTime->ncellA() > m_minCells && larTime->ncellC() > m_minCells && std::fabs(m_ECTimeDiff) < m_timeCut ) { // Only fill histograms if a minimum number of cells were found and time difference was sensible
      double weight = 1;
      if (m_eWeighted) weight = (larTime->energyA()+larTime->energyC())/1000; 
      m_LArCollTime_h->Fill(m_ECTimeDiff,weight);
      m_LArCollTime_lb_h->Fill(m_ECTimeDiff,weight);
      m_LArCollTime_vs_LB_h->Fill(lumi_block, m_ECTimeDiff,weight);
      m_LArCollTime_vs_BCID_h->Fill(bunch_crossing_id, m_ECTimeDiff,weight);
      m_LArCollAvgTime_h->Fill(m_ECTimeAvg,weight);
      m_LArCollAvgTime_vs_LB_h->Fill(lumi_block, m_ECTimeAvg,weight);
      if ( fabs(m_ECTimeDiff) < 10 ) m_LArCollTime_lb_timeCut_h->Fill(lumi_block);
      if ( fabs(m_ECTimeDiff) > 20 and fabs(m_ECTimeDiff) < 30 ) m_LArCollTime_lb_singlebeam_timeCut_h->Fill(lumi_block);
    }
  }
  
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode LArCollisionTimeMonTool::procHistograms()
{
  
  if(endOfLumiBlock ){
    // For online monitoring, reset the histogram after Lumi block finishes
    m_LArCollTime_lb_h->Reset();
  }
  
  msg(MSG::DEBUG) << "End of procHistograms " << endreq;
  return StatusCode::SUCCESS;
}
