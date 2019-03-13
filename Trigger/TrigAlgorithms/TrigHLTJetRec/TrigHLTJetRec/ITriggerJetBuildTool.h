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
#include "xAODJet/JetContainer.h"
#include "AsgTools/IAsgTool.h"
#include <string>

class PseudoJetContainer;

namespace fastjet{
  class ClusterSequence;
}


// class ITriggerJetBuildTool: virtual public IJetBuildTool {
class ITriggerJetBuildTool: virtual public asg::IAsgTool {
ASG_TOOL_INTERFACE(ITriggerJetBuildTool)

public:

  virtual ~ITriggerJetBuildTool() { };

  // set inputs for jet finding. label is a type flag used 
  // for IParticle selection
  virtual void prime(const xAOD::IParticleContainer*) = 0;
  virtual void primeGhost(const xAOD::IParticleContainer*, std::string ghostlabel) = 0;
  virtual int build(fastjet::ClusterSequence*&, xAOD::JetContainer*&) const = 0;
  
  // Allow owner Algorithm to reset the Pseudojets (no managed by the evt store)
  virtual void resetInputs() = 0;

  virtual std::string toString(int verbosity) const = 0;
  virtual std::string dumpPseudoJets() const = 0;
  virtual
    const PseudoJetContainer* getInputPseudoJetContainer() const = 0;
};

#endif
