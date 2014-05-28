/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file BinsLessThanEqualThreshold.h file declares the dqm_algorithms::BinsLessThanEqualThreshold  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_BINS_LESSTHANEQUAL_THRESHOLD_H
#define DQM_ALGORITHMS_BINS_LESSTHANEQUAL_THRESHOLD_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/BinThreshold.h>

namespace dqm_algorithms
{
	struct Bins_LessThanEqual_Threshold : public BinThreshold
        {
	  Bins_LessThanEqual_Threshold(): BinThreshold("LessThanEqual") {};

	};
}

#endif // DQM_ALGORITHMS_BINS_LESSTHANEQUAL_THRESHOLD_H
