/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// ITruthParticleFilterTool.h 
// Header file for class ITruthParticleFilterTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEKERNEL_ITRUTHPARTICLEFILTERTOOL_H 
#define MCPARTICLEKERNEL_ITRUTHPARTICLEFILTERTOOL_H 

// STL includes

// HepMC / CLHEP includes

// FrameWork includes
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IProperty.h"

// Forward declaration
class McEventCollection;

static const InterfaceID IID_ITruthParticleFilterTool("ITruthParticleFilterTool", 1, 0);

class ITruthParticleFilterTool : virtual public IAlgTool,
				 virtual public IProperty
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Destructor: 
   */
  virtual ~ITruthParticleFilterTool();

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

  /** This method will check the validity of the input McEventCollection 
   *  and build a filtered one from the strategy implemented by the 
   *  concrete tool.
   *  It is not const to allow derived tools to build statistics during
   *  the filtering process.
   */
  virtual StatusCode buildMcAod( const McEventCollection* in,
				 McEventCollection* filtered ) = 0;

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
inline const InterfaceID& ITruthParticleFilterTool::interfaceID() 
{ 
   return IID_ITruthParticleFilterTool; 
}


#endif //> MCPARTICLEKERNEL_ITRUTHPARTICLEFILTERTOOL_H
