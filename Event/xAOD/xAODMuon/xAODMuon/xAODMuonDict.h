// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODMuonDict.h 745098 2016-05-05 15:47:04Z wleight $
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
#include "xAODMuon/versions/MuonAuxContainer_v2.h"
#include "xAODMuon/versions/MuonAuxContainer_v3.h"
#include "xAODMuon/versions/MuonAuxContainer_v4.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODMuon/MuonSegmentAuxContainer.h"
#include "xAODMuon/SlowMuonContainer.h"
#include "xAODMuon/SlowMuonAuxContainer.h"

// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"

#ifndef XAOD_ANALYSIS
#ifndef GENERATIONBASE
#include "TrkSegment/SegmentCollection.h"
#endif //GENERATIONBASE
#endif // not XAOD_ANALYSIS

namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODMUON {
      // Classes in this package
     xAOD::MuonContainer                                                       c1;
      xAOD::MuonSegmentContainer                                               c2;
      xAOD::SlowMuonContainer                                                  c4;
      // Links for Muon
      DataLink< xAOD::MuonContainer >                                          l1;
      ElementLink< xAOD::MuonContainer >                                       l2;
      std::vector< DataLink< xAOD::MuonContainer > >                           l3;
      std::vector< ElementLink< xAOD::MuonContainer > >                        l4;
      std::vector< std::vector< ElementLink< xAOD::MuonContainer > > >         l5;
      // Segments
      DataLink< xAOD::MuonSegmentContainer >                                      l6;
      ElementLink< xAOD::MuonSegmentContainer >                                   l7;
      std::vector< DataLink< xAOD::MuonSegmentContainer > >                       l8;
      std::vector< ElementLink< xAOD::MuonSegmentContainer > >                    l9;
      std::vector< std::vector< ElementLink< xAOD::MuonSegmentContainer > > >    l10;
      // Slow Muons
      DataLink< xAOD::SlowMuonContainer >                                        l11;
      ElementLink< xAOD::SlowMuonContainer >                                     l12;
      std::vector< DataLink< xAOD::SlowMuonContainer > >                         l13;
      std::vector< ElementLink< xAOD::SlowMuonContainer > >                      l14;
      std::vector< std::vector< ElementLink< xAOD::SlowMuonContainer > > >       l15;
      // Instantiations of links used by this package
      ElementLink< xAOD::CaloClusterContainer >                                   i1;
      ElementLink< xAOD::TrackParticleContainer >                                 i2;
#ifndef XAOD_ANALYSIS
#ifndef GENERATIONBASE
      // These lines are still needed in order for Reflex to see the
      // member variable of xAOD::MuonSegmentAuxContainer_v1 correctly.
      Trk::SegmentCollection                                                      c3;
      ElementLink< Trk::SegmentCollection >                                       i3;
      std::vector<ElementLink< Trk::SegmentCollection > >                         i4;
#endif // not GENERATIONBASE
#endif // not XAOD_ANALYSIS
   };
}

#endif // XAODMUON_XAODMUONDICT_H
