/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file KolmogorovTest.h file declares the dqm_algorithms::KolmogorovTest  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_KOLMOGOROVTEST_H
#define DQM_ALGORITHMS_KOLMOGOROVTEST_H

#include <dqm_core/Algorithm.h>

namespace dqm_algorithms
{
	struct KolmogorovTest : public dqm_core::Algorithm
        {
	    KolmogorovTest( const std::string & name );

	    KolmogorovTest * clone( );
	    dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
            using dqm_core::Algorithm::printDescription;
            void  printDescription(std::ostream& out);           
          private:
            std::string m_name;
	    std::string m_option;
	};
}

#endif // DQM_ALGORITHMS_KOLMOGOROVTEST_H
