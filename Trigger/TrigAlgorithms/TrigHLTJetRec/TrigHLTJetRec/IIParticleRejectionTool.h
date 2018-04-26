/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETREC_IIPARTICLEREJECTOOl_H
#define JETREC_IIPARTICLEREJECTOOl_H

// PS March 2018

/// IIParticleRejectionTool is a dual-use tool interface for a tool
/// for rejecting IParticles.

#include "AsgTools/IAsgTool.h"
#include "xAODBase/IParticle.h"

class IIParticleRejectionTool : virtual public asg::IAsgTool {
ASG_TOOL_INTERFACE(IIParticleRejectionTool)

public:

  /// Destructor.
  virtual ~IIParticleRejectionTool() { };

  /// Method to build the collection and return it to the caller.
  /// Returns 0 for error.
  virtual bool operator()(const xAOD::IParticle*) = 0;
  virtual std::size_t tested() const = 0;
  virtual std::size_t rejected() const = 0;
};

#endif
