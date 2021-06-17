/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SYSTEMATICS_HANDLES__I_SYSTEMATICS_SVC_H
#define SYSTEMATICS_HANDLES__I_SYSTEMATICS_SVC_H

#include <AsgServices/IAsgService.h>

#include <SystematicsHandles/SysListType.h>

namespace CP
{
  /// \brief the interface for the central systematics service
  ///
  /// This service is used as a core part of the common CP algorithms
  /// to allow communicating systematics information between
  /// algorithms in a sequence, without those algorithms needing to
  /// talk to each other directly.

  class ISystematicsSvc : virtual public asg::IAsgService
  {
  public:

    DeclareInterfaceID (CP::ISystematicsSvc, 1, 0);

    /// \brief get the list of systematics
    virtual const SysListType& systematicsVector () const = 0;
  };
}

#endif
