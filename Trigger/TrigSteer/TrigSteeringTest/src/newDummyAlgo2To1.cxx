/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringTest/newDummyAlgo2To1.h"
#include "TrigNavigation/TriggerElement.h"

using namespace PESA;


newDummyAlgo2To1::newDummyAlgo2To1(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::ComboAlgo(name, pSvcLocator),
    m_rejectCounter(0)
{
  declareProperty("rejectRate", m_rejectRate=0, "Factor defining how frequently this algo should make output TE inactive");
}


HLT::ErrorCode newDummyAlgo2To1::hltExecute(std::vector<const HLT::TriggerElement*>& te_in,
					    HLT::TriggerElement* te_out)
{
  msg() << MSG::INFO << "Executing this newDummyAlgo2To1 " << name() << endreq;

  for (std::vector<const HLT::TriggerElement*>::const_iterator it = te_in.begin();
       it != te_in.end(); ++it) {
    msg() << MSG::INFO << "Executing this newDummyAlgo2To1 " << name() << " for types "
	  << (*it)->getId() << " -> " << te_out->getId() << endreq;
  }
  if ( m_rejectRate != 0 )  {
    m_rejectCounter ++;
    if ( m_rejectCounter == m_rejectRate ) {
      te_out->setActiveState(false);
      m_rejectCounter = 0;
    }
  }
  return HLT::OK;
}
