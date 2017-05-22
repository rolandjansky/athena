/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DQM_ALGORITHMS_MAXIMUMBIN_H
#define DQM_ALGORITHMS_MAXIMUMBIN_H

#include <dqm_core/Algorithm.h>

namespace dqm_algorithms
{
	class MaximumBin : public dqm_core::Algorithm
  {
    public:
	  MaximumBin();

    //overwrite virtual functions
	  MaximumBin * clone( );
	  dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
    using dqm_core::Algorithm::printDescription;
	  void  printDescription(std::ostream& out);
	};
}

#endif // DQM_ALGORITHMS_MAXIMUMBIN_H
