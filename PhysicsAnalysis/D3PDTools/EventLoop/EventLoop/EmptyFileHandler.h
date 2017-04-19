/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP__EMPTY_FILE_HANDLER_H
#define EVENT_LOOP__EMPTY_FILE_HANDLER_H

//        Copyright Iowa State University 2015.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


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
