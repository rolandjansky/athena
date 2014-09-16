// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODMuonDict.h 612402 2014-08-19 07:28:08Z htorres $
#ifndef XAODMUON_XAODMUONDICT_H
#define XAODMUON_XAODMUONDICT_H

// Needed to successfully generate the dictionary in standalone mode:
#if defined(__GCCXML__) and not defined(EIGEN_DONT_VECTORIZE)
# ifndef EIGEN_DONT_VECTORIZE
#   define EIGEN_DONT_VECTORIZE
# endif
#endif // __GCCXML__
 
// Local include(s):
#include "xAODMuon/versions/MuonContainer_v1.h"
#include "xAODMuon/versions/MuonAuxContainer_v1.h"
#include "xAODMuon/versions/MuonSegmentContainer_v1.h"
#include "xAODMuon/versions/MuonSegmentAuxContainer_v1.h"

// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"

#ifndef XAOD_ANALYSIS
#include "TrkSegment/SegmentCollection.h"
#endif // not XAOD_ANALYSIS

namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODMUON {
      // Classes in this package
      xAOD::MuonContainer_v1                                              c1;
      xAOD::MuonSegmentContainer_v1                                       c2;
      // Links for this package
      DataLink< xAOD::MuonContainer_v1 >                                  l1;
      ElementLink< xAOD::MuonContainer_v1 >                               l2;
      std::vector< DataLink< xAOD::MuonContainer_v1 > >                   l3;
      std::vector< ElementLink< xAOD::MuonContainer_v1 > >                l4;
      std::vector< std::vector< ElementLink< xAOD::MuonContainer_v1 > > > l5;
      DataLink< xAOD::MuonSegmentContainer_v1 >                           l6;
      ElementLink< xAOD::MuonSegmentContainer_v1 >                        l7;
      std::vector< DataLink< xAOD::MuonSegmentContainer_v1 > >            l8;
      std::vector< ElementLink< xAOD::MuonSegmentContainer_v1 > >         l9;
      std::vector< std::vector< ElementLink< xAOD::MuonSegmentContainer_v1 > > > l10;
      // Instantiations of links used by this package
      ElementLink< xAOD::CaloClusterContainer >                           i1;
      ElementLink< xAOD::TrackParticleContainer >                         i2;
#ifndef XAOD_ANALYSIS
      // These lines are still needed in order for Reflex to see the
      // member variable of xAOD::MuonSegmentAuxContainer_v1 correctly.
      Trk::SegmentCollection                                              c3;
      ElementLink< Trk::SegmentCollection >                               i3;
      std::vector<ElementLink< Trk::SegmentCollection > >                 i4;
#endif // not XAOD_ANALYSIS
   };
}

#endif // XAODMUON_XAODMUONDICT_H
