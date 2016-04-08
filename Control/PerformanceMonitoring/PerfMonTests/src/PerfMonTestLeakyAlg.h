///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PerfMonTestLeakyAlg.h 
// Header file for class PerfMonTest::LeakyAlg
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PERFMONTESTS_PERFMONTESTLEAKYALG_H 
#define PERFMONTESTS_PERFMONTESTLEAKYALG_H 

// STL includes
#include <string>
#include <list>
#include <vector>

// HepMC / CLHEP includes

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

namespace PerfMonTest {

class LeakyAlg : public AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  LeakyAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~LeakyAlg(); 

  // Assignment operator: 
  //LeakyAlg &operator=(const LeakyAlg &alg); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

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
  LeakyAlg();

  /// Property to setup the size of the leak
  int m_leakSize;

  struct Leak {
    std::vector<int> m_data;
  };

  /// nbr of Leak objects
  int m_nbrLeaks;

  /// container to hold the leaked objects
  std::list<Leak*> m_leaks;
}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace PerfMonTest

#endif //> PERFMONTESTS_PERFMONTESTLEAKYALG_H
