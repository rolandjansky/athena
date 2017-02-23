/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4H8SD_H8CALIBSDTOOL_H
#define LARG4H8SD_H8CALIBSDTOOL_H

// STL includes
#include <string>
#include <vector>

// LArG4 includes
#include "LArG4Code/CalibSDTool.h"

class ILArCalibCalculatorSvc;
namespace LArG4
{

  /// @class H8CalibSDTool
  /// @brief Sensitive detector tool which handles calib SDs for the H8
  /// testbeam setup (?).
  ///
  /// Based on the LArG4H8CalibSDTool implementation.
  ///
  class H8CalibSDTool : public CalibSDTool
  {

  public:

    /// Constructor
    H8CalibSDTool(const std::string& type, const std::string& name,
                  const IInterface* parent);

  private:
    StatusCode initializeCalculators() override final;

    /// Create the SD wrapper for current worker thread
    G4VSensitiveDetector* makeSD() override final;

    /// Hit collection name
    std::string m_hitCollName;

    ServiceHandle<ILArCalibCalculatorSvc> m_tbbarcrycalc;
    ServiceHandle<ILArCalibCalculatorSvc> m_barpscalc;
    ServiceHandle<ILArCalibCalculatorSvc> m_barcalc;
    ServiceHandle<ILArCalibCalculatorSvc> m_h8defaultcalc;

    /// @name SD volumes
    /// @{
    std::vector<std::string> m_barCryVolumes;
    std::vector<std::string> m_bpInVolumes;
    std::vector<std::string> m_bpDeadVolumes;
    std::vector<std::string> m_bpCalibVolumes;
    std::vector<std::string> m_barInVolumes;
    std::vector<std::string> m_barDeadVolumes;
    /// @}

  }; // class H8CalibSDTool

}

#endif
