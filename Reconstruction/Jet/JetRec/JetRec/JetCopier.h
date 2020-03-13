// this file is -*- C++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETREC_JETCOPIER_H
#define JETREC_JETCOPIER_H

///
/// \class JetCopier
///
/// Creates a new JetContainer by doing a shallow copy of an input JetVector
///
/// This tool implements the IJetProvider interface. The JetContainer it returns is build by
/// doing a shallow copy of an input JetVector.
/// The JetVector key is also a Property of the tool.
///

#include "AsgTools/AsgTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "JetInterface/IJetProvider.h"
#include "xAODJet/JetContainer.h"
#include "xAODCore/ShallowAuxContainer.h"

class JetCopier
  : public asg::AsgTool,
    virtual public JetProvider<xAOD::ShallowAuxContainer>
{
  ASG_TOOL_CLASS(JetCopier, IJetProvider)

  public:
    using asg::AsgTool::AsgTool;

    virtual StatusCode initialize() override;
    virtual std::pair<std::unique_ptr<xAOD::JetContainer>, std::unique_ptr<SG::IAuxStore> > getJets() const override;

  private:
    // Handle Input JetContainer
    SG::ReadHandleKey<xAOD::JetContainer> m_inputJets {this, "InputJets", "", "Jet collection to be copied"};
};

#endif
