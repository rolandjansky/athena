#ifndef SYSTEMATICS_HANDLES__HELPERS_H
#define SYSTEMATICS_HANDLES__HELPERS_H

//        Copyright Iowa State University 2017.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


#include <string>

namespace CP
{
  class SystematicSet;
}

namespace EL
{
  namespace detail
  {
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
}

#endif
