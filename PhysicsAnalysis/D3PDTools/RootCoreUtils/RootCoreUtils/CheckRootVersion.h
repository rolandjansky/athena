/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ROOT_CORE_UTILS__CHECK_ROOT_VERSION_H
#define ROOT_CORE_UTILS__CHECK_ROOT_VERSION_H

// This module still needs to be documented.  The interface provided
// in this module is intended for experts only.  The module is
// considered to be in the pre-alpha stage.



#include <RootCoreUtils/Global.h>

namespace RCU
{
  /// effects: check whether we are using a consistent root version
  /// guarantee: strong
  /// failures: version missmatch
  void check_root_version ();

  /// effects: disable the root version check
  /// guarantee: no-fail
  void disable_root_version_check ();
}

#endif
