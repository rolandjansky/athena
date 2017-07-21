/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file BasicHistoCheck.h file declares the dqm_core::BasicHistoCheck  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_BASICHISTOCHECK_H
#define DQM_ALGORITHMS_BASICHISTOCHECK_H

#include <dqm_core/Algorithm.h>

namespace dqm_algorithms
{
	struct BasicHistoCheck : public dqm_core::Algorithm
        {
	  BasicHistoCheck(const std::string & name);

	    //overwrites virtual functions
	  BasicHistoCheck * clone( );
	  dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
          using dqm_core::Algorithm::printDescription;
	  void printDescription(std::ostream& out); 

	  private:
	  std::string m_name;
	};
}

#endif // DQM_ALGORITHMS_BASICHISTOCHECK_H
