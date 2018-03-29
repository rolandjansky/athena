/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HardScatterCollectionMaker.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_HARDSCATTERCOLLECTIONMAKER_H
#define DERIVATIONFRAMEWORK_HARDSCATTERCOLLECTIONMAKER_H

// Base classes
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
// Just a couple of EDM includes
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertexContainer.h"
// Standard library includes
#include <vector>
#include <string>

namespace DerivationFramework {

  class HardScatterCollectionMaker : public AthAlgTool, public IAugmentationTool {
    public: 
      HardScatterCollectionMaker(const std::string& t, const std::string& n, const IInterface* p);
      ~HardScatterCollectionMaker();
      StatusCode initialize();
      virtual StatusCode addBranches() const;

    private:
      std::string m_eventsKey; //!< Input particle collection (navigates to the vertices)
      std::string m_collectionName; //!< Output collection name stem
      int m_generations; //!< Number of generations after the particle in question to keep
      int addTruthParticle(const xAOD::TruthParticle& old_part, xAOD::TruthParticleContainer* part_cont,
                          xAOD::TruthVertexContainer* vert_cont, std::vector<int>& seen_particles,
                          const int generations) const;
      int addTruthVertex( const xAOD::TruthVertex& old_vert, xAOD::TruthParticleContainer* part_cont,
                          xAOD::TruthVertexContainer* vert_cont, std::vector<int>& seen_particles,
                          const int generations) const;
  }; 
}

#endif // DERIVATIONFRAMEWORK_HARDSCATTERCOLLECTIONMAKER_H
