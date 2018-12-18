#ifndef ASG_TOOLS__I_MESSAGE_PRINTER_H
#define ASG_TOOLS__I_MESSAGE_PRINTER_H

//        Copyright Iowa State University 2017.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


#include <AsgTools/MsgLevel.h>
#include <sstream>

#ifdef XAOD_STANDALONE

namespace asg
{
  /// \brief an interface for printing messages to the screen
  ///
  /// This gets called internally by \ref MsgStream to do the actual
  /// printing to the screen.  The primary purpose is to allow
  /// replacing this with a mock object to check that we do (or do
  /// not) get specific messages in specific situations.  In
  /// particular it is meant to allow checking the error condition in
  /// case of failure, as well as unit testing the messaging mechanism
  /// itself.

  class IMessagePrinter
  {
    /// \brief standard virtual destructor
    /// \par Guarantee
    ///   no-fail
  public:
    virtual ~IMessagePrinter () noexcept = default;


    /// \brief print the given message
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   i/o errors
  public:
    virtual void
    print (MSG::Level reqlvl, const std::string& name,
           const std::string& text) = 0;
  };
}

#endif

#endif
