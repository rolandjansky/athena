/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//! \file PercentSummary.h file declares the dqm_algorithms::summary::PercentSummary class.
//! Valerio Consorti valerio.consorti@roma1.infn.it
                                                                                                                                  

#ifndef DQM_ALGORITHMS_PERCENT_SUMMARY_H
#define DQM_ALGORITHMS_PERCENT_SUMMARY_H

#include <dqm_core/SummaryMaker.h>

namespace dqm_algorithms
{
    namespace summary
    {
	struct PercentSummary : public dqm_core::SummaryMaker
        {
	  PercentSummary();
	  ~PercentSummary();

	  PercentSummary * clone();
	  dqm_core::Result * execute( const std::string & , const dqm_core::Result & result, const dqm_core::ParametersMap & );
	};
    }
}

#endif
