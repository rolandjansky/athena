///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// WriteTruthParticles.h 
// Header file for class WriteTruthParticles
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ANALYSISEXAMPLES_WRITETRUTHPARTICLES_H 
#define ANALYSISEXAMPLES_WRITETRUTHPARTICLES_H 

// STL includes
#include <string>

// HepMC / CLHEP includes

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Forward declaration
class ITruthParticleCnvTool;

class WriteTruthParticles : public AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  WriteTruthParticles( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~WriteTruthParticles(); 

  // Assignment operator: 
  //WriteTruthParticles &operator=(const WriteTruthParticles &alg); 

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
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /// Default constructor: 
  WriteTruthParticles();

  typedef ToolHandle<ITruthParticleCnvTool> CnvTool_t;
  /** Pointer to the AlgTool which will perform the conversion
   *  McEventCollection -> TruthParticleContainer
   */
  CnvTool_t m_cnvTool;

}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> ANALYSISEXAMPLES_WRITETRUTHPARTICLES_H
