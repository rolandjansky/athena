/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MergeCaloClusterContainers.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//this
#include "MergeMuonCaloEnergyContainers.h"

//muonEvent
#include "muonEvent/MuonCaloEnergyContainer.h"
#include "muonEvent/MuonContainer.h"
#include "muonEvent/Muon.h"
#include "muonEvent/CaloEnergy.h"

/////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////

Rec :: MergeMuonCaloEnergyContainers :: MergeMuonCaloEnergyContainers(const std::string& name, ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator),
  m_MuonCaloEnergyContainerTarget("MuonCaloEnergyCollection"),
  m_NewMuonCollection("StacoMuonCollection")
{
  declareProperty("MuonCaloEnergyContainerTarget", m_MuonCaloEnergyContainerTarget);
  declareProperty("NewMuonCollection", m_NewMuonCollection);
}
	
/////////////////////////////////////////////////////////////////
//Initialize
/////////////////////////////////////////////////////////////////

StatusCode Rec::MergeMuonCaloEnergyContainers::initialize()
{
  ATH_MSG_INFO ("Initialize done in "<<name());
  return StatusCode::SUCCESS;
}
	
/////////////////////////////////////////////////////////////////
// Finalize
/////////////////////////////////////////////////////////////////

StatusCode Rec::MergeMuonCaloEnergyContainers::finalize() 
{
  return StatusCode::SUCCESS;
}


/////////////////////////////////////////////////////////////////
// Execute
/////////////////////////////////////////////////////////////////

StatusCode Rec::MergeMuonCaloEnergyContainers::execute() 
{
  const MuonCaloEnergyContainer * muonCaloEnergyContainerTarget(NULL);
  const Analysis::MuonContainer * muonCollection_const(NULL);
	
  if(!evtStore()->retrieve(muonCaloEnergyContainerTarget, m_MuonCaloEnergyContainerTarget).isSuccess())
    {
      ATH_MSG_FATAL ("Cannot retrieve " << m_MuonCaloEnergyContainerTarget <<" !");
      return StatusCode::FAILURE;
    }
  if(!evtStore()->retrieve(muonCollection_const, m_NewMuonCollection).isSuccess())
    {
      ATH_MSG_FATAL("Cannot retrieve " << m_NewMuonCollection<<" !");
      return StatusCode::FAILURE;
    }
	
  Analysis::MuonContainer * muonCollection(const_cast<Analysis::MuonContainer *>(muonCollection_const));
  
  for(Analysis::MuonContainer :: iterator it=muonCollection->begin(); it!=muonCollection->end(); it++)
    {
      Analysis::Muon *muon(*it);
      if(muon->caloEnergyLink().isValid())
	{
	  CaloEnergy* caloEnergy(new CaloEnergy(**(muon->caloEnergyLink())));
	  const_cast<MuonCaloEnergyContainer *>(muonCaloEnergyContainerTarget)->push_back(caloEnergy);
	  muon->set_energyLoss(muonCaloEnergyContainerTarget, caloEnergy);
	}
    }
  return StatusCode::SUCCESS;
}
	
