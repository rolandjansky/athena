/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EVENT_LOOP__MANAGER_STEP_H
#define EVENT_LOOP__MANAGER_STEP_H

#include <EventLoop/Global.h>

namespace EL
{
  namespace Detail
  {
    /// \brief the individual steps of preparing a job for submission
    ///
    /// Essentially the \ref Driver and \ref Manager objects are giant
    /// state machines, and this denotes the current overall state
    /// (with extra information in \ref ManagerData).
    ///
    /// It should be noted that not every step will be used on every
    /// operation (e.g. submit or retrieve), but instead there is a
    /// separate sequence of steps for each of them.
    ///
    /// The alternative to this enum would be to have a separate
    /// callback in the \ref Manager class for each step, which would
    /// have the advantage that there isn't one function with a very
    /// large `switch` statement, but this approach has other
    /// advantages:
    /// * This makes it clear and obvious in which order steps get
    ///   executed.
    /// * The effort for adding/removing individual steps is a lot
    ///   lower with an enum than with virtual functions.
    /// * It is very easy to bind the same action to multiple steps,
    ///   which comes into play if that action needs to be performed
    ///   for multiple operations.

    enum class ManagerStep
    {
      /// this is just the initial step we do, nothing really happens
      /// here
      initial,

      /// the initial submit step
      initialSubmit,

      /// update the submitDir variable to be an absolute path
      updateSubmitDir,

      /// in this step we fill the options structure, combining the
      /// different locations from which we can pull configuration
      /// options.
      fillOptions,

      /// extract any options into ManagerData for which it is
      /// appropriate
      extractOptions,

      /// create the submission directory
      createSubmitDir,

      /// in this step we update the output location of streams that
      /// don't yet have one defined
      updateOutputLocation,

      /// add any system algorithms to the job that may need to get
      /// added
      addSystemAlgs,

      /// prepare files in our submission directory
      prepareSubmitDir,

      /// create the directories needed in the batch directory
      batchCreateDirectories,

      /// create the batch-driver job object
      batchCreateJob,

      /// create the variables needed for the batch-run script
      batchScriptVar,

      /// create the batch-run script
      batchMakeScript,

      /// create the list of batch job indices
      batchMakeIndices,

      /// do the actual job submission
      submitJob,

      /// save the output in the direct driver
      directSaveOutput,

      /// do whatever needs to be done after the submission is done
      postSubmit,

      /// the final submit step
      finalSubmit,

      /// the initial resubmit step
      initialResubmit,

      /// read the configuration for resubmit
      readConfigResubmit,

      /// read the job status in batch driver
      batchJobStatusResubmit,

      /// do everything the batch driver needs to do before the resubmit
      batchPreResubmit,

      /// call the actual doResubmit method
      doResubmit,

      /// the final resubmit step
      finalResubmit,

      /// the initial retrieve step
      initialRetrieve,

      /// read the configuration for retrieve
      readConfigRetrieve,

      /// read the job status in batch driver
      batchJobStatusRetrieve,

      /// call the actual doRetrieve method
      doRetrieve,

      /// the final retrieve step
      finalRetrieve,

      final
    };
  }
}

#endif
