/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4ActiveSDTool.h"

#include "LArG4Code/LArG4CalibSD.h"

#include "LArG4Code/LArG4Hit.h"
#include "CxxUtils/make_unique.h"

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

LArG4ActiveSDTool::LArG4ActiveSDTool(const std::string& type, const std::string& name,
                                     const IInterface *parent)
  : LArG4SDTool(type,name,parent)
  , m_HitColl("LArCalibrationHitActive")
  , m_stacSD(nullptr)
  , m_presBarSD(nullptr)
  , m_posIWSD(nullptr)
  , m_negIWSD(nullptr)
  , m_posOWSD(nullptr)
  , m_negOWSD(nullptr)
  , m_presECSD(nullptr)
  , m_bobSD(nullptr)
  , m_fcal1SD(nullptr)
  , m_fcal2SD(nullptr)
  , m_fcal3SD(nullptr)
  , m_sliceSD(nullptr)
  , m_miniSD(nullptr)
{
  declareProperty("StacVolumes",m_stacVolumes);
  declareProperty("PresamplerVolumes",m_presBarVolumes);
  declareProperty("PosIWVolumes",m_posIWVolumes);
  declareProperty("NegIWVolumes",m_negIWVolumes);
  declareProperty("PosOWVolumes",m_posOWVolumes);
  declareProperty("NegOWVolumes",m_negOWVolumes);
  declareProperty("PresVolumes",m_presECVolumes);
  declareProperty("BOBarretteVolumes",m_bobVolumes);
  declareProperty("FCAL1Volumes",m_fcal1Volumes);
  declareProperty("FCAL2Volumes",m_fcal2Volumes);
  declareProperty("FCAL3Volumes",m_fcal3Volumes);
  declareProperty("SliceVolumes",m_sliceVolumes);
  declareProperty("MiniVolumes",m_miniVolumes);

  declareInterface<ISensitiveDetector>(this);
}

StatusCode LArG4ActiveSDTool::initializeSD()
{
  // Lots of singleton calculators !!!
  m_presBarSD = new LArG4CalibSD( "Barrel::Presampler::Module::Calibration" , new LArG4::BarrelPresampler::CalibrationCalculator() , m_doPID );
  m_stacSD = new LArG4CalibSD( "EMB::STAC::Calibration" , new LArG4::Barrel::CalibrationCalculator() , m_doPID );
  m_posIWSD = new LArG4CalibSD( "EMEC::Pos::InnerWheel::Calibration" , new LArG4::EC::CalibrationCalculator(LArWheelCalculator::InnerAbsorberWheel, 1) , m_doPID );
  m_negIWSD = new LArG4CalibSD( "EMEC::Neg::InnerWheel::Calibration" , new LArG4::EC::CalibrationCalculator(LArWheelCalculator::InnerAbsorberWheel, -1) , m_doPID );
  m_posOWSD = new LArG4CalibSD( "EMEC::Pos::OuterWheel::Calibration" , new LArG4::EC::CalibrationCalculator(LArWheelCalculator::OuterAbsorberWheel, 1) , m_doPID );
  m_negOWSD = new LArG4CalibSD( "EMEC::Neg::OuterWheel::Calibration" , new LArG4::EC::CalibrationCalculator(LArWheelCalculator::OuterAbsorberWheel, -1) , m_doPID );
  m_presECSD  = new LArG4CalibSD( "Endcap::Presampler::LiquidArgon::Calibration" , new LArG4::EC::PresamplerCalibrationCalculator() , m_doPID );
  m_bobSD   = new LArG4CalibSD( "EMEC::BackOuterBarrette::Calibration", new LArG4::EC::CalibrationCalculator(LArWheelCalculator::BackOuterBarretteWheelCalib, 1) , m_doPID );
  m_fcal1SD = new LArG4CalibSD( "FCAL::Module1::Gap::Calibration" , LArG4::FCAL::LArFCAL1CalibCalculator::GetCalculator() , m_doPID );
  m_fcal2SD = new LArG4CalibSD( "FCAL::Module2::Gap::Calibration" , LArG4::FCAL::LArFCAL2CalibCalculator::GetCalculator() , m_doPID );
  m_fcal3SD = new LArG4CalibSD( "FCAL::Module3::Gap::Calibration" , LArG4::FCAL::LArFCAL3CalibCalculator::GetCalculator() , m_doPID );
  m_sliceSD = new LArG4CalibSD( "HEC::Module::Depth::Slice::Wheel::Calibration" , new LArG4::HEC::LArHECCalibrationWheelCalculator(LArG4::HEC::kWheelActive) , m_doPID );
  m_miniSD = new LArG4CalibSD( "MiniFCAL::Wafer" , new LArG4::MiniFCAL::MiniFCALCalibrationCalculator(LArG4::MiniFCAL::kActive) , m_doPID );

  std::map<G4VSensitiveDetector*,std::vector<std::string>*> configuration;
  configuration[m_presBarSD] = &m_presBarVolumes;
  configuration[m_stacSD]    = &m_stacVolumes;
  configuration[m_posIWSD]   = &m_posIWVolumes;
  configuration[m_posOWSD]   = &m_posOWVolumes;
  configuration[m_negIWSD]   = &m_negIWVolumes;
  configuration[m_negOWSD]   = &m_negOWVolumes;
  configuration[m_presECSD]  = &m_presECVolumes;
  configuration[m_bobSD]     = &m_bobVolumes;
  configuration[m_fcal1SD]   = &m_fcal1Volumes;
  configuration[m_fcal2SD]   = &m_fcal2Volumes;
  configuration[m_fcal3SD]   = &m_fcal3Volumes;
  configuration[m_sliceSD]   = &m_sliceVolumes;
  configuration[m_miniSD]    = &m_miniVolumes;
  setupAllSDs(configuration);

  // make sure they have the identifiers they need
  setupHelpers(m_presBarSD);
  setupHelpers(m_stacSD);
  setupHelpers(m_posIWSD);
  setupHelpers(m_posOWSD);
  setupHelpers(m_negIWSD);
  setupHelpers(m_negOWSD);
  setupHelpers(m_presECSD);
  setupHelpers(m_bobSD);
  setupHelpers(m_fcal1SD);
  setupHelpers(m_fcal2SD);
  setupHelpers(m_fcal3SD);
  setupHelpers(m_sliceSD);
  setupHelpers(m_miniSD);

  return StatusCode::SUCCESS;
}

StatusCode LArG4ActiveSDTool::Gather()
{
  // In this case, *unlike* other SDs, the *tool* owns the collection
  if (!m_HitColl.isValid()) m_HitColl = CxxUtils::make_unique<CaloCalibrationHitContainer>(m_HitColl.name());
  m_presBarSD->EndOfAthenaEvent( &*m_HitColl );
  m_stacSD   ->EndOfAthenaEvent( &*m_HitColl );
  m_posIWSD  ->EndOfAthenaEvent( &*m_HitColl );
  m_negIWSD  ->EndOfAthenaEvent( &*m_HitColl );
  m_posOWSD  ->EndOfAthenaEvent( &*m_HitColl );
  m_negOWSD  ->EndOfAthenaEvent( &*m_HitColl );
  m_presECSD ->EndOfAthenaEvent( &*m_HitColl );
  m_bobSD    ->EndOfAthenaEvent( &*m_HitColl );
  m_fcal1SD  ->EndOfAthenaEvent( &*m_HitColl );
  m_fcal2SD  ->EndOfAthenaEvent( &*m_HitColl );
  m_fcal3SD  ->EndOfAthenaEvent( &*m_HitColl );
  m_sliceSD  ->EndOfAthenaEvent( &*m_HitColl );
  m_miniSD   ->EndOfAthenaEvent( &*m_HitColl );
  return StatusCode::SUCCESS;
}
