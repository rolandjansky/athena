/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4SD_DEADSDTOOL_H
#define LARG4SD_DEADSDTOOL_H

// System includes
#include <string>
#include <vector>

// Project includes
#include "LArG4Code/CalibSDTool.h"

namespace LArG4
{

  class DeadSDTool : public CalibSDTool
  {

    public:

      // Constructor
      DeadSDTool(const std::string& type, const std::string& name,
                 const IInterface* parent);

    private:

      /// Initialize Calculator Services
      StatusCode initializeCalculators() override final;

      /// Create the SD wrapper for current worker thread
      G4VSensitiveDetector* makeSD() override final;

      /// Hit collection name
      std::string m_hitCollName;

      /// Do we add the escaped energy processing?
      /// This is only in "mode 1" (Tile+LAr), not in "DeadLAr" mode
      bool m_do_eep;

      // The volumes per SD, and the corresponding SDs
      std::vector<std::string> m_barCryVolumes;
      std::vector<std::string> m_barCryLArVolumes;
      std::vector<std::string> m_barCryMixVolumes;
      std::vector<std::string> m_DMVolumes;
      std::vector<std::string> m_barPresVolumes;
      std::vector<std::string> m_barVolumes;
      std::vector<std::string> m_ECCryVolumes;
      std::vector<std::string> m_ECCryLArVolumes;
      std::vector<std::string> m_ECCryMixVolumes;
      std::vector<std::string> m_ECSupportVolumes;
      std::vector<std::string> m_HECWheelVolumes;
      //LArG4CalibSD* m_barCrySD;
      //LArG4CalibSD* m_barCryLArSD;
      //LArG4CalibSD* m_barCryMixSD;
      //LArG4CalibSD* m_DMSD;
      //LArG4CalibSD* m_barPresSD;
      //LArG4CalibSD* m_barSD;
      //LArG4CalibSD* m_ECCrySD;
      //LArG4CalibSD* m_ECCryLArSD;
      //LArG4CalibSD* m_ECCryMixSD;
      //LArG4CalibSD* m_ECSupportSD;
      //LArG4CalibSD* m_HECWheelSD;
      //LArG4CalibSD* m_uninstSD;

      ServiceHandle<ILArCalibCalculatorSvc> m_embccalc; //BarrelCryostat::CalibrationCalculator()
      ServiceHandle<ILArCalibCalculatorSvc> m_embclarcalc; //BarrelCryostat::CalibrationLArCalculator()
      ServiceHandle<ILArCalibCalculatorSvc> m_mixcalc; //BarrelCryostat::CalibrationMixedCalculator()
      ServiceHandle<ILArCalibCalculatorSvc> m_dmcalc; //DM::CalibrationCalculator()
      ServiceHandle<ILArCalibCalculatorSvc> m_embpscalc; //BarrelPresampler::CalibrationCalculator()
      ServiceHandle<ILArCalibCalculatorSvc> m_embcalc; //Barrel::CalibrationCalculator()
      ServiceHandle<ILArCalibCalculatorSvc> m_emeccalc; //EndcapCryostat::CalibrationCalculator()
      ServiceHandle<ILArCalibCalculatorSvc> m_emecclarcalc; //EndcapCryostat::CalibrationLArCalculator()
      ServiceHandle<ILArCalibCalculatorSvc> m_ememixcalc; //EndcapCryostat::CalibrationMixedCalculator()
      ServiceHandle<ILArCalibCalculatorSvc> m_emesupcalc; //EMECSupportCalibrationCalculator()
      ServiceHandle<ILArCalibCalculatorSvc> m_heccalc; //HEC::LArHECCalibrationWheelCalculator(HEC::kWheelDead)
      ServiceHandle<ILArCalibCalculatorSvc> m_defcalc; //CalibrationDefaultCalculator()

  };

} // namespace LArG4

#endif
