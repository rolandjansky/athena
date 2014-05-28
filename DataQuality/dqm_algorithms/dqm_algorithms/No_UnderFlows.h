/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file No_UnderFlows.h file declares the dqm_algorithms::No_UnderFlows  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_NO_UNDERFLOWS_H
#define DQM_ALGORITHMS_NO_UNDERFLOWS_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/BasicHistoCheck.h>

namespace dqm_algorithms
{
	struct No_UnderFlows : public BasicHistoCheck
        {
	     No_UnderFlows():  BasicHistoCheck("No_UnderFlows")  {};

              
	};

}

#endif // DQM_ALGORITHMS_NO_UNDERFLOWS_H
