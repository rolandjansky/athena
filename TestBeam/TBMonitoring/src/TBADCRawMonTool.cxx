//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


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
  if(m_monitor_adc == false){
    ATH_MSG_INFO ( name() << " Not monitoring adc " );
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
  ATH_MSG_DEBUG ( "in mybookHists()" );

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

   ATH_MSG_DEBUG ( "histo path: " << m_path + "/BeamDetectors/" );

  SetBookStatus(true);
  
  ATH_MSG_DEBUG ( "Leaving mybookHists()" );

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TBADCRawMonTool::fillHists()
/*---------------------------------------------------------*/
{
  ATH_MSG_DEBUG ( "In fillHists()" );

  // Fill some tacking objects for testing
  if(m_testTool){
    FillRandomDetect();
  }
  
  if(this->histsNotBooked()) {
    this->mybookHists();
  }

  // ADC monitor
  if(m_monitor_adc){  
    TBADCRawCont * adcRawCont=NULL;
    StatusCode sc = evtStore()->retrieve(adcRawCont);
    if (sc.isFailure()){
      ATH_MSG_DEBUG 
        ( "BeamDetectorMonitoring: Retrieval of TBADCRawCont failed" );
    } else {
      
      TBADCRawCont::const_iterator it_adc = adcRawCont->begin();
      TBADCRawCont::const_iterator last_adc = adcRawCont->end();
      
      // loop over list of scint to monitor :
      for(int nameind=0; nameind < m_adcNum; nameind++){      

	for(it_adc = adcRawCont->begin();it_adc!=last_adc;it_adc++){	
	// Check if adcRawCont is in sync with m_adcNames - defined in book hist
	  if((*it_adc)->getDetectorName() == m_adcNames[nameind]) break;
	}
	if(it_adc==last_adc){
          ATH_MSG_DEBUG ( "Did not found" << m_adcNames[nameind] );
          continue;
        }

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
}
