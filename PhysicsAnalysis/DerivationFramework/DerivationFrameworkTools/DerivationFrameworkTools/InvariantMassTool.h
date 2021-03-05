/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InvariantMassTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_INVARIANTMASSTOOL_H
#define DERIVATIONFRAMEWORK_INVARIANTMASSTOOL_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

#include "ExpressionEvaluation/ExpressionParserUser.h"

namespace DerivationFramework {

 enum  EInvariantMassToolParser { kInvariantMassToolParser1, kInvariantMassToolParser2, kInvariantMassToolParserNum };
 class InvariantMassTool : public ExpressionParserUser<AthAlgTool,kInvariantMassToolParserNum>, public IAugmentationTool {
    public: 
      InvariantMassTool(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:
      std::string m_expression;
      std::string m_expression2;
      std::string m_sgName;
      float m_massHypothesis,m_massHypothesis2;
      std::string m_containerName;
      std::string m_containerName2;
      StatusCode getInvariantMasses(std::vector<float>*) const;
      float calculateInvariantMass(float,float,float,float,float,float,float) const;		
      float calculateInvariantMass(float,float,float,float,float,float,float,float) const;
  }; 
}

#endif // DERIVATIONFRAMEWORK_INVARIANTMASSTOOL_H
