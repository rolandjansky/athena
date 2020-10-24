/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef ASG_MESSAGING_ASG_MESSAGING_FORWARD_H
#define ASG_MESSAGING_ASG_MESSAGING_FORWARD_H

// Local include(s):
#include "AsgMessaging/MsgStream.h"
#include "AsgMessaging/MsgStreamMacros.h"

#include <functional>

namespace asg
{
  /// \brief base class to forward messages to another class
  ///
  /// The idea is that this class should closely mirror the behavior
  /// of \ref AsgMessaging but instead of representing a named
  /// component in its own right, it forwards all the messages to
  /// another component.  This is used for things like the systematics
  /// data handles that report all their messages as part of the
  /// owning algorithm.

  class AsgMessagingForward
  {
    //
    // public interface
    //

    /// \brief forwarding constructor
    ///
    /// This is templated to allow arbitrary parent classes to be
    /// passed in here using the `this` pointer.
  public:
    template<typename T> explicit AsgMessagingForward( T *owner );


    /// Test the output level of the object
    ///
    /// \param lvl The message level to test against
    /// \return boolean Indicting if messages at given level will be printed
    /// \returns <code>true</code> If messages at level "lvl" will be printed
    ///
    bool msgLvl( const MSG::Level lvl ) const;

    /// The standard message stream.
    ///
    /// \returns A reference to the default message stream of this object.
    ///
    MsgStream& msg() const;

    /// The standard message stream.
    ///
    /// \param lvl The message level to set the stream to
    /// \returns A reference to the default message stream, set to level "lvl"
    ///
    MsgStream& msg( const MSG::Level lvl ) const;



    //
    // private interface
    //

    /// \brief the message stream we use
    ///
    /// This used to be a simple pointer to the `MsgStream` itself,
    /// but in AthenaMT the actual object used is local to the thread.
    /// So instead of pointing to it directly we are now using a
    /// function to look it up, which will get the thread-local
    /// object.
  private:
    std::function<MsgStream& ()> m_msg;
  };



  template<typename T>  AsgMessagingForward ::
  AsgMessagingForward( T *owner )
    : m_msg ([owner] () -> MsgStream& {return owner->msg();})
  {}
}

#endif
