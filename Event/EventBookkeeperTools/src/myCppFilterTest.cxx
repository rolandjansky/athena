/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// Creation: Joao Costa July 2010 <joao.firmino.da.costa@desy.de> and inspired from alg from David Cote, September 2008. <david.cote@cern.ch> 
/////////////////////////////////////////////////////////////////// 
#include "EventBookkeeperTools/myCppFilterTest.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "StoreGate/StoreGateSvc.h"
#include <TTree.h>
#include <TFile.h>
#include <TROOT.h>
#include <vector>

myCppFilterTest::myCppFilterTest(const std::string& name, ISvcLocator* pSvcLocator) :
  // *****************************************************
  AthFilterAlgorithm(name, pSvcLocator),
  m_cutflowSvc("CutFlowSvc/CutFlowSvc",name),
  m_cut1(0),
  m_cut2(0),
  m_cycle(-1)
{
  declareProperty("EventBookkeeperCollectionName",m_EvtBookCollName = "EventBookkeepers");
  declareProperty("StreamName",m_InputStreamName = "InputStreamName");
  declareProperty("counter",m_counter = 0);
  declareProperty("cutfSvc", m_cutflowSvc);
  declareProperty("cut1", m_cut1 = 1.);
  declareProperty("cut2", m_cut2 = 0.);

  m_counter=0;
}

myCppFilterTest::~myCppFilterTest() {

}

StatusCode myCppFilterTest::initialize(){
  //*******************************************************

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "initialize()" << endreq;


  
                                                                                
  // Locate the StoreGateSvc and initialize our local ptr
  // ****************************************************
  // Get the ICutFlowSvc
  StatusCode sc = m_cutflowSvc.retrieve();
  if (sc.isFailure()) {
     log << MSG::ERROR << "Cannot get ICutFlowSvc interface." << endreq;
     return(sc);
   } else {
     log << MSG::INFO << "Found ICutFlowSvc." << endreq;
   }
 
  // Get filter EB  and only then register the cuts
  filterCutID = this->cutID();
  cut1ID =  m_cutflowSvc->registerCut("cut1","First cut",filterCutID);
  cut1ID =  m_cutflowSvc->registerCut("cut2","Second cut",filterCutID);


  

  return StatusCode::SUCCESS;
}


StatusCode myCppFilterTest::execute() {
  MsgStream log(msgSvc(), name());



  // Here you do your analysis with the event selection

  m_cutflowSvc->addEvent(cut1ID);
  m_cutflowSvc->addEvent(cut1ID);
  m_cutflowSvc->addEvent(filterCutID);




  return (StatusCode::SUCCESS);
}

//*****************************************************
StatusCode myCppFilterTest::finalize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "finalize()" << endreq;






  return StatusCode::SUCCESS;
}
 

