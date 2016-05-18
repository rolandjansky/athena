/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4FCALSDTool.h"
#include "LArG4Code/LArG4SimpleSD.h"

#include "LArG4Code/LArG4Hit.h"

#include "LArG4FCAL/LArFCAL1Calculator.h"
#include "LArG4FCAL/LArFCAL2Calculator.h"
#include "LArG4FCAL/LArFCAL3Calculator.h"

#include "CxxUtils/make_unique.h"
#include "G4SDManager.hh"

LArG4FCALSDTool::LArG4FCALSDTool(const std::string& type, const std::string& name,
                                 const IInterface *parent)
  : LArG4SDTool(type,name,parent)
  , m_HitColl("LArHitFCAL")
  , m_fcal1SD(nullptr)
  , m_fcal2SD(nullptr)
  , m_fcal3SD(nullptr)
{
  declareProperty("FCAL1Volumes",m_fcal1Volumes);
  declareProperty("FCAL2Volumes",m_fcal2Volumes);
  declareProperty("FCAL3Volumes",m_fcal3Volumes);
  declareInterface<ISensitiveDetector>(this);
}

StatusCode LArG4FCALSDTool::initializeSD()
{
  m_fcal1SD = new LArG4SimpleSD( "LAr::FCAL::Module1::Gap" , LArFCAL1Calculator::GetInstance() , m_timeBinType , m_timeBinWidth );
  m_fcal2SD = new LArG4SimpleSD( "LAr::FCAL::Module2::Gap" , LArFCAL2Calculator::GetInstance() , m_timeBinType , m_timeBinWidth );
  m_fcal3SD = new LArG4SimpleSD( "LAr::FCAL::Module3::Gap" , LArFCAL3Calculator::GetInstance() , m_timeBinType , m_timeBinWidth );

  std::map<G4VSensitiveDetector*,std::vector<std::string>*> configuration;
  configuration[m_fcal1SD] = &m_fcal1Volumes;
  configuration[m_fcal2SD] = &m_fcal2Volumes;
  configuration[m_fcal3SD] = &m_fcal3Volumes;
  setupAllSDs(configuration);

  // Make sure the ID helpers are all set up
  setupHelpers(m_fcal1SD);
  setupHelpers(m_fcal2SD);
  setupHelpers(m_fcal3SD);
  
  return StatusCode::SUCCESS;
}

StatusCode LArG4FCALSDTool::Gather()
{
  // In this case, *unlike* other SDs, the *tool* owns the collection
  if (!m_HitColl.isValid()) m_HitColl = CxxUtils::make_unique<LArHitContainer>(m_HitColl.name());
  // Hand this collection name off to the SDs.  They will be writing to the
  // collection, but only one at a time!
  m_fcal1SD->EndOfAthenaEvent( &*m_HitColl );
  m_fcal2SD->EndOfAthenaEvent( &*m_HitColl );
  m_fcal3SD->EndOfAthenaEvent( &*m_HitColl );

  // Additions for optional fast simulation
  if (m_useFrozenShowers){
    // FIXME: this is very expensive, especially for a fastsim!
    LArG4SimpleSD * fastSD = dynamic_cast<LArG4SimpleSD*>( G4SDManager::GetSDMpointer()->FindSensitiveDetector("FCALFastSimDedicatedSD") );
    if (fastSD){
      fastSD->EndOfAthenaEvent( &*m_HitColl );
    } else {
      ATH_MSG_WARNING( "Could not find FCALFastSimDedicatedSD, but fast sim was configured" );
    }
  }

  return StatusCode::SUCCESS;
}
