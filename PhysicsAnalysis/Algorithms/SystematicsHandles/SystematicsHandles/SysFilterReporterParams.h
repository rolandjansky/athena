/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef SYSTEMATICS_HANDLES__FILTER_REPORTER_PARAMS_H
#define SYSTEMATICS_HANDLES__FILTER_REPORTER_PARAMS_H

#include <AsgMessaging/AsgMessagingForward.h>
#include <SystematicsHandles/SysCopyHandle.h>
#include <SystematicsHandles/SysDecorationHandle.h>
#include <xAODEventInfo/EventInfo.h>
#include <functional>

class StatusCode;

namespace CP
{
  /// \brief the properties and persistent data for systematics aware
  /// filter reporters
  ///
  /// This is a systematics-aware version of the \ref
  /// EL::FilterReporter mechanism.  This is somewhat more
  /// complicated, since there is per-systematics filter decisions and
  /// an overall event-level filter decision, for details see \ref
  /// SysFilterReporter.

  class SysFilterReporterParams final : public asg::AsgMessagingForward
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
    explicit SysFilterReporterParams (T *owner);


    /// \brief do anything we need to do in initialize
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   configuration/initialization errors
  public:
    StatusCode initialize ();


    /// \brief do anything we need to do in finalize
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   finalization errors
  public:
    StatusCode finalize ();



    //
    // private interface
    //

    // this class does most of the actual work, and needs our
    // internals for that.
    friend class SysFilterReporterCombiner;
    friend class SysFilterReporter;

    /// \brief the function to call setFilterPassed() on the algorithm
    ///
    /// This is using a `std::function` object, so as to avoid tying
    /// this to a particular algorithm class.
  private:
    std::function<void(bool)> m_setFilterPassed;

    /// \brief the event info we run on
  private:
    SysCopyHandle<xAOD::EventInfo> m_eventInfoHandle;

    /// \brief the decoration for writing the scale factor
  private:
    SysDecorationHandle<char> m_eventDecisionOutputDecoration;

    /// \brief counter for passed events
  private:
    unsigned m_passedOne {0};
    unsigned m_passedAll {0};

    /// \brief counter for total events
  private:
    unsigned m_total {0};

    /// \brief whether the handle was initialized
  private:
    bool m_isInitialized {false};
  };



  //
  // inline/template methods
  //

  template<typename T> SysFilterReporterParams ::
  SysFilterReporterParams (T *owner)
    : AsgMessagingForward (owner)
    , m_setFilterPassed ([owner] (bool val_setFilterPassed) {owner->setFilterPassed (val_setFilterPassed);})
    , m_eventInfoHandle (owner, "eventInfo", "EventInfo", "the event info object to run on")
    , m_eventDecisionOutputDecoration (owner, "eventDecisionOutputDecoration", "", "the decoration for the event decision")
  {}
}

#endif
