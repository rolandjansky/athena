/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaSuperClusterBuilder.h"
//
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloUtils/CaloCellList.h"
#include "CaloUtils/CaloLayerCalculator.h"
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
#include "CaloGeoHelpers/proxim.h"

#include <vector>
#include <cmath>

namespace {
  /**
   * Duplicate code 
   * @brief Return eta/phi ranges encompassing +- 1 cell.
   * @param eta Central eta value.
   * @param phi Central phi value.
   * @param sampling The sampling to use.
   * @param[out] deta Range in eta.
   * @param[out] dphi Range in phi.
   *
   * This can be a little tricky due to misalignments and the fact
   * that cells have different sizes in different regions.  Also,
   * CaloLayerCalculator takes only a symmetric eta range.
   * We try to find the neighboring cells by starting from the center
   * cell and looking a little bit more than half its width in either
   * direction, and finding the centers of those cells.  Then we use
   * the larger of these for the symmetric range.
   */
  void etaphi_range (double eta,
		     double phi,
		     CaloCell_ID::CaloSample sampling,
		     double& deta,
		     double& dphi)
  {
    deta = 0;
    dphi = 0;
    // Get the DD element for the central cell.
    const CaloDetDescrManager* dd_man = CaloDetDescrManager::instance();
    const CaloDetDescrElement* elt = dd_man->get_element_raw(sampling, eta, phi);
    if (!elt) return;

    // Should be smaller than the eta half-width of any cell.
    constexpr double eps = 0.001;

    // Now look in the negative eta direction.
    const CaloDetDescrElement* elt_l = dd_man->get_element_raw(sampling,
							       eta - elt->deta() - eps,
							       phi);
    double deta_l = 0; // Eta difference on the low (left) side.
    if (elt_l){
      deta_l = std::abs (eta - elt_l->eta_raw()) + eps;
    }
    // Now look in the positive eta direction.
    const CaloDetDescrElement* elt_r = dd_man->get_element_raw(sampling,
							       eta + elt->deta() + eps,
							       phi);
    double deta_r = 0; // Eta difference on the high (right) side.
    if (elt_r){
      deta_r = std::abs (eta - elt_r->eta_raw()) + eps;
    }
  
    // Total deta is twice the maximum.
    deta = 2 * std::max (deta_r, deta_l);
    // Now for the phi variation.
    // The phi size can change as a function of eta, but not of phi.
    // Thus we have to look again at the adjacent eta cells, and
    // take the largest variation.
    // Now look in the negative eta direction.
    elt_l = dd_man->get_element_raw(sampling,
				    eta - elt->deta() - eps,
				    CaloPhiRange::fix (phi - elt->dphi() - eps));
    
    double dphi_l = 0; // Phi difference on the low-eta () side.
    if (elt_l){
      dphi_l = std::abs (CaloPhiRange::fix (phi - elt_l->phi_raw())) + eps;
    }
    // Now look in the positive eta direction.
    elt_r = dd_man->get_element_raw(sampling,
				    eta + elt->deta() + eps,
				    CaloPhiRange::fix (phi - elt->dphi() - eps));
    double dphi_r = 0; // Phi difference on the positive (down) side.
    if (elt_r){
      dphi_r = std::abs (CaloPhiRange::fix (phi - elt_r->phi_raw())) + eps;
    }
    // Total dphi is twice the maximum.
    dphi = 2 * std::max (dphi_l, dphi_r);
  }
}


//////////////////////////////////////////////////////////////////////////////
//Athena interfaces.
//////////////////////////////////////////////////////////////////////////////

//Constructor.
egammaSuperClusterBuilder::egammaSuperClusterBuilder(const std::string& name, 
						     ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator)
{
  
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
  } else {
    m_clusterCorrectionTool.disable();
  }
  if (m_calibrateClusters) {
    ATH_CHECK(m_MVACalibSvc.retrieve());
  } 
  // else {
  //   m_MVACalibSvc.disable();
  // }

  return StatusCode::SUCCESS;
}

bool egammaSuperClusterBuilder::matchesInWindow(const xAOD::CaloCluster *ref,
						const xAOD::CaloCluster *clus) const
{
  //First the case where we are both endcap and barrel, i.e in the crack
  //Check around both measurements of the seed
  if (ref->hasSampling(CaloSampling::EMB2) && ref->hasSampling(CaloSampling::EME2)) {
    float dEta(fabs(ref->eta()-clus->eta()));
    float dPhi(fabs(P4Helpers::deltaPhi(ref->phi(), clus->phi())));
    //
    float dEtaBarrel (fabs(ref->etaSample(CaloSampling::EMB2)-clus->eta())); 
    float dPhiBarrel (fabs(P4Helpers::deltaPhi(ref->phiSample(CaloSampling::EMB2),clus->phi())));
    //
    float dEtaEndcap (fabs(ref->etaSample(CaloSampling::EME2)-clus->eta()));
    float dPhiEndcap (fabs(P4Helpers::deltaPhi(ref->phiSample(CaloSampling::EME2),clus->phi())));
    //Matches any in case of split
    return ( (dEta < m_searchWindowEtaBarrel && dPhi < m_searchWindowPhiBarrel) ||
	     (dEta < m_searchWindowEtaEndcap && dPhi < m_searchWindowPhiEndcap) ||
	     (dEtaBarrel < m_searchWindowEtaBarrel && dPhiBarrel < m_searchWindowPhiBarrel) ||
	     (dEtaEndcap < m_searchWindowEtaEndcap && dPhiEndcap < m_searchWindowPhiEndcap));
  }
  else if (xAOD::EgammaHelpers::isBarrel(clus)) {
    float dEta(fabs(ref->eta()-clus->eta()));
    float dPhi(fabs(P4Helpers::deltaPhi(ref->phi(), clus->phi())));
    return (dEta < m_searchWindowEtaBarrel && dPhi < m_searchWindowPhiBarrel);
  } else {
    float dEta(fabs(ref->eta()-clus->eta()));
    float dPhi(fabs(P4Helpers::deltaPhi(ref->phi(), clus->phi())));
    return (dEta < m_searchWindowEtaEndcap && dPhi < m_searchWindowPhiEndcap);
  }
}

std::unique_ptr<xAOD::CaloCluster>
egammaSuperClusterBuilder::createNewCluster(const std::vector<const xAOD::CaloCluster*>& clusters,
					    xAOD::EgammaParameters::EgammaType egType)
{
  const auto acSize = clusters.size();
  if (acSize == 0) {
    ATH_MSG_ERROR("Missing the seed cluster! Should not happen.");
    return nullptr;
  }

  std::unique_ptr<xAOD::CaloCluster> newCluster(CaloClusterStoreHelper::makeCluster(clusters.at(0)->getCellLinks()->getCellContainer()));
  if (!newCluster) {
    ATH_MSG_ERROR("CaloClusterStoreHelper::makeCluster failed.");
    return nullptr;
  }
  newCluster->setClusterSize(xAOD::CaloCluster::SuperCluster);


  // Let's try to find the eta and phi of the hottest cell in L2.
  // This will be used as the center for restricting the cluster size.
  // In the future can refine (or add sanity checks) to the selection

  float etaRef = clusters[0]->eta(); // these are just default values, overwritten by findCentralPosition
  float phiRef = clusters[0]->phi();
  bool isBarrel = xAOD::EgammaHelpers::isBarrel(clusters[0]);
  
  bool foundHotCell = findCentralPosition(etaRef, phiRef, isBarrel, clusters);

  float eta0 = etaRef;
  float phi0 = phiRef;

  if (foundHotCell) {
    //Get the hotest in raw co-ordinates
    const CaloDetDescrManager* mgr = CaloDetDescrManager::instance();
    if (!mgr) {
      ATH_MSG_ERROR("Couldn't create instance of CaloDetDescrManager this should never happen");
      return nullptr;
    }
    CaloCell_ID::CaloSample xsample = (isBarrel) ? CaloCell_ID::EMB2 :CaloCell_ID::EME2;
    const CaloDetDescrElement* dde = mgr->get_element(xsample, etaRef, phiRef);
    if (dde) {
      eta0 = dde->eta_raw();
      phi0 = dde->phi_raw();
    } else {
      ATH_MSG_WARNING("Couldn't get CaloDetDescrElement from mgr for eta = " << etaRef << ", phi = " << phiRef);
    }
  } else {
    ATH_MSG_DEBUG("Did not find the hot cell; probably a cluster that will be filtered out later");
  }
  //Set the eta0/phi0 based on the references, but in raw coordinates
  newCluster->setEta0(eta0);
  newCluster->setPhi0(phi0);
  ATH_MSG_DEBUG("========== Reference Position  ==== ");
  ATH_MSG_DEBUG("Reference eta : "<<newCluster->eta0());
  ATH_MSG_DEBUG("Reference phi : "<<newCluster->phi0());

  //Need a vector of element Links to the constituent Cluster
  std::vector< ElementLink< xAOD::CaloClusterContainer > > constituentLinks;
  static const SG::AuxElement::Accessor < ElementLink < xAOD::CaloClusterContainer > > sisterCluster("SisterCluster");
  //

  // Now add the clusters (other than L1 and PS)
  for (size_t i = 0; i < acSize; i++) {
    //Add te EM cells of the accumulated to the cluster
    if (addEMCellsToCluster(newCluster.get(),clusters[i], etaRef, phiRef, isBarrel).isFailure()) {
      ATH_MSG_DEBUG("There was problem adding the topocluster cells to the the cluster; potentially no L2 or L3 cells in cluster");
      return nullptr;
    }
    //
    //Set the element Link to the constitents
    if (sisterCluster.isAvailable(*clusters[i])) {
      constituentLinks.push_back(sisterCluster(*clusters[i]));
    }else{
      ATH_MSG_WARNING("No sister Link available");
    }
  }

  // Now calculate the cluster size; use that for restricting the L1 cells
  double phiSizePlus = 0;
  double phiSizeMinus = 0;

  findPhiSize(phiSizePlus, phiSizeMinus, phiRef, newCluster.get());

  // now add L1 cells
  for (size_t i = 0; i < acSize; i++) {
    //Add te EM cells of the accumulated to the cluster
    if (addL0L1EMCellsToCluster(newCluster.get(),clusters[i], etaRef, phiRef, isBarrel, phiSizePlus, phiSizeMinus).isFailure()) {
      ATH_MSG_WARNING("There was problem adding the topocluster PS and L1 cells to the the cluster");
      // Could potentially downgrade this to a debug. Getting rid of malformed clusters is the right thing to do.
      return nullptr;
    }
  }

  //
  //Set the link from the super cluster to the constituents (accumulated) clusters used. 
  static const SG::AuxElement::Accessor < std::vector< ElementLink< xAOD::CaloClusterContainer > > > caloClusterLinks("constituentClusterLinks");
  caloClusterLinks(*newCluster.get()) = constituentLinks;
  //
  ///Calculate the kinematics of the new cluster, after all cells are added
  CaloClusterKineHelper::calculateKine(newCluster.get(), true, true);
  //
  //Check to see if cluster doesn't have EMB2 OR EME2. If not, kill it.
  if (!newCluster->hasSampling(CaloSampling::EMB2) && !newCluster->hasSampling(CaloSampling::EME2)) {
    ATH_MSG_WARNING("Supercluster doesn't have energy in layer 2. Skipping...");
    return nullptr;     
  }
  //
  //If adding all EM cells I am somehow below the seed threshold then remove 
  //this one
  if(newCluster->et()<m_EtThresholdCut ){
    return nullptr;
  }

  // Apply SW-style summation of TileGap3 cells (if necessary).
  if (addTileGap3CellsinWindow(newCluster.get()).isFailure()) {
    ATH_MSG_ERROR("Problem with the input cluster when running AddTileGap3CellsinWindow?");
    return nullptr;
  }
  CaloClusterKineHelper::calculateKine(newCluster.get(), true, true);

  // Apply correction  calibration
  if (calibrateCluster(newCluster.get(), egType).isFailure()) {
    ATH_MSG_WARNING("There was problem calibrating the object");
    return nullptr;
  }
  // return the new cluster
  return newCluster;  
}

StatusCode egammaSuperClusterBuilder::addEMCellsToCluster(xAOD::CaloCluster *newCluster,
							  const xAOD::CaloCluster *ref,
							  double etaRef, double phiRef, bool isBarrel) const
{
  if (!newCluster || !ref) {
    ATH_MSG_ERROR("Invalid input in AddEMCellsToCluster");
    return StatusCode::FAILURE;
  }
  //
  xAOD::CaloCluster::const_cell_iterator cell_itr = ref->begin();
  xAOD::CaloCluster::const_cell_iterator cell_end = ref->end();
  //Loop over cells
  for (; cell_itr != cell_end; ++cell_itr) { 
 
    //sanity check on the cell
    const CaloCell* cell = *cell_itr; 
    if (!cell){
      continue;
    }

    // lets remove from consideration if outside the range
    if (isBarrel) { // if the center is in the barrel
      if (fabs(etaRef - cell->eta()) > m_addCellsWindowEtaBarrel){
	continue;
      }
      if (fabs(P4Helpers::deltaPhi(phiRef, cell->phi())) > m_addCellsWindowPhiBarrel){
	continue;
      }
    } else { // if the center is in the endcap
      if (fabs(etaRef - cell->eta()) > m_addCellsWindowEtaEndcap){
	continue;
      }
      if (fabs(P4Helpers::deltaPhi(phiRef, cell->phi())) > m_addCellsWindowPhiEndcap){
	continue;
      }
    }
    
    const CaloDetDescrElement *dde = cell->caloDDE();
    if(!dde){
      continue;
    }

    auto sampling = dde->getSampling();

    // I think we will leave this for AddTileGap3CellsinWindow
    // //First add all TileGap3 (consider only E4 cell).
    // if (CaloCell_ID::TileGap3 == sampling) {
    //   if( fabs(cell->eta()) >1.4 && fabs(cell->eta()) < 1.6 ) {
    // 	newCluster->addCell(cell_itr.index(), cell_itr.weight());
    //   }
    //   continue;  
    //   //If it is TileGap we are done here
    //   //either added or not , next cell please
    // }

    // skip if sampling is L1 or presample, since that is a separate step
    if (CaloCell_ID::EMB1 == sampling || CaloCell_ID::EME1 == sampling ||
	CaloCell_ID::PreSamplerB == sampling || CaloCell_ID::PreSamplerE == sampling) {
      continue;
    }

    
    //For the cells we have not skipped either because TileGap, bounds, or L1
    if (dde->getSubCalo() == CaloCell_ID::LAREM) {
      //Avoid summing inner wheel Endcap cells 
      if(! (dde->is_lar_em_endcap_inner()) ){
	newCluster->addCell(cell_itr.index(), cell_itr.weight());
      }
    }
  }//Loop over cells
  if (newCluster->size()==0){
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


StatusCode egammaSuperClusterBuilder::addL0L1EMCellsToCluster(xAOD::CaloCluster *newCluster,
							      const xAOD::CaloCluster *ref,
							      double etaRef, double phiRef, bool isBarrel,
							      double phiSizePlus, double phiSizeMinus) const
{
  if (!newCluster || !ref) {
    ATH_MSG_ERROR("Invalid input in addL0L1EMCellsToCluster");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("phiRef = " << phiRef << ", phiSizePlus = " << phiSizePlus << ", phiSizeMinus = " << phiSizeMinus);

  double phiPlus = phiRef + phiSizePlus + 0.001;
  double phiMinus = phiRef - phiSizeMinus - 0.001;

  ATH_MSG_DEBUG("phi range = " << phiMinus << " to " << phiPlus);

  //
  xAOD::CaloCluster::const_cell_iterator cell_itr = ref->begin();
  xAOD::CaloCluster::const_cell_iterator cell_end = ref->end();
  //Loop over cells
  for (; cell_itr != cell_end; ++cell_itr) { 
 
    //sanity check on the cell
    const CaloCell* cell = *cell_itr; 
    if (!cell){
      continue;
    }

    // lets remove from consideration if outside the range
    if (isBarrel) { // if the center is in the barrel
      if (fabs(etaRef - cell->eta()) > m_addCellsWindowEtaBarrel){
	continue;
      }
      if (fabs(P4Helpers::deltaPhi(phiRef, cell->phi())) > m_addCellsWindowPhiBarrel){
	continue;
      }
    } else { // if the center is in the endcap
      if (fabs(etaRef - cell->eta()) > m_addCellsWindowEtaEndcap){
	continue;
      }
      if (fabs(P4Helpers::deltaPhi(phiRef, cell->phi())) > m_addCellsWindowPhiEndcap){
	continue;
      }
    }
    
    const CaloDetDescrElement *dde = cell->caloDDE();
    if(!dde){
      continue;
    }

    auto sampling = dde->getSampling();

    // only deal with L1 or PS
    if (CaloCell_ID::EMB1 == sampling || CaloCell_ID::EME1 == sampling ||
	CaloCell_ID::PreSamplerB == sampling || CaloCell_ID::PreSamplerE == sampling) {
      double cell_phi = proxim(cell->phi(), phiRef);
      bool inRange = cell_phi > phiMinus && cell_phi < phiPlus;
      ATH_MSG_DEBUG("Found PS or L1 cell with phi = " << cell_phi << "; inRange = " << inRange);
      if (inRange) {
	newCluster->addCell(cell_itr.index(), cell_itr.weight());
      }
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode egammaSuperClusterBuilder::calibrateCluster(xAOD::CaloCluster* newCluster,
						       const xAOD::EgammaParameters::EgammaType egType) {

  ATH_MSG_DEBUG("========== Initial ==== ");
  ATH_MSG_DEBUG("Cluster Energy Initial: "<<newCluster->e());
  ATH_MSG_DEBUG("Cluster eta  Initial: "<<newCluster->eta());
  ATH_MSG_DEBUG("Cluster phi Initial: "<<newCluster->phi());
  ATH_MSG_DEBUG("Cluster etaBE(1) Initial: "<<newCluster->etaBE(1));
  ATH_MSG_DEBUG("Cluster phiBE(1) Initial: "<<newCluster->phiBE(1));
  ATH_MSG_DEBUG("Cluster etaBE(2) Initial: "<<newCluster->etaBE(2));
  ATH_MSG_DEBUG("Cluster phiBE(2) Initial: "<<newCluster->phiBE(2));
  //Refine Eta1
  if(m_refineEta1){
    ATH_CHECK(refineEta1Position(newCluster));
  }
  //Save the state before the corrections
  newCluster->setAltE(newCluster->e());
  newCluster->setAltEta(newCluster->eta());
  newCluster->setAltPhi(newCluster->phi());
  ATH_MSG_DEBUG("==========no correction/calibration ==== ");
  ATH_MSG_DEBUG("Cluster Energy no correction/calibration: "<<newCluster->e());
  ATH_MSG_DEBUG("Cluster eta  no correction/calibration: "<<newCluster->eta());
  ATH_MSG_DEBUG("Cluster phi no correction/calibration: "<<newCluster->phi());
  ATH_MSG_DEBUG("Cluster etaBE(1) no correction/calibration: "<<newCluster->etaBE(1));
  ATH_MSG_DEBUG("Cluster phiBE(1) no correction/calibration: "<<newCluster->phiBE(1));
  ATH_MSG_DEBUG("Cluster etaBE(2) no correction/calibration: "<<newCluster->etaBE(2));
  ATH_MSG_DEBUG("Cluster phiBE(2) no correction/calibration: "<<newCluster->phiBE(2));
  // first do the corrections
  if (m_correctClusters) {
    ATH_CHECK(m_clusterCorrectionTool->execute(Gaudi::Hive::currentContext(),newCluster,egType,xAOD::EgammaHelpers::isBarrel(newCluster)));
  }
  newCluster->setRawE(newCluster->e());
  newCluster->setRawEta(newCluster->eta());
  newCluster->setRawPhi(newCluster->phi());
  ATH_MSG_DEBUG("========== after position correction ==== ");
  ATH_MSG_DEBUG("Cluster Energy after position corrections: "<<newCluster->e());
  ATH_MSG_DEBUG("Cluster eta  after position corrections: "<<newCluster->eta());
  ATH_MSG_DEBUG("Cluster phi after position corrections: "<<newCluster->phi());
  ATH_MSG_DEBUG("Cluster etaBE(1) after position corrections: "<<newCluster->etaBE(1));
  ATH_MSG_DEBUG("Cluster phiBE(1) after position corrections: "<<newCluster->phiBE(1));
  ATH_MSG_DEBUG("Cluster etaBE(2) after position corrections: "<<newCluster->etaBE(2));
  ATH_MSG_DEBUG("Cluster phiBE(2) after position corrections: "<<newCluster->phiBE(2));
  //
  ATH_CHECK(fillPositionsInCalo(newCluster));
  //At this point we do not have the final tracks vertices attached on the cluster/ new egamma Rec Object.
  //So We will need at the end to do the final update in the EMClusterTool
  //For now apply just cluster info only calibration.
  if (m_calibrateClusters) {
    ATH_CHECK(m_MVACalibSvc->execute(*newCluster,egType));
  }
  ATH_MSG_DEBUG("========== cluster only calibration ==== ");
  ATH_MSG_DEBUG("Cluster Energy after cluster only calibration: "<<newCluster->e());
  return StatusCode::SUCCESS;
}
// ==========================================================================
StatusCode egammaSuperClusterBuilder::fillPositionsInCalo(xAOD::CaloCluster* cluster){
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

StatusCode egammaSuperClusterBuilder::refineEta1Position(xAOD::CaloCluster* cluster) const {
  
  // This only makes sense if we have cells there
  if (!cluster->hasSampling(CaloSampling::EMB1) &&  !cluster->hasSampling(CaloSampling::EME1)) {
    ATH_MSG_DEBUG("No  layer sampling - skipping refine eta ");
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG("Refine layer 1 eta position");
  //
  //Now calculare the position using cells in barrel or endcap  or both
  const double aeta=fabs(cluster->etaBE(2));
  if(aeta<1.6 && cluster->hasSampling(CaloSampling::EMB1)){
    ATH_CHECK(makeCorrection1(cluster,CaloSampling::EMB1));
  }
  if(aeta>1.3 && cluster->hasSampling(CaloSampling::EME1)){
    ATH_CHECK(makeCorrection1(cluster,CaloSampling::EME1));
  }
  return StatusCode::SUCCESS;

}

StatusCode egammaSuperClusterBuilder::makeCorrection1(xAOD::CaloCluster* cluster,    
						      const CaloSampling::CaloSample sample) const {

  //Protections.
  ATH_MSG_DEBUG("Hottest cell in layer 1 ATLAS co-ordinates (eta,phi): (" << cluster->etamax(sample) << " , " << cluster->phimax(sample) << ")");
  if (cluster->etamax(sample)==-999. || cluster->phimax(sample)==-999.) {
    return StatusCode::SUCCESS;
  }
  if (fabs(cluster->etamax(sample))<1E-6 && fabs(cluster->phimax(sample))<1E-6) {
    return StatusCode::SUCCESS;
  }
  //
  //Get the hotest in raw co-ordinates
  const CaloDetDescrManager* mgr = CaloDetDescrManager::instance();
  if (!mgr) {
    ATH_MSG_ERROR("Couldn't create instance of CaloDetDescrManager this should never happen");
    return StatusCode::FAILURE;
  }
  //
  //We have two kinds of enums ...
  CaloCell_ID::CaloSample xsample = (sample == CaloSampling::EMB1) ? CaloCell_ID::EMB1 :CaloCell_ID::EME1;
  //
  const CaloDetDescrElement* dde = mgr->get_element (xsample, cluster->etamax(sample), cluster->phimax(sample));
  if (!dde) {
    ATH_MSG_WARNING("Couldn't get CaloDetDescrElement from mgr for: cluster->etamax(sample): " 
		    << cluster->etamax(sample) <<" cluster->phimax(sample): " << cluster->phimax(sample) << 
		    " will not refine the position in layer1");
    return StatusCode::SUCCESS;
  }
  //
  double etamax = dde->eta_raw();
  double phimax = dde->phi_raw(); 
  ATH_MSG_DEBUG("Hottest cell in layer 1 Calo co-ordinates (eta,phi): (" << etamax << " , " << phimax << ")");
  //  
  //now Locate the +-1 range
  double detastr(-999), dphistr(-999);
  //Raw co-ordinates used here
  etaphi_range(etamax, phimax,xsample,detastr, dphistr);
  //
  //Given the range refine the position employing the smaller window
  if (detastr > 0 && dphistr > 0) {
    CaloLayerCalculator helper;
    const auto cellLink = cluster->getCellLinks();
    helper.fill(cellLink->begin(), cellLink->end(), etamax, phimax, detastr, dphistr, sample);

    //Here is where we (re-)fill the eta in the 1st sampling 
    if(helper.etam()!=-999.) {
      //This is "real" atlas co-ordinates
      cluster->setEta(sample, helper.etam());
      ATH_MSG_DEBUG("Refined Eta layer 1: " << cluster->etaSample(sample));
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode egammaSuperClusterBuilder::addTileGap3CellsinWindow(xAOD::CaloCluster *myCluster) const{
  ATH_MSG_DEBUG("Add Remaining cells in window");
    if (!myCluster) {
      ATH_MSG_ERROR("Invalid input in addRemainingCellsToCluster");
      return StatusCode::FAILURE;
    }

    // This should never be the case now, since I commented it out
    // //DO NOT RUN IF THERE ARE ALREADY ADDED
    // if (myCluster->hasSampling(CaloSampling::TileGap3)) {
    //   return StatusCode::SUCCESS;
    // }
    
    static const double searchWindowEta = 0.2;
    static const double searchWindowPhi = 2*M_PI/64.0 + M_PI/64 ; // ~ 0.15 rad
    std::vector<const CaloCell*> cells;
    cells.reserve(16);
    const CaloCellContainer* inputcells=myCluster->getCellLinks()->getCellContainer();

    if (!inputcells) {
      ATH_MSG_ERROR("No cell container in addRemainingCellsToCluster?");
      return StatusCode::FAILURE;      
    }

    CaloCellList myList(inputcells);

    const std::vector<CaloSampling::CaloSample> samples = {CaloSampling::TileGap3};
    for ( auto samp : samples ) {
      myList.select(myCluster->eta0(), myCluster->phi0(), searchWindowEta, searchWindowPhi,samp);
      cells.insert(cells.end(), myList.begin(), myList.end());
    }

    for ( auto cell : cells ) {
      if( !cell || !cell->caloDDE() ) {
	continue;
      }
      if ( (CaloCell_ID::TileGap3 == cell->caloDDE()->getSampling()) &&
	   (std::abs(cell->eta()) > 1.4 && std::abs(cell->eta()) < 1.6)) {
	int index = inputcells->findIndex(cell->caloDDE()->calo_hash());
	myCluster->addCell(index, 1.);
      }
    }   
  return StatusCode::SUCCESS;
}

bool egammaSuperClusterBuilder::findCentralPosition(float& eta, float& phi, bool& isBar,
						    const std::vector<const xAOD::CaloCluster*>& clusters) const
{

  // local variables for finding the maximum
  float etaRef = 999.;
  float phiRef = 999.;
  bool isBarrel = true;
  float emax = -999*Gaudi::Units::GeV;

  for (auto cluster : clusters) {
    if (cluster->hasSampling(CaloSampling::EMB2)) {
      float thisEmax = cluster->energy_max(CaloSampling::EMB2);
      if (thisEmax > emax) {
	emax = thisEmax;
	isBarrel = true;
	etaRef = cluster->etamax(CaloSampling::EMB2);
	phiRef = cluster->phimax(CaloSampling::EMB2);
      }
    }
    if (cluster->hasSampling(CaloSampling::EME2)) {
      float thisEmax = cluster->energy_max(CaloSampling::EME2);
      if (thisEmax > emax) {
	emax = thisEmax;
	isBarrel = false;
	etaRef = cluster->etamax(CaloSampling::EME2);
	phiRef = cluster->phimax(CaloSampling::EME2);
      }
    }
  }
  if (emax > 0) {
    eta = etaRef;
    phi = phiRef;
    isBar = isBarrel;
    return true;
  } else {
    return false;
  }
}

void egammaSuperClusterBuilder::findPhiSize(double& phiSizePlus, double& phiSizeMinus,
					    float phiRef,
					    const xAOD::CaloCluster* cluster) const
{
  
  auto cell_itr = cluster->cell_cbegin();
  auto cell_end = cluster->cell_cend();   
  
  for (; cell_itr != cell_end; ++cell_itr) { 
    const CaloCell* cell = *cell_itr; 
    if (!cell){
      continue;
    }    
    
    const CaloDetDescrElement *dde = cell->caloDDE();
    if(!dde){
      continue;
    }

    if (CaloCell_ID::EMB2 == dde->getSampling() || CaloCell_ID::EME2 == dde->getSampling()) {
      double cell_phi = proxim(cell->phi(), phiRef);
      if (cell_phi > phiRef) {
	auto diff = cell_phi - phiRef;
	if (diff > phiSizePlus) {
	  phiSizePlus = diff;
	}
      } else {
	auto diff = phiRef - cell_phi;
	if (diff > phiSizeMinus) {
	  phiSizeMinus = diff;
	}
      }
    }
  }
  // some safety checks
  if (phiSizePlus > 1.0) {
    ATH_MSG_WARNING("phiSizePlus is large: " << phiSizePlus << ", capping at 1.0");
    phiSizePlus = 1.0;
  }
  if (phiSizeMinus > 1.0) {
    ATH_MSG_WARNING("phiSizeMinus is large: " << phiSizeMinus << ", capping at 1.0");
    phiSizeMinus = 1.0;
  }
  
}
