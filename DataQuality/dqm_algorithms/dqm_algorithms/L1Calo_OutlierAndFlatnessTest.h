/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file L1Calo_OutlierAndFlatnessTest.h file declares the dqm_algorithms::L1Calo_OutlierAndFlatnessTest  class.
 * \author Steffen Schaepe
*/

#ifndef DQM_ALGORITHMS_L1CALO_OUTLIERANDFLATNESSTEST_H
#define DQM_ALGORITHMS_L1CALO_OUTLIERANDFLATNESSTEST_H

#include <dqm_core/Algorithm.h>

namespace dqm_algorithms
{
	class L1Calo_OutlierAndFlatnessTest : public dqm_core::Algorithm {
    public:
	   L1Calo_OutlierAndFlatnessTest(const std::string & name);

	    //overwrites virtual functions
	   L1Calo_OutlierAndFlatnessTest * clone( );
	   dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
	   using dqm_core::Algorithm::printDescription;
	   void printDescription(std::ostream& out); 

	  private:
     std::string m_name;

     unsigned int m_counterSkip;
	};
}

#endif // DQM_ALGORITHMS_L1CALO_OUTLIERANDFLATNESSTEST_H
