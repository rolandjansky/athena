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
// For handles
#include "GaudiKernel/ServiceHandle.h"
// Standard library includes
#include <string>

// Forward declaration
class StoreGateSvc;

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
      ServiceHandle<StoreGateSvc> m_metaStore; //!< Handle on the metadata store for init
  }; 
}

#endif // DERIVATIONFRAMEWORK_HARDSCATTERCOLLECTIONMAKER_H
