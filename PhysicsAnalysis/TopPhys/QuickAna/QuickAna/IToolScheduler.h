/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__I_TOOL_SCHEDULER_H
#define QUICK_ANA__I_TOOL_SCHEDULER_H

//        Copyright Iowa State University 2014.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


// This module still needs to be documented.  The interface provided
// in this module is intended for experts only.  The module is
// considered to be in the pre-alpha stage.



#include <QuickAna/Global.h>

#include <AsgTools/IAsgTool.h>
#include <vector>
#include <memory>

namespace CP
{
  class SystematicSet;
}

namespace ana
{
  /// TODO: needs documentation
  class IToolScheduler : virtual public asg::IAsgTool
  {
    //
    // public interface
    //

    ASG_TOOL_INTERFACE(IToolScheduler)


    /// effects: add all the tools from the configuration
    /// guarantee: basic
    /// failures: tool creation errors
    /// failures: out of memory III
  public:
    StatusCode addTools (const Configuration& conf, InternalConfiguration& internal,
			 const std::string& prefix);

    /// effects: add another tool to the scheduler
    /// guarantee: strong
    /// failures: out of memory I
  public:
    virtual StatusCode addTool (std::unique_ptr<IAnaTool> tool) = 0;

    /// returns: the list of all systematics the tools can be affected by
    /// guarantee: strong
    /// failures: out of memory II
  public:
    virtual CP::SystematicSet affectingSystematics () const = 0;

    /// returns: the list of all systematics the tools recommend to use
    /// guarantee: strong
    /// failures: out of memory II
  public:
    virtual CP::SystematicSet recommendedSystematics() const = 0;

    /// effects: configure this tool for the given list of systematic
    ///   variations.  any requested systematics that are not
    ///   affecting this tool will be silently ignored (unless they
    ///   cause other errors).
    /// guarantee: basic
    /// failures: tool specific
  public:
    virtual StatusCode
    applySystematicVariation (const CP::SystematicSet& systConfig) = 0;

    /// effects: fill the event objects structure with the fully
    ///   corrected objects for the current systematics
    /// guarantee: basic
    /// failures: tool specific
    /// warning: this should only be called once per event
  public:
    virtual StatusCode
    fillEventObjects (IEventObjects*& object) = 0;

    /// \brief the list of systematics configured
    /// \par Guarantee
    ///   no-fail
    /// \pre initialize() has been called
  public:
    virtual const std::vector<CP::SystematicSet>& systematics () const = 0;

    /// \brief set the value of \ref systematics
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   systematics configuration error
    /// \pre initialize() has been called
  public:
    virtual StatusCode
    setSystematics (const std::vector<CP::SystematicSet>& val_systematics) = 0;


    /// \brief make an EventData object describing the objects created
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    virtual EventData getEventData () const = 0;
  };
}

#endif
