/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "EMClusterTool.h"
#include "CaloEvent/CaloClusterCellLink.h"
#include "GaudiKernel/EventContext.h"
#include "StoreGate/StoreGateSvc.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloUtils/CaloCellDetPos.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "StoreGate/WriteHandle.h"

// =============================================================
EMClusterTool::EMClusterTool(const std::string& type,
                             const std::string& name,
                             const IInterface* parent)
  : AthAlgTool(type, name, parent)
  , m_caloCellDetPos{}
  , m_doTopoSeededContainer(false)
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

  m_doTopoSeededContainer = (m_outputTopoSeededClusterContainerKey.key() !=
                               m_outputClusterContainerKey.key() &&
                             !m_doSuperClusters);
  ATH_CHECK(
    m_outputTopoSeededClusterContainerKey.initialize(m_doTopoSeededContainer));
  m_outputTopoSeededClusterContainerCellLinkKey =
    m_outputTopoSeededClusterContainerKey.key() + "_links";

  ATH_CHECK(m_outputTopoSeededClusterContainerCellLinkKey.initialize(
    m_doTopoSeededContainer));

  // Get the cluster correction tool
  if (m_clusterCorrectionTool.retrieve().isFailure()) {
    ATH_MSG_ERROR("Failed to retrieve " << m_clusterCorrectionTool);
    return StatusCode::SUCCESS;
  }

  ATH_MSG_DEBUG("Retrieved tool " << m_clusterCorrectionTool);

  // Get the cluster correction tool
  if (m_MVACalibSvc.retrieve().isFailure()) {
    ATH_MSG_ERROR("Failed to retrieve " << m_MVACalibSvc);
    return StatusCode::SUCCESS;
  }

  ATH_MSG_DEBUG("Retrieved tool " << m_MVACalibSvc);

  ATH_MSG_DEBUG("Initialization successful");

  return StatusCode::SUCCESS;
}

StatusCode
EMClusterTool::contExecute(const EventContext& ctx,
                           const CaloDetDescrManager& mgr,
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

  // Create output cluster container for topo-seeded clusters and register in
  // StoreGate Only if they differ from the main output cluster container and if
  // we do not do supercluster
  SG::WriteHandle<xAOD::CaloClusterContainer> outputTopoSeededClusterContainer;
  SG::WriteHandle<CaloClusterCellLinkContainer>
    outputTopoSeededClusterContainerCellLink;

  if (m_doTopoSeededContainer) {
    outputTopoSeededClusterContainer =
      SG::WriteHandle<xAOD::CaloClusterContainer>(
        m_outputTopoSeededClusterContainerKey, ctx);

    ATH_CHECK(outputTopoSeededClusterContainer.record(
      std::make_unique<xAOD::CaloClusterContainer>(),
      std::make_unique<xAOD::CaloClusterAuxContainer>()));

    outputTopoSeededClusterContainerCellLink =
      SG::WriteHandle<CaloClusterCellLinkContainer>(
        m_outputTopoSeededClusterContainerCellLinkKey, ctx);

    ATH_CHECK(outputTopoSeededClusterContainerCellLink.record(
      std::make_unique<CaloClusterCellLinkContainer>()));
  }

  // Loop over electrons and create new clusters
  xAOD::EgammaParameters::EgammaType egType = xAOD::EgammaParameters::electron;
  if (electronContainer) {
    for (auto electron : *electronContainer) {
      setNewCluster(ctx, mgr, electron, outputClusterContainer.ptr(), egType);
    }
  }

  if (photonContainer) {
    // Loop over photons and create new clusters
    for (auto photon : *photonContainer) {
      egType = (xAOD::EgammaHelpers::isConvertedPhoton(photon)
                  ? xAOD::EgammaParameters::convertedPhoton
                  : xAOD::EgammaParameters::unconvertedPhoton);

      if (!m_doTopoSeededContainer ||
          !photon->author(xAOD::EgammaParameters::AuthorCaloTopo35)) {
        setNewCluster(ctx, mgr, photon, outputClusterContainer.ptr(), egType);
      } else {
        setNewCluster(
          ctx, mgr, photon, outputTopoSeededClusterContainer.ptr(), egType);
      }
    }
  }
  // Now finalize the cluster: based on code in
  // CaloClusterStoreHelper::finalizeClusters
  auto sg = outputClusterContainer.storeHandle().get();
  for (xAOD::CaloCluster* cl : *outputClusterContainer) {
    cl->setLink(outputClusterContainerCellLink.ptr(), sg);
  }

  if (m_doTopoSeededContainer) {
    auto tssg = outputTopoSeededClusterContainer.storeHandle().get();
    for (xAOD::CaloCluster* cl : *outputTopoSeededClusterContainer) {
      cl->setLink(outputTopoSeededClusterContainerCellLink.ptr(), tssg);
    }
  }

  return StatusCode::SUCCESS;
}

void
EMClusterTool::setNewCluster(const EventContext& ctx,
                             const CaloDetDescrManager& mgr,
                             xAOD::Egamma* eg,
                             xAOD::CaloClusterContainer* outputClusterContainer,
                             xAOD::EgammaParameters::EgammaType egType) const
{
  if (!eg) {
    return;
  }

  if (!eg->caloCluster()) {
    ATH_MSG_DEBUG("egamma object does not have a cluster associated");
    return;
  }
  typedef ElementLink<xAOD::CaloClusterContainer> ClusterLink_t;
  xAOD::CaloCluster* cluster = nullptr;

  // Special Case for topo seeded photons.
  if (eg->author(xAOD::EgammaParameters::AuthorCaloTopo35)) {
    cluster = new xAOD::CaloCluster(*(eg->caloCluster()));
    fillPositionsInCalo(cluster, mgr);
  } // Doing superClusters
  else if (m_doSuperClusters) {
    // copy over for super clusters
    cluster = makeNewSuperCluster(*(eg->caloCluster()), eg);
  } else {
    cluster = makeNewCluster(ctx, *(eg->caloCluster()), mgr, eg, egType);
  }

  outputClusterContainer->push_back(cluster);

  // Set the link to the new cluster
  ClusterLink_t clusterLink(cluster, *outputClusterContainer);
  const std::vector<ClusterLink_t> clusterLinks{ clusterLink };
  eg->setCaloClusterLinks(clusterLinks);
}

xAOD::CaloCluster*
EMClusterTool::makeNewCluster(const EventContext& ctx,
                              const xAOD::CaloCluster& cluster,
                              const CaloDetDescrManager& mgr,
                              xAOD::Egamma* eg,
                              xAOD::EgammaParameters::EgammaType egType) const
{
  // Create new cluster based on an existing one
  // const CaloCluster* cluster : input cluster

  // protection against cluster not in barrel nor endcap
  if (!cluster.inBarrel() && !cluster.inEndcap()) {
    ATH_MSG_ERROR("Cluster neither in barrel nor in endcap, Skipping cluster");
    return nullptr;
  }

  if (static_cast<int>(egType) < 0 ||
      egType >= xAOD::EgammaParameters::NumberOfEgammaTypes) {
    ATH_MSG_WARNING("Invalid egamma type");
    return nullptr;
  }

  bool isBarrel = xAOD::EgammaHelpers::isBarrel(&cluster);
  xAOD::CaloCluster::ClusterSize cluSize = xAOD::CaloCluster::CSize_Unknown;
  switch (egType) {
    case xAOD::EgammaParameters::electron: {
      cluSize =
        isBarrel ? xAOD::CaloCluster::SW_37ele : xAOD::CaloCluster::SW_55ele;
      break;
    }
    case xAOD::EgammaParameters::convertedPhoton: {
      cluSize = isBarrel ? xAOD::CaloCluster::SW_37Econv
                         : xAOD::CaloCluster::SW_55Econv;
      break;
    }
    default: {
      cluSize =
        (isBarrel ? xAOD::CaloCluster::SW_37gam : xAOD::CaloCluster::SW_55gam);
    }
  }
  xAOD::CaloCluster* newCluster = makeNewCluster(ctx, cluster, mgr, cluSize);

  if (newCluster && m_MVACalibSvc->execute(*newCluster, *eg).isFailure()) {
    ATH_MSG_ERROR("Problem executing MVA cluster tool");
  }
  return newCluster;
}

xAOD::CaloCluster*
EMClusterTool::makeNewCluster(
  const EventContext& ctx,
  const xAOD::CaloCluster& cluster,
  const CaloDetDescrManager& mgr,
  const xAOD::CaloCluster::ClusterSize& cluSize) const
{

  xAOD::CaloCluster* newClus = CaloClusterStoreHelper::makeCluster(
    cluster.getCellLinks()->getCellContainer(),
    cluster.eta0(),
    cluster.phi0(),
    cluSize);
  if (newClus) {
    if (m_clusterCorrectionTool->execute(ctx, newClus).isFailure()) {
      ATH_MSG_ERROR("Problem executing cluster correction tool");
    }
    fillPositionsInCalo(newClus, mgr);
    // Fill the raw state using the cluster with correct size  but not MVA
    newClus->setRawE(newClus->calE());
    newClus->setRawEta(newClus->calEta());
    newClus->setRawPhi(newClus->calPhi());
    // Fill the Alt state using the 3x5 cluster
    newClus->setAltE(cluster.calE());
    newClus->setAltEta(cluster.calEta());
    newClus->setAltPhi(cluster.calPhi());
  } else {
    ATH_MSG_ERROR("Null newClus");
  }

  return newClus;
}

xAOD::CaloCluster*
EMClusterTool::makeNewSuperCluster(const xAOD::CaloCluster& cluster,
                                   xAOD::Egamma* eg) const
{
  //
  xAOD::CaloCluster* newClus = new xAOD::CaloCluster(cluster);
  if (m_applySuperClusters) {
    if (newClus && m_MVACalibSvc->execute(*newClus, *eg).isFailure()) {
      ATH_MSG_ERROR("Problem executing MVA cluster tool");
    }
  }

  return newClus;
}
void
EMClusterTool::fillPositionsInCalo(xAOD::CaloCluster* cluster,
                                   const CaloDetDescrManager& mgr) const
{

  bool isBarrel = xAOD::EgammaHelpers::isBarrel(cluster);
  CaloCell_ID::CaloSample sample =
    isBarrel ? CaloCell_ID::EMB2 : CaloCell_ID::EME2;

  // eta and phi of the cluster in the calorimeter frame
  double eta;
  double phi;
  m_caloCellDetPos.getDetPosition(
    mgr, sample, cluster->eta(), cluster->phi(), eta, phi);

  cluster->insertMoment(xAOD::CaloCluster::ETACALOFRAME, eta);
  cluster->insertMoment(xAOD::CaloCluster::PHICALOFRAME, phi);

  //  eta in the second sampling
  m_caloCellDetPos.getDetPosition(
    mgr, sample, cluster->etaBE(2), cluster->phiBE(2), eta, phi);
  cluster->insertMoment(xAOD::CaloCluster::ETA2CALOFRAME, eta);
  cluster->insertMoment(xAOD::CaloCluster::PHI2CALOFRAME, phi);

  //  eta in the first sampling
  sample = isBarrel ? CaloCell_ID::EMB1 : CaloCell_ID::EME1;
  m_caloCellDetPos.getDetPosition(
    mgr, sample, cluster->etaBE(1), cluster->phiBE(1), eta, phi);
  cluster->insertMoment(xAOD::CaloCluster::ETA1CALOFRAME, eta);
  cluster->insertMoment(xAOD::CaloCluster::PHI1CALOFRAME, phi);
}
