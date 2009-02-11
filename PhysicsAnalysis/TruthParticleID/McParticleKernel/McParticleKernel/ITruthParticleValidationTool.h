/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// ITruthParticleValidationTool.h 
// Header file for class ITruthParticleValidationTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEKERNEL_ITRUTHPARTICLEVALIDATIONTOOL_H 
#define MCPARTICLEKERNEL_ITRUTHPARTICLEVALIDATIONTOOL_H 

/** @class ITruthParticleValidationTool is the interface towards AlgTools which
 *  aim at validating the process of McAOD making.
 *  Too much troubles in the past releases made this "software" validation
 *  very critical.
 *  All the validation is done during the execute() method which will be called
 *  by the McParticleAlgs/McAodValidationAlg algorithm.
 *  Each instance of an AlgTool deriving from @c ITruthParticleValidationTool
 *  will implement a very specific test (vertex momentum conservation, particle
 *  reconnection,... you name it).
 *  Everything will be steered by job-options.
 */

// STL includes

// HepMC / CLHEP includes

// FrameWork includes
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IProperty.h"

// Forward declaration

static const InterfaceID IID_ITruthParticleValidationTool("ITruthParticleValidationTool", 1, 0);

class ITruthParticleValidationTool : virtual public IAlgTool,
				     virtual public IProperty
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Destructor: 
   */
  virtual ~ITruthParticleValidationTool();

  // Athena algorithm's Hooks
  virtual StatusCode  initialize() = 0;
  virtual StatusCode  execute()    = 0;
  virtual StatusCode  finalize()   = 0;

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////
  static const InterfaceID& interfaceID();

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 
inline const InterfaceID& ITruthParticleValidationTool::interfaceID() 
{ 
   return IID_ITruthParticleValidationTool; 
}


#endif //> MCPARTICLEKERNEL_ITRUTHPARTICLEVALIDATIONTOOL_H
