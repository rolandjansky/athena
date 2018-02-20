/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4H62004FCALSDTool.h"
#include "LArG4H62004SD.h"

#include "CxxUtils/make_unique.h"

LArG4H62004FCALSDTool::LArG4H62004FCALSDTool(const std::string& type, const std::string& name, const IInterface *parent)
  : LArG4SDTool(type,name,parent)
  , m_HitColl("LArHitFCAL")
  , m_fcal1calc("FCAL1Calculator", name)
  , m_fcal2calc("FCAL2Calculator", name)
  , m_fcalcoldcalc("FCALColdCalculator", name)
  , m_fcal1SD(nullptr)
  , m_fcal2SD(nullptr)
  , m_fcalColdSD(nullptr)
{
  declareProperty("FCAL1Calculator", m_fcal1calc);
  declareProperty("FCAL2Calculator", m_fcal2calc);
  declareProperty("FCALColdCalculator", m_fcalcoldcalc);
  declareProperty( "FCAL1Volumes" , m_fcal1Volumes );
  declareProperty( "FCAL2Volumes" , m_fcal2Volumes );
  declareProperty( "FCALColdVolumes" , m_fcalColdVolumes );
}

StatusCode LArG4H62004FCALSDTool::initializeCalculators()
{
  ATH_CHECK(m_fcal1calc.retrieve());
  ATH_CHECK(m_fcal2calc.retrieve());
  ATH_CHECK(m_fcalcoldcalc.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode LArG4H62004FCALSDTool::initializeSD()
{
  m_fcal1SD = new LArG4H62004SD( "LAr::FCAL::Module1::Gap::H6" , &*m_fcal1calc , m_timeBinType , m_timeBinWidth );
  m_fcal2SD = new LArG4H62004SD( "LAr::FCAL::Module2::Gap::H6" , &*m_fcal2calc , m_timeBinType , m_timeBinWidth );
  m_fcalColdSD = new LArG4H62004SD( "LAr::FCAL::ColdTC::H6" , &*m_fcalcoldcalc , m_timeBinType , m_timeBinWidth );

  std::map<G4VSensitiveDetector*,std::vector<std::string>*> configuration;
  configuration[m_fcal1SD] = &m_fcal1Volumes;
  configuration[m_fcal2SD] = &m_fcal2Volumes;
  configuration[m_fcalColdSD] = &m_fcalColdVolumes;
  setupAllSDs(configuration);

  // Make sure the ID helpers are all set up
  setupHelpers(m_fcal1SD);
  setupHelpers(m_fcal2SD);
  setupHelpers(m_fcalColdSD);

  return StatusCode::SUCCESS;
}

StatusCode LArG4H62004FCALSDTool::Gather()
{
  // In this case, *unlike* other SDs, the *tool* owns the collection
  if (!m_HitColl.isValid()) m_HitColl = CxxUtils::make_unique<LArHitContainer>(m_HitColl.name());
  // Hand this collection name off to the SDs.  They will be writing to the
  // collection, but only one at a time!
  m_fcal1SD->EndOfAthenaEvent( &*m_HitColl );
  m_fcal2SD->EndOfAthenaEvent( &*m_HitColl );
  m_fcalColdSD->EndOfAthenaEvent( &*m_HitColl );
  return StatusCode::SUCCESS;
}
