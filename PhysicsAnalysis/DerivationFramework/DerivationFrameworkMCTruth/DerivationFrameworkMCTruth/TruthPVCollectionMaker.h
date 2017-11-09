/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_TRUTHPVCOLLECTIONMAKER_H
#define DERIVATIONFRAMEWORK_TRUTHPVCOLLECTIONMAKER_H

// Base classes
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
// Standard library includes
#include <string>

namespace DerivationFramework {

  class TruthPVCollectionMaker : public AthAlgTool, public IAugmentationTool {
    public: 
      TruthPVCollectionMaker(const std::string& t, const std::string& n, const IInterface* p);
      ~TruthPVCollectionMaker();
      StatusCode initialize();
      virtual StatusCode addBranches() const;

    private:
      std::string m_eventsKey; //!< Input event collection (navigates to the vertices)
      std::string m_collectionName; //!< Output collection name
  }; 
}

#endif // DERIVATIONFRAMEWORK_TRUTHPVCOLLECTIONMAKER_H
