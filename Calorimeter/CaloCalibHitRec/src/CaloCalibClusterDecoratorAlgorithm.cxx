/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloCalibClusterDecoratorAlgorithm.h"

//Core classes
#include "StoreGate/WriteDecorHandle.h"

StatusCode CaloCalibClusterDecoratorAlgorithm::initialize(){

  ATH_CHECK(m_mapIdentifierToCalibHitsReadHandleKey.initialize());
  ATH_CHECK(m_mapTruthBarcodeToTruthParticleReadHandleKey.initialize());

  ATH_CHECK(m_caloClusterWriteDecorHandleKeyNLeadingTruthParticles.initialize());

  ATH_CHECK(m_truthAttributerTool.retrieve());

  return StatusCode::SUCCESS;
}

StatusCode CaloCalibClusterDecoratorAlgorithm::execute(const EventContext& ctx) const{

  SG::ReadHandle<std::map<Identifier,std::vector<const CaloCalibrationHit*> > > mapIdentifierToCalibHitsReadHandle(m_mapIdentifierToCalibHitsReadHandleKey, ctx);
  if(!mapIdentifierToCalibHitsReadHandle.isValid()){
    ATH_MSG_WARNING("Could not retrieve map between Identifier and calibraiton hits from Storegae");
    return StatusCode::FAILURE;
  }

  SG::ReadHandle<std::map<unsigned int,const xAOD::TruthParticle* > > mapTruthBarcodeToTruthParticleReadHandle(m_mapTruthBarcodeToTruthParticleReadHandleKey, ctx);
  if(!mapTruthBarcodeToTruthParticleReadHandle.isValid()){
    ATH_MSG_WARNING("Could not retrieve map between truth barcode and truth particle from Storegate");
    return StatusCode::FAILURE;
  }

  SG::WriteDecorHandle<xAOD::CaloClusterContainer, std::vector< std::pair<unsigned int, double> > > caloClusterWriteDecorHandleNLeadingTruthParticles(m_caloClusterWriteDecorHandleKeyNLeadingTruthParticles, ctx);
  StatusCode sc;
  
  for (auto thisCaloCluster : *caloClusterWriteDecorHandleNLeadingTruthParticles){

    std::vector<std::pair<unsigned int, double > > newBarCodeTruthPairs;
    sc = m_truthAttributerTool->calculateTruthEnergies(*thisCaloCluster, m_numTruthParticles, *mapIdentifierToCalibHitsReadHandle, *mapTruthBarcodeToTruthParticleReadHandle, newBarCodeTruthPairs);
    if (sc == StatusCode::FAILURE) return sc;
    
    for (const auto& thisPair : newBarCodeTruthPairs) ATH_MSG_DEBUG("Cluster Final loop: Particle with barcode " << thisPair.first << " has truth energy of " <<  thisPair.second << " for cluster with e, eta " << thisCaloCluster->e() << " and " << thisCaloCluster->eta());
    caloClusterWriteDecorHandleNLeadingTruthParticles(*thisCaloCluster) = newBarCodeTruthPairs;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode CaloCalibClusterDecoratorAlgorithm::finalize(){
  return StatusCode::SUCCESS;
}
