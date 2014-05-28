/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file Bins_Diff_FromAvg.h file declares the dqm_algorithms::BinContentComp  class.
 * \author Evan Wulf
*/

#ifndef DQM_ALGORITHMS_PASSINPUT_H
#define DQM_ALGORITHMS_PASSINPUT_H

#include <dqm_core/Algorithm.h>

namespace dqm_algorithms
{
	struct PassInput : public dqm_core::Algorithm
        {
	  PassInput();
	  
	  ~PassInput();

	    //overwrites virtual functions
	  PassInput * clone( );
	  dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
	  using dqm_core::Algorithm::printDescription;
	  void  printDescription(std::ostream& out);
	};
}

#endif // DQM_ALGORITHMS_PASSINPUT_H
