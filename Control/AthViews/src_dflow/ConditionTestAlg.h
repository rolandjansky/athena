///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ConditionTestAlg.h 
// Header file for class ConditionTestAlg
// Author: B. Wynne <bwynne@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHVIEWS_CONDITIONTESTALG_H
#define ATHVIEWS_CONDITIONTESTALG_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthExHive/CondDataObj.h"

namespace AthViews {

class ConditionTestAlg
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  ConditionTestAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~ConditionTestAlg(); 

  // Assignment operator: 
  //ConditionTestAlg &operator=(const ConditionTestAlg &alg); 

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
  ConditionTestAlg();

  /// Containers
  SG::WriteCondHandleKey<CondDataObj> m_condKeyTest{ this, "TestConditionsData", "testConditionsData", "" };

  // vars
}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace AthViews
#endif //> !ATHVIEWS_CONDITIONTESTALG_H
