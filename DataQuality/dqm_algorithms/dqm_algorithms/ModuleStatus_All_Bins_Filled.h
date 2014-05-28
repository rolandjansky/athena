/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file All_Bins_Filled.h file declares the dqm_algorithms::All_Bins_Filled  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_MODULESTATUS_ALL_BINS_FILLED_H
#define DQM_ALGORITHMS_MODULESTATUS_ALL_BINS_FILLED_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/BasicHistoCheckModuleStatus.h>

namespace dqm_algorithms
{
	struct ModuleStatus_All_Bins_Filled : public BasicHistoCheckModuleStatus
        {
	     ModuleStatus_All_Bins_Filled():  BasicHistoCheckModuleStatus("ModuleStatus_All_Bins_Filled")  {};

              
	};

}

#endif // DQM_ALGORITHMS_MODULESTATUS_ALL_BINS_FILLED_H
