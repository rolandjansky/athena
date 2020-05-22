// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODTRIGMUON_XAODTRIGMUONDICT_H
#define XAODTRIGMUON_XAODTRIGMUONDICT_H

// Local include(s).
#include "xAODTrigMuon/L2StandAloneMuon.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "xAODTrigMuon/L2StandAloneMuonAuxContainer.h"
#include "xAODTrigMuon/versions/L2StandAloneMuon_v1.h"
#include "xAODTrigMuon/versions/L2StandAloneMuonContainer_v1.h"
#include "xAODTrigMuon/versions/L2StandAloneMuonAuxContainer_v1.h"
#include "xAODTrigMuon/versions/L2StandAloneMuon_v2.h"
#include "xAODTrigMuon/versions/L2StandAloneMuonContainer_v2.h"
#include "xAODTrigMuon/versions/L2StandAloneMuonAuxContainer_v2.h"

#include "xAODTrigMuon/L2IsoMuon.h"
#include "xAODTrigMuon/L2IsoMuonContainer.h"
#include "xAODTrigMuon/L2IsoMuonAuxContainer.h"
#include "xAODTrigMuon/versions/L2IsoMuon_v1.h"
#include "xAODTrigMuon/versions/L2IsoMuonContainer_v1.h"
#include "xAODTrigMuon/versions/L2IsoMuonAuxContainer_v1.h"

#include "xAODTrigMuon/L2CombinedMuon.h"
#include "xAODTrigMuon/L2CombinedMuonContainer.h"
#include "xAODTrigMuon/L2CombinedMuonAuxContainer.h"
#include "xAODTrigMuon/versions/L2CombinedMuon_v1.h"
#include "xAODTrigMuon/versions/L2CombinedMuonContainer_v1.h"
#include "xAODTrigMuon/versions/L2CombinedMuonAuxContainer_v1.h"

// EDM include(s).
#include "xAODCore/tools/DictHelpers.h"

// Instantiate all necessary types for the dictionary.
namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODTRIGMUON {
      // Local type(s).
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, L2StandAloneMuonContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, L2StandAloneMuonContainer_v2 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, L2IsoMuonContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, L2CombinedMuonContainer_v1 );
      // Type(s) needed for the dictionary generation to succeed.
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TrackParticleContainer );
   };
}

#endif // XAODTRIGMUON_XAODTRIGMUONDICT_H
