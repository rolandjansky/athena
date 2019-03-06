/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef QUICK_ANA__I_QUICK_ANA_H
#define QUICK_ANA__I_QUICK_ANA_H

// This module still needs to be documented.  The interface provided
// in this module is intended for experts only.  The module is
// considered to be in the pre-alpha stage.



#include <QuickAna/Global.h>

#include <PATInterfaces/ISystematicsTool.h>
#include <QuickAna/xAODInclude.h>

namespace ana
{
  class IQuickAna : public CP::ISystematicsTool
  {
    //
    // public interface
    //

    // ASG_TOOL_INTERFACE(IQuickAna)

    /// effects: process the current event
    /// guarantee: basic
  public:
    virtual StatusCode process () = 0;


    /// effects: process the given event
    /// guarantee: basic
  public:
#ifdef ROOTCORE
    StatusCode process (xAOD::TEvent& event);
#endif


    /// effects: calculate the weight for this event
    /// returns: the calculated weight
    /// guarantee: basic
    /// failures: tool dependent
  public:
    virtual float weight () = 0;


    /// description: the weight for the event (without object weights)
    /// guarantee: no-fail
  public:
    virtual float eventWeight () = 0;


    /// description the overall object structure
    /// guarantee: no-fail
  public:
    virtual const IEventObjects& objects () const = 0;


    /// \brief get the internal configuration structure
  public:
    virtual const InternalConfiguration& internalConfiguration () = 0;


    /// description: the various object collections we are using, or
    ///   NULL if they are disabled
    /// guarantee: no-fail
  public:
    //virtual xAOD::EventInfoContainer* eventinfo () const = 0;
    virtual xAOD::EventInfo* eventinfo () const = 0;
    virtual xAOD::MuonContainer* muons () const = 0;
    virtual xAOD::ElectronContainer* electrons () const = 0;
    virtual xAOD::PhotonContainer* photons () const = 0;
    virtual xAOD::TauJetContainer* taus () const = 0;
    virtual xAOD::JetContainer* jets () const = 0;
    virtual xAOD::JetContainer* pflow_jets () const = 0;
    virtual xAOD::JetContainer* fat_jets () const = 0;
    virtual xAOD::MissingET* met () const = 0;
    virtual xAOD::MissingET* met2 () const = 0;


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

#include <QuickAna/IQuickAna.icc>

#endif
