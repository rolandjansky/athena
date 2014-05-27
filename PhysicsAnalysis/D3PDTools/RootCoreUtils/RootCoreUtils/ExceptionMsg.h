#ifndef ROOT_CORE_UTILS__EXCEPTION_MSG_H
#define ROOT_CORE_UTILS__EXCEPTION_MSG_H

//          Copyright Nils Krumnack 2008 - 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


// This module defines an exception that contains nothing more than an
// error message.  The interface provided in this module is intended
// for the general user.  The module is considered to be in the
// pre-alpha stage.



//protect
#include <RootCoreUtils/Global.h>

#include <exception>
#include <string>

namespace RCU
{
  class ExceptionMsg : public std::exception
  {
    //
    // public interface
    //

    // effects: test the invariant of this object
    // guarantee: no-fail
  public:
    void testInvariant () const;


    // effects: create an exception with the given message
    // guarantee: strong
    // failures: out of memory II
    // requires: val_file != 0
    // requires: val_line != 0
    // requires: !val_message.empty()
  public:
    ExceptionMsg (const char *const val_file, const unsigned val_line,
		  const std::string& val_message);


    // effects: destroy this object
    // guarantee: no-fail
  public:
    virtual ~ExceptionMsg () throw ();



    //
    // interface inherited from std::exception
    //

    // returns: the message associated with this exception
    // guarantee: no-fail
  public:
    virtual const char *what () const throw ();



    //
    // private interface
    //

    /// description: the location where the message occured
  private:
    const char *m_file;
  private:
    unsigned m_line;

    /// description: the actual message
  private:
    std::string m_message;
  };
}

#endif
