/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file MDTWorstCaseSummary.h file declares the dqm_algorithms::summary::MDTWorstCaseSummary class.
 */   


#ifndef DQM_ALGORITHMS_MDTWORSTCASESUMMARY_H
#define DQM_ALGORITHMS_MDTWORSTCASESUMMARY_H

#include <dqm_core/SummaryMaker.h>

namespace dqm_algorithms
{
  namespace summary
    {
      /*! A simple summary
	Compute the MDT Coverage to ML Granularity per region (BA,BC,EA,EC)
	based on four separate plots in four different layers (Inner, Middle, Outer, Extra)
	returns green if > 90 %, red if less than 90%, and undefined if enough of the plots
	failed the MinStat requirement
      */
      struct MDTWorstCaseSummary : public dqm_core::SummaryMaker
      {
	MDTWorstCaseSummary();
	~MDTWorstCaseSummary();
	MDTWorstCaseSummary* clone();
	dqm_core::Result* execute( const std::string & , const dqm_core::Result & , const dqm_core::ParametersMap & );
      };
    }
}

#endif
