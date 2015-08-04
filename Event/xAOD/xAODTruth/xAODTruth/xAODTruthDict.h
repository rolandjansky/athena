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
#include "xAODTruth/versions/TruthParticleContainer_v1.h"
#include "xAODTruth/versions/TruthParticleAuxContainer_v1.h"
#include "xAODTruth/versions/TruthVertexContainer_v1.h"
#include "xAODTruth/versions/TruthVertexAuxContainer_v1.h"
#include "xAODTruth/versions/TruthEventBaseContainer_v1.h"
#include "xAODTruth/versions/TruthEventContainer_v1.h"
#include "xAODTruth/versions/TruthEventAuxContainer_v1.h"
#include "xAODTruth/versions/TruthPileupEventContainer_v1.h"
#include "xAODTruth/versions/TruthPileupEventAuxContainer_v1.h"
#include "xAODTruth/versions/TruthMetaDataContainer_v1.h"
#include "xAODTruth/versions/TruthMetaDataAuxContainer_v1.h"
#include "xAODTruth/xAODTruthHelpers.h"

namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODTRUTH {
      // The DataVector types:
      xAOD::TruthParticleContainer_v1    c1;
      xAOD::TruthVertexContainer_v1      c2;
      xAOD::TruthEventBaseContainer_v1   c3;
      xAOD::TruthEventContainer_v1       c4;
      xAOD::TruthPileupEventContainer_v1 c5;
      xAOD::TruthMetaDataContainer_v1    c6;

      // The smart pointer types:
      DataLink< xAOD::TruthParticleContainer_v1 > dl1;
      std::vector< DataLink< xAOD::TruthParticleContainer_v1 > > dl2;
      DataLink< xAOD::TruthVertexContainer_v1 > dl3;
      std::vector< DataLink< xAOD::TruthVertexContainer_v1 > > dl4;
      ElementLink< xAOD::TruthParticleContainer_v1 > el1;
      std::vector< ElementLink< xAOD::TruthParticleContainer_v1 > > el2;
      std::vector< std::vector< ElementLink< xAOD::TruthParticleContainer_v1 > > > el3;
      ElementLink< xAOD::TruthVertexContainer_v1 > el4;
      std::vector< ElementLink< xAOD::TruthVertexContainer_v1 > > el5;
      std::vector< std::vector< ElementLink< xAOD::TruthVertexContainer_v1 > > > el6;
   };
}

#endif // XAODTRUTH_XAODTRUTHDICT_H
