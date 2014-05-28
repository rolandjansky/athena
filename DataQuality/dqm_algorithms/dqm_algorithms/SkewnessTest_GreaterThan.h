/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file SkewnessTest_GreaterThan.h file declares the dqm_algorithms::SkewnessTest_GreaterThan  class.
 * \author andrea.dotti@cern.ch
*/

#ifndef DQM_ALGORITHMS_SKEWNESSTEST_GREATERTHAN_H
#define DQM_ALGORITHMS_SKEWNESSTEST_GREATERTHAN_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/SkewnessTest.h>

namespace dqm_algorithms
{
	struct SkewnessTest_GreaterThan : public SkewnessTest
        {
	  SkewnessTest_GreaterThan(): SkewnessTest("GreaterThan") {};

	};
}

#endif // DQM_ALGORITHMS_SKEWNESSTEST_GREATERTHAN_H
