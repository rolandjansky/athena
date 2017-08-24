/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// ClusterTrueEnergyDecorator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Ioannis Nomidis

#include "DerivationFrameworkCalo/ClusterTrueEnergyDecorator.h"
#include "AthenaKernel/errorcheck.h"

#include "CaloCalibHitRec/CalibHitToCaloCellTool.h"
#include "CaloEvent/CaloCellContainer.h"


// Constructor
DerivationFramework::ClusterTrueEnergyDecorator::ClusterTrueEnergyDecorator(const std::string& t,
						  const std::string& n,
						  const IInterface* p) :
  AthAlgTool(t,n,p)
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);

  declareProperty("CalibHitToCaloCellTool",m_calibhitToCaloCellTool);
  declareProperty("egammaClustersContName",m_egammaClustersContName="egammaClusters");
}

// Destructor
DerivationFramework::ClusterTrueEnergyDecorator::~ClusterTrueEnergyDecorator() {}

// Athena initialize and finalize
StatusCode DerivationFramework::ClusterTrueEnergyDecorator::initialize()
{
  ATH_MSG_VERBOSE("initialize() ...");

  if(m_calibhitToCaloCellTool.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_calibhitToCaloCellTool);
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Retrieved tool " << m_calibhitToCaloCellTool);
   
  return StatusCode::SUCCESS;
}
//
StatusCode DerivationFramework::ClusterTrueEnergyDecorator::finalize()
{
  ATH_MSG_DEBUG( "finalize()" );
  return StatusCode::SUCCESS;
}

// The decoration itself
StatusCode DerivationFramework::ClusterTrueEnergyDecorator::addBranches() const
{
  const xAOD::CaloClusterContainer *clContainer = evtStore()->retrieve<const xAOD::CaloClusterContainer>(m_egammaClustersContName.c_str()) ;
  if (!clContainer) ATH_MSG_ERROR("Can't find egamma clusters");

  // get truth cells
  int singleElectronBarcode = 10001;
  if (!m_calibhitToCaloCellTool->processCalibHitsFromParticle(singleElectronBarcode).isSuccess()) {
    ATH_MSG_FATAL("Tool " << m_calibhitToCaloCellTool << " failed.");
    return StatusCode::FAILURE;
  }

  // m_truthCellsEtot = m_calibhitToCaloCellTool->getTruthCellsCont(CalibHitUtils::EnergyTotal);
  // m_truthCellsEvis = m_calibhitToCaloCellTool->getTruthCellsCont(CalibHitUtils::EnergyVisible);
  // m_truthCellsEem = m_calibhitToCaloCellTool->getTruthCellsCont(CalibHitUtils::EnergyEM);

  const CaloCellContainer *truthCellsEtot =0;
  const CaloCellContainer *truthCellsEvis =0;
  const CaloCellContainer *truthCellsEem =0;
  ATH_CHECK(evtStore()->retrieve(truthCellsEtot, "TruthLArCellsEtot"));
  ATH_CHECK(evtStore()->retrieve(truthCellsEvis, "TruthLArCellsEvis"));
  ATH_CHECK(evtStore()->retrieve(truthCellsEem, "TruthLArCellsEem"));
  ATH_MSG_DEBUG("Retrieved truth cell containers");
    
  for (auto cl : *clContainer) {
    if (!cl) continue;
    cl->auxdecor<float>("clusterTrueEtot") = getTrueEnergyInCluster(cl,truthCellsEtot);
    cl->auxdecor<float>("clusterTrueEvis") = getTrueEnergyInCluster(cl,truthCellsEvis);
    cl->auxdecor<float>("clusterTrueEem") = getTrueEnergyInCluster(cl,truthCellsEem);
    ATH_MSG_DEBUG("added decoration clusterTrueEtot " << cl->auxdata<float>("clusterTrueEtot"));
  }
  return StatusCode::SUCCESS;
}

float DerivationFramework::ClusterTrueEnergyDecorator::getTrueEnergyInCluster(const xAOD::CaloCluster* cluster,const CaloCellContainer* truthCells) const
{
  if (!cluster) {
    ATH_MSG_DEBUG("no valid cluster");
    return 0;
  }
  
  const CaloClusterCellLink* cellLinks = cluster->getCellLinks();
  if (!cellLinks) {
    ATH_MSG_DEBUG("no valid cell links");
    return 0;
  }

  double sumE = 0.;
  xAOD::CaloCluster::const_cell_iterator cell_itr = cluster->begin();
  xAOD::CaloCluster::const_cell_iterator cell_end = cluster->end();	
  for (; cell_itr != cell_end; ++cell_itr) {
    const CaloCell* cell = *cell_itr;

    const CaloCell* truthCell = truthCells->findCell(cell->caloDDE()->calo_hash());
    if (!truthCell) continue; //not found
    // note: supercluster may contain the cell more than once; pick up weight from link to count the energy correctly
    sumE += cell_itr.weight() * truthCell->energy();    
  }
  return sumE;
}
