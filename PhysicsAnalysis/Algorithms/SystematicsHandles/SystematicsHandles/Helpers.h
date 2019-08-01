/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SYSTEMATICS_HANDLES__HELPERS_H
#define SYSTEMATICS_HANDLES__HELPERS_H

#include <string>

namespace CP
{
  class SystematicSet;

  /// \brief nominal systematics name
  std::string nominalSystematicsName ();


  /// \brief make the name for the given systematics
  /// \par Guarantee
  ///   strong
  /// \par Failures
  ///   out of memory II
  std::string makeSystematicsName (const std::string& name,
                                   const CP::SystematicSet& sys);


  /// \brief make the name for the given systematics, filtered for
  /// the given affecting pattern
  /// \par Guarantee
  ///   strong
  /// \par Failures
  ///   out of memory II
  std::string makeSystematicsName (const std::string& name,
                                   const std::string& affecting,
                                   const CP::SystematicSet& sys);
}

#endif
