/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file ReferenceMasking_Bins_GreaterThan_Threshold.h file declares the dqm_algorithms::ReferenceMasking_Bins_GreaterThan_Threshold  class.
 * \author andrea.dotti@cern.ch
*/

#ifndef DQM_ALGORITHMS_REFERENCEMASKING_BINS_GREATERTHAN_THRESHOLD_H
#define DQM_ALGORITHMS_REFERENCEMASKING_BINS_GREATERTHAN_THRESHOLD_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/ReferenceMasking.h>

namespace dqm_algorithms
{
	struct ReferenceMasking_Bins_GreaterThan_Threshold : public ReferenceMasking
        {
	  ReferenceMasking_Bins_GreaterThan_Threshold(): ReferenceMasking("Bins_GreaterThan_Threshold") {};

	};
}

#endif // DQM_ALGORITHMS_REFERENCEMASKING_BINS_GREATERTHAN_THRESHOLD_H
