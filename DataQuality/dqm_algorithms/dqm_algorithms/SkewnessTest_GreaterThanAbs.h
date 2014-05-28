/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file SkewnessTest_GreaterThanAbs.h file declares the dqm_algorithms::SkewnessTest_GreaterThanAbs  class.
 * \author andrea.dotti@cern.ch
*/

#ifndef DQM_ALGORITHMS_SKEWNESSTEST_GREATERTHANABS_H
#define DQM_ALGORITHMS_SKEWNESSTEST_GREATERTHANABS_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/SkewnessTest.h>

namespace dqm_algorithms
{
	struct SkewnessTest_GreaterThanAbs : public SkewnessTest
        {
	  SkewnessTest_GreaterThanAbs(): SkewnessTest("GreaterThanAbs") {};

	};
}

#endif // DQM_ALGORITHMS_SKEWNESSTEST_GREATERTHANABS_H
