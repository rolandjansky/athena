/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LumiBlockComps/LumiBlockTester.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventInfo.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/errorcheck.h"

//--------------------------------------------------
LumiBlockTester::LumiBlockTester(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator),
  m_lumiTool("LuminosityTool"),
  m_liveTool("TrigLivefractionTool"),
  m_muTool("LumiBlockMuTool")
{
  declareProperty("LuminosityTool", m_lumiTool);
  declareProperty("TrigLivefractionTool", m_liveTool);
  declareProperty("LumiBlockMuTool", m_muTool);
}

StatusCode
LumiBlockTester::initialize() 
{
  ATH_MSG_INFO("LumiBlockTester::initialize()");

  // Get the luminosity tool
  CHECK(m_lumiTool.retrieve());

  // Get the livefration tool
  CHECK(m_liveTool.retrieve());

  // Get the mu tool
  CHECK(m_muTool.retrieve());

  ATH_MSG_INFO("LumiBlockTester::initialize() done");

  return StatusCode::SUCCESS;
}

StatusCode
LumiBlockTester::execute() 
{
  ATH_MSG_DEBUG("LumiBlockTester::execute()");

  const EventInfo* eventInfo_c=0;
  CHECK(evtStore()->retrieve(eventInfo_c));

  unsigned int lumiblock = eventInfo_c->event_ID()->lumi_block();
  unsigned int bcid = eventInfo_c->event_ID()->bunch_crossing_id();

  ATH_MSG_DEBUG(" lumiblock " << lumiblock << " BCID " << bcid);

  float avgmu = m_lumiTool->lbAverageInteractionsPerCrossing();
  float instmu = 0.;

  if (m_lumiTool->muToLumi() > 0.)
    instmu = m_lumiTool->lbLuminosityPerBCID()/m_lumiTool->muToLumi(); 
  else
    ATH_MSG_DEBUG(" Lumi: " << m_lumiTool->lbLuminosityPerBCID() << " muToLumi: " << m_lumiTool->muToLumi() << "!");

  float live = m_liveTool->livefractionPerBCID();
  float lumilive = m_liveTool->lbAverageLivefraction();

  ATH_MSG_INFO( "LB: " << lumiblock << " BCID: " << bcid << " <mu>: " << avgmu << " mu: " << instmu << " livefraction: " << live << " lumiavg livefraction: " << lumilive );

  avgmu = m_muTool->averageInteractionsPerCrossing();
  instmu = m_muTool->actualInteractionsPerCrossing();
  ATH_MSG_INFO( "From muTool - <mu>: " << avgmu << " mu: " << instmu);

  const EventInfo* eventInfo;
  CHECK(evtStore()->retrieve(eventInfo));
  instmu = eventInfo->actualInteractionsPerCrossing();
  avgmu = eventInfo->averageInteractionsPerCrossing();
  ATH_MSG_INFO( "From EvInfo - <mu>: " << avgmu << " mu: " << instmu);

  ATH_MSG_DEBUG("LumiBlockTester::execute() done");
  return StatusCode::SUCCESS;
}
