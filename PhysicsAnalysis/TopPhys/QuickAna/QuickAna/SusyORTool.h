/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__SUSY_OR_TOOL_H
#define QUICK_ANA__SUSY_OR_TOOL_H

/// This file provides the SUSY-specific overlap-removal code.
/// Like the default ORTool implementation, this wraps the OverlapRemovalTool
/// from Association Utils. However, the usage of the tool is a little bit
/// different.


#include <QuickAna/Global.h>

#include <QuickAna/AnaTool.h>
#include <AsgTools/AnaToolHandle.h>
#include <QuickAna/xAODInclude.h>

// OverlapRemovalTool doesn't forward declare it!
#include "AssociationUtils/OverlapDecorationHelper.h"

// Forward declarations
namespace ORUtils
{
  class IOverlapTool;
}

namespace ana
{
  /// @brief Implementation of the SUSY overlap-removal recommendations.
  /// This QuickAna tool wraps the OverlapRemovalTool and applies the custom
  /// overlap-removal prescription recommended by the SUSY working group.
  ///
  class SusyORTool : public AnaTool
  {
    //
    // public interface
    //

    ASG_TOOL_CLASS (SusyORTool, ana::IAnaTool)

  public:

    /// effects: standard constructor
    /// guarantee: strong
    /// failures: out of memory II
    SusyORTool (const std::string& name);


    /// \copydoc IAnaTool::setObjectType
    virtual StatusCode
    setObjectType (ObjectType type, const std::string& workingPoint) override;


    /// effects: initialize this tool
    /// guarantee: basic
    /// failures: configuration errors
    StatusCode initialize() override;


    /// returns: the processing step we are working on
    /// guarantee: no-fail
    virtual AnalysisStep step () const override;


    /// \copydoc IAnaTool::inputTypes
    virtual unsigned inputTypes () const override;


    /// \copydoc IAnaTool::outputTypes
    virtual unsigned outputTypes () const override;


    /// effects: apply this analysis tool to the objects
    /// guarantee: basic
    /// failures: tool dependent
    virtual StatusCode execute (IEventObjects& objects) override;

  protected:

    /// \brief print overlap removal debugging information
    void printObj (const char* APP_NAME, const char* type,
                   const xAOD::IParticle* obj) const;

  private:

    // Configuration

    /// Helper used to reset decorations
    std::unique_ptr<ORUtils::OverlapDecorationHelper> m_decHelper;

    /// The ele-mu overlap tool
    asg::AnaToolHandle<ORUtils::IOverlapTool> m_eleMuTool;
    /// The ele-jet overlap tool
    asg::AnaToolHandle<ORUtils::IOverlapTool> m_eleJetTool;
    /// The mu-jet overlap tool
    asg::AnaToolHandle<ORUtils::IOverlapTool> m_muJetTool;
    /// The pho-ele overlap tool
    asg::AnaToolHandle<ORUtils::IOverlapTool> m_phoEleTool;
    /// The pho-jet overlap tool
    asg::AnaToolHandle<ORUtils::IOverlapTool> m_phoJetTool;

    /// OR input decoration name
    std::string m_orInputLabel;

    /// OR output decoration name
    std::string m_orOutputLabel;

    /// B-jet decoration name
    std::string m_bJetLabel;

    /// Use a specialized muon OR
    bool m_customMuons;

  };
}

#endif
