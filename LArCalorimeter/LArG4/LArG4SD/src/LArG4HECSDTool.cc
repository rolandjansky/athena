/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4HECSDTool.h"
#include "LArG4Code/LArG4SimpleSD.h"

#include "LArG4Code/LArG4Hit.h"

#include "LArG4HEC/LArHECCalculator.h"
#include "LArG4HEC/LArHECLocalCalculator.h"
#include "LArG4HEC/LArHECWheelCalculator.h"

#include "CxxUtils/make_unique.h"

LArG4HECSDTool::LArG4HECSDTool(const std::string& type, const std::string& name,
                               const IInterface *parent)
  : LArG4SDTool(type,name,parent)
  , m_HitColl("LArHitHEC")
  //, m_sliceSD(nullptr)
  //, m_localSD(nullptr)
  , m_wheelSD(nullptr)
{
  //declareProperty("SliceVolumes",m_sliceVolumes);
  //declareProperty("LocalVolumes",m_localVolumes);
  declareProperty("WheelVolumes",m_wheelVolumes);
  // Only Wheel volumes? Why are the others there?
  declareInterface<ISensitiveDetector>(this);
}

StatusCode LArG4HECSDTool::initializeSD()
{
  //m_sliceSD = new LArG4SimpleSD( "LAr::HEC::Module::Depth::Slice" , LArHECCalculator::GetCalculator() , m_timeBinType , m_timeBinWidth );
  //m_localSD = new LArG4SimpleSD( "LAr::HEC::Module::Depth::Slice::Local" , LArHECLocalCalculator::GetCalculator() , m_timeBinType , m_timeBinWidth );
  m_wheelSD = new LArG4SimpleSD( "LAr::HEC::Module::Depth::Slice::Wheel" , LArHECWheelCalculator::GetCalculator() , m_timeBinType , m_timeBinWidth );

  std::map<G4VSensitiveDetector*,std::vector<std::string>*> configuration;
  //configuration[m_sliceSD] = &m_sliceVolumes;
  //configuration[m_localSD] = &m_localVolumes;
  configuration[m_wheelSD] = &m_wheelVolumes;
  setupAllSDs(configuration);

  // Make sure the ID helpers are all set up
  //setupHelpers(m_sliceSD);
  //setupHelpers(m_localSD);
  setupHelpers(m_wheelSD);

  return StatusCode::SUCCESS;
}

StatusCode LArG4HECSDTool::Gather()
{
  // In this case, *unlike* other SDs, the *tool* owns the collection
  if (!m_HitColl.isValid()) m_HitColl = CxxUtils::make_unique<LArHitContainer>(m_HitColl.name());
  // Hand this collection name off to the SDs.  They will be writing to the
  // collection, but only one at a time!
  //m_sliceSD->EndOfAthenaEvent( &*m_HitColl );
  //m_localSD->EndOfAthenaEvent( &*m_HitColl );
  m_wheelSD->EndOfAthenaEvent( &*m_HitColl );
  return StatusCode::SUCCESS;
}
