/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file JetWorstCaseSummary.h file declares the dqm_algorithms::summary::JetWorstCaseSummary class.
 */   


#ifndef DQM_ALGORITHMS_JETWORSTCASESUMMARY_H
#define DQM_ALGORITHMS_JETWORSTCASESUMMARY_H

#include <dqm_core/SummaryMaker.h>

namespace dqm_algorithms
{
  namespace summary
    {
      /*! A simple summary
	This summary maker calculates result for a region for Jet Monitoring as the worst result
	of childs. If at least one child is red or yellow the summary result will be yellow.
	Green if there are no red and yellow childs.
	Childs with undefined state will not be considered.
	Summary will  result in undefined state if all childs are in 
	Undefined state
      */
      struct JetWorstCaseSummary : public dqm_core::SummaryMaker
      {
	JetWorstCaseSummary();
	~JetWorstCaseSummary();
	JetWorstCaseSummary* clone();
	dqm_core::Result* execute( const std::string & , const dqm_core::Result & , const dqm_core::ParametersMap & );
      };
    }
}

#endif
