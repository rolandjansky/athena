// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrackingDict.h 573808 2013-12-04 14:09:11Z salzburg $
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
#include "AthLinks/ElementLinkVector.h"
 
// Local include(s):
#include "xAODTracking/versions/TrackParticleContainer_v1.h"
#include "xAODTracking/versions/TrackParticleAuxContainer_v1.h"
#include "xAODTracking/versions/NeutralParticleContainer_v1.h"
#include "xAODTracking/versions/NeutralParticleAuxContainer_v1.h"
#include "xAODTracking/versions/Vertex_v1.h"
#include "xAODTracking/versions/VertexContainer_v1.h"
#include "xAODTracking/versions/VertexAuxContainer_v1.h"
#include "xAODTracking/versions/PrepRawDataContainer_v1.h"
#include "xAODTracking/versions/PrepRawDataAuxContainer_v1.h"
#include "xAODTracking/versions/MeasurementStateOnSurfaceContainer_v1.h"
#include "xAODTracking/versions/MeasurementStateOnSurfaceAuxContainer_v1.h"
#include "xAODTracking/TrackingPrimitives.h"

namespace {
  struct GCCXML_DUMMY_INSTANTIATION_XAODTRACKING {
     xAOD::TrackParticleContainer_v1                                              c1;
     DataLink< xAOD::TrackParticleContainer_v1 >                                  l1;
     ElementLink< xAOD::TrackParticleContainer_v1 >                               l2;
     ElementLinkVector< xAOD::TrackParticleContainer_v1 >                         l3;
     std::vector< DataLink< xAOD::TrackParticleContainer_v1 > >                   l4;
     std::vector< ElementLink< xAOD::TrackParticleContainer_v1 > >                l5;
     std::vector< ElementLinkVector< xAOD::TrackParticleContainer_v1 > >          l6;
     std::vector< std::vector< ElementLink< xAOD::TrackParticleContainer_v1 > > > l7;

     xAOD::NeutralParticleContainer_v1                                              nc1;
     DataLink< xAOD::NeutralParticleContainer_v1 >                                  nl1;
     ElementLink< xAOD::NeutralParticleContainer_v1 >                               nl2;
     ElementLinkVector< xAOD::NeutralParticleContainer_v1 >                         nl3;
     std::vector< DataLink< xAOD::NeutralParticleContainer_v1 > >                   nl4;
     std::vector< ElementLink< xAOD::NeutralParticleContainer_v1 > >                nl5;
     std::vector< ElementLinkVector< xAOD::NeutralParticleContainer_v1 > >          nl6;
     std::vector< std::vector< ElementLink< xAOD::NeutralParticleContainer_v1 > > > nl7;
     
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
      ElementLink< xAOD::VertexContainer_v1 >                                     el1;
      std::vector< ElementLink< xAOD::VertexContainer_v1 > >                      el2;
      std::vector< std::vector< ElementLink< xAOD::VertexContainer_v1 > > >       el3;

      // Container(s):
      xAOD::PrepRawDataContainer_v1                                                    c4;
      // Data link(s):
      DataLink< xAOD::PrepRawDataContainer_v1 >                                        pdl1;
      std::vector< DataLink< xAOD::PrepRawDataContainer_v1 > >                         pdl2;
      // Element link(s):
      ElementLink< xAOD::PrepRawDataContainer_v1 >                                     pel1;
      std::vector< ElementLink< xAOD::PrepRawDataContainer_v1 > >                      pel2;
      std::vector< std::vector< ElementLink< xAOD::PrepRawDataContainer_v1 > > >       pel3;

      // Container(s):
      xAOD::MeasurementStateOnSurfaceContainer_v1                                                    c5;
      // Data link(s):
      DataLink< xAOD::MeasurementStateOnSurfaceContainer_v1 >                                        mdl1;
      std::vector< DataLink< xAOD::MeasurementStateOnSurfaceContainer_v1 > >                         mdl2;
      // Element link(s):
      ElementLink< xAOD::MeasurementStateOnSurfaceContainer_v1 >                                     mel1;
      std::vector< ElementLink< xAOD::MeasurementStateOnSurfaceContainer_v1 > >                      mel2;
      std::vector< std::vector< ElementLink< xAOD::MeasurementStateOnSurfaceContainer_v1 > > >       mel3;



  };
}

#endif // XAODTRACKPARTICLE_XAODTRACKPARTICLEDICT_H
