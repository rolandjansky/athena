/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SYSTEMATICS_HANDLES__I_SYS_HANDLE_BASE_H
#define SYSTEMATICS_HANDLES__I_SYS_HANDLE_BASE_H

#include <string>

namespace CP
{
  /// \brief a basic interface for all systematics handles
  ///
  /// This is used by \ref SysListHandle to communicate with the
  /// individual data handles.

  class ISysHandleBase
  {
    /// \brief standard (virtual) destructor
  public:
    virtual ~ISysHandleBase () noexcept = default;

    /// \brief get the regular expression for the affecting
    /// systematics if this is an input handle
    ///
    /// This returns the empty string if this is either not an input
    /// handle, or if the input handle is not affected by systematics.
    ///
    /// This should only be called inside execute(), not initialize(),
    /// to allow upgrades to reading the affecting systematics from
    /// the event store.
  public:
    virtual std::string getInputAffecting () const = 0;
  };
}

#endif
