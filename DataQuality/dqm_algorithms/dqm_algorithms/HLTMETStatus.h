/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*  Algorithm: dqm_algorithms::HLTMETStatus 
 *  Author   : Venkatesh Kaushik <venkat.kaushik@cern.ch>
 *  Date     : Feb 21 2010
 */


#ifndef DQM_ALGORITHMS_HLTMETSTATUS_H
#define DQM_ALGORITHMS_HLTMETSTATUS_H

#include <dqm_core/Algorithm.h>

namespace dqm_algorithms
{
	struct HLTMETStatus : public dqm_core::Algorithm
        {
	  HLTMETStatus();

	    //overwrites virtual functions
	  HLTMETStatus * clone( );
	  dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
	  using dqm_core::Algorithm::printDescription;
	  void  printDescription(std::ostream& out);
 
	  private:
	  std::string m_name;
	};
}

#endif // DQM_ALGORITHMS_HLTMETSTATUS_H
