/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ASGTOOLS_IASGCOMPONENT_H
#define ASGTOOLS_IASGCOMPONENT_H

#include "AsgMessaging/INamedInterface.h"

namespace asg
{
  /// \brief base class for the dual-use component interface classes
  ///
  /// So far this interface doesn't really do a lot, but it is here as
  /// a placeholder for potential future use.

  class IAsgComponent : virtual public INamedInterface
  {
  };
}

#endif
