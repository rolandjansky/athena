/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ASG_MESSAGING__MESSAGE_HELPERS_H
#define ASG_MESSAGING__MESSAGE_HELPERS_H

#ifdef XAOD_STANDALONE
#include <AsgMessaging/MsgStream.h>
#else
#include "AthenaBaseComps/AthMessaging.h"
#endif

namespace asg
{
  /// \brief a helper class for functions dealing with messages
  ///
  /// This class contains only static member functions and it is
  /// solely meant as a work-around for a limitation in ROOT
  /// dictionaries: python/root will not autoload a dictionary when a
  /// function contained in that dictionary is requested.  However, if
  /// the function is a static member function of a class it will work
  /// just fine, so instead of standalone functions these are now all
  /// static member functions.

  struct MsgHelpers final
  {
    /// \brief the message stream for the given package identifier
    ///
    /// This is for package-level streaming, which in itself is
    /// discouraged, but sometimes it is just not practical to wrap code
    /// that wants to write out a message into a tool or algorithm.
    /// Maybe if we have dual-use services some day, this may become
    /// less relevant.
    ///
    /// Normally users shouldn't access this directly, but rely on the
    /// wrappers defined above.
    static MsgStream& pkgMsgStream (const std::string& package);

    /// \brief set the package message level for the given name
    ///
    /// This is mostly to have a single, stable function that can be
    /// called from python-configuration, without having to rely on any
    /// of the details of the implementation above.
    static void setPkgMsgLevel (const std::string& package, MSG::Level level);

    /// \brief print all package message levels
    ///
    /// This is mostly to have a single, stable function that can be
    /// called from python-configuration, without having to rely on any
    /// of the details of the implementation above.
    static void printAllPkgMsgLevels ();
  };
}

#endif
