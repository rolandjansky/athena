///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PerfMonTestMallocAlg.h 
// Header file for class PerfMonTest::MallocAlg
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PERFMONTESTS_PERFMONTESTMALLOCALG_H 
#define PERFMONTESTS_PERFMONTESTMALLOCALG_H 

// STL includes
#include <string>

// HepMC / CLHEP includes

// FrameWork includes
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"

// Forward declaration

namespace PerfMonTest {

class MallocAlg : public Algorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  MallocAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~MallocAlg(); 

  // Assignment operator: 
  //MallocAlg &operator=(const MallocAlg &alg); 

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
  MallocAlg();

  /// MsgStream instance (a std::cout like with print-out levels)
  MsgStream m_msg;

  /// event number at which to actually do stuff
  unsigned int m_evtNbr;

  /// switch between using a C-array and a std::vector
  bool m_useStdVector;

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace PerfMonTest

#endif //> PERFMONTESTS_PERFMONTESTMALLOCALG_H
