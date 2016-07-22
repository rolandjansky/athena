/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "H8CalibSDTool.h"

// LArG4 includes
#include "LArG4Code/SDWrapper.h"
//#include "LArG4Code/VCalibrationCalculator.h"
#include "LArG4Code/EscapedEnergyProcessing.h"
//#include "LArG4Code/CalibrationDefaultCalculator.h"

//#include "CaloG4Sim/VEscapedEnergyProcessing.h"
#include "CaloG4Sim/EscapedEnergyRegistry.h"

#include "LArG4Barrel/PresamplerCalibrationCalculator.h"
#include "LArG4Barrel/LArBarrelCalibrationCalculator.h"

// Local includes
#include "LArTBCryostatCalibrationCalculator.h"
#include "H8CalibrationDefaultCalculator.h"

namespace LArG4
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  H8CalibSDTool::H8CalibSDTool(const std::string& type, const std::string& name,
                               const IInterface* parent)
    : CalibSDTool(type, name, parent),
      m_hitCollName("LArH8CalibrationHits")
  {
    declareProperty("BarrelCryDeadVolumes", m_barCryVolumes);
    declareProperty("BarrelPreInactiveVolumes", m_bpInVolumes);
    declareProperty("BarrelPreDeadVolumes", m_bpDeadVolumes);
    declareProperty("BarrelPreCalibVolumes", m_bpCalibVolumes);
    declareProperty("BarrelInVolumes", m_barInVolumes);
    declareProperty("BarrelDeadVolumes", m_barDeadVolumes);
  }

  //---------------------------------------------------------------------------
  // Create SD wrapper for current thread
  //---------------------------------------------------------------------------
  G4VSensitiveDetector* H8CalibSDTool::makeSD()
  {
    // Create the wrapper
    auto sdWrapper = new CalibSDWrapper("H8CalibSDWrapper", m_hitCollName);

    // Create the SDs.
    // Lots of singleton calculators !!!

    sdWrapper->addSD(
      makeOneSD("LAr::TBBarrelCryostat::Dead",
                new LArTBCryostatCalibrationCalculator(),
                m_barCryVolumes)
    );

    sdWrapper->addSD(
      makeOneSD("LAr::TBBarrel::Presampler::Inactive",
                new LArG4::BarrelPresampler::CalibrationCalculator(),
                m_bpInVolumes)
    );

    sdWrapper->addSD(
      makeOneSD("LAr::TBBarrel::Presampler::Dead",
                new LArG4::BarrelPresampler::CalibrationCalculator(),
                m_bpDeadVolumes)
    );

    // Why is this one commented out?
    //sdWrapper->addSD(
    //  makeOneSD("LAr::TBBarrel::Presampler::Module::Calibration",
    //    new LArG4::BarrelPresampler::CalibrationCalculator(),
    //        m_bpModVolumes)
    //);

    sdWrapper->addSD(
      makeOneSD("LAr::TBBarrel::Inactive",
                new LArG4::Barrel::CalibrationCalculator(),
                m_barInVolumes)
    );

    sdWrapper->addSD(
      makeOneSD("LAr::TBBarrel::Dead",
                new LArG4::Barrel::CalibrationCalculator(),
                m_barDeadVolumes)

    );

    std::vector<std::string> emptyStringVec;
    auto uninstSD =
      makeOneSD("Default::Dead::Uninstrumented::Calibration::Region",
                new H8CalibrationDefaultCalculator(),
                emptyStringVec);
    // WARNING: This probably isn't thread safe!
    CaloG4::VEscapedEnergyProcessing* eep =
      new EscapedEnergyProcessing( uninstSD.get() );
    auto registry = CaloG4::EscapedEnergyRegistry::GetInstance();
    registry->AddAndAdoptProcessing("LAr::", eep);
    sdWrapper->addSD( std::move(uninstSD) );

    return sdWrapper;
  }

} // namespace LArG4
