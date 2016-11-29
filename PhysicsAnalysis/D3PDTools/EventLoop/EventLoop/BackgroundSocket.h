/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP__BACKGROUND_SOCKET_H
#define EVENT_LOOP__BACKGROUND_SOCKET_H

//        Copyright Iowa State University 2015.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


#include <EventLoop/Global.h>

#include <memory>
#include <string>

namespace EL
{
  /// \brief This class wraps around the socket used for communication
  /// to the background worker and implements a simple messaging
  /// mechanism.
  ///
  /// Messages here just consist of a type and a string as a payload.
  /// We do not worry about bringing thinks into a portable format, as
  /// this is only meant for communicating between machines employing
  /// the same architecture.

  class BackgroundSocket
  {
    //
    // public interface
    //

    /// \brief the type of message we send around
  public:
    enum Type
    {
      /// \brief start the job
      ///
      /// the payload indicates a file containing the information
      /// about the job.
      T_START_JOB,

      /// \brief set the next input file
      ///
      /// the payload is the name of the file to open
      T_SET_INPUT_FILE,

      /// \brief set the tree entry
      ///
      /// the payload is a Long64_t wrapped into a string that
      /// contains the tree entry
      T_SET_ENTRY,

      /// \brief execute the event
      ///
      /// there is no payload for this
      T_EXECUTE,

      /// \brief confirm a \ref T_EXECUTE
      T_EXECUTE_CONFIRM,

      /// \brief end the job
      ///
      /// the payload indicates a file to which to save the histogram
      /// information.
      T_END_JOB,

      /// \brief confirm a \ref T_END_JOB
      T_END_JOB_CONFIRM,

      /// \brief an invalid message
      ///
      /// this is guaranteed to be the last entry in the enum, so that
      /// it can be used to check that the enum is read correctly
      T_INVALID
    };



    /// \brief test the invariant of this object
    /// \par Guarantee
    ///   no-fail
  public:
    void testInvariant () const;


    /// \brief standard constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory I
  public:
    BackgroundSocket (int val_socket);


    /// \brief standard destructor
    /// \par Guarantee
    ///   no-fail
  public:
    ~BackgroundSocket ();


    /// \brief write a message to the socket
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   i/o errors
  public:
    void write (Type type, const std::string& payload);


    /// \brief read a message from the socket
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   i/o errors
  public:
    void read (Type& type, std::string& payload);


    /// \brief make a pair of sockets suitable for communication
    ///
    /// I rather do this myself instead of leaving it to the user, as
    /// this allows me to make sure the settings are correct.
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   i/o errors
  public:
    static void makeSockets (int sockets [2]);

    /// \brief start a background worker connected by socket
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   low level failures
  public:
    static std::auto_ptr<BackgroundSocket>
    startWorker (const BackgroundJob& job);



    //
    // private interface
    //

    /// \brief the socket we are communicating with
  private:
    int m_socket;

    /// \brief the input buffer we are using
  private:
    std::string m_inputBuffer;

    /// \brief the output buffer we are using
  private:
    std::string m_outputBuffer;
  };
}

#endif
