/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file AddReference_Bins_Diff_FromAvg.h file declares the dqm_algorithms::AddReference_Bins_Diff_FromAvg  class.
 * \author andrea.dotti@cern.ch
*/

#ifndef DQM_ALGORITHMS_ADDREFERENCE_BINS_DIFF_FROMAVG_H
#define DQM_ALGORITHMS_ADDREFERENCE_BINS_DIFF_FROMAVG_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/AddReference.h>

namespace dqm_algorithms
{
	struct AddReference_Bins_Diff_FromAvg : public AddReference
        {
	  AddReference_Bins_Diff_FromAvg(): AddReference("Bins_Diff_FromAvg") {};

	};
}

#endif // DQM_ALGORITHMS_ADDREFERENCE_BINS_DIFF_FROMAVG_H
