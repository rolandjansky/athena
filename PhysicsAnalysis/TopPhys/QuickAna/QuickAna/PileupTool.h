/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef QUICK_ANA__PILEUP_TOOL_H
#define QUICK_ANA__PILEUP_TOOL_H

// This module still needs to be documented.  The interface provided
// in this module is intended for experts only.  The module is
// considered to be in the pre-alpha stage.



#include <QuickAna/Global.h>

#include <AthContainers/AuxElement.h>
#include <QuickAna/AnaTool.h>
#include <AsgTools/AnaToolHandle.h>

namespace CP {class IPileupReweightingTool;}

namespace ana
{
  /// @class PileupTool
  /// @brief QuickAna tool which wraps and applies the pileup reweighting
  ///
  /// Official pileup reweighting twiki page:
  /// https://twiki.cern.ch/twiki/bin/view/AtlasProtected/ExtendedPileupReweighting
  ///
  class PileupTool : virtual public AnaTool
  {
    //
    // public interface
    //

    ASG_TOOL_CLASS (PileupTool, ana::IAnaTool)

  public:

    /// effects: standard constructor
    /// guarantee: strong
    /// failures: out of memory II
    PileupTool (const std::string& name);

    /// \copydoc IAnaTool::setObjectType
    virtual StatusCode
    setObjectType (ObjectType type, const std::string& workingPoint) override;

    /// returns: the processing step we are working on
    /// guarantee: no-fail
    virtual AnalysisStep step () const override;

    /// \copydoc IAnaTool::inputTypes
    virtual unsigned inputTypes () const override;

    /// \copydoc IAnaTool::outputTypes
    virtual unsigned outputTypes () const override;

    /// effects: initialize this tool
    /// guarantee: basic
    /// failures: configuration errors
    StatusCode initialize() override;

    /// effects: apply this analysis tool to the objects
    /// guarantee: basic
    /// failures: tool dependent
    virtual StatusCode execute (IEventObjects& objects) override;

    /// \brief the files with the vertex distribution in data
  public:
    std::vector<std::string> muDataFiles;

    /// \brief the files with the vertex distribution for the current MC
  public:
    std::vector<std::string> muMcFiles;

    /// \brief the pileup reweighting tool itself
  private:
    asg::AnaToolHandle<CP::IPileupReweightingTool> m_tool;

  };
}

#endif
