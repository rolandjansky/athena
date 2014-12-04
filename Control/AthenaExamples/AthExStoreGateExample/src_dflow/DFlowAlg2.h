///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DFlowAlg2.h 
// Header file for class DFlowAlg2
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHEXSTOREGATEEXAMPLE_ATHEX_DFLOWALG2_H
#define ATHEXSTOREGATEEXAMPLE_ATHEX_DFLOWALG2_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/UpdateHandle.h"

namespace AthEx {

class DFlowAlg2
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  DFlowAlg2( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~DFlowAlg2(); 

  // Assignment operator: 
  //DFlowAlg2 &operator=(const DFlowAlg2 &alg); 

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
  DFlowAlg2();

  /// Containers
  
  // vars
  SG::ReadHandle<int>  m_r_int;
  SG::UpdateHandle<int> m_rw_int;
  SG::WriteHandle<std::vector<int> > m_ints;
}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace AthEx
#endif //> !ATHEXSTOREGATEEXAMPLE_ATHEX_DFLOWALG2_H
