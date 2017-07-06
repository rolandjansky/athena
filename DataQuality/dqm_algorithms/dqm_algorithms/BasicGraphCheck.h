/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file BasicGraphCheck.h file declares the dqm_core::BasicGraphCheck  class.
 * \author Peter Onyisi following Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_BASICGRAPHCHECK_H
#define DQM_ALGORITHMS_BASICGRAPHCHECK_H

#include <dqm_core/Algorithm.h>

namespace dqm_algorithms
{
	struct BasicGraphCheck : public dqm_core::Algorithm
        {
	  BasicGraphCheck(const std::string & name);

	    //overwrites virtual functions
	  BasicGraphCheck * clone( );
	  dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
          using dqm_core::Algorithm::printDescription;
	  void printDescription(std::ostream& out); 

	  private:
	  std::string m_name;
	};
}

#endif
