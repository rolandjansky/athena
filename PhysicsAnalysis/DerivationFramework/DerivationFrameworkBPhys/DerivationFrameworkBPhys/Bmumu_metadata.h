/* 
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   Bmumu_metadata.h
 * @author Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch>
 *
 * @brief  Store JO metadata specific to the Bmumu analysis.
 */

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
  ///
  /// @class  Bmumu_metadata
  /// @author Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch.>
  ///
  /// @brief  Store JO metadata specific to the Bmumu analysis.
  ///
  /// Store JO metadata specific to the Bmumu analysis in the output file.
  /// This class inherits from BPhysMetadataBase.
  ///
  class Bmumu_metadata : virtual public BPhysMetadataBase {
    public: 
    /// @brief Main constructor
    Bmumu_metadata(const std::string& t, const std::string& n,
		     const IInterface* p);
  }; // class
} // namespace

#endif // DERIVATIONFRAMEWORK_Bmumu_metadata_H
