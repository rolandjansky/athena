/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringTest/newDummyAlgoScan.h"
#include "TrigNavigation/TriggerElement.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

using namespace PESA;


newDummyAlgoScan::newDummyAlgoScan(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::AllTEAlgo(name, pSvcLocator)
{
  declareProperty("NumberOfOutputTEs", m_numberOfOutputTEs = 1, "configure the number of output TEs this algorithm will create");
}


HLT::ErrorCode newDummyAlgoScan::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& /*tes_in*/,
					      unsigned int type_out)
{
  // since this is an AllTEAlgo, we have to call the monitoring ourselves:
  beforeExecMonitors().ignore();

  // remove this line in real algorithm!!
  if ( msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Executing newDummyAlgoScan (" << name()
	  << "). This is an unseeded algorithm that will produce exactly "
	  << m_numberOfOutputTEs << " output TEs." << endreq;
  }

  // do the scan here
  // ...



  // create one output TE for each particle found during the scan:
  for (unsigned int i = 0; i < m_numberOfOutputTEs; ++i) {

    TrigRoiDescriptor *roi = new TrigRoiDescriptor(true);     //defines an roi
    HLT::TriggerElement* te = addRoI(type_out, roi); // note: addRoI is defined in AllTEAlgo base class
    te->setActiveState(true); // set this trigger element to be active

  }

  // since this is an AllTEAlgo, we have to call the monitoring ourselves:
  afterExecMonitors().ignore();

  // everything's ok
  return HLT::OK;
}
