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
      ExpressionParsing::ExpressionParser *m_parser;
      std::string m_sgName;
      float m_massHypothesis;
      std::string m_containerName;
      StatusCode getInvariantMasses(std::vector<float>*&) const;
      float calculateInvariantMass(float,float,float,float,float,float,float) const;		
  }; 
}

#endif // DERIVATIONFRAMEWORK_INVARIANTMASSTOOL_H
