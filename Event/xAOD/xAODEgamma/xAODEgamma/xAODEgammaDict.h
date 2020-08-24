// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODEGAMMA_XAODEGAMMADICT_H
#define XAODEGAMMA_XAODEGAMMADICT_H

// Local include(s):
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/EgammaAuxContainer.h"
#include "xAODEgamma/versions/Egamma_v1.h"
#include "xAODEgamma/versions/EgammaContainer_v1.h"
#include "xAODEgamma/versions/EgammaAuxContainer_v1.h"

#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODEgamma/versions/Electron_v1.h"
#include "xAODEgamma/versions/ElectronContainer_v1.h"
#include "xAODEgamma/versions/ElectronAuxContainer_v1.h"
#include "xAODEgamma/versions/ElectronAuxContainer_v2.h"
#include "xAODEgamma/versions/ElectronAuxContainer_v3.h"

#include "xAODEgamma/Photon.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"
#include "xAODEgamma/versions/Photon_v1.h"
#include "xAODEgamma/versions/PhotonContainer_v1.h"
#include "xAODEgamma/versions/PhotonAuxContainer_v1.h"
#include "xAODEgamma/versions/PhotonAuxContainer_v2.h"
#include "xAODEgamma/versions/PhotonAuxContainer_v3.h"

#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEgamma/ElectronxAODHelpers.h"
#include "xAODEgamma/PhotonxAODHelpers.h"
#include "xAODEgamma/EgammaTruthxAODHelpers.h"

// EDM include(s).
#include "xAODCore/tools/DictHelpers.h"

// Instantiate all necessary types for the dictionary.
namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODEGAMMA {
      // Local type(s).
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, EgammaContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, ElectronContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, PhotonContainer_v1 );
      // Type(s) needed for the dictionary generation to succeed.
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TrackParticleContainer );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, CaloClusterContainer );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, VertexContainer );
      // Type returned by helper function(s).
      std::set< const xAOD::TrackParticle* > setTP;
   };
}

#endif // XAODEGAMMA_XAODEGAMMADICT_H
