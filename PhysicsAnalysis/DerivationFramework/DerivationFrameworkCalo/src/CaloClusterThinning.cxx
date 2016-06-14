/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// CaloClusterThinning.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Simone Mazza (simone.mazza@mi.infn.it)

#include "DerivationFrameworkCalo/CaloClusterThinning.h"
#include "DerivationFrameworkCalo/ClustersInCone.h"

#include "ExpressionEvaluation/ExpressionParser.h"
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "ExpressionEvaluation/SGNTUPProxyLoader.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODJet/JetContainer.h"

#include <vector>
#include <string>
#include <algorithm>

// Constructor
DerivationFramework::CaloClusterThinning::CaloClusterThinning(const std::string& t,
							      const std::string& n,
							      const IInterface* p ) :
  AthAlgTool(t,n,p),
  m_thinningSvc("ThinningSvc",n),
  m_ntot(0),
  m_ntotTopo(0),
  m_npass(0),
  m_npassTopo(0),
  m_is_muons(false),
  m_is_egamma(false),
  m_is_tau(false),
  m_run_calo(false),
  m_run_topo(false),
  m_sgKey(""),
  m_CaloClSGKey(""),
  m_TopoClSGKey(""),
  m_selectionString(""),
  m_coneSize(-1.0),
  m_and(false),
  m_parser(0)
{
  declareInterface<DerivationFramework::IThinningTool>(this);
  declareProperty("ThinningService", m_thinningSvc);
  declareProperty("SGKey", m_sgKey);
  declareProperty("CaloClCollectionSGKey", m_CaloClSGKey);
  declareProperty("TopoClCollectionSGKey", m_TopoClSGKey);
  declareProperty("SelectionString", m_selectionString);
  declareProperty("ConeSize", m_coneSize);
  declareProperty("ApplyAnd", m_and);
}

// Destructor
DerivationFramework::CaloClusterThinning::~CaloClusterThinning() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::CaloClusterThinning::initialize()
{
  // Decide which collections need to be checked for ID TrackParticles
  ATH_MSG_VERBOSE("initialize() ...");

  if (m_CaloClSGKey!=""){
    m_run_calo = true;
    ATH_MSG_INFO("Using " << m_CaloClSGKey << "as the source collection for calo clusters");
  }
  if (m_TopoClSGKey!=""){
    m_run_topo = true;
    ATH_MSG_INFO("Using " << m_TopoClSGKey << "as the source collection for topo calo clusters");
  }
  if (m_CaloClSGKey=="" && m_TopoClSGKey==""){
    ATH_MSG_FATAL("No CalCaloTopoCluster or CaloCluster collection provided for thinning.");
    return StatusCode::FAILURE;
  }

  if (m_sgKey=="") {
    ATH_MSG_FATAL("No particle collection provided for thinning.");
    return StatusCode::FAILURE;
  } else { ATH_MSG_INFO("Calo clusters associated with objects in " << m_sgKey << " will be retained in this format with the rest being thinned away");}

  // Set up the text-parsing machinery for selectiong the photon directly according to user cuts
  if (m_selectionString!="") {
    ExpressionParsing::MultipleProxyLoader *proxyLoaders = new ExpressionParsing::MultipleProxyLoader();
    proxyLoaders->push_back(new ExpressionParsing::SGxAODProxyLoader(evtStore()));
    proxyLoaders->push_back(new ExpressionParsing::SGNTUPProxyLoader(evtStore()));
    if (m_selectionString!="") {
      m_parser = new ExpressionParsing::ExpressionParser(proxyLoaders);
      m_parser->loadExpression(m_selectionString);
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::CaloClusterThinning::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  ATH_MSG_INFO("Processed "<< m_ntot <<" clusters, of which "<< m_npass<< " were retained ");
  ATH_MSG_INFO("Processed "<< m_ntotTopo <<" topo clusters, of which "<< m_npassTopo << " were retained ");
  if (m_selectionString!="") {
    delete m_parser;
    m_parser = 0;
  }
  return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode DerivationFramework::CaloClusterThinning::doThinning() const
{
  m_is_muons = false;
  m_is_egamma = false;
  m_is_tau = false;

  // Retrieve egCluster collection
  const xAOD::CaloClusterContainer* importedCaloCluster;
  if(m_run_calo){
    if (evtStore()->retrieve(importedCaloCluster,m_CaloClSGKey).isFailure()) {
      ATH_MSG_ERROR("No CaloCluster collection with name " << m_CaloClSGKey << " found in StoreGate!");
      return StatusCode::FAILURE;
    }
  }

  // Retrieve CalCaloTopo collection if required
  const xAOD::CaloClusterContainer* importedTopoCaloCluster;
  if(m_run_topo){
    if (evtStore()->retrieve(importedTopoCaloCluster,m_TopoClSGKey).isFailure()) {
      ATH_MSG_ERROR("No CalCaloTopo collection with name " << m_TopoClSGKey << " found in StoreGate!");
      return StatusCode::FAILURE;
    }
  }
  // Check the event contains tracks
  unsigned int nClusters = 0;
  if(m_run_calo) {
    nClusters = importedCaloCluster->size();
  }
  unsigned int nTopoClusters = 0;
  if(m_run_topo) {
    nTopoClusters = importedTopoCaloCluster->size();
  }
  if (nClusters==0 && nTopoClusters==0 ) {
    return StatusCode::SUCCESS;
  }

  // Set up a mask with the same entries as the full TrackParticle collection(s)
  std::vector<bool> mask(nClusters,false), topomask(nTopoClusters,false);
  m_ntot += nClusters;
  m_ntotTopo += nTopoClusters;

  // Retrieve particle container
  const xAOD::IParticleContainer* importedParticles(0);
  if (evtStore()->retrieve(importedParticles,m_sgKey).isFailure()) {
    ATH_MSG_ERROR("No collection with name " << m_sgKey << " found in StoreGate!");
    return StatusCode::FAILURE;
  }

  //No particles in the input
  unsigned int nEgammas(importedParticles->size());
  if (nEgammas==0) {
    return StatusCode::SUCCESS;
  }

  m_is_egamma =false;
  m_is_muons=false;

  // Are we dealing with a compatible collection?
  const xAOD::ElectronContainer* testElectrons = dynamic_cast<const xAOD::ElectronContainer*>(importedParticles);
  const xAOD::PhotonContainer* testPhotons = dynamic_cast<const xAOD::PhotonContainer*>(importedParticles);
  if (testElectrons || testPhotons) {
    m_is_egamma = true;
  }
  const xAOD::MuonContainer* testMuons = dynamic_cast<const xAOD::MuonContainer*>(importedParticles);
  if (testMuons) {
    m_is_muons  = true;
  }
  const xAOD::TauJetContainer* testTaus = dynamic_cast<const xAOD::TauJetContainer*>(importedParticles);
  if (testTaus) {
    m_is_tau = true;
  }
  
  if( !(m_is_egamma || m_is_muons || m_is_tau) ) {
    ATH_MSG_ERROR("This tool only works with Egamma, Muons and Taus, " << m_sgKey << " is not a compatible collection");
    return StatusCode::FAILURE;
  }

  //Selection 
  // Execute the text parsers if requested
  if (m_selectionString!="") {
    std::vector<int> entries =  m_parser->evaluateAsVector();
    unsigned int nEntries = entries.size();
    // check the sizes are compatible
    if (nEgammas!= nEntries ) {
      ATH_MSG_ERROR("Sizes incompatible! Are you sure your selection string used e-gamma objects??");
      return StatusCode::FAILURE;
    }
    std::vector< const xAOD::IParticle* > particlesToCheck ={}; 
    // identify which e-gammas to keep for the thinning check
    for (unsigned int i=0; i<nEgammas; ++i) {
      if (entries[i]==true) {
	particlesToCheck.push_back((*importedParticles)[i]);
      }
    }
    ////
    for (auto particle : particlesToCheck){  
      if(m_run_calo) { 
	// Always keep own particle clusters
	if( particleCluster(mask, particle, importedCaloCluster) != StatusCode::SUCCESS ) {
	  return StatusCode::FAILURE;
	}
	if( setClustersMask(mask, particle, importedCaloCluster) != StatusCode::SUCCESS ) {
	  return StatusCode::FAILURE;
	}
      }
      if(m_run_topo) {
	if( particleCluster(topomask, particle, importedTopoCaloCluster) != StatusCode::SUCCESS ) {
	  return StatusCode::FAILURE;
	}
	if( setClustersMask(topomask, particle, importedTopoCaloCluster) != StatusCode::SUCCESS ) {
	  return StatusCode::FAILURE;
	}
      }
    }
  }
  else{//No selection string provided
    for (auto particle : *importedParticles){
      if(m_run_calo) {
	if( particleCluster(mask, particle, importedCaloCluster) != StatusCode::SUCCESS ) {
	  return StatusCode::FAILURE;
	}
	if( setClustersMask(mask, particle, importedCaloCluster) != StatusCode::SUCCESS ) {
	  return StatusCode::FAILURE;
	}
      }
      if(m_run_topo) {
	if( particleCluster(topomask, particle, importedTopoCaloCluster) != StatusCode::SUCCESS ) {
	  return StatusCode::FAILURE;
	}
	if( setClustersMask(topomask, particle, importedTopoCaloCluster) != StatusCode::SUCCESS ) {
	  return StatusCode::FAILURE;
	}
      }
    }
  }
  //
  // Count up the mask contents
  for (unsigned int i=0; i<nClusters; ++i) {
    if (mask[i]) ++m_npass;
  }
  for (unsigned int i=0; i<nTopoClusters; ++i) {
    if (topomask[i]) ++m_npassTopo;
  }
  //
  // Execute the thinning service based on the mask. Finish.
  if (m_and) {
    if(m_run_calo){
      if (m_thinningSvc->filter(*importedCaloCluster, mask, IThinningSvc::Operator::And).isFailure()) {
	ATH_MSG_ERROR("Application of thinning service failed! ");
	return StatusCode::FAILURE;
      }
    }
    if(m_run_topo) {
      if (m_thinningSvc->filter(*importedTopoCaloCluster, topomask, IThinningSvc::Operator::And).isFailure()) {
	ATH_MSG_ERROR("Application of thinning service failed! ");
	return StatusCode::FAILURE;
      }
    }
  }
  if (!m_and) {
    if(m_run_calo){
      if (m_thinningSvc->filter(*importedCaloCluster, mask, IThinningSvc::Operator::Or).isFailure()) {
	ATH_MSG_ERROR("Application of thinning service failed! ");
	return StatusCode::FAILURE;
      }
    }
    if(m_run_topo) {
      if (m_thinningSvc->filter(*importedTopoCaloCluster, topomask, IThinningSvc::Operator::Or).isFailure()) {
	ATH_MSG_ERROR("Application of thinning service failed! ");
	return StatusCode::FAILURE;
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::CaloClusterThinning::setClustersMask( std::vector<bool>& mask,
                                                                const xAOD::IParticle* particle,
                                                                const xAOD::CaloClusterContainer* cps) const{

  if(!cps) {
    ATH_MSG_ERROR("Cluster collection not found");
    return StatusCode::FAILURE;
  }
   
  if(m_is_egamma){
    const xAOD::Egamma* egamma = dynamic_cast<const xAOD::Egamma*>(particle);
    if(!egamma) {
      ATH_MSG_ERROR("Egamma cast failed");
      return StatusCode::FAILURE;
    }
    DerivationFramework::ClustersInCone::select(egamma, m_coneSize, cps, mask); 
  }    
  if(m_is_muons){
    const xAOD::Muon* muon = dynamic_cast<const xAOD::Muon*>(particle);
    if(!muon) {
      ATH_MSG_ERROR("Muon cast failed");
      return StatusCode::FAILURE;
    }
    DerivationFramework::ClustersInCone::select(muon, m_coneSize, cps, mask); 
  }
  return StatusCode::SUCCESS;
}


StatusCode DerivationFramework::CaloClusterThinning::particleCluster(  std::vector<bool>& mask,
								       const xAOD::IParticle* particle,
								       const xAOD::CaloClusterContainer* cps) const{
  if(!cps) {
    ATH_MSG_ERROR("Cluster collection not found");
    return StatusCode::FAILURE;
  }

  if (m_is_egamma){
    const xAOD::Egamma* egamma = dynamic_cast<const xAOD::Egamma*>(particle);
    if(!egamma) {
      ATH_MSG_ERROR("Egamma cast failed");
      return StatusCode::FAILURE;
    }
    auto it = std::find (cps->begin(), cps->end(), egamma->caloCluster());
    if(it != cps->end()){
      int ItsCluster = std::distance(cps->begin(), it);
      mask[ItsCluster] = true;
    }
  }
    
  if (m_is_muons){
    const xAOD::Muon* muon = dynamic_cast<const xAOD::Muon*>(particle);
    if(!muon) {
      ATH_MSG_ERROR("Muon cast failed");
      return StatusCode::FAILURE;
    }
    auto it = std::find (cps->begin(), cps->end(), muon->cluster());
    if(it != cps->end()){
      int ItsCluster = std::distance(cps->begin(), it);
      mask[ItsCluster] = true;
    }
  }

  if (m_is_tau){
    const xAOD::TauJet* tau = dynamic_cast<const xAOD::TauJet*> (particle);
    if(!tau){
      ATH_MSG_ERROR("TauJet cast failed");
      return StatusCode::FAILURE;
    }
    xAOD::JetConstituentVector vec = tau->jet()->getConstituents();
    xAOD::JetConstituentVector::iterator it = vec.begin();
    xAOD::JetConstituentVector::iterator itE = vec.end();
    TLorentzVector LC_P4=tau->p4(xAOD::TauJetParameters::DetectorAxis);    
    for( ; it!=itE; it++){
      TLorentzVector cluster_P4;
      cluster_P4.SetPtEtaPhiM(1,(*it)->Eta(),(*it)->Phi(),1);
      if(LC_P4.DeltaR(cluster_P4)>0.2) continue;
      const xAOD::CaloCluster* cl = dynamic_cast<const xAOD::CaloCluster *>( (*it)->rawConstituent() );
      auto it_cps = std::find(cps->begin(), cps->end(), cl);
      if( it_cps != cps->end()){
	int ItsCluster = std::distance(cps->begin(), it_cps);
	mask[ItsCluster] = true;
      }
    }
  }

  return StatusCode::SUCCESS;
}
