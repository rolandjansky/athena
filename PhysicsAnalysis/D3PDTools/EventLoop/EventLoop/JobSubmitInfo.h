/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EVENT_LOOP__JOB_SUBMIT_INFO_H
#define EVENT_LOOP__JOB_SUBMIT_INFO_H

#include <EventLoop/Global.h>

#include <SampleHandler/MetaObject.h>
#include <string>
#include <vector>

namespace EL
{
  namespace Detail
  {
    /// \brief an internal data structure for passing data between
    /// different job routines
    ///
    /// \warn This is an **internal** data structure that users should
    /// not directly interface with.  It is not guaranteed to stay
    /// around and can change or even disappear from one EventLoop
    /// version to the next.
    ///
    /// The main reason for this design is that I have a fair amount
    /// of information that I need to prepare a job for
    /// submission/execution, and the process involves a number of
    /// steps.  By centralizing all that information I can more easily
    /// restructure the submission process and factor out utility
    /// functions.

    struct JobSubmitInfo final
    {
      /// \brief standard constructor
      ///
      /// This is mostly defined, so that this include file doesn't
      /// need to include a whole bunch of other files, just to
      /// destruct their objects
      JobSubmitInfo () noexcept;

      /// \brief standard destructor
      ///
      /// This is mostly defined, so that this include file doesn't
      /// need to include a whole bunch of other files, just to
      /// destruct their objects
      ~JobSubmitInfo () noexcept;


      /// \brief the path to the submission directory
      std::string submitDir;

      /// \brief whether we are merely resubmitting this job
      bool resubmit {false};

      /// \brief if we are resubmitting, the resubmit option chosen
      std::string resubmitOption;

      /// \brief the options with which we submit this job
      SH::MetaObject options;

      /// \brief the user job we are submitting
      Job *job {nullptr};

      /// \brief if we run in batch, the batch job object we are using
      std::unique_ptr<BatchJob> batchJob;

      /// \brief if we run in batch, this is the list of job-indices
      /// to run
      std::vector<std::size_t> batchJobIndices;
    };
  }
}

#endif
