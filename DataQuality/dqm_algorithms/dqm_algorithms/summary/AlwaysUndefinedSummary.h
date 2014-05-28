/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file AlwaysUndefinedSummary.h
 * \author Akimasa Ishikawa (akimasa.ishikawa@cern.ch)
*/
                                                                                                                                  

#ifndef DQM_ALGORITHMS_ALWAYS_GREEN_SUMMARY_H
#define DQM_ALGORITHMS_ALWAYS_GREEN_SUMMARY_H

#include <dqm_core/SummaryMaker.h>

namespace dqm_algorithms
{
    namespace summary
    {
	struct AlwaysUndefinedSummary : public dqm_core::SummaryMaker
        {
	  AlwaysUndefinedSummary();
	  ~AlwaysUndefinedSummary();

	  AlwaysUndefinedSummary * clone();
	  dqm_core::Result * execute( const std::string & , const dqm_core::Result & result, const dqm_core::ParametersMap & );
	};
    }
}

#endif
