/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file Simple_landau_Fit.h file declares the dqm_algorithms::Simple_landau_Fit  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_SIMPLE_LANDAU_FIT_H
#define DQM_ALGORITHMS_SIMPLE_LANDAU_FIT_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/RootFit.h>

namespace dqm_algorithms
{
	struct Simple_landau_Fit : public RootFit
        {
	     Simple_landau_Fit():  RootFit("landau")  {};

              
	};

}

#endif // DQM_ALGORITHMS_SIMPLE_LANDAU_FIT_H
