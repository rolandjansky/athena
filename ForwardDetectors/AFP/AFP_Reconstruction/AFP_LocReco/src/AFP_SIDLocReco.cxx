/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_LocReco/AFP_SIDLocReco.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h" 

using namespace std;

AFP_SIDLocReco::AFP_SIDLocReco(const string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_recoToolHandle("AFP_SIDLocRecoTool")
{
  ATH_MSG_DEBUG("begin AFP_SIDLocReco::AFP_SIDLocReco");

  declareProperty("recoTool", m_recoToolHandle, "Tool that makes all tracks reconstruction");

  ATH_MSG_DEBUG("end AFP_SIDLocReco::AFP_SIDLocReco");
}


StatusCode AFP_SIDLocReco::initialize()
{
  ATH_MSG_DEBUG("begin AFP_SIDLocReco::initialize()");

  // --- Prepare clustering tools ---
  StatusCode sc = m_recoToolHandle.retrieve(); //Dot, not asterisk! This is a method of the ToolHandle, not of the tool it holds.
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Failed to retrieve recoClusters " << m_recoToolHandle);
    return sc;
  }

  ATH_MSG_DEBUG("end AFP_SIDLocReco::initialize()");
  return StatusCode::SUCCESS;
}

StatusCode AFP_SIDLocReco::execute()
{
  ATH_MSG_DEBUG("begin AFP_SIDLocReco::execute()");

  if (m_recoToolHandle->execute().isFailure()) {
    ATH_MSG_WARNING ("Failed to reconstruct tracks.");
    return StatusCode::SUCCESS;
  }

  ATH_MSG_DEBUG("end AFP_SIDLocReco::execute()");
  return StatusCode::SUCCESS;
}

StatusCode AFP_SIDLocReco::finalize()
{
  return StatusCode::SUCCESS;
}
