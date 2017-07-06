/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file BasicGraphCheck.cxx checks basic histogram quanitites like over/underflow; if filled and returns dqm_core::Result
 * \author Peter Onyisi, following Haleh Hadavand
 */

#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/BasicGraphCheck.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <TGraph.h>
#include <TClass.h>
#include <ers/ers.h>

#include <dqm_core/AlgorithmManager.h>

namespace
{
  dqm_algorithms::BasicGraphCheck GraphFilling( "Graph_Not_Empty" );
  dqm_algorithms::BasicGraphCheck GraphEmpty( "Graph_Empty" );

}


dqm_algorithms::BasicGraphCheck::BasicGraphCheck( const std::string & name )
  : m_name( name )
{
  dqm_core::AlgorithmManager::instance().registerAlgorithm(name, this);
}

dqm_algorithms::BasicGraphCheck * 
dqm_algorithms::BasicGraphCheck::clone()
{
  
  return new BasicGraphCheck( m_name );
}


dqm_core::Result *
dqm_algorithms::BasicGraphCheck::execute(	const std::string & name , 
						const TObject & object, 
                                                const dqm_core::AlgorithmConfig & config)
{  
  TGraph * graph;
  
  if( object.IsA()->InheritsFrom( "TGraph" ) ) {
    graph = (TGraph*)&object;
  } else {
    throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TGraph" );
  }

  double minstat = dqm_algorithms::tools::GetFirstFromMap( "MinStat", config.getParameters(), -1 );
  
  if (graph->GetN() < minstat ) {
    dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
    result->tags_["InsufficientN"] = graph->GetN();
    return result;
  }
  
  if (m_name == "Graph_Not_Empty") {
    if (graph->GetN() != 0) {
      ERS_DEBUG(1, "Graph " <<graph->GetName()<<" is Not Empty");
      return new dqm_core::Result(dqm_core::Result::Green);
    }else {
      ERS_DEBUG(1, "Graph " <<graph->GetName()<<" is Empty");
      return new dqm_core::Result(dqm_core::Result::Red);
    }
  } else if (m_name == "Graph_Empty") {
    if (graph->GetN() == 0) {
      ERS_DEBUG(1, "Graph " <<graph->GetName()<<" is Empty");
      return new dqm_core::Result(dqm_core::Result::Green);
    }else {
      ERS_DEBUG(1, "Graph " <<graph->GetName()<<" is Not Empty");
      return new dqm_core::Result(dqm_core::Result::Red);
    }
  } else {
    return new dqm_core::Result();
  }
  
  
}

void
dqm_algorithms::BasicGraphCheck::printDescription(std::ostream& out)
{
  if ( m_name == "Graph_Not_Empty"){
    out<<"Graph_Not_Empty: Checks that graph is not empty\n"<<std::endl;
  } else if ( m_name == "Graph_Empty"){
    out<<"Graph_Empty: Checks that graph is empty\n"<<std::endl;
  }
  
  out<<"Optional Parameter: MinStat: Minimum graph statistics needed to perform Algorithm\n"<<std::endl;
}

