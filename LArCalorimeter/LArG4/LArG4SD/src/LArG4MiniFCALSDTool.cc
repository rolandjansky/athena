/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4MiniFCALSDTool.h"
#include "LArG4Code/LArG4SimpleSD.h"

#include "LArG4Code/LArG4Hit.h"

#include "LArG4MiniFCAL/MiniFCALCalculator.h"

#include "CxxUtils/make_unique.h"

LArG4MiniFCALSDTool::LArG4MiniFCALSDTool(const std::string& type, const std::string& name, const IInterface *parent)
  : LArG4SDTool(type,name,parent)
  , m_HitColl("LArHitMiniFCAL") // I don't know what this should be!!!
{
  declareProperty("MiniVolumes",m_miniVolumes);
  declareInterface<ISensitiveDetector>(this);
}

StatusCode LArG4MiniFCALSDTool::initializeSD()
{
  m_miniSD = new LArG4SimpleSD( "MiniFCAL::Wafer" , MiniFCALCalculator::GetCalculator() , m_timeBinType , m_timeBinWidth );

  std::map<G4VSensitiveDetector*,std::vector<std::string>*> configuration;
  configuration[m_miniSD] = &m_miniVolumes;
  setupAllSDs(configuration);

  // Make sure the ID helpers are all set up
  setupHelpers(m_miniSD);

  return StatusCode::SUCCESS;
}

StatusCode LArG4MiniFCALSDTool::Gather()
{
  // In this case, *unlike* other SDs, the *tool* owns the collection
#ifdef ATHENAHIVE
  // Temporary fix for Hive until isValid is fixed
  m_HitColl = CxxUtils::make_unique<LArHitContainer>(m_HitColl.name());
#else
  if (!m_HitColl.isValid()) m_HitColl = CxxUtils::make_unique<LArHitContainer>(m_HitColl.name());
#endif
  // Hand this collection name off to the SDs.  They will be writing to the
  // collection, but only one at a time!
  m_miniSD->EndOfAthenaEvent( &*m_HitColl );
  return StatusCode::SUCCESS;
}
