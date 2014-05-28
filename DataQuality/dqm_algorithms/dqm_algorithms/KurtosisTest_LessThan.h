/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file KurtosisTest_LessThan.h file declares the dqm_algorithms::KurtosisTest_LessThan  class.
 * \author andrea.dotti@cern.ch
*/

#ifndef DQM_ALGORITHMS_KURTOSISTEST_LESSTHAN_H
#define DQM_ALGORITHMS_KURTOSISTEST_LESSTHAN_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/KurtosisTest.h>

namespace dqm_algorithms
{
	struct KurtosisTest_LessThan : public KurtosisTest
        {
	  KurtosisTest_LessThan(): KurtosisTest("LessThan") {};

	};
}

#endif // DQM_ALGORITHMS_KURTOSISTEST_LESSTHAN_H
