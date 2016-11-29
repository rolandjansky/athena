/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_D3PDREADER_SVC_H
#define EVENT_LOOP_D3PDREADER_SVC_H

//          Copyright Nils Krumnack 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


/// This module defines the services interfacing with the D3PDReader.
/// The interface provided in this module is intended for the general
/// user.  The module is considered to be in the pre-alpha stage.



#include <EventLoop/Global.h>

#include <RootCore/Packages.h>

#ifdef ROOTCORE_PACKAGE_D3PDReader

#include <EventLoop/Algorithm.h>

namespace D3PDReader
{
  class D3PDReadStats;
  class Event;
}

namespace EL
{
  class D3PDReaderSvc : public Algorithm
  {
    //
    // public interface
    //

    /// description: the name of the service
  public:
    static const std::string name;


    /// effects: test the invariant of this object
    /// guarantee: no-fail
  public:
    void testInvariant () const;


    /// effects: standard constructor.
    /// guarantee: no-fail
  public:
    D3PDReaderSvc ();


    /// effects: standard destructor.
    /// guarantee: no-fail
  public:
    ~D3PDReaderSvc ();


    /// description: the event we are using
    /// guarantee: strong
    /// failures: service not yet configured
    /// postcondition: event != 0
  public:
    D3PDReader::Event *event () const;



    //
    // interface inherited from Algorithm
    //

    /// effects: return the name of this algorithm
    /// guarantee: no-fail
  public:
    virtual const char *GetName () const override;


    /// effects: do all changes to work with a new input file,
    ///   e.g. set new branch addresses.  if firstFile is set, this
    ///   method is called just before init() is called
    /// guarantee: basic
    /// failures: algorithm dependent
  private:
    virtual StatusCode changeInput (bool firstFile) override;


    /// effects: process the next event
    /// guarantee: basic
    /// failures: algorithm dependent
    /// rationale: the virtual part of Algorithm::process
  private:
    virtual StatusCode execute () override;


    /// effects: do everything that needs to be done after completing
    ///   work on this worker
    /// guarantee: basic
    /// failures: algorithm dependent
    /// rationale: currently there is no use foreseen, but this
    ///   routine is provided regardless
  private:
    virtual StatusCode finalize () override;



    //
    // private interface
    //

    /// description: the event structure used
  private:
    D3PDReader::Event *m_event; //!

    /// description: whether we collect D3PDPerfStats statistics
  private:
    bool m_useStats; //!

    /// description: the stats we use for caching and the mode and
    ///   argument for caching
  private:
    const D3PDReader::D3PDReadStats *m_cacheStats; //!
  private:
    unsigned m_cacheMode; //!
  private:
    double m_cacheArg; //!
    

    ClassDef(D3PDReaderSvc, 1);
  };
}

#endif

#endif
