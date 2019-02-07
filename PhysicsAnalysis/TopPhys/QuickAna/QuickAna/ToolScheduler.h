/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef QUICK_ANA__TOOL_SCHEDULER_H
#define QUICK_ANA__TOOL_SCHEDULER_H

// This module still needs to be documented.  The interface provided
// in this module is intended for experts only.  The module is
// considered to be in the pre-alpha stage.



#include <QuickAna/Global.h>

#include <AsgTools/AsgTool.h>
#include <QuickAna/EventObjects.h>
#include <QuickAna/IToolScheduler.h>

namespace CP
{
  class SystematicSet;
}

namespace ana
{
  /// TODO: needs documentation
  class ToolScheduler : virtual public IToolScheduler, virtual public asg::AsgTool
  {
    //
    // public interface
    //

    ASG_TOOL_CLASS (ToolScheduler, ana::IToolScheduler)


    /// effects: test the invariant of this object
    /// guarantee: no-fail
  public:
    void testInvariant () const;


    /// effects: standard constructor
    /// guarantee: strong
    /// failures: out of memory II
  public:
    ToolScheduler (const std::string& name);


    /// \brief standard destructor
    ///
    /// I defined my own destructor to break include file dependencies
    /// in Athena
    /// \par Guarantee
    ///   no-fail
  public:
    ~ToolScheduler ();



    //
    // inherited interface
    //

  public:
    virtual StatusCode initialize() override;

  public:
    virtual StatusCode addTool (std::unique_ptr<IAnaTool> tool) override;

  public:
    virtual CP::SystematicSet affectingSystematics () const override;

  public:
    virtual CP::SystematicSet recommendedSystematics() const override;

  public:
    virtual StatusCode
    applySystematicVariation (const CP::SystematicSet& systConfig) override;

  public:
    virtual StatusCode
    fillEventObjects (IEventObjects*& object) override;

    /// \copydoc IToolScheduler::systematics
  public:
    virtual const std::vector<CP::SystematicSet>& systematics () const override;

  public:
    virtual StatusCode
    setSystematics (const std::vector<CP::SystematicSet>& val_systematics)
      override;

  public:
    virtual EventData getEventData () const override;



    //
    // private interface
    //

    /// description: the list of object definitions we are using
  private:
    std::vector<std::unique_ptr<IAnaTool> > m_tools;

    /// description: the event objects
  private:
    EventObjects m_objects;

    /// \brief the current postfix
  private:
    std::string m_postfix;

    /// \brief the value of \ref systematics
  private:
    std::vector<CP::SystematicSet> m_systematics;
  };
}

#endif
