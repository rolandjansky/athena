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
  MsgStream log(msgSvc(), name());
  
  log << MSG::DEBUG
      << "initialize"
      << endreq;
  
  StatusCode sc = service( "StoreGateSvc", m_StoreGate);
  if( sc.isFailure() ) {
    log << MSG::FATAL << name()
	<< ": Unable to locate Service StoreGateSvc"
	<< endreq;
    return sc;
  }
  // set to true in book hist
  m_isBooked = false;
  
  return StatusCode::SUCCESS;
}//init

StatusCode TBBeamQualityDoubleHitsMonTool::bookHists()
{ //book
  
  MsgStream log(msgSvc(), name());
  
  log << MSG::DEBUG
      << "booking histograms"
      << endreq;
  
  if(!m_isBooked) {
    
    log << MSG::DEBUG << "in bookHists()" << endreq;
    log << MSG::DEBUG << "Base path:" << m_path << endreq;
    log << MSG::DEBUG << "Histo path:" << m_histoPath << endreq;
    log << MSG::DEBUG << "Full path: " <<   m_path+m_histoPath << endreq;
    
    // number of scintillators selected
    m_scintnum = m_scintselect.size();
    
    log << MSG::DEBUG
	<< " Number of Selected Scintillators: "
	<< m_scintnum
	<< endreq;
    
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
    log << MSG::DEBUG
	<< "Booked Histograms"
	<< endreq;
  }   
  return StatusCode::SUCCESS;  
} //book

StatusCode TBBeamQualityDoubleHitsMonTool::fillHists()
{ //fill
  
  MsgStream log(msgSvc(),name());
  
  log << MSG::DEBUG
      << "TBBeamQualityDoubleHitsMonTool: Started fillHists"
      << endreq;
  
  // checking out StoreGateSvc
  StatusCode sc = service("StoreGateSvc",m_StoreGate);
  if ( sc.isFailure() )
    {
      log << MSG::ERROR
          << "Cannot alllocate StoreGate service!"
          << endreq;
    }
  
  TBScintillatorRawCont * scint_raw;
  
  sc = m_StoreGate->retrieve(scint_raw, m_SGScintRawKey);
  
  if (sc.isFailure()){
    log << MSG::DEBUG
	<< "TBBeamQualityDoubleHitsMonTool: Can't Retrieve "
	<< m_SGScintRawKey
	<<" fro SG"
	<< endreq;
    
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
  
  log << MSG::DEBUG
      << "Filled Histograms"
      << endreq;
  
  return StatusCode::SUCCESS;
} //fill



