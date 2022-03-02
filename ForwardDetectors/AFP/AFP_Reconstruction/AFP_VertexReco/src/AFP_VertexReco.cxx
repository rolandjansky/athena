/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_VertexReco/AFP_VertexReco.h"


AFP_VertexReco::AFP_VertexReco(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
{
  ATH_MSG_DEBUG("in AFP_VertexReco::AFP_VertexReco");
}

AFP_VertexReco::~AFP_VertexReco()
{
  ATH_MSG_DEBUG("in AFP_VertexReco::~AFP_VertexReco");
}

StatusCode AFP_VertexReco::initialize()
{
  ATH_MSG_DEBUG("in AFP_VertexReco::initialize()");
  ATH_CHECK( AthReentrantAlgorithm::initialize() );

  // AFP vertex tool
  if (m_recoToolHandle.retrieve().isFailure()) {
    ATH_MSG_WARNING("Failed to retrieve tool " << m_recoToolHandle );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("Retrieved tool " << m_recoToolHandle );
  }

  return StatusCode::SUCCESS;
}

StatusCode AFP_VertexReco::execute(const EventContext& ctx) const
{
  if (m_recoToolHandle->execute(ctx).isFailure()) {
    ATH_MSG_WARNING ("Failed to reconstruct tracks.");
    return StatusCode::SUCCESS;
  }

  return StatusCode::SUCCESS;
}
