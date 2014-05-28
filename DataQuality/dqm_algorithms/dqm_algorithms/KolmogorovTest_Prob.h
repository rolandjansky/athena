/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file KolmogorovTest_Prob.h file declares the dqm_algorithms::KolmogorovTest_Prob  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_KOLMOGOROVTEST_PROB_H
#define DQM_ALGORITHMS_KOLMOGOROVTEST_PROB_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/KolmogorovTest.h>

namespace dqm_algorithms
{
	struct KolmogorovTest_Prob : public KolmogorovTest
        {
	  KolmogorovTest_Prob(): KolmogorovTest("Prob") {};

	};
}

#endif // DQM_ALGORITHMS_KOLMOGOROVTEST_PROB_H
