///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SgStressConsumer.h 
// Header file for class SgStressConsumer
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef STOREGATETESTS_SGSTRESSCONSUMER_H 
#define STOREGATETESTS_SGSTRESSCONSUMER_H 

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"

// Forward declaration
class StoreGateSvc;

class SgStressConsumer : public Algorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  SgStressConsumer( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~SgStressConsumer(); 

  // Assignment operator: 
  //SgStressConsumer &operator=(const SgStressConsumer &alg); 

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
  // Private methods: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  SgStressConsumer();

  /// Retrieve data
  StatusCode readData();

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
  /// Pointer to StoreGate
  StoreGateSvc_t m_storeGate;

  /// MsgStream instance (a std::cout like with print-out levels)
  MsgStream m_msg;

  // Containers
  
  /// Name of the input data
  std::string m_dataName;

  /// Number of payload objects
  std::size_t m_nObjs;

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> STOREGATETESTS_SGSTRESSCONSUMER_H
