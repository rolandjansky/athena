/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file DivideReference_Bins_Diff_FromAvg.h file declares the dqm_algorithms::DivideReference_Bins_Diff_FromAvg  class.
 * \author andrea.dotti@cern.ch
*/

#ifndef DQM_ALGORITHMS_DIVIDEREFERENCE_BINS_DIFF_FROMAVG_H
#define DQM_ALGORITHMS_DIVIDEREFERENCE_BINS_DIFF_FROMAVG_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/DivideReference.h>

namespace dqm_algorithms
{
	struct DivideReference_Bins_Diff_FromAvg : public DivideReference
        {
	  DivideReference_Bins_Diff_FromAvg(): DivideReference("Bins_Diff_FromAvg") {};

	};
}

#endif // DQM_ALGORITHMS_DIVIDEREFERENCE_BINS_DIFF_FROMAVG_H
