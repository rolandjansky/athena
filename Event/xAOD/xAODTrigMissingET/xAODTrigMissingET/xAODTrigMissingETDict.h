// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigMissingETDict.h 630930 2014-11-25 14:57:49Z gwatts $
#ifndef xAODTrigMissingET_xAODTrigMissingET_DICT_H
#define xAODTrigMissingET_xAODTrigMissingET_DICT_H

// Include all headers here that need to have dictionaries
// generated for them.

// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"

// Local include(s):
#include "xAODTrigMissingET/TrigMissingET.h"
#include "xAODTrigMissingET/TrigMissingETContainer.h"
#include "xAODTrigMissingET/versions/TrigMissingET_v1.h"
#include "xAODTrigMissingET/versions/TrigMissingETContainer_v1.h"
#include "xAODTrigMissingET/versions/TrigMissingETAuxContainer_v1.h"

namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODTRIGMISSINGET {
      xAOD::TrigMissingETContainer_v1 c1;
      DataLink< xAOD::TrigMissingETContainer_v1 > dl1;
      std::vector< DataLink< xAOD::TrigMissingETContainer_v1 > > dl2;
      ElementLink< xAOD::TrigMissingETContainer_v1 > el1;
      std::vector< ElementLink< xAOD::TrigMissingETContainer_v1 > > el2;
      std::vector< std::vector< ElementLink< xAOD::TrigMissingETContainer_v1 > > > el3;
   };
} // private namespace

#endif
