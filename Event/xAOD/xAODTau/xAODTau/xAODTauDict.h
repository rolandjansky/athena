// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTauDict.h 632166 2014-11-29 15:06:03Z krasznaa $
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
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"

/// Declare a dummy CLID for xAOD::TauJetContainer_v1. This is only necessary
/// to get DataLink<xAOD::TauJetContainer_v1> and
/// ElementLink<xAOD::TauJetContainer_v1> compiling in the dictionary. The CLID
/// is not needed in the "real" code, since users are never exposed to the _v1
/// classes in Athena anymore.
CLASS_DEF( xAOD::TauJetContainer_v1, 12345678, 10 )

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
   };
}

#endif // XAODTAU_XAODTAUDICT_H
