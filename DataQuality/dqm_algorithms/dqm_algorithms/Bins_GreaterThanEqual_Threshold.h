/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file Bins_GreaterThanEqual_Threshold.h file declares the dqm_algorithms::Bins_GreaterThanEqual_Threshold  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_BINS_GREATERTHANEQUAL_THRESHOLD_H
#define DQM_ALGORITHMS_BINS_GREATERTHANEQUAL_THRESHOLD_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/BinThreshold.h>

namespace dqm_algorithms
{
	struct Bins_GreaterThanEqual_Threshold : public BinThreshold
        {
	  Bins_GreaterThanEqual_Threshold(): BinThreshold("GreaterThanEqual") {};

	};
}

#endif // DQM_ALGORITHMS_BINS_GREATERTHANEQUAL_THRESHOLD_H
