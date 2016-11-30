/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EMClusterTool.h"

#include "egammaInterfaces/IegammaSwTool.h"
#include "egammaMVACalib/IegammaMVATool.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"
#include "StoreGate/StoreGateSvc.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"

#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloUtils/CaloCellDetPos.h"
#include "CxxUtils/make_unique.h"


// =============================================================
EMClusterTool::EMClusterTool(const std::string& type, const std::string& name, const IInterface* parent) :
  egammaBaseTool(type, name, parent)
{
  declareProperty("ClusterCorrectionToolName", m_ClusterCorrectionToolName = "egammaSwTool/egammaswtool");
  declareProperty("MVACalibTool", m_MVACalibTool);
  declareProperty("OutputClusterContainerName", m_outputClusterContainerName, 
    "Name of the output cluster container");
  declareProperty("OutputTopoSeededClusterContainerName",
    m_outputTopoSeededClusterContainerName, 
    "Name of the output cluster container for topo-seeded clusters (can be the same as the other clusters)");
  declareProperty("ElectronContainerName", m_electronContainerName, 
    "Name of the input electron container");
  declareProperty("PhotonContainerName", m_photonContainerName, 
    "Name of the input photon container");
  declareProperty("doSuperCluster", m_doSuperClusters = true, 
    "Do Super Cluster Reco");
  declareProperty("applyMVAToSuperCluster", m_applySuperClusters = true, 
    "Protection to not do anything for superClusters");


  declareInterface<IEMClusterTool>(this);
  
  m_caloCellDetPos = CxxUtils::make_unique<CaloCellDetPos>();
}

EMClusterTool::~EMClusterTool() {  }

// const InterfaceID& ClusterTool::interfaceID() {
//   return IID_IClusterTool;
// }

// =============================================================
StatusCode EMClusterTool::initialize() {

  ATH_MSG_DEBUG("Initializing " << name() << "...");

  // Get the cluster correction tool
  m_clusterCorrectionTool = ToolHandle<IegammaSwTool>(m_ClusterCorrectionToolName);
  if(m_clusterCorrectionTool.retrieve().isFailure()) {
    ATH_MSG_ERROR("Failed to retrieve " << m_clusterCorrectionTool);
    return StatusCode::SUCCESS;
  } 
  else {
    ATH_MSG_DEBUG("Retrieved tool " << m_clusterCorrectionTool);   
  }


  // Get the cluster correction tool
  if(m_MVACalibTool.retrieve().isFailure()) {
    ATH_MSG_ERROR("Failed to retrieve " << m_MVACalibTool);
    return StatusCode::SUCCESS;
  } 
  else {
    ATH_MSG_DEBUG("Retrieved tool " << m_MVACalibTool);   
  }

  ATH_MSG_DEBUG("Initialization successful");

  return StatusCode::SUCCESS;
}

// =========================================================================
StatusCode EMClusterTool::finalize() {
  return StatusCode::SUCCESS;
}

// =========================================================================
StatusCode EMClusterTool::contExecute() 
{
  // Create output cluster container and register in StoreGate
  xAOD::CaloClusterContainer* outputClusterContainer = 0;

  outputClusterContainer = CaloClusterStoreHelper::makeContainer(&*evtStore(), 
								 m_outputClusterContainerName, 
								 msg());
  if (!outputClusterContainer) {
    ATH_MSG_ERROR("Failed to record Output Cluster Container " 
		  << m_outputClusterContainerName);
    return StatusCode::FAILURE;
  }

  // Create output cluster container for topo-seeded clusters and register in StoreGate
  // Only if they differ from the main output cluster container
  // and if we do not do supercluster
  xAOD::CaloClusterContainer* outputTopoSeededClusterContainer = outputClusterContainer;
  bool doTopoSeededContainer= (m_outputTopoSeededClusterContainerName != m_outputClusterContainerName && !m_doSuperClusters);
  if(doTopoSeededContainer){
    outputTopoSeededClusterContainer = CaloClusterStoreHelper::makeContainer(&*evtStore(), 
									     m_outputTopoSeededClusterContainerName, 
									     msg());  
    if (!outputTopoSeededClusterContainer){
      ATH_MSG_ERROR("Failed to record Output Topo-seeded Cluster Container " 
		    << m_outputTopoSeededClusterContainerName);
      return StatusCode::FAILURE;
    }
  }
  
  // Retrieve electron and photon containers
  xAOD::ElectronContainer *electronContainer = nullptr;
  ATH_CHECK (evtStore()->retrieve(electronContainer, m_electronContainerName) );
  
  xAOD::PhotonContainer *photonContainer = nullptr;
  ATH_CHECK( evtStore()->retrieve(photonContainer, m_photonContainerName) );
    
  // Loop over electrons and create new clusters
  xAOD::EgammaParameters::EgammaType egType = xAOD::EgammaParameters::electron;
  //Only do this for non-supercluster (i.e. default SW) electrons.
  for (auto electron : *electronContainer){
    setNewCluster(electron, outputClusterContainer, egType);
  }
  // Loop over photons and create new clusters
  for (auto photon : *photonContainer){
    egType = (xAOD::EgammaHelpers::isConvertedPhoton(photon) ? 
	      xAOD::EgammaParameters::convertedPhoton :
	      xAOD::EgammaParameters::unconvertedPhoton);
    
    if ( !photon->author(xAOD::EgammaParameters::AuthorCaloTopo35) ){
      setNewCluster(photon, outputClusterContainer, egType);
    }
    else{
      setNewCluster(photon, outputTopoSeededClusterContainer, egType);
    }
  }
  return StatusCode::SUCCESS;
}

// ==========================================================================
void EMClusterTool::setNewCluster(xAOD::Egamma *eg,
                                  xAOD::CaloClusterContainer *outputClusterContainer,
                                  xAOD::EgammaParameters::EgammaType egType)
{
  if (!eg) {return;}

  if (!eg->caloCluster()){
    ATH_MSG_DEBUG("egamma object does not have a cluster associated");
    return;
  }
  typedef ElementLink<xAOD::CaloClusterContainer> ClusterLink_t;
  xAOD::CaloCluster* cluster = 0;

  //Special Case for topo seeded photons. 
  if (eg->author(xAOD::EgammaParameters::AuthorCaloTopo35)) {
    cluster = new xAOD::CaloCluster(*(eg->caloCluster()));
    fillPositionsInCalo(cluster);
  } // Doing superClusters
  else if ( m_doSuperClusters){
    //copy over for super clusters 
    cluster = makeNewSuperCluster(*(eg->caloCluster()),eg);
  }
  else {
    cluster = makeNewCluster(*(eg->caloCluster()), eg, egType);
  }
  outputClusterContainer->push_back(cluster);

  // Set the link to the new cluster
  ClusterLink_t clusterLink(cluster, *outputClusterContainer);
  const std::vector<ClusterLink_t> clusterLinks{clusterLink};
  eg->setCaloClusterLinks( clusterLinks );
  
}

// ==========================================================================
xAOD::CaloCluster* EMClusterTool::makeNewCluster(const xAOD::CaloCluster& cluster, xAOD::Egamma *eg, 
						 xAOD::EgammaParameters::EgammaType egType) {
  //
  // Create new cluster based on an existing one
  // const CaloCluster* cluster : input cluster
  //
  // protection against cluster not in barrel nor endcap
  if (!cluster.inBarrel() && !cluster.inEndcap() ){
    ATH_MSG_ERROR("Cluster neither in barrel nor in endcap, Skipping cluster");
    return 0;
  }
  
  if ((int) egType < 0 || egType >= xAOD::EgammaParameters::NumberOfEgammaTypes){
    ATH_MSG_WARNING("Invalid egamma type");
    return 0;
  }

  bool isBarrel = xAOD::EgammaHelpers::isBarrel(&cluster);  
  xAOD::CaloCluster::ClusterSize cluSize=xAOD::CaloCluster:: CSize_Unknown;
  if (egType == xAOD::EgammaParameters::electron){
    cluSize= (isBarrel ? xAOD::CaloCluster::SW_37ele : xAOD::CaloCluster::SW_55ele);
  }
  else if (egType == xAOD::EgammaParameters::convertedPhoton){
    cluSize= (isBarrel ? xAOD::CaloCluster::SW_37Econv : xAOD::CaloCluster::SW_55Econv);
  }
  else  {//(egType == xAOD::EgammaParameters::unconvertedPhoton)
    cluSize = (isBarrel ? xAOD::CaloCluster::SW_37gam : xAOD::CaloCluster::SW_55gam);
  }
  xAOD::CaloCluster *newCluster = makeNewCluster(cluster, cluSize);
 
  if (newCluster && m_MVACalibTool->execute(newCluster,eg).isFailure()){
    ATH_MSG_ERROR("Problem executing MVA cluster tool");
  }
  return newCluster;
}

// ==========================================================================
xAOD::CaloCluster* EMClusterTool::makeNewCluster(const xAOD::CaloCluster& cluster, 
						 const xAOD::CaloCluster::ClusterSize& cluSize) {

  xAOD::CaloCluster* newClus = CaloClusterStoreHelper::makeCluster(cluster.getCellLinks()->getCellContainer(),
								   cluster.eta0(),cluster.phi0(),
								   cluSize);
  if(newClus){  
    if (m_clusterCorrectionTool->execute(newClus).isFailure()){
      ATH_MSG_ERROR("Problem executing cluster correction tool");
    }
    fillPositionsInCalo(newClus);
    //Fill the raw state using the cluster with correct size  but not MVA
    newClus->setRawE(newClus->calE());
    newClus->setRawEta(newClus->calEta());
    newClus->setRawPhi(newClus->calPhi());
    //Fill the Alt state using the 3x5 cluster
    newClus->setAltE(cluster.calE());
    newClus->setAltEta(cluster.calEta());
    newClus->setAltPhi(cluster.calPhi());
  }
  else {
    ATH_MSG_ERROR("Null newClus");
  }

  return newClus;
}
xAOD::CaloCluster* EMClusterTool::makeNewSuperCluster(const xAOD::CaloCluster& cluster,xAOD::Egamma *eg) {
  //
  xAOD::CaloCluster* newClus = new xAOD::CaloCluster(cluster);
  if(m_applySuperClusters){ 
    if (newClus && m_MVACalibTool->execute(newClus,eg).isFailure()){
      ATH_MSG_ERROR("Problem executing MVA cluster tool");
    }
  }
 
  ATH_MSG_DEBUG("Cluster Energy after final calibration: "<<newClus->e());

  return newClus;
}
// ==========================================================================
void EMClusterTool::fillPositionsInCalo(xAOD::CaloCluster* cluster) const{

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
