/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file Simple_gausplusexpo_Fit.h file declares the dqm_algorithms::algorithm::Simple_gausplusexpo_Fit  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_SIMPLE_GAUSPLUSEXPO_FIT_H
#define DQM_ALGORITHMS_SIMPLE_GAUSPLUSEXPO_FIT_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/RootFit.h>

namespace dqm_algorithms
{
	struct Simple_gausplusexpo_Fit : public RootFit
        {
	     Simple_gausplusexpo_Fit():  RootFit("gausplusexpo")  {};

              
	};

}

#endif // DQM_ALGORITHMS_SIMPLE_GAUSPLUSEXPO_FIT_H
