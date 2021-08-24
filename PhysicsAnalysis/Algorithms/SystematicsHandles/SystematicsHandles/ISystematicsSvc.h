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
    virtual std::vector<CP::SystematicSet>
    makeSystematicsVector () const = 0;

    /// \brief register a set of recommended and affecting systematics
    ///
    /// This is usually obtained from an \ref CP::ISystematicsTool and
    /// is used for calculating the list of systematics to use on the
    /// fly.
    virtual StatusCode
    addSystematics (const CP::SystematicSet& recommended,
                    const CP::SystematicSet& affecting) const = 0;

    /// \brief get the systematics for the given object in the event store
    virtual CP::SystematicSet
    getObjectSystematics (const std::string& name) const = 0;

    /// \brief set the systematics for the given object in the event store
    virtual StatusCode
    setObjectSystematics (const std::string& name,
                          const CP::SystematicSet& systematics) const = 0;

    /// \brief get the systematics for the given object in the event store
    virtual CP::SystematicSet
    getDecorSystematics (const std::string& objectName,
                         const std::string& decorName) const = 0;

    /// \brief set the systematics for the given object in the event store
    virtual StatusCode
    setDecorSystematics (const std::string& objectName,
                         const std::string& decorName,
                         const CP::SystematicSet& systematics) const = 0;

    /// \brief register a (shallow) copy from one object to the next
    virtual StatusCode
    registerCopy (const std::string& fromName,
                  const std::string& toName) const = 0;

    /// \brief make the name for the given systematics
    virtual StatusCode
    makeSystematicsName (std::string& result,
                         const std::string& name,
                         const CP::SystematicSet& sys) const = 0;
  };
}

#endif
