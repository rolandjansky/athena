/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/*! \file BinHeight_LessThanEqual_Threshold.h file declares the dqm_algorithms::BinHeight_LessThanEqual_Threshold  class.
 * \author Margherita Spalla, following what's done in BinsGreaterThanThreshold.h
*/

#ifndef DQM_ALGORITHMS_BINHEIGHT_LESSTHANEQUAL_THRESHOLD_H
#define DQM_ALGORITHMS_BINHEIGHT_LESSTHANEQUAL_THRESHOLD_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/BinHeightThreshold.h>

namespace dqm_algorithms
{
	struct BinHeight_LessThanEqual_Threshold : public BinHeightThreshold
        {
	  BinHeight_LessThanEqual_Threshold(): BinHeightThreshold("LessThanEqual") {};

	};
}

#endif // DQM_ALGORITHMS_BINHEIGHT_LESSTHANEQUAL_THRESHOLD_H
