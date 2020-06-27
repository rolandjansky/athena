/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CollectionMakerHelpers.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_COLLECTIONMAKERHELPERS_H
#define DERIVATIONFRAMEWORK_COLLECTIONMAKERHELPERS_H

// Just a couple of EDM includes
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertexContainer.h"
// Standard library includes
#include <vector>

namespace DerivationFramework {

  class CollectionMakerHelpers {

    public:
      //!< Helper function to add truth particles to a collection
      static int addTruthParticle(const xAOD::TruthParticle& oldPart, xAOD::TruthParticleContainer* partCont,
                          xAOD::TruthVertexContainer* vertCont, std::vector<int>& seenParticles,
                          const int generations, bool includeVertex=true);
      //!< Helper function to add truth vertices to a collection
      static int addTruthVertex( const xAOD::TruthVertex& oldVert, xAOD::TruthParticleContainer* partCont,
                          xAOD::TruthVertexContainer* vertCont, std::vector<int>& seenParticles,
                          const int generations);
      //!< Helper function to set up a truth particle based on an old particle
      static xAOD::TruthParticle* setupTruthParticle(const xAOD::TruthParticle& oldPart, xAOD::TruthParticleContainer* partCont);
  }; 
}

#endif // DERIVATIONFRAMEWORK_COLLECTIONMAKERHELPERS_H
