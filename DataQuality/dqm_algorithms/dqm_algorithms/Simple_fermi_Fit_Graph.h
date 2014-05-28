/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file Simple_erf_Fit_Graph.h file declares the dqm_algorithms::algorithm::Simple_fermi_Fit_Graph class.  \n Format copied from Simple_gaus_Fit.h
 * \author Matt King and Akimasa Ishikawa
*/

#ifndef DQM_ALGORITHMS_SIMPLE_FERMI_FIT_GRAPH_H
#define DQM_ALGORITHMS_SIMPLE_FERMI_FIT_GRAPH_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/RootFitGraph.h>

namespace dqm_algorithms
{
	struct Simple_fermi_Fit_Graph : public RootFitGraph
        {
	     Simple_fermi_Fit_Graph():  RootFitGraph("fermi")  {};

              
	};

}

#endif // DQM_ALGORITHMS_SIMPLE_FERMI_FIT_GRAPH_H
