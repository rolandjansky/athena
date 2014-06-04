// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODMissingETDict.h 595403 2014-05-05 12:58:53Z krasznaa $
#ifndef XAODMISSINGET_XAODMISSINGETDICT_H
#define XAODMISSINGET_XAODMISSINGETDICT_H

// System include(s):
#include <vector>

// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"

// Local include(s):
#include "xAODMissingET/versions/MissingETContainer_v1.h"
#include "xAODMissingET/versions/MissingETAuxContainer_v1.h"
#include "xAODMissingET/versions/MissingETComponent_v1.h"
#include "xAODMissingET/versions/MissingETComponentMap_v1.h"
#include "xAODMissingET/versions/MissingETAuxComponentMap_v1.h"
#include "xAODMissingET/versions/MissingETBase.h"

namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODMISSINGET {
      // Container(s):
      DataVector< xAOD::MissingET_v1 > c1;
      xAOD::MissingETContainer_v1 c2;
      DataVector< xAOD::MissingETComponent_v1 > c3;
      xAOD::MissingETComponentMap_v1 c4;
      // Smart pointer(s):
      DataLink< xAOD::MissingETContainer_v1 > dl1;
      std::vector< DataLink< xAOD::MissingETContainer_v1 > > dl2;
      ElementLink< xAOD::MissingETContainer_v1 > el1;
      std::vector< ElementLink< xAOD::MissingETContainer_v1 > > el2;
      std::vector< std::vector< ElementLink< xAOD::MissingETContainer_v1 > > > el3;
      DataLink< xAOD::MissingETComponentMap_v1 > dl3;
      std::vector< DataLink< xAOD::MissingETComponentMap_v1 > > dl4;
      ElementLink< xAOD::MissingETComponentMap_v1 > el4;
      std::vector< ElementLink< xAOD::MissingETComponentMap_v1 > > el5;
      std::vector< std::vector< ElementLink< xAOD::MissingETComponentMap_v1 > > > el6;

      // Smart pointers needed for the correct generation of the auxiliary
      // class dictionaries:
      ElementLink< xAOD::IParticleContainer > el7;
      std::vector< ElementLink< xAOD::IParticleContainer > > el8;
      std::vector< std::vector< ElementLink< xAOD::IParticleContainer > > > el9;
      std::vector< MissingETBase::Types::bitmask_t > vec1;
      std::vector< unsigned long long > vec2;
   };
}

#endif // XAODMISSINGET_XAODMISSINGETDICT_H
