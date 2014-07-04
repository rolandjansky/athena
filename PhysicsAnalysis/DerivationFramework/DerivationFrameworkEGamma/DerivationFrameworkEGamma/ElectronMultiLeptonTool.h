/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ElectronMultiLeptonTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_ELECTRONMULTILEPTONTOOL_H
#define DERIVATIONFRAMEWORK_ELECTRONMULTILEPTONTOOL_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "ElectronPhotonSelectorTools/TElectronMultiLeptonSelector.h"

namespace DerivationFramework {

  class ElectronMultiLeptonTool : public AthAlgTool, public IAugmentationTool {
    public: 
      ElectronMultiLeptonTool(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:
      std::string m_sgName;
      Root::TElectronMultiLeptonSelector* m_mlTool;
  }; 
}

#endif // DERIVATIONFRAMEWORK_ELECTRONMULTILEPTONTOOL_H
