/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ITriggerJetBuildTool.h

#ifndef ITriggerJetBuildTool_H
#define ITriggerJetBuildTool_H

/// ITriggerJetBuildTool is a dual-use tool interface for a tool
/// that builds a jet collection in the trigger environment

#include "JetInterface/IJetBuildTool.h"
#include "xAODBase/IParticleContainer.h"
#include <string>

class PseudoJetContainer;

class ITriggerJetBuildTool: virtual public IJetBuildTool {
ASG_TOOL_INTERFACE(ITriggerJetBuildTool)

public:

  virtual ~ITriggerJetBuildTool() { };

  // set inputs for jet finding. label is a type flag used 
  // for IParticle selection
  virtual void prime(const xAOD::IParticleContainer*) = 0;
  
  // Allow owner Algorithm to reset the Pseudojets (no managed by the evt store)
  virtual void resetInputs() = 0;

  virtual std::string toString(int verbosity) const = 0;
  virtual std::string dumpPseudoJets() const = 0;
  virtual
    const PseudoJetContainer* getInputPseudoJetContainer() const = 0;
};

#endif
