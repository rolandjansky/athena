/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file BinsEqualThreshold.h file declares the dqm_algorithms::BinsEqualThreshold  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_BINS_EQUAL_THRESHOLD_H
#define DQM_ALGORITHMS_BINS_EQUAL_THRESHOLD_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/BinThreshold.h>

namespace dqm_algorithms
{
	struct Bins_Equal_Threshold : public BinThreshold
        {
	  Bins_Equal_Threshold(): BinThreshold("Equal") {};

	};
}

#endif // DQM_ALGORITHMS_BINS_EQUAL_THRESHOLD_H
