// JtRecToolTrigger.h. This file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TriggerJetBuildTool_H
#define TriggerJetBuildTool_H

// P Sherwood Dec 2017

/// \class TriggerJetBuildTool
///
/// This class contains the Run2 trigger specifics for JetRecTool
///
/// The tool receives its imputs fro the trigger Algrithm 
/// via the prime  method. Due to the constness of the build method,
/// The trigger Algorithm is also reposnsible for clearing the tool
/// before reusing it.
/// the execute() method delegates jet building to an IJetFinder object,
/// and then calls jet modifiers to act on th newly formed jet.

#include "JetRec/PseudoJetContainer.h"
#include "TrigHLTJetRec/IIParticleRejectionTool.h"
#include "TrigHLTJetRec/ITriggerJetBuildTool.h"
#include "JetInterface/IJetExecuteTool.h"
#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetFinder.h"
#include "JetInterface/IJetModifier.h"
#include "xAODJet/JetContainerInfo.h"

class TriggerJetBuildTool : 
  public asg::AsgTool, virtual public ITriggerJetBuildTool{
  ASG_TOOL_CLASS(TriggerJetBuildTool, ITriggerJetBuildTool)
  
  public:
  
  TriggerJetBuildTool(const std::string& name);
  virtual ~TriggerJetBuildTool() override {}
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  // Set EDM inputs for jet finding
  virtual void prime(const xAOD::IParticleContainer*) override;
  virtual void primeGhost(const xAOD::IParticleContainer*, std::string ghostlabel) override;

  virtual int build(fastjet::ClusterSequence*&, xAOD::JetContainer*&) const override;
  
  virtual std::string toString(int verbosity) const override;
  void print() const override;

  //remove input PseudoJets
  virtual void resetInputs() override;

  virtual std::string dumpPseudoJets() const override;
  virtual const PseudoJetContainer* getInputPseudoJetContainer() const override;

private:
  


  //IParticles 
  ToolHandle<IJetFinder> m_finder;
  ToolHandleArray<IJetModifier> m_modifiers;


  std::string m_concreteTypeStr; // determines m_concreteType

  // m_concreteType -  affects IParticle->PseudoJet 
  xAOD::JetInput::Type m_concreteType{xAOD::JetInput::Uncategorized};

  // Store each PseudoJetContainer made by a call to prime();
  PseudoJetContainer m_inContainer;

  bool m_noNegE{true};
  bool m_isTrigger{true};
  ToolHandle<IIParticleRejectionTool> m_iParticleRejecter;

  std::size_t m_nprime{0};
};

#endif
