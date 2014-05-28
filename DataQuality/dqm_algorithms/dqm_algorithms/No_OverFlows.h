/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file No_OverFlows.h file declares the dqm_algorithms::No_OverFlows  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_NO_OVERFLOWS_H
#define DQM_ALGORITHMS_NO_OVERFLOWS_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/BasicHistoCheck.h>

namespace dqm_algorithms
{
	struct No_OverFlows : public BasicHistoCheck
        {
	     No_OverFlows():  BasicHistoCheck("No_OverFlows")  {};

              
	};

}

#endif // DQM_ALGORITHMS_NO_OVERFLOWS_H
