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
  m_filterCutID(),
  m_cut1ID(),
  m_cut2ID(),
  m_cut1(0),
  m_cut2(0)
{
  declareProperty("counter",m_counter = 0);
  declareProperty("cut1", m_cut1 = 1.);
  declareProperty("cut2", m_cut2 = 0.);

  m_counter=0;
}


myCppFilterTest::~myCppFilterTest() {

}


StatusCode myCppFilterTest::initialize(){
  ATH_MSG_DEBUG("initialize()");

  // Get filter EB  and only then register the cuts
  m_filterCutID = this->cutID();
  m_cut1ID      = cutFlowSvc()->registerCut("cut1","First cut",m_filterCutID);
  m_cut2ID      = cutFlowSvc()->registerCut("cut2","Second cut",m_filterCutID);

  return StatusCode::SUCCESS;
}


StatusCode myCppFilterTest::execute() {
  // Here you do your analysis with the event selection

  cutFlowSvc()->addEvent(m_cut1ID);
  cutFlowSvc()->addEvent(m_cut2ID);
  cutFlowSvc()->addEvent(m_filterCutID);

  return (StatusCode::SUCCESS);
}


//*****************************************************
StatusCode myCppFilterTest::finalize() {
  ATH_MSG_DEBUG("finalize()");
  return StatusCode::SUCCESS;
}
