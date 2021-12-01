/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_LocReco/AFP_TDLocReco.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h" 

using namespace std;

AFP_TDLocReco::AFP_TDLocReco(const string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_recoToolHandle("AFP_TDLocRecoTool")
{
  ATH_MSG_DEBUG("begin AFP_TDLocReco::AFP_TDLocReco");

  declareProperty("recoTool", m_recoToolHandle, "Tool that makes all tracks reconstruction");

  ATH_MSG_DEBUG("end AFP_TDLocReco::AFP_TDLocReco");
}


StatusCode AFP_TDLocReco::initialize()
{
  ATH_MSG_DEBUG("begin AFP_TDLocReco::initialize()");

  // --- Prepare time of flight tools ---
  StatusCode sc = m_recoToolHandle.retrieve(); //Dot, not asterisk! This is a method of the ToolHandle, not of the tool it holds.
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Failed to retrieve hits " << m_recoToolHandle);
    return sc;
  }

  ATH_MSG_DEBUG("end AFP_TDLocReco::initialize()");
  return StatusCode::SUCCESS;
}

StatusCode AFP_TDLocReco::execute()
{
  ATH_MSG_DEBUG("begin AFP_TDLocReco::execute()");

  if (m_recoToolHandle->execute().isFailure()) {
    ATH_MSG_WARNING ("Failed to reconstruct tracks.");
    return StatusCode::SUCCESS;
  }

  ATH_MSG_DEBUG("end AFP_TDLocReco::execute()");
  return StatusCode::SUCCESS;
}

StatusCode AFP_TDLocReco::finalize()
{
  return StatusCode::SUCCESS;
}
