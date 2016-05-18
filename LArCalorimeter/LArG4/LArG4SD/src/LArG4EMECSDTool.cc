/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4EMECSDTool.h"
#include "LArG4Code/LArG4SimpleSD.h"

#include "LArG4Code/LArG4Hit.h"

#include "LArG4EC/EnergyCalculator.h"
#include "LArG4EC/LArEndcapPresamplerCalculator.h"

#include "CxxUtils/make_unique.h"
#include "G4SDManager.hh"

LArG4EMECSDTool::LArG4EMECSDTool(const std::string& type, const std::string& name,
                                 const IInterface *parent)
  : LArG4SDTool(type,name,parent)
  , m_HitColl("LArHitEMEC")
  , m_posIWSD(nullptr)
  , m_negIWSD(nullptr)
  , m_posOWSD(nullptr)
  , m_negOWSD(nullptr)
  , m_presSD(nullptr)
  , m_bobSD(nullptr)
{
  declareProperty("PosIWVolumes",m_posIWVolumes);
  declareProperty("NegIWVolumes",m_negIWVolumes);
  declareProperty("PosOWVolumes",m_posOWVolumes);
  declareProperty("NegOWVolumes",m_negOWVolumes);
  declareProperty("PresVolumes",m_presVolumes);
  declareProperty("BOBarretteVolumes",m_bobVolumes);
  //declareProperty("BIBarretteVolumes",m_bibVolumes={"LArMgr::LAr::EMEC::BackInnerBarrette::Module::Phidiv"});
  declareInterface<ISensitiveDetector>(this);
}

StatusCode LArG4EMECSDTool::initializeSD()
{
  m_posIWSD = new LArG4SimpleSD( "LAr::EMEC::Pos::InnerWheel" , new LArG4::EC::EnergyCalculator(LArWheelCalculator::InnerAbsorberWheel, LArG4::EC::EnergyCalculator::EMEC_ECOR_ROPT, 1) , m_timeBinType , m_timeBinWidth );
  m_negIWSD = new LArG4SimpleSD( "LAr::EMEC::Neg::InnerWheel" , new LArG4::EC::EnergyCalculator(LArWheelCalculator::InnerAbsorberWheel, LArG4::EC::EnergyCalculator::EMEC_ECOR_ROPT, -1) , m_timeBinType , m_timeBinWidth );
  m_posOWSD = new LArG4SimpleSD( "LAr::EMEC::Pos::OuterWheel" , new LArG4::EC::EnergyCalculator(LArWheelCalculator::OuterAbsorberWheel, LArG4::EC::EnergyCalculator::EMEC_ECOR_ROPT, 1) , m_timeBinType , m_timeBinWidth );
  m_negOWSD = new LArG4SimpleSD( "LAr::EMEC::Neg::OuterWheel" , new LArG4::EC::EnergyCalculator(LArWheelCalculator::OuterAbsorberWheel, LArG4::EC::EnergyCalculator::EMEC_ECOR_ROPT, -1) , m_timeBinType , m_timeBinWidth );
  m_presSD  = new LArG4SimpleSD( "LAr::Endcap::Presampler::LiquidArgon" , LArEndcapPresamplerCalculator::GetCalculator() , m_timeBinType , m_timeBinWidth );
  m_bobSD   = new LArG4SimpleSD( "LAr::EMEC::BackOuterBarrette::Module::Phidiv", new LArG4::EC::EnergyCalculator(LArWheelCalculator::BackOuterBarretteWheel) , m_timeBinType , m_timeBinWidth);
  //m_bibSD   = new LArG4SimpleSD( "LAr::EMEC::BackInnerBarrette::Module::Phidiv" , new LArG4::EC::EnergyCalculator(LArWheelCalculator::BackInnerBarretteWheel) , m_timeBinType , m_timeBinWidth);

  std::map<G4VSensitiveDetector*,std::vector<std::string>*> configuration;
  configuration[m_posIWSD] = &m_posIWVolumes;
  configuration[m_posOWSD] = &m_posOWVolumes;
  configuration[m_negIWSD] = &m_negIWVolumes;
  configuration[m_negOWSD] = &m_negOWVolumes;
  configuration[m_presSD]  = &m_presVolumes;
  configuration[m_bobSD]   = &m_bobVolumes;
  //configuration[m_bibSD] = &m_bibVolumes;
  setupAllSDs(configuration);

  // Make sure the ID helpers are all set up
  setupHelpers(m_posIWSD);
  setupHelpers(m_posOWSD);
  setupHelpers(m_negIWSD);
  setupHelpers(m_negOWSD);
  setupHelpers(m_presSD);
  setupHelpers(m_bobSD);
  //setupHelpers(m_bibSD);

  return StatusCode::SUCCESS;
}

StatusCode LArG4EMECSDTool::Gather()
{
  // In this case, *unlike* other SDs, the *tool* owns the collection
  if (!m_HitColl.isValid()) m_HitColl = CxxUtils::make_unique<LArHitContainer>(m_HitColl.name());

  // Hand this collection name off to the SDs.  They will be writing to the
  // collection, but only one at a time!
  m_posIWSD->EndOfAthenaEvent( &*m_HitColl );
  m_negIWSD->EndOfAthenaEvent( &*m_HitColl );
  m_posOWSD->EndOfAthenaEvent( &*m_HitColl );
  m_negOWSD->EndOfAthenaEvent( &*m_HitColl );
  m_presSD->EndOfAthenaEvent( &*m_HitColl );
  m_bobSD->EndOfAthenaEvent( &*m_HitColl );
  //m_bibSD->EndOfAthenaEvent( &*m_HitColl );

  // Additions for optional fast simulation
  if (m_useFrozenShowers){
    // FIXME: this is very expensive, especially for a fastsim!
    LArG4SimpleSD * fastSD = dynamic_cast<LArG4SimpleSD*>
      ( G4SDManager::GetSDMpointer()->FindSensitiveDetector("EndcapFastSimDedicatedSD") );
    if (fastSD){ 
      fastSD->EndOfAthenaEvent( &*m_HitColl );
    } else {
      ATH_MSG_WARNING( "Could not find EndcapFastSimDedicatedSD, but fast sim was configured" );
    }
  }

  return StatusCode::SUCCESS;
}
