/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file DivideReference_All_Bins_Filled.h file declares the dqm_algorithms::DivideReference_All_Bins_Filled  class.
 * \author andrea.dotti@cern.ch
*/

#ifndef DQM_ALGORITHMS_DIVIDEREFERENCE_ALL_BINS_FILLED_H
#define DQM_ALGORITHMS_DIVIDEREFERENCE_ALL_BINS_FILLED_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/DivideReference.h>

namespace dqm_algorithms
{
	struct DivideReference_All_Bins_Filled : public DivideReference
        {
	  DivideReference_All_Bins_Filled(): DivideReference("All_Bins_Filled") {};

	};
}

#endif // DQM_ALGORITHMS_DIVIDEREFERENCE_ALL_BINS_FILLED_H
