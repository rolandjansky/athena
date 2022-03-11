/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaSuperClusterBuilderBase.h"

#include "CaloUtils/CaloCellList.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloUtils/CaloLayerCalculator.h"

#include "egammaRecEvent/egammaRecContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"
#include "xAODEgamma/EgammaEnums.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEgamma/PhotonxAODHelpers.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"

#include "CaloGeoHelpers/proxim.h"
#include "FourMomUtils/P4Helpers.h"

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
void
etaphi_range(const CaloDetDescrManager& dd_man,
             double eta,
             double phi,
             CaloCell_ID::CaloSample sampling,
             double& deta,
             double& dphi)
{
  // Should be smaller than the eta half-width of any cell.
  constexpr double eps = 0.001;
  deta = 0;
  dphi = 0;
  // Get the DD element for the central cell.
  const CaloDetDescrElement* elt = dd_man.get_element_raw(sampling, eta, phi);
  if (!elt)
    return;

  // Now look in the negative eta direction.
  const CaloDetDescrElement* elt_l =
    dd_man.get_element_raw(sampling, eta - elt->deta() - eps, phi);
  double deta_l = 0; // Eta difference on the low (left) side.
  if (elt_l) {
    deta_l = std::abs(eta - elt_l->eta_raw()) + eps;
  }
  // Now look in the positive eta direction.
  const CaloDetDescrElement* elt_r =
    dd_man.get_element_raw(sampling, eta + elt->deta() + eps, phi);
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
  elt_l = dd_man.get_element_raw(sampling,
                                 eta - elt->deta() - eps,
                                 CaloPhiRange::fix(phi - elt->dphi() - eps));

  double dphi_l = 0; // Phi difference on the low-eta () side.
  if (elt_l) {
    dphi_l = std::abs(CaloPhiRange::fix(phi - elt_l->phi_raw())) + eps;
  }
  // Now look in the positive eta direction.
  elt_r = dd_man.get_element_raw(sampling,
                                 eta + elt->deta() + eps,
                                 CaloPhiRange::fix(phi - elt->dphi() - eps));
  double dphi_r = 0; // Phi difference on the positive (down) side.
  if (elt_r) {
    dphi_r = std::abs(CaloPhiRange::fix(phi - elt_r->phi_raw())) + eps;
  }
  // Total dphi is twice the maximum.
  dphi = 2 * std::max(dphi_l, dphi_r);
}

/** Function to decorate the calo cluster with position variables.
 * Filling eta phi in calo-frame:
 * - xAOD::CaloCluster::ETACALOFRAME
 * - xAOD::CaloCluster::PHICALOFRAME
 * - xAOD::CaloCluster::ETA2CALOFRAME
 * - xAOD::CaloCluster::PHI2CALOFRAME
 * - xAOD::CaloCluster::ETA1CALOFRAME
 * - xAOD::CaloCluster::PHI1CALOFRAME
 */

void
fillPositionsInCalo(xAOD::CaloCluster* cluster, const CaloDetDescrManager& mgr)
{
  const bool isBarrel = xAOD::EgammaHelpers::isBarrel(cluster);
  CaloCell_ID::CaloSample sample =
    isBarrel ? CaloCell_ID::EMB2 : CaloCell_ID::EME2;
  // eta and phi of the cluster in the calorimeter frame
  double eta;
  double phi;
  CaloCellDetPos::getDetPosition(
    mgr, sample, cluster->eta(), cluster->phi(), eta, phi);
  cluster->insertMoment(xAOD::CaloCluster::ETACALOFRAME, eta);
  cluster->insertMoment(xAOD::CaloCluster::PHICALOFRAME, phi);
  //  eta in the second sampling
  CaloCellDetPos::getDetPosition(
    mgr, sample, cluster->etaBE(2), cluster->phiBE(2), eta, phi);
  cluster->insertMoment(xAOD::CaloCluster::ETA2CALOFRAME, eta);
  cluster->insertMoment(xAOD::CaloCluster::PHI2CALOFRAME, phi);
  //  eta in the first sampling
  sample = isBarrel ? CaloCell_ID::EMB1 : CaloCell_ID::EME1;
  CaloCellDetPos::getDetPosition(
    mgr, sample, cluster->etaBE(1), cluster->phiBE(1), eta, phi);
  cluster->insertMoment(xAOD::CaloCluster::ETA1CALOFRAME, eta);
  cluster->insertMoment(xAOD::CaloCluster::PHI1CALOFRAME, phi);
}

/** functions to make 1st sampling (strips) specific corrections*/
void
makeCorrection1(xAOD::CaloCluster* cluster,
                const CaloDetDescrManager& mgr,
                const CaloSampling::CaloSample sample)
{
  // Protections.
  if (cluster->etamax(sample) == -999. || cluster->phimax(sample) == -999.) {
    return;
  }
  if (std::abs(cluster->etamax(sample)) < 1E-6 &&
      std::abs(cluster->phimax(sample)) < 1E-6) {
    return;
  }
  // Get the hottest in raw co-ordinates
  // We have two kinds of enums ...
  CaloCell_ID::CaloSample xsample =
    (sample == CaloSampling::EMB1) ? CaloCell_ID::EMB1 : CaloCell_ID::EME1;
  //
  const CaloDetDescrElement* dde =
    mgr.get_element(xsample, cluster->etamax(sample), cluster->phimax(sample));
  if (!dde) {
    return;
  }
  //
  double etamax = dde->eta_raw();
  double phimax = dde->phi_raw();
  // now Locate the +-1 range
  double detastr(-999);
  double dphistr(-999);
  // Raw co-ordinates used here
  etaphi_range(mgr, etamax, phimax, xsample, detastr, dphistr);
  //
  // Given the range refine the position employing the smaller window
  if (detastr > 0 && dphistr > 0) {
    CaloLayerCalculator helper;
    const auto* const cellLink = cluster->getCellLinks();
    helper.fill(cellLink->begin(),
                cellLink->end(),
                etamax,
                phimax,
                detastr,
                dphistr,
                sample);

    // Here is where we (re-)fill the eta in the 1st sampling
    if (helper.etam() != -999.) {
      // This is "real" atlas co-ordinates
      cluster->setEta(sample, helper.etam());
    }
  }
}

/** function to refine position in eta1*/
void
refineEta1Position(xAOD::CaloCluster* cluster, const CaloDetDescrManager& mgr)
{
  // This only makes sense if we have cells there
  if (!cluster->hasSampling(CaloSampling::EMB1) &&
      !cluster->hasSampling(CaloSampling::EME1)) {
    return;
  }
  // Now calculare the position using cells in barrel or endcap or both
  const double aeta = std::abs(cluster->etaBE(2));
  if (aeta < 1.6 && cluster->hasSampling(CaloSampling::EMB1)) {
    makeCorrection1(cluster, mgr, CaloSampling::EMB1);
  }
  if (aeta > 1.3 && cluster->hasSampling(CaloSampling::EME1)) {
    makeCorrection1(cluster, mgr, CaloSampling::EME1);
  }
}
/** Find the reference position (eta, phi) relative to which cells are
   restricted.
*/
egammaSuperClusterBuilderBase::CentralPosition
findCentralPositionEM2(const std::vector<const xAOD::CaloCluster*>& clusters)
{
  egammaSuperClusterBuilderBase::CentralPosition cp;
  for (const auto* cluster : clusters) {
    if (cluster->hasSampling(CaloSampling::EMB2)) {
      const float thisEmax = cluster->energy_max(CaloSampling::EMB2);
      if (thisEmax > cp.emaxB) {
        cp.emaxB = thisEmax;
        cp.etaB = cluster->etamax(CaloSampling::EMB2);
        cp.phiB = cluster->phimax(CaloSampling::EMB2);
      }
    }
    if (cluster->hasSampling(CaloSampling::EME2)) {
      const float thisEmax = cluster->energy_max(CaloSampling::EME2);
      if (thisEmax > cp.emaxEC) {
        cp.emaxEC = thisEmax;
        cp.etaEC = cluster->etamax(CaloSampling::EME2);
        cp.phiEC = cluster->phimax(CaloSampling::EME2);
      }
    }
  }
  return cp;
}

} // end of anonymous namespace

//////////////////////////////////////////////////////////////////////////////
// Athena interfaces.
//////////////////////////////////////////////////////////////////////////////

// Constructor.
egammaSuperClusterBuilderBase::egammaSuperClusterBuilderBase(
  const std::string& name,
  ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
{

  m_searchWindowPhiBarrel = m_searchWindowPhiCellsBarrel * s_cellPhiSize * 0.5;
  m_searchWindowEtaBarrel = m_searchWindowEtaCellsBarrel * s_cellEtaSize * 0.5;
  m_searchWindowPhiEndcap = m_searchWindowPhiCellsEndcap * s_cellPhiSize * 0.5;
  m_searchWindowEtaEndcap = m_searchWindowEtaCellsEndcap * s_cellEtaSize * 0.5;

  m_addCellsWindowEtaBarrel =
    m_addCellsWindowEtaCellsBarrel * s_cellEtaSize * 0.5;
  m_addCellsWindowEtaEndcap =
    m_addCellsWindowEtaCellsEndcap * s_cellEtaSize * 0.5;
  // The +- to account for the different L3 eta granularity
  m_extraL3EtaSize = m_extraL3EtaSizeCells * s_cellEtaSize * 0.5;
  // the + is to account for different L0/L1 phi granularity
  m_extraL0L1PhiSize = m_extraL0L1PhiSizeCells * s_cellPhiSize;
}

StatusCode
egammaSuperClusterBuilderBase::initialize()
{
  m_searchWindowPhiBarrel = m_searchWindowPhiCellsBarrel * s_cellPhiSize * 0.5;
  m_searchWindowEtaBarrel = m_searchWindowEtaCellsBarrel * s_cellEtaSize * 0.5;
  m_searchWindowPhiEndcap = m_searchWindowPhiCellsEndcap * s_cellPhiSize * 0.5;
  m_searchWindowEtaEndcap = m_searchWindowEtaCellsEndcap * s_cellEtaSize * 0.5;

  if (m_addCellsWindowEtaCellsBarrel % 2 == 0 ||
      m_addCellsWindowEtaCellsEndcap % 2 == 0) {
    ATH_MSG_FATAL("For adding cells relative to the hottest cell to be "
                  "symmetric in eta, the AddCells "
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
  m_addCellsWindowEtaBarrel =
    m_addCellsWindowEtaCellsBarrel * s_cellEtaSize * 0.5;
  m_addCellsWindowEtaEndcap =
    m_addCellsWindowEtaCellsEndcap * s_cellEtaSize * 0.5;
  // The +- to account for the different L3 eta granularity
  m_extraL3EtaSize = m_extraL3EtaSizeCells * s_cellEtaSize * 0.5;

  // the + is to account for different L0/L1 phi granularity
  m_extraL0L1PhiSize = m_extraL0L1PhiSizeCells * s_cellPhiSize;
  ATH_MSG_INFO(
    '\n'
    << "e/gamma super clusters" << '\n'
    << "--> Eta Window size for L0/L1/L2 cells : " << '\n'
    << "Barrel +- " << m_addCellsWindowEtaBarrel << '\n'
    << "EndCap +- " << m_addCellsWindowEtaEndcap << '\n'
    << "--> Eta Window size for L3 cells : " << '\n'
    << "Barrel +- " << (m_addCellsWindowEtaBarrel + m_extraL3EtaSize) << '\n'
    << "EndCap +- " << (m_addCellsWindowEtaEndcap + m_extraL3EtaSize) << '\n'
    << " -> Phi window is fully dynamic for L2/L3" << '\n'
    << " -> L0/L1 cells in phi will be collected in a window" << '\n'
    << "(L2 neg extend - " << m_extraL0L1PhiSize << " , "
    << "L2 pos extend + " << m_extraL0L1PhiSize << ")");

  return StatusCode::SUCCESS;
}

bool
egammaSuperClusterBuilderBase::matchesInWindow(
  const xAOD::CaloCluster* ref,
  const xAOD::CaloCluster* clus) const
{
  // First the case where the seed is both endcap and barrel, i.e. in the crack
  // Check around both measurements of the seed
  if (ref->hasSampling(CaloSampling::EMB2) &&
      ref->hasSampling(CaloSampling::EME2)) {
    const float dEta(std::abs(ref->eta() - clus->eta()));
    const float dPhi(std::abs(P4Helpers::deltaPhi(ref->phi(), clus->phi())));
    //
    const float dEtaBarrel(
      std::abs(ref->etaSample(CaloSampling::EMB2) - clus->eta()));
    const float dPhiBarrel(std::abs(
      P4Helpers::deltaPhi(ref->phiSample(CaloSampling::EMB2), clus->phi())));
    //
    const float dEtaEndcap(
      std::abs(ref->etaSample(CaloSampling::EME2) - clus->eta()));
    const float dPhiEndcap(std::abs(
      P4Helpers::deltaPhi(ref->phiSample(CaloSampling::EME2), clus->phi())));
    // Matches any in case of split
    return (
      (dEta < m_searchWindowEtaBarrel && dPhi < m_searchWindowPhiBarrel) ||
      (dEta < m_searchWindowEtaEndcap && dPhi < m_searchWindowPhiEndcap) ||
      (dEtaBarrel < m_searchWindowEtaBarrel &&
       dPhiBarrel < m_searchWindowPhiBarrel) ||
      (dEtaEndcap < m_searchWindowEtaEndcap &&
       dPhiEndcap < m_searchWindowPhiEndcap));
  }
  if (xAOD::EgammaHelpers::isBarrel(clus)) {
    const float dEta(std::abs(ref->eta() - clus->eta()));
    const float dPhi(std::abs(P4Helpers::deltaPhi(ref->phi(), clus->phi())));
    return (dEta < m_searchWindowEtaBarrel && dPhi < m_searchWindowPhiBarrel);
  }
  const float dEta(std::abs(ref->eta() - clus->eta()));
  const float dPhi(std::abs(P4Helpers::deltaPhi(ref->phi(), clus->phi())));
  return (dEta < m_searchWindowEtaEndcap && dPhi < m_searchWindowPhiEndcap);
}

bool
egammaSuperClusterBuilderBase::createNewCluster(
  const EventContext& ctx,
  const std::vector<const xAOD::CaloCluster*>& clusters,
  const CaloDetDescrManager& mgr,
  xAOD::EgammaParameters::EgammaType egType,
  xAOD::CaloClusterContainer* newClusters,
  xAOD::CaloClusterContainer* precorrClusters) const
{

  const auto acSize = clusters.size();
  if (clusters.empty()) {
    ATH_MSG_ERROR("Missing the seed cluster! Should not happen.");
    return false;
  }

  // create a new empty cluster
  // collection will own it if not nullptr
  xAOD::CaloCluster* newCluster = CaloClusterStoreHelper::makeCluster(
    newClusters, clusters[0]->getCellLinks()->getCellContainer());

  if (!newCluster) {
    ATH_MSG_ERROR("CaloClusterStoreHelper::makeCluster failed.");
    return false;
  }
  //
  newCluster->setClusterSize(xAOD::CaloCluster::SuperCluster);
  // Let's try to find the eta and phi of the hottest cell in L2.
  // This will be used as the center for restricting the cluster size.
  CentralPosition cpRef = findCentralPositionEM2(clusters);
  // these are the same as the reference but in calo frame
  // (after the processing below)
  CentralPosition cp0 = cpRef;
  // Get the hotest in raw co-ordinates
  if (cp0.emaxB > 0) {
    const CaloDetDescrElement* dde =
      mgr.get_element(CaloCell_ID::EMB2, cpRef.etaB, cpRef.phiB);
    if (dde) {
      cp0.etaB = dde->eta_raw();
      cp0.phiB = dde->phi_raw();
    } else {
      ATH_MSG_WARNING("Couldn't get CaloDetDescrElement from mgr for eta = "
                      << cpRef.etaB << ", phi = " << cpRef.phiB);
    }
  }

  if (cp0.emaxEC > 0) {
    const CaloDetDescrElement* dde =
      mgr.get_element(CaloCell_ID::EME2, cpRef.etaEC, cpRef.phiEC);
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

  ATH_MSG_VERBOSE("Cluster, seed eta = " << newCluster->eta0()
		  << " phi = " << newCluster->phi0()
		  << " size of vec = " << acSize);

  // Actually fill the cluster here
  if (fillClusterConstrained(*newCluster, clusters, cp0).isFailure()) {
    newClusters->pop_back();
    return false;
  }
  // Apply SW-style summation of TileGap3 cells (if necessary).
  float eta0 = std::abs(newCluster->eta0());
  // In Run2, we did not impose restriction to include TG3 cells at this level.
  // It should have been [1.37,1.63]. It has no impact on performance as TG3 was
  // only used in energy calibration BDT in [1.4,1.6].
  // In Run three we restrict to [1.37,1.75]
  if (!m_useExtendedTG3 ||
      (eta0 > s_ClEtaMinForTG3cell && eta0 < s_ClEtaMaxForTG3cell)) {
    if (addTileGap3CellsinWindow(*newCluster, mgr).isFailure()) {
      ATH_MSG_ERROR(
		    "Problem with the input cluster when running AddTileGap3CellsinWindow?");
      newClusters->pop_back();
      return false;
    }
  }
  /// Calculate the kinematics of the new cluster, after all cells are added
  CaloClusterKineHelper::calculateKine(newCluster, true, true);

  ATH_MSG_VERBOSE("energy in TG3 " << newCluster->eSample(CaloSampling::TileGap3)
		  << " uncal E = " << newCluster->e()
		  << " raw " << newCluster->rawE()
		  << " alt " << newCluster->altE());

  // If adding all EM cells we are somehow below the seed threshold then remove
  if (newCluster->et() < m_EtThresholdCut) {
    newClusters->pop_back();
    return false;
  }

  // Check to see if cluster pases basic requirements. If not, kill it.
  if (!m_egammaCheckEnergyDepositTool.empty() &&
      !m_egammaCheckEnergyDepositTool->checkFractioninSamplingCluster(
        newCluster)) {
    newClusters->pop_back();
    return false;
  }

  // Apply correction calibration
  if (calibrateCluster(ctx, newCluster, mgr, egType, precorrClusters)
        .isFailure()) {
    ATH_MSG_WARNING("There was problem calibrating the object");
    newClusters->pop_back();
    return false;
  }

  // Avoid negative energy clusters
  if (newCluster->et() < 0) {
    ATH_MSG_DEBUG("Negative et after calibration/corrections");
    newClusters->pop_back();
    return false;
  }

  ATH_MSG_VERBOSE("Good supercluster, cal E = " << newCluster->e()
		  << " raw " << newCluster->rawE()
		  << " alt " << newCluster->altE());

  if (m_linkToConstituents) {
    // EDM vector to constituent clusters
    std::vector<ElementLink<xAOD::CaloClusterContainer>> constituentLinks;
    static const SG::AuxElement::Accessor<ElementLink<xAOD::CaloClusterContainer>>
      sisterCluster("SisterCluster");
    for (size_t i = 0; i < acSize; i++) {
      // Set the element Link to the constitents
      if (sisterCluster.isAvailable(*clusters[i])) {
        constituentLinks.push_back(sisterCluster(*clusters[i]));
      } else {
        ATH_MSG_WARNING("No sister Link available");
      }
    }
    // Set the link from the super cluster to the constituents (accumulated)
    // clusters used.
    static const SG::AuxElement::Accessor<
      std::vector<ElementLink<xAOD::CaloClusterContainer>>>
      caloClusterLinks("constituentClusterLinks");
    caloClusterLinks(*newCluster) = constituentLinks;
  }
  // return the new cluster
  return true;
}

bool
egammaSuperClusterBuilderBase::seedClusterSelection(
  const xAOD::CaloCluster* clus) const
{
  // The seed should have 2nd sampling
  if (!clus->hasSampling(CaloSampling::EMB2) &&
      !clus->hasSampling(CaloSampling::EME2)) {
    return false;
  }
  const double eta2 = std::abs(clus->etaBE(2));
  if (eta2 > 10) {
    return false;
  }
  // Accordeon Energy samplings 1 to 3
  const double EMAccEnergy =
    clus->energyBE(1) + clus->energyBE(2) + clus->energyBE(3);
  const double EMAccEt = EMAccEnergy / cosh(eta2);
  // Require minimum energy for supercluster seeding.
  return EMAccEt >= m_EtThresholdCut;
}

StatusCode
egammaSuperClusterBuilderBase::fillClusterConstrained(
  xAOD::CaloCluster& tofill,
  const std::vector<const xAOD::CaloCluster*>& clusters,
  const egammaSuperClusterBuilderBase::CentralPosition& cp0) const
{
  const float addCellsWindowEtaBarrel = m_addCellsWindowEtaBarrel;
  const float addCellsWindowEtaEndcap = m_addCellsWindowEtaEndcap;
  const float addCellsWindowL3EtaBarrel =
    m_addCellsWindowEtaBarrel + m_extraL3EtaSize;
  const float addCellsWindowL3EtaEndcap =
    m_addCellsWindowEtaEndcap + m_extraL3EtaSize;

  // Loop for L2/L3
  for (const xAOD::CaloCluster* tocheck : clusters) {
    xAOD::CaloCluster::const_cell_iterator cell_itr = tocheck->begin();
    xAOD::CaloCluster::const_cell_iterator cell_end = tocheck->end();
    // Loop over cells
    for (; cell_itr != cell_end; ++cell_itr) {
      // sanity check on the cell
      const CaloCell* cell = *cell_itr;
      if (!cell) {
        continue;
      }
      const CaloDetDescrElement* dde = cell->caloDDE();
      if (!dde) {
        continue;
      }
      // we want only LAREM
      if (!(dde->getSubCalo() == CaloCell_ID::LAREM)) {
        continue;
      }
      // we want L2 or L3 cells
      const auto sampling = dde->getSampling();
      const bool isL2Cell =
        (CaloCell_ID::EMB2 == sampling || CaloCell_ID::EME2 == sampling);
      const bool isL3Cell =
        (CaloCell_ID::EMB3 == sampling || CaloCell_ID::EME3 == sampling);

      if ((!isL2Cell) && (!isL3Cell)) {
        continue;
      }
      // Also exclude the inner wheel Endcap
      if (dde->is_lar_em_endcap_inner()) {
        continue;
      }

      bool inEtaRange = false;
      // Check if is inside the eta range wrt to the hottest
      // cell(s) for the cluster we construct
      if (cp0.emaxB > 0) { // barrel
        if (isL2Cell &&
            (std::abs(cp0.etaB - dde->eta_raw()) < addCellsWindowEtaBarrel)) {
          inEtaRange = true;
        }
        if (isL3Cell &&
            (std::abs(cp0.etaB - dde->eta_raw()) < addCellsWindowL3EtaBarrel)) {
          inEtaRange = true;
        }
      }
      if (cp0.emaxEC > 0) { // endcap
        if (isL2Cell &&
            (std::abs(cp0.etaEC - dde->eta_raw()) < addCellsWindowEtaEndcap)) {
          inEtaRange = true;
        }
        if (isL3Cell && (std::abs(cp0.etaEC - dde->eta_raw()) <
                         addCellsWindowL3EtaEndcap)) {
          inEtaRange = true;
        }
      }
      if (!inEtaRange) {
        continue;
      }
      tofill.addCell(cell_itr.index(), cell_itr.weight());
    } // Loop over cells for L2/L3
  }   // Loop over clusters for L2/L3

  // We should have a size here
  if (tofill.size() == 0) {
    return StatusCode::FAILURE;
  }
  // Now calculate the cluster size in 2nd layer
  // use that for constraining the L0/L1 cells we add
  const PhiSize phiSize = findPhiSize(cp0, tofill);
  const float phiPlusB = cp0.phiB + phiSize.plusB + m_extraL0L1PhiSize;
  const float phiMinusB = cp0.phiB - phiSize.minusB - m_extraL0L1PhiSize;
  const float phiPlusEC = cp0.phiEC + phiSize.plusEC + m_extraL0L1PhiSize;
  const float phiMinusEC = cp0.phiEC - phiSize.minusEC - m_extraL0L1PhiSize;

  // Loop for L0/L1
  for (const xAOD::CaloCluster* tocheck : clusters) {
    xAOD::CaloCluster::const_cell_iterator cell_itr = tocheck->begin();
    xAOD::CaloCluster::const_cell_iterator cell_end = tocheck->end();
    // Loop over cells
    for (; cell_itr != cell_end; ++cell_itr) {
      // sanity check on the cell
      const CaloCell* cell = *cell_itr;
      if (!cell) {
        continue;
      }
      const CaloDetDescrElement* dde = cell->caloDDE();
      if (!dde) {
        continue;
      }

      // only deal with L1 or PS
      const auto sampling = dde->getSampling();
      const bool isL0L1Cell =
        (CaloCell_ID::EMB1 == sampling || CaloCell_ID::EME1 == sampling ||
         CaloCell_ID::PreSamplerB == sampling ||
         CaloCell_ID::PreSamplerE == sampling);
      if (!isL0L1Cell) {
        continue;
      }

      bool inEtaRange = false;
      // Check if is inside the eta range wrt to the hottest
      // cell(s) for the cluster we construct
      if (cp0.emaxB > 0) { // barrel
        if (std::abs(cp0.etaB - dde->eta_raw()) < addCellsWindowEtaBarrel) {
          inEtaRange = true;
        }
      }
      if (cp0.emaxEC > 0) { // endcap
        if (std::abs(cp0.etaEC - dde->eta_raw()) < addCellsWindowEtaEndcap) {
          inEtaRange = true;
        }
      }
      if (!inEtaRange) {
        continue;
      }

      // Add L0/L1 when we are in the narrow range
      bool inPhiRange = false;
      if (cp0.emaxB > 0) { // barrel
        const double cell_phi = proxim(dde->phi_raw(), cp0.phiB);
        if (cell_phi > phiMinusB && cell_phi < phiPlusB) {
          inPhiRange = true;
        }
      }
      if (cp0.emaxEC > 0) { // endcap
        const double cell_phi = proxim(dde->phi_raw(), cp0.phiEC);
        if (cell_phi > phiMinusEC && cell_phi < phiPlusEC) {
          inPhiRange = true;
        }
      }
      if (!inPhiRange) {
        continue;
      }

      tofill.addCell(cell_itr.index(), cell_itr.weight());
    } // Cell Loop for L0/L1
  }   // Cluster loop for L0/L1
  return StatusCode::SUCCESS;
}

StatusCode
egammaSuperClusterBuilderBase::addTileGap3CellsinWindow(
  xAOD::CaloCluster& tofill,
  const CaloDetDescrManager& mgr) const
{

  double searchWindowEta = m_useExtendedTG3 ? 0.35 : 0.2;
  constexpr double searchWindowPhi = 2 * M_PI / 64.0 + M_PI / 64; // ~ 0.15 rad
  std::vector<const CaloCell*> cells;
  cells.reserve(16);
  const CaloCellContainer* inputcells =
    tofill.getCellLinks()->getCellContainer();

  if (!inputcells) {
    ATH_MSG_ERROR("No cell container in addRemainingCellsToCluster?");
    return StatusCode::FAILURE;
  }

  CaloCellList myList(&mgr, inputcells);

  const std::vector<CaloSampling::CaloSample> samples = {
    CaloSampling::TileGap3
  };

  for (auto samp : samples) {
    // quite slow
    myList.select(
      tofill.eta0(), tofill.phi0(), searchWindowEta, searchWindowPhi, samp);
    cells.insert(cells.end(), myList.begin(), myList.end());
  }

  for (const auto* cell : cells) {
    if (!cell) {
      continue;
    }
    const CaloDetDescrElement* dde = cell->caloDDE();
    if (!dde) {
      continue;
    }

    float maxEta = s_TG3Run2E4cellEtaMax;
    float minEta = s_TG3Run2E4cellEtaMin;
    if (m_useExtendedTG3) {
      minEta = s_TG3Run3E3cellEtaMin;
      // if |eta2| < 1.56, keep only E3, else keep E3+E4.
      // |eta2| uses as the eta of the highest energy cell in layer 2 as proxy
      if (std::abs(tofill.eta0()) > 1.56) {
	maxEta = s_TG3Run3E4cellEtaMax;
      }
    }
    float cellaEtaRaw = std::abs(dde->eta_raw());
    if (cellaEtaRaw >= minEta && cellaEtaRaw <= maxEta) {
      int index = inputcells->findIndex(dde->calo_hash());
      tofill.addCell(index, 1.);
      ATH_MSG_VERBOSE("Adding a TG3 cell " << cell
		      << " sampling (check) " << dde->getSampling()
		      << " raw eta = " << dde->eta_raw()
		      << " raw phi = " << dde->phi_raw()
		      << " energy = " << cell->e()
		      << " to cluster with seed eta = " << tofill.eta0()
		      << " phi = " << tofill.phi0());
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode
egammaSuperClusterBuilderBase::calibrateCluster(
  const EventContext& ctx,
  xAOD::CaloCluster* newCluster,
  const CaloDetDescrManager& mgr,
  const xAOD::EgammaParameters::EgammaType egType,
  xAOD::CaloClusterContainer* precorrClusters) const
{

  refineEta1Position(newCluster, mgr);
  // Save the state before the corrections
  newCluster->setAltE(newCluster->e());
  newCluster->setAltEta(newCluster->eta());
  newCluster->setAltPhi(newCluster->phi());
  // first do the corrections
  if (precorrClusters) {
    precorrClusters->push_back(std::make_unique<xAOD::CaloCluster>());
    *precorrClusters->back() = *newCluster;
  }
  ATH_CHECK(m_clusterCorrectionTool->execute(
    ctx, newCluster, egType, xAOD::EgammaHelpers::isBarrel(newCluster)));
  newCluster->setRawE(newCluster->e());
  newCluster->setRawEta(newCluster->eta());
  newCluster->setRawPhi(newCluster->phi());
  //
  fillPositionsInCalo(newCluster, mgr);
  ATH_CHECK(m_MVACalibSvc->execute(*newCluster, egType));

  return StatusCode::SUCCESS;
}

egammaSuperClusterBuilderBase::PhiSize
egammaSuperClusterBuilderBase::findPhiSize(
  const egammaSuperClusterBuilderBase::CentralPosition& cp0,
  const xAOD::CaloCluster& cluster) const
{

  PhiSize phiSize;
  auto cell_itr = cluster.cell_cbegin();
  auto cell_end = cluster.cell_cend();
  for (; cell_itr != cell_end; ++cell_itr) {

    const CaloCell* cell = *cell_itr;
    if (!cell) {
      continue;
    }

    const CaloDetDescrElement* dde = cell->caloDDE();
    if (!dde) {
      continue;
    }

    if (cp0.emaxB > 0 && CaloCell_ID::EMB2 == dde->getSampling()) {
      const float phi0 = cp0.phiB;
      double cell_phi = proxim(dde->phi_raw(), phi0);
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
    } else if (cp0.emaxEC > 0 && CaloCell_ID::EME2 == dde->getSampling()) {
      const float phi0 = cp0.phiEC;
      double cell_phi = proxim(dde->phi_raw(), phi0);
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
    ATH_MSG_WARNING("phiSizePlusB is large: " << phiSize.plusB
                                              << ", capping at 1.0");
    phiSize.plusB = 1.0;
  }
  if (phiSize.plusEC > 1.0) {
    ATH_MSG_WARNING("phiSizePlusEC is large: " << phiSize.plusEC
                                               << ", capping at 1.0");
    phiSize.plusEC = 1.0;
  }
  if (phiSize.minusB > 1.0) {
    ATH_MSG_WARNING("phiSizeMinusB is large: " << phiSize.minusB
                                               << ", capping at 1.0");
    phiSize.minusB = 1.0;
  }
  if (phiSize.minusEC > 1.0) {
    ATH_MSG_WARNING("phiSizeMinusEC is large: " << phiSize.minusEC
                                                << ", capping at 1.0");
    phiSize.minusEC = 1.0;
  }
  return phiSize;
}
