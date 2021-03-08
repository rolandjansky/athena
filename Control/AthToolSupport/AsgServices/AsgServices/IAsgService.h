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
#include "AsgServices/AsgServiceMacros.h"
#include "AsgMessaging/INamedInterface.h"

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
