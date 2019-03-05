/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef QUICK_ANA__OPTIMIZER_SCHEDULER_H
#define QUICK_ANA__OPTIMIZER_SCHEDULER_H

// This module still needs to be documented.  The interface provided
// in this module is intended for experts only.  The module is
// considered to be in the pre-alpha stage.



#include <QuickAna/Global.h>

#include <AsgTools/AsgTool.h>
#include <QuickAna/IToolScheduler.h>

namespace ana
{

  /// The optimized tool scheduler.
  ///
  /// Holds and manages the OptimizedStore and the OptimizedTools.
  ///
  class OptimizedScheduler : virtual public IToolScheduler, virtual public asg::AsgTool
  {
    //
    // public interface
    //

    ASG_TOOL_CLASS (OptimizedScheduler, ana::IToolScheduler)


    /// effects: test the invariant of this object
    /// guarantee: no-fail
  public:
    void testInvariant () const;


    /// effects: standard constructor
    /// guarantee: strong
    /// failures: out of memory II
  public:
    OptimizedScheduler (const std::string& name);


    /// \brief standard destructor
    ///
    /// I defined my own destructor to break include file dependencies
    /// in Athena
    /// \par Guarantee
    ///   no-fail
  public:
    ~OptimizedScheduler ();



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

  public:
    virtual const std::vector<CP::SystematicSet>&
    systematics () const override;

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
    std::vector<std::unique_ptr<OptimizedTool>> m_tools;

    /// description: the master object we are using
  private:
    std::unique_ptr<OptimizedStore> m_master;

    /// description: the list of nuisance parameter points we are
    ///   evaluating
  private:
    std::vector<CP::SystematicSet> m_targetSystematics;

    /// description: the map of event objects
  private:
    std::map<CP::SystematicSet,std::unique_ptr<OptimizedObjects>> m_objectsMap;

    /// description: the current event objects
  private:
    OptimizedObjects *m_currentObjects;


    /// \brief the current run number
    ///
    /// this is used together with \ref m_eventNumber to determine
    /// whether we started on a new event
  private:
    uint32_t m_runNumber;


    /// \brief the current event number
    ///
    /// this is used together with \ref m_runNumber to determine
    /// whether we started on a new event
  private:
    uint32_t m_eventNumber;


    /// \brief whether we are on a new event
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   xAOD failures
  private:
    bool isNewEvent ();
  };
}

#endif
