/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4SD_MINIFCALSDTOOL_H
#define LARG4SD_MINIFCALSDTOOL_H

// System includes
#include <string>
#include <vector>

// Project includes
#include "LArG4Code/SimpleSDTool.h"
#include "LArG4Code/ILArCalculatorSvc.h"

namespace LArG4
{

  /// @class MiniFCALSDTool
  /// @brief SD tool which manages the Mini FCAL sensitive detectors.
  ///
  /// NOTE: this design is in flux.
  ///
  class MiniFCALSDTool : public SimpleSDTool
  {

  public:

    // Constructor
    MiniFCALSDTool(const std::string& type, const std::string& name,
                   const IInterface* parent);

  private:

    // Initialize Calculator Services
    StatusCode initializeCalculators() override final;

    /// Create the SD wrapper for current worker thread
    G4VSensitiveDetector* makeSD() override final;

    /// The list of volumes
    std::vector<std::string> m_miniVolumes;

    ServiceHandle<ILArCalculatorSvc> m_calc; //MiniFCALCalculator::GetCalculator()

  }; // class MiniFCALSDTool

} // namespace LArG4

#endif
