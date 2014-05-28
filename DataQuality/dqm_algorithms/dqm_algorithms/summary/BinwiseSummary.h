/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file BinwiseSummary.h file declares the dqm_algorithms::summary::BinwiseSummary class, a simple summary maker which combines the binwise status from the appropriate algorithms in it's region, masking according to the Role=Mask histogram, and produces a dq result based on these and the other parameters in the region a la WorstCaseSummary
 * \author Evan Wulf 
*/
   


#ifndef DQM_ALGORITHMS_BINWISESUMMARY_H
#define DQM_ALGORITHMS_BINWISESUMMARY_H

#include <dqm_core/SummaryMaker.h>


namespace dqm_algorithms
{
  namespace summary
    {
      struct BinwiseSummary : public dqm_core::SummaryMaker
      {
	BinwiseSummary();
	~BinwiseSummary();
	BinwiseSummary* clone();
	dqm_core::Result* execute( const std::string & name, const dqm_core::Result & , const dqm_core::ParametersMap & );
      };
    }
}

#endif //#ifndef DQM_ALGORITHMS_BINWISESUMMARY_H
