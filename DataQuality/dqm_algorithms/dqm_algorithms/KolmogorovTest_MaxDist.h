/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file KolmogorovTest_MaxDist.h file declares the dqm_algorithms::KolmogorovTest_MaxDist  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_KOLMOGOROVTEST_MAXDIST_H
#define DQM_ALGORITHMS_KOLMOGOROVTEST_MAXDIST_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/KolmogorovTest.h>

namespace dqm_algorithms
{
	struct KolmogorovTest_MaxDist : public KolmogorovTest
        {
	  KolmogorovTest_MaxDist(): KolmogorovTest("MaxDist") {};

	};
}

#endif // DQM_ALGORITHMS_KOLMOGOROVTEST_MAXDIST_H
