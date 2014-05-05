/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PyAnalysisExamples/MySelectionAlg.h"
#include "PyAnalysisExamples/MyCutClass.h"
#include "PyAnalysisExamples/MyDetObj.h"
#include "PyAnalysisExamples/MyTool.h"
#include "PyAnalysisExamples/MyObj.h"

#include "StoreGate/StoreGateSvc.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "egammaEvent/ElectronContainer.h"


using CLHEP::GeV;
using namespace AthPyEx;


MySelectionAlg::MySelectionAlg (const std::string& name, ISvcLocator* pSvcLocator)
  : Algorithm(name, pSvcLocator),
    m_StoreGate(0),
    m_ThrPT(10*GeV),
    m_NewKey("MyNewContainer"),
    m_ContainerKey("ElectronCollection"),
    m_cut(0),
    m_myTool(0)
{
   declareProperty("ThrPT",        m_ThrPT);
   declareProperty("NewKey",       m_NewKey);
   declareProperty("ContainerKey", m_ContainerKey);
}

// Initialize
StatusCode MySelectionAlg::initialize()
{
  MsgStream log( messageService(), name() );
  log << MSG::INFO << "initialize" << endreq;
  
  // get pointer to StoreGate
  StatusCode sc = service("StoreGateSvc", m_StoreGate);
  if (sc.isFailure())
    {
      log << MSG::ERROR << "Unable to retrieve pointer to StoreGateSvc" << endreq;
      return sc;
    }

  // instantiate cut class
  m_cut = new MyCutClass (m_ThrPT);

  // get pointer to DetectorStore
  StoreGateSvc* detStore;
  sc = service("DetectorStore", detStore);
  if (sc.isFailure())
    {
      log << MSG::ERROR << "Unable to get pointer to DetectorStore" << endreq;
      return sc;
    }

  // record an object in DetectorStore
  MyDetObj *detObj = new MyDetObj ("MyDetObj0");
  sc = detStore->record(detObj, "myDetObj", false);

  /// get a handle on MyTool
  IToolSvc* toolSvc;
  sc = service("ToolSvc",toolSvc);
  if (sc.isFailure())
    {
      log << MSG::ERROR << " Can't get ToolSvc" << endreq;
      return StatusCode::FAILURE;
    }

  IAlgTool *tmp_myTool = 0;
  sc = toolSvc->retrieveTool("AthPyEx::MyTool",tmp_myTool);
  m_myTool=dynamic_cast<AthPyEx::MyTool *>(tmp_myTool);
  if (sc.isFailure() || !m_myTool)
    {
      log << MSG::ERROR << "Can't get handle on analysis tools" << endreq;
      return StatusCode::FAILURE;
    }

  m_myTool->setA(100);

  return StatusCode::SUCCESS;
}


// finalize
StatusCode MySelectionAlg::finalize()
{
  delete m_cut;
  return StatusCode::SUCCESS;
}


// execute
StatusCode MySelectionAlg::execute()
{
  MsgStream log( messageService(), name() );
  log << MSG::DEBUG << "execute" << endreq;

  log << MSG::INFO << "MyTool: " << m_myTool->getA() << endreq;

  MyObj *obj = new MyObj("aaa");
  obj->setA(123);
  log << MSG::INFO << "before - MyObj: " << obj->getA() << endreq;
  StatusCode sc = m_myTool->execute(obj);
  log << MSG::INFO << "after  - MyObj: " << obj->getA() << endreq;
  delete obj;

  if (sc.isFailure())
    {
      log << MSG::ERROR << "MyTool::execute() failed" << endreq;
      return StatusCode::SUCCESS;
    }

  // retrive ElectronContainer
  log << MSG::DEBUG << "retrive ElectronContainer" << endreq;
  const ElectronContainer *eCon;
  sc = m_StoreGate->retrieve(eCon, m_ContainerKey);
  if (sc.isFailure())
    {
      log << MSG::ERROR << "Could not retrieve ElectronContainer" << endreq;
      return StatusCode::SUCCESS;
    }

  ElectronContainer *newCon = new ElectronContainer(SG::VIEW_ELEMENTS);

  ElectronContainer::const_iterator it  = eCon->begin();
  ElectronContainer::const_iterator itE = eCon->end();
  for (; it!=itE; ++it)
    if (m_cut->isAccepted(**it)) newCon->push_back(*it);
  
  log << MSG::DEBUG << "record new ElectronContainer : size=" << newCon->size() << endreq;
  sc = m_StoreGate->record(newCon, m_NewKey, false);
  if (sc.isFailure())
    {
      log << MSG::ERROR << "Could not record new Container" << endreq;
      return sc;
    }
				  
  return StatusCode::SUCCESS;
}
