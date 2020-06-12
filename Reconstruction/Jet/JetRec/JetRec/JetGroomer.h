// this file is -*- C++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETREC_JETGROOMER_H
#define JETREC_JETGROOMER_H

///
/// \class JetGroomer
///
/// Creates a new JetContainer by grooming an input jet collection
///
/// This tool implements the IJetProvider interface. The JetContainer it returns is built by
/// running an IJetGroomer tool (e.g. JetTrimmer) on the input jets.
///

#include "AsgTools/AsgTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "JetInterface/IJetProvider.h"
#include "JetInterface/IJetGroomer.h"
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
    virtual std::pair<std::unique_ptr<xAOD::JetContainer>, std::unique_ptr<SG::IAuxStore> > getJets() const override;

  private:
    // Handle Input JetContainer
    SG::ReadHandleKey<xAOD::JetContainer> m_inputJetsKey {this, "UngroomedJets", "", "Jet collection to be groomed"};

    // Handle Input PseudoJetContainer
    // Needed to extract the constituents
    SG::ReadHandleKey<PseudoJetContainer> m_inputPseudoJetsKey {this, "ParentPseudoJets", "", "Input constituents"};

    ToolHandle<IJetGroomer> m_groomer ={this , "Groomer" , {} , "Tool grooming the jets (trim, prune, softdrop etc)"};

};

#endif
