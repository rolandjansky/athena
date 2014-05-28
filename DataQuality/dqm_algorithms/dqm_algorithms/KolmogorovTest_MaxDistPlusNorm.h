/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file KolmogorovTest_MaxDistPlusNorm.h file declares the dqm_algorithms::KolmogorovTest_MaxDistPlusNorm  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_KOLMOGOROVTEST_MAXDISTPLUSNORM_H
#define DQM_ALGORITHMS_KOLMOGOROVTEST_MAXDISTPLUSNORM_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/KolmogorovTest.h>

namespace dqm_algorithms
{
	struct KolmogorovTest_MaxDistPlusNorm : public KolmogorovTest
        {
	  KolmogorovTest_MaxDistPlusNorm(): KolmogorovTest("MaxDistPlusNorm") {};

	};
}

#endif // DQM_ALGORITHMS_KOLMOGOROVTEST_MAXDISTPLUSNORM_H
