/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__OR_TOOL_H
#define QUICK_ANA__OR_TOOL_H

/// This file defines the overlap removal tool: ORTool

#include <QuickAna/Global.h>

#include <QuickAna/AnaTool.h>
#include <AsgTools/AnaToolHandle.h>
#include <AssociationUtils/ToolBox.h>
#include <AssociationUtils/OverlapRemovalInit.h>
#include <QuickAna/xAODInclude.h>

// Forward declarations
namespace ORUtils
{
  class IOverlapRemovalTool;
  class IOverlapTool;
}

namespace ana
{

  /// @brief QuickAna tool for applying standard overlap removal.
  ///
  /// This AnaTool uses the overlap removal tools from AssociationUtils.
  /// It is able to apply either the harmonization note recommendations or the
  /// updated overlap removal recommendations via the RecommendationSet
  /// configuration property.
  ///
  /// For more details on the overlap removal recommendations, see the
  /// AssociationUtils documentation:
  /// https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/AnalysisCommon/AssociationUtils/trunk/doc/README.rst
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class ORTool : public AnaTool
  {

    ASG_TOOL_CLASS(ORTool, ana::IAnaTool)

  public:

    /// @brief Standard constructor
    ///
    /// @par guarantee: strong
    /// @par failures: out of memory II
    ORTool(const std::string& name);

    /// \copydoc IAnaTool::setObjectType
    virtual StatusCode
    setObjectType(ObjectType type, const std::string& workingPoint) override;

    /// @brief Initialize this tool
    ///
    /// @par guarantee: basic
    /// @par failures: configuration errors
    StatusCode initialize() override;

    /// @brief Returns the processing step we are working on
    ///
    /// @par guarantee: no-fail
    virtual AnalysisStep step() const override;

    /// \copydoc IAnaTool::inputTypes
    virtual unsigned inputTypes() const override;

    /// \copydoc IAnaTool::outputTypes
    virtual unsigned outputTypes() const override;

    /// @brief Apply overlap removal to the objects.
    ///
    /// @par guarantee: basic
    /// @par failures: tool dependent
    virtual StatusCode execute(IEventObjects& objects) override;

  private:

    // Configuration

    /// The OR toolbox
    ORUtils::ToolBox m_orToolBox;

    /// OR config flags
    ORUtils::ORFlags m_orFlags;

    /// QuickAna analysis selection name
    std::string m_anaSelectionName;

    /// Sliding-DR option: 'muon', 'electron', 'both'
    std::string m_slidingConeType;

    /// Use JVT for prioritization
    bool m_jvtPriorities;

    /// Muon Jet removal pT
    bool m_applyRelPt;

    /// WP for 4l, only reject jets
    bool m_onlyRejJets;

  }; // class ORTool

} // namespace ana

#endif
