/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file KolmogorovTest_Norm.h file declares the dqm_algorithms::KolmogorovTest_Norm  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_KOLMOGOROVTEST_NORM_H
#define DQM_ALGORITHMS_KOLMOGOROVTEST_NORM_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/KolmogorovTest.h>

namespace dqm_algorithms
{
	struct KolmogorovTest_Norm : public KolmogorovTest
        {
	  KolmogorovTest_Norm(): KolmogorovTest("Norm") {};

	};
}

#endif // DQM_ALGORITHMS_KOLMOGOROVTEST_NORM_H
