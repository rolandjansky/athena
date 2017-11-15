#ifndef SYSTEMATICS_HANDLES__I_SYS_HANDLE_BASE_H
#define SYSTEMATICS_HANDLES__I_SYS_HANDLE_BASE_H

//        Copyright Iowa State University 2017.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


#include <string>

namespace EL
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
