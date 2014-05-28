/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file BinsNotEqualThreshold.h file declares the dqm_algorithms::BinsNotEqualThreshold  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_BINS_NOTEQUAL_THRESHOLD_H
#define DQM_ALGORITHMS_BINS_NOTEQUAL_THRESHOLD_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/BinThreshold.h>

namespace dqm_algorithms
{
	struct Bins_NotEqual_Threshold : public BinThreshold
        {
	  Bins_NotEqual_Threshold(): BinThreshold("NotEqual") {};

	};
}

#endif // DQM_ALGORITHMS_BINS_NOTEQUAL_THRESHOLD_H
