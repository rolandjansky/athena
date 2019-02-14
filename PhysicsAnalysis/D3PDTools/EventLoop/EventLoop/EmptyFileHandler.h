/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack

#ifndef EVENT_LOOP__EMPTY_FILE_HANDLER_H
#define EVENT_LOOP__EMPTY_FILE_HANDLER_H

#include <EventLoop/Global.h>

#include <string>

namespace EL
{
  /// \brief check the output of the given (completed) job, rerun all
  /// (empty) files that did not get a fileExecute() and merge in the
  /// outputs
  ///
  /// This is primarily for the PROOF driver, which can't process
  /// empty files as part of the normal operation.
  /// \par Guarantee
  ///   basic
  /// \par Failures
  ///   i/o errors\n
  ///   job errors\n
  ///   events found in empty files
  void processEmptyFiles (const std::string& submitdir,
			  const Job& job);
}

#endif
