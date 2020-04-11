/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInterfaces/AllTEAlgo.h"
#include "TrigInterfaces/AlgoConfig.h"
#include "TrigInterfaces/ExceptionHandler.h"
#include "TrigNavigation/Navigation.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigSuperRoi.h"
#include "TrigTimeAlgs/TrigTimer.h"

#include "GaudiKernel/GaudiException.h"

using namespace HLT;


AllTEAlgo::AllTEAlgo(const std::string& name, ISvcLocator* pSvcLocator) 
  : TECreateAlgo(name, pSvcLocator),
    m_provideInactiveTEs(false) { 
  declareProperty("provideInactiveTEs", m_provideInactiveTEs=false, 
		  "Use inactive TriggerElements for seeding the algorithm.");
}
 
ErrorCode AllTEAlgo::createAndProcessTEs(std::vector<unsigned int>& inputs,
					 unsigned int output,
					 TEVec& outputTEs,
					 const std::vector<unsigned int>& /*topologicalStartFrom*/)
{
  // the caching of error code
  if (m_returnErrorCode.action() > HLT::Action::CONTINUE ) {    
    ATH_MSG_DEBUG ( name() 
		    << " already failed in this event, doing nothing" );	
    return m_returnErrorCode;
  }


  if (!runAlgo()) {
    config()->getNavigation()->getAllOfType(output, outputTEs, !m_provideInactiveTEs); 
    return HLT::OK;
  }

  if ( Algo::totalTimeTimer() ) Algo::totalTimeTimer()->start();  

  
  beforeEventMonitors().ignore();
  std::vector<TEVec> inputSets;

  for (std::vector<unsigned int>::const_iterator input = inputs.begin();
       input != inputs.end(); ++input) {

    TEVec inputSet;
    config()->getNavigation()->getAllOfType(*input, inputSet);
    inputSets.push_back(inputSet);
  }

  ErrorCode ec = HLT::OK;
  try {
      ec= hltExecute(inputSets, output);
  } catch (...) {
    ec = trigExceptionHandler(&msg());
  } 
        
  mapAndRecordError(ec);
    
  m_config->getNavigation()->getAllOfType(output, outputTEs);

  afterEventMonitors().ignore();

  if ( Algo::totalTimeTimer() ) Algo::totalTimeTimer()->stop();

  if ( m_config->getSteeringOPILevel() > 0 ) {
    addSteeringOperationalInfo(true, outputTEs.size(), 0);
  }

  return m_returnErrorCode;
}


// This is called if algo is not in 1st position -- should never happen for Topologicals.

ErrorCode AllTEAlgo::plugin_processTE(TEConstVec&, TriggerElement*)
{
  ATH_MSG_ERROR ( "AllTEAlgo::plugin_processTE should never be called!  (this happens if this instance is not in 1st position of the Sequence)" );
  return m_returnErrorCode = HLT::BAD_JOB_SETUP;
}



TriggerElement* AllTEAlgo::addRoI(unsigned int type, const TrigRoiDescriptor* roiDescriptor)
{
  // retrieve initial node
  HLT::TriggerElement* initialTE = m_config->getNavigation()->getInitialNode();

  // attach roi TriggerElement, this has no type !
  HLT::TriggerElement* roiTE = m_config->getNavigation()->addRoINode( initialTE );

  // if an roiDescriptor is provided, attach it to the RoI node:
  if (roiDescriptor) {
    std::string key;
    m_config->getNavigation()->attachFeature( roiTE, roiDescriptor, HLT::Navigation::ObjectCreatedByNew, key, "secondaryRoI"+m_config->getInstance() );
  }

  // attach first and only TE (of given type) to this roi TE:
  return m_config->getNavigation()->addNode(roiTE, type);
}


#if 0
TriggerElement* AllTEAlgo::addRoI(unsigned int type, const TrigSuperRoi* roiDescriptor)
{
  // retrieve initial node
  HLT::TriggerElement* initialTE = m_config->getNavigation()->getInitialNode();

  // attach roi TriggerElement, this has no type !
  HLT::TriggerElement* roiTE = m_config->getNavigation()->addRoINode( initialTE );

  // if an roiDescriptor is provided, attach it to the RoI node:
  if (roiDescriptor) {
    std::string key;
    m_config->getNavigation()->attachFeature( roiTE, roiDescriptor, HLT::Navigation::ObjectCreatedByNew, key, "secondaryRoI"+m_config->getInstance() );
  }

  // attach first and only TE (of given type) to this roi TE:
  return m_config->getNavigation()->addNode(roiTE, type);
}
#endif


#if 0
//// OBSOLETE METHOD, DO NOT USE!!!
TriggerElement* AllTEAlgo::addRoI(unsigned int type, double eta, double phi)
{
  ATH_MSG_FATAL ( "AllTEAlgo::addRoI( type eta, phi): obsolete method - do not use" );

  return 0;
}
#endif

HLT::ErrorCode
AllTEAlgo::processRobRequests(const std::vector< unsigned int >& inputs ) {

  std::vector<TEConstVec> inputSets;

  for (std::vector<unsigned int>::const_iterator input = inputs.begin();
       input != inputs.end(); ++input) {

    TEVec inputSet;
    config()->getNavigation()->getAllOfType(*input, inputSet);
    inputSets.push_back(constify(inputSet));
  }

  ErrorCode ec = HLT::OK;
  try {
    ATH_MSG_DEBUG ( "AllTEAlgo::processRobRequests(): calling prepareRobRequests() of AllTEAlgo implementation" );
    ec = prepareRobRequests(inputSets );
  } catch (...) {
    ec = trigExceptionHandler(&msg());
  } 

  return ec;
}


HLT::ErrorCode
AllTEAlgo::prepareRobRequests(const std::vector<TEConstVec>& /*inputTEVs*/ )
{
  ATH_MSG_DEBUG ( "prepareRobRequests() not implemented for AllTEAlgo " << name() );
  return HLT::ErrorCode( Action::CONTINUE );
}
