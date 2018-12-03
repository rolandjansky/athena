/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_TAUDECORATOR_H
#define DERIVATIONFRAMEWORK_TAUDECORATOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

namespace DerivationFramework {

  class TauDecorator : public AthAlgTool, public IAugmentationTool {
    public: 
      TauDecorator(const std::string& t, const std::string& n, const IInterface* p);
      ~TauDecorator();
      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:
      std::string m_tauContainerName;
  }; 
}

#endif // DERIVATIONFRAMEWORK_TAUDECORATOR_H
