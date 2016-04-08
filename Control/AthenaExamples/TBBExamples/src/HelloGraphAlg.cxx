/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBBExamples/IHelloTool.h"
#include "HelloGraphAlg.h"
#include "AthenaKernel/CloneTool.h"
#include <boost/bind.hpp>
#include <tuple>
using namespace std;
using namespace boost;
using namespace tbb::flow;

HelloGraphAlg::HelloGraphAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), m_nIters(0), m_concurrency(1),
  m_myPrivateHelloTool("HelloTool",this), m_myPublicHelloTool("HelloTool"), m_graph(), m_input(m_graph)
{
  
  // Part 2: Declare the properties
  declareProperty("NIters", m_nIters, "number of iterations to feed to the graph");

  declareProperty("Concurrency", m_concurrency, "number of tasks to execute concurrently in the functional part of the graph: 0=unlimited , 1=serial (default), N tasks");

  declareProperty("MyPrivateHelloTool", m_myPrivateHelloTool, "private IHelloTool");
  declareProperty("MyPublicHelloTool", m_myPublicHelloTool, "public, shared IHelloTool");

}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
class Summer {
private:
  int m_sum;
public:
  Summer() : m_sum(0) {}
  int operator()(const std::tuple<int,int,int>& res) {
    m_sum += get<0>(res) + get<1>(res) + get<2>(res);
    cout << "this iteration " 
	 << get<0>(res) + get<1>(res) + get<2>(res) << endl;
    return m_sum;
  }
};

void printGraphResult(int res) {
  cout << "accumulated graph result " << res << endl;
}

StatusCode HelloGraphAlg::initialize() {

  // Part 1: print where you are
  ATH_MSG_INFO ("initialize()");

  ATH_MSG_INFO 
    ("  " << m_myPrivateHelloTool.propertyName() 
     << " = " << m_myPrivateHelloTool.type() 
     << endreq
     << "  " << m_myPublicHelloTool.propertyName() 
     << " = " << m_myPublicHelloTool.type());


  // Part 3: Retrieve the tools using the ToolHandles
  if ( m_myPrivateHelloTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL 
      (m_myPrivateHelloTool.propertyName() << ": Failed to retrieve tool "
       << m_myPrivateHelloTool.type());
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO 
      (m_myPrivateHelloTool.propertyName() << ": Retrieved tool " 
       << m_myPrivateHelloTool.type());
  }
  if ( m_myPublicHelloTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL 
      (m_myPublicHelloTool.propertyName() << ": Failed to retrieve tool "
       << m_myPublicHelloTool);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO (m_myPublicHelloTool.propertyName() << ": Retrieved tool "
		  << m_myPublicHelloTool.type());
  }

  IHelloTool* clonedPrivate(0);
  if(!(CloneTool::clone(*m_myPrivateHelloTool, "clonedPrivate", clonedPrivate).isSuccess())) {
    ATH_MSG_FATAL("Error cloning " << m_myPrivateHelloTool.name());
    return StatusCode::FAILURE;
  }
  IHelloTool* clonedPublic(0);
  if(!(CloneTool::clone(*m_myPublicHelloTool, "clonedPublic", clonedPublic).isSuccess())) {
    ATH_MSG_FATAL("Error cloning " << m_myPublicHelloTool.name());
    return StatusCode::FAILURE;
  }

  function_node<int,int>* pAdder =
    new function_node<int,int>(m_graph, m_concurrency, 
			       bind(&IHelloTool::addToIt, &*m_myPublicHelloTool, _1));
  m_nodes.push_back(pAdder);

  function_node<int,int>* pAdder2 =
    new function_node<int,int>(m_graph, m_concurrency, 
			       bind(&IHelloTool::addToIt, clonedPublic, _1));
  m_nodes.push_back(pAdder2);

  function_node<int,int>* pSubtracter =
    new function_node<int,int>(m_graph, m_concurrency, 
			       bind(&IHelloTool::addToIt, &*m_myPrivateHelloTool, _1));
  m_nodes.push_back(pSubtracter);

  function_node<int,int>* pSubtracter2 =
    new function_node<int,int>(m_graph, m_concurrency, 
			       bind(&IHelloTool::addToIt, clonedPrivate, _1));
  m_nodes.push_back(pSubtracter2);
  
  join_node< std::tuple<int,int,int>, queueing >* pJoiner =
    new join_node< std::tuple<int,int,int>, queueing >(m_graph);
  m_nodes.push_back(pJoiner);

  function_node< std::tuple<int,int,int>, int >* pSummer =
    new function_node< std::tuple<int,int,int>, int >(m_graph, serial, Summer());
  m_nodes.push_back(pSummer);

  function_node< int >* pPrinter = 
    new function_node< int >(m_graph, serial, bind(&printGraphResult,_1));
  m_nodes.push_back(pPrinter);

  make_edge(m_input, *pAdder);
  make_edge(m_input, *pAdder2);
  make_edge(m_input, *pSubtracter);
  make_edge(*pSubtracter, *pSubtracter2);
  make_edge(*pAdder, input_port<0>(*pJoiner));
  make_edge(*pAdder2, input_port<1>(*pJoiner));
  make_edge(*pSubtracter2, input_port<2>(*pJoiner));
  make_edge(*pJoiner, *pSummer);
  make_edge(*pSummer, *pPrinter);

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode HelloGraphAlg::execute() {
  // Part 1: print where you are
  ATH_MSG_INFO ("execute()");

  for (size_t i=0; i<m_nIters; ++i) m_input.try_put(i);
  m_graph.wait_for_all();
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode HelloGraphAlg::finalize() {

  // Part 1: print where you are
  ATH_MSG_INFO ("finalize()");
  
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode HelloGraphAlg::beginRun() {
  // Part 1: print where you are
  ATH_MSG_INFO ("beginRun()");

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode HelloGraphAlg::endRun() {
  // Part 1: print where you are
  ATH_MSG_INFO ("endRun()");

  return StatusCode::SUCCESS;
}
