/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__EVENTINFO_TOOL_H
#define QUICK_ANA__EVENTINFO_TOOL_H

// This module still needs to be documented.  The interface provided
// in this module is intended for experts only.  The module is
// considered to be in the pre-alpha stage.



#include <QuickAna/Global.h>

#include <QuickAna/AnaTool.h>

namespace ana
{
  /// TODO: this class needs documentation
  class EventInfoTool : public AnaTool
  {
    //
    // public interface
    //

    ASG_TOOL_CLASS (EventInfoTool, ana::IAnaTool)


    /// effects: standard constructor
    /// guarantee: strong
    /// failures: out of memory II
  public:
    EventInfoTool (const std::string& name);


    /// \copydoc IAnaTool::setObjectType
  public:
    virtual StatusCode
    setObjectType (ObjectType type, const std::string& workingPoint) override;


    /// effects: initialize this tool
    /// guarantee: basic
    /// failures: configuration errors
  public:
    StatusCode initialize() override;


    /// returns: the processing step we are working on
    /// guarantee: no-fail
  public:
    virtual AnalysisStep step () const override;


    /// \copydoc IAnaTool::inputTypes
  public:
    virtual unsigned inputTypes () const override;


    /// \copydoc IAnaTool::outputTypes
  public:
    virtual unsigned outputTypes () const override;


    /// effects: apply this analysis tool to the objects
    /// guarantee: basic
    /// failures: tool dependent
  public:
    virtual StatusCode execute (IEventObjects& objects) override;


    /// description: the EVENTINFO tool
  public:
    //
  };
}

#endif
