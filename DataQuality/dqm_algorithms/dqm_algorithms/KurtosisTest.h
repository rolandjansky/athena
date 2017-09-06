/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file KurtosisTest.h file declares the dqm_algorithms::KurtosisTest class.
 * \author andrea.dotti@cern.ch
*/

#ifndef DQM_ALGORITHMS_KURTOSISTEST_H
#define DQM_ALGORITHMS_KURTOSISTEST_H

#include <dqm_core/Algorithm.h>

namespace dqm_algorithms
{
	struct KurtosisTest : public dqm_core::Algorithm
        {
	  KurtosisTest(const std::string & name);

	    //overwrites virtual functions
	  KurtosisTest * clone( );
	  dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
          using dqm_core::Algorithm::printDescription;
	  void  printDescription(std::ostream& out);
	  bool CompareKurtosisTest( const std::string& type, double value , double threshold );
	  private:
	  std::string m_name;
	};
}

#endif // DQM_ALGORITHMS_KURTOSISTEST_H
