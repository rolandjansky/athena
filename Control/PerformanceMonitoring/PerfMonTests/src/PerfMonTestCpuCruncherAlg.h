///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// PerfMonTestCpuCruncherAlg.h 
// Header file for class PerfMonTest::CpuCruncherAlg
// Author: S.Binet<binet@cern.ch>, A. S. Mete<amete@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PERFMONTESTS_PERFMONTESTCPUCRUNCHERALG_H 
#define PERFMONTESTS_PERFMONTESTCPUCRUNCHERALG_H 

// STL includes
#include <string>
#include <random>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

namespace PerfMonTest {

class CpuCruncherAlg : public AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  CpuCruncherAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~CpuCruncherAlg(); 

  // Assignment operator: 
  //CpuCruncherAlg &operator=(const CpuCruncherAlg &alg); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

  // Perform math operations to burn CPU for a number of iterations
  double burn(unsigned long nIterations);

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  CpuCruncherAlg();

  /// Property to setup the mean (in ms) of CPU time to consume
  float m_meanCpuTime;

  /// Property to setup the RMS  (in ms) of CPU time to consume
  float m_rmsCpuTime;

  /// Random number setup
  std::default_random_engine m_random;
  std::normal_distribution<double> m_distribution;

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace PerfMonTest

#endif //> PERFMONTESTS_PERFMONTESTCPUCRUNCHERALG_H
