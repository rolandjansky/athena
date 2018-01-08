/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4H62004InactiveSDTool.h"

#include "LArG4H62004CalibSD.h"

#include "CxxUtils/make_unique.h"

LArG4H62004InactiveSDTool::LArG4H62004InactiveSDTool(const std::string& type, const std::string& name, const IInterface *parent)
  : LArG4SDTool(type,name,parent)
  , m_HitColl("LArCalibrationHitInactive")
  , m_emepiwcalc("EMECPosInnerWheelCalibrationCalculator", name)
  , m_heccalc("LocalCalibrationInactiveCalculator", name)
  , m_fcal1calc("LArFCAL1H62004CalibCalculator", name)
  , m_fcal2calc("LArFCAL2H62004CalibCalculator", name)
  , m_emecSD(nullptr)
  , m_hecSD(nullptr)
  , m_fcal1SD(nullptr)
  , m_fcal2SD(nullptr)
{
  declareProperty("EMECVolumes",m_emecVolumes);
  declareProperty("HECVolumes",m_hecVolumes);
  declareProperty("FCAL1Volumes",m_fcal1Volumes);
  declareProperty("FCAL2Volumes",m_fcal2Volumes);
  declareProperty("EMECPosIWCalibrationCalculator", m_emepiwcalc);
  declareProperty("HECWheelInactiveCalculator", m_heccalc);
  declareProperty("FCAL1CalibCalculator", m_fcal1calc);
  declareProperty("FCAL2CalibCalculator", m_fcal2calc);
}

StatusCode LArG4H62004InactiveSDTool::initializeCalculators()
{
  ATH_CHECK(m_emepiwcalc.retrieve());
  ATH_CHECK(m_heccalc.retrieve());
  ATH_CHECK(m_fcal1calc.retrieve());
  ATH_CHECK(m_fcal2calc.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode LArG4H62004InactiveSDTool::initializeSD()
{
  // Setup calculator and collection
  if (m_emecVolumes.size()>0) m_emecSD  = new LArG4H62004CalibSD( "LAr::EMEC::InnerModule::Inactive::H6" , &*m_emepiwcalc , m_doPID );
  if (m_hecVolumes.size()>0)  m_hecSD  = new LArG4H62004CalibSD( "LAr::HEC::Local::Inactive::H6" , &*m_heccalc , m_doPID );
  if (m_fcal1Volumes.size()>0) m_fcal1SD  = new LArG4H62004CalibSD( "LAr::FCAL::Inactive1::H6" , &*m_fcal1calc , m_doPID );
  if (m_fcal2Volumes.size()>0) m_fcal2SD  = new LArG4H62004CalibSD( "LAr::FCAL::Inactive2::H6" , &*m_fcal2calc , m_doPID );

  std::map<G4VSensitiveDetector*,std::vector<std::string>*> configuration;
  if (m_emecVolumes.size()>0) configuration[m_emecSD]  = &m_emecVolumes;
  if (m_hecVolumes.size()>0)  configuration[m_hecSD]   = &m_hecVolumes;
  if (m_fcal1Volumes.size()>0) configuration[m_fcal1SD]  = &m_fcal1Volumes;
  if (m_fcal2Volumes.size()>0) configuration[m_fcal2SD]  = &m_fcal2Volumes;
  setupAllSDs(configuration);

  // make sure they have the identifiers they need
  if (m_emecVolumes.size()>0) setupHelpers(m_emecSD);
  if (m_hecVolumes.size()>0)  setupHelpers(m_hecSD);
  if (m_fcal1Volumes.size()>0) setupHelpers(m_fcal1SD);
  if (m_fcal2Volumes.size()>0) setupHelpers(m_fcal2SD);

  return StatusCode::SUCCESS;
}

StatusCode LArG4H62004InactiveSDTool::Gather()
{
  // In this case, *unlike* other SDs, the *tool* owns the collection
  if (!m_HitColl.isValid()) m_HitColl = CxxUtils::make_unique<CaloCalibrationHitContainer>(m_HitColl.name());
  if (m_emecVolumes.size()>0) m_emecSD ->EndOfAthenaEvent( &*m_HitColl );
  if (m_hecVolumes.size()>0)  m_hecSD  ->EndOfAthenaEvent( &*m_HitColl );
  if (m_fcal1Volumes.size()>0) m_fcal1SD ->EndOfAthenaEvent( &*m_HitColl );
  if (m_fcal2Volumes.size()>0) m_fcal2SD ->EndOfAthenaEvent( &*m_HitColl );
  return StatusCode::SUCCESS;
}
