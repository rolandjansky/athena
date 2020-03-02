// this file is -*- C++ -*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETREC_JETCOPIER_H
#define JETREC_JETCOPIERW_H

///
/// \class JetCopier
///
/// Creates a new JetContainer by doing a shallow copy of an input JetVector
///
/// This tool implements the IJetProvider interface. The JetContainer it returns is build by
/// doing a shallow copy of an input JetVector.
/// The JetVector key is also a Property of the tool.
///


#include "StoreGate/ReadHandleKey.h"
#include "JetInterface/IJetProvider.h"
#include "xAODJet/JetContainer.h"
#include "JetRec/PseudoJetContainer.h"
#include "JetRec/JetFromPseudojet.h"

class JetCopier : public asg::AsgTool, virtual public IJetProvider{
  ASG_TOOL_CLASS(JetCopier, IJetProvider)

  public:
    JetCopier(std::string name);
    virtual StatusCode initialize() override;
    virtual xAOD::JetContainer* getJets() const override;

  private:
    // Handle Input JetContainer
    SG::ReadHandleKey<xAOD::JetContainer> m_inputJets {this, "InputJets", "inputjets", "input jets"};
};

#endif
