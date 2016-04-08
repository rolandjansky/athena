/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//#####################################################
//#                                                   #
//# File   : TBBeamQualityDoubleHitsMonTool.cxx       #
//# Package: TBMonitoring                             #
//#                                                   #
//# Author : Marco Bieri <mbieri@sfu.ca>              #
//#                                                   #
//# Last Moified: Nov 18/2004                         #
//#####################################################

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"

#include "TBEvent/TBScintillatorRawCont.h"
#include "TBEvent/TBScintillatorRaw.h"

#include "TBMonitoring/TBBeamQualityDoubleHitsMonTool.h"

#include <iostream>
#include <vector>
#include <string>

TBBeamQualityDoubleHitsMonTool::TBBeamQualityDoubleHitsMonTool(const std::string& type,
							       const std::string& name,
							       const IInterface* parent)
  : MonitorToolBase(type, name, parent)
{
  declareInterface<IMonitorToolBase>(this);
  // before/after BQ cuts
  declareProperty("FillTime",m_time="after");
  declareProperty("histoPathBase",m_path = "/stat");
  declareProperty("histoPath",m_histoPath="/BeamQuality/DoubleHits/");
  declareProperty("ScintMax",m_scintadcmax=1024);
  declareProperty("ScintMin",m_scintadcmin=0);
  declareProperty("ScintBin",m_scintbin=100);
  
  // 1st one is main scintillator
  declareProperty("ScintSelect",m_scintselect);
  
  m_histo_double=NULL; 
}

TBBeamQualityDoubleHitsMonTool::~TBBeamQualityDoubleHitsMonTool()
{
  if( m_histo_double )
    delete[]  m_histo_double;
}

StatusCode TBBeamQualityDoubleHitsMonTool::initialize()
{//init
  ATH_MSG_DEBUG ( "initialize" );
  
  // set to true in book hist
  m_isBooked = false;
  
  return StatusCode::SUCCESS;
}//init

StatusCode TBBeamQualityDoubleHitsMonTool::bookHists()
{ //book
  ATH_MSG_DEBUG ( "booking histograms" );
  
  if(!m_isBooked) {
    
    ATH_MSG_DEBUG ( "in bookHists()" );
    ATH_MSG_DEBUG ( "Base path:" << m_path );
    ATH_MSG_DEBUG ( "Histo path:" << m_histoPath );
    ATH_MSG_DEBUG ( "Full path: " <<   m_path+m_histoPath );
    
    // number of scintillators selected
    m_scintnum = m_scintselect.size();
    
    ATH_MSG_DEBUG ( " Number of Selected Scintillators: " << m_scintnum );
    
    std::string path = m_path+m_histoPath;
    
    m_histo_double = new IHistogram2D*[m_scintnum];
    
    // start at 1 since 0 is the main scint
    for(int isel=1;isel<m_scintnum;isel++){
      
      if (m_time == "before" ){
	htitle=m_scintselect[isel];
	htitle+=" vs ";
	htitle+=m_scintselect[0];
	htitle+=" before BQ cut";
	
	hname = path;
	hname+=m_scintselect[isel];
	hname+="vs";
	hname+=m_scintselect[0];
	hname+="beforeBQcut";
	
	m_histo_double[isel] = ToolHistoSvc()->book(hname,htitle,m_scintbin,m_scintadcmin,m_scintadcmax,m_scintbin,m_scintadcmin,m_scintadcmax);      
      }      
      
      if (m_time == "after" ){
	htitle=m_scintselect[isel];
        htitle+=" vs ";
        htitle+=m_scintselect[0];
        htitle+=" afterBQcut ";
	
	std::string hname = path;
	hname+=m_scintselect[isel];
	hname+="vs";
	hname+=m_scintselect[0]; 
	hname+="afterBQcut";
	
	m_histo_double[isel] = ToolHistoSvc()->book(hname,htitle,m_scintbin,m_scintadcmin,m_scintadcmax,m_scintbin,m_scintadcmin,m_scintadcmax);
      }
      m_isBooked=true;
    }
    ATH_MSG_DEBUG ( "Booked Histograms" );
  }   
  return StatusCode::SUCCESS;  
} //book

StatusCode TBBeamQualityDoubleHitsMonTool::fillHists()
{ //fill
  ATH_MSG_DEBUG ( "TBBeamQualityDoubleHitsMonTool: Started fillHists" );
  
  TBScintillatorRawCont * scint_raw = nullptr;
  StatusCode sc = evtStore()->retrieve(scint_raw, m_SGScintRawKey);
  
  if (sc.isFailure()){
    ATH_MSG_DEBUG
      ( "TBBeamQualityDoubleHitsMonTool: Can't Retrieve "
	<< m_SGScintRawKey
	<<" from SG" );
    
  }else { //else
    
    TBScintillatorRawCont::const_iterator it_scint_raw   = scint_raw->begin();
    TBScintillatorRawCont::const_iterator last_scint_raw   = scint_raw->end();
    
    for(;it_scint_raw!=last_scint_raw;it_scint_raw++) {
      
      const TBScintillatorRaw * scint_raw = (*it_scint_raw);
      
      for (unsigned int ifind=0 ; ifind<m_scintselect.size();ifind++) {
	
	if(scint_raw->getDetectorName()==m_scintselect[ifind]){//ifname
	  
	  m_scintselectadc[ifind] = scint_raw->getADCSignal();
	  
	  ifind = m_scintselect.size();
	}//ifname
      }
    }     
    
    for( unsigned int ifill=1; ifill<m_scintselect.size(); ifill++) { //fill
      
      m_histo_double[ifill]->fill(m_scintselectadc[0],m_scintselectadc[ifill],1);
      
    } //fill
  }// else
  
  ATH_MSG_DEBUG ( "Filled Histograms" );
  
  return StatusCode::SUCCESS;
} //fill



