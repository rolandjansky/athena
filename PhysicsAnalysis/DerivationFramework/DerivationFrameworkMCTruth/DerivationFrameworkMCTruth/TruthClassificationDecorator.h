/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_TRUTHCLASSIFICATIONDECORATOR_H
#define DERIVATIONFRAMEWORK_TRUTHCLASSIFICATIONDECORATOR_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"

class IMCTruthClassifier;

namespace DerivationFramework {

  class TruthClassificationDecorator : public AthAlgTool, public IAugmentationTool {
    public: 
      TruthClassificationDecorator(const std::string& t, const std::string& n, const IInterface* p);
      ~TruthClassificationDecorator();
      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:
      mutable unsigned int m_ntotpart;
      std::string m_particlesKey;
      ToolHandle<IMCTruthClassifier> m_classifier;
  }; 
}

#endif // DERIVATIONFRAMEWORK_TRUTHCLASSIFICATIONDECORATOR_H 
