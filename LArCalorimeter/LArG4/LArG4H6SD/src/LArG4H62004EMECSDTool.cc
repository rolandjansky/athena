/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4H62004EMECSDTool.h"
#include "LArG4H62004SD.h"

#include "LArG4Code/LArG4Hit.h"

#include "LArG4EC/EnergyCalculator.h"

#include "CxxUtils/make_unique.h"

LArG4H62004EMECSDTool::LArG4H62004EMECSDTool(const std::string& type, const std::string& name, const IInterface *parent)
  : LArG4SDTool(type,name,parent)
  , m_HitColl("LArHitEMEC")
  , m_emecSD(nullptr)
{
  declareInterface<ISensitiveDetector>(this);
}

StatusCode LArG4H62004EMECSDTool::initializeSD()
{
  m_emecSD = new LArG4H62004SD( "LAr::EMEC::InnerModule::H6" , new LArG4::EC::EnergyCalculator(LArWheelCalculator::InnerAbsorberModule,LArG4::EC::EnergyCalculator::EMEC_ECOR_GADJ) , m_timeBinType , m_timeBinWidth );

  std::map<G4VSensitiveDetector*,std::vector<std::string>*> configuration;
  configuration[m_emecSD] = &m_volumeNames;
  setupAllSDs(configuration);

  // Make sure the ID helpers are all set up
  setupHelpers(m_emecSD);

  return StatusCode::SUCCESS;
}

StatusCode LArG4H62004EMECSDTool::Gather()
{
  // In this case, *unlike* other SDs, the *tool* owns the collection
  if (!m_HitColl.isValid()) m_HitColl = CxxUtils::make_unique<LArHitContainer>(m_HitColl.name());
  // Hand this collection name off to the SDs.  They will be writing to the
  // collection, but only one at a time!
  m_emecSD->EndOfAthenaEvent( &*m_HitColl );
  return StatusCode::SUCCESS;
}
