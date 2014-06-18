/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DeltaRTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_DELTARTOOL_H
#define DERIVATIONFRAMEWORK_DELTARTOOL_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

namespace DerivationFramework {

  class DeltaRTool : public AthAlgTool, public IAugmentationTool {
    public: 
      DeltaRTool(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:
      std::string m_sgName;
      std::string m_phi1BranchName,m_phi2BranchName,m_eta1BranchName,m_eta2BranchName;
      StatusCode getDeltaRs(std::vector<float>*&) const;
      float calculateDeltaR(float,float,float,float) const;		
  }; 
}

#endif // DERIVATIONFRAMEWORK_DELTARTOOL_H
