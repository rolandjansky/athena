/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_BACKGROUND_DRIVER_HH
#define EVENT_LOOP_BACKGROUND_DRIVER_HH

//          Copyright Nils Krumnack 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.



#include <EventLoop/Global.h>

#include <EventLoop/Driver.h>

namespace EL
{
  /// \brief a test driver for the background worker
  ///
  /// This is not meant for users, but for unit tests of the
  /// background process mechanism to run without the complication of
  /// any farm processes.

  class BackgroundDriver : public Driver
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
    ///   out of memory I
  public:
    BackgroundDriver ();



    //
    // interface inherited from Driver
    //

    /// \copydoc Driver::doUpdateJob
    /// \sa      Driver::doUpdateJob
  private:
    virtual void
    doUpdateJob (Job& job, const std::string& location) const override;

    /// \copydoc Driver::doUpdateJob
    /// \sa      Driver::doUpdateJob
  private:
    virtual void
    doSubmit (const Job& job, const std::string& location) const override;



    //
    // private interface
    //

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Winconsistent-missing-override"
    ClassDef(BackgroundDriver, 1);
#pragma GCC diagnostic pop
  };
}

#endif
