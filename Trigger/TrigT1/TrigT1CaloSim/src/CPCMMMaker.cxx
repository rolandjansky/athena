/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <numeric>

#include "TrigT1CaloEvent/CMMCPHits.h"
#include "TrigT1CaloEvent/CPMHits.h"
#include "TrigT1CaloEvent/CPMTower.h"
#include "TrigT1CaloEvent/CPMRoI.h"
#include "TrigT1CaloEvent/EmTauROI.h"
#include "TrigT1CaloEvent/EmTauROI_ClassDEF.h"
#include "TrigT1CaloEvent/CPBSCollectionV1.h"
#include "TrigT1CaloSim/CPCMMMaker.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

namespace LVL1 {


CPCMMMaker::CPCMMMaker(const std::string& name, ISvcLocator* pSvcLocator)
                       : AthAlgorithm(name, pSvcLocator), 
			 m_cpHitsTool("LVL1::L1CPHitsTools/L1CPHitsTools")
{
  declareProperty("CPMTowerLocation",
         m_cpmTowerLocation       = TrigT1CaloDefs::CPMTowerLocation);
  declareProperty("CPMHitsLocation",
         m_cpmHitsLocation        = TrigT1CaloDefs::CPMHitsLocation);
  declareProperty("CMMCPHitsLocation",
         m_cmmHitsLocation        = TrigT1CaloDefs::CMMCPHitsLocation);
  declareProperty("CPMRoILocation",
         m_cpmRoiLocation         = TrigT1CaloDefs::CPMRoILocation);
  declareProperty("EmTauROILocation",
         m_emTauRoiLocation       = TrigT1CaloDefs::EmTauROILocation);
  declareProperty("CPBSCollectionLocation",
         m_cpBsCollectionLocation = TrigT1CaloDefs::CPBSCollectionLocation);
  declareProperty("RunSimulation", m_runSimulation = true);
}

CPCMMMaker::~CPCMMMaker()
{
}

// Initialize

StatusCode CPCMMMaker::initialize()
{
  if (m_runSimulation) {

    // Retrieve Hits tool
    ATH_CHECK( m_cpHitsTool.retrieve() );
  }

  return StatusCode::SUCCESS;
}

// Execute

StatusCode CPCMMMaker::execute()
{
  if (m_runSimulation) {

    // CMM-CP

    StatusCode sc = makeCmmCp();
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Unable to make CMM-CP" );
    }
    else {
      ATH_MSG_DEBUG( "Made CMM-CP" );
    }

    // CPM RoIs

    sc = makeCpmRoi();
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Unable to make CPM RoIs" );
    }
    else {
      ATH_MSG_DEBUG( "Made CPM RoIs" );
    }
  }

  // Fill CP bytestream container

  StatusCode sc = makeCpBsCollection();
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Unable to make CP bytestream container" );
  }

  return StatusCode::SUCCESS;
}

// Finalize

StatusCode CPCMMMaker::finalize()
{
  return StatusCode::SUCCESS;
}

// Make CMM-CP

StatusCode CPCMMMaker::makeCmmCp()
{
  // Find CPM hits

  StatusCode sc;
  const CPMHitsCollection* hitCollection = 0;
  if (evtStore()->contains<CPMHitsCollection>(m_cpmHitsLocation)) {
    sc = evtStore()->retrieve(hitCollection, m_cpmHitsLocation);
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !hitCollection) {
    ATH_MSG_DEBUG( "No CPM Hits container found" );
    hitCollection = 0;
  }
  else {
    ATH_MSG_DEBUG( "Found CPM Hits container" );
  }

  // Create CMM-CP container

  CMMHitsCollection* cmm = new CMMHitsCollection;
  if (hitCollection) m_cpHitsTool->formCMMCPHits(hitCollection, cmm);

  // Save in StoreGate

  ATH_CHECK( evtStore()->overwrite(cmm, m_cmmHitsLocation,true,false,false) );

  return StatusCode::SUCCESS;
}

// Make CPM RoI container

StatusCode CPCMMMaker::makeCpmRoi()
{
  // Find EmTauROIs

  StatusCode sc;
  const EmTauROICollection* etrCollection = 0;
  if (evtStore()->contains<EmTauROICollection>(m_emTauRoiLocation)) {
    sc = evtStore()->retrieve(etrCollection, m_emTauRoiLocation);
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !etrCollection) {
    ATH_MSG_DEBUG( "No EmTauROIs found" );
    etrCollection = 0;
  }

  // Fill CPMRoIs

  CPMRoICollection* roiCollection = new CPMRoICollection;
  if (etrCollection) {
    EmTauROICollection::const_iterator pos  = etrCollection->begin();
    EmTauROICollection::const_iterator pose = etrCollection->end();
    for (; pos != pose; ++pos) {
      ATH_MSG_DEBUG( "Add new CPMRoI" );
      roiCollection->push_back(new CPMRoI((*pos)->roiWord()));
    }
  }

  // Save container

  ATH_CHECK( evtStore()->overwrite(roiCollection, m_cpmRoiLocation,true,false,false) );

  return StatusCode::SUCCESS;
}

// Make CP bytestream container

StatusCode CPCMMMaker::makeCpBsCollection()
{
  // Find CPM trigger towers

  StatusCode sc;
  const CPMTowerCollection* ttCollection = 0;
  if (evtStore()->contains<CPMTowerCollection>(m_cpmTowerLocation)) {
    sc = evtStore()->retrieve(ttCollection, m_cpmTowerLocation);
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !ttCollection) {
    ATH_MSG_DEBUG( "No CPM Towers found" );
    ttCollection = 0;
  }

  // Find CPM hits

  const CPMHitsCollection* hitCollection = 0;
  if (evtStore()->contains<CPMHitsCollection>(m_cpmHitsLocation)) {
    sc = evtStore()->retrieve(hitCollection, m_cpmHitsLocation);
  } else sc = StatusCode::FAILURE;
  if (sc.isFailure() || !hitCollection) {
    ATH_MSG_DEBUG( "No CPM Hits found" );
    hitCollection = 0;
  }

  // Find CMM hits

  const CMMHitsCollection* cmmHitCollection = 0;
  sc = evtStore()->retrieve(cmmHitCollection, m_cmmHitsLocation);
  if (sc.isFailure() || !cmmHitCollection) {
    ATH_MSG_DEBUG( "No CMM Hits found" );
    cmmHitCollection = 0;
  }

  // Create CP container

  CPBSCollectionV1* cp = new CPBSCollectionV1(ttCollection, hitCollection,
                                                        cmmHitCollection);
  ATH_CHECK( evtStore()->overwrite(cp, m_cpBsCollectionLocation,true,false,false) );

  return StatusCode::SUCCESS;
}

} // end namespace
