#ifndef ROOT_CORE_UTILS__MESSAGE_H
#define ROOT_CORE_UTILS__MESSAGE_H

//          Copyright Nils Krumnack 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


// This module still needs to be documented.  The interface provided
// in this module is intended for experts only.  The module is
// considered to be in the pre-alpha stage.



#include <RootCoreUtils/Global.h>

#include <RootCoreUtils/MessageType.h>

namespace RCU
{
  struct Message
  {
    //
    // public interface
    //

    /// description: the location where the message was send
  public:
    const char *package;
  public:
    const char *file;
  public:
    unsigned line;

    /// description: the type of this message
  public:
    MessageType type;

    /// description: the actual message we are sending or 0 if there
    ///   isn't one
  public:
    const char *message;


    /// effects: standard constructor
    /// guarantee: no-fail
  public:
    Message ();


    /// effects: print the given message
    /// guarantee: basic
    /// failures: i/o errors
  public:
    void send () const;
  };
}

#endif
