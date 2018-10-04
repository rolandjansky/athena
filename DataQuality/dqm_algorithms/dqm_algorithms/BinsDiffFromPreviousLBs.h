/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file BinsDiffFromPreviousLBs.h file declares the dqm_algorithms::BinContentComp  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_BINS_DIFF_FROMPREVLBS_H
#define DQM_ALGORITHMS_BINS_DIFF_FROMPREVLBS_H

#include <dqm_core/Algorithm.h>

namespace dqm_algorithms
{
	struct BinsDiffFromPreviousLBs : public dqm_core::Algorithm
        {
	  BinsDiffFromPreviousLBs();
	  
	  ~BinsDiffFromPreviousLBs();

	    //overwrites virtual functions
	  BinsDiffFromPreviousLBs * clone( );
	  dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
          using dqm_core::Algorithm::printDescription;
	  void  printDescription(std::ostream& out);
	  int getMax(std::vector<int>, int max);
	  int getMin(std::vector<int>, int min);
	  bool areConsecutive(std::vector<int>, int elements);
	};
}

#endif // DQM_ALGORITHMS_BINS_DIFF_FROMPREVLBS_H
