/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file BinsSymmetric.h file declares the dqm_algorithms::BinsSymmetric  class.
 * \author Michele Petteni
*/

#ifndef DQM_ALGORITHMS_BINSSYMMETRIC_H
#define DQM_ALGORITHMS_BINSSYMMETRIC_H

#include <dqm_core/Algorithm.h>

namespace dqm_algorithms
{
	struct BinsSymmetric : public dqm_core::Algorithm
        {
	  BinsSymmetric();

	  ~BinsSymmetric();

	    //overwrites virtual functions
	  BinsSymmetric * clone( );
	  dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
          using dqm_core::Algorithm::printDescription;
	  void  printDescription(std::ostream& out);
	  
	};
}

#endif // DQM_ALGORITHMS_BINSSYMMETRIC_H
