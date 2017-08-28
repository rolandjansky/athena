/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file BasicStatCheck.h file declares the dqm_algorithms::BasicStatCheck  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_BASICSTATCHECK_H
#define DQM_ALGORITHMS_BASICSTATCHECK_H

#include <dqm_core/Algorithm.h>

namespace dqm_algorithms
{
	struct BasicStatCheck : public dqm_core::Algorithm
        {
	  BasicStatCheck(const std::string & name);

	    //overwrites virtual functions
	  BasicStatCheck * clone( );
	  dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
          using dqm_core::Algorithm::printDescription;
	  void  printDescription(std::ostream& out); 
	  private:
	  std::string m_name;
	};
}

#endif // DQM_ALGORITHMS_BASICSTATCHECK_H
