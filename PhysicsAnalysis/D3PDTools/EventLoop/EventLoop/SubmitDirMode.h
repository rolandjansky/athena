/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EVENT_LOOP__SUBMIT_DIR_MODE_H
#define EVENT_LOOP__SUBMIT_DIR_MODE_H

#include <EventLoop/Global.h>

namespace EL
{
  namespace Detail
  {
    /// \brief the mode for creating the submission directory

    enum class SubmitDirMode
    {
      /// create the directory as is, raise an error if it already
      /// exists
      NO_CLOBBER,

      /// create the directory as is, removing existing directories if
      /// needed
      OVERWRITE,

      /// append a unique suffix to the directory name
      UNIQUE,

      /// append a unique suffix to the directory name and place a
      /// symbolic link in place of the originally requested directory
      UNIQUE_LINK
    };
  }
}

#endif
