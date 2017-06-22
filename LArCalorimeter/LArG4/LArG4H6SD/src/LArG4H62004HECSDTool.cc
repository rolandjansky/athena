/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4H62004HECSDTool.h"
#include "LArG4H62004SD.h"

#include "LArG4Code/LArG4Hit.h"

#include "CxxUtils/make_unique.h"

LArG4H62004HECSDTool::LArG4H62004HECSDTool(const std::string& type, const std::string& name, const IInterface *parent)
  : LArG4SDTool(type,name,parent)
  , m_HitColl("LArHitHEC")
  , m_calculator("LArH62004HECLocalCalculator", name)
  , m_wheelSD(nullptr)
{
  declareInterface<ISensitiveDetector>(this);
  declareProperty("Calculator", m_calculator);
}

StatusCode LArG4H62004HECSDTool::initializeCalculators()
{
  ATH_CHECK(m_calculator.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode LArG4H62004HECSDTool::initializeSD()
{
  m_wheelSD = new LArG4H62004SD( "LAr::HEC::Module::Depth::Slice::Local::H6" , &*m_calculator , m_timeBinType , m_timeBinWidth );

  std::map<G4VSensitiveDetector*,std::vector<std::string>*> configuration;
  configuration[m_wheelSD] = &m_volumeNames;
  setupAllSDs(configuration);

  // Make sure the ID helpers are all set up
  setupHelpers(m_wheelSD);

  return StatusCode::SUCCESS;
}

StatusCode LArG4H62004HECSDTool::Gather()
{
  // In this case, *unlike* other SDs, the *tool* owns the collection
  if (!m_HitColl.isValid()) m_HitColl = CxxUtils::make_unique<LArHitContainer>(m_HitColl.name());
  // Hand this collection name off to the SDs.  They will be writing to the
  // collection, but only one at a time!
  m_wheelSD->EndOfAthenaEvent( &*m_HitColl );
  return StatusCode::SUCCESS;
}
