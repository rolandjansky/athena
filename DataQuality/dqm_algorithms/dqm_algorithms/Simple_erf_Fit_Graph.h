/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file Simple_erf_Fit_Graph.h file declares the dqm_algorithms::algorithm::Simple_erf_Fit_Graph class.
 * \author Akimasa Ishikawa (akimasa.ishikawa@cern.ch)
*/

#ifndef DQM_ALGORITHMS_SIMPLE_ERF_FIT_GRAPH_H
#define DQM_ALGORITHMS_SIMPLE_ERF_FIT_GRAPH_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/RootFitGraph.h>

namespace dqm_algorithms
{
	struct Simple_erf_Fit_Graph : public RootFitGraph
        {
	     Simple_erf_Fit_Graph():  RootFitGraph("erf")  {};

              
	};

}

#endif // DQM_ALGORITHMS_SIMPLE_ERF_FIT_GRAPH_H
