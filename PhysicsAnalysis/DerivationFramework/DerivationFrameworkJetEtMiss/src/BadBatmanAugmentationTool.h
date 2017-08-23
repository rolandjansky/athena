/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BatBatmanAugmentationTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Chris Young (christopher.young@cern.ch)
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_BADBATMANAUGMENTATIONTOOL_H
#define DERIVATIONFRAMEWORK_BADBATMANAUGMENTATIONTOOL_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace DerivationFramework {

  class BadBatmanAugmentationTool : public AthAlgTool, public IAugmentationTool {
  public: 
    BadBatmanAugmentationTool(const std::string& t, const std::string& n, const IInterface* p);

    StatusCode initialize();
    StatusCode finalize();
    virtual StatusCode addBranches() const;

  private:
    
  }; 
}

#endif // DERIVATIONFRAMEWORK_PFLOWAUGMENTATIONTOOL_H
