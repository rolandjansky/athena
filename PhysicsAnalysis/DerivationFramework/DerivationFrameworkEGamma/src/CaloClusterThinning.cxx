/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// CaloClusterThinning.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Simone Mazza (simone.mazza@mi.infn.it)

#include "DerivationFrameworkEGamma/CaloClusterThinning.h"
#include "DerivationFrameworkEGamma/ClustersInCone.h"

#include "ExpressionEvaluation/ExpressionParser.h"
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "ExpressionEvaluation/SGNTUPProxyLoader.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODEgamma/EgammaContainer.h"

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
  //m_ntotFrwd(0),
  m_npass(0),
  m_npassTopo(0),
  //m_npassFrwd(0),
  m_is_muons(false),
  m_is_egamma(false),
  m_run_calo(false),
  m_run_topo(false),
  m_sgKey(""),
  m_CaloClSGKey(""),
  m_TopoClSGKey(""),
  //m_FrwdClSGKey("ForwardElectronClusters"),
  m_selectionString(""),
  m_coneSize(-1.0),
  m_and(false),
  m_keep(true)
{
  declareInterface<DerivationFramework::IThinningTool>(this);
  declareProperty("ThinningService", m_thinningSvc);
  declareProperty("SGKey", m_sgKey);
  declareProperty("CaloClCollectionSGKey", m_CaloClSGKey);
  declareProperty("TopoClCollectionSGKey", m_TopoClSGKey);
  //declareProperty("FrwdClCollectionSGKey", m_FrwdClSGKey);
  declareProperty("SelectionString", m_selectionString);
  declareProperty("ConeSize", m_coneSize);
  declareProperty("ApplyAnd", m_and);
  declareProperty("AlwaysKeepLinkedClusters", m_keep);
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
  if(m_run_calo) nClusters = importedCaloCluster->size();
  unsigned int nTopoClusters = 0;
  if(m_run_topo) nTopoClusters = importedTopoCaloCluster->size();
  //unsigned int nFrwdClusters = importedFrwdCaloCluster->size();
  if (nClusters==0 && nTopoClusters==0 /*|| nFrwdClusters==0*/) return StatusCode::SUCCESS;


  // Set up a mask with the same entries as the full TrackParticle collection(s)
  std::vector<bool> mask, topomask;//, frwdmask;
  mask.assign(nClusters,false); // default: don't keep any clusters
  topomask.assign(nTopoClusters,false);
  //frwdmask.assign(nTopoClusters,false);
  m_ntot += nClusters;
  m_ntotTopo += nTopoClusters;
  //m_ntotFrwd += nFrwdClusters;

  // Retrieve particle container

  //const xAOD::EgammaContainer* importedParticles(0);
  const xAOD::IParticleContainer* importedParticles(0);
  if (evtStore()->retrieve(importedParticles,m_sgKey).isFailure()) {
    ATH_MSG_ERROR("No collection with name " << m_sgKey << " found in StoreGate!");
    return StatusCode::FAILURE;
  }

  // Execute the text parsers if requested
  unsigned int nEgammas(importedParticles->size());
  if (nEgammas==0) return StatusCode::SUCCESS;
  std::vector< const xAOD::IParticle* > particlesToCheck; particlesToCheck.clear();
  if (m_selectionString!="") {
    std::vector<int> entries =  m_parser->evaluateAsVector();
    unsigned int nEntries = entries.size();
    // check the sizes are compatible
    if (nEgammas!= nEntries ) {
      ATH_MSG_ERROR("Sizes incompatible! Are you sure your selection string used e-gamma objects??");
      return StatusCode::FAILURE;
    } else {
      // identify which e-gammas to keep for the thinning check
      for (unsigned int i=0; i<nEgammas; ++i) if (entries[i]==1) particlesToCheck.push_back((*importedParticles)[i]);
    }
  }

  // Are we dealing with a compatible collection?
  const xAOD::ElectronContainer* testElectrons = dynamic_cast<const xAOD::ElectronContainer*>(importedParticles);
  const xAOD::PhotonContainer* testPhotons = dynamic_cast<const xAOD::PhotonContainer*>(importedParticles);
  bool isEgamma(false), isMuons(false);
  if (testElectrons || testPhotons) isEgamma = true;
  const xAOD::MuonContainer* testMuons = dynamic_cast<const xAOD::MuonContainer*>(importedParticles);
  if (testMuons) isMuons = true;
  if( !(isEgamma || isMuons) ) ATH_MSG_ERROR("This tool only works with Egamma and Muons, " << m_sgKey << " is not a compatible collection");

  m_is_muons = isMuons;
  m_is_egamma = isEgamma;

  // Always keep own particle clusters
  if(m_keep){
    for (auto particle : *importedParticles){
      if(m_run_calo) if( particleCluster(mask, particle, importedCaloCluster) != StatusCode::SUCCESS ) return StatusCode::FAILURE;
      if(m_run_topo) if( particleCluster(topomask, particle, importedTopoCaloCluster) != StatusCode::SUCCESS ) return StatusCode::FAILURE;
    }
  }

  // Set elements in the mask to true if there is a corresponding ElementLink from a reconstructed object
  if (m_selectionString=="") { // check all objects as user didn't provide a selection string
    if(m_run_calo) if( setClustersMask(mask, importedParticles, importedCaloCluster) != StatusCode::SUCCESS ) return StatusCode::FAILURE;
    if(m_run_topo) if( setClustersMask(topomask, importedParticles, importedTopoCaloCluster) != StatusCode::SUCCESS ) return StatusCode::FAILURE;
  } else { // check only photons passing user selection string
    if(m_run_calo) if( setClustersMask(mask, particlesToCheck, importedCaloCluster) != StatusCode::SUCCESS ) return StatusCode::FAILURE;
    if(m_run_topo) if( setClustersMask(topomask, particlesToCheck, importedTopoCaloCluster) != StatusCode::SUCCESS ) return StatusCode::FAILURE;
  }

  // Count up the mask contents
  for (unsigned int i=0; i<nClusters; ++i) {
    if (mask[i]) ++m_npass;
  }
  for (unsigned int i=0; i<nTopoClusters; ++i) {
    if (topomask[i]) ++m_npassTopo;
  }

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
                                                                const xAOD::IParticleContainer* particles,
                                                                const xAOD::CaloClusterContainer*& cps) const{


  if(!cps) {
    ATH_MSG_ERROR("Cluster collection not found");
		return StatusCode::FAILURE;
  }else{
    DerivationFramework::ClustersInCone clIC;
    for (auto particle : *particles) {
      if(m_is_egamma){
	const xAOD::Egamma* egamma = dynamic_cast<const xAOD::Egamma*>(particle);
	if(!egamma) {
	  ATH_MSG_ERROR("Egamma cast failed");
		return StatusCode::FAILURE;
	}else{ //To Simone: Avoid derefencing NULL, either would return SC if !egamma or else here
	  clIC.select(egamma, m_coneSize, cps, mask); // check clusters in a cone around the particles
	}
      }
      if(m_is_muons){
	const xAOD::Muon* muon = dynamic_cast<const xAOD::Muon*>(particle);
	if(!muon) {
	  ATH_MSG_ERROR("Muon cast failed");
		return StatusCode::FAILURE;
	}
	else{ //To Simone: Avoid derefencing NULL, either would return SC if !muon or else here
	  clIC.select(muon, m_coneSize, cps, mask); // check clusters in a cone around the particles
	}
      }
    }
  }
	return StatusCode::SUCCESS;
}
StatusCode DerivationFramework::CaloClusterThinning::setClustersMask( std::vector<bool>& mask,
                                                                std::vector<const xAOD::IParticle*>& particles,
                                                                const xAOD::CaloClusterContainer*& cps) const{

  if(!cps) { //To simone maybe you should make these methods return a status code and return after an error instead of my if - else
      ATH_MSG_ERROR("Cluster collection not found");
			return StatusCode::FAILURE;
  }
  else{
    DerivationFramework::ClustersInCone clIC;
    for (auto particle : particles) {
      if(m_is_egamma){
	const xAOD::Egamma* egamma = dynamic_cast<const xAOD::Egamma*>(particle);
	if(!egamma) {
	  ATH_MSG_ERROR("Egamma cast failed");
		return StatusCode::FAILURE;
	}else{
	clIC.select(egamma, m_coneSize, cps, mask); // check clusters in a cone around the particles
	}
      }
      if(m_is_muons){
	const xAOD::Muon* muon = dynamic_cast<const xAOD::Muon*>(particle);
	if(!muon) {
	  ATH_MSG_ERROR("Muon cast failed");
		return StatusCode::FAILURE;
	}
	else{
	clIC.select(muon, m_coneSize, cps, mask); // check clusters in a cone around the particles
	}
      }
    }
  }
	return StatusCode::SUCCESS;
}

template <class T> StatusCode DerivationFramework::CaloClusterThinning::particleCluster(  std::vector<bool>& mask,
                                                                                    T*& particle,
                                                                                    const xAOD::CaloClusterContainer*& cps) const{
  if(!cps) {
    ATH_MSG_ERROR("Cluster collection not found");
		return StatusCode::FAILURE;
  }
  else {
    // Are we dealing with egamma or muons?
    if (m_is_egamma){
      const xAOD::Egamma* egamma = dynamic_cast<const xAOD::Egamma*>(particle);
      if(!egamma) {
	ATH_MSG_ERROR("Egamma cast failed");
	return StatusCode::FAILURE;
      }
      else{
	auto it = std::find (cps->begin(), cps->end(), egamma->caloCluster());
	if(it != cps->end()){
	  int ItsCluster = std::distance(cps->begin(), it);
	  mask[ItsCluster] = true;
	}
      }
    }

    if (m_is_muons){
      const xAOD::Muon* muon = dynamic_cast<const xAOD::Muon*>(particle);
      if(!muon) {
	ATH_MSG_ERROR("Muon cast failed");
	return StatusCode::FAILURE;
      }else{
	auto it = std::find (cps->begin(), cps->end(), muon->cluster());
	if(it != cps->end()){
	  int ItsCluster = std::distance(cps->begin(), it);
	  mask[ItsCluster] = true;
	}
      }
    }
  }
	return StatusCode::SUCCESS;
}
