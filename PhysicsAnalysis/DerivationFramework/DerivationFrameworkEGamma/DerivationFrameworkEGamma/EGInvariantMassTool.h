/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EGInvariantMassTool.h, (c) ATLAS Detector software
// author: giovanni.marchiori@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_EGINVARIANTMASSTOOL_H
#define DERIVATIONFRAMEWORK_EGINVARIANTMASSTOOL_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "ExpressionEvaluation/ExpressionParserUser.h"

namespace DerivationFramework {

  enum EEGInvariantMassToolParser {kParser1,kParser2, kNumEGInvariantMassToolParser};
  class EGInvariantMassTool
     : public ExpressionParserUser<AthAlgTool,kNumEGInvariantMassToolParser>,
       public IAugmentationTool
  {
    public:
      EGInvariantMassTool(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:
      std::string m_expression1, m_expression2;
      std::string m_sgName;
      float m_mass1Hypothesis, m_mass2Hypothesis;
      std::string m_container1Name;
      std::string m_container2Name;
      std::string m_pt1BranchName, m_eta1BranchName, m_phi1BranchName;
      std::string m_pt2BranchName, m_eta2BranchName, m_phi2BranchName;
      bool m_checkCharge;
      float m_mindR;
      bool m_doTransverseMass;
      StatusCode getInvariantMasses(std::vector<float>*) const;
  }; 
}

#endif // DERIVATIONFRAMEWORK_EGINVARIANTMASSTOOL_H
