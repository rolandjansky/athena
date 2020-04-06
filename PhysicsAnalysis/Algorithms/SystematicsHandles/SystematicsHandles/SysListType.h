/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SYSTEMATICS_HANDLES__SYS_LIST_TYPE_H
#define SYSTEMATICS_HANDLES__SYS_LIST_TYPE_H

#include <string>
#include <vector>
#include <AsgTools/CLASS_DEF.h>

namespace CP
{
  class SystematicSet;
}

CLASS_DEF( std::vector<CP::SystematicSet> , 79952367 , 1 )

namespace CP
{
  /// \brief the type for the systematics list in the event store
  ///
  /// For now this is just a vector of SystematicSet, but I made a
  /// typedef for this as I am not sure this will necessarily be
  /// performant in all situations.  This way at least I will get
  /// compilation errors if I change it.
  typedef std::vector<CP::SystematicSet> SysListType;

  /// \brief the default name for putting the systematics list into
  /// the event store
  const std::string& sysListDefaultName ();
}

#endif
