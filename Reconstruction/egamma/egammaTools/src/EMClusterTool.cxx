/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "EMClusterTool.h"
#include "CaloEvent/CaloClusterCellLink.h"
#include "GaudiKernel/EventContext.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "StoreGate/WriteHandle.h"

EMClusterTool::EMClusterTool(const std::string& type,
                             const std::string& name,
                             const IInterface* parent)
  : AthAlgTool(type, name, parent)
{
  declareInterface<IEMClusterTool>(this);
}

StatusCode
EMClusterTool::initialize()
{

  ATH_MSG_DEBUG("Initializing " << name() << "...");

  ATH_CHECK(m_outputClusterContainerKey.initialize());

  m_outputClusterContainerCellLinkKey =
    m_outputClusterContainerKey.key() + "_links";
  ATH_CHECK(m_outputClusterContainerCellLinkKey.initialize());

  // Get the cluster correction tool
  ATH_CHECK(m_MVACalibSvc.retrieve());

  ATH_MSG_DEBUG("Initialization successful");

  return StatusCode::SUCCESS;
}

StatusCode
EMClusterTool::contExecute(const EventContext& ctx,
                           xAOD::ElectronContainer* electronContainer,
                           xAOD::PhotonContainer* photonContainer) const
{

  // Create output cluster container and register in StoreGate
  SG::WriteHandle<xAOD::CaloClusterContainer> outputClusterContainer(
    m_outputClusterContainerKey, ctx);

  ATH_CHECK(outputClusterContainer.record(
    std::make_unique<xAOD::CaloClusterContainer>(),
    std::make_unique<xAOD::CaloClusterAuxContainer>()));

  SG::WriteHandle<CaloClusterCellLinkContainer> outputClusterContainerCellLink(
    m_outputClusterContainerCellLinkKey, ctx);

  ATH_CHECK(outputClusterContainerCellLink.record(
    std::make_unique<CaloClusterCellLinkContainer>()));

  // Loop over electrons and create new clusters
  if (electronContainer) {
    for (auto electron : *electronContainer) {
      setNewCluster(ctx, electron, outputClusterContainer.ptr());
    }
  }

  if (photonContainer) {
    // Loop over photons and create new clusters
    for (auto photon : *photonContainer) {
      setNewCluster(ctx, photon, outputClusterContainer.ptr());
    }
  }
  // Now finalize the cluster: based on code in
  // CaloClusterStoreHelper::finalizeClusters
  for (xAOD::CaloCluster* cl : *outputClusterContainer) {
    cl->setLink(outputClusterContainerCellLink.ptr(), ctx);
  }

  return StatusCode::SUCCESS;
}

void
EMClusterTool::setNewCluster(
  const EventContext& ctx,
  xAOD::Egamma* eg,
  xAOD::CaloClusterContainer* outputClusterContainer) const
{
  if (!eg) {
    return;
  }
  if (!eg->caloCluster()) {
    ATH_MSG_DEBUG("egamma object does not have a cluster associated");
    return;
  }
  using ClusterLink_t = ElementLink<xAOD::CaloClusterContainer>;
  //create new cluster
  xAOD::CaloCluster* cluster = new xAOD::CaloCluster();
  //store owns it
  outputClusterContainer->push_back(cluster);
  //copy over 
  (*cluster)=*(eg->caloCluster());
  //and do final calibration
  if (m_MVACalibSvc->execute(*cluster, *eg).isFailure()) {
    ATH_MSG_ERROR("Problem executing MVA cluster tool");
  }
  // Set the link to the new cluster
  ClusterLink_t clusterLink(cluster, *outputClusterContainer, ctx);
  const std::vector<ClusterLink_t> clusterLinks{ clusterLink };
  eg->setCaloClusterLinks(clusterLinks);
}

