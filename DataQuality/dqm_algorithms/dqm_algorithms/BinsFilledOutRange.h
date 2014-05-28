/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file BinsFilledOutRange.h file declares the dqm_algorithms::BinsFilledOutRange  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_BINSFILLEDOUTRANGE_H
#define DQM_ALGORITHMS_BINSFILLEDOUTRANGE_H

#include <dqm_core/Algorithm.h>

namespace dqm_algorithms
{
	struct BinsFilledOutRange : public dqm_core::Algorithm
        {
	  BinsFilledOutRange();

	  ~BinsFilledOutRange();

	    //overwrites virtual functions
	  BinsFilledOutRange * clone( );
	  dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
          using dqm_core::Algorithm::printDescription;
	  void  printDescription(std::ostream& out);
	};
}

#endif // DQM_ALGORITHMS_BINSFILLEDOUTRANGE_H
