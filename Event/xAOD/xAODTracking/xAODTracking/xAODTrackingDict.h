// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_XAODTRACKINGDICT_H
#define XAODTRACKING_XAODTRACKINGDICT_H

// Needed to successfully generate the dictionary in standalone mode:
#if defined(__GCCXML__) and not defined(EIGEN_DONT_VECTORIZE)
#   define EIGEN_DONT_VECTORIZE
#endif // __GCCXML__

// System include(s):
#include <bitset>
 
// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"
 
// Local include(s):
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/versions/TrackParticleContainer_v1.h"
#include "xAODTracking/versions/TrackParticleAuxContainer_v1.h"
#include "xAODTracking/versions/TrackParticleAuxContainer_v2.h"
#include "xAODTracking/versions/TrackParticleAuxContainer_v3.h"
#include "xAODTracking/versions/TrackParticleAuxContainer_v4.h"
#include "xAODTracking/NeutralParticleContainer.h"
#include "xAODTracking/versions/NeutralParticleContainer_v1.h"
#include "xAODTracking/versions/NeutralParticleAuxContainer_v1.h"
#include "xAODTracking/versions/Vertex_v1.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/versions/VertexContainer_v1.h"
#include "xAODTracking/versions/VertexContainer_v1.h"
#include "xAODTracking/versions/VertexAuxContainer_v1.h"
#include "xAODTracking/TrackMeasurementValidationContainer.h"
#include "xAODTracking/versions/TrackMeasurementValidationContainer_v1.h"
#include "xAODTracking/versions/TrackMeasurementValidationAuxContainer_v1.h"
#include "xAODTracking/TrackStateValidationContainer.h"
#include "xAODTracking/versions/TrackStateValidationContainer_v1.h"
#include "xAODTracking/versions/TrackStateValidationAuxContainer_v1.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"

#include "xAODTracking/SCTRawHitValidation.h"
#include "xAODTracking/SCTRawHitValidationContainer.h"
#include "xAODTracking/SCTRawHitValidationAuxContainer.h"
#include "xAODTracking/versions/SCTRawHitValidation_v1.h"
#include "xAODTracking/versions/SCTRawHitValidationContainer_v1.h"
#include "xAODTracking/versions/SCTRawHitValidationAuxContainer_v1.h"

namespace {
  struct GCCXML_DUMMY_INSTANTIATION_XAODTRACKING {
     xAOD::TrackParticleContainer_v1                                              c1;
     DataLink< xAOD::TrackParticleContainer_v1 >                                  l1;
     std::vector< DataLink< xAOD::TrackParticleContainer_v1 > >                   l2;
     ElementLink< xAOD::TrackParticleContainer >                                  l3;
     std::vector< ElementLink< xAOD::TrackParticleContainer_v1 > >                l4;
     std::vector< std::vector< ElementLink< xAOD::TrackParticleContainer_v1 > > > l5;

     xAOD::NeutralParticleContainer_v1                                              nc1;
     DataLink< xAOD::NeutralParticleContainer_v1 >                                  nl1;
     std::vector< DataLink< xAOD::NeutralParticleContainer_v1 > >                   nl2;
     ElementLink< xAOD::NeutralParticleContainer >                                  nl3;
     std::vector< ElementLink< xAOD::NeutralParticleContainer_v1 > >                nl4;
     std::vector< std::vector< ElementLink< xAOD::NeutralParticleContainer_v1 > > > nl5;
     
#if ( ! defined(XAOD_STANDALONE) ) && ( ! defined(XAOD_MANACORE) )
     std::bitset< 11 >                                                            dummy1;
     TrackCollection                                                              c2;
     ElementLink<TrackCollection>                                                 l8;
#endif // not XAOD_STANDALONE and not XAOD_MANACORE

      // Container(s):
      xAOD::VertexContainer_v1                                                    c3;
      // Data link(s):
      DataLink< xAOD::VertexContainer_v1 >                                        dl1;
      std::vector< DataLink< xAOD::VertexContainer_v1 > >                         dl2;
      // Element link(s):
      ElementLink< xAOD::VertexContainer >                                        el1;
      std::vector< ElementLink< xAOD::VertexContainer_v1 > >                      el2;
      std::vector< std::vector< ElementLink< xAOD::VertexContainer_v1 > > >       el3;

      // Container(s):
      xAOD::TrackMeasurementValidationContainer_v1                                                    c4;
      // Data link(s):
      DataLink< xAOD::TrackMeasurementValidationContainer_v1 >                                        pdl1;
      std::vector< DataLink< xAOD::TrackMeasurementValidationContainer_v1 > >                         pdl2;
      // Element link(s):
      ElementLink< xAOD::TrackMeasurementValidationContainer >                                     pel1;
      std::vector< ElementLink< xAOD::TrackMeasurementValidationContainer_v1 > >                   pel2;
      std::vector< std::vector< ElementLink< xAOD::TrackMeasurementValidationContainer_v1 > > >    pel3;

      // Container(s):
      xAOD::TrackStateValidationContainer_v1                                                    c5;
      // Data link(s):
      DataLink< xAOD::TrackStateValidationContainer_v1 >                                        mdl1;
      std::vector< DataLink< xAOD::TrackStateValidationContainer_v1 > >                         mdl2;
      // Element link(s):
      ElementLink< xAOD::TrackStateValidationContainer >                                     mel1;
      std::vector< ElementLink< xAOD::TrackStateValidationContainer_v1 > >                      mel2;
      std::vector< std::vector< ElementLink< xAOD::TrackStateValidationContainer_v1 > > >       mel3;

      std::vector<std::vector<std::vector<int> > > vecDummyIntX;
      std::vector<std::vector<std::vector<float> > > vecDummyFltX;
      std::vector<std::vector<unsigned long> > vecDummyULX;

      // Container(s):
      xAOD::SCTRawHitValidationContainer_v1                                                    c6;
      // Data link(s):
      DataLink< xAOD::SCTRawHitValidationContainer_v1 >                                        rdodl1;
      std::vector< DataLink< xAOD::SCTRawHitValidationContainer_v1 > >                         rdodl2;
      // Element link(s):
      ElementLink< xAOD::SCTRawHitValidationContainer >                                        rdoel1;
      std::vector< ElementLink< xAOD::SCTRawHitValidationContainer_v1 > >                      rdoel2;
      std::vector< std::vector< ElementLink< xAOD::SCTRawHitValidationContainer_v1 > > >       rdoel3;

  };
}

#endif // XAODTRACKPARTICLE_XAODTRACKPARTICLEDICT_H
