//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/ISvcLocator.h"

#include "TBMonitoring/TBTailCatcherRawMonTool.h"

#include "TBEvent/TBTailCatcherRaw.h"
#include "TBEvent/TBScintillatorRaw.h"
#include "TBEvent/TBTailCatcher.h"
#include "TBEvent/TBScintillator.h"

#include <vector>
#include <string>

/*---------------------------------------------------------*/
TBTailCatcherRawMonTool::TBTailCatcherRawMonTool(const std::string & type, 
				 const std::string & name,
				 const IInterface* parent)
  : MonitorToolBase(type, name, parent),
    m_histo_tcScintADC(nullptr),
    m_histo_tcreco(nullptr),
    m_histo_tcTotal(nullptr),
    m_tcScintNum(0)
/*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);
  //declareProperty("histoPathBase",m_path = "/stat"); 
  m_path = "/stat";
  
  //TailCatcher histogram properties
  declareProperty("TailCatcherMax",m_posMax=1024);
  declareProperty("TailCatcherMin",m_posMin=0);
  declareProperty("TailCatcherMaxReco",m_posMaxreco=100);
  declareProperty("TailCatcherMinReco",m_posMinreco=-20);
  declareProperty("TailCatcherBinNum",m_numBin=500);
  declareProperty("MonitorTailCatcherRaw",m_monitor_tailcatcher_raw=true);
  declareProperty("MonitorTailCatcherReco",m_monitor_tailcatcher_reco=true);
  declareProperty("TestTailCatcherMonTool",m_testTool=false);
}

/*---------------------------------------------------------*/
TBTailCatcherRawMonTool::~TBTailCatcherRawMonTool()
/*---------------------------------------------------------*/
{}

/*---------------------------------------------------------*/
StatusCode TBTailCatcherRawMonTool:: initialize()
/*---------------------------------------------------------*/
{
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TBTailCatcherRawMonTool::bookHists()
/*---------------------------------------------------------*/
{
  // This is called by parent class in initialize().
  // We want to book histos according to 1st event so 
  // we use mybookHist()
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TBTailCatcherRawMonTool::mybookHists()
/*---------------------------------------------------------*/
{
  ATH_MSG_DEBUG ( "in mybookHists()" );

  //Start of tailcatcher raw histos
  TBTailCatcherRaw * tcRaw=NULL;
  TBTailCatcher * tcReco=NULL;
  if(m_monitor_tailcatcher_raw){
    //Retrieve tailcatcher raw 
    StatusCode sc = evtStore()->retrieve(tcRaw);
    
    if (sc.isFailure()){
      ATH_MSG_INFO 
        ( "TBTailCatcherMonTool: Retrieval of TailCatcher Raw failed"  );
      m_monitor_tailcatcher_raw=false;
    }else m_tcScintNum = tcRaw->size(); //Get number of scintillators with the tailcatcher
  }
  if(m_monitor_tailcatcher_reco){
    //Retrieve tailcatcher raw 
    StatusCode sc = evtStore()->retrieve(tcReco);
    
    if (sc.isFailure()){
      ATH_MSG_INFO 
        ( "TBTailCatcherMonTool: Retrieval of TailCatcher failed"  );
      m_monitor_tailcatcher_reco=false;
    }else m_tcScintNum = tcReco->size(); //Get number of scintillators with the tailcatcher
  }




  if(m_monitor_tailcatcher_reco|m_monitor_tailcatcher_raw){
       
    ATH_MSG_DEBUG ( "Number od Scintillator in tailcatcher: " << m_tcScintNum);
       
       if(m_tcScintNum == 0){
	 ATH_MSG_INFO 
           ( "!! Warning !! "
	     << "There is no scintillator recorded in the tailcather." );
	 SetBookStatus(false);
       } else {


	 TBTailCatcherRaw::const_iterator iter;
	 if(m_monitor_tailcatcher_raw){
	   iter = tcRaw->begin();
	   //Create m_tcScintNum of histograms
	    m_histo_tcScintADC = new IHistogram1D*[m_tcScintNum];
	 }
	 TBTailCatcher::const_iterator iterreco;
	 if(m_monitor_tailcatcher_reco){
	   iterreco = tcReco->begin();
	   //Create m_tcScintNum of histograms
	    m_histo_tcreco = new IHistogram1D*[m_tcScintNum];

	    std::string hname = m_path;
	    hname += "/BeamDetectors/TailCatcher/";
	    hname += "tcTotal";
	    std::string htitle = "TailCatcher sum of signal in pC";
	    m_histo_tcTotal = 
	      ToolHistoSvc()->book(hname,htitle,m_numBin,m_posMinreco,m_posMaxreco);
	 }
    
	 
	 

	 for(int i = 0; i < m_tcScintNum; i++){
	   ATH_MSG_DEBUG ( "Number of scintillators in book loop: "
                           << m_tcScintNum );
	   std::string name;
	   if(m_monitor_tailcatcher_reco) name = (*iterreco)->getDetectorName();
	   if(m_monitor_tailcatcher_raw) name = (*iter)->getDetectorName();
	   m_scintNames.push_back(name);
	   if(i<12){
	     name = "layer1"; 
	   }else if(i<24){
	     name = "layer2";
	   }else if(i<30){
	     name = "layer3";
	   }else if(i<36){
	     name = "layer4";
	   }else if(i<42){
	     name = "layer5";
	   }else if(i<48){
	     name = "layer6";
	   }
	   ATH_MSG_DEBUG ( "Scint Name: " << m_scintNames[i] );

	   std::string hname = m_path;
	   std::string htitle = "TailCatcher Scintillator ";

	   if(m_monitor_tailcatcher_raw) {
             ++iter;
	     hname += "/BeamDetectors/TailCatcher/";
	     hname += name;
	     hname += m_scintNames[i];

	     
	     htitle += m_scintNames[i];
	     htitle += " -- ADC count";
	     
	     m_histo_tcScintADC[i] = 
	       ToolHistoSvc()->book(hname,htitle,m_numBin,m_posMin,m_posMax);
	   }

	   if(m_monitor_tailcatcher_reco) {
	     ++iterreco;
	     hname = m_path;
	     hname += "/BeamDetectors/TailCatcher/";
	     hname += name;
	     hname += m_scintNames[i];
	     hname += "Reco";
	     htitle = "TailCatcher Scintillator ";
	     htitle += m_scintNames[i];
	     htitle += " -- pC";
	     m_histo_tcreco[i] = 
	       ToolHistoSvc()->book(hname,htitle,m_numBin,m_posMinreco,m_posMaxreco);
	     
	   }
	  
	 }
	 SetBookStatus(true);
       }
       
     
   } //End of tailcatcher histo

  ATH_MSG_DEBUG ( "histo path: " << m_path + "/BeamDetectors/" );

  ATH_MSG_DEBUG ( "Leaving mybookHists()" );

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TBTailCatcherRawMonTool::fillHists()
/*---------------------------------------------------------*/
{
  ATH_MSG_DEBUG ( "In fillHists()" );

  // Fill some tacking objects for testing
  if(m_testTool){
    FillRandomDetect();
  }
  
  if(this->histsNotBooked()) {
    ATH_CHECK ( this->mybookHists() );
  }

  // Tailcatcher monitor
  if(m_monitor_tailcatcher_raw){  
    TBTailCatcherRaw * tcRaw=NULL;
    StatusCode sc = evtStore()->retrieve(tcRaw);
    if (sc.isFailure()){
      ATH_MSG_DEBUG 
        ( "BeamDetectorMonitoring: Retrieval of TBTailCatcherRaw failed"  );
    } else {
      
      TBTailCatcherRaw::const_iterator it_tcScint = tcRaw->begin();
      
      // loop over list of scint to monitor :
      for(int nameind=0; nameind < m_tcScintNum; nameind++){
	
	// Check if tcScint is in sync with m_scintNames - defined in book hist
	if((*it_tcScint)->getDetectorName() != m_scintNames[nameind]){
	  ATH_MSG_ERROR ( "Monitoring Raw TC. Error: booking and filling out of sync." );
	  return StatusCode::FAILURE;
	}

	// now it_scint contains the right scint
	const TBScintillatorRaw * scint = (*it_tcScint);
	if(!scint->isADCOverflow()){	
	  m_histo_tcScintADC[nameind]->fill(scint->getADCSignal(),1.0);
	}
	++it_tcScint;
      }
    }
  }  // End of tailcatcher fill

  if(m_monitor_tailcatcher_reco){  
    TBTailCatcher * tcReco=NULL;
    StatusCode sc = evtStore()->retrieve(tcReco);
    if (sc.isFailure()){
      ATH_MSG_DEBUG 
        ( "BeamDetectorMonitoring: Retrieval of TBTailCatcher failed" );
    } else {
      
      TBTailCatcher::const_iterator it_tcScint = tcReco->begin();
      
      float sum=0;
      // loop over list of scint to monitor :
      for(int nameind=0; nameind < m_tcScintNum; nameind++){
	
	// Check if tcScint is in sync with m_scintNames - defined in book hist
	if((*it_tcScint)->getDetectorName() != m_scintNames[nameind]){
	  ATH_MSG_ERROR ( "Monitoring Reco TC. Error: booking and filling out of sync." );
	  return StatusCode::FAILURE;
	}

	// now it_scint contains the right scint
	const TBScintillator * scint = (*it_tcScint);
	if(!scint->isSignalOverflow()){	
	  m_histo_tcreco[nameind]->fill(scint->getSignal(),1.0);
	  sum+=scint->getSignal();
	}
	++it_tcScint;
      }
      m_histo_tcTotal->fill(sum,1.0);
    }
  }  // End of tailcatcher fill

return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
void TBTailCatcherRawMonTool::FillRandomDetect()
/*---------------------------------------------------------*/
{
}
