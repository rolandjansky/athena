/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4SD_EMECSDTOOL_H
#define LARG4SD_EMECSDTOOL_H

// System includes
#include <string>
#include <vector>

// Project includes
#include "LArG4Code/SimpleSDTool.h"
#include "LArG4Code/ILArCalculatorSvc.h"

namespace LArG4
{

  /// @class EMECSDTool
  /// @brief SD tool which manages EM endcap sensitive detectors.
  ///
  /// NOTE: this design is in flux, migrating to be more multi-threading-friendly
  ///
  class EMECSDTool : public SimpleSDTool
  {

    public:

      /// Constructor
      EMECSDTool(const std::string& type, const std::string& name,
                 const IInterface* parent);

    private:

      /// Initialize Calculator Services
      StatusCode initializeCalculators() override final;

      /// Create the SD wrapper for current worker thread
      G4VSensitiveDetector* makeSD() override final;

      /// @name List of volumes for each SD and the corresponding SD
      /// @{
      std::vector<std::string> m_posIWVolumes;
      std::vector<std::string> m_negIWVolumes;
      std::vector<std::string> m_posOWVolumes;
      std::vector<std::string> m_negOWVolumes;
      std::vector<std::string> m_presVolumes;
      std::vector<std::string> m_bobVolumes;
      /// @}

      ServiceHandle<ILArCalculatorSvc> m_emepiwcalc; //EnergyCalculator(LArG4::InnerAbsorberWheel, LArG4::EMEC_ECOR_ROPT, 1)
      ServiceHandle<ILArCalculatorSvc> m_emeniwcalc; //EC::EnergyCalculator(LArWheelCalculator::InnerAbsorberWheel, LArG4::EMEC_ECOR_ROPT, -1),
      ServiceHandle<ILArCalculatorSvc> m_emepowcalc; //EC::EnergyCalculator(LArWheelCalculator::OuterAbsorberWheel, EC::EnergyCalculator::EMEC_ECOR_ROPT, 1),
      ServiceHandle<ILArCalculatorSvc> m_emenowcalc; //EC::EnergyCalculator(LArWheelCalculator::OuterAbsorberWheel, EC::EnergyCalculator::EMEC_ECOR_ROPT, -1),
      ServiceHandle<ILArCalculatorSvc> m_emepscalc; //LArEndcapPresamplerCalculator::GetCalculator()
      ServiceHandle<ILArCalculatorSvc> m_emeobarcalc; //EC::EnergyCalculator(LArWheelCalculator::BackOuterBarretteWheel)



  }; // class EMECSDTool

} // namespace LArG4

#endif
