// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigBphysDict.h 632753 2014-12-01 17:04:27Z jwalder $
#ifndef XAODTRIGBPHYS_AODTRIGBPHYSDICT_H
#define XAODTRIGBPHYS_AODTRIGBPHYSDICT_H

// Needed to successfully generate the dictionary in standalone mode:
#if defined(__GCCXML__) and not defined(EIGEN_DONT_VECTORIZE)
#   define EIGEN_DONT_VECTORIZE
#endif // __GCCXML__

// STL include(s):
#include <vector>

// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"

// Local include(s):
#include "xAODTrigBphys/TrigBphysContainer.h"
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
      std::vector< std::vector< ElementLink< xAOD::TrackParticleContainer > > > bphys_17;

     // Instantiate the classes used by xAOD::xAODTrigBphys so that
     // Reflex would see them with their "correct type". Note that the
     // dictionary for these types comes from other places. This instantiation
     // is just needed for "Reflex related technical reasons"...
     ElementLink< xAOD::TrackParticleContainer > auxlink1;
     std::vector< ElementLink< xAOD::TrackParticleContainer > > auxlink2;
     std::vector< std::vector< ElementLink< xAOD::TrackParticleContainer > > > auxlink3;

     ElementLink< xAOD::IParticleContainer > auxlink4;
     std::vector< ElementLink< xAOD::IParticleContainer > > auxlink5;
     std::vector< std::vector< ElementLink< xAOD::IParticleContainer > > > auxlink6;
  };
}

#endif // XAODTRIGBPHYS_AODTRIGBPHYSDICT_H
