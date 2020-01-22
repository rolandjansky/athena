/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef EVENT_LOOP_BATCH_DRIVER_HH
#define EVENT_LOOP_BATCH_DRIVER_HH

#include <EventLoop/Global.h>

#include <EventLoop/Driver.h>
#include <SampleHandler/Global.h>

namespace EL
{
  /// \brief the base class for all drivers running on batch systems
  ///
  /// This class implements extra functionality for packaging
  /// EventLoop jobs into sub-jobs and then retrieving the results.

  class BatchDriver : public Driver
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
    BatchDriver ();


    /// description: these shell commands are run verbatim on each
    ///   worker node before execution
  public:
    std::string shellInit;



    //
    // interface inherited from Driver
    //

  protected:
    virtual ::StatusCode
    doManagerStep (Detail::ManagerData& data) const override;



    //
    // virtual interface
    //



    //
    // private interface
    //

    /// \brief the code for setting up the release
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II\n
    ///   failed to read environment variables
  private:
    std::string defaultReleaseSetup (const Detail::ManagerData& data) const;

    /// effects: create the run script to be used
    /// guarantee: basic, may create a partial script
    /// failures: out of memory II
    /// failures: i/o errors
  private:
    void makeScript (Detail::ManagerData& data,
                     std::size_t njobs) const;


    /// effects: merge the fetched histograms
    /// returns: wether all histograms have been fetched
    /// guarantee: strong
    /// failures: out of memory II
    /// failures: i/o errors
  private:
    static bool mergeHists (Detail::ManagerData& data);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Winconsistent-missing-override"
    ClassDef(BatchDriver, 1);
#pragma GCC diagnostic pop
  };
}

#endif
