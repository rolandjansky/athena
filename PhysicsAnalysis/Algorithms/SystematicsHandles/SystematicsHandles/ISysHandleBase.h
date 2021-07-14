/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SYSTEMATICS_HANDLES__I_SYS_HANDLE_BASE_H
#define SYSTEMATICS_HANDLES__I_SYS_HANDLE_BASE_H

#include <string>
#include <vector>

class StatusCode;

namespace CP
{
  class ISystematicsSvc;
  class SystematicSet;


  /// \brief a basic interface for all systematics handles
  ///
  /// This is used by \ref SysListHandle to communicate with the
  /// individual data handles.

  class ISysHandleBase
  {
    /// \brief standard (virtual) destructor
  public:
    virtual ~ISysHandleBase () noexcept = default;

    /// \brief whether this is an empty handle
  public:
    virtual bool empty () const noexcept = 0;

    /// \brief get the name pattern before substitution
  public:
    virtual std::string getNamePattern () const = 0;

    /// \brief get the affecting systematics if this is an input
    /// handle
    ///
    /// This returns the empty set if this is either not an input
    /// handle, or if the input handle is not affected by systematics.
  public:
    virtual CP::SystematicSet
    getInputAffecting (const ISystematicsSvc& svc) const = 0;

    /// \brief register and cache the systematics
  public:
    virtual StatusCode
    fillSystematics (const ISystematicsSvc& svc,
                     const CP::SystematicSet& fullAffecting,
                     const std::vector<CP::SystematicSet>& sysList) = 0;
  };
}

#endif
