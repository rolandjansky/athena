/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef CHECKEXC_H
#define CHECKEXC_H

#include <ostream>
#include <stdexcept>

/// Checks the result of a call, and if it is false, or StatusCode::FAILURE,
/// raise an exception.  Use this for cases where you don't have code to recover
/// from a failure, or where you don't expect the command to fail under normal
/// circumstances.
#define CHECK_EXC(ARG)                                                        \
  do {                                                                        \
    const bool result = static_cast<bool>(ARG);                               \
    if (!result) {                                                            \
      std::ostringstream os;                                                  \
      os << "Failed to execute: \"" << #ARG << "\"";                          \
      throw std::runtime_error(os.str());                                     \
    }                                                                         \
  } while (false)

#endif//CHECKEXC_H
