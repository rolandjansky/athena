/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file CheckHisto_Mean.h file declares the dqm_algorithms::CheckHisto_Mean  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_CHECKHISTO_MEAN_H
#define DQM_ALGORITHMS_CHECKHISTO_MEAN_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/BasicStatCheck.h>

namespace dqm_algorithms
{
	struct CheckHisto_Mean : public BasicStatCheck
        {
	     CheckHisto_Mean():  BasicStatCheck("Mean")  {};

              
	};

}

#endif // DQM_ALGORITHMS_CHECKHISTO_MEAN_H
