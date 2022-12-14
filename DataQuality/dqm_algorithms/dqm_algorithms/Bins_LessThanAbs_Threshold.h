/*
  Copyright (C) 2002-2022CERN for the benefit of the ATLAS collaboration
*/

/*! \file BinsLessThanThreshold.h file declares the dqm_algorithms::BinsLessThanThreshold  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_BINS_LESSTHANABS_THRESHOLD_H
#define DQM_ALGORITHMS_BINS_LESSTHANABS_THRESHOLD_H

#include <dqm_algorithms/BinThreshold.h>

namespace dqm_algorithms
{
	struct Bins_LessThanAbs_Threshold : public BinThreshold
        {
	  Bins_LessThanAbs_Threshold(): BinThreshold("LessThanAbs") {};

	};
}

#endif // DQM_ALGORITHMS_BINS_LESSTHANABS_THRESHOLD_H
