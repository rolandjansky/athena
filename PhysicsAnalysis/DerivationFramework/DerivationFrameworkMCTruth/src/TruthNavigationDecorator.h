/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_TRUTHNAVIGATIONDECORATOR_H
#define DERIVATIONFRAMEWORK_TRUTHNAVIGATIONDECORATOR_H

// Interface classes
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

// EDM includes -- typedefs, so can't just be forward declared
#include "xAODTruth/TruthParticleContainer.h"

// STL includes
#include <string>
#include <vector>

namespace DerivationFramework {

  class TruthNavigationDecorator : public AthAlgTool, public IAugmentationTool {
    public: 
      TruthNavigationDecorator(const std::string& t, const std::string& n, const IInterface* p);
      ~TruthNavigationDecorator();
      virtual StatusCode addBranches() const;

    private:
      /// Parameter: input particle collections
      std::vector<std::string> m_inputKeys;
      /// Helper function for finding all the parents of a particle
      void find_parents( const xAOD::TruthParticle* part ,
                         std::vector<ElementLink<xAOD::TruthParticleContainer> >& parents ,
                         std::map<int,ElementLink<xAOD::TruthParticleContainer> >& linkMap ,
                         std::vector<int>& seen_particles ) const;
      /// Helper function for finding all the children of a particle
      void find_children( const xAOD::TruthParticle* part ,
                          std::vector<ElementLink<xAOD::TruthParticleContainer> >& parents ,
                          std::map<int,ElementLink<xAOD::TruthParticleContainer> >& linkMap ,
                          std::vector<int>& seen_particles ) const;
  }; 
}

#endif // DERIVATIONFRAMEWORK_TRUTHNAVIGATIONDECORATOR_H
