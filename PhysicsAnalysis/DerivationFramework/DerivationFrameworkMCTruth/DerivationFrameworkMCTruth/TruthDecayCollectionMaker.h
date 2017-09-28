/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TruthDecayCollectionMaker.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_TRUTHDECAYCOLLECTIONMAKER_H
#define DERIVATIONFRAMEWORK_TRUTHDECAYCOLLECTIONMAKER_H

// Base classes
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
// EDM -- typedefs so these are includes
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertexContainer.h"
// Standard library includes
#include <vector>
#include <string>

namespace DerivationFramework {

  class TruthDecayCollectionMaker : public AthAlgTool, public IAugmentationTool {
    public: 
      TruthDecayCollectionMaker(const std::string& t, const std::string& n, const IInterface* p);
      ~TruthDecayCollectionMaker();
      StatusCode initialize();
      virtual StatusCode addBranches() const;

    private:
      std::vector<int> m_pdgIdsToKeep;
      std::string m_particlesKey;
      std::string m_verticesKey;
      std::string m_collectionName; 
      // Helper functions for building up the decay product collections
      ElementLink<xAOD::TruthParticleContainer> addTruthParticle( const xAOD::TruthParticle& old_part, xAOD::TruthParticleContainer* part_cont,
                                                                  xAOD::TruthVertexContainer* vert_cont, std::vector<int>& seen_particles) const;
      ElementLink<xAOD::TruthVertexContainer> addTruthVertex( const xAOD::TruthVertex& old_vert, xAOD::TruthParticleContainer* part_cont,
                                                              xAOD::TruthVertexContainer* vert_cont, std::vector<int>& seen_particles) const;
  }; 
}

#endif // DERIVATIONFRAMEWORK_TRUTHDECAYCOLLECTIONMAKER_H
