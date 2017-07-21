/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file Chi2Test.h file declares the dqm_algorithms::Chi2Test  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_CHI2TEST_H
#define DQM_ALGORITHMS_CHI2TEST_H

#include <dqm_core/Algorithm.h>

namespace dqm_algorithms
{
	struct Chi2Test : public dqm_core::Algorithm
        {
	    Chi2Test( const std::string & name );

	    Chi2Test * clone( );
	    dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
            using dqm_core::Algorithm::printDescription;
            void  printDescription(std::ostream& out);           
          private:
            std::string m_name;
	    std::string m_option;
	};
}

#endif // DQM_ALGORITHMS_CHI2TEST_H
