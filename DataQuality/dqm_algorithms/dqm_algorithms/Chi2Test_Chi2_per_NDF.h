/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file Chi2Test_Chi2_per_NDF.h file declares the dqm_algorithms::Chi2Test_Chi2_per_NDF  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_CHI2TEST_CHI2_PER_NDF_H
#define DQM_ALGORITHMS_CHI2TEST_CHI2_PER_NDF_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/Chi2Test.h>

namespace dqm_algorithms
{
	struct Chi2Test_Chi2_per_NDF : public Chi2Test
        {
	  Chi2Test_Chi2_per_NDF(): Chi2Test("Chi2_per_NDF") {};

	};
}

#endif // DQM_ALGORITHMS_CHI2TEST_CHI2_PER_NDF_H
