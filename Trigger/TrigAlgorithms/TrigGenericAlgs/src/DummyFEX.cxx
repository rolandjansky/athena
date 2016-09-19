/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInterfaces/AlgoConfig.h"
#include "TrigNavigation/TriggerElement.h"
#include "TrigNavigation/Navigation.h"
// #include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigGenericAlgs/DummyFEX.h"





DummyFEX::DummyFEX(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::FexAlgo(name, pSvcLocator)
{
}


HLT::ErrorCode DummyFEX::hltExecute(const HLT::TriggerElement* te_in,
				     HLT::TriggerElement* te_out)
{
  if ( msgLvl() <= MSG::DEBUG ) 
    msg() << MSG::DEBUG << "Executing this DummyFEX " << name() << " for types "
	  << te_in->getId() << " -> " << te_out->getId() << endmsg;

  return HLT::OK;
}
