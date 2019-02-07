/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EVENT_LOOP__UNIT_TEST_CONFIG_H
#define EVENT_LOOP__UNIT_TEST_CONFIG_H

#include <EventLoopTest/Global.h>

#include <EventLoop/Global.h>
#include <SampleHandler/Global.h>
#include <memory>

namespace EL
{
  /// \brief a class describing how to run the jobs for driver unit
  /// tests
  ///
  /// This is so that I can write driver unit tests as briefly as
  /// possible.  The unit tests should derive a class from this class
  /// that sets the proper members in the constructor and then pass
  /// the object into the parametric unit test.

  struct UnitTestConfig
  {
    /// \brief the driver being tested
    ///
    /// This is the only member the derived classes actually have to
    /// set.  More members will be added over time, but those will
    /// meaningful default values
  public:
    std::shared_ptr<Driver> m_driver;


    /// \brief virtual destructor for virtual base class
    /// \par Guarantee
    ///   no-fail
  public:
    virtual ~UnitTestConfig () noexcept = default;


    /// \brief create a file write for the given file
    ///
    /// This allows to place the test files into non-standard locations
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   i/o errors
  public:
    virtual std::unique_ptr<SH::DiskWriter>
    make_file_writer (const std::string& name) const;


    /// \brief apply extra job configuration parameters
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   config specific
  public:
    virtual void setupJob (Job& job) const;
  };
}

#endif
