/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InactiveSDTool.h"

#include "LArG4Code/SDWrapper.h"

// All the calculators I need
#include "LArG4Barrel/LArBarrelCalibrationCalculator.h"
#include "LArG4Barrel/PresamplerCalibrationCalculator.h"
#include "LArG4EC/PresamplerCalibrationCalculator.h"
#include "LArG4EC/CalibrationCalculator.h"
#include "LArG4EC/PresamplerCalibrationCalculator.h"
#include "LArG4FCAL/LArFCAL1CalibCalculator.h"
#include "LArG4FCAL/LArFCAL2CalibCalculator.h"
#include "LArG4FCAL/LArFCAL3CalibCalculator.h"
#include "LArG4MiniFCAL/MiniFCALCalibrationCalculator.h"
#include "LArG4HEC/LArHECCalibrationWheelCalculator.h"

namespace LArG4
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  InactiveSDTool::InactiveSDTool(const std::string& type, const std::string& name,
                                 const IInterface *parent)
    : CalibSDTool(type, name, parent),
      m_hitCollName("LArCalibrationHitInactive")
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
  }

  //---------------------------------------------------------------------------
  // Create SD wrapper for current thread
  //---------------------------------------------------------------------------
  G4VSensitiveDetector* InactiveSDTool::makeSD()
  {
    // Create the wrapper
    auto sdWrapper = new CalibSDWrapper("LArInactiveSDWrapper", m_hitCollName);

    sdWrapper->addSD(
      makeOneSD("LAr::Barrel::Presampler::Inactive",
                new BarrelPresampler::CalibrationCalculator(),
                m_barPreVolumes)
    );
    sdWrapper->addSD(
      makeOneSD("LAr::Barrel::Inactive",
                new Barrel::CalibrationCalculator(),
                m_barVolumes)
    );
    sdWrapper->addSD(
      makeOneSD("LAr::EMEC::Pos::InnerWheel::Inactive",
                new EC::CalibrationCalculator(LArWheelCalculator::InnerAbsorberWheel, 1),
                m_ECPosInVolumes)
    );
    sdWrapper->addSD(
      makeOneSD("LAr::EMEC::Pos::OuterWheel::Inactive",
                new EC::CalibrationCalculator(LArWheelCalculator::OuterAbsorberWheel, 1),
                m_ECPosOutVolumes)
    );
    sdWrapper->addSD(
      makeOneSD("LAr::EMEC::Neg::InnerWheel::Inactive",
                new EC::CalibrationCalculator(LArWheelCalculator::InnerAbsorberWheel, -1),
                m_ECNegInVolumes)
    );
    sdWrapper->addSD(
      makeOneSD("LAr::EMEC::Neg::OuterWheel::Inactive",
                new EC::CalibrationCalculator(LArWheelCalculator::OuterAbsorberWheel, -1),
                m_ECNegOutVolumes)
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
      makeOneSD("LAr::HEC::Wheel::Inactive",
                new HEC::LArHECCalibrationWheelCalculator(HEC::kWheelInactive),
                m_HECWheelVolumes)
    );
    sdWrapper->addSD(
      makeOneSD("LAr::FCAL::Inactive1",
                FCAL::LArFCAL1CalibCalculator::GetCalculator(),
                m_fcal1Volumes)
    );
    sdWrapper->addSD(
      makeOneSD("LAr::FCAL::Inactive2",
                FCAL::LArFCAL2CalibCalculator::GetCalculator(),
                m_fcal2Volumes)
    );
    sdWrapper->addSD(
      makeOneSD("LAr::FCAL::Inactive3",
                FCAL::LArFCAL3CalibCalculator::GetCalculator(),
                m_fcal3Volumes)
    );
    sdWrapper->addSD(
      makeOneSD("MiniFCALMother::Inactive",
                new MiniFCAL::MiniFCALCalibrationCalculator(MiniFCAL::kInactive),
                m_miniMomVolumes)
    );
    sdWrapper->addSD(
      makeOneSD("MiniFCAL::Inactive",
                new MiniFCAL::MiniFCALCalibrationCalculator(MiniFCAL::kInactive),
                m_miniVolumes)
    );
    sdWrapper->addSD(
      makeOneSD("MiniFCAL::Layer::Inactive",
                new MiniFCAL::MiniFCALCalibrationCalculator(MiniFCAL::kInactive),
                m_miniLayVolumes)
    );

    // Return the wrapper as my SD
    return sdWrapper;
  }

} // namespace LArG4
