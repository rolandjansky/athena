// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODHIEventDict.h 693494 2015-09-07 14:59:45Z krasznaa $
#ifndef XAODHIEVENT_XAODHIEVENTDICT_H
#define XAODHIEVENT_XAODHIEVENTDICT_H

// System include(s):
#include <vector>
#include <set>

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "AthLinks/DataLink.h"

// Local include(s):
#include "xAODHIEvent/HIEventShape.h"
#include "xAODHIEvent/HIEventShapeContainer.h"
#include "xAODHIEvent/HIEventShapeAuxContainer.h"
#include "xAODHIEvent/versions/HIEventShape_v1.h"
#include "xAODHIEvent/versions/HIEventShapeContainer_v1.h"
#include "xAODHIEvent/versions/HIEventShapeAuxContainer_v1.h"
#include "xAODHIEvent/versions/HIEventShape_v2.h"
#include "xAODHIEvent/versions/HIEventShapeContainer_v2.h"
#include "xAODHIEvent/versions/HIEventShapeAuxContainer_v2.h"

/// Declare a dummy CLID for xAOD::HIEventShape_v1 and
/// xAOD::HIEventShapeContainer_v1. To be able to still
/// define/use smart pointers for them. For schema evolution purposes.
CLASS_DEF( xAOD::HIEventShape_v1, 11112222, 10 )
CLASS_DEF( xAOD::HIEventShapeContainer_v1, 22223333, 10 )

namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODHIEVENT {

      xAOD::HIEventShapeContainer_v1 c1;
      xAOD::HIEventShapeContainer_v2 c2;

      DataLink< xAOD::HIEventShape_v1 > dl1;
      std::vector< DataLink< xAOD::HIEventShape_v1 > > dl2;
      DataLink< xAOD::HIEventShape_v2 > dl3;
      std::vector< DataLink< xAOD::HIEventShape_v2 > > dl4;

      ElementLink< xAOD::HIEventShapeContainer_v1 > el1;
      std::vector< ElementLink< xAOD::HIEventShapeContainer_v1 > > el2;
      std::vector< std::vector< ElementLink< xAOD::HIEventShapeContainer_v1 > > > el3;
      ElementLink< xAOD::HIEventShapeContainer_v2 > el4;
      std::vector< ElementLink< xAOD::HIEventShapeContainer_v2 > > el5;
      std::vector< std::vector< ElementLink< xAOD::HIEventShapeContainer_v2 > > > el6;

      std::set< uint32_t > set1;
      std::vector< std::set< uint32_t > > set2;

   };
} // private namespace

#endif // XAODEVENTINFO_XAODEVENTINFODICT_H
