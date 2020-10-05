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
  ATH_CHECK(m_actIntPerXKey.initialize());
  ATH_CHECK(m_aveIntPerXKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode LumiBlockMuWriter::execute(const EventContext& ctx)  const
{
  ATH_MSG_DEBUG("LumiBlockMuWriter::execute()");

  float actualMu = 0.0;
  float muToLumi = 0.0;
  float aveIntPerX = 0.0;
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
    aveIntPerX = lumiData->lbAverageInteractionsPerCrossing();
  }

  SG::WriteDecorHandle<xAOD::EventInfo,float> actIntPerXDecor(m_actIntPerXKey,ctx);
  if (!actIntPerXDecor.isPresent()) {
    ATH_MSG_ERROR( "actIntPerXDecor.isPresent check fails" );
    return StatusCode::FAILURE;
  }
  if (!actIntPerXDecor.isAvailable()) {
    actIntPerXDecor(0) = actualMu;
  }
    
  SG::WriteDecorHandle<xAOD::EventInfo,float> aveIntPerXDecor(m_aveIntPerXKey,ctx);
  if (!aveIntPerXDecor.isPresent()) {
    ATH_MSG_ERROR( "aveIntPerXDecor.isPresent check fails" );
    return StatusCode::FAILURE;
  }
  if (!aveIntPerXDecor.isAvailable()) {
    aveIntPerXDecor(0) = aveIntPerX;
  }

  return StatusCode::SUCCESS;
}
