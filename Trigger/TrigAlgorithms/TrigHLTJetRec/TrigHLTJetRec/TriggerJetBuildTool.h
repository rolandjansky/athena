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
#include "JetInterface/IJetFinderMT.h"
#include "JetInterface/IJetModifier.h"
#include "xAODJet/JetContainerInfo.h"

class TriggerJetBuildTool : 
  public asg::AsgTool, virtual public ITriggerJetBuildTool{
  ASG_TOOL_CLASS(TriggerJetBuildTool, ITriggerJetBuildTool)
  
  public:
  
  TriggerJetBuildTool(const std::string& name);
  virtual ~TriggerJetBuildTool() override {}
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  // Set EDM inputs for jet finding
  virtual void prime(const xAOD::IParticleContainer*) override;
 
  virtual std::string toString(int verbosity) const override;
  void print() const override;

  virtual const xAOD::JetContainer* build() const override;

  //remove input PseudoJets
  virtual void resetInputs() override;

  virtual std::string dumpPseudoJets() const override;
  virtual const PseudoJetContainer* getInputPseudoJetContainer() const;

private:
  


  //IParticles 
  const xAOD::IParticleContainer* m_jetFinderInputs{nullptr};

  ToolHandle<IJetFinderMT> m_finder;
  ToolHandleArray<IJetModifier> m_modifiers;


  // std::string m_label; // labels Extractors, determines m_inputType
  std::string m_concreteTypeStr; // determines m_concreteType

  // m_inputType - updated from m_label, used 
  xAOD::JetInput::Type m_inputType{xAOD::JetInput::Uncategorized};
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
