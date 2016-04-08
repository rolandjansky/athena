// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHELLOWORLD_HELLOGRAPHALG_H
#define ATHEXHELLOWORLD_HELLOGRAPHALG_H 1

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include <string>

#include "tbb/flow_graph.h"
#include "DataModel/DataVector.h"

/////////////////////////////////////////////////////////////////////////////
// derived from http://threadingbuildingblocks.org/docs/help/reference/flow_graph/message_flow_graph_example.htm
class IHelloTool;

class HelloGraphAlg : public AthAlgorithm {
public:
  HelloGraphAlg( const std::string& name, ISvcLocator* pSvcLocator );
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  StatusCode beginRun();
  StatusCode endRun();
  
private:
  size_t m_nIters;  ///property
  size_t m_concurrency; ///property

  ToolHandle< IHelloTool > m_myPrivateHelloTool;
  ToolHandle< IHelloTool > m_myPublicHelloTool;
  
  tbb::flow::graph m_graph;
  tbb::flow::broadcast_node<int> m_input;
  DataVector<tbb::flow::graph_node> m_nodes;
};

#endif // ATHEXHELLOWORLD_HELLOGRAPHALG_H
