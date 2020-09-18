/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaSuperClusterBuilder.h"
//
#include "CaloUtils/CaloCellList.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloUtils/CaloLayerCalculator.h"
//
#include "egammaRecEvent/egammaRecContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"
#include "xAODEgamma/EgammaEnums.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEgamma/PhotonxAODHelpers.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
//
#include "CaloGeoHelpers/proxim.h"
#include "FourMomUtils/P4Helpers.h"

#include <cmath>
#include <vector>

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
void
etaphi_range(const CaloDetDescrManager& dd_man,
             double eta,
             double phi,
             CaloCell_ID::CaloSample sampling,
             double& deta,
             double& dphi)
{
  deta = 0;
  dphi = 0;
  // Get the DD element for the central cell.
  const CaloDetDescrElement* elt = dd_man.get_element_raw(sampling, eta, phi);
  if (!elt)
    return;

  // Should be smaller than the eta half-width of any cell.
  constexpr double eps = 0.001;

  // Now look in the negative eta direction.
  const CaloDetDescrElement* elt_l = dd_man.get_element_raw(sampling, eta - elt->deta() - eps, phi);
  double deta_l = 0; // Eta difference on the low (left) side.
  if (elt_l) {
    deta_l = std::abs(eta - elt_l->eta_raw()) + eps;
  }
  // Now look in the positive eta direction.
  const CaloDetDescrElement* elt_r = dd_man.get_element_raw(sampling, eta + elt->deta() + eps, phi);
  double deta_r = 0; // Eta difference on the high (right) side.
  if (elt_r) {
    deta_r = std::abs(eta - elt_r->eta_raw()) + eps;
  }

  // Total deta is twice the maximum.
  deta = 2 * std::max(deta_r, deta_l);
  // Now for the phi variation.
  // The phi size can change as a function of eta, but not of phi.
  // Thus we have to look again at the adjacent eta cells, and
  // take the largest variation.
  // Now look in the negative eta direction.
  elt_l = dd_man.get_element_raw(
    sampling, eta - elt->deta() - eps, CaloPhiRange::fix(phi - elt->dphi() - eps));

  double dphi_l = 0; // Phi difference on the low-eta () side.
  if (elt_l) {
    dphi_l = std::abs(CaloPhiRange::fix(phi - elt_l->phi_raw())) + eps;
  }
  // Now look in the positive eta direction.
  elt_r = dd_man.get_element_raw(
    sampling, eta + elt->deta() + eps, CaloPhiRange::fix(phi - elt->dphi() - eps));
  double dphi_r = 0; // Phi difference on the positive (down) side.
  if (elt_r) {
    dphi_r = std::abs(CaloPhiRange::fix(phi - elt_r->phi_raw())) + eps;
  }
  // Total dphi is twice the maximum.
  dphi = 2 * std::max(dphi_l, dphi_r);
}
}

//////////////////////////////////////////////////////////////////////////////
// Athena interfaces.
//////////////////////////////////////////////////////////////////////////////

// Constructor.
egammaSuperClusterBuilder::egammaSuperClusterBuilder(const std::string& name,
                                                     ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
{

  m_searchWindowPhiBarrel = m_searchWindowPhiCellsBarrel * s_cellPhiSize * 0.5;
  m_searchWindowEtaBarrel = m_searchWindowEtaCellsBarrel * s_cellEtaSize * 0.5;
  m_searchWindowPhiEndcap = m_searchWindowPhiCellsEndcap * s_cellPhiSize * 0.5;
  m_searchWindowEtaEndcap = m_searchWindowEtaCellsEndcap * s_cellEtaSize * 0.5;

  m_addCellsWindowPhiBarrel = m_addCellsWindowPhiCellsBarrel * s_cellPhiSize * 0.5;
  m_addCellsWindowEtaBarrel = m_addCellsWindowEtaCellsBarrel * s_cellEtaSize * 0.5;
  m_addCellsWindowPhiEndcap = m_addCellsWindowPhiCellsEndcap * s_cellPhiSize * 0.5;
  m_addCellsWindowEtaEndcap = m_addCellsWindowEtaCellsEndcap * s_cellEtaSize * 0.5;

  // the +1 is to account for the offset in the centers of the clusters
  m_extraL0L1PhiSize = m_extraL0L1PhiSizeCells * s_cellPhiSize;
}

StatusCode
egammaSuperClusterBuilder::initialize()
{
  ATH_MSG_DEBUG(" Initializing egamma SuperClusterBuilder");
  m_searchWindowPhiBarrel = m_searchWindowPhiCellsBarrel * s_cellPhiSize * 0.5;
  m_searchWindowEtaBarrel = m_searchWindowEtaCellsBarrel * s_cellEtaSize * 0.5;
  m_searchWindowPhiEndcap = m_searchWindowPhiCellsEndcap * s_cellPhiSize * 0.5;
  m_searchWindowEtaEndcap = m_searchWindowEtaCellsEndcap * s_cellEtaSize * 0.5;

  m_addCellsWindowPhiBarrel = m_addCellsWindowPhiCellsBarrel * s_cellPhiSize * 0.5;
  m_addCellsWindowEtaBarrel = m_addCellsWindowEtaCellsBarrel * s_cellEtaSize * 0.5;
  m_addCellsWindowPhiEndcap = m_addCellsWindowPhiCellsEndcap * s_cellPhiSize * 0.5;
  m_addCellsWindowEtaEndcap = m_addCellsWindowEtaCellsEndcap * s_cellEtaSize * 0.5;

  // the +1 is to account for the offset in the centers of the clusters
  m_extraL0L1PhiSize = m_extraL0L1PhiSizeCells * s_cellPhiSize;

  if (m_addCellsWindowPhiCellsBarrel % 2 == 0 || m_addCellsWindowPhiCellsEndcap % 2 == 0 ||
      m_addCellsWindowEtaCellsBarrel % 2 == 0 || m_addCellsWindowEtaCellsEndcap % 2 == 0) {
    ATH_MSG_FATAL("For adding cells relative to the hottest cell to be symmetric, the AddCells "
                  "window size needs to be odd");
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_clusterCorrectionTool.retrieve());
  ATH_CHECK(m_MVACalibSvc.retrieve());

  if (!m_egammaCheckEnergyDepositTool.empty()) {
    ATH_CHECK(m_egammaCheckEnergyDepositTool.retrieve());
  } else {
    m_egammaCheckEnergyDepositTool.disable();
  }

  return StatusCode::SUCCESS;
}

bool
egammaSuperClusterBuilder::matchesInWindow(const xAOD::CaloCluster* ref,
                                           const xAOD::CaloCluster* clus) const
{
  // First the case where we are both endcap and barrel, i.e in the crack
  // Check around both measurements of the seed
  if (ref->hasSampling(CaloSampling::EMB2) && ref->hasSampling(CaloSampling::EME2)) {
    float dEta(fabs(ref->eta() - clus->eta()));
    float dPhi(fabs(P4Helpers::deltaPhi(ref->phi(), clus->phi())));
    //
    float dEtaBarrel(fabs(ref->etaSample(CaloSampling::EMB2) - clus->eta()));
    float dPhiBarrel(fabs(P4Helpers::deltaPhi(ref->phiSample(CaloSampling::EMB2), clus->phi())));
    //
    float dEtaEndcap(fabs(ref->etaSample(CaloSampling::EME2) - clus->eta()));
    float dPhiEndcap(fabs(P4Helpers::deltaPhi(ref->phiSample(CaloSampling::EME2), clus->phi())));
    // Matches any in case of split
    return ((dEta < m_searchWindowEtaBarrel && dPhi < m_searchWindowPhiBarrel) ||
            (dEta < m_searchWindowEtaEndcap && dPhi < m_searchWindowPhiEndcap) ||
            (dEtaBarrel < m_searchWindowEtaBarrel && dPhiBarrel < m_searchWindowPhiBarrel) ||
            (dEtaEndcap < m_searchWindowEtaEndcap && dPhiEndcap < m_searchWindowPhiEndcap));
  } if (xAOD::EgammaHelpers::isBarrel(clus)) {
    float dEta(fabs(ref->eta() - clus->eta()));
    float dPhi(fabs(P4Helpers::deltaPhi(ref->phi(), clus->phi())));
    return (dEta < m_searchWindowEtaBarrel && dPhi < m_searchWindowPhiBarrel);
  } 
    float dEta(fabs(ref->eta() - clus->eta()));
    float dPhi(fabs(P4Helpers::deltaPhi(ref->phi(), clus->phi())));
    return (dEta < m_searchWindowEtaEndcap && dPhi < m_searchWindowPhiEndcap);
  
}

std::unique_ptr<xAOD::CaloCluster>
egammaSuperClusterBuilder::createNewCluster(const EventContext& ctx,
                                            const std::vector<const xAOD::CaloCluster*>& clusters,
                                            const CaloDetDescrManager& mgr,
                                            xAOD::EgammaParameters::EgammaType egType) const
{

  const auto acSize = clusters.size();
  if (acSize == 0) {
    ATH_MSG_ERROR("Missing the seed cluster! Should not happen.");
    return nullptr;
  }

  std::unique_ptr<xAOD::CaloCluster> newCluster(
    CaloClusterStoreHelper::makeCluster(clusters.at(0)->getCellLinks()->getCellContainer()));
  if (!newCluster) {
    ATH_MSG_ERROR("CaloClusterStoreHelper::makeCluster failed.");
    return nullptr;
  }
  newCluster->setClusterSize(xAOD::CaloCluster::SuperCluster);
  // Let's try to find the eta and phi of the hottest cell in L2.
  // This will be used as the center for restricting the cluster size.
  // In the future can refine (or add sanity checks) to the selection
  CentralPosition cpRef = findCentralPosition(clusters);
  // these are the same as the reference but in calo frame (after the processing below)
  CentralPosition cp0 = cpRef;

  // Get the hotest in raw co-ordinates
  if (cp0.emaxB > 0) {
    CaloCell_ID::CaloSample xsample = CaloCell_ID::EMB2;
    const CaloDetDescrElement* dde = mgr.get_element(xsample, cpRef.etaB, cpRef.phiB);
    if (dde) {
      cp0.etaB = dde->eta_raw();
      cp0.phiB = dde->phi_raw();
    } else {
      ATH_MSG_WARNING("Couldn't get CaloDetDescrElement from mgr for eta = "
                      << cpRef.etaB << ", phi = " << cpRef.phiB);
    }
  }
  if (cp0.emaxEC > 0) {
    CaloCell_ID::CaloSample xsample = CaloCell_ID::EME2;
    const CaloDetDescrElement* dde = mgr.get_element(xsample, cpRef.etaEC, cpRef.phiEC);
    if (dde) {
      cp0.etaEC = dde->eta_raw();
      cp0.phiEC = dde->phi_raw();
    } else {
      ATH_MSG_WARNING("Couldn't get CaloDetDescrElement from mgr for eta = "
                      << cpRef.etaEC << ", phi = " << cpRef.phiEC);
    }
  }

  // Set the eta0/phi0 based on the references, but in raw coordinates
  if (cp0.emaxB >= cp0.emaxEC) {
    newCluster->setEta0(cp0.etaB);
    newCluster->setPhi0(cp0.phiB);
  } else {
    newCluster->setEta0(cp0.etaEC);
    newCluster->setPhi0(cp0.phiEC);
  }

  // Need a vector of element Links to the constituent Cluster
  std::vector<ElementLink<xAOD::CaloClusterContainer>> constituentLinks;
  static const SG::AuxElement::Accessor<ElementLink<xAOD::CaloClusterContainer>> sisterCluster(
    "SisterCluster");

  // Now add the cluster cells (other than L1 and PS)
  for (size_t i = 0; i < acSize; i++) {
    // Add te EM cells of the accumulated to the cluster
    if (addEMCellsToCluster(newCluster.get(), clusters[i], cp0).isFailure()) {
      ATH_MSG_DEBUG("There was problem adding the topocluster cells to the the cluster:  "
                    "potentially no L2 or L3 cells in cluster");
      return nullptr;
    }
    // Set the element Link to the constitents
    if (sisterCluster.isAvailable(*clusters[i])) {
      constituentLinks.push_back(sisterCluster(*clusters[i]));
    } else {
      ATH_MSG_WARNING("No sister Link available");
    }
  }

  // Now calculate the cluster size; use that for restricting the L1 cells
  PhiSize phiSize = findPhiSize(cp0, newCluster.get());

  // now add L1 cells
  for (size_t i = 0; i < acSize; i++) {
    // Add te EM cells of the accumulated to the cluster
    if (addL0L1EMCellsToCluster(newCluster.get(), clusters[i], cp0, phiSize).isFailure()) {
      ATH_MSG_WARNING(
        "There was problem adding the topocluster PS and L1 cells to the the cluster");
      return nullptr;
    }
  }

  // Apply SW-style summation of TileGap3 cells (if necessary).
  if (addTileGap3CellsinWindow(newCluster.get(), mgr).isFailure()) {
    ATH_MSG_ERROR("Problem with the input cluster when running AddTileGap3CellsinWindow?");
    return nullptr;
  }

  // Set the link from the super cluster to the constituents (accumulated) clusters used.
  static const SG::AuxElement::Accessor<std::vector<ElementLink<xAOD::CaloClusterContainer>>>
    caloClusterLinks("constituentClusterLinks");
  caloClusterLinks(*newCluster) = constituentLinks;

  /// Calculate the kinematics of the new cluster, after all cells are added
  CaloClusterKineHelper::calculateKine(newCluster.get(), true, true);

  // If adding all EM cells we are somehow below the seed threshold then remove
  if (newCluster->et() < m_EtThresholdCut) {
    return nullptr;
  }

  // Check to see if cluster pases basic requirements. If not, kill it.
  if (!m_egammaCheckEnergyDepositTool.empty() &&
      !m_egammaCheckEnergyDepositTool->checkFractioninSamplingCluster(newCluster.get())) {
    ATH_MSG_DEBUG("Cluster failed sample check: dont make ROI");
    return nullptr;
  }

  // Apply correction  calibration
  if (calibrateCluster(ctx, newCluster.get(), mgr, egType).isFailure()) {
    ATH_MSG_WARNING("There was problem calibrating the object");
    return nullptr;
  }
  // return the new cluster
  return newCluster;
}

StatusCode
egammaSuperClusterBuilder::addEMCellsToCluster(
  xAOD::CaloCluster* newCluster,
  const xAOD::CaloCluster* ref,
  const egammaSuperClusterBuilder::CentralPosition& cp0) const
{
  if (!newCluster || !ref) {
    ATH_MSG_ERROR("Invalid input in AddEMCellsToCluster");
    return StatusCode::FAILURE;
  }
  //
  xAOD::CaloCluster::const_cell_iterator cell_itr = ref->begin();
  xAOD::CaloCluster::const_cell_iterator cell_end = ref->end();
  // Loop over cells
  for (; cell_itr != cell_end; ++cell_itr) {

    // sanity check on the cell
    const CaloCell* cell = *cell_itr;
    if (!cell) {
      continue;
    }

    bool addCell = false;

    // lets remove from consideration if outside the range
    if (cp0.emaxB > 0) { // has cells in the barrel
      if (fabs(cp0.etaB - cell->caloDDE()->eta_raw()) < m_addCellsWindowEtaBarrel &&
          fabs(P4Helpers::deltaPhi(cp0.phiB, cell->caloDDE()->phi_raw())) <
            m_addCellsWindowPhiBarrel) {
        addCell = true;
      }
    }
    if (cp0.emaxEC > 0) { // has cells in the endcap
      if (fabs(cp0.etaEC - cell->caloDDE()->eta_raw()) < m_addCellsWindowEtaEndcap &&
          fabs(P4Helpers::deltaPhi(cp0.phiEC, cell->caloDDE()->phi_raw())) <
            m_addCellsWindowPhiEndcap) {
        addCell = true;
      }
    }

    if (!addCell)
      continue;

    const CaloDetDescrElement* dde = cell->caloDDE();
    if (!dde) {
      continue;
    }

    auto sampling = dde->getSampling();

    // skip if sampling is L1 or presample, since that is a separate step
    if (CaloCell_ID::EMB1 == sampling || CaloCell_ID::EME1 == sampling ||
        CaloCell_ID::PreSamplerB == sampling || CaloCell_ID::PreSamplerE == sampling) {
      continue;
    }

    // For the cells we have not skipped either because TileGap, bounds, or L1
    if (dde->getSubCalo() == CaloCell_ID::LAREM) {
      // Avoid summing inner wheel Endcap cells
      if (!(dde->is_lar_em_endcap_inner())) {
        newCluster->addCell(cell_itr.index(), cell_itr.weight());
      }
    }
  } // Loop over cells
  if (newCluster->size() == 0) {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode
egammaSuperClusterBuilder::addL0L1EMCellsToCluster(
  xAOD::CaloCluster* newCluster,
  const xAOD::CaloCluster* ref,
  const egammaSuperClusterBuilder::CentralPosition& cp0,
  const egammaSuperClusterBuilder::PhiSize& phiSize) const
{
  if (!newCluster || !ref) {
    ATH_MSG_ERROR("Invalid input in addL0L1EMCellsToCluster");
    return StatusCode::FAILURE;
  }

  float phiPlusB = cp0.phiB + phiSize.plusB + m_extraL0L1PhiSize;
  float phiMinusB = cp0.phiB - phiSize.minusB - m_extraL0L1PhiSize;

  ATH_MSG_DEBUG("barrel phi range = " << phiMinusB << " to " << phiPlusB);

  float phiPlusEC = cp0.phiEC + phiSize.plusEC + m_extraL0L1PhiSize;
  float phiMinusEC = cp0.phiEC - phiSize.minusEC - m_extraL0L1PhiSize;

  ATH_MSG_DEBUG("endcap phi range = " << phiMinusEC << " to " << phiPlusEC);

  //
  xAOD::CaloCluster::const_cell_iterator cell_itr = ref->begin();
  xAOD::CaloCluster::const_cell_iterator cell_end = ref->end();
  // Loop over cells
  for (; cell_itr != cell_end; ++cell_itr) {

    // sanity check on the cell
    const CaloCell* cell = *cell_itr;
    if (!cell) {
      continue;
    }

    bool addCell = false;

    // lets remove from consideration if outside the range
    if (cp0.emaxB > 0) { // has cells in the barrel
      if (fabs(cp0.etaB - cell->caloDDE()->eta_raw()) < m_addCellsWindowEtaBarrel &&
          fabs(P4Helpers::deltaPhi(cp0.phiB, cell->caloDDE()->phi_raw())) <
            m_addCellsWindowPhiBarrel) {
        addCell = true;
      }
    }
    if (cp0.emaxEC > 0) { // has cells in the endcap
      if (fabs(cp0.etaEC - cell->caloDDE()->eta_raw()) < m_addCellsWindowEtaEndcap &&
          fabs(P4Helpers::deltaPhi(cp0.phiEC, cell->caloDDE()->phi_raw())) <
            m_addCellsWindowPhiEndcap) {
        addCell = true;
      }
    }

    if (!addCell)
      continue;
    const CaloDetDescrElement* dde = cell->caloDDE();
    if (!dde) {
      continue;
    }

    auto sampling = dde->getSampling();

    // only deal with L1 or PS
    if (CaloCell_ID::EMB1 == sampling || CaloCell_ID::PreSamplerB == sampling) {
      double cell_phi = proxim(cell->caloDDE()->phi_raw(), cp0.phiB);
      bool inRange = cell_phi > phiMinusB && cell_phi < phiPlusB;
      ATH_MSG_DEBUG("Found PS or L1 cell with phi = " << cell_phi << "; inRange = " << inRange);
      if (inRange) {
        newCluster->addCell(cell_itr.index(), cell_itr.weight());
      }
    }
    if (CaloCell_ID::EME1 == sampling || CaloCell_ID::PreSamplerE == sampling) {
      double cell_phi = proxim(cell->caloDDE()->phi_raw(), cp0.phiEC);
      bool inRange = cell_phi > phiMinusEC && cell_phi < phiPlusEC;
      ATH_MSG_DEBUG("Found PS or L1 cell with phi = " << cell_phi << "; inRange = " << inRange);
      if (inRange) {
        newCluster->addCell(cell_itr.index(), cell_itr.weight());
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode
egammaSuperClusterBuilder::calibrateCluster(const EventContext& ctx,
                                            xAOD::CaloCluster* newCluster,
                                            const CaloDetDescrManager& mgr,
                                            const xAOD::EgammaParameters::EgammaType egType) const
{

  ATH_CHECK(refineEta1Position(newCluster, mgr));
  // Save the state before the corrections
  newCluster->setAltE(newCluster->e());
  newCluster->setAltEta(newCluster->eta());
  newCluster->setAltPhi(newCluster->phi());
  // first do the corrections
  ATH_CHECK(m_clusterCorrectionTool->execute(
    ctx, newCluster, egType, xAOD::EgammaHelpers::isBarrel(newCluster)));
  newCluster->setRawE(newCluster->e());
  newCluster->setRawEta(newCluster->eta());
  newCluster->setRawPhi(newCluster->phi());
  //
  ATH_CHECK(fillPositionsInCalo(newCluster, mgr));
  // At this point we do not have the final tracks vertices attached on the cluster/ new egamma Rec
  // Object. So We will need at the end to do the final update in the EMClusterTool For now apply
  // just cluster info only calibration.
  ATH_CHECK(m_MVACalibSvc->execute(*newCluster, egType));

  return StatusCode::SUCCESS;
}
// ==========================================================================
StatusCode
egammaSuperClusterBuilder::fillPositionsInCalo(xAOD::CaloCluster* cluster,
                                               const CaloDetDescrManager& mgr) const
{
  const bool isBarrel = xAOD::EgammaHelpers::isBarrel(cluster);
  CaloCell_ID::CaloSample sample = isBarrel ? CaloCell_ID::EMB2 : CaloCell_ID::EME2;
  // eta and phi of the cluster in the calorimeter frame
  double eta;
  double phi;
  m_caloCellDetPos.getDetPosition(mgr, sample, cluster->eta(), cluster->phi(), eta, phi);
  cluster->insertMoment(xAOD::CaloCluster::ETACALOFRAME, eta);
  cluster->insertMoment(xAOD::CaloCluster::PHICALOFRAME, phi);
  //  eta in the second sampling
  m_caloCellDetPos.getDetPosition(mgr, sample, cluster->etaBE(2), cluster->phiBE(2), eta, phi);
  cluster->insertMoment(xAOD::CaloCluster::ETA2CALOFRAME, eta);
  cluster->insertMoment(xAOD::CaloCluster::PHI2CALOFRAME, phi);
  //  eta in the first sampling
  sample = isBarrel ? CaloCell_ID::EMB1 : CaloCell_ID::EME1;
  m_caloCellDetPos.getDetPosition(mgr, sample, cluster->etaBE(1), cluster->phiBE(1), eta, phi);
  cluster->insertMoment(xAOD::CaloCluster::ETA1CALOFRAME, eta);
  cluster->insertMoment(xAOD::CaloCluster::PHI1CALOFRAME, phi);

  return StatusCode::SUCCESS;
}

StatusCode
egammaSuperClusterBuilder::refineEta1Position(xAOD::CaloCluster* cluster,
                                              const CaloDetDescrManager& mgr) const
{

  // This only makes sense if we have cells there
  if (!cluster->hasSampling(CaloSampling::EMB1) && !cluster->hasSampling(CaloSampling::EME1)) {
    ATH_MSG_DEBUG("No  layer sampling - skipping refine eta ");
    return StatusCode::SUCCESS;
  }
  // Now calculare the position using cells in barrel or endcap  or both
  const double aeta = fabs(cluster->etaBE(2));
  if (aeta < 1.6 && cluster->hasSampling(CaloSampling::EMB1)) {
    ATH_CHECK(makeCorrection1(cluster, mgr, CaloSampling::EMB1));
  }
  if (aeta > 1.3 && cluster->hasSampling(CaloSampling::EME1)) {
    ATH_CHECK(makeCorrection1(cluster, mgr, CaloSampling::EME1));
  }
  return StatusCode::SUCCESS;
}

StatusCode
egammaSuperClusterBuilder::makeCorrection1(xAOD::CaloCluster* cluster,
                                           const CaloDetDescrManager& mgr,
                                           const CaloSampling::CaloSample sample) const
{

  // Protections.
  ATH_MSG_DEBUG("Hottest cell in layer 1 ATLAS co-ordinates (eta,phi): ("
                << cluster->etamax(sample) << " , " << cluster->phimax(sample) << ")");
  if (cluster->etamax(sample) == -999. || cluster->phimax(sample) == -999.) {
    return StatusCode::SUCCESS;
  }
  if (fabs(cluster->etamax(sample)) < 1E-6 && fabs(cluster->phimax(sample)) < 1E-6) {
    return StatusCode::SUCCESS;
  }
  // Get the hotest in raw co-ordinates
  //
  // We have two kinds of enums ...
  CaloCell_ID::CaloSample xsample =
    (sample == CaloSampling::EMB1) ? CaloCell_ID::EMB1 : CaloCell_ID::EME1;
  //
  const CaloDetDescrElement* dde =
    mgr.get_element(xsample, cluster->etamax(sample), cluster->phimax(sample));
  if (!dde) {
    ATH_MSG_WARNING("Couldn't get CaloDetDescrElement from mgr for: cluster->etamax(sample): "
                    << cluster->etamax(sample) << " cluster->phimax(sample): "
                    << cluster->phimax(sample) << " will not refine the position in layer1");
    return StatusCode::SUCCESS;
  }
  //
  double etamax = dde->eta_raw();
  double phimax = dde->phi_raw();
  ATH_MSG_DEBUG("Hottest cell in layer 1 Calo co-ordinates (eta,phi): (" << etamax << " , "
                                                                         << phimax << ")");
  //
  // now Locate the +-1 range
  double detastr(-999);
  double dphistr(-999);
  // Raw co-ordinates used here
  etaphi_range(mgr, etamax, phimax, xsample, detastr, dphistr);
  //
  // Given the range refine the position employing the smaller window
  if (detastr > 0 && dphistr > 0) {
    CaloLayerCalculator helper;
    const auto cellLink = cluster->getCellLinks();
    helper.fill(cellLink->begin(), cellLink->end(), etamax, phimax, detastr, dphistr, sample);

    // Here is where we (re-)fill the eta in the 1st sampling
    if (helper.etam() != -999.) {
      // This is "real" atlas co-ordinates
      cluster->setEta(sample, helper.etam());
      ATH_MSG_DEBUG("Refined Eta layer 1: " << cluster->etaSample(sample));
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode
egammaSuperClusterBuilder::addTileGap3CellsinWindow(xAOD::CaloCluster* myCluster,
                                                    const CaloDetDescrManager& mgr) const
{

  if (!myCluster) {
    ATH_MSG_ERROR("Invalid input in addRemainingCellsToCluster");
    return StatusCode::FAILURE;
  }

  static const double searchWindowEta = 0.2;
  static const double searchWindowPhi = 2 * M_PI / 64.0 + M_PI / 64; // ~ 0.15 rad
  std::vector<const CaloCell*> cells;
  cells.reserve(16);
  const CaloCellContainer* inputcells = myCluster->getCellLinks()->getCellContainer();

  if (!inputcells) {
    ATH_MSG_ERROR("No cell container in addRemainingCellsToCluster?");
    return StatusCode::FAILURE;
  }

  CaloCellList myList(inputcells);

  const std::vector<CaloSampling::CaloSample> samples = { CaloSampling::TileGap3 };
  for (auto samp : samples) {
    myList.select(
      mgr, myCluster->eta0(), myCluster->phi0(), searchWindowEta, searchWindowPhi, samp);
    cells.insert(cells.end(), myList.begin(), myList.end());
  }

  for (auto cell : cells) {
    if (!cell || !cell->caloDDE()) {
      continue;
    }
    if ((CaloCell_ID::TileGap3 == cell->caloDDE()->getSampling()) &&
        (std::abs(cell->caloDDE()->eta_raw()) > 1.4 &&
         std::abs(cell->caloDDE()->eta_raw()) < 1.6)) {
      int index = inputcells->findIndex(cell->caloDDE()->calo_hash());
      myCluster->addCell(index, 1.);
    }
  }
  return StatusCode::SUCCESS;
}

egammaSuperClusterBuilder::CentralPosition
egammaSuperClusterBuilder::findCentralPosition(
  const std::vector<const xAOD::CaloCluster*>& clusters) const
{

  CentralPosition cp;
  for (auto cluster : clusters) {
    if (cluster->hasSampling(CaloSampling::EMB2)) {
      float thisEmax = cluster->energy_max(CaloSampling::EMB2);
      if (thisEmax > cp.emaxB) {
        cp.emaxB = thisEmax;
        cp.etaB = cluster->etamax(CaloSampling::EMB2);
        cp.phiB = cluster->phimax(CaloSampling::EMB2);
      }
    }
    if (cluster->hasSampling(CaloSampling::EME2)) {
      float thisEmax = cluster->energy_max(CaloSampling::EME2);
      if (thisEmax > cp.emaxEC) {
        cp.emaxEC = thisEmax;
        cp.etaEC = cluster->etamax(CaloSampling::EME2);
        cp.phiEC = cluster->phimax(CaloSampling::EME2);
      }
    }
  }
  return cp;
}

egammaSuperClusterBuilder::PhiSize
egammaSuperClusterBuilder::findPhiSize(const egammaSuperClusterBuilder::CentralPosition& cp0,
                                       const xAOD::CaloCluster* cluster) const
{

  PhiSize phiSize;

  auto cell_itr = cluster->cell_cbegin();
  auto cell_end = cluster->cell_cend();

  for (; cell_itr != cell_end; ++cell_itr) {
    const CaloCell* cell = *cell_itr;
    if (!cell) {
      continue;
    }

    const CaloDetDescrElement* dde = cell->caloDDE();
    if (!dde) {
      continue;
    }

    if (CaloCell_ID::EMB2 == dde->getSampling()) {
      const float phi0 = cp0.phiB;
      double cell_phi = proxim(cell->caloDDE()->phi_raw(), phi0);
      if (cell_phi > phi0) {
        auto diff = cell_phi - phi0;
        if (diff > phiSize.plusB) {
          phiSize.plusB = diff;
        }
      } else {
        auto diff = phi0 - cell_phi;
        if (diff > phiSize.minusB) {
          phiSize.minusB = diff;
        }
      }
    } else if (CaloCell_ID::EME2 == dde->getSampling()) {
      const float phi0 = cp0.phiEC;
      double cell_phi = proxim(cell->caloDDE()->phi_raw(), phi0);
      if (cell_phi > phi0) {
        auto diff = cell_phi - phi0;
        if (diff > phiSize.plusEC) {
          phiSize.plusEC = diff;
        }
      } else {
        auto diff = phi0 - cell_phi;
        if (diff > phiSize.minusEC) {
          phiSize.minusEC = diff;
        }
      }
    }
  }
  // some safety checks
  if (phiSize.plusB > 1.0) {
    ATH_MSG_WARNING("phiSizePlusB is large: " << phiSize.plusB << ", capping at 1.0");
    phiSize.plusB = 1.0;
  }
  if (phiSize.plusEC > 1.0) {
    ATH_MSG_WARNING("phiSizePlusEC is large: " << phiSize.plusEC << ", capping at 1.0");
    phiSize.plusEC = 1.0;
  }
  if (phiSize.minusB > 1.0) {
    ATH_MSG_WARNING("phiSizeMinusB is large: " << phiSize.minusB << ", capping at 1.0");
    phiSize.minusB = 1.0;
  }
  if (phiSize.minusEC > 1.0) {
    ATH_MSG_WARNING("phiSizeMinusEC is large: " << phiSize.minusEC << ", capping at 1.0");
    phiSize.minusEC = 1.0;
  }
  return phiSize;
}
