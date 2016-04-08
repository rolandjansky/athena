///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PerfMonTestErroneousAlg.h 
// Header file for class PerfMonTest::ErroneousAlg
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PERFMONTESTS_PERFMONTESTERRONEOUSALG_H 
#define PERFMONTESTS_PERFMONTESTERRONEOUSALG_H 

// STL includes
#include <string>
#include <list>
#include <vector>

// HepMC / CLHEP includes

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

namespace PerfMonTest {

class ErroneousAlg : public AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  ErroneousAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~ErroneousAlg(); 

  // Assignment operator: 
  //ErroneousAlg &operator=(const ErroneousAlg &alg); 

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
  ErroneousAlg();
  
  /// three member functions which will exhibit faulty behaviour
  bool jumpOnUninitializedValue();
  bool invalidRead();
  bool mismatchedFree();
  
  bool shouldIJump(bool shouldIJump)
  {
    return not shouldIJump;
  }
  
}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace PerfMonTest

#endif //> PERFMONTESTS_PERFMONTESTERRONEOUSALG_H
