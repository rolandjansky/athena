/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file JarqueBeraTest.h file declares the dqm_algorithms::JarqueBeraTest class, implementing the Jarque-Bera statistical test.
 * \author andrea.dotti@cern.ch
*/

#ifndef DQM_ALGORITHMS_JARQUEBERATEST_H
#define DQM_ALGORITHMS_JARQUEBERATEST_H

#include <dqm_core/Algorithm.h>

namespace dqm_algorithms
{
  struct JarqueBeraTest : public dqm_core::Algorithm
  {
    JarqueBeraTest( const std::string & name );
    
    JarqueBeraTest * clone( );
    dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
    using dqm_core::Algorithm::printDescription;
    void  printDescription(std::ostream& out);
    private:
    std::string m_name;
  };
 
}

#endif // DQM_ALGORITHMS_JARQUEBERATEST_H
