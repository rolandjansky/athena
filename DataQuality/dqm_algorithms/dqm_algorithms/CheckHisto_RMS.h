/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file CheckHisto_RMS.h file declares the dqm_algorithms::CheckHisto_RMS  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_CHECKHISTO_RMS_H
#define DQM_ALGORITHMS_CHECKHISTO_RMS_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/BasicStatCheck.h>

namespace dqm_algorithms
{
	struct CheckHisto_RMS : public BasicStatCheck
        {
	     CheckHisto_RMS():  BasicStatCheck("RMS")  {};

              
	};

}

#endif // DQM_ALGORITHMS_CHECKHISTO_RMS_H
