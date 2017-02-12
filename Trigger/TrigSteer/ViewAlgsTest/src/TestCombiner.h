///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TestCombiner.h 
// Header file for class TestCombiner
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef VIEWALGSTEST_TESTCOMBINER_H
#define VIEWALGSTEST_TESTCOMBINER_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"

#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"


class TestCombiner
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  TestCombiner( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~TestCombiner(); 

  // Assignment operator: 
  //TestCombiner &operator=(const TestCombiner &alg); 

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
  TestCombiner();

  /// Containers
  SG::ReadHandle< xAOD::TrigCompositeContainer > m_inputA;
  SG::ReadHandle< xAOD::TrigCompositeContainer > m_inputB;
  SG::WriteHandle< xAOD::TrigCompositeContainer > m_output;
  SG::WriteHandle< xAOD::TrigCompositeAuxContainer > m_outputAux;



}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !VIEWALGSTEST_TESTCOMBINER_H
