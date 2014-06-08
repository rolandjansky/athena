// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigBphysDict.h 600124 2014-06-03 15:35:20Z krasznaa $
#ifndef XAODTRIGBPHYS_AODTRIGBPHYSDICT_H
#define XAODTRIGBPHYS_AODTRIGBPHYSDICT_H

// STL include(s):
#include <vector>

// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"

// Local include(s):
#include "xAODTrigBphys/versions/TrigBphys_v1.h"
#include "xAODTrigBphys/versions/TrigBphysContainer_v1.h"
#include "xAODTrigBphys/versions/TrigBphysAuxContainer_v1.h"

namespace{
  struct GCCXML_DUMMY_INSTANTIATION_XAODTRIGBPHYS {

      xAOD::TrigBphysContainer_v1 bphys_l1;
      ElementLink< xAOD::TrigBphysContainer_v1 > bphys_l2;
      DataLink< xAOD::TrigBphysContainer_v1 > bphys_l3;
      std::vector< ElementLink< xAOD::TrigBphysContainer_v1 > > bphys_l4;
      std::vector< DataLink< xAOD::TrigBphysContainer_v1 > > bphys_l5;
      std::vector< std::vector< ElementLink< xAOD::TrigBphysContainer_v1 > > > bphys_l6;

  };
}

#endif // XAODTRIGBPHYS_AODTRIGBPHYSDICT_H
