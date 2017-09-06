/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file SkewnessTest.h file declares the dqm_algorithms::SkewnessTest class.
 * \author andrea.dotti@cern.ch
*/

#ifndef DQM_ALGORITHMS_SKEWNESSTEST_H
#define DQM_ALGORITHMS_SKEWNESSTEST_H

#include <dqm_core/Algorithm.h>

namespace dqm_algorithms
{
	struct SkewnessTest : public dqm_core::Algorithm
        {
	  SkewnessTest(const std::string & name);

	    //overwrites virtual functions
	  SkewnessTest * clone( );
	  dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
          using dqm_core::Algorithm::printDescription;
	  void  printDescription(std::ostream& out);
	  bool CompareSkewnessTest( const std::string& type, double value , double threshold );
	  private:
	  std::string m_name;
	};
}

#endif // DQM_ALGORITHMS_SKEWNESSTEST_H
