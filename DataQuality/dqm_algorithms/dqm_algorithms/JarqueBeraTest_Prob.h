/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file JarqueBearTest_Prob.h file declares the dqm_algorithms::JarqueBearTest_Prob  class.
 * \author andrea.dotti@cen.ch
*/

#ifndef DQM_ALGORITHMS_JARQUEBERATEST_PROB_H
#define DQM_ALGORITHMS_JARQUEBERATEST_PROB_H

#include <dqm_algorithms/JarqueBeraTest.h>

namespace dqm_algorithms
{
  struct JarqueBeraTest_Prob : public JarqueBeraTest
  {
    JarqueBeraTest_Prob() : JarqueBeraTest("Prob") {} ;
  };
}

#endif // DQM_ALGORITHMS_JARQUEBERATEST_PROB_H
