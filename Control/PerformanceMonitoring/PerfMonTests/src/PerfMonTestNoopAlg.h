///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PerfMonTestNoopAlg.h 
// Header file for class PerfMonTest::NoopAlg
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PERFMONTESTS_PERFMONTESTNOOPALG_H 
#define PERFMONTESTS_PERFMONTESTNOOPALG_H 

// STL includes
#include <string>

// HepMC / CLHEP includes

// FrameWork includes
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"

// Forward declaration
class StoreGateSvc;

namespace PerfMonTest {

class NoopAlg : public Algorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  NoopAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~NoopAlg(); 

  // Assignment operator: 
  //NoopAlg &operator=(const NoopAlg &alg); 

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
  NoopAlg();

  typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
  /// Pointer to StoreGate
  StoreGateSvc_t m_storeGate;

  /// MsgStream instance (a std::cout like with print-out levels)
  MsgStream m_msg;

  /// Containers
  

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace PerfMonTest

#endif //> PERFMONTESTS_PERFMONTESTNOOPALG_H
