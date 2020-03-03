/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ANA_ALGORITHM__ANA_FILTER_HANDLE_GUARD_H
#define ANA_ALGORITHM__ANA_FILTER_HANDLE_GUARD_H

#include <AnaAlgorithm/Global.h>

#include <AsgMessaging/AsgMessagingForward.h>

namespace EL
{
  /// \brief a guard class for use with ref AnaFilterHandle
  ///
  /// In order to ensure that the filter decision will always be set
  /// as part of `execute()`, this guard class should be used like
  /// this:
  ///
  /// ```
  /// StatusCode Foo::execute()
  /// {
  ///   ...
  ///   static constexpr bool DEFAULT_DECISION = false;
  ///   EL::AnaFilterHandleGuard filterGuard (m_filterHandle, DEFAULT_DECISION);
  ///   ...
  ///   filterGuard.setPassed ();
  ///   return StatusCode::SUCCESS;
  /// }
  /// ```

  class AnaFilterHandleGuard final : public asg::AsgMessagingForward
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
    AnaFilterHandleGuard (AnaFilterHandle& val_handle,
                          bool val_passedDefault);


    /// \brief standard destructor
    /// \par Guarantee
    ///   no-fail
  public:
    ~AnaFilterHandleGuard () noexcept;


    /// \brief report the filter decision
    /// \par Guarantee
    ///   no-fail
  public:
    void setPassed (bool val_passed = true) noexcept;



    //
    // private interface
    //

    /// \brief the \ref AnaFilterHandle object
  private:
    AnaFilterHandle& m_handle;

    /// \brief the value of passed we will set
  private:
    bool m_passed {false};
  };
}

#endif
