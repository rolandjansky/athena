/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file BinContentComp.h file declares the dqm_algorithms::BinContentComp  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_BINCONTENTCOMP_H
#define DQM_ALGORITHMS_BINCONTENTCOMP_H

#include <dqm_core/Algorithm.h>

namespace dqm_algorithms
{
	struct BinContentComp : public dqm_core::Algorithm
        {
	  BinContentComp();

	  ~BinContentComp();

	    //overwrites virtual functions
	  BinContentComp * clone( );
	  dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
          using dqm_core::Algorithm::printDescription;
	  void  printDescription(std::ostream& out);
	};
}

#endif // DQM_ALGORITHMS_BINCONTENTCOMP_H
