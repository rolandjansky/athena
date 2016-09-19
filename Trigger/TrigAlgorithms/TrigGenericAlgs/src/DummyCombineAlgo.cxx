/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigGenericAlgs/DummyCombineAlgo.h"
#include "TrigNavigation/TriggerElement.h"

using namespace PESA;

DummyCombineAlgo::DummyCombineAlgo(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::ComboAlgo(name, pSvcLocator)
{}


HLT::ErrorCode DummyCombineAlgo::hltExecute(HLT::TEConstVec& /*inputs*/, HLT::TriggerElement* /*output*/)
{
  if ( msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Executing DummyCombineAlgo (" << name()
	  << ")." << endmsg;
  }

  // everything's ok
  return HLT::OK;
}
