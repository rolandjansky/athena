/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// NTUPAugmentationExample.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_NTUPAUGMENTATIONTOOLEXAMPLE_H
#define DERIVATIONFRAMEWORK_NTUPAUGMENTATIONTOOLEXAMPLE_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

namespace DerivationFramework {

  class NTUPAugmentationToolExample : public AthAlgTool, public IAugmentationTool {
    public: 
      NTUPAugmentationToolExample(const std::string& t, const std::string& n, const IInterface* p);
      virtual StatusCode addBranches() const;
  }; 
}

#endif // DERIVATIONFRAMEWORK_NTUPAUGMENTATIONTOOLEXAMPLE_H
