/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file SkewnessTest_LessThanAbs.h file declares the dqm_algorithms::SkewnessTest_LessThanAbs  class.
 * \author andrea.dotti@cern.ch
*/

#ifndef DQM_ALGORITHMS_SKEWNESSTEST_LESSTHANABS_H
#define DQM_ALGORITHMS_SKEWNESSTEST_LESSTHANABS_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/SkewnessTest.h>

namespace dqm_algorithms
{
	struct SkewnessTest_LessThanAbs : public SkewnessTest
        {
	  SkewnessTest_LessThanAbs(): SkewnessTest("LessThanAbs") {};

	};
}

#endif // DQM_ALGORITHMS_SKEWNESSTEST_LESSTHANABS_H
