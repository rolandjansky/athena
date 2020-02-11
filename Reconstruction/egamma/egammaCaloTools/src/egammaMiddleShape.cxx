/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "egammaMiddleShape.h"
#include "egammaUtils/egammaEnergyPositionAllSamples.h"

#include "CaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "CaloUtils/CaloLayerCalculator.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

#include "egammaUtils/egammaqweta2c.h"
#include <cmath>

// Constructor
egammaMiddleShape::egammaMiddleShape(const std::string &type, const std::string &name, const IInterface *parent)
    : AthAlgTool(type, name, parent) {
  // declare Interface
  declareInterface<IegammaMiddleShape>(this);
}

egammaMiddleShape::~egammaMiddleShape(){ 
}

StatusCode egammaMiddleShape::initialize()
{
    ATH_MSG_DEBUG(" Initializing egammaMiddleShape");
    return StatusCode::SUCCESS;
}


StatusCode egammaMiddleShape::finalize(){
    return StatusCode::SUCCESS;
}

StatusCode egammaMiddleShape::execute(const xAOD::CaloCluster& cluster,
                                      const CaloDetDescrManager& cmgr,
                                      const CaloCellContainer& cell_container,
                                      Info& info) const {
  //
  // Estimate shower shapes from 2nd compartment
  // based on hottest cell and deta,dphi
  // with eta = cluster->eta(sam)
  //      phi = cluster->phi(sam)
  // and search for hottest cell in a 7X7 window
  // in the second sampling
  //

  ATH_MSG_DEBUG(" egammaMiddleShape: execute");

  // check if cluster is in barrel or in the end-cap
  if (!cluster.inBarrel() && !cluster.inEndcap()) {
    ATH_MSG_DEBUG(" egammaMiddleShape: Cluster is neither in Barrel nor in "
                  "Endcap, cannot calculate ShowerShape ");
    return StatusCode::SUCCESS;
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
    return StatusCode::SUCCESS;
  }

  cmgr.decode_sample(subcalo, barrel, sampling_or_module,
                     (CaloCell_ID::CaloSample)sam);

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

  // search the hottest cell around the (eta,phi)
  // (eta,phi) are defined as etaSample() and phiSample
  // around this position a hot cell is searched for in a window
  // (m_neta*m_deta,m_nphi*m_dphi), by default (m_neta,m_nphi)=(7,7)
  CaloLayerCalculator calc;
  StatusCode sc =
      calc.fill(&cell_container, cluster.etaSample(sam), cluster.phiSample(sam),
                m_neta * deta, m_nphi * dphi, (CaloSampling::CaloSample)sam);
  if (sc.isFailure()) {
    ATH_MSG_WARNING("CaloLayerCalculator failed  fill ");
  }
  double etamax = calc.etarmax();
  double phimax = calc.phirmax();

  // estimate the relevant quantities around the hottest cell
  // in the following eta X phi windows
  // 3X3
  sc = calc.fill(&cell_container, etamax, phimax, 3. * deta, 3. * dphi,
                 (CaloSampling::CaloSample)sam);
  if (sc.isFailure()) {
    ATH_MSG_WARNING("CaloLayerCalculator failed 3x3 fill ");
  }
  info.e233 = calc.em();
  // 3X5
  sc = calc.fill(&cell_container, etamax, phimax, 3. * deta, 5. * dphi,
                 (CaloSampling::CaloSample)sam);
  if (sc.isFailure()) {
    ATH_MSG_WARNING("CaloLayerCalculator failed 3x5 fill ");
  }
  info.e235 = calc.em();
  double etaw = calc.etas();
  info.phiw = calc.phis();
  info.etaw = egammaqweta2c::Correct(eta, etacell, etaw);
  info.width = etaw;
  info.poscs2 = egammaqweta2c::RelPosition(eta, etacell);
  // 5X5
  if (m_ExecOtherVariables) {
    sc = calc.fill(&cell_container, etamax, phimax, 5. * deta, 5. * dphi,
                   (CaloSampling::CaloSample)sam);
    if (sc.isFailure()) {
      ATH_MSG_WARNING("CaloLayerCalculator failed 5x5 fill ");
    }
    info.e255 = calc.em();
  }
  // 3X7
  sc = calc.fill(&cell_container, etamax, phimax, 3. * deta, 7. * dphi,
                 (CaloSampling::CaloSample)sam);
  if (sc.isFailure()) {
    ATH_MSG_WARNING("CaloLayerCalculator failed 3x7 fill ");
  }
  info.e237 = calc.em();
  // 7x7
  sc = calc.fill(&cell_container, etamax, phimax, 7. * deta, 7. * dphi,
                 (CaloSampling::CaloSample)sam);
  if (sc.isFailure()) {
    ATH_MSG_WARNING("CaloLayerCalculator failed 7x7 fill ");
  }
  info.e277 = calc.em();

  return StatusCode::SUCCESS;
}

