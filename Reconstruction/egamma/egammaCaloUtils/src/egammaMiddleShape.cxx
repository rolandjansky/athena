/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#include "egammaCaloUtils/egammaMiddleShape.h"
#include "egammaUtils/egammaEnergyPositionAllSamples.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloEvent/CaloCluster.h"
#include "CaloUtils/CaloLayerCalculator.h"
#include "xAODCaloEvent/CaloCluster.h"

#include "egammaUtils/egammaqweta2c.h"
#include <cmath>

StatusCode
egammaMiddleShape::execute(const xAOD::CaloCluster& cluster,
                           const CaloDetDescrManager& cmgr,
                           const CaloCellContainer& cell_container,
                           Info& info,
                           bool doRetaOnly) 
{
  //
  // Estimate shower shapes from 2nd compartment
  // based on hottest cell and deta,dphi
  // with eta = cluster->eta(sam)
  //      phi = cluster->phi(sam)
  // and search for hottest cell in a 7X7 window
  // in the second sampling
  //
  //
  StatusCode sc = StatusCode::SUCCESS;
  // check if cluster is in barrel or in the end-cap
  if (!cluster.inBarrel() && !cluster.inEndcap()) {
    return sc;
  }

  // check if cluster is in barrel or end-cap
  bool in_barrel = egammaEnergyPositionAllSamples::inBarrel(cluster, 2);
  CaloSampling::CaloSample sam = CaloSampling::EMB2;
  if (in_barrel) {
    sam = CaloSampling::EMB2;
  } else {
    sam = CaloSampling::EME2;
  }

  // granularity in (eta,phi) in the pre sampler
  // CaloCellList needs both enums: subCalo and CaloSample
  double eta = 0;
  double phi = 0;
  double deta = 0;
  double dphi = 0;
  float etacell = 0;
  bool barrel = false;
  CaloCell_ID::SUBCALO subcalo = CaloCell_ID::LAREM;
  int sampling_or_module = 0;

  etacell = cluster.etamax(sam);
  eta = cluster.etaSample(sam);
  phi = cluster.phiSample(sam);
  if ((eta == 0. && phi == 0.) || fabs(eta) > 100) {
    return sc;
  }

  CaloDetDescrManager::decode_sample(
    subcalo, barrel, sampling_or_module, (CaloCell_ID::CaloSample)sam);

  // Get the corresponding grannularities : needs to know where you are
  //                  the easiest is to look for the CaloDetDescrElement
  const CaloDetDescrElement* dde =
    cmgr.get_element(subcalo, sampling_or_module, barrel, eta, phi);
  // if object does not exist then return
  if (!dde) {
    return sc;
  }
  // local granularity
  deta = dde->deta();
  dphi = dde->dphi();

  // search the hottest cell around the (eta,phi)
  // (eta,phi) are defined as etaSample() and phiSample
  // around this position a hot cell is searched for in a window
  // (7*m_deta,7*m_dphi)
  CaloLayerCalculator calc;

  sc = calc.fill(cmgr,
                 &cell_container,
                 cluster.etaSample(sam),
                 cluster.phiSample(sam),
                 7.0 * deta,
                 7.0 * dphi,
                 (CaloSampling::CaloSample)sam);
  double etamax = calc.etarmax();
  double phimax = calc.phirmax();

  // 3X7
  sc = calc.fill(cmgr,
                 &cell_container,
                 etamax,
                 phimax,
                 3. * deta,
                 7. * dphi,
                 (CaloSampling::CaloSample)sam);
  info.e237 = calc.em();
  // 7x7
  sc = calc.fill(cmgr,
                 &cell_container,
                 etamax,
                 phimax,
                 7. * deta,
                 7. * dphi,
                 (CaloSampling::CaloSample)sam);
  info.e277 = calc.em();

  if (!doRetaOnly) {
    // estimate the relevant quantities around the hottest cell
    // in the following eta X phi windows
    // 3X3
    sc = calc.fill(cmgr,
                   &cell_container,
                   etamax,
                   phimax,
                   3. * deta,
                   3. * dphi,
                   (CaloSampling::CaloSample)sam);
    info.e233 = calc.em();
    // 3X5
    sc = calc.fill(cmgr,
                 &cell_container,
                 etamax,
                 phimax,
                 3. * deta,
                 5. * dphi,
                 (CaloSampling::CaloSample)sam);

    info.e235 = calc.em();
    double etaw = calc.etas();
    info.phiw = calc.phis();
    info.etaw = egammaqweta2c::Correct(eta, etacell, etaw);
    info.width = etaw;
    info.poscs2 = egammaqweta2c::RelPosition(eta, etacell);
    // 5X5
    sc = calc.fill(cmgr,
                   &cell_container,
                   etamax,
                   phimax,
                   5. * deta,
                   5. * dphi,
                   (CaloSampling::CaloSample)sam);
    info.e255 = calc.em();
  }
  return sc;
}

