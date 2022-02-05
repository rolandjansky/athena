/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack
/// @author David Adams <dladams@bnl.gov> (for original implementation for tools)

#ifndef ASGSERVICES_IASGSERVICE_H
#define ASGSERVICES_IASGSERVICE_H

// System include(s):
#include <string>

// Local include(s):
#include "AsgMessaging/INamedInterface.h"

#ifdef XAOD_STANDALONE
/// \brief standalone version of the Gaudi interface declaration
///
/// This can't be a no-op, because the Gaudi version needs to be
/// followed by a semicolon, so we need a statement that requires to
/// be followed by a semi-colon.
#define DeclareInterfaceID(iface, major, minor) \
  static constexpr std::nullptr_t interfaceID = nullptr
#endif

namespace asg
{
  /// Base class for the dual-use service interface classes
  ///
  /// Loosely based on IAsgTool class

  class IAsgService
    : virtual public INamedInterface
  {
  public:

    /// Print the state of the service
    virtual void print() const = 0;

  }; // class IAsgService

} // namespace asg

#endif // ASGSERVICES_IASGSERVICE_H
