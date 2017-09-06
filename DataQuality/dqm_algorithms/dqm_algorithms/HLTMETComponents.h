/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*  Algorithm: dqm_algorithms::HLTMETComponents 
 *  Author   : Venkatesh Kaushik <venkat.kaushik@cern.ch>
 *  Date     : Mar 6 2010
 */


#ifndef DQM_ALGORITHMS_HLTMETCOMPONENTS_H
#define DQM_ALGORITHMS_HLTMETCOMPONENTS_H

#include <dqm_core/Algorithm.h>

namespace dqm_algorithms
{
	struct HLTMETComponents : public dqm_core::Algorithm
        {
	  HLTMETComponents();

	    //overwrites virtual functions
	  HLTMETComponents * clone( );
	  dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
          using dqm_core::Algorithm::printDescription;
	  void  printDescription(std::ostream& out);
	};
}

#endif // DQM_ALGORITHMS_HLTMETCOMPONENTS_H
