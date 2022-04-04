/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_GlobReco/AFP_GlobReco.h"


AFP_GlobReco::AFP_GlobReco(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
{
  ATH_MSG_DEBUG("in AFP_GlobReco::AFP_GlobReco");
}

AFP_GlobReco::~AFP_GlobReco()
{
  ATH_MSG_DEBUG("in AFP_GlobReco::~AFP_GlobReco");
}

StatusCode AFP_GlobReco::initialize()
{
  ATH_MSG_DEBUG("in AFP_GlobReco::initialize()");
  CHECK( AthReentrantAlgorithm::initialize() );

  // proton track tool
  if (m_recoToolHandle.retrieve().isFailure()) {
    ATH_MSG_WARNING("Failed to retrieve tool " << m_recoToolHandle );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("Retrieved tool " << m_recoToolHandle );
  }

  return StatusCode::SUCCESS;
}

StatusCode AFP_GlobReco::execute(const EventContext& ctx) const
{
  if (m_recoToolHandle->execute(ctx).isFailure()) {
    ATH_MSG_WARNING ("Failed to reconstruct tracks.");
    return StatusCode::SUCCESS;
  }

  return StatusCode::SUCCESS;
}

StatusCode AFP_GlobReco::finalize()
{
  ATH_MSG_DEBUG("in AFP_GlobReco::finalize()");
  return StatusCode::SUCCESS;
}
