/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file GrubbsOutlierTest.h
 *  \author Venkatesh Kaushik <venkat.kaushik@cern.ch>
 *  @brief outlier-bins in 1d histogram
*/

#ifndef DQM_ALGORITHMS_GRUBBSOUTLIERTEST_H
#define DQM_ALGORITHMS_GRUBBSOUTLIERTEST_H

#include <dqm_core/Algorithm.h>
#include <vector>

namespace dqm_algorithms
{

  struct GrubbsOutlierTest : public dqm_core::Algorithm
  {
    GrubbsOutlierTest(const std::string &name);

    GrubbsOutlierTest * clone( );
    dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
    int GrubbsTest(std::vector<float>& vec, std::vector<size_t>&binIndices, float signLevel);
    using dqm_core::Algorithm::printDescription;
    void  printDescription(std::ostream& out);
    
    private:
    std::string m_name;
  };
}

#endif // DQM_ALGORITHMS_GRUBBSOUTLIERTEST_H
