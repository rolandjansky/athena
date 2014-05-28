/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file BinsDiffByStrips.h file declares the dqm_algorithms::BinContentComp  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_BINSDIFFBYSTRIPS_H
#define DQM_ALGORITHMS_BINSDIFFBYSTRIPS_H

#include <dqm_core/Algorithm.h>

namespace dqm_algorithms
{
	struct BinsDiffByStrips : public dqm_core::Algorithm
        {
	  BinsDiffByStrips();
	  
	  ~BinsDiffByStrips();

	    //overwrites virtual functions
	  BinsDiffByStrips * clone( );
	  dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
          using dqm_core::Algorithm::printDescription;
	  void  printDescription(std::ostream& out);
          void find_n(std::string, int&);
	};
}

#endif // DQM_ALGORITHMS_BINSDIFFBYSTRIPS_H
