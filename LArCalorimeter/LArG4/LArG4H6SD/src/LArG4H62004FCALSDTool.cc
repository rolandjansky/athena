/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4H62004FCALSDTool.h"
#include "LArG4H62004SD.h"

#include "LArG4FCAL/LArFCAL1Calculator.h"
#include "LArG4FCAL/LArFCAL2Calculator.h"
#include "LArG4H6COLDTCMod0Calculator.h"

#include "CxxUtils/make_unique.h"

LArG4H62004FCALSDTool::LArG4H62004FCALSDTool(const std::string& type, const std::string& name, const IInterface *parent)
  : LArG4SDTool(type,name,parent)
  , m_HitColl("LArHitFCAL")
  , m_fcal1SD(nullptr)
  , m_fcal2SD(nullptr)
  , m_fcalColdSD(nullptr)
{
  declareProperty( "FCAL1Volumes" , m_fcal1Volumes );
  declareProperty( "FCAL2Volumes" , m_fcal2Volumes );
  declareProperty( "FCALColdVolumes" , m_fcalColdVolumes );
  declareInterface<ISensitiveDetector>(this);
}

StatusCode LArG4H62004FCALSDTool::initializeSD()
{
  m_fcal1SD = new LArG4H62004SD( "LAr::FCAL::Module1::Gap::H6" , LArFCAL1Calculator::GetInstance() , m_timeBinType , m_timeBinWidth );
  m_fcal2SD = new LArG4H62004SD( "LAr::FCAL::Module2::Gap::H6" , LArFCAL2Calculator::GetInstance() , m_timeBinType , m_timeBinWidth );
  LArG4H6COLDTCMod0Calculator* aCalc = LArG4H6COLDTCMod0Calculator::GetInstance();
  aCalc->SetOutOfTimeCut(10000.);
  m_fcalColdSD = new LArG4H62004SD( "LAr::FCAL::ColdTC::H6" , aCalc , m_timeBinType , m_timeBinWidth );

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
