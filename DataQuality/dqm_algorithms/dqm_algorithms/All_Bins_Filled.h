/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file All_Bins_Filled.h file declares the dqm_algorithms::All_Bins_Filled  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_ALL_BINS_FILLED_H
#define DQM_ALGORITHMS_ALL_BINS_FILLED_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/BasicHistoCheck.h>

namespace dqm_algorithms
{
	struct All_Bins_Filled : public BasicHistoCheck
        {
	     All_Bins_Filled():  BasicHistoCheck("All_Bins_Filled")  {};

              
	};

}

#endif // DQM_ALGORITHMS_ALL_BINS_FILLED_H
