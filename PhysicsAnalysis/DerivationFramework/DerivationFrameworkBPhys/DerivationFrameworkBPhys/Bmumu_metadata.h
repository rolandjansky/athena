/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// Bmumu_metadata.h, (c) ATLAS Detector software
//============================================================================
// 
// Author : Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch.>
// Changes:
//
// Store JO metadata specific to the Bmumu analysis in the output file.
// This class inherits from BPhysMetadataBase.
//
//============================================================================
//
#ifndef DERIVATIONFRAMEWORK_Bmumu_metadata_H
#define DERIVATIONFRAMEWORK_Bmumu_metadata_H

#include <string>
#include <map>
#include <vector>

#include "DerivationFrameworkBPhys/BPhysMetadataBase.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace DerivationFramework {

  class Bmumu_metadata : virtual public BPhysMetadataBase {
    public: 
      Bmumu_metadata(const std::string& t, const std::string& n,
		     const IInterface* p);

      // virtual StatusCode initialize();
      // virtual StatusCode finalize();
      
      // virtual StatusCode addBranches() const;

  private:

  }; // class
} // namespace

#endif // DERIVATIONFRAMEWORK_Bmumu_metadata_H
