/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file Bins_GreaterThanAbs_Threshold.h file declares the dqm_algorithms::Bins_GreaterThanAbs_Threshold  class.
 * \author Evan Wulf
*/

#ifndef DQM_ALGORITHMS_BINS_GREATERTHANABS_THRESHOLD_H
#define DQM_ALGORITHMS_BINS_GREATERTHANABS_THRESHOLD_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/BinThreshold.h>

namespace dqm_algorithms
{
	struct Bins_GreaterThanAbs_Threshold : public BinThreshold
        {
	  Bins_GreaterThanAbs_Threshold(): BinThreshold("GreaterThanAbs") {};

	};
}

#endif // DQM_ALGORITHMS_BINS_GREATERTHANABS_THRESHOLD_H
