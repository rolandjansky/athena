///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DFlowAlg1.h 
// Header file for class DFlowAlg1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHVIEWS_ATHVIEWS_DFLOWALG1_H
#define ATHVIEWS_ATHVIEWS_DFLOWALG1_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthViews/AthViewAlgorithm.h"
#include "SGTools/BuiltinsClids.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "AthViews/View.h"

namespace AthViews {

class DFlowAlg1
  //: public ::AthViewAlgorithm
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  DFlowAlg1( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~DFlowAlg1(); 

  // Assignment operator: 
  //DFlowAlg1 &operator=(const DFlowAlg1 &alg); 

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
  DFlowAlg1();

  /// Containers
  
  // vars
  SG::ReadHandleKey<int> m_r_int;
  SG::WriteHandleKey<int> m_w_int;

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace AthViews
#endif //> !ATHVIEWS_ATHVIEWS_DFLOWALG1_H
