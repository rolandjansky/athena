/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file AddReference_All_Bins_Filled.h file declares the dqm_algorithms::AddReference_All_Bins_Filled  class.
 * \author andrea.dotti@cern.ch
*/

#ifndef DQM_ALGORITHMS_ADDREFERENCE_ALL_BINS_FILLED_H
#define DQM_ALGORITHMS_ADDREFERENCE_ALL_BINS_FILLED_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/AddReference.h>

namespace dqm_algorithms
{
	struct AddReference_All_Bins_Filled : public AddReference
        {
	  AddReference_All_Bins_Filled(): AddReference("All_Bins_Filled") {};

	};
}

#endif // DQM_ALGORITHMS_ADDREFERENCE_ALL_BINS_FILLED_H
