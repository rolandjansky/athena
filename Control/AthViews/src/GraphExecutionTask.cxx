/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Framework
#include "AthViews/GraphExecutionTask.h"

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/ContextSpecificPtr.h"
#include "GaudiKernel/IAlgResourcePool.h"

// C++
//#include <functional>
//#include "tbb/flow_graph.h"


//The method for scheduling a subgraph
tbb::task* GraphExecutionTask::execute()
{
	//Quit now if there's nothing to do
	if ( !m_algoNames.size() ) return nullptr;

	//Get the control mechanism for the algorithm pointers
	SmartIF< IAlgResourcePool > algResourcePool = 0;
	algResourcePool = m_serviceLocator->service( "AlgResourcePool" );

	//Try to get control of the first named algorithm in the list
	std::string name = m_algoNames[0];
	IAlgorithm * ialgoPtr = nullptr;
	StatusCode testPool = algResourcePool->acquireAlgorithm( name, ialgoPtr, false ); //true = blocking, doesn't work in single-thread mode
	if ( !testPool.isSuccess() )
	{
		//If you can't get control of the algorithm, re-queue this task
		tbb::task::enqueue( *this );
		return nullptr;
	}

	//Run the algorithm in this context
	Algorithm * algoPtr = dynamic_cast< Algorithm* >( ialgoPtr );
	if ( !algoPtr ) return nullptr;
	algoPtr->setContext( m_eventContext );
	algoPtr->sysExecute();

	//Release the algorithm
	testPool = algResourcePool->releaseAlgorithm( name, ialgoPtr );

	//Remove the completed algorithm name from the list
	m_algoNames.erase( m_algoNames.begin() );

	//If the list still has entries, add this task on to the queue again
	if ( m_algoNames.size() )
	{
		this->recycle_to_reexecute();
	}
	return nullptr;


	/*tbb::flow::graph g;
	std::vector< tbb::flow::continue_node< tbb::flow::continue_msg >* > allNodes;
	for ( unsigned int algIndex = 0; algIndex < m_algoNames.size(); ++algIndex )
	{
		//Create a node for each algorithm
		allNodes.push_back( new tbb::flow::continue_node< tbb::flow::continue_msg>
				( g, [&,algIndex]( const tbb::flow::continue_msg & )
				  {
				    auto name = m_algoNames[ algIndex ];

				    //Now get control
				    IAlgorithm * ialgoPtr = nullptr;
				    StatusCode testPool = m_algResourcePool->acquireAlgorithm( name, ialgoPtr, false ); //true = blocking, let's see what happens
				    if ( !testPool.isSuccess() )
				    {
				      //return;
				      std::cout << "re-execute " << name << std::endl;
				      this->add_to_depth(-1);
				      this->recycle_to_reexecute();
				      return ;//0;
				    }
				    Algorithm * algoPtr = dynamic_cast< Algorithm* >( ialgoPtr );
				    if ( !algoPtr ) return ;//0;
				    
				    //Retrieve old context
				    //auto oldContext = algoPtr->getContext();

				    //Run algorithm in context provided
				    algoPtr->setContext( m_eventContext );
				    algoPtr->sysExecute();

				    //Return the algorithm context to what it was before
				    //if ( oldContext )
				    //{
				    //  algoPtr->setContext( oldContext );
				    //}

				    //Let go of the alg again
				    testPool = m_algResourcePool->releaseAlgorithm( name, ialgoPtr );

				    //return tbb::flow::continue_msg();
				  }
				) );

		//Connect 2nd node to 1st, and so on
		if ( algIndex )
		{
			tbb::flow::make_edge( *allNodes[ algIndex - 1 ], *allNodes[ algIndex ] );
		}
	}

	allNodes[0]->try_put( tbb::flow::continue_msg() );
	g.wait_for_all();*/
}
