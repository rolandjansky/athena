/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file Simple_sinusoid_Fit.h file declares the dqm_algorithms::algorithm::Simple_sinusoid_Fit  class.
 * \author Cristobal Cuenca Almenar, November 29th 2009
*/

#ifndef DQM_ALGORITHMS_SIMPLE_SINUSOID_FIT_H
#define DQM_ALGORITHMS_SIMPLE_SINUSOID_FIT_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/RootFit.h>

namespace dqm_algorithms
{
	struct Simple_sinusoid_Fit : public RootFit
        {
	     Simple_sinusoid_Fit():  RootFit("sinusoid")  {};

              
	};

}

#endif // DQM_ALGORITHMS_SIMPLE_SINUSOID_FIT_H
