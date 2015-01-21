#ifndef PATINTERFACES_SYSTEMATIC_LIST_H
#define PATINTERFACES_SYSTEMATIC_LIST_H

//        Copyright Iowa State University 2014.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


// This module provides the global lists of systematics as well as
// some tools for working with lists of systematics.  The interface
// provided in this module is intended for the general user.  The
// module is considered to be in the pre-alpha stage.



#include <PATInterfaces/Global.h>

#include <PATInterfaces/SystematicVariation.h>
#include <set>

namespace CP
{
  /// description: the list of all known systematics
  typedef std::set<SystematicVariation> systematics_list_type;
  systematics_list_type& systematics_list();


  /// description: the list of all systematics that should be applied
  ///   by default
  systematics_list_type& default_systematics();


  /// returns: the given systematics configuration joined into a single string
  /// guarantee: strong
  /// failures: out of memory II
  std::string
  joinSystematicList (const std::set<SystematicVariation>& systConfig);


  /// returns: the given systematics string split into individual
  ///   systematics
  /// guarantee: strong
  /// failures: out of memory II
  std::set<SystematicVariation>
  splitSystematicList (const std::string& systematics);


  /// effects: filter the systematics for the affected systematics
  /// returns: success
  /// guarantee: strong
  /// failures: out of memory II
  /// failures: requesting a systematic that's not on the global list
  /// failures: requesting multiple variations on the same systematic
  ///   (e.g. up & down)
  /// failures: requesting an unsupported variation on an otherwise
  ///   supported systematic (e.g. a 2 sigma variation and the tool
  ///   only supports 1 sigma variations)
  SystematicCode filterForAffectingSystematics
  (const std::set<SystematicVariation>& systConfig,
   const std::set<SystematicVariation>& affectingSystematics,
   std::set<SystematicVariation>& filteredSystematics);
}

#endif
