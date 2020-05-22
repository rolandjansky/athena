// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODTRIGEGAMMA_XAODTRIGEGAMMADICT_H
#define XAODTRIGEGAMMA_XAODTRIGEGAMMADICT_H

// Local include(s).
#include "xAODTrigEgamma/TrigPhoton.h"
#include "xAODTrigEgamma/TrigPhotonContainer.h"
#include "xAODTrigEgamma/TrigPhotonAuxContainer.h"
#include "xAODTrigEgamma/versions/TrigPhoton_v1.h"
#include "xAODTrigEgamma/versions/TrigPhotonContainer_v1.h"
#include "xAODTrigEgamma/versions/TrigPhotonAuxContainer_v1.h"

#include "xAODTrigEgamma/TrigElectron.h"
#include "xAODTrigEgamma/TrigElectronContainer.h"
#include "xAODTrigEgamma/TrigElectronAuxContainer.h"
#include "xAODTrigEgamma/versions/TrigElectron_v1.h"
#include "xAODTrigEgamma/versions/TrigElectronContainer_v1.h"
#include "xAODTrigEgamma/versions/TrigElectronAuxContainer_v1.h"

#include "xAODTrigEgamma/ElectronTrigAuxContainer.h"
#include "xAODTrigEgamma/PhotonTrigAuxContainer.h"
#include "xAODTrigEgamma/versions/ElectronTrigAuxContainer_v1.h"
#include "xAODTrigEgamma/versions/PhotonTrigAuxContainer_v1.h"

// EDM include(s).
#include "xAODCore/tools/DictHelpers.h"

// Instantiate all necessary types for the dictionary.
namespace{
   struct GCCXML_DUMMY_INSTANTIATION_XAODTRIGEGAMMA {
      // Local type(s).
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TrigElectronContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TrigPhotonContainer_v1 );
      // Type(s) needed for the dictionary generation to succeed.
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TrigEMClusterContainer );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TrackParticleContainer );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, VertexContainer );
   };
}

#endif // XAODTRIGEGAMMA_XAODTRIGEGAMMADICT_H
