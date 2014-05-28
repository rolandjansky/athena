/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file AddReference_Bins_Equal_Threshold.h file declares the dqm_algorithms::AddReference_Bins_Equal_Threshold  class.
 * \author andrea.dotti@cern.ch
*/

#ifndef DQM_ALGORITHMS_ADDREFERENCE_BINS_EQUAL_THRESHOLD_H
#define DQM_ALGORITHMS_ADDREFERENCE_BINS_EQUAL_THRESHOLD_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/AddReference.h>

namespace dqm_algorithms
{
	struct AddReference_Bins_Equal_Threshold : public AddReference
        {
	  AddReference_Bins_Equal_Threshold(): AddReference("Bins_Equal_Threshold") {};

	};
}

#endif // DQM_ALGORITHMS_ADDREFERENCE_BINS_EQUAL_THRESHOLD_H
