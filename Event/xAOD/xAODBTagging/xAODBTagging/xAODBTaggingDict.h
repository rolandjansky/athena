// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODBTAGGING_XAODBTAGGINGDICT_H
#define XAODBTAGGING_XAODBTAGGINGDICT_H

// Local include(s).
#include "xAODBTagging/BTagging.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTaggingAuxContainer.h"
#include "xAODBTagging/BTaggingTrigAuxContainer.h"
#include "xAODBTagging/versions/BTagging_v1.h"
#include "xAODBTagging/versions/BTaggingContainer_v1.h"
#include "xAODBTagging/versions/BTaggingAuxContainer_v1.h"
#include "xAODBTagging/versions/BTaggingTrigAuxContainer_v1.h"

#include "xAODBTagging/BTagVertex.h"
#include "xAODBTagging/BTagVertexContainer.h"
#include "xAODBTagging/BTagVertexAuxContainer.h"
#include "xAODBTagging/versions/BTagVertex_v1.h"
#include "xAODBTagging/versions/BTagVertexContainer_v1.h"
#include "xAODBTagging/versions/BTagVertexAuxContainer_v1.h"

#include "xAODBTagging/SecVtxHelper.h"

// EDM include(s).
#include "xAODCore/tools/DictHelpers.h"

// Instantiate all necessary types for the dictionary.
namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODBTAGGING {
      // Local type(s).
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, BTaggingContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, BTagVertexContainer_v1 );
      // Type(s) needed for the dictionary generation to succeed.
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TrackParticleContainer );
   };
}

#endif // XAODBTAGGING_XAODBTAGGINGDICT_H
