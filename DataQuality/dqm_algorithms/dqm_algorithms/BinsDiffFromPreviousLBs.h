/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/*! \file BinsDiffFromPreviousLBs.cxx evaluates the number of consecutive bins with bin value different from set threshold. Also usable for
 single bins.                                                                                                                             
 * \author Giulia Ucchielli                                                                                                              
*/

#ifndef DQM_ALGORITHMS_BINS_DIFF_FROMPREVLBS_H
#define DQM_ALGORITHMS_BINS_DIFF_FROMPREVLBS_H

#include <dqm_core/Algorithm.h>
#include <iosfwd>
#include <vector>
#include <string>

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
	  bool areConsecutive(std::vector<int>, int elements);
	};
}

#endif // DQM_ALGORITHMS_BINS_DIFF_FROMPREVLBS_H
