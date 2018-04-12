/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_TRUTHCOLLECTIONMAKERTAU_H
#define DERIVATIONFRAMEWORK_TRUTHCOLLECTIONMAKERTAU_H

// Interface classes
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

// For the tool handle
#include "GaudiKernel/ToolHandle.h"

// Forward declarations
namespace tauRecTools{
  class IBuildTruthTaus;
}

namespace DerivationFramework {

  class TruthCollectionMakerTau : public AthAlgTool, public IAugmentationTool {
  public: 
    TruthCollectionMakerTau(const std::string& t, const std::string& n, const IInterface* p);
    ~TruthCollectionMakerTau();
    StatusCode initialize();
    virtual StatusCode addBranches() const;

  private:

    ToolHandle<tauRecTools::IBuildTruthTaus> m_buildTruthTaus;

  }; 
}

#endif // DERIVATIONFRAMEWORK_TRUTHCOLLECTIONMAKERTAU_H
