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

    /// \brief test the invariant of this object
    /// \par Guarantee
    ///   no-fail
  public:
    void testInvariant () const;


    /// \brief standard default constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   low level errors I
  public:
    Driver ();


    /// \brief the list of options to jobs with this driver
    /// \par Guarantee
    ///   no-fail
    /// \post result != 0
  public:
    SH::MetaObject *options ();
    const SH::MetaObject *options () const;


    /// \brief submit the given job with the given output location
    ///   and wait for it to finish
    ///
    /// This is mostly for small jobs and backward compatibility.  For
    /// longer jobs use \ref submitOnly instead.
    ///
    /// \par Guarantee
    ///   basic, may partially submit
    /// \par Failures
    ///   out of memory II
    /// \par Failures
    ///   can't create directory at location\n
    ///   submission errors
  public:
    void submit (const Job& job, const std::string& location) const;


    /// \brief submit the given job with the given output location
    ///   and return immediately
    ///
    /// This method allows you to submit jobs to your local batch
    /// system, log out and at a later point log back in again.
    ///
    /// \par Guarantee
    ///   basic, may partially submit
    /// \par Failures
    ///   out of memory II\n
    ///   can't create directory at location\n
    ///   submission errors
    /// \warn not all drivers support this.  some will do all their
    ///   work in the submit function.
    /// \warn you normally need to call wait() or retrieve() before
    ///   you can use the output.
  public:
    void submitOnly (const Job& job, const std::string& location) const;


    /// \brief resubmit all failed sub-jobs for the job in the given
    ///   location
    ///
    /// \parm option driver-specific option string selecting which
    ///   jobs to resubmit (and how)
    /// \par Guarantee
    ///   basic, may partially resubmit
    /// \par Failures
    ///   out of memory III\n
    ///   job resubmission errors\n
    ///   job can't be read\n
    ///   job was made with different driver
  public:
    static void resubmit (const std::string& location,
                          const std::string& option);


    /// \brief retrieve all the output for the job in the given
    ///   location
    ///
    /// While job failures will cause this method to fail you can
    /// typically retry it multiple times if you can use partial
    /// results.
    ///
    /// \return whether the job completed successfully
    /// \par Guarantee
    ///   basic, may partially retrieve
    /// \par Failures
    ///   out of memory III\n
    ///   job failures\n
    ///   job can't be read\n
    ///   job was made with different driver
  public:
    static bool retrieve (const std::string& location);


    /// \brief retrieve all the output for the job in the given
    ///   location and wait until it is finished completely.  poll the
    ///   output every time seconds.
    ///
    /// While job failures will cause this method to fail you can
    /// typically retry it multiple times if you can use partial
    /// results.
    ///
    /// Typically sleeping for 60 seconds is an appropriate interval,
    /// but if it doesn't work for you, you can change it here.
    ///
    /// \par Guarantee
    ///   basic, may partially retrieve
    /// \par Failures
    ///   out of memory III\n
    ///   job failures\n
    ///   job can't be read\n
    ///   job was made with different driver
  public:
    static bool wait (const std::string& location, unsigned time = 60);


    /// \brief update the internal location of files, after moving
    ///   the submission directory
    /// \par Guarantee
    ///   basic, may update partially
    /// \par Failures
    ///   out of memory II
    /// \warn only move the submission directory after all your
    ///   jobs are finished, or the results will be unpredictable
  public:
    static void updateLocation (const std::string& location);



    //
    // semi-public interface
    //

    /// \brief make the name of the merged output data file
    ///
    /// This is optional, but it is convenient for drivers to put
    /// these files into the same location.
    ///
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    static std::string
    mergedOutputName (const std::string& location, const OutputStream& output,
		      const std::string& sample);


    /// \brief create all the output directories for merged outputs
    ///
    /// This is optional, but it is convenient for drivers that want
    /// to keep their outputs locally.
    ///
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   out of memory II\n
    ///   i/o errors
  public:
    static void
    mergedOutputMkdir (const std::string& location, const Job& job);


    /// \brief create and save a sample handler assuming we created
    ///   all the merged files at the requested locations
    ///
    /// This is optional, but it is convenient for drivers that want
    /// to keep their outputs locally.
    ///
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   out of memory II\n
    ///   i/o errors
  public:
    static void
    mergedOutputSave (const std::string& location, const Job& job);


    /// \brief make the output sample handler for the given job or
    ///   stream from the information stored in the histogram files.
    ///
    /// This is optional, but it is convenient for drivers that use
    /// (conventional) writers
    ///
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   out of memory II\n
    ///   i/o errors
  public:
    static void
    diskOutputSave (const std::string& location, const Job& job);


    /// \brief this flag is set to true when the wait() function is
    /// running and a SIGINT is caught, meaning that control should be
    /// returned to the user as soon as possible. drivers can use it
    /// to abort long running operations in doRetrieve before
    /// completion
  protected:
    static bool abortRetrieve;


    //
    // virtual interface
    //

    /// \brief update the job before it is submitted
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   out of memory II\n
    ///   job specifications unfulfillable
  private:
    virtual void
    doUpdateJob (Job& job, const std::string& location) const;


    /// \copydoc submitOnly
    /// \par Rationale
    ///   the virtual part of \ref submitOnly
  private:
    virtual void
    doSubmit (const Job& job, const std::string& location) const;


    /// \copydoc resubmit
    /// \par Rationale
    ///   the virtual part of \ref resubmit
  private:
    virtual void
    doResubmit (const std::string& location,
                const std::string& option) const;


    /// \copydoc retrieve
    /// \par Rationale
    ///   the virtual part of \ref retrieve
  private:
    virtual bool
    doRetrieve (const std::string& location) const;



    //
    // private interface
    //

    /// \brief members directly corresponding to accessors
  private:
    SH::MetaObject m_options;

    ClassDef(Driver, 1);
  };
}

#endif
