/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef EVENT_LOOP_DIRECT_DRIVER_HH
#define EVENT_LOOP_DIRECT_DRIVER_HH

#include <EventLoop/Global.h>

#include <EventLoop/Driver.h>

namespace EL
{
  /// \brief a \ref Driver that runs directly inside the submission
  /// job itself
  ///
  /// This is both to allow for small and quick jobs that don't need
  /// distributed running, and to allow for testing/debugging for
  /// which you may need to run in the debugger.

  class DirectDriver final : public Driver
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
    DirectDriver ();



    //
    // interface inherited from Driver
    //

  protected:
    virtual ::StatusCode
    doManagerStep (Detail::ManagerData& data) const override;



    //
    // private interface
    //

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Winconsistent-missing-override"
    ClassDef(DirectDriver, 1);
#pragma GCC diagnostic pop
  };
}

#endif
