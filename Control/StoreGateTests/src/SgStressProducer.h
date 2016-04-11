///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SgStressProducer.h 
// Header file for class SgStressProducer
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef STOREGATETESTS_SGSTRESSPRODUCER_H 
#define STOREGATETESTS_SGSTRESSPRODUCER_H 

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"

// Forward declaration
template<class Data>
class DataPool;
namespace SgTests { struct PayLoad; }

class SgStressProducer : public AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  SgStressProducer( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~SgStressProducer(); 

  // Assignment operator: 
  //SgStressProducer &operator=(const SgStressProducer &alg); 

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
  SgStressProducer();

  /// Create data
  StatusCode createData();

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  // Containers
  
  /// Name of the input data
  std::string m_dataName;

  /// Number of payload objects
  std::size_t m_nObjs;

  /// Size of the payload objects
  std::size_t m_dataSize;

  /// switch to use or not an arena (memory pool) to allocate payload
  bool m_useDataPool;
}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> STOREGATETESTS_SGSTRESSPRODUCER_H
