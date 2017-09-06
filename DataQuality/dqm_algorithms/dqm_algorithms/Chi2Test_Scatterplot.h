/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file Chi2Test_Scatterplot.h file declares the dqm_algorithms::Chi2Test_Scatterplot  class.
 * \author Ian Moult
*/

#ifndef DQM_ALGORITHMS_CHI2TEST_SCATTERPLOT_H
#define DQM_ALGORITHMS_CHI2TEST_SCATTERPLOT_H

#include <dqm_core/Algorithm.h>

namespace dqm_algorithms
{
	struct Chi2Test_Scatterplot : public dqm_core::Algorithm
        {
	    Chi2Test_Scatterplot ();

	    Chi2Test_Scatterplot * clone( );
	    dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
            using dqm_core::Algorithm::printDescription;
            void  printDescription(std::ostream& out);           
	};
}

#endif // DQM_ALGORITHMS_CHI2TEST_SCATTERPLOT_H
