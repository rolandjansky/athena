// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODASSOCIATIONS_XAODASSOCIATIONSDICT_H
#define XAODASSOCIATIONS_XAODASSOCIATIONSDICT_H

// Needed to successfully generate the dictionary in standalone mode:
#if defined(__GCCXML__) and not defined(EIGEN_DONT_VECTORIZE)
# ifndef EIGEN_DONT_VECTORIZE
#   define EIGEN_DONT_VECTORIZE
# endif
#endif // __GCCXML__
 
// Local include(s):
#include "xAODAssociations/TrackParticleClusterAssociationContainer.h"
#include "xAODAssociations/TrackParticleClusterAssociationAuxContainer.h"

// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"

namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODASSOCIATIONS {
      // Classes in this package
     xAOD::TrackParticleClusterAssociation                                                        o1;
     xAOD::TrackParticleClusterAssociationContainer                                               c1;
      // Links for this package
      DataLink< xAOD::TrackParticleClusterAssociationContainer >                                   l1;
      ElementLink< xAOD::TrackParticleClusterAssociationContainer >                                l2;
      std::vector< DataLink< xAOD::TrackParticleClusterAssociationContainer > >                    l3;
      std::vector< ElementLink< xAOD::TrackParticleClusterAssociationContainer > >                 l4;
      std::vector< std::vector< ElementLink< xAOD::TrackParticleClusterAssociationContainer > > >  l5;
      // Instantiations of links used by this package
      ElementLink< xAOD::TrackParticleContainer >                                                  i1;
      ElementLink< xAOD::CaloClusterContainer >                                                    i2;
      std::vector< ElementLink< xAOD::CaloClusterContainer > >                                     i3;
   };
}

#endif // XAODASSOCIATIONS_XAODASSOCIATIONSDICT_H
