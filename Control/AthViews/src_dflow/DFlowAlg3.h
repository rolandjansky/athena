///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DFlowAlg3.h 
// Header file for class DFlowAlg3
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHVIEWS_ATHVIEWS_DFLOWALG3_H
#define ATHVIEWS_ATHVIEWS_DFLOWALG3_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "AthExHive/HiveDataObj.h"
#include "AthExHive/CondDataObj.h"

namespace AthViews {

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
  SG::ReadHandleKey<int>  m_r_int;
  SG::ReadHandleKey<std::vector<int> > m_r_ints;
  SG::WriteHandleKey<int> m_w_dflowDummy;
  SG::ReadHandleKey<HiveDataObj> m_testUpdate;
  SG::ReadCondHandleKey<CondDataObj> m_condKeyTest{ this, "TestConditionsData", "testConditionsData", "" };

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace AthViews
#endif //> !ATHVIEWS_ATHVIEWS_DFLOWALG3_H
