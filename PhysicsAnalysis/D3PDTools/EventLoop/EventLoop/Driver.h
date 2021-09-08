/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EVENT_LOOP_DRIVER_HH
#define EVENT_LOOP_DRIVER_HH

#include <EventLoop/Global.h>

#include <TObject.h>
#include <SampleHandler/MetaObject.h>

class StatusCode;

namespace EL
{
  /// \brief the base class for the various EventLoop drivers that
  /// allow to run jobs on different backends
  ///
  /// The interface here is intended for users to interact with
  /// directly, but it is considered an expert level task to define
  /// new implementations of this class.

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
    /// \return The actual location of the submit directory, if the
    /// job was configured to generate a unique directory.
    /// \par Guarantee
    ///   basic, may partially submit
    /// \par Failures
    ///   out of memory II
    /// \par Failures
    ///   can't create directory at location\n
    ///   submission errors
  public:
    std::string submit (const Job& job, const std::string& location) const;


    /// \brief submit the given job with the given output location
    ///   and return immediately
    ///
    /// This method allows you to submit jobs to your local batch
    /// system, log out and at a later point log back in again.
    ///
    /// \return The actual location of the submit directory, if the
    /// job was configured to generate a unique directory.
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
    std::string submitOnly (const Job& job, const std::string& location) const;


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
    mergedOutputSave (Detail::ManagerData& data);


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
    diskOutputSave (Detail::ManagerData& data);


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

  protected:
    virtual ::StatusCode
    doManagerStep (Detail::ManagerData& data) const;



    //
    // private interface
    //

    friend class Detail::DriverManager;

    /// \brief members directly corresponding to accessors
  private:
    SH::MetaObject m_options;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Winconsistent-missing-override"
    ClassDef(Driver, 1);
#pragma GCC diagnostic pop
  };
}

#endif
