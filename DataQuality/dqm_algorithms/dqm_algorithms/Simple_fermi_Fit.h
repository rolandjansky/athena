/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file Simple_fermi_Fit.h file declares the dqm_algorithms::algorithm::Simple_fermi_Fit class.  \n Format copied from Simple_gaus_Fit.h
 * \author Matt King
*/

#ifndef DQM_ALGORITHMS_SIMPLE_FERMI_FIT_H
#define DQM_ALGORITHMS_SIMPLE_FERMI_FIT_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/RootFit.h>

namespace dqm_algorithms
{
	struct Simple_fermi_Fit : public RootFit
        {
	     Simple_fermi_Fit():  RootFit("fermi")  {};

              
	};

}

#endif // DQM_ALGORITHMS_SIMPLE_FERMI_FIT_H
