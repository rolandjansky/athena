/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EGTransverseMassTool.h, (c) ATLAS Detector software
// author: giovanni.marchiori@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_EGTRANSVERSEMASSTOOL_H
#define DERIVATIONFRAMEWORK_EGTRANSVERSEMASSTOOL_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

#include "ExpressionEvaluation/ExpressionParserUser.h"

namespace DerivationFramework {

  class EGTransverseMassTool : public ExpressionParserUser<AthAlgTool>, public IAugmentationTool {
    public:
      EGTransverseMassTool(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:
      std::string m_expression1;
      float m_METmin;
      std::string m_sgName;
      float m_mass1Hypothesis;
      std::string m_container1Name;
      std::string m_container2Name;
      std::string m_pt1BranchName, m_phi1BranchName;
      std::string m_pt2BranchName, m_phi2BranchName;
      StatusCode getTransverseMasses(std::vector<float>*&) const;
  }; 
}

#endif // DERIVATIONFRAMEWORK_EGTRANSVERSEMASSTOOL_H
