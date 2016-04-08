// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODBTaggingDict.h 688348 2015-08-10 09:03:52Z kleney $
#ifndef XAODBTAGGING_XAODBTAGGINGDICT_H
#define XAODBTAGGING_XAODBTAGGINGDICT_H

// Needed to successfully generate the dictionary in standalone mode:
#if defined(__GCCXML__) and not defined(EIGEN_DONT_VECTORIZE)
#   define EIGEN_DONT_VECTORIZE
#endif // __GCCXML__

// System include(s):
#include <vector>

// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"

// Local include(s):
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTaggingAuxContainer.h"
#include "xAODBTagging/BTaggingTrigAuxContainer.h"
#include "xAODBTagging/BTagVertexContainer.h" 
#include "xAODBTagging/BTagVertexAuxContainer.h"
// #include "xAODBTagging/versions/BTaggingContainer_v1.h"
// #include "xAODBTagging/versions/BTaggingAuxContainer_v1.h"
// #include "xAODBTagging/versions/BTaggingTrigAuxContainer_v1.h"
// #include "xAODBTagging/versions/BTagVertexContainer_v1.h" 
// #include "xAODBTagging/versions/BTagVertexAuxContainer_v1.h"

namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODBTAGGING {
      xAOD::BTaggingContainer_v1 c1;
      DataLink< xAOD::BTaggingContainer_v1 > dl1;
      std::vector< DataLink< xAOD::BTaggingContainer_v1 > > dl2;
      ElementLink< xAOD::BTaggingContainer_v1 > el1;
      std::vector< ElementLink< xAOD::BTaggingContainer_v1 > > el2;
      std::vector< std::vector< ElementLink< xAOD::BTaggingContainer_v1 > > > el3;


      std::vector< std::vector< bool > > atmp;
      std::vector< std::vector< int > > anothertmp;

      
      xAOD::BTagVertexContainer_v1 cx1;
      DataLink< xAOD::BTagVertexContainer_v1 > dlx1;
      std::vector< DataLink< xAOD::BTagVertexContainer_v1 > > dlx2;
      ElementLink< xAOD::BTagVertexContainer_v1 > elx1;
      std::vector< ElementLink< xAOD::BTagVertexContainer_v1 > > elx2;
      std::vector< std::vector< ElementLink< xAOD::BTagVertexContainer_v1 > > > elx3;

     ElementLink< xAOD::TrackParticleContainer > auxlink1;
     std::vector<ElementLink< xAOD::TrackParticleContainer > > auxlink2;
   };
}

#endif // XAODBTAGGING_XAODBTAGGINGDICT_H
