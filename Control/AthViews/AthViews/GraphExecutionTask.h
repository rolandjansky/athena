/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GAUDIHIVE_GRAPHEXECUTIONTASK_H
#define GAUDIHIVE_GRAPHEXECUTIONTASK_H

// Framework include files
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/IAlgResourcePool.h"
#include "GaudiKernel/EventContext.h"

// External libs
#include "tbb/task.h"
#include <vector>
#include <string>

class GraphExecutionTask: public tbb::task
{
  public:
    GraphExecutionTask( std::vector< std::string > const& algoNames, EventContext * inputContext, SmartIF< IService > & algPool ):
	    m_eventContext( inputContext ),
	    m_algoNames( algoNames ),
	    m_algResourcePool( algPool ){};

  virtual tbb::task* execute();

  private:  
    EventContext * m_eventContext;
    std::vector< std::string > m_algoNames;
    SmartIF< IAlgResourcePool > m_algResourcePool;
};

#endif
