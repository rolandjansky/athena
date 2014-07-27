/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringTest/newDummyAlgoAllTEs.h"
#include "TrigNavigation/TriggerElement.h"

using namespace PESA;


newDummyAlgoAllTEs::newDummyAlgoAllTEs(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::AllTEAlgo(name, pSvcLocator)
{
  //  declareProperty("ParamSetFile", m_paramSet);
}


HLT::ErrorCode newDummyAlgoAllTEs::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in,
					      unsigned int type_out)
{
  // since this is an AllTEAlgo, we have to call the monitoring ourselves:
  beforeExecMonitors().ignore();

  if ( msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Executing this newDummyAlgoAllTEs " << name() << endreq;
  }


  HLT::TEVec allTEs;

  for (std::vector<HLT::TEVec>::const_iterator it = tes_in.begin();
       it != tes_in.end(); ++it) {

    for (HLT::TEVec::const_iterator inner_it = (*it).begin();
	 inner_it != (*it).end(); ++inner_it) {
      if ( msgLvl() <= MSG::DEBUG) {
	msg() << MSG::DEBUG << "Executing this newDummyAlgoAllTEs " << name() << " for TE type "
	      << (*inner_it)->getId() << endreq;
      }
      allTEs.push_back(*inner_it);
    }
  }


  // create output TE:
  // Create an output TE seeded by the inputs
  HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(allTEs, type_out);
  outputTE->setActiveState(true);


  // since this is an AllTEAlgo, we have to call the monitoring ourselves:
  afterExecMonitors().ignore();

  return HLT::OK;
}
