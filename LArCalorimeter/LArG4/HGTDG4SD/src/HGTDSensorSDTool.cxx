/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// HGTD Sensitive Detector.
// The Hits are processed here. For every hit I get the position and
// an information on the sensor in which the interaction happened

// Class header
#include "HGTDSensorSDTool.h"

// For the SD itself
#include "HGTDSensorSD.h"

// for the identifiers....
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/HGTD_ID.h"

// for the collection
#include "CxxUtils/make_unique.h"

// G4 stuff
#include "G4LogicalVolumeStore.hh"
#include "G4SDManager.hh"

// STL includes
#include <exception>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HGTDSensorSDTool::HGTDSensorSDTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase( type , name , parent ),
    m_LArHitColl("LArHitHGTD")
{
  declareProperty("TimeBinType",m_timeBinType);
  declareProperty("TimeBinWidth",m_timeBinWidth);
  declareProperty("TimeWindowMin",m_timeWindowMin);
  declareProperty("TimeWindowMax",m_timeWindowMax);
  declareInterface<ISensitiveDetector>(this);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StatusCode HGTDSensorSDTool::initialize()
{
  const CaloIdManager* caloIdManager=nullptr;
  CHECK( detStore()->retrieve(caloIdManager) );

  m_hgtdID = caloIdManager->getHGTD_ID();
  if(m_hgtdID==0)
    throw std::runtime_error("HGTDSensorSDTool: Invalid HGTD ID helper");

  // initialize the sensor
  m_hgtdSD = new HGTDSensorSD(name(), m_timeBinType, m_timeBinWidth, m_timeWindowMin, m_timeWindowMax);

  // setup the helper for the sensor
  setupHelpers(m_hgtdSD);

  return StatusCode::SUCCESS;
}

G4VSensitiveDetector* HGTDSensorSDTool::makeSD() { 
  return m_hgtdSD; 
}

StatusCode HGTDSensorSDTool::Gather()
{
  // In this case, *unlike* other SDs, the *tool* owns the collection
#ifdef ATHENAHIVE
  // Temporary fix for Hive until isValid is fixed
  m_LArHitColl = CxxUtils::make_unique<LArHitContainer>(m_outputCollectionNames[0]);
#else
  if (!m_LArHitColl.isValid()) m_LArHitColl = CxxUtils::make_unique<LArHitContainer>(m_outputCollectionNames[0]);
#endif
  m_hgtdSD->EndOfAthenaEvent( &*m_LArHitColl );
  
  return StatusCode::SUCCESS;
}

void HGTDSensorSDTool::setupHelpers( HGTDSensorSD* hgtd_sd ) const
{
  hgtd_sd->setupHelpers( m_hgtdID );
}

