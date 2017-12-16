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


#include <vector>

using CLHEP::GeV;

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
  
  declareProperty("RefineEta1", m_refineEta1 = true, 
		  "Whether to Refine Eta1 calculation");

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

  //Need a vector of element Links to the constituent Cluster
  std::vector< ElementLink< xAOD::CaloClusterContainer > > constituentLinks;
  static const SG::AuxElement::Accessor < ElementLink < xAOD::CaloClusterContainer > > sisterCluster("SisterCluster");
  //
  //Start with the seed 
  //Add the EM cells of the seed cluster
  if (AddEMCellsToCluster(newCluster,clusters[0]).isFailure()) {
    ATH_MSG_DEBUG("There was problem adding the cells to cluster");
    delete newCluster;
    return nullptr;
  }  
  //Set the element Link to the relevant constitent
  if (sisterCluster.isAvailable(*clusters[0])) {
    constituentLinks.push_back(sisterCluster(*clusters[0]));
  } else{
    ATH_MSG_WARNING("No sister Link available");
  }
  
  // calculate the seed cluster kinematics.
  CaloClusterKineHelper::calculateKine(newCluster, true, true);
  //
  //Set the eta0/phi0 based on the eta/phi of the seed 
  newCluster->setEta0(newCluster->eta());
  newCluster->setPhi0(newCluster->phi());
  ATH_MSG_DEBUG("========== Seed  ==== ");
  ATH_MSG_DEBUG("Seed eta : "<<newCluster->eta0());
  ATH_MSG_DEBUG("Seed phi : "<<newCluster->phi0());

  //
  // Now continue with the remaining clusters
  for (size_t i = 1; i < acSize; i++) {
    //Add te EM cells of the accumulated to the cluster
    if (AddEMCellsToCluster(newCluster,clusters[i]).isFailure()) {
      ATH_MSG_WARNING("There was problem adding the topocluster cells to the the cluster");
      delete newCluster;
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
  //
  //Set the link from the super cluster to the constituents (accumulated) clusters used. 
  static const SG::AuxElement::Accessor < std::vector< ElementLink< xAOD::CaloClusterContainer > > > caloClusterLinks("constituentClusterLinks");
  caloClusterLinks(*newCluster) = constituentLinks;
  //
  ///Calculate the kinematics of the new cluster, after all cells are added
  CaloClusterKineHelper::calculateKine(newCluster, true, true);
  //
  //Check to see if cluster doesn't have EMB2 OR EME2. If not, kill it.
  if (!newCluster->hasSampling(CaloSampling::EMB2) && !newCluster->hasSampling(CaloSampling::EME2)) {
    ATH_MSG_WARNING("Supercluster doesn't have energy in layer 2. Skipping...");
    delete newCluster;
    return nullptr;     
  }
  //
  //If adding all EM cells I am somehow below the seed threshold then remove 
  //this one
  if(newCluster->et()<m_EtThresholdCut ){
    delete newCluster;
    return nullptr;
  }

  // Apply SW-style summation of TileGap3 cells (if necessary).
  if (AddTileGap3CellsinWindow(newCluster).isFailure()) {
    ATH_MSG_ERROR("Problem with the input cluster when running AddTileGap3CellsinWindow?");
    return nullptr;
  }
  CaloClusterKineHelper::calculateKine(newCluster, true, true);

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
							  const xAOD::CaloCluster *ref) const {
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
    const CaloDetDescrElement *dde = cell->caloDDE();
    if(!dde){
      continue;
    }
    //First add all TileGap3 (consider only E4 cell).
    if (CaloCell_ID::TileGap3 == dde->getSampling()) {
      if( fabs(cell->eta()) >1.4 && fabs(cell->eta()) < 1.6 ) {
	newCluster->addCell(cell_itr.index(), cell_itr.weight());
      }
      continue;  
      //If it is TileGap we are done here
      //either added or not , next cell please
    }
    //First the case where we are both endcap and barrel, i.e in the crack
    if (ref->hasSampling(CaloSampling::EMB2) && ref->hasSampling(CaloSampling::EME2)) {
      //Here we want to allow them either because they around the barrel center 
      //or because they are around the endcap center , so skip only if it fails both 
      if ( (fabs(ref->etaSample(CaloSampling::EMB2)-cell->eta()) > m_addCellsWindowEtaBarrel)&&
	   (fabs(ref->etaSample(CaloSampling::EME2)-cell->eta()) > m_addCellsWindowEtaEndcap) ){
	continue;
      }
      if ((fabs(P4Helpers::deltaPhi(ref->phiSample(CaloSampling::EMB2),cell->phi())) > m_addCellsWindowPhiBarrel)&&
	  (fabs(P4Helpers::deltaPhi(ref->phiSample(CaloSampling::EME2),cell->phi())) > m_addCellsWindowPhiEndcap) ){
	continue;
      }
    }
    else if (xAOD::EgammaHelpers::isBarrel(ref)) {//else if it is barrel
      if (fabs(ref->eta()-cell->eta()) > m_addCellsWindowEtaBarrel){
	continue;
      }
      if (fabs(P4Helpers::deltaPhi(ref->phi(),cell->phi())) > m_addCellsWindowPhiBarrel){
	continue;
      }
    } else { //here must be endcap only
      if (fabs(ref->eta()-cell->eta()) > m_addCellsWindowEtaEndcap){
	continue;
      }
      if (fabs(P4Helpers::deltaPhi(ref->phi(),cell->phi())) > m_addCellsWindowPhiEndcap){
	continue;
      }
    }
    //For the cells we have not skipped either because TileGap or because out of bounds
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


StatusCode egammaSuperClusterBuilder::CalibrateCluster(xAOD::CaloCluster* newCluster,
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
    ATH_CHECK(m_clusterCorrectionTool->execute(newCluster,egType,xAOD::EgammaHelpers::isBarrel(newCluster)));
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
    ATH_CHECK(m_MVACalibTool->execute(newCluster,egType));
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

StatusCode egammaSuperClusterBuilder::AddTileGap3CellsinWindow(xAOD::CaloCluster *myCluster) const{
  ATH_MSG_DEBUG("Add Remaining cells in window");
    if (!myCluster) {
      ATH_MSG_ERROR("Invalid input in AddRemainingCellsToCluster");
      return StatusCode::FAILURE;
    }

    //DO NOT RUN IF THERE ARE ALREADY ADDED
    if (myCluster->hasSampling(CaloSampling::TileGap3)) {
      return StatusCode::SUCCESS;
    }
    static const double searchWindowEta = 0.2;
    static const double searchWindowPhi = 2*M_PI/64.0 + M_PI/64 ; // ~ 0.15 rad
    std::vector<const CaloCell*> cells;
    cells.reserve(16);
    const CaloCellContainer* inputcells=myCluster->getCellLinks()->getCellContainer();

    if (!inputcells) {
      ATH_MSG_ERROR("No cell container in AddRemainingCellsToCluster?");
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
