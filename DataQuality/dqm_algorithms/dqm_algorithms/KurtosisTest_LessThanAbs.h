/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file KurtosisTest_LessThanAbs.h file declares the dqm_algorithms::KurtosisTest_LessThanAbs  class.
 * \author andrea.dotti@cern.ch
*/

#ifndef DQM_ALGORITHMS_KURTOSISTEST_LESSTHANABS_H
#define DQM_ALGORITHMS_KURTOSISTEST_LESSTHANABS_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/KurtosisTest.h>

namespace dqm_algorithms
{
	struct KurtosisTest_LessThanAbs : public KurtosisTest
        {
	  KurtosisTest_LessThanAbs(): KurtosisTest("LessThanAbs") {};

	};
}

#endif // DQM_ALGORITHMS_KURTOSISTEST_LESSTHANABS_H
