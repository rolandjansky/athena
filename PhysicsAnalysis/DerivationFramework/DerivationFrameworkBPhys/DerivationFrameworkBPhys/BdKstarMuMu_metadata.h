/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   BdKstarMuMu_metadata.h
 * @author Pavel Reznicek <pavel.reznicek@cern.ch>
 *
 * @brief  Store JO metadata specific to the Bd->KstarMuMu analysis.
 */

#ifndef DERIVATIONFRAMEWORK_BdKstarMuMu_metadata_H
#define DERIVATIONFRAMEWORK_BdKstarMuMu_metadata_H

#include <string>
#include <map>
#include <vector>

#include "DerivationFrameworkBPhys/BPhysMetadataBase.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace DerivationFramework {
  ///
  /// @class  BdKstarMuMu_metadata
  /// @author Pavel Reznicek <pavel.reznicek@cern.ch>
  ///
  /// @brief  Store JO metadata specific to the Bd->KstarMuMu analysis.
  ///
  /// Store JO metadata specific to the Bd->KstarMuMu analysis in the output file.
  /// This class inherits from BPhysMetadataBase.
  ///
  class BdKstarMuMu_metadata : virtual public BPhysMetadataBase {
    public:
    /// @brief Main constructor
    BdKstarMuMu_metadata(const std::string& t,
                         const std::string& n,
                         const IInterface*  p);
  }; // class
} // namespace

#endif // DERIVATIONFRAMEWORK_BdKstarMuMu_metadata_H
