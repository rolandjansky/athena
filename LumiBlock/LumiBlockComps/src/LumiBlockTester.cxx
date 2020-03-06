/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LumiBlockTester.h"
#include "StoreGate/ReadCondHandle.h"
#include "AthenaKernel/errorcheck.h"

//--------------------------------------------------
LumiBlockTester::LumiBlockTester(const std::string& name, ISvcLocator* pSvcLocator):
  AthReentrantAlgorithm(name,pSvcLocator),
  m_muTool("LumiBlockMuTool")
{
  declareProperty("LumiBlockMuTool", m_muTool);
}

StatusCode
LumiBlockTester::initialize() 
{
  ATH_MSG_INFO("LumiBlockTester::initialize()");

  ATH_CHECK(m_eventInfoKey.initialize());

  ATH_CHECK( m_luminosityCondDataKey.initialize() );
  ATH_CHECK( m_trigLiveFractionCondDataKey.initialize() );

  // Get the mu tool
  CHECK(m_muTool.retrieve());

  ATH_MSG_INFO("LumiBlockTester::initialize() done");

  return StatusCode::SUCCESS;
}

StatusCode
LumiBlockTester::execute (const EventContext& ctx)  const
{
  ATH_MSG_DEBUG("LumiBlockTester::execute()");

  SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfoKey, ctx);

  // only there for serial running; remove when only doing MT
  if(!eventInfo.isValid()) {
    ATH_MSG_FATAL("Failed to retrieve "<< m_eventInfoKey.key());
    return StatusCode::FAILURE;
  }

  unsigned int lumiblock = eventInfo->lumiBlock();
  unsigned int bcid = eventInfo->bcid();

  ATH_MSG_DEBUG(" lumiblock " << lumiblock << " BCID " << bcid);

  SG::ReadCondHandle<LuminosityCondData> lumiData (m_luminosityCondDataKey, ctx);
  float avgmu = lumiData->lbAverageInteractionsPerCrossing();
  float instmu = 0.;

  if (lumiData->muToLumi() > 0.)
    instmu = lumiData->lbLuminosityPerBCIDVector().at(bcid)/lumiData->muToLumi(); 
  else
    ATH_MSG_DEBUG(" Lumi: " << lumiData->lbLuminosityPerBCIDVector().at(bcid) << " muToLumi: " << lumiData->muToLumi() << "!");

  SG::ReadCondHandle<TrigLiveFractionCondData> liveData (m_trigLiveFractionCondDataKey, ctx);
  float live = liveData->l1LiveFractionVector().at(bcid);
  float lumilive = liveData->lbAverageLiveFraction();

  ATH_MSG_INFO( "LB: " << lumiblock << " BCID: " << bcid << " <mu>: " << avgmu << " mu: " << instmu << " livefraction: " << live << " lumiavg livefraction: " << lumilive );

  avgmu = m_muTool->averageInteractionsPerCrossing();
  instmu = m_muTool->actualInteractionsPerCrossing();
  ATH_MSG_INFO( "From muTool - <mu>: " << avgmu << " mu: " << instmu);

  instmu = eventInfo->actualInteractionsPerCrossing();
  avgmu = eventInfo->averageInteractionsPerCrossing();
  ATH_MSG_INFO( "From EvInfo - <mu>: " << avgmu << " mu: " << instmu);

  ATH_MSG_DEBUG("LumiBlockTester::execute() done");
  return StatusCode::SUCCESS;
}
