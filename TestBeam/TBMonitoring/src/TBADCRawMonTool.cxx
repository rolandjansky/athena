//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"

#include "TBMonitoring/TBADCRawMonTool.h"

#include "TBEvent/TBADCRaw.h"
#include "TBEvent/TBADCRawCont.h"

#include <vector>
#include <string>

/*---------------------------------------------------------*/
TBADCRawMonTool::TBADCRawMonTool(const std::string & type, 
				 const std::string & name,
				 const IInterface* parent)
  : MonitorToolBase(type, name, parent)
/*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);
  declareProperty("histoPathBase",m_path = "/stat"); 
  
  //ADC histogram properties
  declareProperty("ADCNames",m_adcNames);
  declareProperty("ADCMax",m_posMax=1024);
  declareProperty("ADCMin",m_posMin=0);
  declareProperty("ADCBinNum",m_numBin=1024);
  declareProperty("MonitorADC",m_monitor_adc=true);
  declareProperty("TestADCMonTool",m_testTool=false);
}

/*---------------------------------------------------------*/
TBADCRawMonTool::~TBADCRawMonTool()
/*---------------------------------------------------------*/
{}

/*---------------------------------------------------------*/
StatusCode TBADCRawMonTool:: initialize()
/*---------------------------------------------------------*/
{
  MsgStream log(msgSvc(), name());
  
  StatusCode sc;

  if(m_monitor_adc == false){
    log << MSG::INFO << name() << " Not monitoring adc " << endreq;
  }

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
StatusCode TBADCRawMonTool::bookHists()
/*---------------------------------------------------------*/
{
  // This is called by parent class in initialize().
  // We want to book histos according to 1st event so 
  // we use mybookHist()
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TBADCRawMonTool::mybookHists()
/*---------------------------------------------------------*/
{
 
  MsgStream log(msgSvc(), name());
  
#ifndef NDEBUG
  log << MSG::INFO << "in mybookHists()" << endreq;
#endif

  StatusCode sc;

  //Start of adc raw histos
   if(m_monitor_adc){

     m_adcNum = m_adcNames.size();
     m_histo_adc = new IHistogram1D*[m_adcNum];
     	 
     for(int i=0;i<m_adcNum;i++){
       std::string hname = m_path;
       hname += "/BeamDetectors/ADC/";
       hname += m_adcNames[i];
       
       std::string htitle = "ADC: ";
       htitle += m_adcNames[i];
       
       m_histo_adc[i] = 
       ToolHistoSvc()->book(hname,htitle,m_numBin,m_posMin,m_posMax);
     }
   }

  log << MSG::DEBUG << "histo path: " << m_path + "/BeamDetectors/" << endreq;

  log << MSG::INFO << " \t Monitoring ADCRaw \t  " ;
  if(m_monitor_adc) log << " \t : YES "<< endreq;
  else log << " \t  : NO "<< endreq;

  log << MSG::INFO << " \t testTool \t  " ;
  if(m_testTool) log << " \t \t : YES "<< endreq;
  else log << " \t \t : NO "<< endreq;
  
  SetBookStatus(true);
  
  log << MSG::DEBUG << "Leaving mybookHists()" << endreq;

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TBADCRawMonTool::fillHists()
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

  // ADC monitor
  if(m_monitor_adc){  
    TBADCRawCont * adcRawCont=NULL;
    sc = m_StoreGate->retrieve(adcRawCont);
    if (sc.isFailure()){
      log << MSG::DEBUG 
	  << "BeamDetectorMonitoring: Retrieval of TBADCRawCont failed" 
	  << endreq;
    } else {
      
      TBADCRawCont::const_iterator it_adc = adcRawCont->begin();
      TBADCRawCont::const_iterator last_adc = adcRawCont->end();
      
      // loop over list of scint to monitor :
      for(int nameind=0; nameind < m_adcNum; nameind++){      

	for(it_adc = adcRawCont->begin();it_adc!=last_adc;it_adc++){	
	// Check if adcRawCont is in sync with m_adcNames - defined in book hist
	  if((*it_adc)->getDetectorName() == m_adcNames[nameind]) break;
	}
	if(it_adc==last_adc){log<< MSG::DEBUG << "Did not found" << m_adcNames[nameind] <<endreq;continue;}
	

	// now it_scint contains the right scint
	const TBADCRaw * adc = (*it_adc);
	if(!adc->isOverflow())	m_histo_adc[nameind]->fill(adc->getADC(),1.0);
	it_adc++;
      }
    }
  }  // End of tailcatcher fill

return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
void TBADCRawMonTool::FillRandomDetect()
/*---------------------------------------------------------*/
{
  // Fake different beam detectors/data classes

  MsgStream log(msgSvc(), name());
  // Nothing here!! //
}
