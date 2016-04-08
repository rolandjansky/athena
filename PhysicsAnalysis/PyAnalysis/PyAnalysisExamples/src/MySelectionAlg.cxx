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
  : AthAlgorithm(name, pSvcLocator),
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
  ATH_MSG_INFO ( "initialize" );
  
  // instantiate cut class
  m_cut = new MyCutClass (m_ThrPT);

  // record an object in DetectorStore
  MyDetObj *detObj = new MyDetObj ("MyDetObj0");
  ATH_CHECK( detStore()->record(detObj, "myDetObj", false) );

  IAlgTool *tmp_myTool = nullptr;
  ATH_CHECK( toolSvc()->retrieveTool("AthPyEx::MyTool",tmp_myTool) );
  m_myTool=dynamic_cast<AthPyEx::MyTool *>(tmp_myTool);
  if (!m_myTool)
    {
      ATH_MSG_ERROR ( "Can't get handle on analysis tools" );
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
  ATH_MSG_DEBUG ( "execute" );
  ATH_MSG_INFO ( "MyTool: " << m_myTool->getA() );

  MyObj *obj = new MyObj("aaa");
  obj->setA(123);
  ATH_MSG_INFO ( "before - MyObj: " << obj->getA() );
  ATH_CHECK( m_myTool->execute(obj) );
  ATH_MSG_INFO ( "after  - MyObj: " << obj->getA() );
  delete obj;

  // retrive ElectronContainer
  ATH_MSG_DEBUG ( "retrive ElectronContainer" );
  const ElectronContainer *eCon;
  ATH_CHECK( evtStore()->retrieve(eCon, m_ContainerKey) );

  ElectronContainer *newCon = new ElectronContainer(SG::VIEW_ELEMENTS);

  ElectronContainer::const_iterator it  = eCon->begin();
  ElectronContainer::const_iterator itE = eCon->end();
  for (; it!=itE; ++it)
    if (m_cut->isAccepted(**it)) newCon->push_back(*it);
  
  ATH_MSG_DEBUG ( "record new ElectronContainer : size=" << newCon->size() );
  ATH_CHECK( evtStore()->record(newCon, m_NewKey, false) );
  return StatusCode::SUCCESS;
}
