/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaSuperClusterBuilder.h"
//
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloUtils/CaloCellList.h"
//
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"
#include "egammaRecEvent/egammaRecContainer.h"
#include "xAODEgamma/EgammaEnums.h"
#include "xAODEgamma/PhotonxAODHelpers.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
//
#include "FourMomUtils/P4Helpers.h"


#include <vector>

using CLHEP::GeV;

//////////////////////////////////////////////////////////////////////////////
//Athena interfaces.
//////////////////////////////////////////////////////////////////////////////

//Constructor.
egammaSuperClusterBuilder::egammaSuperClusterBuilder(const std::string& type,
						     const std::string& name,
						     const IInterface* parent) :
  AthAlgTool(type, name, parent),
  m_clusterCorrectionTool("egammaSwTool/egammaswtool")
{

  declareProperty("EtThresholdCut", m_EtThresholdCut = 1.5*GeV,
		  "The minimum Et required of SEED clusters (not applied to secondaries)");

  declareProperty("EMFracCut", m_emFracCut = 0.5,
		  "The minimum EM fraction required of both seed and secondary clusters");

  declareProperty("SearchWindowEtaCellsBarrel", m_searchWindowEtaCellsBarrel = 3, 
		  "Number of cells in eta of window in which to search for topoclusters");

  declareProperty("SearchWindowPhiCellsBarrel", m_searchWindowPhiCellsBarrel = 5,
		  "Number of cells in phi of window in which to search for topoclusters");

  declareProperty("SearchWindowEtaCellsEndcap", m_searchWindowEtaCellsEndcap = 3, 
		  "Number of cells in eta of window in which to search for topoclusters");

  declareProperty("SearchWindowPhiCellsEndcap", m_searchWindowPhiCellsEndcap = 5,
		  "Number of cells in phi of window in which to search for topoclusters");

  declareProperty("AddCellsWindowEtaCellsBarrel", m_addCellsWindowEtaCellsBarrel = 3, 
		  "Number of cells in eta of window around topocluster center to add cells");

  declareProperty("AddCellsWindowPhiCellsBarrel", m_addCellsWindowPhiCellsBarrel = 999 /*7 for SW*/,
		  "Number of cells in phi of window around topocluster center to add cells");

  declareProperty("AddCellsWindowEtaCellsEndcap", m_addCellsWindowEtaCellsEndcap = 5, 
		  "Number of cells in eta of window around topocluster center to add cells");

  declareProperty("AddCellsWindowPhiCellsEndcap", m_addCellsWindowPhiCellsEndcap = 999 /*5 for SW*/,
		  "Number of cells in phi of window around topocluster center to add cells");

  declareProperty("SumRemainingCellsInWindow", m_sumRemainingCellsInWindow = false);
  
  declareProperty("CorrectClusters", m_correctClusters = true, 
		  "Whether to run cluster corrections");
  
  declareProperty("CalibrateClusters", m_calibrateClusters = true, 
		  "Whether to run cluster calibrations");
  
  declareProperty("MVACalibTool", m_MVACalibTool);
  declareProperty("ClusterCorrectionTool", m_clusterCorrectionTool);
  
  m_searchWindowPhiBarrel = m_searchWindowPhiCellsBarrel * s_cellPhiSize * 0.5;
  m_searchWindowEtaBarrel = m_searchWindowEtaCellsBarrel * s_cellEtaSize * 0.5;
  m_searchWindowPhiEndcap = m_searchWindowPhiCellsEndcap * s_cellPhiSize * 0.5;
  m_searchWindowEtaEndcap = m_searchWindowEtaCellsEndcap * s_cellEtaSize * 0.5;

  m_addCellsWindowPhiBarrel = m_addCellsWindowPhiCellsBarrel * s_cellPhiSize * 0.5;
  m_addCellsWindowEtaBarrel = m_addCellsWindowEtaCellsBarrel * s_cellEtaSize * 0.5;
  m_addCellsWindowPhiEndcap = m_addCellsWindowPhiCellsEndcap * s_cellPhiSize * 0.5;
  m_addCellsWindowEtaEndcap = m_addCellsWindowEtaCellsEndcap * s_cellEtaSize * 0.5;
  
}

StatusCode egammaSuperClusterBuilder::initialize() {
  ATH_MSG_DEBUG(" Initializing egamma SuperClusterBuilder");
  m_searchWindowPhiBarrel = m_searchWindowPhiCellsBarrel * s_cellPhiSize * 0.5;
  m_searchWindowEtaBarrel = m_searchWindowEtaCellsBarrel * s_cellEtaSize * 0.5;
  m_searchWindowPhiEndcap = m_searchWindowPhiCellsEndcap * s_cellPhiSize * 0.5;
  m_searchWindowEtaEndcap = m_searchWindowEtaCellsEndcap * s_cellEtaSize * 0.5;

  m_addCellsWindowPhiBarrel = m_addCellsWindowPhiCellsBarrel * s_cellPhiSize * 0.5;
  m_addCellsWindowEtaBarrel = m_addCellsWindowEtaCellsBarrel * s_cellEtaSize * 0.5;
  m_addCellsWindowPhiEndcap = m_addCellsWindowPhiCellsEndcap * s_cellPhiSize * 0.5;
  m_addCellsWindowEtaEndcap = m_addCellsWindowEtaCellsEndcap * s_cellEtaSize * 0.5;
 
  if (m_correctClusters) {
    ATH_CHECK(m_clusterCorrectionTool.retrieve());
  }
  if (m_calibrateClusters) {
    ATH_CHECK(m_MVACalibTool.retrieve());
  }

  return StatusCode::SUCCESS;
}

bool egammaSuperClusterBuilder::MatchesInWindow(const xAOD::CaloCluster *ref,
						const xAOD::CaloCluster *clus) const
{
  //Get the differences in 2nd layer eta, phi of the
  //satellite and seed for convenience.
  float dEta(fabs(ref->eta()-clus->eta()));
  float dPhi(fabs(P4Helpers::deltaPhi(ref->phi(), clus->phi())));

  if (xAOD::EgammaHelpers::isBarrel(clus)) {
    return (dEta < m_searchWindowEtaBarrel && dPhi < m_searchWindowPhiBarrel);
  } else {
    return (dEta < m_searchWindowEtaEndcap && dPhi < m_searchWindowPhiEndcap);
  }
}

xAOD::CaloCluster* egammaSuperClusterBuilder::CreateNewCluster(const std::vector<const xAOD::CaloCluster*>& clusters,
							       xAOD::EgammaParameters::EgammaType egType)
{
  const auto acSize = clusters.size();
  if (acSize == 0) {
    ATH_MSG_ERROR("Missing the seed cluster! Should not happen.");
    return nullptr;
  }

  xAOD::CaloCluster* newCluster = CaloClusterStoreHelper::makeCluster(clusters.at(0)->getCellLinks()->getCellContainer());
  if (!newCluster) {
    ATH_MSG_ERROR("CaloClusterStoreHelper::makeCluster failed.");
    return nullptr;
  }

   newCluster->setClusterSize(xAOD::CaloCluster::SuperCluster);

   //A vector to keep track of cells we have added (filled by AddEMCellsToCluster)
   std::vector<const CaloCell*> cellsInWindow;

   //Need a vector of element Links to the constituent Cluster
   std::vector< ElementLink< xAOD::CaloClusterContainer > > constituentLinks;

   //Loop over accumulated Clusters, but treat 0 separately to determine 
   static const SG::AuxElement::Accessor < ElementLink < xAOD::CaloClusterContainer > > sisterCluster("SisterCluster");

   //Add the EM cells of the seed cluster
   if (AddEMCellsToCluster(newCluster,clusters[0], cellsInWindow).isFailure()) {
     ATH_MSG_WARNING("There was problem adding the cells to the the cluster");
     delete newCluster;
     return nullptr;
   }  

   //Set the element Link to the constitents
   if (sisterCluster.isAvailable(*clusters[0])) {
     constituentLinks.push_back(sisterCluster(*clusters[0]));
   } else{
     ATH_MSG_WARNING("No sister Link available");
   }

   // calculate the seed cluster kinematics.
   CaloClusterKineHelper::calculateKine(newCluster, true, true);


   // set the seed values in the cluster
   const auto seed_eta = newCluster->eta();
   const auto seed_phi = newCluster->phi();
   newCluster->setEta0(seed_eta);
   newCluster->setPhi0(seed_phi);

   //
   //
   // Now continue with the remaining clusters
   for (size_t i = 1; i < acSize; i++) {
     //Add te EM cells of the accumulated to the cluster
     if (AddEMCellsToCluster(newCluster,clusters[i], cellsInWindow).isFailure()) {
       ATH_MSG_WARNING("There was problem adding the topocluster cells to the the cluster");
       delete newCluster;
       return nullptr;
     }  
     
     //Set the element Link to the constitents
     if (sisterCluster.isAvailable(*clusters[i])) {
       constituentLinks.push_back(sisterCluster(*clusters[i]));
     }else{
       ATH_MSG_WARNING("No sister Link available");
     }
   }
   
   //Set the link from the super cluster to the constituents (accumulated) clusters used. 
   static const SG::AuxElement::Accessor < std::vector< ElementLink< xAOD::CaloClusterContainer > > > caloClusterLinks("constituentClusterLinks");
   caloClusterLinks(*newCluster) = constituentLinks;
   
   //Add all the remaining cells in a window 
   if (m_sumRemainingCellsInWindow) {
     if (AddRemainingCellsToCluster(newCluster, seed_eta, seed_phi, cellsInWindow).isFailure()) {
       ATH_MSG_WARNING("There was problem adding the cells outside of the topoclusters");
       delete newCluster;
       return nullptr;
     }        
   }
   
   ///Calculate the kinematics of the new cluster, after all cells are added
   CaloClusterKineHelper::calculateKine(newCluster, true, true);

   //If adding all EM cells I am somehow below the seed threshold then remove 
   //this one
   if(newCluster->et()<m_EtThresholdCut ){
     delete newCluster;
     return nullptr;
   }
 
   // Apply correction  calibration
   if (CalibrateCluster(newCluster, egType).isFailure()) {
     ATH_MSG_WARNING("There was problem calibrating the object");
     delete newCluster;
     return nullptr;
   }
   // return the new cluster
   return newCluster;  
}

StatusCode egammaSuperClusterBuilder::AddEMCellsToCluster(xAOD::CaloCluster       *newCluster,
							  const xAOD::CaloCluster *ref,
							  std::vector<const CaloCell*>& cellsInWindow) const
{
  if (!newCluster || !ref) {
    ATH_MSG_ERROR("Invalid input in AddEMCellsToCluster");
    return StatusCode::FAILURE;
  }

  const bool isBarrel(xAOD::EgammaHelpers::isBarrel(ref));
  
  xAOD::CaloCluster::const_cell_iterator cell_itr = ref->begin();
  xAOD::CaloCluster::const_cell_iterator cell_end = ref->end();
  //Need to check that the cell belongs to the EM calorimeter,
    //Need to check that the cell belongs to the EM calorimeter,
    for (; cell_itr != cell_end; ++cell_itr) { 
      const CaloCell* cell = *cell_itr; 
      if (!cell){
	continue;
      }    
      if (isBarrel) {
	if (fabs(ref->eta()-cell->eta()) > m_addCellsWindowEtaBarrel)
	  continue;
	if (fabs(P4Helpers::deltaPhi(ref->phi(),cell->phi())) > m_addCellsWindowPhiBarrel)
	  continue;
      } else {
	if (fabs(ref->eta()-cell->eta()) > m_addCellsWindowEtaEndcap)
	  continue;
	if (fabs(P4Helpers::deltaPhi(ref->phi(),cell->phi())) > m_addCellsWindowPhiEndcap)
	  continue;
      }
      
      //Add all LAR EM
      if (cell->caloDDE()->getSubCalo() == CaloCell_ID::LAREM) {
	newCluster->addCell(cell_itr.index(), cell_itr.weight());
	cellsInWindow.push_back(cell);
      }
      //Add TileGap3. Consider only E4 cell
      if (CaloCell_ID::TileGap3 == cell->caloDDE()->getSampling()) {
	if( fabs(cell->eta()) >1.4 && fabs(cell->eta()) < 1.6 ){
	  newCluster->addCell(cell_itr.index(), cell_itr.weight());
	  cellsInWindow.push_back(cell);
	}
      }//TileGap
    }//Loop over cells
    
    if (newCluster->size()==0){
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

StatusCode egammaSuperClusterBuilder::AddRemainingCellsToCluster(xAOD::CaloCluster *myCluster,
								 float seed_eta, float seed_phi,
								 std::vector<const CaloCell*>& cellsInWindow) const
{
  ATH_MSG_DEBUG("Add Remaining cells in window");
  if (!myCluster) {
    ATH_MSG_ERROR("Invalid input in AddRemainingCellsToCluster");
    return StatusCode::FAILURE;
  }

  // determine window size, barrel or EC
  auto searchWindowEta = m_searchWindowEtaEndcap;
  auto searchWindowPhi = m_searchWindowPhiEndcap;
  if (xAOD::EgammaHelpers::isBarrel(myCluster)) {
    searchWindowEta = m_searchWindowEtaBarrel;
    searchWindowPhi = m_searchWindowPhiBarrel;
  }

  std::vector<const CaloCell*> cells;
  cells.reserve(100);
  const CaloCellContainer* inputcells=myCluster->getCellLinks()->getCellContainer();
  CaloCellList myList(inputcells);
  const std::vector<CaloSampling::CaloSample> samples = {CaloSampling::PreSamplerB,
							 CaloSampling::PreSamplerE,
							 CaloSampling::EMB1,
							 CaloSampling::EME1,
							 CaloSampling::EMB2,
							 CaloSampling::EME2,
							 CaloSampling::EMB3,
							 CaloSampling::EME3,
							 CaloSampling::TileGap3};
  
  for ( auto samp : samples ) {
    myList.select(seed_eta, seed_phi, searchWindowEta, searchWindowPhi,samp);
    cells.insert(cells.end(), myList.begin(), myList.end());
  }
  for ( auto cell : cells ) {
    if( !cell || !cell->caloDDE() ) {continue;}
    int index = inputcells->findIndex(cell->caloDDE()->calo_hash());
    if( index == -1 ) {continue;}
    //Check if it's already used.
    if (CaloCell_ID::TileGap3 == cell->caloDDE()->getSampling() && 
	(std::abs(cell->eta()) <= 1.4 || std::abs(cell->eta()) >= 1.6)) {
      continue; // only take them in the gap
    }
    if (std::find(cellsInWindow.begin(),cellsInWindow.end(),cell) != cellsInWindow.end()) {
      continue;
    }
    //Adding with weight '1' now -- don't a priori have weight of cell,
    myCluster->addCell(index, 1.);
  }
  return StatusCode::SUCCESS;
}

StatusCode egammaSuperClusterBuilder::CalibrateCluster(xAOD::CaloCluster* newCluster,
						       const xAOD::EgammaParameters::EgammaType egType) 
{
  //Save the state before the corrections
  newCluster->setAltE(newCluster->e());
  newCluster->setAltEta(newCluster->eta());
  newCluster->setAltPhi(newCluster->phi());
  ATH_MSG_DEBUG("==========no correction/calibration ==== ");
  ATH_MSG_DEBUG("Cluster Energy no correction/calibration: "<<newCluster->e());
  ATH_MSG_DEBUG("Cluster eta  no correction/calibration: "<<newCluster->eta());
  ATH_MSG_DEBUG("Cluster phi no correction/calibration: "<<newCluster->phi());
  ATH_MSG_DEBUG("Cluster etaBE(2) no correction/calibration: "<<newCluster->etaBE(2));
  ATH_MSG_DEBUG("Cluster phiBE(2) no correction/calibration: "<<newCluster->phiBE(2));

  // first do the corrections
  if (m_correctClusters) {
    ATH_CHECK(m_clusterCorrectionTool->execute(newCluster,egType,xAOD::EgammaHelpers::isBarrel(newCluster)));
  }
  newCluster->setRawE(newCluster->e());
  newCluster->setRawEta(newCluster->eta());
  newCluster->setRawPhi(newCluster->phi());
  ATH_MSG_DEBUG("========== after position correction ==== ");
  ATH_MSG_DEBUG("Cluster Energy after position corrections: "<<newCluster->e());
  ATH_MSG_DEBUG("Cluster eta  after position corrections: "<<newCluster->eta());
  ATH_MSG_DEBUG("Cluster phi after position corrections: "<<newCluster->phi());
  ATH_MSG_DEBUG("Cluster etaBE(2) after position corrections: "<<newCluster->etaBE(2));
  ATH_MSG_DEBUG("Cluster phiBE(2) after position corrections: "<<newCluster->phiBE(2));

  //
  ATH_CHECK(fillPositionsInCalo(newCluster));
  //At this point we do not have the final tracks vertices attached on the cluster/ new egamma Rec Object.
  //So We will need at the end to do the final update in the EMClusterTool
  //For now apply just cluster info only calibration.
  if (m_calibrateClusters) {
    ATH_CHECK(m_MVACalibTool->execute(newCluster,egType));
  }
  ATH_MSG_DEBUG("========== cluster only calibration ==== ");
  ATH_MSG_DEBUG("Cluster Energy after cluster only calibration: "<<newCluster->e());

  return StatusCode::SUCCESS;
}
// ==========================================================================
StatusCode egammaSuperClusterBuilder::fillPositionsInCalo(xAOD::CaloCluster* cluster)
{
  const bool isBarrel = xAOD::EgammaHelpers::isBarrel(cluster);
  CaloCell_ID::CaloSample sample = isBarrel ? CaloCell_ID::EMB2 : CaloCell_ID::EME2;
  // eta and phi of the cluster in the calorimeter frame
  double eta, phi;
  m_caloCellDetPos.getDetPosition(sample, cluster->eta(), cluster->phi(), eta, phi); 

  cluster->insertMoment(xAOD::CaloCluster::ETACALOFRAME,eta);
  cluster->insertMoment(xAOD::CaloCluster::PHICALOFRAME,phi);

  //  eta in the second sampling
  m_caloCellDetPos.getDetPosition(sample, cluster->etaBE(2), cluster->phiBE(2), eta, phi);
  cluster->insertMoment(xAOD::CaloCluster::ETA2CALOFRAME,eta);
  cluster->insertMoment(xAOD::CaloCluster::PHI2CALOFRAME,phi);
  //  eta in the first sampling 
  sample = isBarrel ? CaloCell_ID::EMB1 : CaloCell_ID::EME1;
  m_caloCellDetPos.getDetPosition(sample, cluster->etaBE(1), cluster->phiBE(1),eta, phi);
  cluster->insertMoment(xAOD::CaloCluster::ETA1CALOFRAME,eta);
  cluster->insertMoment(xAOD::CaloCluster::PHI1CALOFRAME,phi);

  return StatusCode::SUCCESS;
}
