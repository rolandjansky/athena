///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// McAodValidationAlg.h 
// Header file for class McAodValidationAlg
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEALGS_MCAODVALIDATIONALG_H 
#define MCPARTICLEALGS_MCAODVALIDATIONALG_H 

// STL includes
#include <string>

// HepMC / CLHEP includes

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Forward declaration
class ITruthParticleValidationTool;

class McAodValidationAlg : public AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  McAodValidationAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~McAodValidationAlg(); 

  // Assignment operator: 
  //McAodValidationAlg &operator=(const McAodValidationAlg &alg); 

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
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /// Default constructor: 
  McAodValidationAlg();

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  typedef ToolHandleArray<ITruthParticleValidationTool> IValidationTools_t;
  /** Validation tools to be ran on the events
   */
  IValidationTools_t m_valTools;

  // Containers
  
  /// Location of the TruthParticleContainer to read
  StringProperty m_truthParticlesName;
  
  // switches
  
  /// Random generator seed
  unsigned int m_seed;
  
}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> MCPARTICLEALGS_MCAODVALIDATIONALG_H
