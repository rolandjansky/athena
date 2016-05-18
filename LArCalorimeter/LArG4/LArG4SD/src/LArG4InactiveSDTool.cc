/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4InactiveSDTool.h"

#include "LArG4Code/LArG4CalibSD.h"

#include "LArG4Code/LArG4Hit.h"
#include "CxxUtils/make_unique.h"

// All the calculators that I need
#include "LArG4Barrel/LArBarrelCalibrationCalculator.h"
#include "LArG4Barrel/PresamplerCalibrationCalculator.h"
#include "LArG4EC/CalibrationCalculator.h"
#include "LArG4HEC/LArHECCalibrationWheelCalculator.h"
#include "LArG4FCAL/LArFCAL1CalibCalculator.h"
#include "LArG4FCAL/LArFCAL2CalibCalculator.h"
#include "LArG4FCAL/LArFCAL3CalibCalculator.h"
#include "LArG4MiniFCAL/MiniFCALCalibrationCalculator.h"


LArG4InactiveSDTool::LArG4InactiveSDTool(const std::string& type, const std::string& name,
                                         const IInterface *parent)
  : LArG4SDTool(type,name,parent)
  , m_HitColl("LArCalibrationHitInactive")
  , m_barPreSD(nullptr)
  , m_barSD(nullptr)
  , m_ECPosInSD(nullptr)
  , m_ECPosOutSD(nullptr)
  , m_ECNegInSD(nullptr)
  , m_ECNegOutSD(nullptr)
  , m_HECWheelSD(nullptr)
  , m_fcal1SD(nullptr)
  , m_fcal2SD(nullptr)
  , m_fcal3SD(nullptr)
  , m_miniMomSD(nullptr)
  , m_miniSD(nullptr)
  , m_miniLaySD(nullptr)
{
  declareProperty("BarrelPreVolumes",m_barPreVolumes);
  declareProperty("BarrelVolumes",m_barVolumes);
  declareProperty("ECPosInVolumes",m_ECPosInVolumes);
  declareProperty("ECPosOutVolumes",m_ECPosOutVolumes);
  declareProperty("ECNegInVolumes",m_ECNegInVolumes);
  declareProperty("ECNegOutVolumes",m_ECNegOutVolumes);
  //declareProperty("HECVolumes",m_HECVolumes={"LAr::HEC::Inactive"});
  //declareProperty("HECLocalVolumes",m_HECLocVolumes={"LAr::HEC::Local::Inactive"});
  declareProperty("HECWheelVolumes",m_HECWheelVolumes);
  declareProperty("FCAL1Volumes",m_fcal1Volumes);
  declareProperty("FCAL2Volumes",m_fcal2Volumes);
  declareProperty("FCAL3Volumes",m_fcal3Volumes);
  declareProperty("MiniMomVolumes",m_miniMomVolumes);
  declareProperty("MiniVolumes",m_miniVolumes);
  declareProperty("MiniLayVolumes",m_miniLayVolumes);
  declareInterface<ISensitiveDetector>(this);
}

StatusCode LArG4InactiveSDTool::initializeSD()
{
  // Lots of singleton calculators !!!
  m_barPreSD   = new LArG4CalibSD( "LAr::Barrel::Presampler::Inactive" , new LArG4::BarrelPresampler::CalibrationCalculator() , m_doPID );
  m_barSD      = new LArG4CalibSD( "LAr::Barrel::Inactive" , new LArG4::Barrel::CalibrationCalculator() , m_doPID );
  m_ECPosInSD  = new LArG4CalibSD( "LAr::EMEC::Pos::InnerWheel::Inactive" , new LArG4::EC::CalibrationCalculator(LArWheelCalculator::InnerAbsorberWheel, 1) , m_doPID );
  m_ECPosOutSD = new LArG4CalibSD( "LAr::EMEC::Pos::OuterWheel::Inactive" , new LArG4::EC::CalibrationCalculator(LArWheelCalculator::OuterAbsorberWheel, 1) , m_doPID );
  m_ECNegInSD  = new LArG4CalibSD( "LAr::EMEC::Neg::InnerWheel::Inactive" , new LArG4::EC::CalibrationCalculator(LArWheelCalculator::InnerAbsorberWheel, -1) , m_doPID );
  m_ECNegOutSD = new LArG4CalibSD( "LAr::EMEC::Neg::OuterWheel::Inactive" , new LArG4::EC::CalibrationCalculator(LArWheelCalculator::OuterAbsorberWheel, -1) , m_doPID );
  //m_HECSD      = new LArG4CalibSD( "LAr::HEC::Inactive" , new LArG4::HEC::CalibrationCalculator(LArG4::HEC::kInactive) , m_doPID );
  //m_HECLocSD   = new LArG4CalibSD( "LAr::HEC::Local::Inactive" , new LArG4::HEC::LocalCalibrationCalculator(LArG4::HEC::kLocInactive) , m_doPID );
  m_HECWheelSD = new LArG4CalibSD( "LAr::HEC::Wheel::Inactive" , new LArG4::HEC::LArHECCalibrationWheelCalculator(LArG4::HEC::kWheelInactive) , m_doPID );
  m_fcal1SD    = new LArG4CalibSD( "LAr::FCAL::Inactive1" , LArG4::FCAL::LArFCAL1CalibCalculator::GetCalculator() , m_doPID );
  m_fcal2SD    = new LArG4CalibSD( "LAr::FCAL::Inactive2" , LArG4::FCAL::LArFCAL2CalibCalculator::GetCalculator() , m_doPID );
  m_fcal3SD    = new LArG4CalibSD( "LAr::FCAL::Inactive3" , LArG4::FCAL::LArFCAL3CalibCalculator::GetCalculator() , m_doPID );
  m_miniMomSD  = new LArG4CalibSD( "MiniFCALMother::Inactive" , new LArG4::MiniFCAL::MiniFCALCalibrationCalculator(LArG4::MiniFCAL::kInactive) , m_doPID );
  m_miniSD     = new LArG4CalibSD( "MiniFCAL::Inactive" , new LArG4::MiniFCAL::MiniFCALCalibrationCalculator(LArG4::MiniFCAL::kInactive) , m_doPID );
  m_miniLaySD  = new LArG4CalibSD( "MiniFCAL::Layer::Inactive" , new LArG4::MiniFCAL::MiniFCALCalibrationCalculator(LArG4::MiniFCAL::kInactive) , m_doPID );

  std::map<G4VSensitiveDetector*,std::vector<std::string>*> configuration;
  configuration[m_barPreSD]   = &m_barPreVolumes;
  configuration[m_barSD]      = &m_barVolumes;
  configuration[m_ECPosInSD]  = &m_ECPosInVolumes;
  configuration[m_ECPosOutSD] = &m_ECPosOutVolumes;
  configuration[m_ECNegInSD]  = &m_ECNegInVolumes;
  configuration[m_ECNegOutSD] = &m_ECNegOutVolumes;
  //configuration[m_HECSD]      = &m_HECVolumes;
  //configuration[m_HECLocSD]   = &m_HECLocVolumes;
  configuration[m_HECWheelSD] = &m_HECWheelVolumes;
  configuration[m_fcal1SD]    = &m_fcal1Volumes;
  configuration[m_fcal2SD]    = &m_fcal2Volumes;
  configuration[m_fcal3SD]    = &m_fcal3Volumes;
  configuration[m_miniMomSD]  = &m_miniMomVolumes;
  configuration[m_miniSD]     = &m_miniVolumes;
  configuration[m_miniLaySD]  = &m_miniLayVolumes;
  setupAllSDs(configuration);

  // Make sure the ID helpers are all set up
  setupHelpers(m_barPreSD);
  setupHelpers(m_barSD);
  setupHelpers(m_ECPosInSD);
  setupHelpers(m_ECPosOutSD);
  setupHelpers(m_ECNegInSD);
  setupHelpers(m_ECNegOutSD);
  //setupHelpers(m_HECSD);
  //setupHelpers(m_HECLocSD);
  setupHelpers(m_HECWheelSD);
  setupHelpers(m_fcal1SD);
  setupHelpers(m_fcal2SD);
  setupHelpers(m_fcal3SD);
  setupHelpers(m_miniMomSD);
  setupHelpers(m_miniSD);
  setupHelpers(m_miniLaySD);

  return StatusCode::SUCCESS;
}

StatusCode LArG4InactiveSDTool::Gather()
{
  // In this case, *unlike* other SDs, the *tool* owns the collection
  if (!m_HitColl.isValid()) m_HitColl = CxxUtils::make_unique<CaloCalibrationHitContainer>(m_HitColl.name());
  m_barPreSD->EndOfAthenaEvent( &*m_HitColl );
  m_barSD      ->EndOfAthenaEvent( &*m_HitColl );
  m_ECPosInSD  ->EndOfAthenaEvent( &*m_HitColl );
  m_ECPosOutSD ->EndOfAthenaEvent( &*m_HitColl );
  m_ECNegInSD  ->EndOfAthenaEvent( &*m_HitColl );
  m_ECNegOutSD ->EndOfAthenaEvent( &*m_HitColl );
  //m_HECSD      ->EndOfAthenaEvent( &*m_HitColl );
  //m_HECLocSD   ->EndOfAthenaEvent( &*m_HitColl );
  m_HECWheelSD ->EndOfAthenaEvent( &*m_HitColl );
  m_fcal1SD    ->EndOfAthenaEvent( &*m_HitColl );
  m_fcal2SD    ->EndOfAthenaEvent( &*m_HitColl );
  m_fcal3SD    ->EndOfAthenaEvent( &*m_HitColl );
  m_miniMomSD  ->EndOfAthenaEvent( &*m_HitColl );
  m_miniSD     ->EndOfAthenaEvent( &*m_HitColl );
  m_miniLaySD  ->EndOfAthenaEvent( &*m_HitColl );
  return StatusCode::SUCCESS;
}
