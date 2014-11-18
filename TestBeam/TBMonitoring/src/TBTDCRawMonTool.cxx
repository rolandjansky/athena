//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/ISvcLocator.h"

#include "TBMonitoring/TBTDCRawMonTool.h"

#include "TBEvent/TBTDCRaw.h"
#include "TBEvent/TBTDCRawCont.h"

#include <vector>
#include <string>

/*---------------------------------------------------------*/
TBTDCRawMonTool::TBTDCRawMonTool(const std::string & type, 
				 const std::string & name,
				 const IInterface* parent)
  : MonitorToolBase(type, name, parent)
/*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);
  declareProperty("histoPathBase",m_path = "/stat"); 
  
  //TDC histogram properties
  declareProperty("TDCNames",m_tdcNames);
  declareProperty("TDCMax",m_posMax=1024);
  declareProperty("TDCMin",m_posMin=0);
  declareProperty("TDCBinNum",m_numBin=1024);
  declareProperty("MonitorTDC",m_monitor_tdc=true);
  declareProperty("TestTDCMonTool",m_testTool=false);
}

/*---------------------------------------------------------*/
TBTDCRawMonTool::~TBTDCRawMonTool()
/*---------------------------------------------------------*/
{}

/*---------------------------------------------------------*/
StatusCode TBTDCRawMonTool:: initialize()
/*---------------------------------------------------------*/
{
  if(m_monitor_tdc == false){
    ATH_MSG_INFO ( name() << " Not monitoring tdc " );
  }

  //set to true whitin bookHist() 
  m_isBooked = false;

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TBTDCRawMonTool::bookHists()
/*---------------------------------------------------------*/
{
  // This is called by parent class in initialize().
  // We want to book histos according to 1st event so 
  // we use mybookHist()
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TBTDCRawMonTool::mybookHists()
/*---------------------------------------------------------*/
{
  ATH_MSG_DEBUG ( "in mybookHists()" );

  //Start of tdc raw histos
   if(m_monitor_tdc){
     
     m_tdcNum = m_tdcNames.size();
     m_histo_tdc = new IHistogram1D*[m_tdcNum];
     
     for(int i=0;i<m_tdcNum;i++){
       std::string hname = m_path;
       hname += "/BeamDetectors/TDC/";
       hname += m_tdcNames[i];
       
       std::string htitle = "TDC: ";
       htitle += m_tdcNames[i];
       
       m_histo_tdc[i] = 
	 ToolHistoSvc()->book(hname,htitle,m_numBin,m_posMin,m_posMax);
     }
   }  //End of tdc histo

   ATH_MSG_DEBUG ( "histo path: " << m_path + "/BeamDetectors/" );

  SetBookStatus(true);
  
  ATH_MSG_DEBUG ( "Leaving mybookHists()" );

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TBTDCRawMonTool::fillHists()
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

  if(m_monitor_tdc){  
    TBTDCRawCont * tdcRawCont=NULL;
    StatusCode sc = evtStore()->retrieve(tdcRawCont);
    if (sc.isFailure()){
      ATH_MSG_DEBUG 
        ( "BeamDetectorMonitoring: Retrieval of TBTDCRawCont failed" );
    } else {
      
      TBTDCRawCont::const_iterator it_tdc = tdcRawCont->begin();
      TBTDCRawCont::const_iterator last_tdc = tdcRawCont->end();
      
      // loop over list of scint to monitor :
      for(int nameind=0; nameind < m_tdcNum; nameind++){      
	ATH_MSG_DEBUG ( " Looking for " << m_tdcNames[nameind] );
	for(it_tdc = tdcRawCont->begin();it_tdc!=last_tdc;it_tdc++){	
	// Check if tdcRawCont is in sync with m_tdcNames - defined in book hist
	  if((*it_tdc)->getDetectorName() == m_tdcNames[nameind]) break;
	}
	if(it_tdc==last_tdc) {
          ATH_MSG_DEBUG ( "Did not found" << m_tdcNames[nameind] );
          continue;
        }

	// now it_scint contains the right scint
	const TBTDCRaw * tdc = (*it_tdc);
	
	if(!tdc->isOverflow()) m_histo_tdc[nameind]->fill(tdc->getTDC(),1.0);
	it_tdc++;
      }
    }
  }
  ATH_MSG_DEBUG ( " fillHists() ended" );
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
void TBTDCRawMonTool::FillRandomDetect()
/*---------------------------------------------------------*/
{
}
