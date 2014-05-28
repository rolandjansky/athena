/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file Bins_Diff_FromAvg.h file declares the dqm_algorithms::BinContentComp  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_BINS_DIFF_FROMAVG_H
#define DQM_ALGORITHMS_BINS_DIFF_FROMAVG_H

#include <dqm_core/Algorithm.h>

namespace dqm_algorithms
{
	struct Bins_Diff_FromAvg : public dqm_core::Algorithm
        {
	  Bins_Diff_FromAvg();
	  
	  ~Bins_Diff_FromAvg();

	    //overwrites virtual functions
	  Bins_Diff_FromAvg * clone( );
	  dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
          using dqm_core::Algorithm::printDescription;
	  void  printDescription(std::ostream& out);
	};
}

#endif // DQM_ALGORITHMS_BINS_DIFF_FROMAVG_H
