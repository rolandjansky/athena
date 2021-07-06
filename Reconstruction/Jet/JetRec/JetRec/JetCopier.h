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
#include "AsgDataHandles/WriteHandleKey.h"
#include "JetInterface/IJetProvider.h"
#include "xAODJet/JetContainer.h"
#include "xAODCore/ShallowAuxContainer.h"

// This class doesn't (yet) exist for AnalysisBase, so in that release
// we will simply have to rerun modifiers if we need them.
#ifndef XAOD_STANDALONE
#include "StoreGate/ShallowCopyDecorDeps.h"
#endif

class JetCopier
  : public asg::AsgTool,
    virtual public IJetProvider
{
  ASG_TOOL_CLASS(JetCopier, IJetProvider)

  public:
    using asg::AsgTool::AsgTool;

    // Called in parent initialize()
    virtual StatusCode initialize() override;

#ifndef XAOD_STANDALONE
    // Needed to initialise the ShallowCopyDecorDeps object, which propagates
    // decorations on the original into the copy in StoreGate.
    // Override interface implementation in Athena only
    virtual StatusCode initWithOutput(const SG::WriteHandleKey<xAOD::JetContainer>& outputJets) override;
#endif

    // Called during execution
    virtual StatusCode getAndRecordJets(SG::WriteHandle<xAOD::JetContainer>& jetHandle) const override;
    virtual std::pair<std::unique_ptr<xAOD::JetContainer>, std::unique_ptr<SG::IAuxStore> > getJets() const override;

  private:

    virtual std::pair<std::unique_ptr<xAOD::JetContainer>, std::unique_ptr<SG::IAuxStore> > ShallowCopyJets() const;
    virtual std::pair<std::unique_ptr<xAOD::JetContainer>, std::unique_ptr<SG::IAuxStore> > DeepCopyJets() const;

    // Handle Input JetContainer
    SG::ReadHandleKey<xAOD::JetContainer> m_inputJets {this, "InputJets", "", "Jet collection to be copied"};

    Gaudi::Property<bool> m_shallowCopy {this, "ShallowCopy", true, "True for shallow copy, false for deep copy"};
    Gaudi::Property<bool> m_shallowIO {this, "ShallowIO", false, "True for storing only modified data"};

#ifndef XAOD_STANDALONE
    SG::ShallowCopyDecorDeps<xAOD::JetContainer> m_decorDeps { this, "DecorDeps", {},
          "List of decorations to propagate through the shallow copy." };
#endif
};

#endif
