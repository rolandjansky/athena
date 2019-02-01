/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



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
    "pfJets",
    "fatJets",
    "electrons",
    "photons",
    "muons",
    "taus",
    "met",
    "met2",
    "eventCleaning",
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
    case OBJECT_PFLOW_JET:
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
    case OBJECT_CLEANING:
    case OBJECT_OVERLAP_REMOVAL:
    case OBJECT_TRIGGER:
    case OBJECT_NONE:
      return false;
    }
    RCU_ASSERT0 ("unknown object type");
    return false; //compilerDummy
  }
}
