/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GAUDIKERNEL_ISCHEDULER_H
#define GAUDIKERNEL_ISCHEDULER_H

// Framework include files
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/IInterface.h"

// C++ include files
#include <functional>
#include <vector>
#include <string>

/**@class IScheduler IScheduler.h GaudiKernel/IScheduler.h
 *
 *  General interface for algorithm scheduler.
 * 
 *  @author  Danilo Piparo
 *  @author  Benedikt Hegner
 *  @version 1.1
 */
class GAUDI_API IScheduler: virtual public IInterface {
public:
  /// InterfaceID
  DeclareInterfaceID(IScheduler,1,0);
  
  /// Make an event available to the scheduler
  virtual StatusCode pushNewEvent(EventContext* eventContext) = 0;

  /// Make a list of events available to the scheduler
  /// This method makes a bunch creation of new events atomic to the scheduler
  virtual StatusCode pushNewEvents(std::vector<EventContext*>& eventContexts) = 0; 
    
  /// Retrieve a finished event from the scheduler
  virtual StatusCode popFinishedEvent(EventContext*& eventContext) = 0;

  //HACK
  //virtual StatusCode runAlgorithm( std::string AlgName, EventContext* eventContext ) = 0;
  //virtual StatusCode pushSubEventsAndSuspend( std::vector< EventContext* > subEventContexts ) = 0;
  virtual StatusCode runAlgorithm( std::string AlgName, EventContext* eventContext ){ return StatusCode::FAILURE; };
  virtual StatusCode pushSubEventsAndSuspend( std::vector< EventContext* > subEventContexts ){ return StatusCode::FAILURE; };

  /// Try to retrieve a finished event from the scheduler
  virtual StatusCode tryPopFinishedEvent(EventContext*& eventContext) = 0;
  
  /// Get the free event processing slots
  virtual unsigned int freeSlots() = 0;
  
  virtual void dumpState() {};
  
};
#endif
