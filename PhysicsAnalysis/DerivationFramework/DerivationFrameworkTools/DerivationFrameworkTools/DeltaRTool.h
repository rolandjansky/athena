/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DeltaRTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_DELTARTOOL_H
#define DERIVATIONFRAMEWORK_DELTARTOOL_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

#include "ExpressionEvaluation/ExpressionParserUser.h"

namespace DerivationFramework {

  enum EDeltaRToolParser {kDeltaRToolParser1,kDeltaRToolParser2,kDeltaRToolParserNum};
  class DeltaRTool : public ExpressionParserUser<AthAlgTool,kDeltaRToolParserNum>, public IAugmentationTool {
    public:
      DeltaRTool(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:
      std::string m_expression;
      std::string m_2ndExpression;
      std::string m_sgName;
      std::string m_containerName;
      std::string m_2ndContainerName;
      StatusCode getDeltaRs(std::vector<float>*) const;
      float calculateDeltaR(float,float,float,float) const;
  }; 
}

#endif // DERIVATIONFRAMEWORK_DELTARTOOL_H
