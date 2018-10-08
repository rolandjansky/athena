/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InactiveSDTool.h"

#include "LArG4Code/SDWrapper.h"


namespace LArG4
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  InactiveSDTool::InactiveSDTool(const std::string& type, const std::string& name,
                                 const IInterface *parent)
    : CalibSDTool(type, name, parent)
    , m_hitCollName("LArCalibrationHitInactive")
    , m_embpscalc("BarrelPresamplerCalibrationCalculator", name)
    , m_embcalc("BarrelCalibrationCalculator", name)
    , m_emepiwcalc("EMECPosInnerWheelCalibrationCalculator", name)
    , m_emeniwcalc("EMECNegInnerWheelCalibrationCalculator", name)
    , m_emepowcalc("EMECPosOuterWheelCalibrationCalculator", name)
    , m_emenowcalc("EMECNegOuterWheelCalibrationCalculator", name)
    , m_heccalc("HECCalibrationWheelInactiveCalculator", name)
    , m_fcal1calc("FCAL1CalibCalculator", name)
    , m_fcal2calc("FCAL2CalibCalculator", name)
    , m_fcal3calc("FCAL3CalibCalculator", name)
    , m_minfcalcalc("MiniFCALInactiveCalibrationCalculator", name)
  {
    declareProperty("HitCollectionName", m_hitCollName);
    declareProperty("BarrelPreVolumes", m_barPreVolumes);
    declareProperty("BarrelVolumes", m_barVolumes);
    declareProperty("ECPosInVolumes", m_ECPosInVolumes);
    declareProperty("ECPosOutVolumes", m_ECPosOutVolumes);
    declareProperty("ECNegInVolumes", m_ECNegInVolumes);
    declareProperty("ECNegOutVolumes", m_ECNegOutVolumes);
    //declareProperty("HECVolumes", m_HECVolumes={"LAr::HEC::Inactive"});
    //declareProperty("HECLocalVolumes", m_HECLocVolumes={"LAr::HEC::Local::Inactive"});
    declareProperty("HECWheelVolumes", m_HECWheelVolumes);
    declareProperty("FCAL1Volumes", m_fcal1Volumes);
    declareProperty("FCAL2Volumes", m_fcal2Volumes);
    declareProperty("FCAL3Volumes", m_fcal3Volumes);
    declareProperty("MiniMomVolumes", m_miniMomVolumes);
    declareProperty("MiniVolumes", m_miniVolumes);
    declareProperty("MiniLayVolumes", m_miniLayVolumes);

    declareProperty("EMBPSCalibrationCalculator", m_embpscalc);
    declareProperty("EMBCalibrationCalculator", m_embcalc);
    declareProperty("EMECPosIWCalibrationCalculator", m_emepiwcalc);
    declareProperty("EMECNegIWCalibrationCalculator", m_emeniwcalc);
    declareProperty("EMECPosOWCalibrationCalculator", m_emepowcalc);
    declareProperty("EMECNegOWCalibrationCalculator", m_emenowcalc);
    declareProperty("HECWheelInactiveCalculator", m_heccalc);
    declareProperty("FCAL1CalibCalculator", m_fcal1calc);
    declareProperty("FCAL2CalibCalculator", m_fcal2calc);
    declareProperty("FCAL3CalibCalculator", m_fcal3calc);
    declareProperty("MiniFCALInactiveCalibrationCalculator", m_minfcalcalc);
  }

  //---------------------------------------------------------------------------
  // Initialization of Athena-components
  //---------------------------------------------------------------------------
  StatusCode InactiveSDTool::initializeCalculators()
  {
    ATH_CHECK(m_embpscalc.retrieve());
    ATH_CHECK(m_embcalc.retrieve());
    ATH_CHECK(m_emepiwcalc.retrieve());
    ATH_CHECK(m_emepowcalc.retrieve());
    ATH_CHECK(m_emeniwcalc.retrieve());
    ATH_CHECK(m_emenowcalc.retrieve());
    ATH_CHECK(m_heccalc.retrieve());
    ATH_CHECK(m_fcal1calc.retrieve());
    ATH_CHECK(m_fcal2calc.retrieve());
    ATH_CHECK(m_fcal3calc.retrieve());
    ATH_CHECK(m_minfcalcalc.retrieve());

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Create SD wrapper for current thread
  //---------------------------------------------------------------------------
  G4VSensitiveDetector* InactiveSDTool::makeSD()
  {
    const std::string deadHitCollName=m_hitCollName+"_DEAD";
    // Create the wrapper
    auto sdWrapper = new CalibSDWrapper("LArInactiveSDWrapper", m_hitCollName, deadHitCollName);

    sdWrapper->addSD(
      makeOneSD("LAr::Barrel::Presampler::Inactive", &*m_embpscalc, m_barPreVolumes)
    );
    sdWrapper->addSD(
      makeOneSD("LAr::Barrel::Inactive", &*m_embcalc, m_barVolumes)
    );
    sdWrapper->addSD(
      makeOneSD("LAr::EMEC::Pos::InnerWheel::Inactive", &*m_emepiwcalc, m_ECPosInVolumes)
    );
    sdWrapper->addSD(
      makeOneSD("LAr::EMEC::Pos::OuterWheel::Inactive", &*m_emepowcalc, m_ECPosOutVolumes)
    );
    sdWrapper->addSD(
      makeOneSD("LAr::EMEC::Neg::InnerWheel::Inactive", &*m_emeniwcalc, m_ECNegInVolumes)
    );
    sdWrapper->addSD(
      makeOneSD("LAr::EMEC::Neg::OuterWheel::Inactive", &*m_emenowcalc, m_ECNegOutVolumes)
    );

    //sdWrapper->addSD(
    //  makeOneSD("LAr::HEC::Inactive",
    //            new HEC::CalibrationCalculator(HEC::kInactive),
    //            m_HECVolumes)
    //);

    //sdWrapper->addSD(
    //  makeOneSD("LAr::HEC::Local::Inactive",
    //            new HEC::LocalCalibrationCalculator(HEC::kLocInactive),
    //            m_HECLocVolumes)
    //);

    sdWrapper->addSD(
      makeOneSD("LAr::HEC::Wheel::Inactive", &*m_heccalc, m_HECWheelVolumes)
    );
    sdWrapper->addSD(
      makeOneSD("LAr::FCAL::Inactive1", &*m_fcal1calc, m_fcal1Volumes)
    );
    sdWrapper->addSD(
      makeOneSD("LAr::FCAL::Inactive2", &*m_fcal2calc, m_fcal2Volumes)
    );
    sdWrapper->addSD(
      makeOneSD("LAr::FCAL::Inactive3", &*m_fcal3calc, m_fcal3Volumes)
    );
    sdWrapper->addSD(
      makeOneSD("MiniFCALMother::Inactive", &*m_minfcalcalc, m_miniMomVolumes)
    );
    sdWrapper->addSD(
      makeOneSD("MiniFCAL::Inactive", &*m_minfcalcalc, m_miniVolumes)
    );
    sdWrapper->addSD(
      makeOneSD("MiniFCAL::Layer::Inactive", &*m_minfcalcalc, m_miniLayVolumes)
    );

    // Return the wrapper as my SD
    return sdWrapper;
  }

} // namespace LArG4
