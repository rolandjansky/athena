/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4H62004ActiveSDTool.h"

// For the SDs (including all their calculators)
#include "LArG4H62004CalibSD.h"
#include "LArG4EC/CalibrationCalculator.h"
#include "LArG4HEC/LocalCalibrationCalculator.h"
#include "LArFCAL1H62004CalibCalculator.h"
#include "LArFCAL2H62004CalibCalculator.h"
#include "LArG4H6COLDTCMod0CalibCalculator.h"

// For the hit collections
#include "CxxUtils/make_unique.h"

LArG4H62004ActiveSDTool::LArG4H62004ActiveSDTool(const std::string& type, const std::string& name, const IInterface *parent)
  : LArG4SDTool(type,name,parent)
  , m_HitColl("LArCalibrationHitActive")
  , m_emecSD(nullptr)
  , m_hecSD(nullptr)
  , m_fcal1SD(nullptr)
  , m_fcal2SD(nullptr)
  , m_fcalColdSD(nullptr)
{
  declareProperty("EMECVolumes",m_emecVolumes);
  declareProperty("HECVolumes",m_hecVolumes);
  declareProperty("FCAL1Volumes",m_fcal1Volumes);
  declareProperty("FCAL2Volumes",m_fcal2Volumes);
  declareProperty("FCALColdVolumes",m_fcalColdVolumes);
  declareInterface<ISensitiveDetector>(this);
}

StatusCode LArG4H62004ActiveSDTool::initializeSD()
{
  // Lots of singleton calculators !!!
  if (m_emecVolumes.size()>0) m_emecSD = new LArG4H62004CalibSD( "EMEC::InnerModule::Calibration::H6" , new LArG4::EC::CalibrationCalculator(LArWheelCalculator::InnerAbsorberWheel, 1) , m_doPID );
  if (m_hecVolumes.size()>0) m_hecSD = new LArG4H62004CalibSD( "HEC::Module::Depth::Slice::Local::Calibration::H6", new LArG4::HEC::LocalCalibrationCalculator(LArG4::HEC::kLocActive) , m_doPID );
  if (m_fcal1Volumes.size()>0) m_fcal1SD = new LArG4H62004CalibSD( "LAr::FCAL::Module1::Gap::Calibration::H6" , LArFCAL1H62004CalibCalculator::GetCalculator() , m_doPID );
  if (m_fcal2Volumes.size()>0) m_fcal2SD = new LArG4H62004CalibSD( "LAr::FCAL::Module2::Gap::Calibration::H6" , LArFCAL2H62004CalibCalculator::GetCalculator() , m_doPID );
  if (m_fcalColdVolumes.size()>0) m_fcalColdSD = new LArG4H62004CalibSD( "LAr::FCAL::ColdTC::Gap::Calibration::H6" , new LArG4H6COLDTCMod0CalibCalculator() , m_doPID );

  std::map<G4VSensitiveDetector*,std::vector<std::string>*> configuration;
  if (m_emecVolumes.size()>0) configuration[m_emecSD]  = &m_emecVolumes;
  if (m_hecVolumes.size()>0)  configuration[m_hecSD]   = &m_hecVolumes;
  if (m_fcal1Volumes.size()>0)  configuration[m_fcal1SD]   = &m_fcal1Volumes;
  if (m_fcal2Volumes.size()>0)  configuration[m_fcal2SD]   = &m_fcal2Volumes;
  if (m_fcalColdVolumes.size()>0)  configuration[m_fcalColdSD]   = &m_fcalColdVolumes;
  setupAllSDs(configuration);

  // make sure they have the identifiers they need
  if (m_emecVolumes.size()>0) setupHelpers(m_emecSD);
  if (m_hecVolumes.size()>0)  setupHelpers(m_hecSD);
  if (m_fcal1Volumes.size()>0) setupHelpers(m_fcal1SD);
  if (m_fcal2Volumes.size()>0) setupHelpers(m_fcal2SD);
  if (m_fcalColdVolumes.size()>0) setupHelpers(m_fcalColdSD);

  return StatusCode::SUCCESS;
}

StatusCode LArG4H62004ActiveSDTool::Gather()
{
  // In this case, *unlike* other SDs, the *tool* owns the collection
  if (!m_HitColl.isValid()) m_HitColl = CxxUtils::make_unique<CaloCalibrationHitContainer>(m_HitColl.name());
  if (m_emecVolumes.size()>0) m_emecSD ->EndOfAthenaEvent( &*m_HitColl );
  if (m_hecVolumes.size()>0)  m_hecSD  ->EndOfAthenaEvent( &*m_HitColl );
  if (m_fcal1Volumes.size()>0) m_fcal1SD ->EndOfAthenaEvent( &*m_HitColl );
  if (m_fcal2Volumes.size()>0) m_fcal2SD ->EndOfAthenaEvent( &*m_HitColl );
  if (m_fcalColdVolumes.size()>0) m_fcalColdSD ->EndOfAthenaEvent( &*m_HitColl );
  return StatusCode::SUCCESS;
}
