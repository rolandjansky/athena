/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_BACKGROUND_JOB_HH
#define EVENT_LOOP_BACKGROUND_JOB_HH

//          Copyright Nils Krumnack 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


/// This module defines the arguments passed from the BACKGROUND driver to
/// the BACKGROUND worker.  The interface provided in this class is
/// intended for experts only.  The module is considered to be in the
/// pre-alpha stage.



#include <EventLoop/Global.h>

#include <SampleHandler/MetaObject.h>

namespace EL
{
  /// \brief the job description as communicated to the background
  /// process
  ///
  /// This is streamed to the disk in the main thread and then read
  /// back in another thread.  Ideally we'd just stream it into the
  /// message, but the root documentation is a little sparse on the
  /// details for that.
  ///
  /// There is some memory leak in the background thread, but that
  /// ought to be fine since this is only read once and doing
  /// otherwise would be tedious.

  struct BackgroundJob : public TObject
  {
    /// \brief the list of algorithms we are using
  public:
    std::vector<Algorithm*> algs;

    /// \brief the list of output streams we are using
  public:
    std::vector<OutputStream> output;

    /// \brief the sample meta-information
  public:
    SH::MetaObject meta;

    ClassDef(BackgroundJob, 1);
  };
}

#endif
