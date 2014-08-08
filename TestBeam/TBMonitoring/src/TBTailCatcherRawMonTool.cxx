//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/MsgStream.h"
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
  : MonitorToolBase(type, name, parent)
/*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);
  declareProperty("histoPathBase",m_path = "/stat"); 
  
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
  MsgStream log(msgSvc(), name());
  
  StatusCode sc;


  sc = service( "StoreGateSvc", m_StoreGate);
  if( sc.isFailure() ) {
    log << MSG::FATAL << name() 
	<< ": Unable to locate Service StoreGateSvc" 
	<< endreq;
    return sc;
  }

  //set to true whitin bookHist() 
  m_isBooked = false;

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
 
  MsgStream log(msgSvc(), name());
  
#ifndef NDEBUG
  log << MSG::INFO << "in mybookHists()" << endreq;
#endif

  StatusCode sc;

  //Start of tailcatcher raw histos
  TBTailCatcherRaw * tcRaw=NULL;
  TBTailCatcher * tcReco=NULL;
  if(m_monitor_tailcatcher_raw){
    //Retrieve tailcatcher raw 
    sc = m_StoreGate->retrieve(tcRaw);
    
    if (sc.isFailure()){
      log << MSG::INFO 
	  << "TBTailCatcherMonTool: Retrieval of TailCatcher Raw failed" 
	  << endreq;
      m_monitor_tailcatcher_raw=false;
    }else m_tcScintNum = tcRaw->size(); //Get number of scintillators with the tailcatcher
  }
  if(m_monitor_tailcatcher_reco){
    //Retrieve tailcatcher raw 
    sc = m_StoreGate->retrieve(tcReco);
    
    if (sc.isFailure()){
      log << MSG::INFO 
	  << "TBTailCatcherMonTool: Retrieval of TailCatcher failed" 
	  << endreq;
      m_monitor_tailcatcher_reco=false;
    }else m_tcScintNum = tcRaw->size(); //Get number of scintillators with the tailcatcher
  }




  if(m_monitor_tailcatcher_reco|m_monitor_tailcatcher_raw){
       
       log << MSG::DEBUG << "Number od Scintillator in tailcatcher: "
	   << m_tcScintNum
	   << endreq;
       
       if(m_tcScintNum == 0){
	 log << MSG::INFO 
	     << "!! Warning !! "
	     << "There is no scintillator recorded in the tailcather."
	     << endreq;
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
	   log << MSG::DEBUG << "Number of scintillators in book loop: "
	       << m_tcScintNum
	       << endreq;
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
	   log << MSG::DEBUG << "Scint Name: " 
	       << m_scintNames[i]
	       << endreq;

	   std::string hname = m_path;
	   std::string htitle = "TailCatcher Scintillator ";

	   if(m_monitor_tailcatcher_raw) {
	     iter++;
	     hname += "/BeamDetectors/TailCatcher/";
	     hname += name;
	     hname += m_scintNames[i];

	     
	     htitle += m_scintNames[i];
	     htitle += " -- ADC count";
	     
	     m_histo_tcScintADC[i] = 
	       ToolHistoSvc()->book(hname,htitle,m_numBin,m_posMin,m_posMax);
	   }

	   if(m_monitor_tailcatcher_reco) {
	     iterreco++;
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

  log << MSG::DEBUG << "histo path: " << m_path + "/BeamDetectors/" << endreq;

  log << MSG::INFO << " \t Monitoring TailCatcherRaw   " ;
  if(m_monitor_tailcatcher_raw) log << " \t : YES "<< endreq;
  else log << " \t : NO "<< endreq;
  log << MSG::INFO << " \t Monitoring TailCatcherReco   " ;
  if(m_monitor_tailcatcher_reco) log << " \t : YES "<< endreq;
  else log << " \t : NO "<< endreq;

  log << MSG::INFO << " \t testTool \t \t  " ;
  if(m_testTool) log << " \t : YES "<< endreq;
  else log << " \t : NO "<< endreq;
  

  
  log << MSG::DEBUG << "Leaving mybookHists()" << endreq;

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TBTailCatcherRawMonTool::fillHists()
/*---------------------------------------------------------*/
{
 
  MsgStream log(msgSvc(), name());

  log << MSG::DEBUG << "In fillHists()" << endreq;

  // Fill some tacking objects for testing
  if(m_testTool){
    FillRandomDetect();
  }
  
  if(this->histsNotBooked()) {
    this->mybookHists();
  }

  StatusCode sc;

  // Tailcatcher monitor
  if(m_monitor_tailcatcher_raw){  
    TBTailCatcherRaw * tcRaw=NULL;
    sc = m_StoreGate->retrieve(tcRaw);
    if (sc.isFailure()){
      log << MSG::DEBUG 
	  << "BeamDetectorMonitoring: Retrieval of TBTailCatcherRaw failed" 
	  << endreq;
    } else {
      
      TBTailCatcherRaw::const_iterator it_tcScint = tcRaw->begin();
      
      // loop over list of scint to monitor :
      for(int nameind=0; nameind < m_tcScintNum; nameind++){
	
	// Check if tcScint is in sync with m_scintNames - defined in book hist
	if((*it_tcScint)->getDetectorName() != m_scintNames[nameind]){
	  log << MSG::ERROR << "Monitoring Raw TC. Error: booking and filling out of sync."
	      << endreq;
	  return StatusCode::FAILURE;
	}

	// now it_scint contains the right scint
	const TBScintillatorRaw * scint = (*it_tcScint);
	if(!scint->isADCOverflow()){	
	  m_histo_tcScintADC[nameind]->fill(scint->getADCSignal(),1.0);
	}
	it_tcScint++;
      }
    }
  }  // End of tailcatcher fill

  if(m_monitor_tailcatcher_reco){  
    TBTailCatcher * tcReco=NULL;
    sc = m_StoreGate->retrieve(tcReco);
    if (sc.isFailure()){
      log << MSG::DEBUG 
	  << "BeamDetectorMonitoring: Retrieval of TBTailCatcher failed" 
	  << endreq;
    } else {
      
      TBTailCatcher::const_iterator it_tcScint = tcReco->begin();
      
      float sum=0;
      // loop over list of scint to monitor :
      for(int nameind=0; nameind < m_tcScintNum; nameind++){
	
	// Check if tcScint is in sync with m_scintNames - defined in book hist
	if((*it_tcScint)->getDetectorName() != m_scintNames[nameind]){
	  log << MSG::ERROR << "Monitoring Reco TC. Error: booking and filling out of sync."
	      << endreq;
	  return StatusCode::FAILURE;
	}

	// now it_scint contains the right scint
	const TBScintillator * scint = (*it_tcScint);
	if(!scint->isSignalOverflow()){	
	  m_histo_tcreco[nameind]->fill(scint->getSignal(),1.0);
	  sum+=scint->getSignal();
	}
	it_tcScint++;
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
  // Fake different beam detectors/data classes

  MsgStream log(msgSvc(), name());
  // Nothing here!! //
}
