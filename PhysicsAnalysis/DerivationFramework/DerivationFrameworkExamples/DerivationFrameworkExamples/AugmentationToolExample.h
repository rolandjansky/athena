/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AugmentationExample.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_AUGMENTATIONTOOLEXAMPLE_H
#define DERIVATIONFRAMEWORK_AUGMENTATIONTOOLEXAMPLE_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

namespace DerivationFramework {

  class AugmentationToolExample : public AthAlgTool, public IAugmentationTool {
    public: 
      AugmentationToolExample(const std::string& t, const std::string& n, const IInterface* p);
      virtual StatusCode addBranches() const;
  }; 
}

#endif // DERIVATIONFRAMEWORK_AUGMENTATIONTOOLEXAMPLE_H
