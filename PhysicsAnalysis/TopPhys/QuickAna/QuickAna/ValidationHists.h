/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef QUICK_ANA__VALIDATION_HISTS_H
#define QUICK_ANA__VALIDATION_HISTS_H

// This module still needs to be documented.  The interface provided
// in this module is intended for experts only.  The module is
// considered to be in the pre-alpha stage.



#include <QuickAna/Global.h>

#include <QuickAna/xAODInclude.h>
#include <string>

#ifdef ROOTCORE
#include <EventLoop/Global.h>
#endif

class TH1;

namespace ana
{
  class ValidationHists
  {
    //
    // public interface
    //

    /// effects: test the invariant of this object
    /// guarantee: no-fail
  public:
    void testInvariant () const;


    /// effects: standard constructor
    /// guarantee: strong
    /// failures: out of memory I
  public:
    ValidationHists (EL::IWorker *val_worker,
		     const InternalConfiguration& configuration);


    /// effects: fill the histograms for the given objects
    /// guarantee: basic
    /// failures: histogram creation errors
  public:
    void fillHists (const std::string& prefix, const IEventObjects& objects,
		    float weight);


    /// effects: fill the histograms for the given object type
    /// guarantee: basic
    /// failures: histogram creation errors
  public:
    void fillHists (const std::string& prefix,
		    const xAOD::IParticleContainer& particles,
		    float weight);
    void fillHists (const std::string& prefix,
		    const xAOD::MissingET& met,
		    float weight);


    /// effects: fill a single histogram with the given binning and value
    /// guarantee: basic
    /// failures: histogram creation errors
  public:
    void fillHist (const std::string& name, unsigned nbins, float low,
		   float high, float value, float weight);



    //
    // private interface
    //

    /// description: the worker we are using
  private:
    EL::IWorker *m_worker;

    /// description: the actual histograms we are using
  private:
    std::unordered_map<std::string,TH1*> m_hists;

    /// \brief the accessor to the selection
  private:
    SG::AuxElement::Accessor<SelectType> m_selectionAccessor;
  };
}

#endif
