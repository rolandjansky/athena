#ifndef PATINTERFACES_SYSTEMATICS_UTIL_H
#define PATINTERFACES_SYSTEMATICS_UTIL_H

//        Copyright Iowa State University 2014.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


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
