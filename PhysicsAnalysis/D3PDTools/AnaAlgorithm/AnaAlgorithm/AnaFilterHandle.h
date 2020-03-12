/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ANA_ALGORITHM__ANA_FILTER_HANDLE_H
#define ANA_ALGORITHM__ANA_FILTER_HANDLE_H

#include <AnaAlgorithm/Global.h>

#include <AsgMessaging/AsgMessagingForward.h>
#include <functional>

class StatusCode;

namespace EL
{
  /// \brief a handle for applying algorithm filter decisions
  ///
  /// This is meant to replace the old/traditional mechanism for
  /// recording filter decisions by algorithms, i.e. calling
  /// setFilterPassed() on the algorithm base class.  This allows to
  /// selectively add filter capabilties to algorithms without having
  /// to derive from a dedicated algorithm class like
  /// `AthFilterAlgorithm`.
  ///
  /// One of the particular reasons to handle filter decisions through
  /// a handle is that it makes it fairly easy to add instrumentation
  /// (e.g. reporting to the cut flow service), without having to make
  /// changes to the base class or indeed without having any extra
  /// code in the base class at all.  Also it makes sure that only the
  /// actual filter algorithms expose properties related to reporting
  /// filter decisions (though that would also be true when using a
  /// base class).
  ///
  /// This should in principle work with any algorithm (or we should
  /// be able to make it work with any algorithm).  While there isn't
  /// any intrinsic reason one couldn't have more than one filter
  /// handle per algorithm, that is currently (02 Mar 20) not
  /// supported, but could be added on request.
  ///
  /// Currently (02 Mar 20) none of the member functions is marked
  /// `const`, which is deliberate to prevent them being called from a
  /// reentrant algorithm.  I suspect if we ever want to use this in
  /// AthenaMT, we will add a second set of member functions that is
  /// marked `const` and will take an event context as an argument.

  class AnaFilterHandle final : public asg::AsgMessagingForward
  {
    //
    // public interface
    //

    /// \brief standard constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory I
  public:
    template<typename T>
    explicit AnaFilterHandle (T *owner);


    /// \brief do anything we need to do in initialize
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   configuration/initialization errors
  public:
    StatusCode initialize ();



    //
    // private interface
    //

    // this class does most of the actual work, and needs our
    // internals for that.
    friend class AnaFilterHandleGuard;

    /// \brief the function to call setFilterPassed() on the algorithm
    ///
    /// This is using a `std::function` object, so as to avoid tying
    /// this to a particular algorithm class.
  private:
    std::function<void(bool)> m_setFilterPassed;

    /// \brief whether the handle was initialized
  private:
    bool m_isInitialized {false};
  };



  //
  // inline/template methods
  //

  template<typename T> AnaFilterHandle ::
  AnaFilterHandle (T *owner)
    : AsgMessagingForward (owner)
    , m_setFilterPassed ([owner] (bool val_setFilterPassed) {owner->setFilterPassed (val_setFilterPassed);})
  {}
}

#endif
