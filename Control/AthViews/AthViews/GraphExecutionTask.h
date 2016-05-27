/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GAUDIHIVE_GRAPHEXECUTIONTASK_H
#define GAUDIHIVE_GRAPHEXECUTIONTASK_H

// Framework include files
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/ISvcLocator.h"

//#include "ForwardSchedulerSvc.h"
#include "GaudiKernel/EventContext.h"

// External libs
#include "tbb/task.h"
#include <vector>
#include <string>

class GraphExecutionTask: public tbb::task {
public:
  GraphExecutionTask( std::vector< std::string > const& algoNames,
		    EventContext* inputContext,
                    ISvcLocator* svcLocator)://,
                    //ForwardSchedulerSvc* schedSvc):
    m_eventContext(inputContext),
    m_algoNames(algoNames),
    //m_schedSvc(schedSvc),
    m_serviceLocator(svcLocator){};
  virtual tbb::task* execute();
private:  
  EventContext * m_eventContext;
  std::vector< std::string > m_algoNames;
  // For the callbacks on task finishing
  //SmartIF<ForwardSchedulerSvc> m_schedSvc;
  SmartIF<ISvcLocator> m_serviceLocator;
};

#endif
