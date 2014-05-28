/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file KurtosisTest_GreaterThanAbs.h file declares the dqm_algorithms::KurtosisTest_GreaterThanAbs  class.
 * \author andrea.dotti@cern.ch
*/

#ifndef DQM_ALGORITHMS_KURTOSISTEST_GREATERTHANABS_H
#define DQM_ALGORITHMS_KURTOSISTEST_GREATERTHANABS_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/KurtosisTest.h>

namespace dqm_algorithms
{
	struct KurtosisTest_GreaterThanAbs : public KurtosisTest
        {
	  KurtosisTest_GreaterThanAbs(): KurtosisTest("GreaterThanAbs") {};

	};
}

#endif // DQM_ALGORITHMS_KURTOSISTEST_GREATERTHANABS_H
