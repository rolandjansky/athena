/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file DivideReference_Bins_GreaterThanEqual_Threshold.h file declares the dqm_algorithms::DivideReference_Bins_GreaterThanEqual_Threshold  class.
 * \author andrea.dotti@cern.ch
*/

#ifndef DQM_ALGORITHMS_DIVIDEREFERENCE_BINS_GREATERTHANEQUAL_THRESHOLD_H
#define DQM_ALGORITHMS_DIVIDEREFERENCE_BINS_GREATERTHANEQUAL_THRESHOLD_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/DivideReference.h>

namespace dqm_algorithms
{
	struct DivideReference_Bins_GreaterThanEqual_Threshold : public DivideReference
        {
	  DivideReference_Bins_GreaterThanEqual_Threshold(): DivideReference("Bins_GreaterThanEqual_Threshold") {};

	};
}

#endif // DQM_ALGORITHMS_DIVIDEREFERENCE_BINS_GREATERTHANEQUAL_THRESHOLD_H
