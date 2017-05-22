/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4H62004SD_H62004EMECSDTOOL_H
#define LARG4H62004SD_H62004EMECSDTOOL_H

// System includes
#include <string>

// Local includes
#include "H62004SimpleSDTool.h"
#include "LArG4Code/ILArCalculatorSvc.h"

namespace LArG4
{

  /// @class H62004EMECSDTool
  /// @brief Tool for constructing H62004 SDs for EMEC.
  ///
  /// Based on the previous LArG4H62004EMECSDTool implementation.
  ///
  /// This implementation uses the LAr SD wrapper design for managing multiple
  /// SDs when running multi-threaded. See ATLASSIM-2606 for discussions.
  ///
  class H62004EMECSDTool : public H62004SimpleSDTool
  {

  public:

    /// Constructor
    H62004EMECSDTool(const std::string& type, const std::string& name,
                     const IInterface* parent);

  private:

    StatusCode initializeCalculators() override final;

    /// Create the SD wrapper for current worker thread
    G4VSensitiveDetector* makeSD() override final;

    /// Hit collection name
    std::string m_hitCollName;

    ServiceHandle<ILArCalculatorSvc> m_calculator;
  }; // class H62004EMECSDTool

} // namespace LArG4

#endif
