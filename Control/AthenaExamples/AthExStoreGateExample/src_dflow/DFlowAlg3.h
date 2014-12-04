///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DFlowAlg3.h 
// Header file for class DFlowAlg3
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHEXSTOREGATEEXAMPLE_ATHEX_DFLOWALG3_H
#define ATHEXSTOREGATEEXAMPLE_ATHEX_DFLOWALG3_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/UpdateHandle.h"

namespace AthEx {

class DFlowAlg3
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  DFlowAlg3( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~DFlowAlg3(); 

  // Assignment operator: 
  //DFlowAlg3 &operator=(const DFlowAlg3 &alg); 

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
  DFlowAlg3();

  /// Containers
  
  // vars
  SG::ReadHandle<int>  m_r_int;
  SG::ReadHandle<std::vector<int> > m_r_ints;
  SG::UpdateHandle<std::vector<int> > m_rw_ints;

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace AthEx
#endif //> !ATHEXSTOREGATEEXAMPLE_ATHEX_DFLOWALG3_H
