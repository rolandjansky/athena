/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// ClusterEnergyPerLayerDecorator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Ludovica Aperio Bella
#include "egammaInterfaces/IegammaSwTool.h"
#include "DerivationFrameworkCalo/ClusterDecorator.h"
#include "xAODEgamma/EgammaContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloUtils/CaloCellDetPos.h"
#include "CxxUtils/make_unique.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

// Constructor
DerivationFramework::ClusterDecorator::ClusterDecorator(const std::string& t,
						  const std::string& n,
						  const IInterface* p) :
  AthAlgTool(t,n,p),
  m_SGKey_photons(""),
  m_SGKey_electrons(""),
  m_OutputClusterSGKey("EgammaSwClusters")
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("ClusterCorrectionToolName", m_ClusterCorrectionToolName = "egammaSwTool/egammaswtool");
  declareProperty("SGKey_photons", m_SGKey_photons);
  declareProperty("SGKey_electrons", m_SGKey_electrons);
  declareProperty("OutputClusterSGKey",m_OutputClusterSGKey="swCluster");	
  declareProperty("SGKey_caloCells", m_CellCollectionName="AODCellContainer","Name of the CaloCellContainer");
  
  m_caloCellDetPos = CxxUtils::make_unique<CaloCellDetPos>();
}



// Destructor
DerivationFramework::ClusterDecorator::~ClusterDecorator() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::ClusterDecorator::initialize()
{
  ATH_MSG_VERBOSE("initialize() ...");

  if(m_SGKey_photons == "" and m_SGKey_electrons == ""){
    ATH_MSG_FATAL("No e-gamma collection provided");
    return StatusCode::FAILURE;
  }

  // Get the cluster correction tool
  m_clusterCorrectionTool = ToolHandle<IegammaSwTool>(m_ClusterCorrectionToolName);
  if(m_clusterCorrectionTool.retrieve().isFailure()) {
    ATH_MSG_ERROR("Failed to retrieve " << m_clusterCorrectionTool);
    return StatusCode::SUCCESS;
  } 
  else {
    ATH_MSG_DEBUG("Retrieved tool " << m_clusterCorrectionTool);   
  }
  
 
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::ClusterDecorator::finalize()
{

  return StatusCode::SUCCESS;
}

// The decoration itself
StatusCode DerivationFramework::ClusterDecorator::addBranches() const
{
  ///Make new container
 // Create output cluster container and register in StoreGate
  xAOD::CaloClusterContainer* SWClusterContainer = 0;
  SWClusterContainer = CaloClusterStoreHelper::makeContainer(&*evtStore(),                                                                        
							     m_OutputClusterSGKey,                                                         
							     msg());                                                                              
  
  for (const std::string& SGkey : {m_SGKey_photons, m_SGKey_electrons})
  {
    if (SGkey == "") continue;
    const xAOD::EgammaContainer *container(0);
    ATH_CHECK( evtStore()->retrieve(container, SGkey) );
    for (auto egamma : *container) {
      
      if(!decorateObject(egamma,SWClusterContainer).isSuccess()) {
	ATH_MSG_WARNING("no SW cluster container filled");
      }
    }
  }
  
  ///Finalize clusters
  CHECK( CaloClusterStoreHelper::finalizeClusters(&*evtStore(),  
						  SWClusterContainer,
						  m_OutputClusterSGKey,                                          
						  msg()));               
  
  
  return StatusCode::SUCCESS;
}


StatusCode DerivationFramework::ClusterDecorator::decorateObject(const xAOD::Egamma*egamma, xAOD::CaloClusterContainer *outputClusterContainer) const {
  
  const CaloCellContainer* cellCont(0);
  xAOD::CaloCluster* egcClone(0);
  
  if (evtStore()->retrieve(cellCont,m_CellCollectionName).isFailure()) {
    ATH_MSG_WARNING(m_CellCollectionName<< " not found");    
  } else {  
    if (! egamma->author(xAOD::EgammaParameters::AuthorCaloTopo35) ) {
      bool isBarrel = xAOD::EgammaHelpers::isBarrel(egamma);  
      xAOD::CaloCluster::ClusterSize cluSize=xAOD::CaloCluster:: CSize_Unknown;
      if (xAOD::EgammaHelpers::isElectron(egamma)) {
	cluSize= (isBarrel ? xAOD::CaloCluster::SW_37ele : xAOD::CaloCluster::SW_55ele);
      }
      else if ( xAOD::EgammaHelpers::isConvertedPhoton(egamma)){
	cluSize= (isBarrel ? xAOD::CaloCluster::SW_37Econv : xAOD::CaloCluster::SW_55Econv);
      }
      else  {//(egType == xAOD::EgammaParameters::unconvertedPhoton)
	cluSize = (isBarrel ? xAOD::CaloCluster::SW_37gam : xAOD::CaloCluster::SW_55gam);
      }
      
      double PhiCalo = 0.;
      if(! (egamma->caloCluster()->retrieveMoment(xAOD::CaloCluster::PHICALOFRAME, PhiCalo))) {
	ATH_MSG_ERROR("Failed to retrieve phi from calo");
      }
      
      double EtaCalo = 0.;
      if(! (egamma->caloCluster()->retrieveMoment(xAOD::CaloCluster::ETACALOFRAME, EtaCalo))) {
	ATH_MSG_ERROR("Failed to retrieve eta from calo");
      }
      
      double Eta0 =   egamma->caloCluster()->eta0() + EtaCalo - egamma->caloCluster()->eta();
      double Phi0 =   egamma->caloCluster()->phi0() + PhiCalo - egamma->caloCluster()->phi();
      
      if (Phi0 < -M_PI)  Phi0 = (Phi0 + 2*M_PI);
      if (Phi0 > M_PI)   Phi0 = (Phi0 - 2*M_PI);
      
      egcClone = CaloClusterStoreHelper::makeCluster( cellCont,
						      Eta0,
						      Phi0,
						      cluSize);

      if(egcClone ) {
	
	if (m_clusterCorrectionTool->execute(egcClone).isFailure()){
	  ATH_MSG_WARNING("Problem executing cluster correction tool");
	  delete egcClone;
	  ElementLink < xAOD::CaloClusterContainer > ldummyClusterLink;
	  egamma->auxdecor< ElementLink< xAOD::CaloClusterContainer > >("SwClusterLink") = ldummyClusterLink;
	  return StatusCode::SUCCESS;
	} else {
	  
	  //Check to see if cluster doesn't have EMB2 OR EME2. If not, kill it.
	  if (!egcClone->hasSampling(CaloSampling::EMB2) &&  !egcClone->hasSampling(CaloSampling::EME2)) {
	    ATH_MSG_WARNING("Supercluster doesn't have energy in layer 2. Skipping...");
	    delete egcClone;
	    ElementLink < xAOD::CaloClusterContainer > ldummyClusterLink;
	    egamma->auxdecor< ElementLink< xAOD::CaloClusterContainer > >("SwClusterLink") = ldummyClusterLink;
	    return StatusCode::SUCCESS;
	  }
	  if (!egcClone->hasSampling(CaloSampling::EMB1) &&  !egcClone->hasSampling(CaloSampling::EME1)) {
	    ATH_MSG_WARNING("Supercluster doesn't have energy in layer 1. Skipping...");
	    delete egcClone;
	    ElementLink < xAOD::CaloClusterContainer > ldummyClusterLink;
	    egamma->auxdecor< ElementLink< xAOD::CaloClusterContainer > >("SwClusterLink") = ldummyClusterLink;
	    return StatusCode::SUCCESS;
	  }	
	  
	  fillPositionsInCalo(egcClone);
	  //Fill the raw state using the cluster with correct size  but not MVA
	  egcClone->setRawE(egcClone->calE());
	  egcClone->setRawEta(egcClone->calEta());
	  egcClone->setRawPhi(egcClone->calPhi());
	  //Fill the Alt state using the 3x5 cluster
	  egcClone->setAltE(egcClone->calE());
	  egcClone->setAltEta(egcClone->calEta());
	  egcClone->setAltPhi(egcClone->calPhi());
	  
	  outputClusterContainer->push_back(egcClone);
	  ElementLink < xAOD::CaloClusterContainer > lSwClusterLink(egcClone, *outputClusterContainer);
	  egamma->auxdecor< ElementLink< xAOD::CaloClusterContainer > >("SwClusterLink") = lSwClusterLink;
	  return StatusCode::SUCCESS;
	}
      } //egcClone
    } // egammaObject
  } // retrive container 
  
  // Set the link to the new cluster
  ElementLink < xAOD::CaloClusterContainer > ldummyClusterLink;
  egamma->auxdecor< ElementLink< xAOD::CaloClusterContainer > >("SwClusterLink") = ldummyClusterLink;
  return StatusCode::SUCCESS;

}


// ==========================================================================
void DerivationFramework::ClusterDecorator::fillPositionsInCalo(xAOD::CaloCluster* cluster) const{
  
  bool isBarrel = xAOD::EgammaHelpers::isBarrel(cluster);
  CaloCell_ID::CaloSample sample = isBarrel ? CaloCell_ID::EMB2 : CaloCell_ID::EME2;
  // eta and phi of the cluster in the calorimeter frame
  double eta, phi;
  m_caloCellDetPos->getDetPosition(sample, cluster->eta(), cluster->phi(), eta, phi); 
  
  cluster->insertMoment(xAOD::CaloCluster::ETACALOFRAME,eta);
  cluster->insertMoment(xAOD::CaloCluster::PHICALOFRAME,phi);
  
  //  eta in the second sampling
  m_caloCellDetPos->getDetPosition(sample, cluster->etaBE(2), cluster->phiBE(2), eta, phi);
  cluster->insertMoment(xAOD::CaloCluster::ETA2CALOFRAME,eta);
  cluster->insertMoment(xAOD::CaloCluster::PHI2CALOFRAME,phi);
  //  eta in the first sampling 
  sample = isBarrel ? CaloCell_ID::EMB1 : CaloCell_ID::EME1;
  m_caloCellDetPos->getDetPosition(sample, cluster->etaBE(1), cluster->phiBE(1),eta, phi);
  cluster->insertMoment(xAOD::CaloCluster::ETA1CALOFRAME,eta);
  cluster->insertMoment(xAOD::CaloCluster::PHI1CALOFRAME,phi);
  
}
