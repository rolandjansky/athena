// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTruthDict.h 670153 2015-05-27 11:42:29Z tbisanz $
#ifndef XAODTRUTH_XAODTRUTHDICT_H
#define XAODTRUTH_XAODTRUTHDICT_H

// System include(s):
#include <vector>

// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"

// Local include(s):
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODTruth/TruthVertexContainer.h"
#include "xAODTruth/TruthVertexAuxContainer.h"
#include "xAODTruth/TruthEventBaseContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthEventAuxContainer.h"
#include "xAODTruth/TruthPileupEventContainer.h"
#include "xAODTruth/TruthPileupEventAuxContainer.h"
#include "xAODTruth/TruthMetaDataContainer.h"
#include "xAODTruth/TruthMetaDataAuxContainer.h"
#include "xAODTruth/xAODTruthHelpers.h"

namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODTRUTH {
      // The DataVector types:
      xAOD::TruthParticleContainer    c1;
      xAOD::TruthVertexContainer      c2;
      xAOD::TruthEventBaseContainer   c3;
      xAOD::TruthEventContainer       c4;
      xAOD::TruthPileupEventContainer c5;
      xAOD::TruthMetaDataContainer    c6;

      // The smart pointer types:
      DataLink< xAOD::TruthParticleContainer > dl1;
      std::vector< DataLink< xAOD::TruthParticleContainer > > dl2;
      DataLink< xAOD::TruthVertexContainer > dl3;
      std::vector< DataLink< xAOD::TruthVertexContainer > > dl4;
      ElementLink< xAOD::TruthParticleContainer > el1;
      std::vector< ElementLink< xAOD::TruthParticleContainer > > el2;
      std::vector< std::vector< ElementLink< xAOD::TruthParticleContainer > > > el3;
      ElementLink< xAOD::TruthVertexContainer > el4;
      std::vector< ElementLink< xAOD::TruthVertexContainer > > el5;
      std::vector< std::vector< ElementLink< xAOD::TruthVertexContainer > > > el6;
   };
}

#endif // XAODTRUTH_XAODTRUTHDICT_H
