///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TruthParticleValidationBaseTool.h 
// Header file for class TruthParticleValidationBaseTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLETOOLS_TRUTHPARTICLEVALIDATIONBASETOOL_H 
#define MCPARTICLETOOLS_TRUTHPARTICLEVALIDATIONBASETOOL_H 1

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

// McParticleKernel includes
#include "McParticleKernel/ITruthParticleValidationTool.h"

// Forward declaration

class TruthParticleValidationBaseTool : virtual public ITruthParticleValidationTool,
                                                public AthAlgTool
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  TruthParticleValidationBaseTool( const std::string& type,
				   const std::string& name, 
				   const IInterface* parent );

  /// Destructor: 
  virtual ~TruthParticleValidationBaseTool(); 

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
  TruthParticleValidationBaseTool();

  // to be implemented (if needed) by derived classes
  virtual StatusCode  initializeTool();
  virtual StatusCode  finalizeTool();

  /** Main method to perform the validation
   */
  virtual StatusCode  executeTool() = 0;

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** Counter of analysed events
   */
  unsigned int m_nAnalysedEvents;
}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline
StatusCode
TruthParticleValidationBaseTool::initializeTool()
{
  return StatusCode::SUCCESS;
}

inline
StatusCode
TruthParticleValidationBaseTool::finalizeTool()
{
  return StatusCode::SUCCESS;
}

#endif //> MCPARTICLETOOLS_TRUTHPARTICLEVALIDATIONBASETOOL_H
