/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file ReferenceMasking_Bins_Diff_FromAvg.h file declares the dqm_algorithms::ReferenceMasking_Bins_Diff_FromAvg  class.
 * \author andrea.dotti@cern.ch
*/

#ifndef DQM_ALGORITHMS_REFERENCEMASKING_BINS_DIFF_FROMAVG_H
#define DQM_ALGORITHMS_REFERENCEMASKING_BINS_DIFF_FROMAVG_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/ReferenceMasking.h>

namespace dqm_algorithms
{
	struct ReferenceMasking_Bins_Diff_FromAvg : public ReferenceMasking
        {
	  ReferenceMasking_Bins_Diff_FromAvg(): ReferenceMasking("Bins_Diff_FromAvg") {};

	};
}

#endif // DQM_ALGORITHMS_REFERENCEMASKING_BINS_DIFF_FROMAVG_H
