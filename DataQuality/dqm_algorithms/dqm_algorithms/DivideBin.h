/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/*! \file DivideBin.h file declares the dqm_algorithms::DivideBin  class.
 * \author Martin White
*/

#ifndef DQM_ALGORITHMS_DIVIDEBIN_H
#define DQM_ALGORITHMS_DIVIDEBIN_H

#include <dqm_core/Algorithm.h>
#include <string>
#include <iosfwd>

namespace dqm_algorithms
{
	struct DivideBin : public dqm_core::Algorithm
        {
	  DivideBin();

	    //overwrites virtual functions
	  DivideBin * clone( );
	  dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
          using dqm_core::Algorithm::printDescription;
	  void  printDescription(std::ostream& out);
};
}

#endif // DQM_ALGORITHMS_DIVIDEBIN_H
