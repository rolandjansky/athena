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
      std::vector<int> m_pdgIdsToKeep; //!< List of PDG IDs to build this collection from
      bool m_keepBHadrons; //!< Option to keep all b-hadrons (better than giving PDG IDs)
      bool m_keepCHadrons; //!< Option to keep all c-hadrons (better than giving PDG IDs)
      bool m_keepBSM; //!< Option to keep all BSM particles (better than giving PDG IDs)
      std::string m_particlesKey; //!< Input particle collection (navigates to the vertices)
      std::string m_collectionName; //!< Output collection name stem
      int m_generations; //!< Number of generations after the particle in question to keep
      // Helper functions for building up the decay product collections
      int addTruthParticle( const xAOD::TruthParticle& old_part, xAOD::TruthParticleContainer* part_cont,
                                  xAOD::TruthVertexContainer* vert_cont, std::vector<int>& seen_particles,
                            const int generations=-1) const;
      int addTruthVertex( const xAOD::TruthVertex& old_vert, xAOD::TruthParticleContainer* part_cont,
                                xAOD::TruthVertexContainer* vert_cont, std::vector<int>& seen_particles,
                          const int generations=-1) const;
  }; 
}

#endif // DERIVATIONFRAMEWORK_TRUTHDECAYCOLLECTIONMAKER_H
