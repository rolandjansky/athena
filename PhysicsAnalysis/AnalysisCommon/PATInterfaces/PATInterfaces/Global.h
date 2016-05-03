#ifndef PATINTERFACES_GLOBAL_H
#define PATINTERFACES_GLOBAL_H

//        Copyright Iowa State University 2014.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


// This module still needs to be documented.  The interface provided
// in this module is intended for experts only.  The module is
// considered to be in the pre-alpha stage.

namespace CP
{
  class CorrectionCode;
  template<class T> class ICorrectionTool;
  class MakeSystematicsVector;
  class ISystematicsTool;
  class SystematicCode;
  class SystematicVariation;
  class SystematicSet;
  class SystematicsTool;
}

bool operator < (const CP::SystematicVariation& a, const CP::SystematicVariation& b);
bool operator == (const CP::SystematicVariation& a, const CP::SystematicVariation& b);

bool operator < (const CP::SystematicSet& a, const CP::SystematicSet& b);
bool operator == (const CP::SystematicSet& a, const CP::SystematicSet& b);

#endif
