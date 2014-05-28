/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file Simple_doublegaus_Fit.h file declares the dqm_algorithms::algorithm::Simple_doublegaus_Fit  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_SIMPLE_DOUBLEGAUS_FIT_H
#define DQM_ALGORITHMS_SIMPLE_DOUBLEGAUS_FIT_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/RootFit.h>

namespace dqm_algorithms
{
	struct Simple_doublegaus_Fit : public RootFit
        {
	     Simple_doublegaus_Fit():  RootFit("doublegaus")  {};

              
	};

}

#endif // DQM_ALGORITHMS_SIMPLE_DOUBLEGAUS_FIT_H
