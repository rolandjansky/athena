/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file Chi2Test_Prob.h file declares the dqm_algorithms::Chi2Test_Prob  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_CHI2TEST_PROB_H
#define DQM_ALGORITHMS_CHI2TEST_PROB_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/Chi2Test.h>

namespace dqm_algorithms
{
	struct Chi2Test_Prob : public Chi2Test
        {
	  Chi2Test_Prob(): Chi2Test("Prob") {};

	};
}

#endif // DQM_ALGORITHMS_CHI2TEST_PROB_H
