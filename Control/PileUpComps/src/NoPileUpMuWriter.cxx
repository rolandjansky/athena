
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "NoPileUpMuWriter.h"

#include <StoreGate/WriteDecorHandle.h>

NoPileUpMuWriter::NoPileUpMuWriter(const std::string& name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name,pSvcLocator)
{
}

StatusCode NoPileUpMuWriter::initialize() 
{
  ATH_CHECK(m_actMuKey.initialize());
  ATH_CHECK(m_aveMuKey.initialize());
  return StatusCode::SUCCESS;
}

StatusCode NoPileUpMuWriter::execute(const EventContext& ctx)  const
{
  SG::WriteDecorHandle<xAOD::EventInfo, float> actMu(m_actMuKey, ctx);
  if (!actMu.isPresent()) {
    ATH_MSG_ERROR("actualInteractionsPerCrossing.isPresent check fails");
    return StatusCode::FAILURE;
  }
  if (!actMu.isAvailable()) {
    actMu(0) = 0.;
  }

  SG::WriteDecorHandle<xAOD::EventInfo, float> aveMu(m_aveMuKey, ctx);
  if (!aveMu.isPresent()) {
    ATH_MSG_ERROR("averageInteractionsPerCrossing.isPresent check fails");
    return StatusCode::FAILURE;
  }
  if (!aveMu.isAvailable()) {
    aveMu(0) = 0.;
  }

  return StatusCode::SUCCESS;
}
