/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInterfaces/TECreateAlgo.h"

#include "TrigInterfaces/AlgoConfig.h"
#include "TrigNavigation/ComboIterator.h"
#include "TrigNavigation/Navigation.h"
#include "TrigNavigation/TriggerElement.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "GaudiKernel/GaudiException.h"
#include "eformat/Issue.h"

using namespace HLT;


StatusCode TECreateAlgo::initialize()
{
  StatusCode code = Algo::initialize();
  if (code.isFailure()) return code;

  //  m_totalTimeWithCreate = (doTiming() ? addTimer("TotalTimeWithCreate") : 0);

  return StatusCode::SUCCESS;
}


ComboIteratorBase* TECreateAlgo::newComboIterator(std::vector<unsigned int>& inputs,
						  bool onlyActive)
{
  std::vector<TEVec> tes;
  
  for (std::vector<unsigned int>::const_iterator input = inputs.begin();
       input != inputs.end(); input++) {
    std::vector<HLT::TriggerElement*> newTEs;
    config()->getNavigation()->getAllOfType(*input, newTEs, onlyActive);
    tes.push_back(newTEs);
  }

  return new ComboIterator(tes, config()->getNavigation());
}


ComboIteratorBase* TECreateAlgo::newComboIterator(std::vector<unsigned int>& inputs,
						  const std::vector<unsigned int>& topologicalStartFrom,
						  bool onlyActive)
{
  // Check if the vector of topological types has the correct size (one)
  if(topologicalStartFrom.size()==0) {
    ATH_MSG_WARNING ("A ComboIteratorTopological starting from zero topological types has been requested, continuing anyway") ;
  } else if(topologicalStartFrom.size()>1) {
    ATH_MSG_WARNING ( "A ComboIteratorTopological starting from more than one (" << topologicalStartFrom.size()
		      << ") topological types has been requested, continuing anyway" );
  }

  // Get topological TEs to start from and create the iterator
  TEVec topologicalTEs;
  if(topologicalStartFrom.size()) config()->getNavigation()->getAllOfType(topologicalStartFrom[0], topologicalTEs, onlyActive);
  return new ComboIteratorTopological(topologicalTEs, inputs, config()->getNavigation(), onlyActive);
}

/*
ErrorCode TECreateAlgo::createAndProcessTEs(std::vector<unsigned int>& inputs, 
					    unsigned int outputType,
					    TEVec& outputTEs,
					    const std::vector<unsigned int>& topologicalStartFrom)

*/



ErrorCode TECreateAlgo::processTEs(TEVec& outputTEs) {

  // the caching of error code
  // we do not want to try another execution if this instance gave an error already in this event
  if (m_returnErrorCode.action() > HLT::Action::CONTINUE ) {    
    ATH_MSG_DEBUG ( name() 
		    << " already failed in this event, doing nothing" );	

    return m_returnErrorCode;
  }
  
  if (!runAlgo()) return HLT::OK;

  beforeEventMonitors().ignore();
  ErrorCode status = HLT::OK;

  // For each TE in the TE vector, ...
  for (std::vector<TriggerElement*>::iterator outputTE = outputTEs.begin();
       outputTE != outputTEs.end(); ++outputTE) {
    if (!(*outputTE)->getActiveState()) continue;
    

    if ( needsExecution( *outputTE ) ) {

      TEConstVec inputs = constify(Navigation::getDirectPredecessors( *outputTE ));

      beforeExecMonitors().ignore();
      // start collecting cache information
      if ( ! startCaching(*outputTE) ) {
	ATH_MSG_ERROR ( name() 
			<< " start of caching on TE " << (*outputTE)->getId()
			<< " failed " );
	return HLT::ErrorCode( Action::ABORT_JOB, Reason::USERDEF_4);
      }

      ErrorCode ec = plugin_processTE(inputs, *outputTE);
      // stop collecting cache information
      if ( ! stopCaching(*outputTE) ) {
	ATH_MSG_ERROR ( name() 
			<< " start of caching on TE " << (*outputTE)->getId()
			<< " failed " );
	return HLT::ErrorCode( Action::ABORT_JOB, Reason::USERDEF_4);
      }

      mapAndRecordError(ec, *outputTE);
      
      afterExecMonitors().ignore();

      if ( m_config->getSteeringOPILevel() > 0 ) {
      	addOperationalInfo(*outputTE);
	addSteeringOperationalInfo(true, 1, *outputTE);
      }

      status = (m_returnErrorCode > status) ? m_returnErrorCode : status;
      
      if (status.action() > HLT::Action::CONTINUE  ) {
        afterEventMonitors().ignore();
        return status;
      }
    } else {
      // move cached results
      if (! cache(*outputTE) ) {
	ATH_MSG_ERROR ( name() 
			<< " caching on TE " << (*outputTE)->getId()
			<< " failed " );
	return HLT::ErrorCode( Action::ABORT_JOB, Reason::USERDEF_4);
      } 

      if ( m_config->getSteeringOPILevel() > 0 ) {
	addSteeringOperationalInfo(false, 1, *outputTE);
      }

      if ( msgLvl() <= MSG::DEBUG )
	ATH_MSG_DEBUG ( name() 
			<< " already ran on RoIs(and obejcts) of TE " << (*outputTE)->getId()
			<< ", reattached features only." );
    }
  }

  afterEventMonitors().ignore();
  return status;
}


bool TECreateAlgo::reset() {
  RoICacheHelper::reset();
  return Algo::reset();
}
