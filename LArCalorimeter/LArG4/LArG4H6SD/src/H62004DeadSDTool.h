/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4H62004SD_H62004DEADSDTOOL_H
#define LARG4H62004SD_H62004DEADSDTOOL_H

// System includes
#include <string>
#include <vector>

// Local includes
#include "H62004CalibSDTool.h"
#include "LArG4Code/ILArCalibCalculatorSvc.h"

namespace LArG4
{

  /// @class H62004DeadSDTool
  /// @brief Tool for constructing H62004 calib SDs for dead material.
  ///
  /// Based on the previous LArG4H62004DeadSDTool implementation.
  ///
  /// This implementation uses the LAr SD wrapper design for managing multiple
  /// SDs when running multi-threaded. See ATLASSIM-2606 for discussions.
  ///
  class H62004DeadSDTool : public H62004CalibSDTool
  {

  public:

    /// Constructor
    H62004DeadSDTool(const std::string& type, const std::string& name,
                     const IInterface* parent);

  private:
    StatusCode initializeCalculators() override final;

    /// Create the SD wrapper for current worker thread
    G4VSensitiveDetector* makeSD() override final;

    /// Hit collection name
    std::string m_hitCollName;

    ServiceHandle<ILArCalibCalculatorSvc> m_calculator;
    /// Do we add the escaped energy processing?
    /// This is only in "mode 1" (Tile+LAr), not in "DeadLAr" mode
    bool m_do_eep;

  }; // class H62004DeadSDTool

} // namespace LArG4

#endif
