/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TBTriggerPatternUnitMonTool.cxx
// PACKAGE:  TBMonitoring
//
// AUTHOR:   Warren Shaw
//
//
// ********************************************************************

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"

#include "TBMonitoring/TBTriggerPatternUnitMonTool.h"

#include "TBEvent/TBTriggerPatternUnit.h"

#include <vector>


/*---------------------------------------------------------*/
TBTriggerPatternUnitMonTool::TBTriggerPatternUnitMonTool(const std::string & type, 
				 const std::string & name,
				 const IInterface* parent)
  : MonitorToolBase(type, name, parent)
/*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);
  declareProperty("histoPathBase",m_path = "/stat");

  declareProperty("MonitorTriggPat",m_monitor_triggpat=true);

  declareProperty("FakeDetectors",m_fake_detector=false);

  declareProperty("TriggFlags",m_triggflag);

  declareProperty("SGkey",m_SGkey="TBTrigPat");

}

/*---------------------------------------------------------*/
TBTriggerPatternUnitMonTool::~TBTriggerPatternUnitMonTool()
/*---------------------------------------------------------*/
{
}

/*---------------------------------------------------------*/
StatusCode TBTriggerPatternUnitMonTool:: initialize()
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

  if(m_triggflag.size()==0){
    m_triggflag.push_back(0x00000001);
    m_triggflag.push_back(0x00000002);
    m_triggflag.push_back(0x00000004);
    m_triggflag.push_back(0x00000008);
    m_triggflag.push_back(0x00000010);
    m_triggflag.push_back(0x00000020);
    m_triggflag.push_back(0x00000040);
    m_triggflag.push_back(0x00000080);
    m_triggflag.push_back(0x00000800);
    m_triggflag.push_back(0x00001000);
    m_triggflag.push_back(0x00002000);
    m_triggflag.push_back(0x00004000);
    m_triggflag.push_back(0x00008000);
  }

  //set to true whitin bookHist() 
  m_isBooked = false;

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TBTriggerPatternUnitMonTool::bookHists()
/*---------------------------------------------------------*/
{
  // This is called by parent class in initialize().
  // We want to book histos according to 1st event so 
  // we use mybookHist()
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TBTriggerPatternUnitMonTool::mybookHists()
/*---------------------------------------------------------*/
{
 
  MsgStream log(msgSvc(), name());

#ifndef NDEBUG
  log << MSG::INFO << "in mybookHists()" << endreq;
#endif

  //StatusCode sc;

   // Trigger Pattern  histos ----------------------------------
   if(m_monitor_triggpat){
     m_histo_triggpat1   = ToolHistoSvc()->book(m_path+"/BeamDetectors/triggpat1","Trigger Pattern",m_triggflag.size(),0,m_triggflag.size());
     m_histo_triggpat2   = ToolHistoSvc()->book(m_path+"/BeamDetectors/triggpat2","Trigger Pattern",m_triggflag.size(),0,m_triggflag.size(),m_triggflag.size(),0,m_triggflag.size());
     
   }

  log << MSG::DEBUG << "histo path: " << m_path + "/BeamDetectors/1" << endreq;

  log << MSG::INFO << " Monitoring Trigger Pattern" ;
  if(m_monitor_triggpat) log << " \t : YES "<< endreq;
  else log << " \t : NO "<< endreq;

  log << MSG::INFO << " Fake Detectors  \t  " ;
  if(m_fake_detector) log << " \t : YES "<< endreq;
  else log << " \t : NO "<< endreq;
  
  SetBookStatus(true);

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TBTriggerPatternUnitMonTool::fillHists()
/*---------------------------------------------------------*/
{
 
  MsgStream log(msgSvc(), name());

#ifndef NDEBUG
  log << MSG::DEBUG << "in fillHists()" << endreq;
#endif

  // Fill some bpc and stuff (testing) 
  if(m_fake_detector) FillRandomDetect();

  if(this->histsNotBooked()) {
    this->mybookHists();
  }

  StatusCode sc;

  // Trigger Pattern monitor ------------------------------------------------------
  if(m_monitor_triggpat){  
    TBTriggerPatternUnit * triggpat_object;
   
    sc = m_StoreGate->retrieve(triggpat_object, m_SGkey);
    if (sc.isFailure()){
      log << MSG::INFO 
	  << "TBTriggerPatternUnitMonitoring: Retrieval of Trigger Pattern failed" 
	  << endreq;
      return sc;
    }else {
      unsigned int word =triggpat_object->getTriggerWord();
      for(unsigned int i=0;i<m_triggflag.size();i++){
	bool trigg=((word & m_triggflag[i])!=0);
	if(trigg){
	  m_histo_triggpat1->fill(i,trigg);
	  for(unsigned int j=i+1;j<m_triggflag.size();j++){
	    if(trigg &&((word & m_triggflag[j])!=0)){
	      m_histo_triggpat2->fill(i,j);
	    }
	  }
	}
      }
    }
  } // Trigg Pattern

  log << MSG::DEBUG << "fillHists() ended" << endreq;
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
void TBTriggerPatternUnitMonTool::FillRandomDetect()
/*---------------------------------------------------------*/
{
  // Fake different beam detectors/data classes

  MsgStream log(msgSvc(), name());

  StatusCode sc;

  // Trigg Pattern -----------------------------------------
  TBTriggerPatternUnit *triggpat = new TBTriggerPatternUnit();
  // set first, third and fifth bit on :
  unsigned int word = 0x00000001;
  word = word | 0x00000004;
  word = word | 0x00000010;
  triggpat->setTriggerWord(word);

  sc = m_StoreGate->record(triggpat,m_SGkey);
  if ( sc.isFailure( ) ) {
    log << MSG::FATAL 
	<< "Cannot record TriggPat" 
	<< endreq;
  }

}
