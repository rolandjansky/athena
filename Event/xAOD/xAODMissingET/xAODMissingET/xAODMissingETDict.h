// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODMissingETDict.h 693950 2015-09-09 15:29:18Z khoo $
#ifndef XAODMISSINGET_XAODMISSINGETDICT_H
#define XAODMISSINGET_XAODMISSINGETDICT_H

// Needed to successfully generate the dictionary in standalone mode:
#if defined(__GCCXML__) and not defined(EIGEN_DONT_VECTORIZE)
#   define EIGEN_DONT_VECTORIZE
#endif // __GCCXML__

// System include(s):
#include <vector>

// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"

// Local include(s):
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETComponentMap.h"
#include "xAODMissingET/MissingETAuxComponentMap.h"
#include "xAODMissingET/MissingETAssociationMap.h"
#include "xAODMissingET/MissingETAuxAssociationMap.h"
#include "xAODMissingET/versions/MissingETAuxAssociationMap_v1.h"
#include "xAODMissingET/versions/MissingETBase.h"

namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODMISSINGET {
      // Container(s):
      DataVector< xAOD::MissingET_v1 > c1;
      xAOD::MissingETContainer_v1 c2;
      DataVector< xAOD::MissingETComponent_v1 > c3;
      xAOD::MissingETComponentMap_v1 c4;
      DataVector< xAOD::MissingETAssociation_v1 > c5;
      xAOD::MissingETAssociationMap_v1 c6;
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
      DataLink< xAOD::MissingETAssociationMap_v1 > dl5;
      std::vector< DataLink< xAOD::MissingETAssociationMap_v1 > > dl6;
      ElementLink< xAOD::MissingETAssociationMap_v1 > el7;
      std::vector< ElementLink< xAOD::MissingETAssociationMap_v1 > > el8;
      std::vector< std::vector< ElementLink< xAOD::MissingETAssociationMap_v1 > > > el9;

      // Smart pointers needed for the correct generation of the auxiliary
      // class dictionaries:
      ElementLink< xAOD::IParticleContainer > el10;
      std::vector< ElementLink< xAOD::IParticleContainer > > el11;
      std::vector< std::vector< ElementLink< xAOD::IParticleContainer > > > el12;

      std::vector< MissingETBase::Types::bitmask_t > vec1;
      std::vector< unsigned long long > vec2;
      std::vector< std::vector< size_t > > vec3;
      std::vector< std::vector< unsigned char > > vec4;
      std::vector< std::vector< unsigned long long > > vec5;
      std::vector< std::vector< std::vector< size_t > > > vec6;
      std::vector< std::vector< std::vector< unsigned long > > > vec7;
      std::vector< std::vector< std::vector< unsigned char > > > vec8;
      std::vector< std::vector<std::vector<unsigned long long> > > vec9;
   };
}

#endif // XAODMISSINGET_XAODMISSINGETDICT_H
