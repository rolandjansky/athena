// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODMUON_XAODMUONDICT_H
#define XAODMUON_XAODMUONDICT_H

// Local include(s).
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "xAODMuon/versions/Muon_v1.h"
#include "xAODMuon/versions/MuonContainer_v1.h"
#include "xAODMuon/versions/MuonAuxContainer_v1.h"
#include "xAODMuon/versions/MuonAuxContainer_v2.h"
#include "xAODMuon/versions/MuonAuxContainer_v3.h"
#include "xAODMuon/versions/MuonAuxContainer_v4.h"
#include "xAODMuon/versions/MuonAuxContainer_v5.h"

#include "xAODMuon/MuonSegment.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODMuon/MuonSegmentAuxContainer.h"
#include "xAODMuon/versions/MuonSegment_v1.h"
#include "xAODMuon/versions/MuonSegmentContainer_v1.h"
#include "xAODMuon/versions/MuonSegmentAuxContainer_v1.h"

#include "xAODMuon/SlowMuon.h"
#include "xAODMuon/SlowMuonContainer.h"
#include "xAODMuon/SlowMuonAuxContainer.h"
#include "xAODMuon/versions/SlowMuon_v1.h"
#include "xAODMuon/versions/SlowMuonContainer_v1.h"
#include "xAODMuon/versions/SlowMuonAuxContainer_v1.h"

// EDM include(s):
#include "xAODCore/tools/DictHelpers.h"

// Instantiate all necessary types for the dictionary.
namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODMUON {
      // Local type(s).
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, MuonContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, MuonSegmentContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, SlowMuonContainer_v1 );
      // Type(s) needed for the dictionary generation to succeed.
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, CaloClusterContainer );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TrackParticleContainer );
#if !(defined(GENERATIONBASE) || defined(XAOD_ANALYSIS))
      // These lines are still needed in order for Reflex to see the
      // member variable of xAOD::MuonSegmentAuxContainer_v1 correctly.
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( Trk, SegmentCollection );
#endif // not (defined(GENERATIONBASE) || defined(XAOD_ANALYSIS))
   };
}

#endif // XAODMUON_XAODMUONDICT_H
