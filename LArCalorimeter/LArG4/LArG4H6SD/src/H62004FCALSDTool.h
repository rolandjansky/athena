/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4H62004SD_H62004FCALSDTOOL_H
#define LARG4H62004SD_H62004FCALSDTOOL_H

// System includes
#include <string>
#include <vector>

// Local includes
#include "H62004SimpleSDTool.h"
#include "LArG4Code/ILArCalculatorSvc.h"

namespace LArG4
{

  /// @class H62004FCALSDTool
  /// @brief Tool for constructing H62004 SDs for FCAL.
  ///
  /// Based on the previous LArG4H62004FCALSDTool implementation.
  ///
  /// This implementation uses the LAr SD wrapper design for managing multiple
  /// SDs when running multi-threaded. See ATLASSIM-2606 for discussions.
  ///
  class H62004FCALSDTool : public H62004SimpleSDTool
  {

  public:

    /// Constructor
    H62004FCALSDTool(const std::string& type, const std::string& name,
                     const IInterface* parent);

  private:

    StatusCode initializeCalculators() override final;

    /// Create the SD wrapper for current worker thread
    G4VSensitiveDetector* makeSD() override final;

    /// Hit collection name
    std::string m_hitCollName;

    ServiceHandle<ILArCalculatorSvc> m_fcal1calc;
    ServiceHandle<ILArCalculatorSvc> m_fcal2calc;
    ServiceHandle<ILArCalculatorSvc> m_fcalcoldcalc;
    /// @name SD volumes
    /// @{
    std::vector<std::string> m_fcal1Volumes;
    std::vector<std::string> m_fcal2Volumes;
    std::vector<std::string> m_fcalColdVolumes;
    /// @}

  }; // class H62004FCALSDTool

} // namespace LArG4

#endif
