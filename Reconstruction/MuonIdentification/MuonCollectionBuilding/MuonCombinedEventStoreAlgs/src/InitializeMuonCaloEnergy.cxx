/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InitializeMuonClusters.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Include files

#include "InitializeMuonCaloEnergy.h"
#include "muonEvent/MuonCaloEnergyContainer.h"

/////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////

Rec::InitializeMuonCaloEnergy::InitializeMuonCaloEnergy(const std::string& name, 
						   ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_muonCaloEnergyContainerName("MuonCaloEnergyContainer")
{
  declareProperty("MuonCaloEnergyContainer", m_muonCaloEnergyContainerName);
}

/////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////

Rec::InitializeMuonCaloEnergy::~InitializeMuonCaloEnergy() {}

/////////////////////////////////////////////////////////////////
// Initialize
/////////////////////////////////////////////////////////////////

StatusCode Rec::InitializeMuonCaloEnergy::initialize() {

  ATH_MSG_INFO ("Initialize done in "<<name());
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////
// Finalize
/////////////////////////////////////////////////////////////////

StatusCode Rec::InitializeMuonCaloEnergy::finalize() {
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////
// Execute
/////////////////////////////////////////////////////////////////

StatusCode Rec::InitializeMuonCaloEnergy::execute() {

  if (evtStore()->contains<MuonCaloEnergyContainer>(m_muonCaloEnergyContainerName) ) {

    const MuonCaloEnergyContainer * tmp;
    if ( evtStore()->retrieve(tmp,m_muonCaloEnergyContainerName).isSuccess() ) {
      if ( evtStore()->remove(tmp).isSuccess() )
	ATH_MSG_DEBUG ("removed pre-existing MuonCaloEnergyContainer" );
    } 
  }
  
  MuonCaloEnergyContainer * caloEnergyContainer = new MuonCaloEnergyContainer();
  StatusCode sc = evtStore()->record(caloEnergyContainer, m_muonCaloEnergyContainerName);
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING ("Not able to create Muon CaloEnergy Container - bailing out ...");
    return StatusCode::SUCCESS;
  }

  return StatusCode::SUCCESS;
}

