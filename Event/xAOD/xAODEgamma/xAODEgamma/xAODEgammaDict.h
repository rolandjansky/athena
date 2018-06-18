// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODEgammaDict.h 789134 2016-12-11 02:15:53Z christos $
#ifndef XAODEGAMMA_XAODEGAMMADICT_H
#define XAODEGAMMA_XAODEGAMMADICT_H

// Needed to successfully generate the dictionary in standalone mode:
#if defined(__GCCXML__) and not defined(EIGEN_DONT_VECTORIZE)
#   define EIGEN_DONT_VECTORIZE
#endif // __GCCXML__

// STL include(s):
#include <vector>

// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"

// Local include(s):
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/versions/EgammaContainer_v1.h"
#include "xAODEgamma/versions/EgammaAuxContainer_v1.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/versions/ElectronContainer_v1.h"
#include "xAODEgamma/versions/ElectronAuxContainer_v1.h"
#include "xAODEgamma/versions/ElectronAuxContainer_v2.h"
#include "xAODEgamma/versions/ElectronAuxContainer_v3.h"

#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/versions/PhotonContainer_v1.h"
#include "xAODEgamma/versions/PhotonAuxContainer_v1.h"
#include "xAODEgamma/versions/PhotonAuxContainer_v2.h"
#include "xAODEgamma/versions/PhotonAuxContainer_v3.h"

#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEgamma/ElectronxAODHelpers.h"
#include "xAODEgamma/PhotonxAODHelpers.h"
#include "xAODEgamma/EgammaTruthxAODHelpers.h"
#include "xAODEgamma/EgammaEnums.h"
#include "xAODEgamma/EgammaDefs.h"
#include "AthContainers/tools/AuxTypeVectorFactory.h"

namespace {
struct GCCXML_DUMMY_INSTANTIATION_XAODEGAMMA {
  xAOD::EgammaContainer_v1 eg_c1;
  DataLink< xAOD::EgammaContainer_v1 > eg_l1;
  ElementLink< xAOD::EgammaContainer_v1 > eg_l2;
  std::vector< DataLink< xAOD::EgammaContainer_v1 > > eg_l3;
  std::vector< ElementLink< xAOD::EgammaContainer_v1 > > eg_l4;
  std::vector< std::vector< ElementLink<  xAOD::EgammaContainer_v1 > > > eg_l5;
  SG::AuxTypeVectorFactory<ElementLink< xAOD::EgammaContainer_v1 > > eg_l6;

  xAOD::ElectronContainer_v1 el_c1;
  DataLink< xAOD::ElectronContainer_v1 > el_l1;
  ElementLink< xAOD::ElectronContainer_v1 > el_l2;
  std::vector< ElementLink<  xAOD::ElectronContainer_v1 > > el_l3;
  std::vector< DataLink< xAOD::ElectronContainer_v1 > > el_l4;
  std::vector< std::vector< ElementLink<  xAOD::ElectronContainer_v1 > > > el_l5;

  xAOD::PhotonContainer_v1 ph_c1;
  DataLink< xAOD::PhotonContainer_v1 > ph_l1;
  ElementLink< xAOD::PhotonContainer_v1 > ph_l2;
  std::vector< ElementLink< xAOD::PhotonContainer_v1 > > ph_l3;
  std::vector< DataLink< xAOD::PhotonContainer_v1 > > ph_l4;
  std::vector< std::vector< ElementLink < xAOD::PhotonContainer_v1 > > > ph_l5;

  // Instantiate the classes used by xAOD::Electron, xAODPhoton so that
  // Reflex would see them with their "correct type". Note that the
  // dictionary for these types comes from other places. This instantiation
  // is just needed for "Reflex related technical reasons"...OA
  ElementLink< xAOD::TrackParticleContainer > auxlink1;
  std::vector< ElementLink< xAOD::TrackParticleContainer > > auxlink2;
  ElementLink< xAOD::CaloClusterContainer > auxlink3;
  std::vector< ElementLink< xAOD::CaloClusterContainer > > auxlink4;
  ElementLink< xAOD::VertexContainer > auxlink5;
  std::vector< ElementLink< xAOD::VertexContainer > > auxlink6;
     
  std::set<const xAOD::TrackParticle*> setTP;

};
}

#endif // XAODEGAMMA_XAODEGAMMADICT_H
