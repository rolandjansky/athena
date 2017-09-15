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
#include <algorithm>

#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"

#include "LArMonTools/LArCollisionTimeMonTool.h"
#include "LArRecEvent/LArCollisionTime.h"

#include "xAODEventInfo/EventInfo.h"

#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"

using namespace std;

/*---------------------------------------------------------*/
LArCollisionTimeMonTool::LArCollisionTimeMonTool(const std::string& type, 
						 const std::string& name,
						 const IInterface* parent)
  : ManagedMonitorToolBase(type, name, parent), 
    m_lumi_blocks( 1500 ),
    m_ECTimeDiff(0),
    m_ECTimeAvg(0),
    m_timeCut(5.0),
    m_minCells(2),
    m_eWeighted(true),
    m_newrun(true),
    //m_bunchGroupTool("BunchGroupTool"),
    m_bunchGroupTool("BunchCrossingTool"),
    m_bcid_init(false)
{
  declareProperty( "m_lumi_blocks"	,      m_lumi_blocks = 1000 );
  declareProperty( "timeDiffCut"	,      m_timeCut = 5.0 );
  declareProperty( "nCells"		,      m_minCells = 2 );
  declareProperty( "eWeighted"		,      m_eWeighted = true );
  declareProperty( "Key"                ,      m_key="LArCollisionTime");
  declareProperty( "histPath"           ,      m_histPath="LArCollisionTime"); 
  declareProperty( "BunchCrossingTool"  ,      m_bunchGroupTool); 
  declareProperty( "TrainFrontDistance" ,      m_distance = 30); 
  declareProperty( "IsOnline"           ,      m_IsOnline=false);
  
  m_eventsCounter = 0;  
  
  m_LArCollTime_h			= nullptr; 
  m_LArCollTime_lb_h			= nullptr;
  m_LArCollTime_lb_timeCut_h		= nullptr;
  m_LArCollTime_lb_singlebeam_timeCut_h = nullptr;
  m_LArCollTime_vs_LB_h			= nullptr; 
  m_LArCollTime_vs_BCID_h		= nullptr;
  m_LArCollAvgTime_h			= nullptr; 
  m_LArCollAvgTime_vs_LB_h		= nullptr; 
  m_LArCollAvgTime_vs_BCID_h            = nullptr;

  m_nhist=1;
}

/*---------------------------------------------------------*/
LArCollisionTimeMonTool::~LArCollisionTimeMonTool()
{
  cleanup();
}

/*---------------------------------------------------------*/
StatusCode 
LArCollisionTimeMonTool::initialize() {

  ManagedMonitorToolBase::initialize().ignore();
  CHECK(m_bunchGroupTool.retrieve());
  ATH_MSG_DEBUG( "Successful Initialize LArCollisionTimeMonTool " );
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode 
LArCollisionTimeMonTool::bookHistograms() {
  
  cleanup(); //to be sure...

  if(m_IsOnline)  m_nhist=2; else m_nhist=1;
  // So far 2 histos, all bcid and inside the train
  m_LArCollTime_h=new TH1F_LW*[m_nhist];
  m_LArCollAvgTime_h=new TH1F_LW*[m_nhist];
  m_LArCollTime_lb_h=new TH1F_LW*[m_nhist];
  m_LArCollTime_lb_timeCut_h=new TH1F_LW*[m_nhist];
  m_LArCollTime_lb_singlebeam_timeCut_h=new TH1F_LW*[m_nhist];
  m_LArCollTime_vs_LB_h=new TH2F_LW*[m_nhist];
  m_LArCollTime_vs_BCID_h=new TH2F_LW*[m_nhist];
  m_LArCollAvgTime_vs_LB_h=new TH2F_LW*[m_nhist];
  m_LArCollAvgTime_vs_BCID_h=new TH2F_LW*[m_nhist];

  std::vector<std::string> hnameadd(m_nhist); 
  if(m_IsOnline) hnameadd={"","_intrain"}; else hnameadd={""};
  std::vector<std::string> htitadd(m_nhist); 
  if(m_IsOnline) htitadd={"",", inside the train"}; else htitadd={""};

  MonGroup generalGroupShift( this, "/LAr/"+m_histPath+"/", run, ATTRIB_MANAGED);
  MonGroup generalGroupLB( this, "/LAr/"+m_histPath+"/", run, ATTRIB_X_VS_LB, "", "merge");
  //if(isNewRun ){ // Commented by B.Trocme to comply with new ManagedMonitorToolBase
    m_newrun=true;
    //
    // Create top folder for histos
    //
  for(unsigned i=0; i<m_nhist; ++i) {   
    m_LArCollTime_h[i] = TH1F_LW::create(("LArCollTime"+hnameadd[i]).data(), 
			       ("LArCollisionTime - difference of avg time from ECC and ECA"+htitadd[i]).data(),
			       101, -50.5, 50.5 );			
    
    m_LArCollTime_h[i]->GetYaxis()->SetTitle("Number of events (weighted by energy/GeV) per ns");
    m_LArCollTime_h[i]->GetXaxis()->SetTitle("<t_{C}> - <t_{A}> (ns)");
    generalGroupShift.regHist(m_LArCollTime_h[i]).ignore();
    
    m_LArCollTime_lb_h[i] = TH1F_LW::create(("LArCollTimeLumiBlock"+hnameadd[i]).data(), 
				     ("LArCollisionTime - difference of avg time from ECC and ECA"+htitadd[i]).data(),
				  101, -50.5, 50.5 );			
    
    m_LArCollTime_lb_h[i]->GetYaxis()->SetTitle("Number of events (weighted by energy/GeV) per ns");
    m_LArCollTime_lb_h[i]->GetXaxis()->SetTitle("<t_{C}> - <t_{A}> (ns)");
    generalGroupShift.regHist(m_LArCollTime_lb_h[i]).ignore();
    
    
    
    m_LArCollTime_vs_LB_h[i] = TH2F_LW::create(("LArCollTime_vs_LB"+hnameadd[i]).data(), 
				        ("LArCollisionTime Vs Luminosity Block - difference of avg time of ECC and ECA as a function of luminosity block"+htitadd[i]).data(),
				     m_lumi_blocks, 0.5, double(m_lumi_blocks)+0.5, 101, -50.5, 50.5 );			
    
    m_LArCollTime_vs_LB_h[i]->GetXaxis()->SetTitle("Luminosity Block Number");
    m_LArCollTime_vs_LB_h[i]->GetYaxis()->SetTitle("<t_{C}> - <t_{A}> (ns)");
    m_LArCollTime_vs_LB_h[i]->GetZaxis()->SetTitle("Number of events (weighted by energy/GeV)");
    generalGroupLB.regHist(m_LArCollTime_vs_LB_h[i]).ignore();
    
    m_LArCollTime_vs_BCID_h[i] = TH2F_LW::create(("LArCollTime_vs_BCID"+hnameadd[i]).data(), 
				          ("LArCollisionTime Vs BCID - difference of avg time of ECC and ECA as a function of BCID"+htitadd[i]).data(),
				       3564, 0.5, 3564.5, 101, -50.5, 50.5 );			
    
    m_LArCollTime_vs_BCID_h[i]->GetXaxis()->SetTitle("Bunch Crossing Number");
    m_LArCollTime_vs_BCID_h[i]->GetYaxis()->SetTitle("<t_{C}> - <t_{A}> (ns)");
    m_LArCollTime_vs_BCID_h[i]->GetZaxis()->SetTitle("Number of events (weighted by energy/GeV)");
    generalGroupShift.regHist(m_LArCollTime_vs_BCID_h[i]).ignore();  
    
    // Average Time
    
    m_LArCollAvgTime_h[i] = TH1F_LW::create(("LArCollAvgTime"+hnameadd[i]).data(), 
				     ("LArCollisionAverageTime - avg time of ECC and ECA"+htitadd[i]).data(),
				  320, -40., 40. );			
    
    m_LArCollAvgTime_h[i]->GetYaxis()->SetTitle("Number of events (weighted by energy/GeV) per ns");
    m_LArCollAvgTime_h[i]->GetXaxis()->SetTitle("(<t_{C}> + <t_{A}>) / 2 (ns)");
    generalGroupShift.regHist(m_LArCollAvgTime_h[i]).ignore();
    
    m_LArCollAvgTime_vs_LB_h[i] = TH2F_LW::create(("LArCollAvgTime_vs_LB"+hnameadd[i]).data(), 
					   ("LArCollisionAvgTime Vs Luminosity Block - avg time of ECC and ECA as a function of luminosity block"+htitadd[i]).data(),
					m_lumi_blocks, 0.5, double(m_lumi_blocks)+0.5, 320, -40., 40. );			
    
    m_LArCollAvgTime_vs_LB_h[i]->GetXaxis()->SetTitle("Luminosity Block Number");
    m_LArCollAvgTime_vs_LB_h[i]->GetYaxis()->SetTitle("(<t_{C}> + <t_{A}>) / 2 (ns)");
    m_LArCollAvgTime_vs_LB_h[i]->GetZaxis()->SetTitle("Number of events (weighted by energy/GeV)");
    generalGroupLB.regHist(m_LArCollAvgTime_vs_LB_h[i]).ignore();
    
    m_LArCollAvgTime_vs_BCID_h[i] = TH2F_LW::create(("LArCollAvgTime_vs_BCID"+hnameadd[i]).data(), 
					     ("LArCollisionAvgTime Vs BCID - avg time of ECC and ECA as a function of luminosity block"+htitadd[i]).data(),
					m_lumi_blocks, 0.5, double(m_lumi_blocks)+0.5, 320, -40., 40. );			
    
    m_LArCollAvgTime_vs_BCID_h[i]->GetXaxis()->SetTitle("Bunch Crossing Number");
    m_LArCollAvgTime_vs_BCID_h[i]->GetYaxis()->SetTitle("(<t_{C}> + <t_{A}>) / 2 (ns)");
    m_LArCollAvgTime_vs_BCID_h[i]->GetZaxis()->SetTitle("Number of events (weighted by energy/GeV)");
    generalGroupShift.regHist(m_LArCollAvgTime_vs_BCID_h[i]).ignore();
    
    //Monitoring events as a function of LB when events lie in a time window of +/-10ns
    m_LArCollTime_lb_timeCut_h[i] = TH1F_LW::create(("LArCollTimeLumiBlockTimeCut"+hnameadd[i]).data(), 
					     ("Events with abs(<t_{C}> - <t_{A}>) < 10ns as a function of LB"+htitadd[i]).data(),
					  m_lumi_blocks, 0.5, double(m_lumi_blocks)+0.5 );			
    
    m_LArCollTime_lb_timeCut_h[i]->GetYaxis()->SetTitle("Number of events per LB");
    m_LArCollTime_lb_timeCut_h[i]->GetXaxis()->SetTitle("Luminosity Block Number");
    generalGroupShift.regHist(m_LArCollTime_lb_timeCut_h[i]).ignore();
    
    //Monitoring events as a function of LB when events lie in a time window of +/-[20-30]ns
    m_LArCollTime_lb_singlebeam_timeCut_h[i] = TH1F_LW::create(("LArCollTimeLumiBlockSingleBeamTimeCut"+hnameadd[i]).data(), 
						        ("Events with 20 ns < abs(<t_{C}> - <t_{A}>) < 30ns as a function of LB"+htitadd[i]).data(),
						     m_lumi_blocks, 0.5, double(m_lumi_blocks)+0.5 );			
    
    m_LArCollTime_lb_singlebeam_timeCut_h[i]->GetYaxis()->SetTitle("Number of events per LB");
    m_LArCollTime_lb_singlebeam_timeCut_h[i]->GetXaxis()->SetTitle("Luminosity Block Number");
    generalGroupShift.regHist(m_LArCollTime_lb_singlebeam_timeCut_h[i]).ignore();
    
  }
  return StatusCode::SUCCESS;
    
    //  }// end isNewRun
  
}


/*---------------------------------------------------------*/
StatusCode 
LArCollisionTimeMonTool::fillHistograms()
{
  ATH_MSG_DEBUG( "in fillHists()" );
  
  // Increment event counter
  m_eventsCounter++;
  
  // --- retrieve event information ---
  const xAOD::EventInfo* event_info;
  unsigned bunch_crossing_id = 0;
  unsigned lumi_block        = 0;
  //double event_time_minutes = -1;
  if (evtStore()->retrieve( event_info ).isFailure()) {
    ATH_MSG_ERROR( "Failed to retrieve EventInfo object" );
    return StatusCode::FAILURE;
  }

  // bunch crossing ID:
  bunch_crossing_id = event_info->bcid();
    
  // luminosity block number
  lumi_block = event_info->lumiBlock();
    
  if(m_bunchGroupTool->bcType(bunch_crossing_id) == Trig::IBunchCrossingTool::Empty) {
     //ATH_MSG_INFO("BCID: "<<bunch_crossing_id<<" empty, not filling CollTime" );
     ATH_MSG_INFO("BCID: "<<bunch_crossing_id<<" empty ? not filling the coll. time" );
     return StatusCode::SUCCESS; // not filling anything in empty bunches
  }

  int bcid_distance = m_bunchGroupTool->distanceFromFront(bunch_crossing_id, Trig::IBunchCrossingTool::BunchCrossings);
  
  // Retrieve LArCollision Timing information
  const LArCollisionTime * larTime;
  if(evtStore()->retrieve(larTime,m_key).isFailure())
  {
    ATH_MSG_WARNING( "Unable to retrieve LArCollisionTime event store" );
    return StatusCode::SUCCESS; // Check if failure shd be returned. VB
  } else {
    ATH_MSG_DEBUG( "LArCollisionTime successfully retrieved from event store" );
  }

  if (larTime and !(event_info->isEventFlagBitSet(xAOD::EventInfo::LAr,3))) {// Do not fill histo if noise burst suspected
    // Calculate the time diff between ECC and ECA
    m_ECTimeDiff = larTime->timeC() - larTime->timeA();
    m_ECTimeAvg  = (larTime->timeC() + larTime->timeA()) / 2.0;
    if (larTime->ncellA() > m_minCells && larTime->ncellC() > m_minCells && std::fabs(m_ECTimeDiff) < m_timeCut ) { // Only fill histograms if a minimum number of cells were found and time difference was sensible
      double weight = 1;
      if (m_eWeighted) weight = (larTime->energyA()+larTime->energyC())*1e-3; 
      m_LArCollTime_h[0]->Fill(m_ECTimeDiff,weight);
      m_LArCollTime_lb_h[0]->Fill(m_ECTimeDiff,weight);
      m_LArCollTime_vs_LB_h[0]->Fill(lumi_block, m_ECTimeDiff,weight);
      m_LArCollTime_vs_BCID_h[0]->Fill(bunch_crossing_id, m_ECTimeDiff,weight);
      m_LArCollAvgTime_h[0]->Fill(m_ECTimeAvg,weight);
      m_LArCollAvgTime_vs_LB_h[0]->Fill(lumi_block, m_ECTimeAvg,weight);
      m_LArCollAvgTime_vs_BCID_h[0]->Fill(bunch_crossing_id, m_ECTimeAvg,weight);
      if ( fabs(m_ECTimeDiff) < 10 ) m_LArCollTime_lb_timeCut_h[0]->Fill(lumi_block);
      if ( fabs(m_ECTimeDiff) > 20 && fabs(m_ECTimeDiff) < 30 ) m_LArCollTime_lb_singlebeam_timeCut_h[0]->Fill(lumi_block);
      if(m_IsOnline && bcid_distance > m_distance) { // fill histos inside the train
        ATH_MSG_INFO("BCID: "<<bunch_crossing_id<<" distance from Front: "<<bcid_distance<<"Filling in train...");    
        m_LArCollTime_h[1]->Fill(m_ECTimeDiff,weight);
        m_LArCollTime_lb_h[1]->Fill(m_ECTimeDiff,weight);
        m_LArCollTime_vs_LB_h[1]->Fill(lumi_block, m_ECTimeDiff,weight);
        m_LArCollTime_vs_BCID_h[1]->Fill(bunch_crossing_id, m_ECTimeDiff,weight);
        m_LArCollAvgTime_h[1]->Fill(m_ECTimeAvg,weight);
        m_LArCollAvgTime_vs_LB_h[1]->Fill(lumi_block, m_ECTimeAvg,weight);
        m_LArCollAvgTime_vs_BCID_h[1]->Fill(bunch_crossing_id, m_ECTimeAvg,weight);
        if ( fabs(m_ECTimeDiff) < 10 ) m_LArCollTime_lb_timeCut_h[1]->Fill(lumi_block);
        if ( fabs(m_ECTimeDiff) > 20 && fabs(m_ECTimeDiff) < 30 ) m_LArCollTime_lb_singlebeam_timeCut_h[1]->Fill(lumi_block);
      }
    }
  }
  
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode LArCollisionTimeMonTool::procHistograms()
{
  
  if(endOfLumiBlockFlag() ){
    // For online monitoring, reset the histogram after Lumi block finishes
    for(unsigned i=0; i<m_nhist; ++i) m_LArCollTime_lb_h[i]->Reset();
  }
  
  ATH_MSG_DEBUG( "End of procHistograms " );
  return StatusCode::SUCCESS;
}


void LArCollisionTimeMonTool::cleanup() {
  //Delete the array of pointers to histograms
  //The histograms themselves should be owned by THistSvc at this point
  delete[] m_LArCollTime_h; 
  m_LArCollTime_h=nullptr;
  delete[] m_LArCollTime_lb_h;			    
  m_LArCollTime_lb_h=nullptr;
  delete[] m_LArCollTime_lb_timeCut_h;		    
  m_LArCollTime_lb_timeCut_h=nullptr;
  delete[] m_LArCollTime_lb_singlebeam_timeCut_h;  
  m_LArCollTime_lb_singlebeam_timeCut_h=nullptr;
  delete[] m_LArCollTime_vs_LB_h;
  m_LArCollTime_vs_LB_h=nullptr;
  delete[] m_LArCollTime_vs_BCID_h;
  m_LArCollTime_vs_BCID_h=nullptr;
  delete[] m_LArCollAvgTime_h; 	
  m_LArCollAvgTime_h=nullptr;
  delete[] m_LArCollAvgTime_vs_LB_h; 		    
  m_LArCollAvgTime_vs_LB_h=nullptr;
  delete[] m_LArCollAvgTime_vs_BCID_h;             
  m_LArCollAvgTime_vs_BCID_h=nullptr;
}

