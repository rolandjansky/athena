/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EGTopoClusterMomentDecorator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_EGTOPOCLUSTERMOMENTDECORATOR_H
#define DERIVATIONFRAMEWORK_EGTOPOCLUSTERMOMENTDECORATOR_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

namespace DerivationFramework {

  class EGTopoClusterMomentDecorator : public AthAlgTool, public IAugmentationTool {
    public: 
      EGTopoClusterMomentDecorator(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:
      std::string m_containerName;
  }; 
}

#endif // DERIVATIONFRAMEWORK_EGTOPOCLUSTERMOMENTDECORATOR_H
