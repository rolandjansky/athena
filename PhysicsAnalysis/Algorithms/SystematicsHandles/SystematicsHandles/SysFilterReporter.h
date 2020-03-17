/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef SYSTEMATICS_HANDLES__FILTER_REPORTER_H
#define SYSTEMATICS_HANDLES__FILTER_REPORTER_H

#include <AsgMessaging/AsgMessagingForward.h>
#include <xAODEventInfo/EventInfo.h>

namespace CP
{
  class SysFilterReporterCombiner;
  class SystematicSet;

  /// \brief a systematics aware filter reporter
  ///
  /// In order to ensure that the filter decision will always be set
  /// as part of `execute()`, this reporter class should be used like
  /// this:
  ///
  /// ```
  /// StatusCode Foo::execute()
  /// {
  ///   ...
  ///   static constexpr bool DEFAULT_DECISION = false;
  ///   SysFilterReporterCombiner filterCombiner
  ///       (m_filterParams, m_systematicsList, DEFAULT_DECISION);
  ///
  ///   return m_systematicsList.foreach ([&](const CP::SystematicSet &sys) -> StatusCode {
  ///     SysFilterReporter filter (filterCombiner, sys);
  ///     ...
  ///     filter.setPassed ();
  ///     return StatusCode::SUCCESS;
  ///     });
  /// }
  /// ```

  class SysFilterReporter final : public asg::AsgMessagingForward
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
    SysFilterReporter (SysFilterReporterCombiner& val_combiner,
                       const SystematicSet& val_sys);


    /// \brief standard destructor
    /// \par Guarantee
    ///   no-fail
  public:
    ~SysFilterReporter () noexcept;


    /// \brief report the filter decision
    /// \par Guarantee
    ///   no-fail
  public:
    void setPassed (bool val_passed = true) noexcept;



    //
    // private interface
    //

    /// \brief the \ref SysFilterReporterParams object
  private:
    SysFilterReporterCombiner& m_combiner;

    /// \brief the \ref SystematicSet object we use
  private:
    const SystematicSet& m_sys;

    /// \brief the event info object we are decorating
  private:
    xAOD::EventInfo *m_eventInfo {nullptr};

    /// \brief the value of passed we will set
  private:
    bool m_passed {false};
  };
}

#endif
