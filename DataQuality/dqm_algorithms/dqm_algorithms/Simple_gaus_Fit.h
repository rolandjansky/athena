/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file Simple_gaus_Fit.h file declares the dqm_algorithms::algorithm::Simple_gaus_Fit  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_SIMPLE_GAUS_FIT_H
#define DQM_ALGORITHMS_SIMPLE_GAUS_FIT_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/RootFit.h>

namespace dqm_algorithms
{
	struct Simple_gaus_Fit : public RootFit
        {
	     Simple_gaus_Fit():  RootFit("gaus")  {};

              
	};

}

#endif // DQM_ALGORITHMS_SIMPLE_GAUS_FIT_H
