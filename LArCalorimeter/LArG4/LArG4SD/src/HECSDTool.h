/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4SD_HECSDTOOL_H
#define LARG4SD_HECSDTOOL_H

// System includes
#include <string>
#include <vector>

// Project includes
#include "LArG4Code/SimpleSDTool.h"
#include "LArG4Code/ILArCalculatorSvc.h"

namespace LArG4
{

  /// @class HECSDTool
  /// @brief SD tool which manages the LAr hadronic endcap sensitive detectors.
  ///
  /// NOTE: this design is in flux, migrating to be more multi-threading-friendly
  ///
  class HECSDTool : public SimpleSDTool
  {

    public:

      /// Constructor
      HECSDTool(const std::string& type, const std::string& name,
                const IInterface* parent);

    private:

      /// Initialize Calculator Services
      StatusCode initializeCalculators() override final;

     /// Create the SD wrapper for current worker thread
      G4VSensitiveDetector* makeSD() override final;

      /// List of volumes for each SD
      /// @{
      std::vector<std::string> m_sliceVolumes;
      std::vector<std::string> m_localVolumes;
      std::vector<std::string> m_wheelVolumes;
      /// @}

      ServiceHandle<ILArCalculatorSvc> m_heccalc; //LArHECWheelCalculator::GetCalculator()
  }; // class HECSDTool

} // namespace LArG4

#endif
