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
#include "StoreGate/ReadHandleKey.h"
#include "xAODBase/IParticleContainer.h"

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
      SG::WriteHandleKey<std::vector<float> > m_sgName {this,"StoreGateEntryName","","SG key of output object"};
      float m_massHypothesis,m_massHypothesis2;
      SG::ReadHandleKey<xAOD::IParticleContainer> m_containerName  {this,"ContainerName","","SG key of first container"};
      SG::ReadHandleKey<xAOD::IParticleContainer> m_containerName2 {this,"SecondContainerName","","SG key of first container"};
      StatusCode getInvariantMasses(std::vector<float>*) const;
      float calculateInvariantMass(float,float,float,float,float,float,float) const;		
      float calculateInvariantMass(float,float,float,float,float,float,float,float) const;
  }; 
}

#endif // DERIVATIONFRAMEWORK_INVARIANTMASSTOOL_H
