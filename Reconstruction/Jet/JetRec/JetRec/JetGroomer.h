// this file is -*- C++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETREC_JETGROOMER_H
#define JETREC_JETGROOMER_H

///
/// \class JetGroomer
///
/// Base class for tools that create a new JetContainer by grooming an input jet collection
///
/// This tool implements the IJetProvider interface. Children should override (only)
/// the initialize and insertGroomedJet methods, to implement a specific instance of
/// jet grooming.
///

#include "AsgTools/AsgTool.h"

#include "AsgDataHandles/ReadHandleKey.h"
#include "AsgDataHandles/WriteHandleKey.h"

#include "JetInterface/IJetProvider.h"

#include "JetEDM/PseudoJetVector.h"
#include "JetRec/PseudoJetContainer.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

class JetGroomer
  : public asg::AsgTool,
    virtual public JetProvider<xAOD::JetAuxContainer>
{
  ASG_TOOL_CLASS(JetGroomer, IJetProvider)

  public:
    using asg::AsgTool::AsgTool;

    virtual StatusCode initialize() override;

    // From IJetProvider
    virtual std::pair<std::unique_ptr<xAOD::JetContainer>, std::unique_ptr<SG::IAuxStore> > getJets() const override final;

    // Implementation of grooming goes here
    // The jet is inserted into the output container, which is necessary for speed
    // in the xAOD container paradigm
  virtual void insertGroomedJet(const xAOD::Jet&, const PseudoJetContainer&, xAOD::JetContainer&, PseudoJetVector&) const = 0;

  protected:
  /// Handle Input JetContainer (this contains the parent ungroomed jets to be trimmed)
  SG::ReadHandleKey<xAOD::JetContainer> m_inputJetContainer {this, "UngroomedJets", "ungroomedinput", "Input ungroomed jet container"};

  /// This is the input to the parent JetContainer. It is needed in order to re-assign the ghost constituents from the final groomed PJ to the xAOD::Jet
  SG::ReadHandleKey<PseudoJetContainer> m_inputPseudoJets {this, "ParentPseudoJets", "inputpseudojet", "input constituents of parent JetContainer"};

   SG::WriteHandleKey<PseudoJetVector> m_finalPseudoJets {this, "FinalPseudoJets_DONOTSET", "", "output pseudojets -- autoconfigured name"};

};

#endif
