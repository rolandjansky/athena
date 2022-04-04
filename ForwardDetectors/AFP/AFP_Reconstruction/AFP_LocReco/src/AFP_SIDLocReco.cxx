/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_LocReco/AFP_SIDLocReco.h"


AFP_SIDLocReco::AFP_SIDLocReco(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
{
  ATH_MSG_DEBUG("in AFP_SIDLocReco::AFP_SIDLocReco");
}

AFP_SIDLocReco::~AFP_SIDLocReco()
{
  ATH_MSG_DEBUG("in AFP_SIDLocReco::~AFP_SIDLocReco");
}

StatusCode AFP_SIDLocReco::initialize()
{
  ATH_MSG_DEBUG("in AFP_SIDLocReco::initialize()");
  CHECK( AthReentrantAlgorithm::initialize() );

  // reco track tool
  if (m_recoToolHandle.retrieve().isFailure()) {
    ATH_MSG_WARNING("Failed to retrieve tool " << m_recoToolHandle );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("Retrieved tool " << m_recoToolHandle );
  }

  return StatusCode::SUCCESS;
}

StatusCode AFP_SIDLocReco::execute(const EventContext& ctx) const
{
  if (m_recoToolHandle->execute(ctx).isFailure()) {
    ATH_MSG_WARNING ("Failed to reconstruct tracks.");
    return StatusCode::SUCCESS;
  }

  return StatusCode::SUCCESS;
}

StatusCode AFP_SIDLocReco::finalize()
{
  ATH_MSG_DEBUG("in AFP_SIDLocReco::finalize()");
  return StatusCode::SUCCESS;
}
