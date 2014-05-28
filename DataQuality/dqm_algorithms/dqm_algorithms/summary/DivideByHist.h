/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file DivideByHist.h file declares the dqm_algorithms::summary::DivideByHist class, a summary maker which divides the histogram from the result of one algorithm by a histogram from the result of another and then runs a test on the resulting histogram, as specified by the algorithm input
 * \author Evan Wulf 
*/
   


#ifndef DQM_ALGORITHMS_DIVIDEBYHIST_H
#define DQM_ALGORITHMS_DIVIDEBYHIST_H

#include <dqm_core/SummaryMaker.h>


namespace dqm_algorithms
{
  namespace summary
    {
      struct DivideByHist : public dqm_core::SummaryMaker
      {
	DivideByHist();
	~DivideByHist();
	DivideByHist* clone();
	dqm_core::Result* execute( const std::string & name, const dqm_core::Result & , const dqm_core::ParametersMap & );
	dqm_core::Result* execute( const std::string & name, const std::multimap<std::string,TObject*> & inputs, const dqm_core::AlgorithmConfig& config );
      };
    }
}

#endif //#ifndef DQM_ALGORITHMS_DIVIDEBYHIST_H
