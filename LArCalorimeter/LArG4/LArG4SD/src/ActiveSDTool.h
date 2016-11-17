/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4SD_ACTIVESDTOOL_H
#define LARG4SD_ACTIVESDTOOL_H

// System includes
#include <string>
#include <vector>

// Project includes
#include "LArG4Code/CalibSDTool.h"

namespace LArG4
{

  /// @class ActiveSDTool
  /// @brief Sensitive detector tool which manages activate-area LAr calib SDs.
  ///
  /// Design is in flux.
  ///
  class ActiveSDTool : public CalibSDTool
  {

    public:

      /// Constructor
      ActiveSDTool(const std::string& type, const std::string& name,
                   const IInterface* parent);

    private:

      /// Create the SD wrapper for current worker thread
      G4VSensitiveDetector* makeSD() override final;

      /// Hit collection name
      std::string m_hitCollName;

      /// @name SD volumes
      /// @{
      std::vector<std::string> m_stacVolumes;
      std::vector<std::string> m_presBarVolumes;
      std::vector<std::string> m_posIWVolumes;
      std::vector<std::string> m_negIWVolumes;
      std::vector<std::string> m_posOWVolumes;
      std::vector<std::string> m_negOWVolumes;
      std::vector<std::string> m_presECVolumes;
      std::vector<std::string> m_bobVolumes;
      std::vector<std::string> m_fcal1Volumes;
      std::vector<std::string> m_fcal2Volumes;
      std::vector<std::string> m_fcal3Volumes;
      std::vector<std::string> m_sliceVolumes;
      std::vector<std::string> m_miniVolumes;
      /// @}

  }; // class ActiveSDTool

} // namespace LArG4

#endif
