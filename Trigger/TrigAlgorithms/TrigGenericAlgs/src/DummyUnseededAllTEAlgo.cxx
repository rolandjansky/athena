/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigGenericAlgs/DummyUnseededAllTEAlgo.h"
#include "TrigNavigation/TriggerElement.h"
#include "IRegionSelector/IRegSelSvc.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

using namespace PESA;

DummyUnseededAllTEAlgo::DummyUnseededAllTEAlgo(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::AllTEAlgo(name, pSvcLocator),
    m_was_run(false),
    m_regionSelector(0)
{
  declareProperty("NumberOfOutputTEs", m_numberOfOutputTEs = 1, "configure the number of output TEs this algorithm will create");
  declareProperty("createRoIDescriptors",  m_createRoIDescriptors=true, "create RoI descriptors ate eta&phi == 0 if set true");
  declareProperty("runOncePerEvent",  m_runOncePerEvent=false, "run opnly once per event");
  declareProperty("prefetchPIX",  m_prefetchPIX=false, "register ROB requests for pixels");
  declareProperty("prefetchSCT",  m_prefetchSCT=false, "register ROB requests for SCT");
  declareProperty("RegionSelectorTool",    m_regionSelectorName = "RegSelSvc", "instance of the RegionSelector service");
}


HLT::ErrorCode DummyUnseededAllTEAlgo::hltInitialize(){

  m_was_run=false; 

  if ( (serviceLocator()->service( m_regionSelectorName, m_regionSelector)).isFailure() ) {
    msg() << MSG::FATAL 
	  << "Unable to retrieve RegionSelector Service  " << m_regionSelectorName << endmsg;
    return HLT::BAD_JOB_SETUP;
  };

  return HLT::OK; 
}


HLT::ErrorCode DummyUnseededAllTEAlgo::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& /*tes_in*/,
						  unsigned int type_out)
{
  if ( m_runOncePerEvent && m_was_run ) {
    return HLT::OK;
  }
  m_was_run = true;


  // since this is an AllTEAlgo, we have to call the monitoring ourselves:
  beforeExecMonitors().ignore();


  if ( msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Executing DummyUnseededAllTEAlgo (" << name()
	  << "). This is an unseeded algorithm that will produce exactly "
	  << m_numberOfOutputTEs << " output TEs." << endmsg;
  }


  // create the output TEs:
  for (unsigned int i = 0; i < m_numberOfOutputTEs; ++i) {

    // roiDescriptor position:
    //    double eta = 0.0;
    //    double phi = 0.0;
    HLT::TriggerElement* te;
    if ( m_createRoIDescriptors ) {
      /// create a fullscan roi - don;t need to specify eta and phi any longer
      te = addRoI(type_out, new TrigRoiDescriptor(true) ); // note: addRoI is defined in AllTEAlgo base class
    } else {
      te = addRoI(type_out, (TrigRoiDescriptor*)0);
    }
    
    te->setActiveState(true); // set this trigger element to be active

  }

  // since this is an AllTEAlgo, we have to call the monitoring ourselves:
  afterExecMonitors().ignore();

  // everything's ok
  return HLT::OK;
}
HLT::ErrorCode DummyUnseededAllTEAlgo::hltEndEvent() {
  m_was_run = false;
  return HLT::OK;
}

HLT::ErrorCode DummyUnseededAllTEAlgo::prepareRobRequests(const std::vector<HLT::TEConstVec>& /* inputTE */){

  std::vector<unsigned int> uIntListOfRobs;

  if (m_prefetchPIX){
    m_regionSelector->DetROBIDListUint( PIXEL, uIntListOfRobs);
  }
  if (m_prefetchSCT){
    m_regionSelector->DetROBIDListUint( SCT, uIntListOfRobs);
  }

  config()->robRequestInfo()->addRequestScheduledRobIDs(uIntListOfRobs); 	

  return HLT::OK;
} 
