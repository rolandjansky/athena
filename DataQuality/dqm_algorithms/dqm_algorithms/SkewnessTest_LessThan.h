/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file SkewnessTest_LessThan.h file declares the dqm_algorithms::SkewnessTest_LessThan  class.
 * \author andrea.dotti@cern.ch
*/

#ifndef DQM_ALGORITHMS_SKEWNESSTEST_LESSTHAN_H
#define DQM_ALGORITHMS_SKEWNESSTEST_LESSTHAN_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/SkewnessTest.h>

namespace dqm_algorithms
{
	struct SkewnessTest_LessThan : public SkewnessTest
        {
	  SkewnessTest_LessThan(): SkewnessTest("LessThan") {};

	};
}

#endif // DQM_ALGORITHMS_SKEWNESSTEST_LESSTHAN_H
