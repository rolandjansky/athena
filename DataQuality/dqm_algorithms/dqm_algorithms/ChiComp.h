/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file ChiComp.h file declares the dqm_algorithms::ChiComp  class.
 * \author Pier-Olivier DeViveiros - inspired by the other dqm algorithms
*/

#ifndef DQM_ALGORITHMS_CHICOMP_H
#define DQM_ALGORITHMS_CHICOMP_H

#include <dqm_core/Algorithm.h>

namespace dqm_algorithms
{
	struct ChiComp : public dqm_core::Algorithm
        {
	  ChiComp( const std::string & name );
	  
	  ChiComp * clone( );
	  dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
          using dqm_core::Algorithm::printDescription;
	  void  printDescription(std::ostream& out);           

          private:
            std::string m_name;
	};
}

#endif // DQM_ALGORITHMS_CHICOMP_H
