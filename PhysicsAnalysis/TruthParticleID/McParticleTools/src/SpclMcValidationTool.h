///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SpclMcValidationTool.h 
// Header file for class SpclMcValidationTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLETOOLS_SPCLMCVALIDATIONTOOL_H 
#define MCPARTICLETOOLS_SPCLMCVALIDATIONTOOL_H 

// STL includes
#include <string>

// FrameWork includes

// McParticleTools includes
#include "TruthParticleValidationBaseTool.h"

// Forward declaration
class IIOMcAodTool;
class McEventCollection;
class TruthParticleContainer;

class SpclMcValidationTool : public TruthParticleValidationBaseTool
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  SpclMcValidationTool( const std::string& type,
			const std::string& name, 
			const IInterface* parent );

  /// Destructor: 
  virtual ~SpclMcValidationTool(); 

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
  SpclMcValidationTool();

  // to be implemented (if needed) by derived classes
  virtual StatusCode  initializeTool();
  virtual StatusCode  finalizeTool();

  /** Main method to perform the validation.
   *  Just a wrapper around the more API-detailed method.
   */
  virtual StatusCode  executeTool();

  /** Main method to perform the validation:
   *   - check that each vertex is identical to the original one
   */
  virtual StatusCode  executeTool( const McEventCollection* refMcEvents,
				   const TruthParticleContainer* mcParts );

  /** Request pointers to the IIOMcAodTool to be able to write 
   *  out TruthParticleContainer for further comparison (off-framework)
   */
  StatusCode setupMcAodWriterTool();
  
  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** Location of the reference McEventCollection
   */
  StringProperty m_mcEventsName;

  /** Location of the 'to-be-validated' TruthParticleContainer
   */
  StringProperty m_truthParticlesName;

  typedef ToolHandle<IIOMcAodTool> McAodTool_t;

  /** Tool to write the TruthParticleContainer into a dedicated file
   */
  McAodTool_t m_mcAodWriterTool;

}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> MCPARTICLETOOLS_SPCLMCVALIDATIONTOOL_H
