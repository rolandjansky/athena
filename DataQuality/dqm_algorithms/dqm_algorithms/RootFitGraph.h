/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/*! \file RootFitGraph.h file declares the dqm_algorithms::RootFitGraph  class.
 * \author Akimasa Ishikawa (akimasa.ishikawa@cern.ch) 15th Apr 2010
*/

#ifndef DQM_ALGORITHMS_ROOTFITGRAPH_H
#define DQM_ALGORITHMS_ROOTFITGRAPH_H

#include <dqm_core/Algorithm.h>
#include <string>
#include <memory>
#include <iosfwd>

class TF1;

namespace dqm_algorithms
{
  struct RootFitGraph : public dqm_core::Algorithm
  {
    RootFitGraph( const std::string & name );
	  
    ~RootFitGraph();
    RootFitGraph * clone( );
    dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
    using dqm_core::Algorithm::printDescription;
    void  printDescription(std::ostream& out);             
    private:
    std::string m_name;
    std::unique_ptr<TF1> m_func;	
  };
}

#endif // DQM_ALGORITHMS_ROOTFITGRAPH_H
