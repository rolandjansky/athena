/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "GoodRunsListsUser/DummyDumperAlg.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "GaudiKernel/ITHistSvc.h"

#include <TList.h>
#include "TTree.h"

DummyDumperAlg::DummyDumperAlg(const std::string& name, ISvcLocator* pSvcLocator) 
 : AthAlgorithm(name, pSvcLocator)
 , m_tree(0)
 , m_eventCounter(0)
 , m_dummyInt(0)
{
  declareProperty( "GRLNameVec", m_grlname );
}


DummyDumperAlg::~DummyDumperAlg()
{
  /// For some reason deleting the tree results in a crash.
  /// Double deletion somewhere?
  //if (m_tree!=0) { delete m_tree; m_tree=0; }
}


StatusCode DummyDumperAlg::initialize() 
{
  ATH_MSG_DEBUG ("initialize()");

  MsgStream log (msgSvc(), name());

  if (service("THistSvc",m_ths).isFailure())
  {
    log << MSG::ERROR << "Couldn't get THistSvc" << endmsg;
    return StatusCode::FAILURE;
  }

  // make tree
  m_tree = new TTree("tree","tree");
  if (m_ths->regTree (m_tree->GetName(), m_tree).isFailure())
  {
    log << MSG::ERROR << "Couldn't register " << m_tree->GetName() << endmsg;
    return StatusCode::FAILURE;
  }

  (void) m_tree->Branch("eventNumber",&m_dummyInt,"eventNumber/I");
  (void) m_tree->Branch("runNumber",&m_dummyInt,"runNumber/I");
  (void) m_tree->Branch("lumiBlock",&m_dummyInt,"lumiBlock/I");
  (void) m_tree->Branch("timeStamp",&m_dummyInt,"timeStamp/I");

  ATH_MSG_DEBUG ("initialize() successful");

  return StatusCode::SUCCESS;
}


StatusCode 
DummyDumperAlg::execute() 
{
  ATH_MSG_DEBUG ("execute()");

  m_eventCounter++;

  // get event number here ...
  int eventNumber(0);
  int runNumber(0);
  int lumiBlock(0);
  int timeStamp(0);

  // retrieve event info
  const EventInfo*  p_evt = 0;
  StatusCode status = evtStore()->retrieve(p_evt);

  if(status.isSuccess() && p_evt!=0) {
    eventNumber = p_evt->event_ID()->event_number();
    runNumber = p_evt->event_ID()->run_number();
    lumiBlock = p_evt->event_ID()->lumi_block();
    timeStamp = p_evt->event_ID()->time_stamp();
    ATH_MSG_DEBUG ("Got run number = " << runNumber << ", event number = " << eventNumber);
  } else {
    ATH_MSG_WARNING ("Unable to retrieve EventInfo from StoreGate. Return failure.");
    return status;
  }

  // fill the dummy tree
  m_tree->SetBranchAddress( "eventNumber", &eventNumber ); 
  m_tree->SetBranchAddress( "runNumber", &runNumber ); 
  m_tree->SetBranchAddress( "lumiBlock", &lumiBlock ); 
  m_tree->SetBranchAddress( "timeStamp", &timeStamp ); 

  m_tree->Fill();

  ATH_MSG_DEBUG ("execute() successful");

  return StatusCode::SUCCESS;
}


StatusCode DummyDumperAlg::finalize() 
{
  ATH_MSG_DEBUG ("finalize()");
  return StatusCode::SUCCESS;
}


