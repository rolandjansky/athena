/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file BinsGreaterThanThreshold.h file declares the dqm_algorithms::BinsGreaterThanThreshold  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_BINS_GREATERTHAN_THRESHOLD_H
#define DQM_ALGORITHMS_BINS_GREATERTHAN_THRESHOLD_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/BinThreshold.h>

namespace dqm_algorithms
{
	struct Bins_GreaterThan_Threshold : public BinThreshold
        {
	  Bins_GreaterThan_Threshold(): BinThreshold("GreaterThan") {};

	};
}

#endif // DQM_ALGORITHMS_BINS_GREATERTHAN_THRESHOLD_H
