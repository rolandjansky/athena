/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4SD_EMBSDTOOL_H
#define LARG4SD_EMBSDTOOL_H

// System includes
#include <string>
#include <vector>

// Project includes
#include "LArG4Code/SimpleSDTool.h"

namespace LArG4
{

  /// @class EMBSDTool
  /// @brief SD tool which manages EM barrel sensitive detectors.
  ///
  /// NOTE: this design is in flux, migrating to be more multi-threading-friendly
  ///
  class EMBSDTool : public SimpleSDTool
  {

    public:

      /// Constructor
      EMBSDTool(const std::string& type, const std::string& name,
                const IInterface* parent);

    private:

      /// Create the SD wrapper for current worker thread
      G4VSensitiveDetector* makeSD() override final;

      /// @name Configuration
      /// @{

      /// Hit collection name
      std::string m_hitCollName;

      /// List of volumes for the stac SD
      std::vector<std::string> m_stacVolumes;
      /// List of volumes for the presampler SD
      std::vector<std::string> m_presVolumes;

      /// @}

  }; // class EMBSDTool

} // namespace LArG4

#endif
