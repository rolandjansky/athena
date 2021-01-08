/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PFlowCalibPFODecoratorAlgorithm.h"

//Core classes
#include "StoreGate/WriteDecorHandle.h"

StatusCode PFlowCalibPFODecoratorAlgorithm::initialize(){

  ATH_CHECK(m_mapIdentifierToCalibHitsReadHandleKey.initialize());
  ATH_CHECK(m_mapTruthBarcodeToTruthParticleReadHandleKey.initialize());

  ATH_CHECK(m_pfoWriteDecorHandleKeyNLeadingTruthParticles.initialize());
  ATH_CHECK(m_feWriteDecorHandleKeyNLeadingTruthParticles.initialize());

  ATH_CHECK(m_truthAttributerTool.retrieve());

  return StatusCode::SUCCESS;
}
StatusCode PFlowCalibPFODecoratorAlgorithm::LinkCalibHitPFO(
							    SG::WriteDecorHandle<xAOD::PFOContainer, std::vector< std::pair<unsigned int, double> > >& pfoWriteDecorHandle,
							    SG::ReadHandle<std::map<Identifier,std::vector<const CaloCalibrationHit*> > >& CalibHitReadHandle,
							    SG::ReadHandle<std::map<unsigned int,const xAOD::TruthParticle* > >& TruthParticleHandle  
							    ) const
{
  //Algorithm to produce links between PFO and CalibHits
  StatusCode sc;
  for (auto thisPFO : *pfoWriteDecorHandle){

    const xAOD::CaloCluster* thisCaloCluster = thisPFO->cluster(0);
    
    std::vector<std::pair<unsigned int, double > > newBarCodeTruthPairs;
    sc = m_truthAttributerTool->calculateTruthEnergies(*thisCaloCluster, m_numTruthParticles, *CalibHitReadHandle, *TruthParticleHandle, newBarCodeTruthPairs);
    if (sc == StatusCode::FAILURE) return sc;
    
    for (const auto& thisPair : newBarCodeTruthPairs) ATH_MSG_DEBUG("Cluster Final loop: Particle with barcode " << thisPair.first << " has truth energy of " <<  thisPair.second << " for cluster with e, eta " << thisCaloCluster->e() << " and " << thisCaloCluster->eta());

    pfoWriteDecorHandle(*thisPFO) = newBarCodeTruthPairs;
  }
  return StatusCode::SUCCESS;
}
// Same algorithm for FE as for PFO
StatusCode PFlowCalibPFODecoratorAlgorithm::LinkCalibHitPFO(
							    SG::WriteDecorHandle<xAOD::FlowElementContainer, std::vector< std::pair<unsigned int, double> > >& pfoWriteDecorHandle,
							    SG::ReadHandle<std::map<Identifier,std::vector<const CaloCalibrationHit*> > >& CalibHitReadHandle,
							    SG::ReadHandle<std::map<unsigned int,const xAOD::TruthParticle* > >& TruthParticleHandle)const
{
  
  StatusCode sc;
  for (auto thisFE : *pfoWriteDecorHandle){
    // retrieve calo cluster, first as the iparticle we retrieve then cast to calocluster ptr
    const xAOD::IParticle* FE_Iparticle=thisFE->otherObjects().at(0);
    const xAOD::CaloCluster* thisCaloCluster = dynamic_cast<const xAOD::CaloCluster*>(FE_Iparticle);
    
    std::vector<std::pair<unsigned int, double > > newBarCodeTruthPairs;
    sc = m_truthAttributerTool->calculateTruthEnergies(*thisCaloCluster, m_numTruthParticles, *CalibHitReadHandle, *TruthParticleHandle, newBarCodeTruthPairs);
    if (sc == StatusCode::FAILURE) return sc;
    
    for (const auto& thisPair : newBarCodeTruthPairs) ATH_MSG_DEBUG("Cluster Final loop: Particle with barcode " << thisPair.first << " has truth energy of " <<  thisPair.second << " for cluster with e, eta " << thisCaloCluster->e() << " and " << thisCaloCluster->eta());
    
    pfoWriteDecorHandle(*thisFE) = newBarCodeTruthPairs;
  }
  return StatusCode::SUCCESS;
}

StatusCode PFlowCalibPFODecoratorAlgorithm::execute(const EventContext& ctx) const{

  SG::ReadHandle<std::map<Identifier,std::vector<const CaloCalibrationHit*> > > mapIdentifierToCalibHitsReadHandle(m_mapIdentifierToCalibHitsReadHandleKey, ctx);
  if(!mapIdentifierToCalibHitsReadHandle.isValid()){
    ATH_MSG_WARNING("Could not retrieve map between Identifier and calibraiton hits from Storegate");
    return StatusCode::FAILURE;
  }

  SG::ReadHandle<std::map<unsigned int,const xAOD::TruthParticle* > > mapTruthBarcodeToTruthParticleReadHandle(m_mapTruthBarcodeToTruthParticleReadHandleKey, ctx);
  if(!mapTruthBarcodeToTruthParticleReadHandle.isValid()){
    ATH_MSG_WARNING("Could not retrieve map between truth barcode and truth particle from Storegate");
    return StatusCode::FAILURE;
  }
  // pfo linker alg
  SG::WriteDecorHandle<xAOD::PFOContainer, std::vector< std::pair<unsigned int, double> > > pfoWriteDecorHandleNLeadingTruthParticles(m_pfoWriteDecorHandleKeyNLeadingTruthParticles, ctx);

  ATH_CHECK(this->LinkCalibHitPFO(
				      pfoWriteDecorHandleNLeadingTruthParticles,
				      mapIdentifierToCalibHitsReadHandle,
				      mapTruthBarcodeToTruthParticleReadHandle)); // end of check block 

  if(m_useFlowElements){
    SG::WriteDecorHandle<xAOD::FlowElementContainer,std::vector<std::pair<unsigned int, double> > > feWriteDecorHandleNLeadingTruthParticles(m_feWriteDecorHandleKeyNLeadingTruthParticles,ctx);
    
    ATH_CHECK(this->LinkCalibHitPFO(
					feWriteDecorHandleNLeadingTruthParticles,
					mapIdentifierToCalibHitsReadHandle,
					mapTruthBarcodeToTruthParticleReadHandle)); // end of check block
  }
  
  return StatusCode::SUCCESS;
}

StatusCode PFlowCalibPFODecoratorAlgorithm::finalize(){
  return StatusCode::SUCCESS;
}


