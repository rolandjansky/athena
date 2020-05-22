// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODTAU_XAODTAUDICT_H
#define XAODTAU_XAODTAUDICT_H

// Local include(s).
#include "xAODTau/TauJet.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTau/versions/TauJet_v1.h"
#include "xAODTau/versions/TauJetContainer_v1.h"
#include "xAODTau/versions/TauJetAuxContainer_v1.h"
#include "xAODTau/versions/TauJet_v2.h"
#include "xAODTau/versions/TauJetContainer_v2.h"
#include "xAODTau/versions/TauJetAuxContainer_v2.h"
#include "xAODTau/versions/TauJet_v3.h"
#include "xAODTau/versions/TauJetContainer_v3.h"
#include "xAODTau/versions/TauJetAuxContainer_v3.h"

#include "xAODTau/DiTauJet.h"
#include "xAODTau/DiTauJetContainer.h"
#include "xAODTau/DiTauJetAuxContainer.h"
#include "xAODTau/versions/DiTauJet_v1.h"
#include "xAODTau/versions/DiTauJetContainer_v1.h"
#include "xAODTau/versions/DiTauJetAuxContainer_v1.h"

#include "xAODTau/TauTrack.h"
#include "xAODTau/TauTrackContainer.h"
#include "xAODTau/TauTrackAuxContainer.h"
#include "xAODTau/versions/TauTrack_v1.h"
#include "xAODTau/versions/TauTrackContainer_v1.h"
#include "xAODTau/versions/TauTrackAuxContainer_v1.h"

#include "xAODTau/TauxAODHelpers.h"

// EDM include(s).
#include "xAODCore/tools/DictHelpers.h"

// Instantiate all necessary types for the dictionary.
namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODTAU {
      // Local type(s).
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TauJetContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TauJetContainer_v2 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TauJetContainer_v3 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, DiTauJetContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TauTrackContainer_v1 );
      // Type(s) needed for the dictionary generation to succeed.
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TrackParticleContainer );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, VertexContainer );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, JetContainer );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, PFOContainer );
   };
}

#endif // XAODTAU_XAODTAUDICT_H
