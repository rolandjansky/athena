// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODHIEventDict.h 612503 2014-08-19 11:58:04Z krasznaa $
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

namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODHIEVENT {

      xAOD::HIEventShapeContainer_v1 c1;

      DataLink< xAOD::HIEventShape_v1 > dl1;
      std::vector< DataLink< xAOD::HIEventShape_v1 > > dl2;

      ElementLink< xAOD::HIEventShapeContainer_v1 > el1;
      std::vector< ElementLink< xAOD::HIEventShapeContainer_v1 > > el2;
      std::vector< std::vector< ElementLink< xAOD::HIEventShapeContainer_v1 > > > el3;

      std::set< uint32_t > set1;
      std::vector< std::set< uint32_t > > set2;

   };
} // private namespace

#endif // XAODEVENTINFO_XAODEVENTINFODICT_H
