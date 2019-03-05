/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef QUICK_ANA__EVENT_TOOL_H
#define QUICK_ANA__EVENT_TOOL_H

// This module still needs to be documented.  The interface provided
// in this module is intended for experts only.  The module is
// considered to be in the pre-alpha stage.



#include <QuickAna/Global.h>

#include <AsgTools/AsgTool.h>
#include <QuickAna/Configuration.h>
#include <QuickAna/IQuickAna.h>

namespace ana
{
  /// TODO: needs documentation
  class QuickAna : virtual public IQuickAna,
                   public asg::AsgTool,
		   public Configuration
  {
    //
    // public interface
    //

    ASG_TOOL_CLASS2 (QuickAna, ana::IQuickAna, CP::ISystematicsTool)


    /// effects: test the invariant of this object
    /// guarantee: no-fail
  public:
    void testInvariant () const;


    /// effects: standard constructor
    /// guarantee: strong
    /// failures: out of memory II
  public:
    QuickAna (const std::string& name);


    /// effects: standard destructor
    /// guarantee: no-fail
  public:
    ~QuickAna ();


    /// rationale: no slicing
#ifndef __CINT__
    QuickAna (const QuickAna& that) = delete;
    QuickAna& operator = (const QuickAna& that) = delete;
#endif


    /// effects: initialize this tool
    /// guarantee: basic
    /// failures: configuration errors
  public:
    virtual StatusCode initialize()
#ifndef __CINT__
      override
#endif
      ;


    /// returns: whether this tool is affected by the given systematis
    /// guarantee: strong
    /// failures: out of memory II
  public:
    virtual bool
    isAffectedBySystematic (const CP::SystematicVariation& systematic) const
#ifndef __CINT__
      override
#endif
      ;

    /// returns: the list of all systematics this tool can be affected by
    /// guarantee: strong
    /// failures: out of memory II
  public:
    virtual CP::SystematicSet
    affectingSystematics () const
#ifndef __CINT__
      override
#endif
      ;


    /// returns: the list of all systematics this tool recommends to use
    /// guarantee: strong
    /// failures: out of memory II
  public:
    virtual CP::SystematicSet
    recommendedSystematics () const
#ifndef __CINT__
      override
#endif
      ;


    /// effects: configure this tool for the given list of systematic
    ///   variations.  any requested systematics that are not
    ///   affecting this tool will be silently ignored (unless they
    ///   cause other errors).
    /// guarantee: basic
    /// failures: systematic unknown
    /// failures: requesting multiple variations on the same
    ///   systematic (e.g. up & down)
    /// failures: requesting an unsupported variation on an otherwise
    ///   supported systematic (e.g. a 2 sigma variation and the tool
    ///   only supports 1 sigma variations)
    /// failures: unsupported combination of supported systematic
    /// failures: other tool specific errors
  public:
    virtual CP::SystematicCode
    applySystematicVariation (const CP::SystematicSet& systConfig)
#ifndef __CINT__
      override
#endif
      ;

    /// description: the tool scheduler we use
  public:
    std::unique_ptr<IToolScheduler> toolScheduler;



    //
    // inherited interface
    //

    /// effects: process the given event
    /// guarantee: basic
  public:
    virtual StatusCode process ()
#ifndef __CINT__
      override
#endif
      ;
    using IQuickAna::process;

    /// effects: calculate the weight for this event
    /// returns: the calculated weight
    /// guarantee: basic
    /// failures: tool dependent
  public:
    virtual float weight ()
#ifndef __CINT__
      override
#endif
      ;


    /// description: the weight for the event (without object weights)
    /// guarantee: no-fail
  public:
    virtual float eventWeight ()
#ifndef __CINT__
      override
#endif
      ;


    /// description the overall object structure
    /// guarantee: no-fail
  public:
    virtual const IEventObjects& objects () const
#ifndef __CINT__
      override
#endif
      ;


    /// \copydoc IQuickAna::internalConfiguration
  public:
    virtual const InternalConfiguration& internalConfiguration ()
#ifndef __CINT__
      override
#endif
      ;


  virtual xAOD::EventInfo* eventinfo () const
#ifndef __CINT__
      override
#endif
      ;
    virtual xAOD::MuonContainer* muons () const
#ifndef __CINT__
      override
#endif
      ;
    virtual xAOD::ElectronContainer* electrons () const
#ifndef __CINT__
      override
#endif
      ;
    virtual xAOD::PhotonContainer* photons () const
#ifndef __CINT__
      override
#endif
      ;
    virtual xAOD::TauJetContainer* taus () const
#ifndef __CINT__
      override
#endif
      ;
    virtual xAOD::JetContainer* jets () const
#ifndef __CINT__
      override
#endif
      ;
     virtual xAOD::JetContainer* pflow_jets () const
#ifndef __CINT__
      override
#endif
      ;
    virtual xAOD::JetContainer* fat_jets () const
#ifndef __CINT__
        override
#endif
      ;
    virtual xAOD::MissingET* met () const
#ifndef __CINT__
      override
#endif
      ;
    virtual xAOD::MissingET* met2 () const
#ifndef __CINT__
      override
#endif
      ;


    /// \brief the list of systematics configured
    /// \par Guarantee
    ///   no-fail
  public:
    virtual const std::vector<CP::SystematicSet>& systematics () const
#ifndef __CINT__
      override
#endif
      ;


    /// \brief set the value of \ref systematics
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   systematics configuration error
  public:
    virtual StatusCode
    setSystematics (const std::vector<CP::SystematicSet>& val_systematics)
#ifndef __CINT__
      override
#endif
      ;


  public:
    virtual EventData getEventData () const override;



    //
    // private interface
    //

    /// description: the pimpl
  private:
    struct Pimpl;
    Pimpl *pimpl;
  };
}

#endif
