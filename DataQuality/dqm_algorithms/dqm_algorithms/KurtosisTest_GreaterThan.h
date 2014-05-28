/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file KurtosisTest_GreaterThan.h file declares the dqm_algorithms::KurtosisTest_GreaterThan  class.
 * \author andrea.dotti@cern.ch
*/

#ifndef DQM_ALGORITHMS_KURTOSISTEST_GREATERTHAN_H
#define DQM_ALGORITHMS_KURTOSISTEST_GREATERTHAN_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/KurtosisTest.h>

namespace dqm_algorithms
{
	struct KurtosisTest_GreaterThan : public KurtosisTest
        {
	  KurtosisTest_GreaterThan(): KurtosisTest("GreaterThan") {};

	};
}

#endif // DQM_ALGORITHMS_KURTOSISTEST_GREATERTHAN_H
