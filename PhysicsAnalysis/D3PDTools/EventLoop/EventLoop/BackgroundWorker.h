/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_BACKGROUND_WORKER_HH
#define EVENT_LOOP_BACKGROUND_WORKER_HH

//          Copyright Nils Krumnack 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.



#include <EventLoop/Global.h>

#include <EventLoop/Worker.h>

namespace EL
{
  /// \brief a worker that runs in the background communicating via
  /// IPC to the rest.

  class BackgroundWorker : public Worker
  {
    //
    // public interface
    //

    /// \brief execute the job
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   job failures
  public:
    static void execute (int socket);


    /// \brief test the invariant of this object
    /// \par Guarantee
    ///   no-fail
  public:
    void testInvariant () const;


    /// \brief standard constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   put of memory I
  public:
    BackgroundWorker (const BackgroundJob& job, TList *output);



    //
    // interface inherited from Worker
    //



    //
    // private interface
    //

  //   /// description: the sample we are working on
  // private:
  //   SH::SamplePtr m_sample;

  //   /// description: the location we store data at
  // private:
  //   std::string m_location;
  };
}

#endif
