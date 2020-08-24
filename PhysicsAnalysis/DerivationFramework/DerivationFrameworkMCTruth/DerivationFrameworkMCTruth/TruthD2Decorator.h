/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TruthD2Decorator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_TRUTHD2DECORATOR_H
#define DERIVATIONFRAMEWORK_TRUTHD2DECORATOR_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

namespace DerivationFramework {

  class TruthD2Decorator : public AthAlgTool, public IAugmentationTool {
    public: 
      TruthD2Decorator(const std::string& t, const std::string& n, const IInterface* p);
      virtual StatusCode addBranches() const;

    private:
      //configurables
      std::string m_jetContainerKey; 
      std::string m_decorationName;
  }; 
}

#endif // DERIVATIONFRAMEWORK_TruthD2Decorator_H
