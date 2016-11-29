/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_DRIVER_HH
#define EVENT_LOOP_DRIVER_HH

//          Copyright Nils Krumnack 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


/// This module defines a base class for classes that implement a
/// driver for running on a particular architecture.  While these
/// classes are meant to be instantiated by the user, the interface
/// provided in this class is intended for experts only.  The module
/// is considered to be in the pre-alpha stage.



#include <EventLoop/Global.h>

#include <TObject.h>
#include <SampleHandler/MetaObject.h>

namespace EL
{
  class Driver : public TObject
  {
    //
    // public interface
    //

    /// effects: test the invariant of this object
    /// guarantee: no-fail
  public:
    void testInvariant () const;


    /// effects: standard default constructor
    /// guarantee: strong
    /// failures: low level errors I
  public:
    Driver ();


    /// description: the list of options to jobs with this driver
    /// guarantee: no-fail
    /// postcondition: result != 0
  public:
    SH::MetaObject *options ();
    const SH::MetaObject *options () const;


    /// effects: submit the given job with the given output location
    ///   and wait for it to finish
    /// guarantee: basic, may partially submit
    /// failures: out of memory II
    /// failures: can't create directory at location 
    /// failures: submission errors
    /// rationale: this is mostly for small jobs and backward
    ///   compatibility.  for longer jobs use the mechanism below.
  public:
    void submit (const Job& job, const std::string& location) const;


    /// effects: submit the given job with the given output location
    ///   and return immediately
    /// guarantee: basic, may partially submit
    /// failures: out of memory II
    /// failures: can't create directory at location 
    /// failures: submission errors
    /// rationale: this method allows you to submit jobs to your local
    ///   batch system, log out and at a later point log back in again.
    /// warning: not all drivers support this.  some will do all their
    ///   work in the submit function.
    /// warning: you normally need to call wait() or retrieve() before
    ///   you can use the output.
  public:
    void submitOnly (const Job& job, const std::string& location) const;


    /// effects: retrieve all the output for the job in the given
    ///   location
    /// returns: whether the job completed successfully
    /// guarantee: basic, may partially retrieve
    /// failures: out of memory III
    /// failures: job failures
    /// failures: job can't be read
    /// failures: job was made with different driver
    /// rationale: while job failures will cause this method to fail
    ///   you can typically retry it multiple times if you can use
    ///   partial results.
  public:
    static bool retrieve (const std::string& location);


    /// effects: retrieve all the output for the job in the given
    ///   location and wait until it is finished completely.  poll the
    ///   output every time seconds.
    /// guarantee: basic, may partially retrieve
    /// failures: out of memory III
    /// failures: job failures
    /// failures: job can't be read
    /// failures: job was made with different driver
    /// rationale: while job failures will cause this method to fail
    ///   you can typically retry it multiple times if you can use
    ///   partial results.
    /// rationale: typically sleeping for 60 seconds is an appropriate
    ///   interval, but if it doesn't work for you, you can change it
    ///   here.
  public:
    static bool wait (const std::string& location, unsigned time = 60);


    /// effects: update the internal location of files, after moving
    ///   the submission directory
    /// guarantee: basic, may update partially
    /// failures: out of memory II
    /// warning: only move the submission directory after all your
    ///   jobs are finished, or the results will be unpredictable
  public:
    static void updateLocation (const std::string& location);



    //
    // semi-public interface
    //

    /// effects: write the list of output objects to disk and clear it
    /// guarantee: basic
    /// failures: i/o errors
    /// rationale: this is made static and public, because depending
    ///   on the implementation it may be called either from the
    ///   Driver or the Worker.  however, normal users would have no
    ///   interest in calling it.
  public:
    static void
    saveOutput (const std::string& location, const std::string& name,
		TList& output);


    /// effects: make the name of the merged output data file
    /// guarantee: strong
    /// failures: out of memory II
    /// rationale: this is optional, but it is convenient for drivers
    ///   to put these files into the same location.
  public:
    static std::string
    mergedOutputName (const std::string& location, const OutputStream& output,
		      const std::string& sample);


    /// effects: create all the output directories for merged outputs
    /// guarantee: basic
    /// failures: out of memory II
    /// failures: i/o errors
    /// rationale: this is optional, but it is convenient for drivers
    ///   that want to keep their outputs locally.
  public:
    static void
    mergedOutputMkdir (const std::string& location, const Job& job);


    /// effects: create and save a sample handler assuming we created
    ///   all the merged files at the requested locations
    /// guarantee: basic
    /// failures: out of memory II
    /// failures: i/o errors
    /// rationale: this is optional, but it is convenient for drivers
    ///   that want to keep their outputs locally.
  public:
    static void
    mergedOutputSave (const std::string& location, const Job& job);


    /// effects: make the output sample handler for the given job or
    ///   stream from the information stored in the histogram files.
    /// guarantee: basic
    /// failures: out of memory II
    /// failures: i/o errors
    /// rationale: this is optional, but it is convenient for drivers
    ///   that use (conventional) writers
  public:
    static void
    diskOutputSave (const std::string& location, const Job& job);


    /// description: this flag is set to true when the wait() function
    /// is running and a SIGINT is caught, meaning that control should be 
    /// returned to the user as soon as possible. drivers can use it to 
    /// abort long running operations in doRetrieve before completion 
  protected:
    static bool abortRetrieve;


    //
    // virtual interface
    //

    /// effects: update the job before it is submitted
    /// guarantee: basic
    /// failures: out of memory II
    /// failures: job specifications unfulfillable
  private:
    virtual void
    doUpdateJob (Job& job, const std::string& location) const;


    /// effects: submit the given job with the given output location
    ///   and wait for it to finish
    /// guarantee: basic, may partially submit
    /// failures: out of memory II
    /// failures: can't create directory at location 
    /// failures: submission errors
    /// rationale: the virtual part of EL::Driver::submitOnly
  private:
    virtual void
    doSubmit (const Job& job, const std::string& location) const;


    /// effects: retrieve all the output for the job in the given
    ///   location
    /// returns: whether the job completed successfully
    /// guarantee: basic, may partially retrieve
    /// failures: out of memory III
    /// failures: job failures
    /// failures: job can't be read
    /// failures: job was made with different driver
    /// rationale: while job failures will cause this method to fail
    ///   you can typically retry it multiple times if you can use
    ///   partial results.
    /// rationale: the virtual part of EL::Driver::retrieve
  private:
    virtual bool
    doRetrieve (const std::string& location) const;



    //
    // private interface
    //

    /// description: members directly corresponding to accessors
  private:
    SH::MetaObject m_options;

    ClassDef(Driver, 1);
  };
}

#endif
