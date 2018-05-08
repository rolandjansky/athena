/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__CLEANING_TOOL_H
#define QUICK_ANA__CLEANING_TOOL_H

/// This file defines the event level jet cleaning tool: CleanTool

#include <QuickAna/Global.h>

#include <QuickAna/AnaTool.h>
#include <AsgTools/AnaToolHandle.h>
#include <QuickAna/xAODInclude.h>

// Forward declarations

class IJetSelector;
namespace ECUtils
{
class IEventCleaningTool;
}

namespace ana
{

  /// @brief QuickAna tool for applying event level jet cleaning.
  ///
  class CleanTool : public AnaTool
  {

    ASG_TOOL_CLASS(CleanTool, ana::IAnaTool)

  public:

    /// @brief Standard constructor
    ///
    /// @par guarantee: strong
    /// @par failures: out of memory II
    CleanTool(const std::string& name);

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

    /// @brief Apply event level cleaning to the jets.
    ///
    /// @par guarantee: basic
    /// @par failures: tool dependent
    virtual StatusCode execute(IEventObjects& objects) override;

  private:

    // Configuration

    asg::AnaToolHandle<IJetSelector> m_jetCleaning_tool;
    asg::AnaToolHandle<ECUtils::IEventCleaningTool> m_evtCleaning_tool;

    std::string m_workingPoint;
    std::string m_jvtDec;
    std::string m_orDec;
    float m_ptCut;
    float m_etaCut;


  }; // class CleanTool

} // namespace ana

#endif
