// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTauDict.h 796007 2017-02-07 15:38:04Z griffith $
#ifndef XAODTAU_XAODTAUDICT_H
#define XAODTAU_XAODTAUDICT_H

// Needed to successfully generate the dictionary in standalone mode:
#if defined(__GCCXML__) and not defined(EIGEN_DONT_VECTORIZE)
#   define EIGEN_DONT_VECTORIZE
#endif // __GCCXML__

// STL include(s):
#include <vector>

// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"
#include "AthLinks/ElementLinkVector.h"

// Local include(s):
#include "xAODTau/versions/TauJetContainer_v1.h"
#include "xAODTau/versions/TauJetAuxContainer_v1.h"
#include "xAODTau/versions/TauJetContainer_v2.h"
#include "xAODTau/versions/TauJetAuxContainer_v2.h"
#include "xAODTau/versions/TauJetContainer_v3.h"
#include "xAODTau/versions/TauJetAuxContainer_v3.h"
#include "xAODTau/versions/DiTauJetContainer_v1.h"
#include "xAODTau/versions/DiTauJetAuxContainer_v1.h"
#include "xAODTau/versions/TauTrackContainer_v1.h"
#include "xAODTau/versions/TauTrackAuxContainer_v1.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTau/DiTauJetContainer.h"
#include "xAODTau/DiTauJetAuxContainer.h"
#include "xAODTau/TauTrackContainer.h"
#include "xAODTau/TauTrackAuxContainer.h"


/// Declare a dummy CLID for xAOD::TauJetContainer_v1. This is only necessary
/// to get DataLink<xAOD::TauJetContainer_v1> and
/// ElementLink<xAOD::TauJetContainer_v1> compiling in the dictionary. The CLID
/// is not needed in the "real" code, since users are never exposed to the _v1
/// classes in Athena anymore.
CLASS_DEF( xAOD::TauJetContainer_v1, 12345678, 10 )
CLASS_DEF( xAOD::TauJetContainer_v2, 123456789, 10 )

namespace {
  struct GCCXML_DUMMY_INSTANTIATION_XAODTAU {
    xAOD::TauJetContainer_v1 c1;
    DataLink< xAOD::TauJetContainer_v1 > l1;
    ElementLink< xAOD::TauJetContainer_v1 > l2;
    ElementLinkVector< xAOD::TauJetContainer_v1 > l3;
    std::vector< DataLink< xAOD::TauJetContainer_v1 > > l4;
    std::vector< ElementLink< xAOD::TauJetContainer_v1 > > l5;
    std::vector< std::vector< ElementLink< xAOD::TauJetContainer_v1 > > > l6;
    std::vector< ElementLinkVector< xAOD::TauJetContainer_v1 > > l7;

    xAOD::TauJetContainer_v2 c2;
    DataLink< xAOD::TauJetContainer_v2 > l8;
    ElementLink< xAOD::TauJetContainer_v2 > l9;
    ElementLinkVector< xAOD::TauJetContainer_v2 > l10;
    std::vector< DataLink< xAOD::TauJetContainer_v2 > > l11;
    std::vector< ElementLink< xAOD::TauJetContainer_v2 > > l12;
    std::vector< std::vector< ElementLink< xAOD::TauJetContainer_v2 > > > l13;
    std::vector< ElementLinkVector< xAOD::TauJetContainer_v2 > > l14;

    xAOD::DiTauJetContainer_v1 c3;
    DataLink< xAOD::DiTauJetContainer_v1 > l15;
    ElementLink< xAOD::DiTauJetContainer_v1 > l16;
    ElementLinkVector< xAOD::DiTauJetContainer_v1 > l17;
    std::vector< DataLink< xAOD::DiTauJetContainer_v1 > > l18;
    std::vector< ElementLink< xAOD::DiTauJetContainer_v1 > > l19;
    std::vector< std::vector< ElementLink< xAOD::DiTauJetContainer_v1 > > > l20;
    std::vector< ElementLinkVector< xAOD::DiTauJetContainer_v1 > > l21;

    xAOD::TauTrackContainer_v1 c4;
    DataLink< xAOD::TauTrackContainer_v1 > l22;
    ElementLink< xAOD::TauTrackContainer_v1 > l23;
    ElementLinkVector< xAOD::TauTrackContainer_v1 > l24;
    std::vector< DataLink< xAOD::TauTrackContainer_v1 > > l25;
    std::vector< ElementLink< xAOD::TauTrackContainer_v1 > > l26;
    std::vector< std::vector< ElementLink< xAOD::TauTrackContainer_v1 > > > l27;
    std::vector< ElementLinkVector< xAOD::TauTrackContainer_v1 > > l28;

    xAOD::TauJetContainer_v2 c5;
    DataLink< xAOD::TauJetContainer_v2 > l29;
    ElementLink< xAOD::TauJetContainer_v2 > l30;
    ElementLinkVector< xAOD::TauJetContainer_v2 > l31;
    std::vector< DataLink< xAOD::TauJetContainer_v2 > > l32;
    std::vector< ElementLink< xAOD::TauJetContainer_v2 > > l33;
    std::vector< std::vector< ElementLink< xAOD::TauJetContainer_v2 > > > l34;
    std::vector< ElementLinkVector< xAOD::TauJetContainer_v2 > > l35;

    xAOD::TauJetContainer_v3 c6;
    DataLink< xAOD::TauJetContainer_v3 > l36;
    ElementLink< xAOD::TauJetContainer_v3 > l37;
    ElementLinkVector< xAOD::TauJetContainer_v3 > l38;
    std::vector< DataLink< xAOD::TauJetContainer_v3 > > l39;
    std::vector< ElementLink< xAOD::TauJetContainer_v3 > > l40;
    std::vector< std::vector< ElementLink< xAOD::TauJetContainer_v3 > > > l41;
    std::vector< ElementLinkVector< xAOD::TauJetContainer_v3 > > l42;


    // Instantiate the classes used by xAOD::TauJetAuxContainer, so that
    // Reflex would see them with their "correct type". Note that the
    // dictionary for these types comes from other places. This instantiation
    // is just needed for "Reflex related technical reasons"...
    ElementLink< xAOD::TrackParticleContainer > auxlink1;
    std::vector< ElementLink< xAOD::TrackParticleContainer > > auxlink2;
    ElementLink< xAOD::JetContainer > auxlink3;
    std::vector< ElementLink< xAOD::JetContainer > > auxlink4;
    ElementLink< xAOD::PFOContainer > auxlink5;
    std::vector< ElementLink< xAOD::PFOContainer > > auxlink6;
    ElementLink< xAOD::VertexContainer > auxlink7;
    std::vector< ElementLink< xAOD::VertexContainer > > auxlink8;
    ElementLink< xAOD::TauTrackContainer > auxlink9;
    std::vector< ElementLink< xAOD::TauTrackContainer > > auxlink10;
   };
}

#endif // XAODTAU_XAODTAUDICT_H
