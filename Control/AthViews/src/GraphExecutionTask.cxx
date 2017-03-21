/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthViews/GraphExecutionTask.h"

// Framework
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IAlgResourcePool.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/ThreadLocalContext.h"


//The method for scheduling a subgraph
tbb::task* GraphExecutionTask::execute()
{
	//Quit now if there's nothing to do
	if ( !m_algoNames.size() )
	{
		return nullptr;
	}

	//Quit if you can't find the alg pool
	if ( !m_algResourcePool )
	{
		return nullptr;
	}

	//Try to get control of the first named algorithm in the list
	std::string name = m_algoNames[0];
	IAlgorithm * ialgoPtr = nullptr;
	StatusCode testPool = m_algResourcePool->acquireAlgorithm( name, ialgoPtr, false ); //true = blocking, doesn't work in single-thread mode
	if ( !testPool.isSuccess() )
	{
		//If you can't get control of the algorithm, re-queue this task
		//this->set_depth(0);
		this->recycle_as_safe_continuation();
		return nullptr;
	}

	//Run the algorithm in this context
	Algorithm * algoPtr = dynamic_cast< Algorithm* >( ialgoPtr );
	if ( !algoPtr )
	{
		return nullptr;
	}
#ifdef GAUDI_SYSEXECUTE_WITHCONTEXT
        Gaudi::Hive::setCurrentContext( *m_eventContext );
        algoPtr->whiteboard()->selectStore( m_eventContext->slot() ).ignore();
	algoPtr->sysExecute( *m_eventContext );
#else
	algoPtr->setContext( m_eventContext );
	algoPtr->sysExecute();
#endif

	//Release the algorithm
	testPool = m_algResourcePool->releaseAlgorithm( name, ialgoPtr );

	//Remove the completed algorithm name from the list
	m_algoNames.erase( m_algoNames.begin() );

	//If the list still has entries, add this task on to the queue again
	if ( m_algoNames.size() )
	{
		//this->add_to_depth(1);
		this->recycle_as_safe_continuation();
	}

	//Done!
	return nullptr;
}
