///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ReadTruthParticles.h 
// Header file for class ReadTruthParticles
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ANALYSISEXAMPLES_READTRUTHPARTICLES_H 
#define ANALYSISEXAMPLES_READTRUTHPARTICLES_H 

/** @class ReadTruthParticles is an example algorithm which exercises the 
 *  TruthParticle interface. It shows how to navigate back to the 
 *  HepMC::GenParticles and retrieve the children (in a HepMC format) as well 
 *  as how to retrieve the (TruthParticle) children of a TruthParticle.
 *  The Algorithm first retrieves a container of TruthParticle, loop over its 
 *  elements and for each of them asks for its HepMC::GenParticle creator (and 
 *  use the GenParticle methods).
 *  Then, for each TruthParticle, it loops over its children in a TruthParticle
 *  format.
 */

// STL includes
#include <string>

// HepMC / CLHEP includes

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// Forward declaration
class StoreGateSvc;
class ITruthParticleCnvTool;

class ReadTruthParticles : public AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  ReadTruthParticles( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~ReadTruthParticles(); 

  // Assignment operator: 
  //ReadTruthParticles &operator=(const ReadTruthParticles &alg); 

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
  ReadTruthParticles();

  // Containers

  /// name of the AOD truth particle container to retrieve from Store Gate
  std::string m_truthParticlesName;

  typedef ToolHandle<ITruthParticleCnvTool> CnvTool_t;
  /** converter tool McEventCollection -> TruthParticle */
  CnvTool_t m_cnvTool;
  
  /** switch to build TruthParticles from ESD TruthEvent */
  bool m_fromESD;

}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> ANALYSISEXAMPLES_READTRUTHPARTICLES_H

