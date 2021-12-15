/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_TruthJetDecorationTool_H
#define DERIVATIONFRAMEWORK_TruthJetDecorationTool_H

// Interface classes
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

// STL includes
#include <string>

namespace DerivationFramework {

  class TruthJetDecorationTool : public AthAlgTool, public IAugmentationTool {
    public: 
      TruthJetDecorationTool(const std::string& t, const std::string& n, const IInterface* p);
      ~TruthJetDecorationTool();
      virtual StatusCode addBranches() const;

    private:
      /// Parameter: input collection key
      std::string m_jetsKey;
  }; 
}

#endif // DERIVATIONFRAMEWORK_TRUTHDRESSINGTool_H
