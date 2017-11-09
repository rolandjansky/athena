#ifndef SYSTEMATICS_HANDLES__SYS_LIST_TYPE_H
#define SYSTEMATICS_HANDLES__SYS_LIST_TYPE_H

//        Copyright Iowa State University 2017.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


#include <string>
#include <vector>

namespace CP
{
  class SystematicSet;
}

namespace EL
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
