/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4EMBSDTool.h"
#include "LArG4Code/LArG4SimpleSD.h"

#include "LArG4Code/LArG4Hit.h"

#include "LArG4Barrel/LArBarrelCalculator.h"
#include "LArG4Barrel/LArBarrelPresamplerCalculator.h"

#include "CxxUtils/make_unique.h"
#include "G4SDManager.hh"

LArG4EMBSDTool::LArG4EMBSDTool(const std::string& type, const std::string& name, const IInterface *parent)
  : LArG4SDTool(type,name,parent)
  , m_HitColl("LArHitEMB")
{
  declareProperty("StacVolumes",m_stacVolumes);
  declareProperty("PresamplerVolumes",m_presVolumes);
  declareInterface<ISensitiveDetector>(this);
}

StatusCode LArG4EMBSDTool::initializeSD()
{
  m_presSD = new LArG4SimpleSD( "LAr::Barrel::Presampler::Module" , LArBarrelPresamplerCalculator::GetCalculator() , m_timeBinType , m_timeBinWidth );
  m_stacSD = new LArG4SimpleSD( "LAr::EMB::STAC" , LArBarrelCalculator::GetCalculator() , m_timeBinType , m_timeBinWidth );

  std::map<G4VSensitiveDetector*,std::vector<std::string>*> configuration;
  configuration[m_presSD] = &m_presVolumes;
  configuration[m_stacSD] = &m_stacVolumes;
  setupAllSDs(configuration);

  // Make sure the helpers are all set up
  setupHelpers(m_presSD);
  setupHelpers(m_stacSD);

  return StatusCode::SUCCESS;
}

StatusCode LArG4EMBSDTool::Gather()
{
  // In this case, *unlike* other SDs, the *tool* owns the collection
#ifdef ATHENAHIVE
  // Temporary fix for Hive until isValid is fixed
  m_HitColl = CxxUtils::make_unique<LArHitContainer>(m_HitColl.name());
#else
  if (!m_HitColl.isValid()) m_HitColl = CxxUtils::make_unique<LArHitContainer>(m_HitColl.name());
#endif
  m_presSD->EndOfAthenaEvent( &*m_HitColl );
  m_stacSD->EndOfAthenaEvent( &*m_HitColl );

  // Additions for optional fast simulation
  if (m_useFrozenShowers){
    LArG4SimpleSD * fastSD = dynamic_cast<LArG4SimpleSD*>( G4SDManager::GetSDMpointer()->FindSensitiveDetector("BarrelFastSimDedicatedSD") );
    if (fastSD){
      fastSD->EndOfAthenaEvent( &*m_HitColl );
    } else {
      ATH_MSG_WARNING( "Could not find BarrelFastSimDedicatedSD, but fast sim was configured" );
    }
  }

  return StatusCode::SUCCESS;
}
