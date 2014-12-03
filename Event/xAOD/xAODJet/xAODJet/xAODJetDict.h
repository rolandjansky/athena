// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODJetDict.h 631914 2014-11-28 16:08:50Z krasznaa $
#ifndef XAODJET_XAODJETDICT_H
#define XAODJET_XAODJETDICT_H

// Needed to successfully generate the dictionary in standalone mode:
#if defined(__GCCXML__) and not defined(EIGEN_DONT_VECTORIZE)
#   define EIGEN_DONT_VECTORIZE
#endif // __GCCXML__

// System include(s):
#include <vector>

// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"
#include "AthLinks/ElementLinkVector.h"

// Local include(s):
#include "xAODJet/JetContainer.h"
#include "xAODJet/versions/JetContainer_v1.h"
#include "xAODJet/versions/JetAuxContainer_v1.h"
#include "xAODJet/versions/JetTrigAuxContainer_v1.h"
#include "xAODJet/JetTypes.h"

namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODJET {
      // Instantiate the container for gccxml:
      xAOD::JetContainer_v1 c1;

      // Smart pointers to Jet_v1:
      DataLink< xAOD::JetContainer_v1 > e1;
      std::vector< DataLink< xAOD::JetContainer_v1 > > e2;
      ElementLink< xAOD::JetContainer_v1 > e3;
      std::vector< ElementLink< xAOD::JetContainer_v1 > > e4;
      std::vector< std::vector< ElementLink< xAOD::JetContainer_v1 > > > e5;
      ElementLinkVector< xAOD::JetContainer_v1 > e6;
      std::vector< ElementLinkVector< xAOD::JetContainer_v1 > > e7;

      // Instantiations to create the EL member dictionaries correctly:
      xAOD::IParticleContainer pc1;
      ElementLink<xAOD::IParticleContainer> e10;

     std::pair<std::string,std::vector<const xAOD::IParticle*> > bug1;
     std::pair<std::string,std::vector<float> > bug2;
     std::pair<std::string,std::vector<std::vector<int> > > bug3;
     std::pair<std::string,std::vector<std::vector<float> > > bug4;

     // momentums     
     //xAOD::JetFourMom_t jet4mom;
     std::vector<xAOD::JetFourMom_t> vjet4mom;
   };
}

#endif // XAODJET_XAODJETDICT_H
