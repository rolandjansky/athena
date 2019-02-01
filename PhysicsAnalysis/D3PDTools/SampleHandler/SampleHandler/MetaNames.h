/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef SAMPLE_HANDLER__META_NAMES_H
#define SAMPLE_HANDLER__META_NAMES_H

#include <SampleHandler/Global.h>

#include <string>

namespace SH
{
  /// \brief the names of common meta-data
  ///
  /// This is supposed to feed into the names in \ref SH::MetaFields
  /// and \ref EL::Job as this uses static functions which have no
  /// load order dependencies.  Over time it may also replace them.

  struct MetaNames
  {
    /// \brief the number of retries for opening a file
    static const std::string& openRetries ();

    /// \brief the default value of \ref openRetries
    static unsigned openRetries_default ();


    /// \brief the amount of time (in seconds) to wait when doing
    /// retries
    ///
    /// when an open fails this is the maximum number of seconds to
    /// wait, the actual time will be randomized.  the rationale is
    /// that if the open failed because a hundred batch jobs try to
    /// open a file at the same time, then it is not very helpful if
    /// those hundred batch jobs retry all at the same time.
    static const std::string& openRetriesWait ();

    /// \brief the default value of \ref openRetriesWait
    static double openRetriesWait_default ();


    /// \brief the name of the sample being processed
    ///
    /// this gets set inside the Sample accessor methods for the name.
    /// the point here is that normally you would access the sample
    /// name via the accessors, but if all you have is the sample
    /// meta-data you can use that to extract the name
    static const std::string& sampleName ();
  };
}

#endif
