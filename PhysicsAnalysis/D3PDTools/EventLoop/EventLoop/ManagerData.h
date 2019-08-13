/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EVENT_LOOP__MANAGER_DATA_H
#define EVENT_LOOP__MANAGER_DATA_H

#include <EventLoop/Global.h>

#include <EventLoop/ManagerStep.h>
#include <SampleHandler/MetaObject.h>
#include <map>
#include <memory>
#include <string>
#include <vector>

class StatusCode;

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

    struct ManagerData final
    {
      /// \brief standard constructor
      ///
      /// This is mostly defined, so that this include file doesn't
      /// need to include a whole bunch of other files, just to
      /// destruct their objects
      ManagerData () noexcept;

      /// \brief standard destructor
      ///
      /// This is mostly defined, so that this include file doesn't
      /// need to include a whole bunch of other files, just to
      /// destruct their objects
      ~ManagerData () noexcept;


      /// \brief the driver we are working on/with
      const Driver *driver {nullptr};

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

      /// \brief whether we successfully submitted the job in this
      /// process
      bool submitted {false};

      /// \brief if we run in batch, the batch job object we are using
      std::unique_ptr<BatchJob> batchJob;

      /// \brief if we run in batch, this is the list of job-indices
      /// to run
      std::vector<std::size_t> batchJobIndices;

      /// \brief the current \ref ManagerStep
      ManagerStep step {ManagerStep::initial};

      /// \brief the next \ref ManagerStep
      ///
      /// By having this part of the manager data I am able to adjust
      /// the flow order via the managers (if desired)
      ManagerStep nextStep {ManagerStep::initial};


      /// \brief the list of managers to run
      std::map<ManagerOrder,std::unique_ptr<Manager> > managers;

      /// \brief add the given manager
      /// \par Guarantee
      ///   strong
      /// \par Failures
      ///   duplicate order entry\n
      ///   out of memory I
      void addManager (std::unique_ptr<Manager> manager);

      /// \brief run all the managers from start to finish
      /// \par Guarantee
      ///   basic
      /// \par Failures
      ///   manager errors
      ::StatusCode run ();
    };
  }
}

#endif
