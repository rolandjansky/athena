///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PerfMonTestManyLeaksAlg.h 
// Header file for class PerfMonTest::ManyLeaksAlg
// Author: R.Sesuter
/////////////////////////////////////////////////////////////////// 
#ifndef PERFMONTESTS_PERFMONTESTMANYLEAKSALG_H 
#define PERFMONTESTS_PERFMONTESTMANYLEAKSALG_H 

// STL includes
#include <string>
#include <list>
#include <vector>

// HepMC / CLHEP includes

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"

namespace PerfMonTest {

class ManyLeaksAlg : public AthAlgorithm
{
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  ManyLeaksAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~ManyLeaksAlg(); 

  // Assignment operator: 
  //ManyLeaksAlg &operator=(const ManyLeaksAlg &alg); 

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
  ManyLeaksAlg();
  
  long* stillReachableFct(long** array);
  
  /// this one's possible lost
  long* possibleLostFct(long** array);
  
  /// this one's indirectly lost
  long* indirectlyLostFct(long** array);
  
  /// this one's definitely lost
  long* definitelyLostFct(long** array);
  
  /// this one's definitely lost
  void leakAll();
  
  /// Property to setup the size of the leak
  int m_leakSize;
  
  /// Property to setup the location of the leak, in initialize (true) or execute (false)
  bool m_leakInInit;
  
 private:
  
  // this one's still reachable
  long* m_stillReachable;
  
  // this one's possible lost
  long* m_possibleLost;
  
  // this one's indirectly lost
  long* m_indirectlyLost;
  
  // this one's definitely lost
  long* m_definitelyLost;
  
  // we still need to reference some pointers,
  // otherwise vagrind labels everything definitely lost
  static long **m_pointers;
}; 
  
// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace PerfMonTest

#endif //> PERFMONTESTS_PERFMONTESTMANYLEAKSALG_H
