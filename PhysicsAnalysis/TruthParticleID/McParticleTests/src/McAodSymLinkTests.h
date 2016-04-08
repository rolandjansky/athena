///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// McAodSymLinkTests.h 
// Header file for class McAodSymLinkTests
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLETESTS_MCAODSYMLINKTESTS_H 
#define MCPARTICLETESTS_MCAODSYMLINKTESTS_H 

// STL includes
#include <string>

// HepMC / CLHEP includes

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"

// Forward declaration
class StoreGateSvc;
class TruthParticleContainer;

class McAodSymLinkTests : public AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  McAodSymLinkTests( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~McAodSymLinkTests(); 

  // Assignment operator: 
  //McAodSymLinkTests &operator=(const McAodSymLinkTests &alg); 

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
  McAodSymLinkTests();

  /// Create a HepMC::GenEvent  and put it into a McEventCollection
  //StatusCode createGenEvent();

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

  /// Input location of the @c TruthParticleContainer
  StringProperty m_truthParticlesName;

};

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> MCPARTICLETESTS_MCAODSYMLINKTESTS_H
