/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_LocReco/AFP_TDLocReco.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h" 

using namespace std;

AFP_TDLocReco::AFP_TDLocReco(const string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
{
  ATH_MSG_DEBUG("in AFP_TDLocReco::AFP_TDLocReco");
}


StatusCode AFP_TDLocReco::initialize()
{
  ATH_MSG_DEBUG("begin AFP_TDLocReco::initialize()");
  CHECK( AthReentrantAlgorithm::initialize() );

  if (m_recoToolHandle.retrieve().isFailure()) {
    ATH_MSG_WARNING("Failed to retrieve tool " << m_recoToolHandle );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("Retrieved tool " << m_recoToolHandle );
  }

  ATH_MSG_DEBUG("end AFP_TDLocReco::initialize()");
  return StatusCode::SUCCESS;
}

StatusCode AFP_TDLocReco::execute(const EventContext& ctx) const
{
  ATH_MSG_DEBUG("begin AFP_TDLocReco::execute()");

  if (m_recoToolHandle->execute(ctx).isFailure()) {
    ATH_MSG_WARNING ("Failed to reconstruct ToF tracks.");
    return StatusCode::SUCCESS;
  }

  ATH_MSG_DEBUG("end AFP_TDLocReco::execute()");
  return StatusCode::SUCCESS;
}

StatusCode AFP_TDLocReco::finalize()
{
  return StatusCode::SUCCESS;
}
