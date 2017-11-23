/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_TruthQGDecorationTool_H
#define DERIVATIONFRAMEWORK_TruthQGDecorationTool_H

// Interface classes
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

// STL includes
#include <string>

namespace DerivationFramework {

  class TruthQGDecorationTool : public AthAlgTool, public IAugmentationTool {
    public: 
      TruthQGDecorationTool(const std::string& t, const std::string& n, const IInterface* p);
      ~TruthQGDecorationTool();
      virtual StatusCode addBranches() const;

    private:
      /// Parameter: input collection key
      std::string m_jetsKey;
      /// Parameter: output decoration
      std::string m_decOutput;
  }; 
}

#endif // DERIVATIONFRAMEWORK_TRUTHDRESSINGTool_H
