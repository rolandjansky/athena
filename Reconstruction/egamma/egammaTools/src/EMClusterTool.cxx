/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "EMClusterTool.h"

#include "egammaInterfaces/IegammaSwTool.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "StoreGate/StoreGateSvc.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"

#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloUtils/CaloCellDetPos.h"

// =============================================================
EMClusterTool::EMClusterTool(const std::string& type, const std::string& name, const IInterface* parent) :
  egammaBaseTool(type, name, parent),
  m_storeGate(0),
  m_caloCellDetPos(0)
{
  declareProperty("ClusterCorrectionToolName", m_ClusterCorrectionToolName = "egammaSwTool/egammaswtool");
  declareProperty("OutputClusterContainerName", m_outputClusterContainerName, 
    "Name of the output cluster container");
  declareProperty("ElectronContainerName", m_electronContainerName, 
    "Name of the input electron container");
  declareProperty("PhotonContainerName", m_photonContainerName, 
    "Name of the input photon container");
  declareProperty("DoPositionInCalo", m_doPositionInCalo = true, 
    "Decorate clusters with positions in calo frame");

  declareInterface<IEMClusterTool>(this);
  
  m_caloCellDetPos = new CaloCellDetPos();
}

EMClusterTool::~EMClusterTool() { delete m_caloCellDetPos; }

// const InterfaceID& ClusterTool::interfaceID() {
//   return IID_IClusterTool;
// }

// =============================================================
StatusCode EMClusterTool::initialize() {

  ATH_MSG_DEBUG("Initializing " << name() << "...");

  if (service("StoreGateSvc", m_storeGate).isFailure()) 
  {
    ATH_MSG_ERROR("Unable to retrieve pointer to StoreGateSvc");
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
  xAOD::CaloClusterContainer* outputClusterContainer = \
    CaloClusterStoreHelper::makeContainer(m_storeGate, 
                                          m_outputClusterContainerName, 
                                          msg());
  if (!outputClusterContainer){
    ATH_MSG_ERROR("Failed to record Output Cluster Container " 
		  << m_outputClusterContainerName);
    return StatusCode::FAILURE;
  }
  
  // Retrieve electron and photon containers
  xAOD::ElectronContainer *electronContainer;
  if (m_storeGate->retrieve(electronContainer, m_electronContainerName).isFailure())
  {
    ATH_MSG_ERROR("Cannot retrieve electron container " << m_electronContainerName);
    return StatusCode::FAILURE;
  }
  
  xAOD::PhotonContainer *photonContainer;
  if (m_storeGate->retrieve(photonContainer, m_photonContainerName).isFailure())
  {
    ATH_MSG_ERROR("Cannot retrieve photon container " << m_photonContainerName);
    return StatusCode::FAILURE;
  }
    
  // Loop over electrons and create new clusters
  xAOD::EgammaParameters::EgammaType egType = xAOD::EgammaParameters::electron;
  for (auto electron : *electronContainer)
    setNewCluster(electron, outputClusterContainer, egType);
  
  // Loop over photons and create new clusters
  for (auto photon : *photonContainer)
  {
     egType = (xAOD::EgammaHelpers::isConvertedPhoton(photon) ? 
               xAOD::EgammaParameters::convertedPhoton :
               xAOD::EgammaParameters::unconvertedPhoton);

    setNewCluster(photon, outputClusterContainer, egType);
  }    
  
  if (CaloClusterStoreHelper::finalizeClusters(m_storeGate, 
                                               outputClusterContainer,
					       m_outputClusterContainerName,
					       msg()).isFailure() )
  {
    ATH_MSG_ERROR("Failed to finalize Output Cluster Container " <<
      m_outputClusterContainerName);
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

// ==========================================================================
void EMClusterTool::setNewCluster(xAOD::Egamma *eg,
                                  xAOD::CaloClusterContainer *outputClusterContainer,
                                  xAOD::EgammaParameters::EgammaType egType)
{
  if (!eg) return;
  
  if (!eg->caloCluster())
  {
    ATH_MSG_DEBUG("egamma object does not have a cluster associated");
    return;
  }
  typedef ElementLink<xAOD::CaloClusterContainer> ClusterLink_t;
  
  xAOD::CaloCluster* cluster =0;
  if(eg->author(xAOD::EgammaParameters::AuthorCaloTopo35)) {
    cluster = new xAOD::CaloCluster(*(eg->caloCluster()));
  }
  else {
    cluster = makeNewCluster(*(eg->caloCluster()), egType);
  }
    outputClusterContainer->push_back(cluster);

  // Set the link to the new cluster
  ClusterLink_t clusterLink(cluster, *outputClusterContainer);
  const std::vector<ClusterLink_t> clusterLinks{clusterLink};
  eg->setCaloClusterLinks( clusterLinks );
  
}

// ==========================================================================
xAOD::CaloCluster* EMClusterTool::makeNewCluster(const xAOD::CaloCluster& cluster, xAOD::EgammaParameters::EgammaType egType)
{
  //
  // Create new cluster based on an existing one
  // const CaloCluster* cluster : input cluster
  //
  
  // protection against cluster not in barrel nor endcap
  if (!cluster.inBarrel() && !cluster.inEndcap() )
  {
    ATH_MSG_ERROR("Cluster neither in barrel nor in endcap, Skipping cluster");
	  return 0;
  }
  
  if ((int) egType < 0 || egType >= xAOD::EgammaParameters::NumberOfEgammaTypes)
  {
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
    cluSize = (isBarrel ? xAOD::CaloCluster::SW_35gam : xAOD::CaloCluster::SW_55gam);
  }

  xAOD::CaloCluster* newClus = CaloClusterStoreHelper::makeCluster(cluster.getCellLinks()->getCellContainer(),
								   cluster.eta0(),cluster.phi0(),
								   cluSize);

  //FIXME/WARNING: The CaloFillRectangularClusterTool does NOT set the overall eta/phi of the cluster
  //For regular SW clusters, this is done by CaloClusterUpdate after all corrections are applied 
  //To maintain the previous behavior, I set the eta/phi of the origial cluster
  newClus->setEta(cluster.eta());
  newClus->setPhi(cluster.phi());
  
  if (m_clusterCorrectionTool->execute(newClus).isFailure())
    ATH_MSG_ERROR("Problem executing cluster correction tool");
  
  if (newClus) fillPositionsInCalo(*newClus);
  return newClus;
}

// ==========================================================================
void EMClusterTool::fillPositionsInCalo(xAOD::CaloCluster& cluster)
{
  bool isBarrel = xAOD::EgammaHelpers::isBarrel(&cluster);
  CaloCell_ID::CaloSample sample = isBarrel ? CaloCell_ID::EMB2 : CaloCell_ID::EME2;
  
  // eta and phi of the cluster in the calorimeter frame
  double eta, phi;
  m_caloCellDetPos->getDetPosition(sample, cluster.eta(), cluster.phi0(), eta, phi);  
  
  cluster.auxdata<float>("etaCalo") = eta;
  cluster.auxdata<float>("phiCalo") = phi;
  
  //  eta in the second sampling
  m_caloCellDetPos->getDetPosition(sample, cluster.etaBE(2), cluster.phiBE(2), 
    eta, phi);
  cluster.auxdata<float>("etas2Calo") = eta;

  //  eta in the first sampling  
  sample = isBarrel ? CaloCell_ID::EMB1 : CaloCell_ID::EME1;
  m_caloCellDetPos->getDetPosition(sample, cluster.etaBE(1), cluster.phiBE(1), 
    eta, phi);
  cluster.auxdata<float>("etas1Calo") = eta;

}
