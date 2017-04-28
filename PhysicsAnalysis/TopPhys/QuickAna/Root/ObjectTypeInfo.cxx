/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//        Copyright Iowa State University 2015.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <QuickAna/ObjectTypeInfo.h>

#include <RootCoreUtils/Assert.h>

//
// method implementations
//

namespace ana
{
  const unsigned ObjectTypeInfo::numTypes;
  const unsigned ObjectTypeInfo::numDefTypes;

  const std::string ObjectTypeInfo::name [numDefTypes] =
  {
    "eventinfo",
    "eventSelect",
    "jets",
    "fatJets",
    "electrons",
    "photons",
    "muons",
    "taus",
    "met",
    "met2",
    "overlapRemoval",
    "trigger",
    "none",
  };

  bool ObjectTypeInfo ::
  supportsIParticle (ObjectType type)
  {
    switch (type)
    {
    case OBJECT_JET:
    case OBJECT_FAT_JET:
    case OBJECT_ELECTRON:
    case OBJECT_PHOTON:
    case OBJECT_MUON:
    case OBJECT_TAU:
      return true;
    case OBJECT_EVENTINFO:
    case OBJECT_EVENT_SELECT:
    case OBJECT_MET:
    case OBJECT_MET2:
    case OBJECT_OVERLAP_REMOVAL:
    case OBJECT_TRIGGER:
    case OBJECT_NONE:
      return false;
    }
    RCU_ASSERT0 ("unknown object type");
    return false; //compilerDummy
  }
}
