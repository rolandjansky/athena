/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#include "egammaCaloUtils/egammaBackShape.h"
#include "egammaUtils/egammaEnergyPositionAllSamples.h"
#include "xAODCaloEvent/CaloCluster.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloUtils/CaloLayerCalculator.h"
#include "CaloUtils/CaloCellList.h"
#include <cmath>

StatusCode
egammaBackShape::execute(const xAOD::CaloCluster& cluster,
                         const CaloDetDescrManager& cmgr,
                         const CaloCellContainer& cell_container,
                         Info& info,
                         bool ExecOtherVariables)
{
  //
  // Estimate shower shapes from third compartment
  // based on hottest cell and deta,dphi
  // with eta = cluster->eta(sam)
  //      phi = cluster->phi(sam)
  //

  // check if cluster is in barrel or in the end-cap
  if (!cluster.inBarrel() && !cluster.inEndcap()) {
    return StatusCode::SUCCESS;
  }

  double eallsamples = egammaEnergyPositionAllSamples::e(cluster);
  double e3 = egammaEnergyPositionAllSamples::e3(cluster);
  // check if cluster is in barrel or end-cap
  bool in_barrel = egammaEnergyPositionAllSamples::inBarrel(cluster, 3);
  // define accordingly position of CaloSampling
  // fraction of energy deposited in third sampling
  info.f3 = fabs(eallsamples) > 0. ? e3 / eallsamples : 0.;

  CaloSampling::CaloSample sam = CaloSampling::EMB3;
  CaloSampling::CaloSample sam2 = CaloSampling::EMB2;
  if (in_barrel) {
    sam = CaloSampling::EMB3;
    sam2 = CaloSampling::EMB2;
  } else {
    sam = CaloSampling::EME3;
    sam2 = CaloSampling::EME2;
  }

  double eta = 0;
  double phi = 0;
  double deta = 0;
  double dphi = 0;
  CaloCell_ID::SUBCALO subcalo = CaloCell_ID::LAREM;
  bool barrel = false;
  int sampling_or_module = 0;

  // From the original (eta,phi) position, find the location
  // (sampling, barrel/end-cap, granularity)
  // For this we use the tool egammaEnergyAllSamples
  // which uses the CaloCluster method inBarrel() and inEndcap()
  // but also, in case close to the crack region where both
  // boolean can be true, the energy reconstructed in the sampling
  //

  // Fetch eta and phi of the sampling
  // Note that we use m_sam2 in the 2nd sampling, not in presampler
  eta = cluster.etamax(sam2);
  phi = cluster.phimax(sam2);

  if ((eta == 0. && phi == 0.) || fabs(eta) > 100) {
    return StatusCode::SUCCESS;
  }

  // granularity in (eta,phi) in the pre sampler
  // CaloCellList needs both enums: subCalo and CaloSample
  CaloDetDescrManager::decode_sample(
    subcalo, barrel, sampling_or_module, (CaloCell_ID::CaloSample)sam);
  // Get the corresponding grannularities : needs to know where you are
  //                  the easiest is to look for the CaloDetDescrElement
  const CaloDetDescrElement* dde =
    cmgr.get_element(subcalo, sampling_or_module, barrel, eta, phi);
  // if object does not exist then return
  if (!dde) {
    return StatusCode::SUCCESS;
  }

  // local granularity
  deta = dde->deta();
  dphi = dde->dphi();
  // change values of eta,phi
  eta = dde->eta_raw();
  phi = dde->phi_raw();

  // estimate the relevant quantities around the hottest cell
  // in the following eta X phi windows
  CaloLayerCalculator calc;

  // 3X3
  StatusCode sc =
    calc.fill(cmgr, &cell_container, eta, phi, 3. * deta, 3. * dphi, sam);
  if (sc.isFailure()) {
    return sc;
  }
  info.e333 = calc.em();

  if (ExecOtherVariables) {

    CaloCellList cell_list(&cmgr, &cell_container);
    // 7x7
    cell_list.select(eta, phi, 7.0 * deta, 7.0 * dphi, sam);

    calc.fill(
      cell_list.begin(), cell_list.end(), eta, phi, 7. * deta, 7. * dphi, sam);
    info.e377 = calc.em();

    // 5X5
    calc.fill(
      cell_list.begin(), cell_list.end(), eta, phi, 5. * deta, 5. * dphi, sam);
    info.e355 = calc.em();

    // 3X7
    calc.fill(
      cell_list.begin(), cell_list.end(), eta, phi, 3. * deta, 7. * dphi, sam);
    info.e337 = calc.em();

    // 3X5
    calc.fill(
      cell_list.begin(), cell_list.end(), eta, phi, 3. * deta, 5. * dphi, sam);
    info.e335 = calc.em();
  }

  // f3core
  if (eallsamples > 0. && info.e333 > -999.) {
    info.f3core = info.e333 / eallsamples;
  }

  return StatusCode::SUCCESS;
}

