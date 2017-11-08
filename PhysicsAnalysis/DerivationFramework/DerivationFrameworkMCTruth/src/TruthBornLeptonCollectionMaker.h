/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TruthBornLeptonCollectionMaker.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_TRUTHBORNLEPTONCOLLECTIONMAKER_H
#define DERIVATIONFRAMEWORK_TRUTHBORNLEPTONCOLLECTIONMAKER_H

// Base classes
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

// EDM includes for the particles we need
#include "xAODTruth/TruthParticle.h"

// STL includes
#include <string>

namespace DerivationFramework {

  class TruthBornLeptonCollectionMaker : public AthAlgTool, public IAugmentationTool {
    public: 
      TruthBornLeptonCollectionMaker(const std::string& t, const std::string& n, const IInterface* p);
      ~TruthBornLeptonCollectionMaker();
      StatusCode initialize();
      virtual StatusCode addBranches() const;

    private:
      std::string m_particlesKey; //!< Input particle collection key
      std::string m_collectionName; //!< Output particle collection key
      /// Helper function for finding bare descendents of born leptons
      bool hasBareDescendent( const xAOD::TruthParticle* p ) const;
  }; 
}

#endif // DERIVATIONFRAMEWORK_TRUTHBORNLEPTONCOLLECTIONMAKER_H
