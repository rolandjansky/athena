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

#include "AsgTools/PropertyWrapper.h"
#include "AsgTools/AsgTool.h"
#include "AsgDataHandles/ReadHandleKey.h"
#include "JetInterface/IJetProvider.h"
#include "xAODJet/JetContainer.h"
#include "xAODCore/ShallowAuxContainer.h"

class JetCopier
  : public asg::AsgTool,
    virtual public IJetProvider
{
  ASG_TOOL_CLASS(JetCopier, IJetProvider)

  public:
    using asg::AsgTool::AsgTool;

    virtual StatusCode initialize() override;

    virtual StatusCode getAndRecordJets(SG::WriteHandle<xAOD::JetContainer>& jetHandle) const override;

    virtual std::pair<std::unique_ptr<xAOD::JetContainer>, std::unique_ptr<SG::IAuxStore> > getJets() const override;
    virtual std::pair<std::unique_ptr<xAOD::JetContainer>, std::unique_ptr<SG::IAuxStore> > ShallowCopyJets() const;
    virtual std::pair<std::unique_ptr<xAOD::JetContainer>, std::unique_ptr<SG::IAuxStore> > DeepCopyJets() const;

  private:
    // Handle Input JetContainer
    SG::ReadHandleKey<xAOD::JetContainer> m_inputJets {this, "InputJets", "", "Jet collection to be copied"};

    Gaudi::Property<bool> m_shallowCopy {this, "ShallowCopy", true, "True for shallow copy, false for deep copy"};
    Gaudi::Property<bool> m_shallowIO {this, "ShallowIO", false, "True for storing only modified data"};
};

#endif
