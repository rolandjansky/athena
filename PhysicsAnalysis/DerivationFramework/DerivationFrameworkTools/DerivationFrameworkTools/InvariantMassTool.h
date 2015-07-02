/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InvariantMassTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_INVARIANTMASSTOOL_H
#define DERIVATIONFRAMEWORK_INVARIANTMASSTOOL_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

namespace ExpressionParsing {
  class ExpressionParser;
}

namespace DerivationFramework {

  class InvariantMassTool : public AthAlgTool, public IAugmentationTool {
    public: 
      InvariantMassTool(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:
      std::string m_expression;
      std::string m_expression2;
      ExpressionParsing::ExpressionParser *m_parser;
      ExpressionParsing::ExpressionParser *m_parser2;
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
