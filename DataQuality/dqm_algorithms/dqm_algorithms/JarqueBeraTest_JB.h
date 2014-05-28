/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file JarqueBearTest_JB.h file declares the dqm_algorithms::JarqueBearTest_JB  class.
 * \author andrea.dotti@cen.ch
*/

#ifndef DQM_ALGORITHMS_JARQUEBERATEST_JB_H
#define DQM_ALGORITHMS_JARQUEBERATEST_JB_H

#include <dqm_algorithms/JarqueBeraTest.h>

namespace dqm_algorithms
{
  struct JarqueBeraTest_JB : public JarqueBeraTest
  {
    JarqueBeraTest_JB() : JarqueBeraTest("JB") {} ;
  };
}

#endif // DQM_ALGORITHMS_JARQUEBERATEST_JB_H
