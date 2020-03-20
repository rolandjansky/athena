/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef SYSTEMATICS_HANDLES__FILTER_REPORTER_COMBINER_H
#define SYSTEMATICS_HANDLES__FILTER_REPORTER_COMBINER_H

#include <AsgMessaging/AsgMessagingForward.h>

class EventContext;

namespace CP
{
  class SysFilterReporterParams;
  class SysListHandle;


  /// \brief a reporter class that combines the filter decisions for
  /// all systematics
  ///
  /// Unlike the non-systematics aware version, when dealing with
  /// systematics we need to combine all the individual filter
  /// decisions into an overall filter decision for the algorithm,
  /// i.e. if and only if the filter decision fails for all
  /// systematics we should set filter to failed for the algorithm
  /// itself, so that we avoid/skip processing of subsequent
  /// algorithms.  For details on usage see \ref SysFilterReporter.

  class SysFilterReporterCombiner final : public asg::AsgMessagingForward
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
    SysFilterReporterCombiner (SysFilterReporterParams& val_params,
                               SysListHandle& systematicsList,
                               bool val_passedDefault);


    /// \brief standard destructor
    /// \par Guarantee
    ///   no-fail
  public:
    ~SysFilterReporterCombiner () noexcept;



    //
    // private interface
    //

    // this class does most of the actual work and acts as accessor to
    // our private data members, and needs our internals for that.
    friend class SysFilterReporter;

    /// \brief the \ref SysFilterReporterParams object
  private:
    SysFilterReporterParams& m_params;

    /// \brief the default value of passed we use for each individual reporter
  private:
    bool m_passedDefault {false};

    /// \brief whether we passed for at least one/all systematics
  private:
    bool m_passedOne {false};
    bool m_passedAll {true};
  };
}

#endif
