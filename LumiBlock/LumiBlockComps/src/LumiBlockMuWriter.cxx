/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LumiBlockMuWriter.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/WriteDecorHandle.h"

LumiBlockMuWriter::LumiBlockMuWriter(const std::string& name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name,pSvcLocator)
{
}

StatusCode LumiBlockMuWriter::initialize() 
{
  ATH_MSG_INFO("LumiBlockMuWriter::initialize()");

  ATH_CHECK(m_lumiDataKey.initialize(SG::AllowEmpty));
  ATH_CHECK(m_actMuKey.initialize());
  ATH_CHECK(m_aveMuKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode LumiBlockMuWriter::execute(const EventContext& ctx)  const
{
  ATH_MSG_DEBUG("LumiBlockMuWriter::execute()");

  float actualMu = 0.0;
  float muToLumi = 0.0;
  float averageMu = 0.0;
  if (!m_lumiDataKey.empty()) {
    SG::ReadCondHandle<LuminosityCondData> lumiData (m_lumiDataKey, ctx);
    if (lumiData->lbAverageLuminosity() != 0 ||
        lumiData->lbAverageInteractionsPerCrossing() != 0) {
      muToLumi = lumiData->muToLumi();
      if( std::abs( muToLumi ) > 0.00001 ) {
        unsigned int bcid = ctx.eventID().bunch_crossing_id();
        actualMu = lumiData->lbLuminosityPerBCIDVector().at(bcid) / muToLumi;
      }
    }
    averageMu = lumiData->lbAverageInteractionsPerCrossing();
  }

  SG::WriteDecorHandle<xAOD::EventInfo,float> actMu(m_actMuKey,ctx);
  if (!actMu.isPresent()) {
    ATH_MSG_ERROR( "actualInteractionsPerCrossing.isPresent check fails" );
    return StatusCode::FAILURE;
  }
  if (!actMu.isAvailable()) {
    actMu(0) = actualMu;
  }
    
  SG::WriteDecorHandle<xAOD::EventInfo,float> aveMu(m_aveMuKey,ctx);
  if (!aveMu.isPresent()) {
    ATH_MSG_ERROR( "averageInteractionsPerCrossing.isPresent check fails" );
    return StatusCode::FAILURE;
  }
  if (!aveMu.isAvailable()) {
    aveMu(0) = averageMu;
  }

  return StatusCode::SUCCESS;
}
