/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file CSCNoisyDead.h file declares the dqm_algorithms::CSCNoisyDead class.
 * \author J.Veatch
*/

#ifndef DQM_ALGORITHMS_CSCNOISYDEAD_H
#define DQM_ALGORITHMS_CSCNOISYDEAD_H

#include <dqm_core/Algorithm.h>

namespace dqm_algorithms
{
	struct CSCNoisyDead : public dqm_core::Algorithm
        {
	  CSCNoisyDead();

	  ~CSCNoisyDead();

	    //overwrites virtual functions
	  CSCNoisyDead * clone( );
	  dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
          using dqm_core::Algorithm::printDescription;
	  void  printDescription(std::ostream& out);
	};
}

#endif // DQM_ALGORITHMS_CSCNOISYDEAD_H
