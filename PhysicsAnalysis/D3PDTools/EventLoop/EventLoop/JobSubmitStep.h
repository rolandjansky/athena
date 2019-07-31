/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EVENT_LOOP__JOB_SUBMIT_STEP_H
#define EVENT_LOOP__JOB_SUBMIT_STEP_H

#include <EventLoop/Global.h>

namespace EL
{
  namespace Detail
  {
    /// \brief the individual steps of preparing a job for submission
    ///
    /// The basic idea is that this allows individual drivers to
    /// augment specific steps in the process in a fairly transparent
    /// fashion.  In principle this could be done as a series of
    /// virtual functions inside \ref Driver, but for now it feels
    /// better to use an enum to cycle through the callbacks.  This
    /// avoids having a large number of virtual functions inside \ref
    /// Driver, and it makes the order of calls a lot more clear.

    enum class JobSubmitStep
    {
      /// this is just the initial step we do, nothing really happens
      /// here
      initial,

      /// \brief update the submitDir variable to be an absolute path
      updateSubmitDir = initial,

      /// in this step we fill the options structure, combining the
      /// different locations from which we can pull configuration
      /// options.
      fillOptions,

      /// in this step we update the output location of streams that
      /// don't yet have one defined
      updateOutputLocation,

      /// add any system algorithms to the job that may need to get
      /// added
      addSystemAlgs,

      /// create the submission directory
      createSubmitDir,

      /// prepare files in our submission directory
      prepareSubmitDir,

      /// create the directories needed in the batch directory
      batchCreateDirectories,

      /// create the batch-driver job object
      batchCreateJob,

      /// create the batch-run script
      batchMakeScript,

      /// create the list of batch job indices
      batchMakeIndices,

      /// do the actual job submission
      submitJob,

      /// do whatever needs to be done after the submission is done
      postSubmit,

      final = postSubmit
    };
  }
}

#endif
