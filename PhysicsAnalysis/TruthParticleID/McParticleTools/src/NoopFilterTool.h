///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// NoopFilterTool.h 
// Header file for class NoopFilterTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLETOOLS_NOOPFILTERTOOL_H 
#define MCPARTICLETOOLS_NOOPFILTERTOOL_H 

// STL includes
#include <string>

// HepMC / CLHEP includes

// McParticleTools includes
#include "TruthParticleFilterBaseTool.h"

// Forward declaration

class NoopFilterTool : public TruthParticleFilterBaseTool
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  NoopFilterTool( const std::string& type,
		  const std::string& name, 
		  const IInterface* parent );

  /// Destructor: 
  virtual ~NoopFilterTool(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /** This method will check the validity of the input McEventCollection 
   *  and build a filtered one from the strategy implemented by this
   *  concrete tool.
   */
  StatusCode buildMcAod( const McEventCollection* in, McEventCollection* out );

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** Default constructor: 
   */
  NoopFilterTool();

  // Containers
  

}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> MCPARTICLETOOLS_NOOPFILTERTOOL_H
