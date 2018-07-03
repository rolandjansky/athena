/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include <memory>

#include "egammaMVASvc.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaDefs.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "PathResolver/PathResolver.h"

egammaMVASvc::egammaMVASvc(const std::string& name, ISvcLocator* svc) :
  base_class( name, svc )
{
}

StatusCode egammaMVASvc::initialize() 
{
  ATH_MSG_DEBUG("In initialize of " << name() << "..." );
  
  ATH_CHECK(m_mvaElectron.retrieve());
  ATH_CHECK(m_mvaUnconvertedPhoton.retrieve());
  ATH_CHECK(m_mvaConvertedPhoton.retrieve());

  return StatusCode::SUCCESS;
}


StatusCode egammaMVASvc::finalize(){
  ATH_MSG_DEBUG( "in finalize" );
  return StatusCode::SUCCESS;
}

StatusCode egammaMVASvc::execute(xAOD::CaloCluster* cluster,
				 const xAOD::Egamma* eg) const
{
  if (!eg || !cluster) {
    ATH_MSG_ERROR("Invalid Pointer to egamma or cluster object");
    return StatusCode::FAILURE;
  }
  double mvaE = getEnergy(cluster, eg);
  ATH_MSG_DEBUG( "Calculated MVA calibrated energy = " << mvaE );
  if (mvaE > eg->m()) {
    cluster->setCalE(mvaE);
  }
  else {
    ATH_MSG_DEBUG("MVA energy (" << mvaE << ") < particle mass ("
                  << eg->m() << "), setting e = cluster energy (" << cluster->e() << ")");
    cluster->setCalE(cluster->e());
  }
  return StatusCode::SUCCESS;
}

StatusCode egammaMVASvc::execute(xAOD::CaloCluster* cluster,
				 const xAOD::EgammaParameters::EgammaType egType) const
{
  if (!cluster) {
    ATH_MSG_ERROR("Invalid Pointer to egamma or cluster object");
    return StatusCode::FAILURE;
  }
  double mvaE = getEnergy(cluster, egType);
  ATH_MSG_DEBUG( "Calculated MVA calibrated energy = " << mvaE );
  if (mvaE > 0) {
    cluster->setCalE(mvaE);
  }
  else {
    cluster->setCalE(cluster->e());
  }
  return StatusCode::SUCCESS;
}

StatusCode egammaMVASvc::hltexecute(xAOD::CaloCluster* cluster, const std::string& egType) const
{
  if(!cluster){
    ATH_MSG_ERROR("Invalid Pointer to cluster object");
    return StatusCode::FAILURE;
  }
  double mvaE = getEnergy(cluster, egType);
  ATH_MSG_DEBUG( "Calculated MVA calibrated energy = " << mvaE );
  if(mvaE > 0){
    cluster->setCalE(mvaE);
  }
  else{
    cluster->setCalE(cluster->e());
  }
  return StatusCode::SUCCESS;
}

float egammaMVASvc::getEnergy(const xAOD::CaloCluster* cluster, 
			       const xAOD::Egamma* eg){
  ATH_MSG_DEBUG("In execute...");
  // Check for errors...
  if ( !eg ){
    ATH_MSG_WARNING("no xAOD::Egamma object provided");
    return 0;
  }
  if( eg->type() == xAOD::Type::Electron  ){
    ATH_MSG_DEBUG("Processing for electron");
    return getEnergy(cluster, static_cast<const xAOD::Electron*>(eg));
  }
  else if (eg->type() == xAOD::Type::Photon ){
    ATH_MSG_DEBUG("Processing for photon");
    // this is because topo seeded electron (author == 128) have cluster in
    // another collection, which is not decorated with etaCalo, m_cl_phiCalo
    // needed by the MVA calibration
    // TODO: make more general: no input -> no MVA
    if (eg->author() == 128) { return cluster->e(); }

    return getEnergy(cluster, static_cast<const xAOD::Photon*>(eg));
  }
  else{
    ATH_MSG_INFO("Unknown Type");
  }
  return 0;
}


float egammaMVASvc::getEnergy(const xAOD::CaloCluster* cluster, 
			       const std::string& egType){
  // Check for errors...
  if ( !cluster ){
    ATH_MSG_WARNING("no xAOD::CaloCluster object provided");
    return 0;
  }
  if (egType == "Electron") {
    ATH_MSG_DEBUG("Processing  for type electron");
    m_MVATreeElectron->update(nullptr, cluster);
    return m_mvaElectron->getMVAEnergy();
  }
  else if(egType == "Photon"){
    ATH_MSG_DEBUG("Processing for type photon");
    m_MVATreePhoton->update(nullptr, cluster);
    return m_mvaPhoton->getMVAEnergy();
  }
  else {
    ATH_MSG_WARNING("Unknown particle type");
  }
  return 0;
}

float egammaMVASvc::getEnergy(const xAOD::CaloCluster* cluster, 
			       const xAOD::EgammaParameters::EgammaType egType){
  return  ( (egType==xAOD::EgammaParameters::electron) ?  
	   getEnergy(cluster, "Electron") : 
	   getEnergy(cluster, "Photon"));
}


float egammaMVASvc::getEnergy(const xAOD::CaloCluster* cluster, 
			       const xAOD::Electron* el){
  if(!el){
    ATH_MSG_ERROR("No electron passed");
    return 0;
  }
  ATH_MSG_DEBUG("updating variables electron");
  m_MVATreeElectron->update(el, cluster);
  return m_mvaElectron->getMVAEnergy();
}

float egammaMVASvc::getEnergy(const xAOD::CaloCluster* cluster, 
			       const xAOD::Photon* ph){
  if(!ph){
    ATH_MSG_ERROR("No photon passed");
    return 0;
  }
  ATH_MSG_DEBUG("updating variables photon");
  m_MVATreePhoton->update(ph, cluster);
  return m_mvaPhoton->getMVAEnergy();
}

