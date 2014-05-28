/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file WorstWBlackSummary.h file declares the dqm_algorithms::summary::WorstWBlackSummary class.
 * \author Andrea Dotti
*/
   


#ifndef DQM_ALGORITHMS_WORSTCASESUMMARY_H
#define DQM_ALGORITHMS_WORSTCASESUMMARY_H

#include <dqm_core/SummaryMaker.h>

namespace dqm_algorithms
{
  namespace summary
    {
      /*! A simple summary
	This summary maker calculates result for a region as the worst result
	of childs. If at least one child is red the summary result will be red.
	Yellow if at least one child is yellow and none is red.
	Green if there are no red and yellow childs.
	Childs with undefined state will not be considered.
	Summary will  result in undefined state if all childs are in 
	Undefined state
      */
      struct WorstWBlackSummary : public dqm_core::SummaryMaker
      {
	WorstWBlackSummary();
	~WorstWBlackSummary();
	WorstWBlackSummary* clone();
	dqm_core::Result* execute( const std::string & , const dqm_core::Result & , const dqm_core::ParametersMap & );
      };
    }
}

#endif
