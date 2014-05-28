/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file Simple_gauspluspol1_Fit.h file declares the dqm_algorithms::Simple_gauspluspol1_Fit  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_SIMPLE_GAUSPLUSPOL1_FIT_H
#define DQM_ALGORITHMS_SIMPLE_GAUSPLUSPOL1_FIT_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/RootFit.h>

namespace dqm_algorithms
{
	struct Simple_gauspluspol1_Fit : public RootFit
        {
	     Simple_gauspluspol1_Fit():  RootFit("gauspluspol1")  {};

              
	};

}

#endif // DQM_ALGORITHMS_SIMPLE_GAUSPLUSPOL1_FIT_H
