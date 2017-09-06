/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file OutlierAndFlatnessTest.h file declares the dqm_algorithms::OutlierAndFlatnessTest  class.
 * \author Steffen Schaepe
*/

#ifndef DQM_ALGORITHMS_OUTLIERANDFLATNESSTEST_H
#define DQM_ALGORITHMS_OUTLIERANDFLATNESSTEST_H

#include <dqm_core/Algorithm.h>

namespace dqm_algorithms
{
	class OutlierAndFlatnessTest : public dqm_core::Algorithm {
    public:
	   OutlierAndFlatnessTest(const std::string & name);

	    //overwrites virtual functions
	   OutlierAndFlatnessTest * clone( );
	   dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
	   using dqm_core::Algorithm::printDescription;
	   void printDescription(std::ostream& out); 

	  private:
     std::string m_name;
	};
}

#endif // DQM_ALGORITHMS_OUTLIERANDFLATNESSTEST_H
