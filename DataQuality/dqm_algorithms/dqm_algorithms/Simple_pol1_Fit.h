/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file Simple_pol1_Fit.h file declares the dqm_algorithms::Simple_pol1_Fit  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_SIMPLE_POL1_FIT_H
#define DQM_ALGORITHMS_SIMPLE_POL1_FIT_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/RootFit.h>

namespace dqm_algorithms
{
	struct Simple_pol1_Fit : public RootFit
        {
	     Simple_pol1_Fit():  RootFit("pol1")  {};

              
	};

}

#endif // DQM_ALGORITHMS_SIMPLE_POL1_FIT_H
