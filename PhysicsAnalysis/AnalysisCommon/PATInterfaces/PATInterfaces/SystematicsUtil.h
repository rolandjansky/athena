/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef PATINTERFACES_SYSTEMATICS_UTIL_H
#define PATINTERFACES_SYSTEMATICS_UTIL_H

#include <PATInterfaces/Global.h>
#include <vector>

namespace CP
{
  /// utility functions for working with systematics
  ///
  /// @author Nils Krumnack <nils.erik.krumnack@cern.ch>
  /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>

  /// effects: determine the list of naive +/-1 sigma variations based
  ///   on the list of recommended/supported systematics
  /// returns: the individual NP points to be evaluated
  /// guarantee: strong
  /// failures: out of memory II
  /// rationale: eventually the statistics forum is going to provide a
  ///   tool that places NP points much more elegantly, but for now
  ///   this should allow users to use the list in a reasonably
  ///   elegant fashion
  std::vector<CP::SystematicSet>
  make_systematics_vector (const SystematicSet& systematics);
}

#endif
