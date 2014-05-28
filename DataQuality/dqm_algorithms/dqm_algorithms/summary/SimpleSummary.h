/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file SimpleSummary.h file declares the dqm_algorithms::summary::SimpleSummary class.
 * \author Haleh Hadavand
*/
                                                                                                                                  

#ifndef DQM_ALGORITHMS_SIMPLE_SUMMARY_H
#define DQM_ALGORITHMS_SIMPLE_SUMMARY_H

#include <dqm_core/SummaryMaker.h>

namespace dqm_algorithms
{
    namespace summary
    {
	struct SimpleSummary : public dqm_core::SummaryMaker
        {
	  SimpleSummary();
	  ~SimpleSummary();

	  SimpleSummary * clone();
	  dqm_core::Result * execute( const std::string & , const dqm_core::Result & result, const dqm_core::ParametersMap & );
	};
    }
}

#endif
