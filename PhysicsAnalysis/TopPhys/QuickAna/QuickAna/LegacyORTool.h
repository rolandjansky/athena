/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__LEGACY_OR_TOOL_H
#define QUICK_ANA__LEGACY_OR_TOOL_H

// This module still needs to be documented.  The interface provided
// in this module is intended for experts only.  The module is
// considered to be in the pre-alpha stage.



#include <QuickAna/Global.h>

#include <QuickAna/AnaTool.h>
#include <AsgTools/AnaToolHandle.h>
#include <QuickAna/xAODInclude.h>

class IOverlapRemovalTool;

namespace ana
{
  /// @brief An AnaTool which provides the object overlap removal
  ///
  /// This implementation uses the legacy overlap removal tool from
  /// AssociationUtils. It is going to be deprecated in favor of the new
  /// ORTool based on the multi-tool design of AssociationUtils.
  ///
  class LegacyORTool : public AnaTool
  {
    //
    // public interface
    //

    ASG_TOOL_CLASS (LegacyORTool, ana::IAnaTool)

  public:

    /// effects: standard constructor
    /// guarantee: strong
    /// failures: out of memory II
    LegacyORTool (const std::string& name);


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


    /// Enum for specifying various overlap removal variants
    enum OR_Type {
      Default = 0,
      run1_top_default,
      run1_SSbjets,
      run1_susy_default
    };

  protected:

    /// \brief print overlap removal debugging information
    void printObj (const char* APP_NAME, const char* type,
                   const xAOD::IParticle* obj) const;

  private:

    // Configuration

    /// description: the OverlapRemoval tool
    asg::AnaToolHandle<IOverlapRemovalTool> m_ortool;

    /// OR input decoration name
    std::string m_orInputLabel;

    /// OR output decoration name
    std::string m_orOutputLabel;

    /// Which OR recommendations are we following?
    // warning: please leave this as a simple integer property, or it
    //   will not compile in Athena!!!
    // OR_Type m_or_type;
    int m_or_type;

  };
}

#endif
