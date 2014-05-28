/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file Histogram_Not_Empty.h file declares the dqm_algorithms::Histogram_Not_Empty  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_HISTOGRAM_NOT_EMPTY_H
#define DQM_ALGORITHMS_HISTOGRAM_NOT_EMPTY_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/BasicHistoCheck.h>

namespace dqm_algorithms
{
	struct Histogram_Not_Empty : public BasicHistoCheck
        {
	     Histogram_Not_Empty():  BasicHistoCheck("Histogram_Not_Empty")  {};

              
	};

}

#endif // DQM_ALGORITHMS_HISTOGRAM_NOT_EMPTY_H
