/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/*! \file BinHeight_GreaterThan_Threshold.h file declares the dqm_algorithms::BinHeight_GreaterThan_Threshold  class.
 * \author Margherita Spalla, following what's done in BinsGreaterThanThreshold.h
*/

#ifndef DQM_ALGORITHMS_BINHEIGHT_GREATERTHAN_THRESHOLD_H
#define DQM_ALGORITHMS_BINHEIGHT_GREATERTHAN_THRESHOLD_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/BinHeightThreshold.h>

namespace dqm_algorithms
{
	struct BinHeight_GreaterThan_Threshold : public BinHeightThreshold
        {
	  BinHeight_GreaterThan_Threshold(): BinHeightThreshold("GreaterThan") {};

	};
}

#endif // DQM_ALGORITHMS_BINHEIGHT_GREATERTHAN_THRESHOLD_H
