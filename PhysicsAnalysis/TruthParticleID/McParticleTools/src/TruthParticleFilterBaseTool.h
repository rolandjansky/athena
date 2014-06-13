///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TruthParticleFilterBaseTool.h 
// Header file for class TruthParticleFilterBaseTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLETOOLS_TRUTHPARTICLEFILTERBASETOOL_H 
#define MCPARTICLETOOLS_TRUTHPARTICLEFILTERBASETOOL_H 1

// STL includes
#include <string>

// HepMC / CLHEP includes
#include "GeneratorObjects/McEventCollection.h"

// FrameWork includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

// McParticleKernel includes
#include "McParticleKernel/ITruthParticleFilterTool.h"

// Forward declaration
class IMcVtxFilterTool;
class ITruthIsolationTool;

class TruthParticleFilterBaseTool : virtual public ITruthParticleFilterTool,
                                            public AthAlgTool
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  TruthParticleFilterBaseTool( const std::string& type,
			       const std::string& name, 
			       const IInterface* parent );

  /// Destructor: 
  virtual ~TruthParticleFilterBaseTool(); 

  // Athena algorithm's Hooks
  StatusCode  initialize();
  StatusCode  execute();
  StatusCode  finalize();

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

  /** Default constructor: 
   */
  TruthParticleFilterBaseTool();

  // to be implemented (if needed) by derived classes
  virtual StatusCode  initializeTool();
  virtual StatusCode  finalizeTool();

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  typedef ToolHandle<IMcVtxFilterTool> McVtxFilterTool_t;
  /** Pointer to the McVtxFilterTool to be able to select additional
   *  vertices on some decay pattern criterion
   */
  McVtxFilterTool_t m_mcVtxFilterTool;

  typedef ToolHandle<ITruthIsolationTool> IsolTool_t;
  /** Pointer to the TruthIsolationTool to be able to compute
   *  transverse energy isolations for various isolation cones cuts.
   *  See McParticleEvent/TruthParticleParameters.h for cone cuts.
   */
  IsolTool_t m_isolationTool;

  // switches
  
  /**
   * Switch to compute or not the Et-isolations for @c TruthParticle (and
   * their underlying @c HepMC::GenParticle).
   * Default is to not compute these Et-isolations (and save CPU)
   */
  BooleanProperty m_doEtIsolation;

  // Containers
  
  /** Location of the input McEventCollection one wants to filter
   */
  StringProperty m_mcEventsName;

  /** Location of the output McEventCollection which has been filtered
   */
  StringProperty m_mcEventsOutputName;

}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline
StatusCode
TruthParticleFilterBaseTool::initializeTool()
{
  return StatusCode::SUCCESS;
}

inline
StatusCode
TruthParticleFilterBaseTool::finalizeTool()
{
  return StatusCode::SUCCESS;
}

#endif //> MCPARTICLETOOLS_TRUTHPARTICLEFILTERBASETOOL_H
