/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file BinsLessThanThreshold.h file declares the dqm_algorithms::BinsLessThanThreshold  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_BINS_LESSTHAN_THRESHOLD_H
#define DQM_ALGORITHMS_BINS_LESSTHAN_THRESHOLD_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/BinThreshold.h>

namespace dqm_algorithms
{
	struct Bins_LessThan_Threshold : public BinThreshold
        {
	  Bins_LessThan_Threshold(): BinThreshold("LessThan") {};

	};
}

#endif // DQM_ALGORITHMS_BINS_LESSTHAN_THRESHOLD_H
